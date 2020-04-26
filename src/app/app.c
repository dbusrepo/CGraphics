#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define __USE_POSIX199309
#include <time.h>
#include <inttypes.h>
#include <stdbool.h>

#include "../graphics_utils/common.h"
#include "../graphics_utils/screen_info_rgb.h"
#include "../graphics_utils/font.h"
#include "screen.h"
#include "app.h"

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

struct app {
    screen_t *screen;
    screen_settings_t *screen_settings;
    uint32_t xsize;
    uint32_t ysize;
    // screen x11 ???
    screen_info_rgb_t *screen_info_rgb;
    int64_t period; // period between drawing // all times are in _nanosecs_
    int64_t start_time;
    stats_t stats;
    bool is_running;
    // TODO:
    update_fptr_t update_fptr; // refactor name?
    render_fptr_t render_fptr;
    key_event_fptr_t key_event_fptr;
};

static inline void init_stats(app_t *app);
static inline int64_t nano_time(void);
static void update(app_t *app, int64_t elapsed_time);
static void key_event(app_t *app, int key_code);
static void clear_screen(app_t *app);
static void render(app_t *app);
static void updateStats(app_t *app);
static void print_final_stats(app_t *app);
static void print_rendering_info(app_t *app);

app_t *init_app(screen_settings_t *screen_settings) {
    app_t *app = malloc(sizeof(app_t));

    app->screen_settings = screen_settings;
    app->screen = init_screen(screen_settings);
    app->screen_info_rgb = get_screen_info(app->screen);
    app->xsize = screen_settings->xsize;
    app->ysize = screen_settings->ysize;
    app->period = NANO_IN_SEC / screen_settings->targetFps;
    app->is_running = false;
    app->start_time = 0;
    app->update_fptr = NULL; // refactor name?
    app->render_fptr = NULL;
    app->key_event_fptr = NULL;
    init_stats(app);

    font_init(app->screen_info_rgb);
    set_text_color(app->screen_info_rgb, 255, 0, 0);

    return app;
}

void run_event_loop(app_t *app,
                    update_fptr_t update_fptr,
                    render_fptr_t render_fptr,
                    key_event_fptr_t key_event_fptr) {

    int64_t over_sleep_time = 0;
    int64_t excess = 0;
    int64_t period = app->period; // save it here

    app->update_fptr = update_fptr;
    app->render_fptr = render_fptr;
    app->key_event_fptr = key_event_fptr;

    int64_t before_time = nano_time();
    app->start_time = before_time;
    app->stats.prev_time = before_time;
    app->is_running = true;

    while (app->is_running) {

        update(app, 0);
        render(app); // TODO

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
            update(app, 0);
            ++skips;
        }
        app->stats.frames_skipped += skips;

        updateStats(app);
    }

    print_final_stats(app);
//    close_screen(app);
    exit(EXIT_SUCCESS);
}

/***************************************************************************************/
// PRIVATE FUNCTIONS

static void update(app_t *app, int64_t elapsed_time) {
    check_event(app->screen, app, key_event);
    app->update_fptr(elapsed_time);
}

static void key_event(app_t *app, int key_code) {
    switch(key_code) {
        case 'q': {
            app->is_running = false;
        }
    }
    app->key_event_fptr(key_code);
}

static inline void init_stats(app_t *app) {
    // https://stackoverflow.com/questions/6891720/initialize-reset-struct-to-zero-null
    static const stats_t empty_stats = { 0 };
    app->stats = empty_stats;
}

static void render(app_t *app) {
    clear_screen(app);
    app->render_fptr();
    print_rendering_info(app);
    blit(app->screen);
}

static void clear_screen(app_t *app) {
    memset(app->screen_info_rgb->buffer, 0x00,
            (size_t)app->xsize * app->ysize * app->screen_info_rgb->bytes_per_pixel);
}

static void print_rendering_info(app_t *app) {
    if (app->screen_settings->show_rendering_info) {
#define BUFFER_SIZE 256
        static char buf[BUFFER_SIZE];
        snprintf(buf, BUFFER_SIZE, "FPS UPS %.0f %.0f",
                 app->stats.average_fps,
                 app->stats.average_ups);
        draw_text(app->screen_info_rgb, app->xsize, app->ysize, 1, 3, buf);
    }
}

static void updateStats(app_t *app) {
    stats_t *stats = &app->stats; // save this
    ++stats->frame_counter;
    stats->interval += app->period;
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

static void print_final_stats(app_t *app) {
    stats_t *stats = &app->stats;
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

static inline int64_t nano_time() {
    struct timespec time;
//    https://stackoverflow.com/questions/3523442/difference-between-clock-realtime-and-clock-monotonic
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    return time.tv_sec * NANO_IN_SEC + time.tv_nsec;
}

