#include "image_loader.h"
#include <stdlib.h>
#include "../app/app.h"

#define STBI_ONLY_JPEG
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_lib/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_lib/stb_image_write.h"

#define IMAGE_FILE_IN "images/img0.jpg"
#define IMAGE_FILE_OUT "images/out.png"

typedef struct {
    app_t *app;
    int w, h;
    int n; // # channels
    uint8_t *image;
} image_loader_t;

static image_loader_t *image_loader;

static void convert_bgr_to_rgb(uint8_t *data, int x, int y, int i);

static void image_loader_init() {

    screen_settings_t *screen_settings = malloc(sizeof(screen_settings));
    screen_settings->window_title = "Image Loader";
    screen_settings->width = 1024;
    screen_settings->height = 768;
    screen_settings->targetFps = 60;
    screen_settings->show_stats = true;
    screen_settings->fullscreen = false;

    image_loader = malloc(sizeof(image_loader_t));
    memset(image_loader, 0, sizeof(image_loader_t));

    image_loader->app = app_init(screen_settings);

    int x,y,n;
    uint8_t *data = stbi_load(IMAGE_FILE_IN, &x, &y, &n, 4); // force 4 channels
    if (data == NULL) {
        printf("Error loading image %s\nExiting...", IMAGE_FILE_IN);
        exit(1);
    }
    convert_bgr_to_rgb(data, x, y, 4);
    image_loader->w = x;
    image_loader->h = y;
    image_loader->image = data;
    image_loader->n = 4;
}

static void convert_bgr_to_rgb(uint8_t *p, int x, int y, int n) {
    uint32_t i, pixel_count = x * y;
    for (i=0; i < pixel_count; ++i) {
        uint8_t t = p[0];
        p[0] = p[2];
        p[2] = t;
        p += n;
    }
}

static void update(int64_t elapsed_time) {
}

static void draw(void) {
//    app_clear_screen(image_loader->app);

//    screen_info_t *screen_info = app_get_screen_info(image_loader->app);
//    uint32_t screen_width = app_get_width(image_loader->app);
//    memset(screen_info->pbuffer, 0, // 0x00
//           screen_width * app_get_height(image_loader->app) * screen_info->bytes_per_pixel);

//    int buffer_stride = screen_width * screen_info->bytes_per_pixel;
//    int image_stride = image_loader->w * image_loader->n;
//    for (int y=0; y != image_loader->h; ++y) {
//        uint8_t *image_row = image_loader->image + y * image_stride;
//        uint32_t *buffer_row = (uint32_t *) (screen_info->pbuffer + y * buffer_stride);
//        for (int x = 0; x != image_loader->w; ++x) {
////            int red = *image_row++;
////            int green = *image_row++;
////            int blue = *image_row++;
////            image_row++; // 4
////            uint32_t color = ext_to_native(screen_info, red, green, blue);
////            *buffer_row++ = color;
//             // ASSUMES RGBA:
////              uint32_t color = *(uint32_t*)image_row;
////              *buffer_row++ = color;
////              image_row +=4;
//        }
//    }
////        memcpy(,
////               image_loader->image + y * image_stride,
////               image_stride);
//    }

    screen_info_t *screen_info = app_get_screen_info(image_loader->app);
    uint32_t screen_width = app_get_width(image_loader->app);

    uint8_t *p_buffer = screen_info->pbuffer;
    uint8_t *p_image = image_loader->image;
    int buffer_stride = screen_width * screen_info->bytes_per_pixel;
    int image_stride = image_loader->w * image_loader->n; // n == bpp
    for (int y=0; y != image_loader->h; ++y) {
        memcpy(p_buffer, p_image, image_stride);
        p_buffer += buffer_stride;
        p_image += image_stride;
    }

    app_show_fps(image_loader->app, 1, 3);
}

static void key_event(int key, int action) {
    if (action == KEY_PRESS) {
        switch (key) {
            case KEY_ESC: {
                app_stop(image_loader->app);
                break;
            }
            case KEY_F1: {
                app_toggle_fullscreen(image_loader->app);
                break;
            }
        }
    }
}

static void finish(void) {
    // back to bgr...
    convert_bgr_to_rgb(image_loader->image, image_loader->w, image_loader->h, image_loader->n);
    if (!stbi_write_png(IMAGE_FILE_OUT, image_loader->w, image_loader->h, image_loader->n, image_loader->image,
            image_loader->w*image_loader->n)) {
        printf("Error writing image %s\nExiting...", IMAGE_FILE_OUT);
        exit(1);
    }
    printf("File %s written.", IMAGE_FILE_OUT);
}

int image_loader_run() {
    printf("Launching Image loader...\n");
    image_loader_init();

    app_callbacks_t *cbs = malloc(sizeof(app_callbacks_t));
    cbs->key_fun = key_event;
    cbs->update = update;
    cbs->draw = draw;
    cbs->finish = finish;
    cbs->print_final_stats = NULL;
    app_run(image_loader->app, cbs);
    return 0;
}
