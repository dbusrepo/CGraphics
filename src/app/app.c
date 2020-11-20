#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include "../graphics_utils/common.h"
#include "../graphics_utils/font.h"
#include "../graphics_utils/screen_callbacks.h"
#include "screen.h"
#include "app.h"

// number of FPS values stored to get an rgb_average
#define NUM_AVG_FPS 20
// no. of frames that can be skipped in any one animation loop MAX_EXEC_SECONDS.e the state is updated but not rendered
#define MAX_FRAME_SKIPS 5
//// Number of frames with a delay of 0 ms before the animation thread yields to other running threads.
//#define NUM_DELAYS_PER_YIELD 16
// period between stats updates // 1 sec
#define UPDATE_STATS_INTERVAL NANO_IN_SEC

static inline int64_t nano_time(void)
{
	struct timespec time;
//    https://stackoverflow.com/questions/3523442/difference-between-clock-realtime-and-clock-monotonic
	clock_gettime(CLOCK_MONOTONIC_RAW, &time);
	return time.tv_sec * NANO_IN_SEC + time.tv_nsec;
}

typedef struct
{
	int64_t start_time;
	int64_t prev_time;
	int64_t frame_counter;
	int64_t frame_skips;
	int64_t total_frame_skips;
	int64_t interval; // interval in ns between stats update
	int64_t total_elapsed_time;
//    int64_t total_time_spent; // this is in seconds
	int64_t count; // fps/ups value index
	double fps_values[NUM_AVG_FPS];
	double ups_values[NUM_AVG_FPS];
	double average_fps;
	double average_ups;
} stats_t;

struct app
{
	screen_t *screen;
//    screen_settings_t *screen_settings;
	uint32_t width;
	uint32_t height;
	bool show_fps;
	// screen_idx x11 ???
	screen_info_t *screen_info_rgb;
	int64_t period; // period between drawing // all times are in _nanosecs_
	stats_t stats;
	bool is_running;

	app_callbacks_t cbs;
};

static void init_stats(app_t *app);

static void update(app_t *app, int64_t elapsed_time);

static void key_event(app_t *app, int key_code);

static void render(app_t *app);

static void updateStats(app_t *app);

static void print_final_stats(app_t *app);

static void finish(app_t *app);

static void init_callbacks(app_t *app, app_callbacks_t *cbs);

app_t* app_init(screen_settings_t *screen_settings)
{
	app_t *app = malloc(sizeof(app_t));
	memset(app, 0, sizeof(app_t));

//    app->screen_settings = screen_settings;
	app->screen = screen_init(screen_settings);
	app->screen_info_rgb = screen_get_info(app->screen);
	app->width = screen_settings->width;
	app->height = screen_settings->height;
	app->show_fps = screen_settings->show_stats;
	app->period = NANO_IN_SEC / screen_settings->targetFps;
	app->is_running = false;
	init_stats(app);
	font_init(app->screen_info_rgb);
	set_text_color(app->screen_info_rgb, 255, 0, 0);
	return app;
}

screen_info_t* app_get_screen_info(app_t *app)
{
	return app->screen_info_rgb;
}

uint32_t app_get_width(app_t *app)
{
	return app->width;
}

uint32_t app_get_height(app_t *app)
{
	return app->height;
}

void app_run(app_t *app, app_callbacks_t *cbs)
{
	int64_t over_sleep_time = 0;
	int64_t excess = 0;
	int64_t period = app->period; // save it here

	// save the callbacks
	init_callbacks(app, cbs);
	screen_set_key_callback(app->screen, app->cbs.key_fun);
	int64_t before_time = nano_time();
	app->stats.start_time = before_time;
	app->stats.prev_time = before_time;
	app->is_running = true;
	while (app->is_running) {
		update(app, 0);
		render(app); // TODO

		int64_t after_time = nano_time();
		int64_t time_diff = after_time - before_time;
		int64_t sleep_time = (period - time_diff) - over_sleep_time;
		if (sleep_time > 0) {
			struct timespec sleep_time_s = { 0, sleep_time };
			nanosleep(&sleep_time_s, NULL);
			over_sleep_time = (nano_time() - after_time) - sleep_time;
		}
		else { // sleepTime <= 0; the draw took longer than the period
			   // print rendering is slowing down?
			excess -= sleep_time; // store excess prev_time value
			over_sleep_time = 0;
		}
		before_time = nano_time();

		/* If draw animation is taking too long, update the state
		 without rendering it, to get the updates/sec nearer to
		 the required FPS. */
		int skips = 0;
		while ((excess > period) && (skips < MAX_FRAME_SKIPS)) {
			// app_update state but don’t draw
			excess -= period;
			update(app, 0);
			++skips;
		}
		app->stats.frame_skips += skips;
		updateStats(app);
	}
	screen_terminate(app->screen);
	print_final_stats(app);
	finish(app);
//    exit(EXIT_SUCCESS);
}

void app_stop(app_t *app)
{
	app->is_running = false;
}

void app_toggle_fullscreen(app_t *app)
{
	screen_toggle_fullscreen(app->screen);
}

void app_clear_screen(app_t *app)
{
	memset(app->screen_info_rgb->pbuffer, 0,
			app->width * app->height * app->screen_info_rgb->bytes_per_pixel);
}

void app_show_fps(app_t *app, int x, int y)
{
	if (app->show_fps) {
		static char buf[128];
		snprintf(buf, 128, "FPS UPS %.0f %.0f", app->stats.average_fps,
				app->stats.average_ups);
		draw_text(app->screen_info_rgb, app->width, app->height, x, y, buf);
	}
}

/***************************************************************************************/
// PRIVATE FUNCTIONS
static void finish(app_t *app)
{
	app->cbs.finish();
	screen_finish(&app->screen);
	free(app);
}

static void update(app_t *app, int64_t elapsed_time)
{
	screen_poll_events(app->screen);
	app->cbs.update(elapsed_time);
}

static void init_stats(app_t *app)
{
	// https://stackoverflow.com/questions/6891720/initialize-reset-struct-to-zero-null
	static const stats_t empty_stats = { 0 };
	app->stats = empty_stats;
}

static void render(app_t *app)
{
	app->cbs.draw();
//    app_show_fps(app);
	screen_blit(app->screen);
}

static void updateStats(app_t *app)
{
	stats_t *stats = &app->stats; // save this
	++stats->frame_counter;
	stats->interval += app->period;
	if (stats->interval >= UPDATE_STATS_INTERVAL) {
		int64_t time_now = nano_time();
		int64_t real_elapsed_time = time_now - stats->prev_time;
//        double timing_error = ((double) (real_elapsed_time - stats->interval) / stats->interval) * 100.0; // TODO
		stats->total_elapsed_time += real_elapsed_time;
		stats->total_frame_skips += stats->frame_skips;
		// calculate the latest FPS and UPS
		double actual_fps = 0.0;
		double actual_ups = 0.0;
		if (stats->total_elapsed_time > 0) {
			actual_fps = (((double) stats->frame_counter
					/ stats->total_elapsed_time) * NANO_IN_SEC);
			actual_ups = (((double) (stats->frame_counter
					+ stats->total_frame_skips) / stats->total_elapsed_time) *
			NANO_IN_SEC);
		}
		stats->fps_values[stats->count % NUM_AVG_FPS] = actual_fps;
		stats->ups_values[stats->count % NUM_AVG_FPS] = actual_ups;
		++stats->count;
		// obtain the rgb_average fps and ups
		double total_fps = 0.0;
		double total_ups = 0.0;
		for (int i = 0; i != NUM_AVG_FPS; i++) {
			total_fps += stats->fps_values[i];
			total_ups += stats->ups_values[i];
		}
		int64_t num_values =
				(stats->count >= NUM_AVG_FPS) ? NUM_AVG_FPS : stats->count;
		stats->average_fps = total_fps / num_values;
		stats->average_ups = total_ups / num_values;
		// print stats? // use timing error and other info??
		// ...
		// reset some values
		stats->frame_skips = 0;
		stats->interval = 0;
		stats->prev_time = time_now;
	}
}

static void print_final_stats(app_t *app)
{
	stats_t *stats = &app->stats;
	printf("\n\n***Final stats:***\n"
			"Frame Count/Loss: %" PRId64 "/" "%"PRId64 "\n"
	"Average FPS: %.2f\n"
	"Average UPS: %.2f\n"
	"Time spent: %" PRId64 "s"
	"\n\n", stats->frame_counter, stats->total_frame_skips, stats->average_fps,
			stats->average_ups, stats->total_elapsed_time / NANO_IN_SEC);
	app->cbs.print_final_stats();
//    https://stackoverflow.com/questions/12450066/flushing-buffers-in-c
//    fflush(stdout);
}

// default empty functions
static void default_key_event(int key, int action)
{
}

static void default_update(int64_t elapsed_time)
{
}

static void default_draw(void)
{
}

static void default_finish(void)
{
}

static void default_print_final_stats()
{
}

static void init_callbacks(app_t *app, app_callbacks_t *cbs)
{
	app->cbs.key_fun = cbs->key_fun != NULL ? cbs->key_fun : default_key_event;
	app->cbs.update = cbs->update != NULL ? cbs->update : default_update;
	app->cbs.draw = cbs->draw != NULL ? cbs->draw : default_draw;
	app->cbs.finish = cbs->finish != NULL ? cbs->finish : default_finish;
	app->cbs.print_final_stats =
			cbs->print_final_stats != NULL ?
					cbs->print_final_stats : default_draw;
}

