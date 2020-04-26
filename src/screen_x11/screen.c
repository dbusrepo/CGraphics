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
#include <tiff.h>

#include "../graphics_utils/common.h"
#include "../graphics_utils/screen_info_rgb_p.h"
#include "../app/screen.h"

// font used with text drawing
#define FONT_NAME "-*-fixed-*-*-*-*-18-*-*-*-*-*-iso8859-*" //"-*-fixed-*-r-*-*-22-*-*-*-*-*-*-*" //"*-helvetica-*-12-*"

typedef struct screen screen_x11_t;

struct screen {
    Display *display;                     // X11: Display (connection to X server)
    Visual *vis;                      // X11: Visual (visual info about X server)
    Window window;                         // X11: Window
    uint32_t depth, bytespp, scanline_pad; // X11: info about X server
    uint32_t xsize, ysize;                 // window and screen_buffer size
    uint32_t buf_size;                      // screen_buffer size
    uint8_t *buffer;                    // offscreen screen_buffer data for ximage
    GC gc;                            // X11: graphics context
//    XFontStruct* font_info;
    XShmSegmentInfo shminfo;          // X11 shm
    XImage *ximg;                   // X11: XImage which will be actually shown
    screen_info_rgb_t *screen_info_rgb;
};

static XImage *create_image(screen_x11_t *screen_x11);
static void set_size_hints(const screen_settings_t *screen_settings, const screen_x11_t *screen_x11);

screen_x11_t *init_screen(screen_settings_t *screen_settings) {
    screen_x11_t *screen_x11 = malloc(sizeof(screen_x11_t));
    screen_x11->xsize = screen_settings->xsize;
    screen_x11->ysize = screen_settings->ysize;
    if (!(screen_x11->display = XOpenDisplay(NULL))) {
        fprintf(stderr, "couldn't open a display connection\n");
        exit(EXIT_FAILURE);
    }
    // find out the supported depths of this server and take the default one
    int default_depth = DefaultDepth(screen_x11->display, 0);
    int count;
    XPixmapFormatValues *pixmap_formats = XListPixmapFormats(screen_x11->display, &count);
    for (int i = 0; i != count; i++) {
        if (pixmap_formats[i].depth == default_depth) {
            screen_x11->depth = (uint32_t) pixmap_formats[i].depth;
            screen_x11->bytespp = (uint32_t) (pixmap_formats[i].bits_per_pixel / BITS_PER_BYTE);
            screen_x11->scanline_pad = (uint32_t) pixmap_formats[i].scanline_pad;
        }
    }
    XFree(pixmap_formats);
    printf("default depth of display %d\n", screen_x11->depth);
    printf("bytes per pixel: %d\n", screen_x11->bytespp);

    // create and map (display) an X window for output
    int screen_num = DefaultScreen(screen_x11->display);
    screen_x11->vis = DefaultVisual(screen_x11->display, 0);
    screen_x11->window = XCreateWindow(screen_x11->display,                // display
                                       DefaultRootWindow(screen_x11->display),     // parent
                                       100, 100,                   // x, y position
                                       (uint32_t) screen_settings->xsize, // width
                                       (uint32_t) screen_settings->ysize, // height
                                       0,                          // border width
                                       (int32_t) screen_x11->depth,          // default_depth (we use max. possible)
                                       CopyFromParent,             // visual class (TrueColor etc)
                                       screen_x11->vis,            // visual
                                       0, NULL);                   // valuemask, window attributes
    set_size_hints(screen_settings, screen_x11);

    XStoreName(screen_x11->display, screen_x11->window, screen_settings->window_title);
    XMapWindow(screen_x11->display, screen_x11->window);

    switch (screen_x11->vis->class) {
        // constants for visual classes are defined in /usr/include/X11/X.h
        case TrueColor: {
            XVisualInfo vi;
            int result = XMatchVisualInfo(screen_x11->display, screen_num, (int32_t) screen_x11->depth, TrueColor, &vi);
            if (result) {
                printf("visual is TrueColor, %d bytes per pix, %d bytes per rgb\n",
                       screen_x11->bytespp,
                       vi.depth / BITS_PER_BYTE);
                //                sinfo = new l3d_screen_info_rgb(vis->red_mask, vis->green_mask, vis->blue_mask, bytespp, vi.default_depth / BITS_PER_BYTE);
            } else {
                fprintf(stderr, "couldn't get visual information, XMatchVisualInfo\n");
                exit(EXIT_FAILURE);
            }
        }
            break;
        case PseudoColor:
            fprintf(stderr, "unsupported visual PseudoColor\n");
            exit(EXIT_FAILURE);
        case StaticColor:
            fprintf(stderr, "unsupported visual StaticColor\n");
            exit(EXIT_FAILURE);
        case GrayScale:
            fprintf(stderr, "unsupported visual GrayScale\n");
            exit(EXIT_FAILURE);
        case StaticGray:
            fprintf(stderr, "unsupported visual StaticGray\n");
            exit(EXIT_FAILURE);
        case DirectColor:
            fprintf(stderr, "unsupported visual DirectColor\n");
            exit(EXIT_FAILURE);
    }

    XSelectInput(screen_x11->display, screen_x11->window, KeyPressMask | ResizeRedirectMask);

    screen_x11->gc = XCreateGC(screen_x11->display, screen_x11->window, 0, NULL); // DefaultGC(screen_x11->dpy, screen_num);
    /* change the foreground color of this GC to white. */
    XSetForeground(screen_x11->display, screen_x11->gc, WhitePixel(screen_x11->display, screen_num));

    // init font for drawing stats. REMOVE?
//    screen_x11->font_info = XLoadQueryFont(screen_x11->display, FONT_NAME);
//    if (!screen_x11->font_info) {
//        fprintf(stderr, "XLoadQueryFont: failed loading font '%s'\n", FONT_NAME);
//        exit(EXIT_FAILURE);
//    }
//    XSetFont(screen_x11->display, screen_x11->gc, screen_x11->font_info->fid);

    screen_x11->ximg = create_image(screen_x11);
    XFlush(screen_x11->display);

    screen_x11->buf_size = screen_x11->xsize * screen_x11->ysize * screen_x11->bytespp; // used for clearing screen_num buf
//    screen_x11->buffer = malloc(xsize*ysize*screen_x11->bytespp); // not necessary with the shm
    screen_x11->buffer = (uint8_t *) (screen_x11->ximg->data);

    screen_x11->screen_info_rgb = init_screen_info_rgb(
            screen_x11->vis->red_mask, screen_x11->vis->green_mask, screen_x11->vis->blue_mask,
            screen_x11->bytespp, screen_x11->depth / BITS_PER_BYTE,
            screen_x11->buffer);

    return screen_x11;
}

void set_size_hints(const screen_settings_t *screen_settings,
                    const screen_x11_t *screen_x11) {// see http://ftp.dim13.org/pub/doc/Xlib.pdf
    XSizeHints *win_size_hints;
    win_size_hints = XAllocSizeHints();
    if (!win_size_hints) {
        fprintf(stderr, "XAllocSizeHints - couldn't allocate\n");
//        exit(EXIT_FAILURE);
    }
//    search for XSizeHints here https://www.x.org/releases/current/doc/libX11/libX11/libX11.pdf
//    https://www.linuxquestions.org/questions/programming-9/%5Bc%5D-x11-window-of-fixed-aspect-ratio-843617/
    win_size_hints->flags= PMinSize | PMaxSize;
//    win_size_hints->width = screen_settings->xsize;
//    win_size_hints->height = screen_settings->ysize;
    win_size_hints->min_width = win_size_hints->max_width = screen_settings->xsize;
    win_size_hints->min_height = win_size_hints->max_height = screen_settings->ysize;
    XSetWMNormalHints(screen_x11->display, screen_x11->window, win_size_hints);
    XFree(win_size_hints);
}

screen_info_rgb_t *get_screen_info(screen_x11_t *screen) {
    return screen->screen_info_rgb;
}

void close_screen(screen_x11_t *screen) {
    XShmDetach(screen->display, &screen->shminfo);
    XDestroyImage(screen->ximg);
    shmdt(screen->shminfo.shmaddr);

    XUnmapWindow(screen->display, screen->window);
    XFreeGC(screen->display, screen->gc); // error if using the default one?
    XDestroyWindow(screen->display, screen->window);
    //delete [] screen_buffer; // automatically done by XDestroyImage ! See man page...
    XCloseDisplay(screen->display);
}

void check_event(screen_x11_t *screen, app_t *app, app_key_event_fptr_t app_key_event_fptr) {
    XEvent event;
    char ch;
    KeySym keysym;
    XComposeStatus xcompstat;

    if(XCheckWindowEvent(screen->display, screen->window, KeyPressMask, &event)) {
        XLookupString(&event.xkey, &ch, 1, &keysym, &xcompstat);
        app_key_event_fptr(app, ch);
    }
}

void blit(screen_x11_t *screen) {
    XShmPutImage(screen->display, screen->window, screen->gc, screen->ximg,
                 0, 0, 0, 0,
                 (unsigned int)screen->xsize, (unsigned int)screen->ysize,
                 False);
    XSync(screen->display, False);
}

/**********************************************************************/
// PRIVATE FUNCTIONS

static XImage *create_image(screen_x11_t *screen_x11) {
    XImage *ximg = XShmCreateImage(screen_x11->display, screen_x11->vis, (unsigned int) screen_x11->depth,
                                   ZPixmap,   // format
                                   0,  // pixels to ignore at beg. of scanline
                                   &screen_x11->shminfo,
                                   (unsigned int)screen_x11->xsize, (unsigned int)screen_x11->ysize);
    screen_x11->shminfo.shmid = shmget(IPC_PRIVATE, (size_t)(ximg->bytes_per_line*ximg->height), IPC_CREAT | 0777 );
    if(screen_x11->shminfo.shmid < 0) {
        fprintf(stderr, "couldn't allocate shared memory\n");
        XDestroyImage(ximg);
        ximg = NULL;
        exit(EXIT_FAILURE);
    } else {
        // attach, and check for errors
        screen_x11->shminfo.shmaddr = ximg->data = (char *)shmat(screen_x11->shminfo.shmid, 0, 0);
        if(screen_x11->shminfo.shmaddr == (char *) -1) {
            fprintf(stderr, "couldn't allocate shared memory\n");
            XDestroyImage(ximg);
            ximg = NULL;
            exit(EXIT_FAILURE);
        } else {
            // set as read/write, and attach to the display
            screen_x11->shminfo.readOnly = False;
            XShmAttach(screen_x11->display, &screen_x11->shminfo);
        }
    }
    return ximg;
}
