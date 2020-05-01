#include <stdlib.h>
#include <stdio.h>
#define __USE_POSIX199309
#include <time.h>
#include <limits.h>
#include <inttypes.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/xf86vmode.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <tiff.h>

#include "../graphics_utils/common.h"
#include "../graphics_utils/screen_info_rgb_p.h"
#include "../app/screen.h"

// font used with text drawing
#define FONT_NAME "-*-fixed-*-*-*-*-18-*-*-*-*-*-iso8859-*" //"-*-fixed-*-r-*-*-22-*-*-*-*-*-*-*" //"*-helvetica-*-12-*"

#define _NET_WM_STATE_REMOVE    0l
#define _NET_WM_STATE_ADD       1l

// Internal key and button state/action definitions
#define STICK 2

typedef struct screen screen_x11_t;

struct screen {
    // callback functions ??
    fun_key_t key_callback;    // see glfw.h for other callbacks (and remember to init them!)
    fun_close_t windowCloseCallback; // TODO aggiungere a app

    // Window and framebuffer attributes
    bool no_resize;     // Resize- and maximize gadgets disabled flag
    bool fullscreen;    // Fullscreen flag
    bool mouseLock;       // Mouse-lock flag // TODO
//    bool       autoPollEvents;  // Auto polling flag // Not used?

    // Window status & parameters
    bool opened;          // Flag telling if window is opened or not // TODO
    bool active;          // Application active flag // TODO
    bool iconified;       // Window iconified flag // TODO
    uint32_t x, y;      // win top left coords
    uint32_t width, height;                 // window and screen_buffer size
    uint32_t depth, bytespp, scanline_pad; // X11: info about X server
    uint32_t buf_size;                      // screen_buffer size
    uint8_t *buffer;                    // offscreen screen_buffer data for ximage
//    XFontStruct* font_info;
    screen_info_rgb_t *screen_info_rgb;

    // ========= PLATFORM SPECIFIC PART ======================================
    Visual *vis;                      // X11: Visual (visual info about X server)
    Window window;                         // X11: Window
    Window root;                    // Root window for screen
    GC gc;                          // X11: graphics context
    XShmSegmentInfo shminfo;        // X11 shm
    XImage *ximg;                   // X11: XImage which will be actually shown
    uint32_t screen_idx;            // Screen ID
    int64_t event_mask;             // event checking mask
    Display *display;                     // X11: Display (connection to X server)
    Cursor cursor;                  // Invisible cursor for hidden cursor
    Atom wmStateFullscreen;         // _NET_WM_STATE_FULLSCREEN atom
    Atom wmState;                   // _NET_WM_STATE atom
    bool pointer_hidden;
    bool pointer_grabbed;

    // GLboolean   overrideRedirect; // True if window is OverrideRedirect // TODO vedere dove usata?

    // Fullscreen data
    struct {
        bool modeChanged;
        XF86VidModeModeInfo oldMode;
    } FS;

    Atom wmPing;
    Atom wmActiveWindow;
    bool hasEWMH;
    bool overrideRedirect;

    // TODO FIX NAMES !!
    struct {
        // Mouse status
        int  mousePosX, mousePosY;
        int  wheelPos;
        char mouseButton[MOUSE_BUTTON_LAST + 1];

        // Keyboard status
        char key[KEY_LAST + 1];
        int  lastChar;

        // User selected settings
        bool  stickyKeys;
        bool  StickyMouseButtons;
        bool  KeyRepeat;
        // Platform specific internal variables
        bool MouseMoved;
        int  CursorPosX, CursorPosY;
    } input;
};

// TODO fix names??
static void init_screen_buffer(screen_x11_t *screen);
static XImage *create_image(screen_x11_t *screen_x11);
static void calc_win_center_coords(screen_x11_t *screen);
static void set_size_hints(screen_x11_t *screen_x11);
static bool check_for_EWMH(screen_x11_t *screen);
// TODO

static void set_video_mode(screen_x11_t *screen_x11, int *width, int *height);
static Atom getSupportedAtom(screen_x11_t *screen_x11, Atom* supportedAtoms,
                             unsigned long atomCount,
                             const char* atomName);
static unsigned long getWindowProperty(screen_x11_t *screen_x11, Window window,
                                       Atom property,
                                       Atom type,
                                       unsigned char** value);

static void set_window_pos(screen_x11_t *screen, int x, int y );
static void set_window_title(screen_x11_t *screen, const char *title);
static Cursor create_null_cursor(Display *display, Window root);
static void hide_mouse_cursor(screen_x11_t *screen);
static void set_mouse_cursor_pos(screen_x11_t *screen, int x, int y);
static void change_video_mode(screen_x11_t *screen, int mode);
static int find_best_video_mode(screen_x11_t *screen, int *width, int *height);
static void restore_video_mode(screen_x11_t *screen);
static void set_window_size(screen_x11_t *screen);
static void leave_full_screen_mode(screen_x11_t *screen);
static void enter_full_screen(screen_x11_t *screen);
static void init_atom_support(screen_x11_t *screen_x11);
static void show_mouse_cursor(screen_x11_t *screen);
static bool process_single_event(screen_x11_t *screen);
static void input_key(screen_x11_t *screen, int key, int action);
static void clear_input(screen_x11_t *screen);

static void grab_mouse(screen_x11_t *screen);

static int translateKey(screen_x11_t *screen, int keycode);


// ...
// TODO see createWindow e _glfwPlatformOpenWindow in glfw/lib/x11/x11_window.c
// e anche glfwOpenWindow in window.c (generico, no x11)
// _glfwPlatformCloseWindow
screen_x11_t *init_screen(screen_settings_t *screen_settings) {

    screen_x11_t *screen = malloc(sizeof(screen_x11_t));
    memset(screen, 0, sizeof(screen_x11_t));

    screen->width = screen_settings->width;
    screen->height = screen_settings->height;
    screen->fullscreen = screen_settings->fullscreen;
    screen->no_resize = true;

    if (!(screen->display = XOpenDisplay(NULL))) {
        fprintf(stderr, "couldn't open a display connection\n");
        exit(EXIT_FAILURE);
    }

    // find out the supported depths of this server and take the default one
    int default_depth = DefaultDepth(screen->display, 0);
    int count;
    XPixmapFormatValues *pixmap_formats = XListPixmapFormats(screen->display, &count);
    for (int i = 0; i != count; i++) {
        if (pixmap_formats[i].depth == default_depth) {
            screen->depth = (uint32_t) pixmap_formats[i].depth;
            screen->bytespp = (uint32_t) (pixmap_formats[i].bits_per_pixel / BITS_PER_BYTE);
            screen->scanline_pad = (uint32_t) pixmap_formats[i].scanline_pad;
        }
    }
    XFree(pixmap_formats);
    printf("default depth of display %d\n", screen->depth);
    printf("bytes per pixel: %d\n", screen->bytespp);

//    // create and map (display) an X window for output
//    int count_screens = ScreenCount(screen->display);
//    printf("Total count screens: %d\n", count_screens);
//    for (int i = 0; i < count_screens; ++i) {
//        Screen *screen = ScreenOfDisplay(screen->display, i);
//        printf("\tScreen %d: %dX%d\n", i + 1, screen->width, screen->height);
//    }

    screen->screen_idx = DefaultScreen(screen->display);
    screen->vis = DefaultVisual(screen->display, screen->screen_idx);
    screen->root = RootWindow(screen->display, screen->screen_idx ); // DefaultRootWindow(screen->display);
    screen->window = XCreateWindow(screen->display,                // display
                                   screen->root,     // parent
                                   0, 0,                   // x, y position, remapped later set_window_with set_window_pos
                                   screen->width, // width
                                   screen->height, // height
                                   0,                          // border width
                                   (int32_t) screen->depth,          // default_depth (we use max. possible)
                                   CopyFromParent,             // visual class (TrueColor etc)
                                   screen->vis,            // visual
                                   0, NULL);                   // valuemask, window attributes

    init_atom_support(screen); // requires the root... // TODO

    // Check whether an EWMH-compliant window manager is running
    screen->hasEWMH = check_for_EWMH(screen);

    if(screen->fullscreen && !screen->hasEWMH ) {
        // This is the butcher's way of removing window decorations
        // Setting the override-redirect attribute on a window makes the window
        // manager ignore the window completely (ICCCM, section 4)
        // The good thing is that this makes undecorated fullscreen windows
        // easy to do; the bad thing is that we have to do everything manually
        // and some things (like iconify/restore) won't work at all, as they're
        // usually performed by the window manager

        XSetWindowAttributes attributes;
        attributes.override_redirect = True;
        XChangeWindowAttributes(screen->display,
                                screen->window,
                                CWOverrideRedirect,
                                &attributes );

        screen->overrideRedirect = true;
    }

    // Create the invisible cursor for hidden cursor mode
    screen->cursor = create_null_cursor(screen->display, screen->root );
    set_window_title(screen, screen_settings->window_title);

//    set_size_hints(screen);

    // Make sure the window is mapped before proceeding
    XMapWindow(screen->display, screen->window);
    calc_win_center_coords(screen);
    set_window_pos(screen, screen->x, screen->y); // da chiamare dopo XMapWindow
    XFlush(screen->display);

    switch (screen->vis->class) {
        // constants for visual classes are defined in /usr/include/X11/X.h
        case TrueColor: {
            XVisualInfo vi;
            int result = XMatchVisualInfo(screen->display, screen->screen_idx, (int32_t) screen->depth, TrueColor, &vi);
            if (result) {
                printf("visual is TrueColor, %d bytes per pix, %d bytes per rgb\n",
                       screen->bytespp,
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

    screen->event_mask = KeyPressMask | KeyReleaseMask;
    // quali mask usare? vedi wa.event_mask =  in createWindow
//    StructureNotifyMask | KeyPressMask | KeyReleaseMask |
//    PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
//    ExposureMask | FocusChangeMask | VisibilityChangeMask;
    XSelectInput(screen->display, screen->window, screen->event_mask);

    screen->gc = XCreateGC(screen->display, screen->window, 0, NULL); // DefaultGC(screen->dpy, screen_num);
    /* change the foreground color of this GC to white. */
    XSetForeground(screen->display, screen->gc, WhitePixel(screen->display, screen->screen_idx));

    screen->screen_info_rgb = init_screen_info_rgb(
            screen->vis->red_mask, screen->vis->green_mask, screen->vis->blue_mask,
            screen->bytespp, screen->depth / BITS_PER_BYTE, NULL);

    screen->pointer_hidden = false;
    screen->pointer_grabbed = false;

    screen->FS.modeChanged = false;
    set_window_size(screen);

    if (screen->fullscreen) {
        enter_full_screen(screen);
    }

    // Process the window map event and any other that may have arrived
    poll_events(screen);

    init_screen_buffer(screen);

    // Clear GLFW window state
    screen->active         = true;
    screen->iconified      = false;
    screen->mouseLock      = true;
//    screen->autoPollEvents = true; // not used
    clear_input(screen);

    // Retrieve and set initial cursor position
    {
        Window window, root;
        int windowX, windowY, rootX, rootY;
        unsigned int mask;

        XQueryPointer(screen->display,
                      screen->window,
                      &root,
                      &window,
                      &rootX, &rootY,  /* Screen coords relative to root window's top-left */
                      &windowX, &windowY, /* Client coords relative to window's top-left */
                      &mask );

        // TODO: Probably check for some corner cases here.

        screen->input.mousePosX = windowX;
        screen->input.mousePosY = windowY;
    }

    screen->key_callback = NULL;
    screen->windowCloseCallback = NULL;

    return screen;
}

void set_key_callback(screen_t *screen, fun_key_t key_fun) {
    screen->key_callback = key_fun;
}

screen_info_rgb_t *get_screen_info(screen_x11_t *screen) {
    return screen->screen_info_rgb;
}

void terminate_screen(screen_x11_t *screen) {
    // vedi _glfwPlatformCloseWindow
    if (screen->fullscreen) {
        leave_full_screen_mode(screen);
    }

    free(screen->screen_info_rgb);
    screen->screen_info_rgb = NULL;

    XShmDetach(screen->display, &screen->shminfo);
    XDestroyImage(screen->ximg);
    shmdt(screen->shminfo.shmaddr);
    //delete [] screen_buffer; // automatically done by XDestroyImage ! See man page...
    screen->buffer = NULL;
    screen->ximg = NULL;

//    XFree(screen->vis); // we are using the default one...
//    screen->vis = NULL;

    XFreeCursor(screen->display, screen->cursor );
    screen->cursor = (Cursor) 0;

    XFreeGC(screen->display, screen->gc); // error if using the default one?
    screen->gc = NULL;

    XUnmapWindow(screen->display, screen->window);
    XDestroyWindow(screen->display, screen->window);

    XFlush(screen->display);
    XCloseDisplay(screen->display);
    memset(screen, 0, sizeof(screen_x11_t));
}

//========================================================================
// Poll for new window and input events
//========================================================================
void poll_events(screen_x11_t *screen) {

    bool close_requested = false;

    // Flag that the cursor has not moved
    screen->input.MouseMoved = false;

    // Process all pending events
    while(XPending(screen->display )) {
        if(process_single_event(screen)) {
            close_requested = true;
        }
    }

    // Did we get mouse movement in fully enabled hidden cursor mode?
    if(screen->input.MouseMoved && screen->pointer_hidden) {

        set_mouse_cursor_pos(screen, screen->width/2, screen->height/2);

        // NOTE: This is a temporary fix.  It works as long as you use offsets
        //       accumulated over the course of a frame, instead of performing
        //       the necessary actions per callback call.
        XFlush(screen->display);
    }

    if(close_requested && screen->windowCloseCallback) {
        close_requested = screen->windowCloseCallback();
    }

    if(close_requested) {
        terminate_screen(screen);
    }
}

//// vedi translateKey in window.c glfw
//void check_event(screen_x11_t *screen) {
//
////    process_single_event(screen);
//
//    poll_events(screen);
//
//    // required in fullscreen ?
////    set_mouse_cursor_pos(screen, 0, 0);
////    set_mouse_cursor_pos(screen, screen->width / 2, screen->height / 2 );
//
//    // NOTE: This is a temporary fix.  It works as long as you use offsets
//    //       accumulated over the course of a frame, instead of performing
//    //       the necessary actions per callback call.
////    XFlush(screen->display);
//
////    if(XCheckWindowEvent(screen_idx->display, screen_idx->window, KeyPressMask, &event)) {
////        XLookupString(&event.xkey, &ch, 1, &keysym, &xcompstat);
////        app_key_event_fptr(app, ch);
////    }
//}

void blit(screen_x11_t *screen) {
    XShmPutImage(screen->display, screen->window, screen->gc, screen->ximg,
                 0, 0, 0, 0,
                 (unsigned int)screen->width, (unsigned int)screen->height,
                 False);
    XSync(screen->display, False);
}



/**************************************************************************************/
// PRIVATE FUNCTIONS
/**************************************************************************************/

static void init_screen_buffer(screen_x11_t *screen) {
    screen->ximg = create_image(screen);
    screen->buf_size = screen->width * screen->height * screen->bytespp; // used for clearing screen_num buf
//    screen->buffer = malloc(width*height*screen->bytespp); // not necessary with the shm
    screen->buffer = (uint8_t *) (screen->ximg->data);
    update_screen_buffer(screen->screen_info_rgb, screen->buffer);
}

// Set window & icon title
static void set_window_title(screen_x11_t *screen, const char *title) {
    XStoreName(screen->display, screen->window, title);
    XSetIconName(screen->display, screen->window, title);
}

static void calc_win_center_coords(screen_x11_t *screen) {
    int display_width = XDisplayWidth(screen->display, screen->screen_idx) / 2; // TODO fix /2 ?? pare che la width comprenda entrambi i monitor...
    int display_height = XDisplayHeight(screen->display, screen->screen_idx);
    screen->x = (display_width - screen->width) / 2;
    screen->y = (display_height - screen->height) / 2;
}

static XImage *create_image(screen_x11_t *screen_x11) {
    XImage *ximg = XShmCreateImage(screen_x11->display, screen_x11->vis, (unsigned int) screen_x11->depth,
                                   ZPixmap,   // format
                                   0,  // pixels to ignore at beg. of scanline
                                   &screen_x11->shminfo,
                                   (unsigned int)screen_x11->width, (unsigned int)screen_x11->height);
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

static void init_atom_support(screen_x11_t *screen_x11) {
    Atom *supportedAtoms;
    unsigned long atomCount;

    Atom wmSupported = XInternAtom( screen_x11->display,
                                    "_NET_SUPPORTED",
                                    True );
    // Now we need to check the _NET_SUPPORTED property of the root window
    atomCount = getWindowProperty( screen_x11, screen_x11->root,
                                   wmSupported,
                                   XA_ATOM,
                                   (unsigned char**) &supportedAtoms );


//    Atom wmActiveWindow = getSupportedAtom(screen_x11, supportedAtoms,
//                                           atomCount,
//                                           "_NET_ACTIVE_WINDOW");

    // See which of the atoms we support that are supported by the WM

    screen_x11->wmState = getSupportedAtom(screen_x11, supportedAtoms,
                                           atomCount,
                                           "_NET_WM_STATE");

    screen_x11->wmStateFullscreen = getSupportedAtom(screen_x11, supportedAtoms,
                                                     atomCount,
                                                     "_NET_WM_STATE_FULLSCREEN");
}

static void enter_full_screen(screen_x11_t *screen) {

//    set_video_mode(screen, screen->width, screen->height); // already done in set win size?

    if(screen->hasEWMH &&
       screen->wmState != None &&
       screen->wmStateFullscreen != None ) {

        if (screen->wmActiveWindow != None) {
            // Ask the window manager to raise and focus the GLFW window
            // Only focused windows with the _NET_WM_STATE_FULLSCREEN state end
            // up on top of all other windows ("Stacking order" in EWMH spec)

            XEvent event = {0};

            event.type = ClientMessage;
            event.xclient.window = screen->window;
            event.xclient.format = 32; // Data is 32-bit longs
            event.xclient.message_type = screen->wmActiveWindow;
            event.xclient.data.l[0] = 1; // Sender is a normal application
            event.xclient.data.l[1] = 0; // We don't really know the timestamp

            XSendEvent(screen->display,
                       screen->root,
                       False,
                       SubstructureNotifyMask | SubstructureRedirectMask,
                       &event);
        }

        XEvent event = {0};

        event.type = ClientMessage;
        event.xclient.window = screen->window;
        event.xclient.format = 32; // Data is 32-bit longs
        event.xclient.message_type = screen->wmState;
        event.xclient.data.l[0] = _NET_WM_STATE_ADD;
        event.xclient.data.l[1] = screen->wmStateFullscreen;
        event.xclient.data.l[2] = 0; // No secondary property
        event.xclient.data.l[3] = 1; // Sender is a normal application

        //    enterFullscreenMode
        XSendEvent(screen->display,
                   screen->root,
                   False,
                   SubstructureNotifyMask | SubstructureRedirectMask,
                   &event);

    } else if(screen->overrideRedirect )
    {
        // In override-redirect mode, we have divorced ourselves from the
        // window manager, so we need to do everything manually
        XRaiseWindow(screen->display, screen->window );
        XSetInputFocus(screen->display, screen->window,
                       RevertToParent, CurrentTime );
        XMoveWindow(screen->display, screen->window, 0, 0 );
        XResizeWindow(screen->display, screen->window,
                      screen->width, screen->height );
    }

    if(screen->mouseLock ) {
        hide_mouse_cursor(screen);
    }

    // HACK: Try to get window inside viewport (for virtual displays) by moving
    // the mouse cursor to the upper left corner (and then to the center)
    // This hack should be harmless on saner systems as well
    set_mouse_cursor_pos(screen, 0, 0);
    set_mouse_cursor_pos(screen, screen->width / 2, screen->height / 2 );
    XFlush(screen->display);
}

//========================================================================
// Retrieve a single window property of the specified type
// Inspired by fghGetWindowProperty from freeglut
//========================================================================
static unsigned long getWindowProperty( screen_x11_t *screen_x11,
                                        Window window,
                                        Atom property,
                                        Atom type,
                                        unsigned char** value )
{
    Atom actualType;
    int actualFormat;
    unsigned long itemCount, bytesAfter;

    XGetWindowProperty(screen_x11->display,
                       window,
                       property,
                       0,
                       LONG_MAX,
                       False,
                       type,
                       &actualType,
                       &actualFormat,
                       &itemCount,
                       &bytesAfter,
                       value );

    if( actualType != type )
    {
        return 0;
    }

    return itemCount;
}

// vedi X11_CreateWindow (video x11 sdl2)
static void set_size_hints(screen_x11_t *screen) {// see http://ftp.dim13.org/pub/doc/Xlib.pdf
//    search for XSizeHints here https://www.x.org/releases/current/doc/libX11/libX11/libX11.pdf
//    https://www.linuxquestions.org/questions/programming-9/%5Bc%5D-x11-window-of-fixed-aspect-ratio-843617/
    XSizeHints *size_hints = XAllocSizeHints();
    if (!size_hints) {
        fprintf(stderr, "XAllocSizeHints - couldn't allocate\n");
        exit(EXIT_FAILURE);
    }
    size_hints->flags = 0;
    if (screen->no_resize && !screen->fullscreen) {
        size_hints->min_width = size_hints->max_width = screen->width;
        size_hints->min_height = size_hints->max_height = screen->height;
        size_hints->flags |= (PMinSize | PMaxSize);
    }
    XSetWMNormalHints(screen->display, screen->window, size_hints );
    XFree(size_hints);
}

//========================================================================
// Set the window size
//========================================================================

static void set_window_size(screen_x11_t *screen)
{
    int mode = 0, rate, sizeChanged = false;
    int width = screen->width; // start with these...
    int height = screen->height;

    if(screen->fullscreen ) {
        // Get the closest matching video mode for the specified window size
        mode = find_best_video_mode(screen, &width, &height);
    }

    set_size_hints(screen);

    // Change window size before changing fullscreen mode?
    if(screen->fullscreen && (width > screen->width) )
    {
        XResizeWindow(screen->display, screen->window, width, height);
        screen->width = width;
        screen->height = height;
        sizeChanged = true;
    }

    if(screen->fullscreen )
    {
        // Change video mode, keeping current refresh rate
        change_video_mode(screen, mode);
    }

    // Set window size (if not already changed)
    if(!sizeChanged)
    {
        XResizeWindow(screen->display, screen->window, width, height);
    }
}

// https://stackoverflow.com/questions/12706631/x11-change-resolution-and-make-window-fullscreen
static int find_best_video_mode(screen_x11_t *screen, int *width, int *height)
{
    XF86VidModeModeInfo **modelist;
    int modecount, i, bestmode, bestmatch, match;

    // Get a list of all available display modes
    if (XF86VidModeGetAllModeLines(screen->display, screen->screen_idx,
                                   &modecount, &modelist ))
    {
        // Find the best matching mode
        bestmode  = -1;
        bestmatch = INT_MAX;
        for( i = 0; i < modecount; i++ )
        {
            match = (*width - modelist[i]->hdisplay) *
                    (*width - modelist[i]->hdisplay) +
                    (*height - modelist[i]->vdisplay) *
                    (*height - modelist[i]->vdisplay);
            if( match < bestmatch )
            {
                bestmatch = match;
                bestmode  = i;
            }
        }

        if( bestmode != -1 )
        {
            // Report width & height of best matching mode
            *width = modelist[ bestmode ]->hdisplay;
            *height = modelist[ bestmode ]->vdisplay;
            printf("full screen mode %d, %d\n", *width, *height);
        }

        // Free modelist
        XFree( modelist );

        if( bestmode != -1 )
        {
            return bestmode;
        }
    }

    return -1;
}

//========================================================================
// Change the current video mode
//========================================================================
static void change_video_mode(screen_x11_t *screen, int mode)
{
    XF86VidModeModeInfo **modelist;
    int modecount;
    int screen_idx = screen->screen_idx;

    // Get a list of all available display modes
    XF86VidModeGetAllModeLines(screen->display, screen_idx, &modecount, &modelist );

//    // Unlock mode switch if necessary
    if(screen->FS.modeChanged) {
        XF86VidModeLockModeSwitch(screen->display, screen_idx, 0);
    }

    // Change the video mode to the desired mode
    XF86VidModeSwitchToMode(screen->display, screen_idx, modelist[mode]);

    // Set viewport to upper left corner (where our window will be)
    XF86VidModeSetViewPort(screen->display, screen_idx, 0, 0);

    // Lock mode switch
    XF86VidModeLockModeSwitch(screen->display, screen_idx, 1);

    XFlush(screen->display);

    // Remember old mode and flag that we have changed the mode
    if(!screen->FS.modeChanged) {
        screen->FS.oldMode = *modelist[0];
        screen->FS.modeChanged = true;
    }

    // Free mode list
    XFree(modelist);
}

//========================================================================
// Change the current video mode
//========================================================================

static void set_video_mode(screen_x11_t *screen_x11, int *width, int *height)
{
    // Find a best match mode
    int best_mode = find_best_video_mode(screen_x11, width, height);
    // Change mode
    change_video_mode(screen_x11, best_mode);
}

//========================================================================
// Check whether the specified atom is supported
//========================================================================

static Atom getSupportedAtom(screen_x11_t *screen_x11, Atom *supportedAtoms, unsigned long atomCount, const char *atomName) {
    Atom atom = XInternAtom( screen_x11->display, atomName, True );
    if( atom != None )
    {
        unsigned long i;
        for( i = 0;  i < atomCount;  i++ )
        {
            if( supportedAtoms[i] == atom )
            {
                return atom;
            }
        }
    }
    return None;
}

static void leave_full_screen_mode(screen_x11_t *screen) {

    restore_video_mode(screen);

    screen->FS.modeChanged = false;

    // Ask the window manager to make the GLFW window a normal window
    // Normal windows usually have frames and other decorations
    XEvent event = { 0 };

    event.type = ClientMessage;
    event.xclient.window = screen->window;
    event.xclient.format = 32; // Data is 32-bit longs
    event.xclient.message_type = screen->wmState;
    event.xclient.data.l[0] = _NET_WM_STATE_REMOVE;
    event.xclient.data.l[1] = screen->wmStateFullscreen;
    event.xclient.data.l[2] = 0; // No secondary property
    event.xclient.data.l[3] = 1; // Sender is a normal application

    XSendEvent(screen->display,
               screen->root,
               False,
               SubstructureNotifyMask | SubstructureRedirectMask,
               &event);

    if(screen->mouseLock ) {
        show_mouse_cursor(screen);
    }
}

static void restore_video_mode(screen_x11_t *screen) {
    // Unlock mode switch
    XF86VidModeLockModeSwitch(screen->display, screen->screen_idx, 0 );

    // Change the video mode back to the old mode
    XF86VidModeSwitchToMode( screen->display,
                             screen->screen_idx,
                             &screen->FS.oldMode );
}

//========================================================================
// Create a blank cursor (for locked mouse mode)
//========================================================================
static Cursor create_null_cursor(Display *display, Window root) {
    Pixmap    cursormask;
    XGCValues xgc;
    GC        gc;
    XColor    col;
    Cursor    cursor;

    cursormask = XCreatePixmap( display, root, 1, 1, 1 );
    xgc.function = GXclear;
    gc = XCreateGC( display, cursormask, GCFunction, &xgc );
    XFillRectangle( display, cursormask, gc, 0, 0, 1, 1 );
    col.pixel = 0;
    col.red = 0;
    col.flags = 4;
    cursor = XCreatePixmapCursor( display, cursormask, cursormask,
                                  &col,&col, 0,0 );
    XFreePixmap( display, cursormask );
    XFreeGC( display, gc );

    return cursor;
}


//========================================================================
// Show mouse cursor (unlock it)
//========================================================================
static void show_mouse_cursor(screen_x11_t *screen) {
    // Un-grab cursor (only in windowed mode: in fullscreen mode we still
    // want the mouse grabbed in order to confine the cursor to the window
    // area)
    if(screen->pointer_grabbed) {
        XUngrabPointer(screen->display, CurrentTime );
        screen->pointer_grabbed = false;
    }

    // Show cursor
    if(screen->pointer_hidden) {
        XUndefineCursor(screen->display, screen->window);
        screen->pointer_hidden = false;
    }
}

//========================================================================
// Hide mouse cursor (lock it)
//========================================================================
static void hide_mouse_cursor(screen_x11_t *screen) {
    // Hide cursor
    if (!screen->pointer_hidden)
    {
        // comment these 2 lines to not hide the cursor
        XDefineCursor(screen->display, screen->window, screen->cursor); // TODO prova a commentare questo?
        screen->pointer_hidden = True;
    }

    grab_mouse(screen);

    // Move cursor to the middle of the window
    set_mouse_cursor_pos(screen, screen->width / 2, screen->height / 2 );
}

static void grab_mouse(screen_x11_t *screen) {// Grab cursor to user window
    if(!screen->pointer_grabbed) {
        if(XGrabPointer(screen->display, screen->window, True,
                          ButtonPressMask | ButtonReleaseMask |
                          PointerMotionMask, GrabModeAsync, GrabModeAsync,
                         screen->window, None, CurrentTime ) == GrabSuccess ) {
            screen->pointer_grabbed = True;
            printf("pointer grabbed\n");
        }
    }
}

//========================================================================
// Set physical mouse cursor position
//========================================================================
void set_mouse_cursor_pos(screen_x11_t *screen, int x, int y)
{
    // Store the new position so we can recognise it later
    screen->input.CursorPosX = x;
    screen->input.CursorPosY = y;

    XWarpPointer(screen->display, None, screen->window, 0,0,0,0, x, y );
//    XFlush(screen->display);
}


//========================================================================
// Check whether the running window manager is EWMH-compliant
//========================================================================
static bool check_for_EWMH(screen_x11_t *screen)
{
    Window *windowFromRoot = NULL;
    Window *windowFromChild = NULL;

    // Hey kids; let's see if the window manager supports EWMH!

    // First we need a couple of atoms, which should already be there
    Atom supportingWmCheck = XInternAtom(screen->display,
                                          "_NET_SUPPORTING_WM_CHECK",
                                          True );
    Atom wmSupported = XInternAtom(screen->display,
                                    "_NET_SUPPORTED",
                                    True );
    if( supportingWmCheck == None || wmSupported == None )
    {
        return false;
    }

    // Then we look for the _NET_SUPPORTING_WM_CHECK property of the root window
    if( getWindowProperty(screen,
                           screen->root,
                           supportingWmCheck,
                           XA_WINDOW,
                           (unsigned char**) &windowFromRoot ) != 1 )
    {
        XFree( windowFromRoot );
        return false;
    }

    // It should be the ID of a child window (of the root)
    // Then we look for the same property on the child window
    if( getWindowProperty( screen, *windowFromRoot,
                           supportingWmCheck,
                           XA_WINDOW,
                           (unsigned char**) &windowFromChild ) != 1 )
    {
        XFree( windowFromRoot );
        XFree( windowFromChild );
        return false;
    }

    // It should be the ID of that same child window
    if( *windowFromRoot != *windowFromChild )
    {
        XFree( windowFromRoot );
        XFree( windowFromChild );
        return false;
    }

    XFree( windowFromRoot );
    XFree( windowFromChild );

    // We are now fairly sure that an EWMH-compliant window manager is running

    Atom *supportedAtoms;
    unsigned long atomCount;

    // Now we need to check the _NET_SUPPORTED property of the root window
    atomCount = getWindowProperty(screen, screen->root,
                                   wmSupported,
                                   XA_ATOM,
                                   (unsigned char**) &supportedAtoms );

    // See which of the atoms we support that are supported by the WM

    screen->wmState = getSupportedAtom(screen, supportedAtoms,
                                         atomCount,
                                         "_NET_WM_STATE" );

    screen->wmStateFullscreen = getSupportedAtom(screen, supportedAtoms,
                                                   atomCount,
                                                   "_NET_WM_STATE_FULLSCREEN" );

    screen->wmPing = getSupportedAtom(screen, supportedAtoms,
                                        atomCount,
                                        "_NET_WM_PING" );

    screen->wmActiveWindow = getSupportedAtom(screen, supportedAtoms,
                                                atomCount,
                                                "_NET_ACTIVE_WINDOW" );

    XFree( supportedAtoms );
    return true;
}

//========================================================================
// Set the window position.
//========================================================================

static void set_window_pos(screen_x11_t *screen, int x, int y ) {
    XMoveWindow(screen->display, screen->window, x, y );
}

//{
//int keysyms_per_keycode_return;
//KeySym *keysym = XGetKeyboardMapping(dpy,
//                                     xe->xkey.keycode,
//                                     1,
//                                     &keysyms_per_keycode_return);
//
///* do something with keysym[0] */
//
//XFree(keysym);
//}

static bool process_single_event(screen_x11_t *screen) {

    XEvent event;
    XNextEvent(screen->display, &event );
//    if (XCheckWindowEvent(screen->display, screen->window, screen->event_mask, &event)) // not used
    // if or while?
    switch (event.type)
    {
        case KeyPress: {
            // A keyboard key was pressed

            // Translate and report key press
            input_key(screen, translateKey(screen, event.xkey.keycode), KEY_PRESS);

//                // Translate and report character input
//                if( _glfwWin.charCallback )
//                {
//                    _glfwInputChar( translateChar( &event.xkey ), KEY_PRESS );
//                }

//                char ch;
//                KeySym keysym;
//                XComposeStatus xcompstat;
////                printf("pressed\n");
//                XLookupString(&event.xkey, &ch, 1, &keysym, &xcompstat);
////                app_key_event_fptr(app, ch);  // TODO
            break;
        }

        case KeyRelease:
//                printf("released\n");
            break;
        default:
            break;
    }

    // The window was not destroyed
    return false;
}


//========================================================================
// Clear all input state
//========================================================================

static void clear_input(screen_x11_t *screen)
{
    // Release all keyboard keys
    for(int i = 0; i <= KEY_LAST; i ++ ) {
        screen->input.key[i] = KEY_RELEASE;
    }

    // Clear last character
    screen->input.lastChar = 0;

    // Release all mouse buttons
    for(int i = 0; i <= MOUSE_BUTTON_LAST; i ++ ) {
        screen->input.mouseButton[i] = KEY_RELEASE;
    }

    // Set mouse position to (0,0)
    screen->input.mousePosX = 0;
    screen->input.mousePosY = 0;

    // Set mouse wheel position to 0
    screen->input.wheelPos = 0;

    // The default is to use non sticky keys and mouse buttons
    screen->input.stickyKeys = false;
    screen->input.StickyMouseButtons = false;

    // The default is to disable key repeat
    screen->input.KeyRepeat = false;
}

//========================================================================
// Register keyboard activity
//========================================================================

static void input_key(screen_x11_t *screen, int key, int action)
{
    bool keyrepeat = false;

    if (key < 0 || key > KEY_LAST) {
        return;
    }

    // Are we trying to release an already released key?
    if (action == KEY_RELEASE && screen->input.key[key] != KEY_PRESS) {
        return;
    }

    // Register key action
    if (action == KEY_RELEASE && screen->input.stickyKeys) {
        screen->input.key[key] = STICK;
    }
    else {
        keyrepeat = (screen->input.key[key] == KEY_PRESS) &&
                    (action == KEY_PRESS);
        screen->input.key[key] = (char) action;
    }

    // Call user callback function
    if (screen->key_callback && (screen->input.KeyRepeat || !keyrepeat)) {
        screen->key_callback(key, action);
    }
}

static int translateKey(screen_x11_t *screen, int keycode) {

    KeySym key, key_lc, key_uc;

    // Try secondary keysym, for numeric keypad keys
    // Note: This way we always force "NumLock = ON", which at least
    // enables users to detect numeric keypad keys
    key = XkbKeycodeToKeysym(screen->display, keycode, 0, 1);
    switch( key )
    {
        // Numeric keypad
        case XK_KP_0:         return KEY_KP_0;
        case XK_KP_1:         return KEY_KP_1;
        case XK_KP_2:         return KEY_KP_2;
        case XK_KP_3:         return KEY_KP_3;
        case XK_KP_4:         return KEY_KP_4;
        case XK_KP_5:         return KEY_KP_5;
        case XK_KP_6:         return KEY_KP_6;
        case XK_KP_7:         return KEY_KP_7;
        case XK_KP_8:         return KEY_KP_8;
        case XK_KP_9:         return KEY_KP_9;
        case XK_KP_Separator:
        case XK_KP_Decimal:   return KEY_KP_DECIMAL;
        case XK_KP_Equal:     return KEY_KP_EQUAL;
        case XK_KP_Enter:     return KEY_KP_ENTER;
        default:              break;
    }

    // Now try pimary keysym
    key = XkbKeycodeToKeysym(screen->display, keycode, 0, 0);
    switch( key )
    {
        // Special keys (non character keys)
        case XK_Escape:       return KEY_ESC;
        case XK_Tab:          return KEY_TAB;
        case XK_Shift_L:      return KEY_LSHIFT;
        case XK_Shift_R:      return KEY_RSHIFT;
        case XK_Control_L:    return KEY_LCTRL;
        case XK_Control_R:    return KEY_RCTRL;
        case XK_Meta_L:
        case XK_Alt_L:        return KEY_LALT;
        case XK_Mode_switch:  // Mapped to Alt_R on many keyboards
        case XK_Meta_R:
        case XK_ISO_Level3_Shift: // AltGr on at least some machines
        case XK_Alt_R:        return KEY_RALT;
        case XK_Super_L:      return KEY_LSUPER;
        case XK_Super_R:      return KEY_RSUPER;
        case XK_Menu:         return KEY_MENU;
        case XK_Num_Lock:     return KEY_KP_NUM_LOCK;
        case XK_Caps_Lock:    return KEY_CAPS_LOCK;
        case XK_Scroll_Lock:  return KEY_SCROLL_LOCK;
        case XK_Pause:        return KEY_PAUSE;
        case XK_KP_Delete:
        case XK_Delete:       return KEY_DEL;
        case XK_BackSpace:    return KEY_BACKSPACE;
        case XK_Return:       return KEY_ENTER;
        case XK_KP_Home:
        case XK_Home:         return KEY_HOME;
        case XK_KP_End:
        case XK_End:          return KEY_END;
        case XK_KP_Page_Up:
        case XK_Page_Up:      return KEY_PAGEUP;
        case XK_KP_Page_Down:
        case XK_Page_Down:    return KEY_PAGEDOWN;
        case XK_KP_Insert:
        case XK_Insert:       return KEY_INSERT;
        case XK_KP_Left:
        case XK_Left:         return KEY_LEFT;
        case XK_KP_Right:
        case XK_Right:        return KEY_RIGHT;
        case XK_KP_Down:
        case XK_Down:         return KEY_DOWN;
        case XK_KP_Up:
        case XK_Up:           return KEY_UP;
        case XK_F1:           return KEY_F1;
        case XK_F2:           return KEY_F2;
        case XK_F3:           return KEY_F3;
        case XK_F4:           return KEY_F4;
        case XK_F5:           return KEY_F5;
        case XK_F6:           return KEY_F6;
        case XK_F7:           return KEY_F7;
        case XK_F8:           return KEY_F8;
        case XK_F9:           return KEY_F9;
        case XK_F10:          return KEY_F10;
        case XK_F11:          return KEY_F11;
        case XK_F12:          return KEY_F12;
        case XK_F13:          return KEY_F13;
        case XK_F14:          return KEY_F14;
        case XK_F15:          return KEY_F15;
        case XK_F16:          return KEY_F16;
        case XK_F17:          return KEY_F17;
        case XK_F18:          return KEY_F18;
        case XK_F19:          return KEY_F19;
        case XK_F20:          return KEY_F20;
        case XK_F21:          return KEY_F21;
        case XK_F22:          return KEY_F22;
        case XK_F23:          return KEY_F23;
        case XK_F24:          return KEY_F24;
        case XK_F25:          return KEY_F25;

            // Numeric keypad (should have been detected in secondary keysym!)
        case XK_KP_Divide:    return KEY_KP_DIVIDE;
        case XK_KP_Multiply:  return KEY_KP_MULTIPLY;
        case XK_KP_Subtract:  return KEY_KP_SUBTRACT;
        case XK_KP_Add:       return KEY_KP_ADD;
        case XK_KP_Equal:     return KEY_KP_EQUAL;
        case XK_KP_Enter:     return KEY_KP_ENTER;

            // The rest (should be printable keys)
        default:
            // Make uppercase
            XConvertCase( key, &key_lc, &key_uc );
            key = key_uc;

            // Valid ISO 8859-1 character?
            if( (key >=  32 && key <= 126) ||
                (key >= 160 && key <= 255) )
            {
                return (int) key;
            }
            return KEY_UNKNOWN;
    }
}


//========================================================================
// Show mouse cursor (unlock it)
//========================================================================

//void _glfwPlatformShowMouseCursor( void )
//{
//    // Un-grab cursor (only in windowed mode: in fullscreen mode we still
//    // want the mouse grabbed in order to confine the cursor to the window
//    // area)
//    if( _glfwWin.pointer_grabbed )
//    {
//        XUngrabPointer( _glfwLibrary.display, CurrentTime );
//        _glfwWin.pointer_grabbed = GL_FALSE;
//    }
//
//    // Show cursor
//    if( _glfwWin.pointerHidden )
//    {
//        XUndefineCursor( _glfwLibrary.display, _glfwWin.window );
//        _glfwWin.pointerHidden = GL_FALSE;
//    }
//}

///************************************************************************************************************/
//#https://stackoverflow.com/questions/12706631/x11-change-resolution-and-make-window-fullscreen
//int find_best_video_mode(screen_x11_t *screen_x11, int screen_idx, unsigned int *width, unsigned int *height)
//{
//    int modeCount;
//    XF86VidModeModeInfo** modes;
//
//    if (XF86VidModeGetAllModeLines(screen_x11->display, screen_idx, &modeCount, &modes))
//    {
//        int bestMode  = -1;
//        int bestMatch = INT_MAX;
//        for(int i = 0; i < modeCount; i ++)
//        {
//            int match = (*width  - modes[i]->hdisplay) *
//                        (*width  - modes[i]->hdisplay) +
//                        (*height - modes[i]->vdisplay) *
//                        (*height - modes[i]->vdisplay);
//            if(match < bestMatch)
//            {
//                bestMatch = match;
//                bestMode  = i;
//            }
//        }
//        *width  = modes[bestMode]->hdisplay;
//        *height = modes[bestMode]->vdisplay;
//        XFree(modes);
//        return bestMode;
//    }
//
//    return -1;
//}


/***************** basic method *************************/
// going in fullscreen

//    XEvent xev = { 0 };
//    Atom _NET_WM_STATE = XInternAtom(screen_x11->display, "_NET_WM_STATE", False);
//    Atom _NET_WM_STATE_FULLSCREEN = XInternAtom(screen_x11->display, "_NET_WM_STATE_FULLSCREEN", False);
//    xev.type = ClientMessage;
//    xev.xclient.window = screen_x11->window;
//    xev.xclient.message_type = _NET_WM_STATE;
//    xev.xclient.format = 32;
//    xev.xclient.data.l[0] = screen_settings->fullscreen ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE; // _NET_WM_STATE_ADD
//    xev.xclient.data.l[1] = _NET_WM_STATE_FULLSCREEN;
//    xev.xclient.data.l[2] = 0;
//    XSendEvent(screen_x11->display, screen_x11->root, False,
//               SubstructureNotifyMask | SubstructureRedirectMask, &xev); //
/***************** end basic method *************************/



// -----------------------------------------------------------
// Purpose : simply retrieve current X screen resolution and
//    the size of current root window of the default
//    screen of curent default window
// -----------------------------------------------------------
//int getRootWindowSize(int *w, int *h)
//{
//    Display* pdsp = NULL;
//    Window wid = 0;
//    XWindowAttributes xwAttr;
//
//    pdsp = XOpenDisplay( NULL );
//    if ( !pdsp ) {
//        fprintf(stderr, "Failed to open default display.\n");
//        return -1;
//    }
//
//    wid = DefaultRootWindow( pdsp );
//    if ( 0 > wid ) {
//        fprintf(stderr, "Failed to obtain the root windows Id "
//                        "of the default screen of given display.\n");
//        return -2;
//    }
//
//    Status ret = XGetWindowAttributes( pdsp, wid, &xwAttr );
//    *w = xwAttr.width;
//    *h = xwAttr.height;
//
//    XCloseDisplay( pdsp );
//    return 0;
//}
//
//int getScreenSize(int *w, int*h)
//{
//
//    Display* pdsp = NULL;
//    Screen* pscr = NULL;
//
//    pdsp = XOpenDisplay( NULL );
//    if ( !pdsp ) {
//        fprintf(stderr, "Failed to open default display.\n");
//        return -1;
//    }
//
//    pscr = DefaultScreenOfDisplay( pdsp );
//    if ( !pscr ) {
//        fprintf(stderr, "Failed to obtain the default screen of given display.\n");
//        return -2;
//    }
//
//    *w = pscr->width;
//    *h = pscr->height;
//
//    XCloseDisplay( pdsp );
//    return 0;
//}


//Screen *pscr = DefaultScreenOfDisplay(screen->display);
//printf("\tScreen %d: %dX%d\n", 1, pscr->width, pscr->height);

// get window size
//    XWindowAttributes xwAttr;
//    Status ret = XGetWindowAttributes( screen->display, default_root_window, &xwAttr );
//    printf("\tScreen %d: %dX%d\n", 1, xwAttr.width, xwAttr.height);

// init font for drawing stats. REMOVE?
//    screen->font_info = XLoadQueryFont(screen->display, FONT_NAME);
//    if (!screen->font_info) {
//        fprintf(stderr, "XLoadQueryFont: failed loading font '%s'\n", FONT_NAME);
//        exit(EXIT_FAILURE);
//    }
//    XSetFont(screen->display, screen->gc, screen->font_info->fid);

