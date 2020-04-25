#include <stdlib.h>
#include <stdio.h>
#define __USE_POSIX199309
#include <time.h>
#include <inttypes.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#include "screen_x11.h"
#include "screen_info_rgb.h"

#define BITS_PER_BYTE 8

#define NANO_IN_MILLI 1000000
#define NANO_IN_SEC (1000 * NANO_IN_MILLI)

// number of FPS values stored to get an average
#define NUM_AVG_FPS 20
// no. of frames that can be skipped in any one animation loop i.e the state is updated but not rendered
#define MAX_FRAME_SKIPS 5
//// Number of frames with a delay of 0 ms before the animation thread yields to other running threads.
//#define NUM_DELAYS_PER_YIELD 16
// period between stats updates // 1 sec
#define UPDATE_STATS_INTERVAL NANO_IN_SEC

typedef struct {
    int64_t frame_counter;
    int64_t frames_skipped;
    int64_t total_frames_skipped;
    int64_t prev_time;
    int64_t interval; // interval in ns between stats update
    int64_t total_elapsed_time;
//    int64_t total_time_spent; // this is in seconds
    int64_t count; // value index
    double fps_values[NUM_AVG_FPS];
    double ups_values[NUM_AVG_FPS];
    double average_fps;
    double average_ups;
} stats_t;

struct screen_x11 {
    Display *dpy;                     // X11: Display (connection to X server)
    Visual *vis;                      // X11: Visual (visual info about X server)
    Window w;                         // X11: Window
    int depth, bytespp, scanline_pad; // X11: info about X server
    int xsize, ysize;                 // window and screen_buffer size
    int bufsize;                      // screen_buffer size
    unsigned char *buffer;            // offscreen screen_buffer data for ximage
    GC gc;                            // X11: graphics context
    XShmSegmentInfo shminfo;          // X11 shm
    XImage *ximg;                   // X11: XImage which will be actually shown
    screen_info_rgb_t *screen_info_rgb;
    int64_t period; // period between drawing // all times are in _nanosecs_
    int64_t start_time;
    stats_t stats;
    update_fptr_t update_fptr; // refactor name?
    render_fptr_t render_fptr;
    key_event_fptr_t key_event_fptr;
    Bool is_running;
};

static void init_stats(screen_x11_t *screen_x11);
static XImage * create_image(screen_x11_t *screen_x11);
static void blit(screen_x11_t *screen_x11);
static inline int64_t nano_time(void);
static void update(screen_x11_t *screen_x11, int64_t elapsed_time);
static void render(screen_x11_t *screen_x11);
static void updateStats(screen_x11_t *screen_x11);

void print_final_stats(screen_x11_t *screen_x11);

screen_x11_t *init_screen_x11(screen_settings_t *screen_settings) {
    screen_x11_t *screen_x11 = malloc(sizeof(screen_x11_t));
    screen_x11->xsize = screen_settings->xsize;
    screen_x11->ysize = screen_settings->ysize;
    if (!(screen_x11->dpy = XOpenDisplay(NULL))) {
        printf("Couldn't open a display connection\n");
        exit(EXIT_FAILURE);
    }

    // find out the supported depths of this server and take the default one
    int defaultDepth = DefaultDepth(screen_x11->dpy, 0);
    int count;
    XPixmapFormatValues *pixmap_formats = XListPixmapFormats(screen_x11->dpy, &count);
    for(int i=0; i<count; i++) {
        if(pixmap_formats[i].depth == defaultDepth) {
            screen_x11->depth        = pixmap_formats[i].depth;
            screen_x11->bytespp      = pixmap_formats[i].bits_per_pixel / BITS_PER_BYTE;
            screen_x11->scanline_pad = pixmap_formats[i].scanline_pad;
        }
    }
    XFree(pixmap_formats);
    printf("default depth of display %d\n", screen_x11->depth);
    printf("bytes per pixel: %d\n", screen_x11->bytespp);

    // create and map (display) an X window for output
    int screen_num = DefaultScreen(screen_x11->dpy);
    screen_x11->vis = DefaultVisual(screen_x11->dpy, 0);
    screen_x11->w = XCreateWindow(screen_x11->dpy,                // display
                          DefaultRootWindow(screen_x11->dpy),     // parent
                          100, 100,                   // x, y position
                          (unsigned int)screen_settings->xsize, // width
                          (unsigned int)screen_settings->ysize, // height
                          0,                          // border width
                          screen_x11->depth,          // defaultDepth (we use max. possible)
                          CopyFromParent,             // visual class (TrueColor etc)
                          screen_x11->vis,            // visual
                          0, NULL);                   // valuemask, window attributes

    XStoreName(screen_x11->dpy, screen_x11->w, screen_settings->window_title);
    XMapWindow(screen_x11->dpy, screen_x11->w);

    switch(screen_x11->vis->class) {
        // constants for visual classes are defined in /usr/include/X11/X.h
        case TrueColor:
            {
                XVisualInfo vi;
                int result = XMatchVisualInfo(screen_x11->dpy, screen_num, screen_x11->depth, TrueColor, &vi);
                if (result) {
                    printf("visual is TrueColor, %d bytes per pix, %d bytes per rgb\n",
                           screen_x11->bytespp,
                           vi.depth / BITS_PER_BYTE);
    //                sinfo = new l3d_screen_info_rgb(vis->red_mask, vis->green_mask, vis->blue_mask, bytespp, vi.defaultDepth / BITS_PER_BYTE);
                } else {
                    printf("Couldn't get visual information, XMatchVisualInfo\n");
                    exit(-1);
                }
            }
            break;
        case PseudoColor: printf("unsupported visual PseudoColor\n"); exit(EXIT_FAILURE);
        case StaticColor: printf("unsupported visual StaticColor\n"); exit(EXIT_FAILURE);
        case GrayScale:   printf("unsupported visual GrayScale\n"); exit(EXIT_FAILURE);
        case StaticGray:  printf("unsupported visual StaticGray\n"); exit(EXIT_FAILURE);
        case DirectColor: printf("unsupported visual DirectColor\n"); exit(EXIT_FAILURE);
    }

    XSelectInput(screen_x11->dpy, screen_x11->w, KeyPressMask);
    // gc is not needed now, but later when blitting with XPutImage
    screen_x11->gc = DefaultGC(screen_x11->dpy, screen_num);
    screen_x11->bufsize = screen_x11->xsize * screen_x11->ysize * screen_x11->bytespp; // used for clearing screen_num buf
    screen_x11->ximg = create_image(screen_x11);
//    screen_x11->buffer = malloc(xsize*ysize*screen_x11->bytespp); // not necessary with the shm
    screen_x11->buffer = (unsigned char *)(screen_x11->ximg->data);

    screen_x11->screen_info_rgb = init_screen_info_rgb(
            screen_x11->vis->red_mask, screen_x11->vis->green_mask, screen_x11->vis->blue_mask,
            screen_x11->bytespp, screen_x11->depth / BITS_PER_BYTE,
            screen_x11->buffer);

    screen_x11->period = NANO_IN_SEC / screen_settings->targetFps;
    screen_x11->start_time = 0;
    screen_x11->is_running = False;
    // TODO app pointer init ??
    init_stats(screen_x11);
    // init the other fields too?

    return screen_x11;
}

void init_stats(screen_x11_t *screen_x11) {
    // https://stackoverflow.com/questions/6891720/initialize-reset-struct-to-zero-null
    static const stats_t empty_stats = { 0 };
    screen_x11->stats = empty_stats;
}

void run_event_loop(screen_x11_t *screen_x11,
        update_fptr_t update_fptr,
        render_fptr_t render_fptr,
        key_event_fptr_t key_event_fptr) {

    int64_t over_sleep_time = 0;
    int64_t excess = 0;
    int64_t period = screen_x11->period; // save it here

    screen_x11->update_fptr = update_fptr;
    screen_x11->render_fptr = render_fptr;
    screen_x11->key_event_fptr = key_event_fptr;

    int64_t before_time = nano_time();
    screen_x11->start_time = before_time;
    screen_x11->stats.prev_time = before_time;
    screen_x11->is_running = True;

    while(screen_x11->is_running) {

        update(screen_x11, 0);
        render(screen_x11); // TODO

        int64_t after_time = nano_time();
        int64_t time_diff = after_time - before_time;
        int64_t sleep_time = (period - time_diff) - over_sleep_time;

        if (sleep_time > 0) {
            struct timespec sleep_time_s = {0, sleep_time };
            nanosleep(&sleep_time_s, NULL);
            over_sleep_time = (nano_time() - after_time) - sleep_time;
        } else { // sleepTime <= 0; the render_fptr took longer than the period
            // print rendering is slowing down?
            excess -= sleep_time; // store excess prev_time value
            over_sleep_time = 0;
        }

        before_time = nano_time();

        /* If render_fptr animation is taking too long, update_fptr the state
           without rendering it, to get the updates/sec nearer to
           the required FPS. */
        int skips = 0;
        while ((excess > period) && (skips < MAX_FRAME_SKIPS)) {
            // update_fptr state but don’t render_fptr
            excess -= period;
            update(screen_x11, 0);
            ++skips;
        }
        screen_x11->stats.frames_skipped += skips;

        updateStats(screen_x11);
    }

    print_final_stats(screen_x11);

    // move to dtor?
    XShmDetach(screen_x11->dpy, &screen_x11->shminfo);
    XDestroyImage(screen_x11->ximg);
    shmdt(screen_x11->shminfo.shmaddr);

    XUnmapWindow(screen_x11->dpy, screen_x11->w);
    XDestroyWindow(screen_x11->dpy, screen_x11->w);
    //delete [] screen_buffer; // automatically done by XDestroyImage ! See man page...
    XCloseDisplay(screen_x11->dpy);
    exit(EXIT_SUCCESS);
}

void render(screen_x11_t *screen_x11) {
    screen_x11->render_fptr();
    blit(screen_x11);
    XSync(screen_x11->dpy, False);
}

void update(screen_x11_t *screen_x11, int64_t elapsed_time) {

    XEvent event;
    char ch;
    KeySym keysym;
    XComposeStatus xcompstat;

    if(XCheckWindowEvent(screen_x11->dpy, screen_x11->w, KeyPressMask, &event)) {
        XLookupString(&event.xkey, &ch, 1, &keysym, &xcompstat);
        switch(ch) {
            case 'q': {
                screen_x11->is_running = False;
            }
        }
        screen_x11->key_event_fptr(ch);
    }

    screen_x11->update_fptr();
}

void updateStats(screen_x11_t *screen_x11) {
    stats_t *stats = &screen_x11->stats; // save this
    ++stats->frame_counter;
    stats->interval += screen_x11->period;
    if (stats->interval >= UPDATE_STATS_INTERVAL) {
        int64_t time_now = nano_time();
        int64_t real_elapsed_time = time_now - stats->prev_time;
//        double timing_error = ((double) (real_elapsed_time - stats->interval) / stats->interval) * 100.0; // TODO
        stats->total_elapsed_time += real_elapsed_time;
        stats->total_frames_skipped += stats->frames_skipped;
        // calculate the latest FPS and UPS
        double actual_fps = 0.0;
        double actual_ups = 0.0;
        if (stats->total_elapsed_time > 0) {
            actual_fps = (((double) stats->frame_counter / stats->total_elapsed_time) * NANO_IN_SEC);
            actual_ups = (((double) (stats->frame_counter + stats->total_frames_skipped) / stats->total_elapsed_time) * NANO_IN_SEC);
        }
        stats->fps_values[stats->count % NUM_AVG_FPS] = actual_fps;
        stats->ups_values[stats->count % NUM_AVG_FPS] = actual_ups;
        ++stats->count;
        // obtain the average fps and ups
        double total_fps = 0.0;
        double total_ups = 0.0;
        for (int i = 0; i != NUM_AVG_FPS; i++) {
            total_fps += stats->fps_values[i];
            total_ups += stats->ups_values[i];
        }
        int64_t num_values = (stats->count >= NUM_AVG_FPS) ? NUM_AVG_FPS : stats->count;
        stats->average_fps = total_fps / num_values;
        stats->average_ups = total_ups / num_values;
        // print stats? // use timing error and other info??
        // ...
        // reset some values
        stats->frames_skipped = 0;
        stats->prev_time = time_now;
        stats->interval = 0;
    }
}

void print_final_stats(screen_x11_t *screen_x11) {
    stats_t *stats = &screen_x11->stats;
    printf("\n***Final stats:***\n"
            "Frame Count/Loss: %" PRId64 "/" "%"PRId64 "\n"
           "Average FPS: %.2f\n"
           "Average UPS: %.2f\n"
           "Time spent: %" PRId64 "s",
           stats->frame_counter, stats->total_frames_skipped,
           stats->average_fps, stats->average_ups,
           stats->total_elapsed_time / NANO_IN_SEC
        );
//    https://stackoverflow.com/questions/12450066/flushing-buffers-in-c
//    fflush(stdout);
}

// https://stackoverflow.com/questions/31108159/what-is-the-use-of-the-inline-keyword-in-c
static inline void blit(screen_x11_t *screen_x11) {
    XShmPutImage(screen_x11->dpy, screen_x11->w, screen_x11->gc, screen_x11->ximg,
                 0, 0, 0, 0,
                 (unsigned int)screen_x11->xsize, (unsigned int)screen_x11->ysize,
                 False);
}

static XImage * create_image(screen_x11_t *screen_x11) {
    XImage *ximg = XShmCreateImage(screen_x11->dpy, screen_x11->vis, (unsigned int) screen_x11->depth,
                                   ZPixmap,   // format
                                   0,  // pixels to ignore at beg. of scanline
                                   &screen_x11->shminfo,
                                   (unsigned int)screen_x11->xsize, (unsigned int)screen_x11->ysize);
    screen_x11->shminfo.shmid = shmget(IPC_PRIVATE, (size_t)(ximg->bytes_per_line*ximg->height), IPC_CREAT | 0777 );
    if(screen_x11->shminfo.shmid < 0) {
        printf("couldn't allocate shared memory\n");
        XDestroyImage(ximg);
        ximg = NULL;
        exit(EXIT_FAILURE);
    } else {
        // attach, and check for errors
        screen_x11->shminfo.shmaddr = ximg->data = (char *)shmat(screen_x11->shminfo.shmid, 0, 0);
        if(screen_x11->shminfo.shmaddr == (char *) -1) {
            printf("couldn't allocate shared memory\n");
            XDestroyImage(ximg);
            ximg = NULL;
            exit(EXIT_FAILURE);
        } else {
            // set as read/write, and attach to the display
            screen_x11->shminfo.readOnly = False;
            XShmAttach(screen_x11->dpy, &screen_x11->shminfo);
        }
    }
    return ximg;
}

int64_t nano_time() {
    struct timespec time;
//    https://stackoverflow.com/questions/3523442/difference-between-clock-realtime-and-clock-monotonic
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    return time.tv_sec * NANO_IN_SEC + time.tv_nsec;
}
