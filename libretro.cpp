#include <unistd.h>
#include <SDL_libretro.h>
#include <libco.h>
#include <file/file_path.h>

#define main osa_main
#define SDL_Flip(screen) SDL_libretro_co_yield()
#define SDL_Delay(i)
#define WaitKey _WaitKey

#include <OpenSyobonAction/main.cpp>

retro_audio_sample_batch_t SDL_libretro_audio_batch_cb;
retro_input_state_t SDL_libretro_input_state_cb;

static void fallback_log(enum retro_log_level level, const char *fmt, ...);
static retro_video_refresh_t video_cb;
static retro_input_poll_t input_poll_cb;

static retro_log_printf_t log_cb = fallback_log;
static retro_environment_t environ_cb;
static cothread_t retro_ct, game_ct;

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
    (void)level;
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
}

byte _WaitKey()
{
    SDL_Event event;
    while (true) {
        SDL_libretro_co_yield();
	while (SDL_PollEvent(&event)) {
	    if (event.type == SDL_KEYDOWN)
		return event.key.keysym.sym;
        }
    }
}

void SDL_libretro_co_yield(void)
{
    co_switch(retro_ct);
}

void SDL_libretro_video_refresh()
{
    SDL_Surface *screen = SDL_GetVideoSurface();
    if (screen)
        video_cb(screen->pixels, screen->w, screen->h, screen->pitch);
}

unsigned retro_api_version(void)
{
    return RETRO_API_VERSION;
}

void retro_set_environment(retro_environment_t cb)
{
    struct retro_log_callback log;
    bool t = true;
    environ_cb = cb;
    if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
        log_cb = log.log;

    environ_cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &t);
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
    video_cb = cb;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
    SDL_libretro_audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
    input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
    SDL_libretro_input_state_cb = cb;
}

void retro_get_system_info(struct retro_system_info *info)
{
    info->need_fullpath = false;
    info->valid_extensions = "";
    info->library_version = "0.2";
    info->library_name = "syobonaction";
    info->block_extract = false;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
    int width = 480;
    int height = 420;
    info->geometry.base_width = width;
    info->geometry.base_height = height;
    info->geometry.max_width = width;
    info->geometry.max_height = height;
    info->timing.fps = 30.0;
    info->timing.sample_rate = 22050;
}

void retro_init(void)
{
    enum retro_pixel_format pixfmt = RETRO_PIXEL_FORMAT_XRGB8888;
    environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixfmt);
    retro_ct = co_active();
}

static void game_main(void)
{
    osa_main(0, NULL);
    co_switch(retro_ct);
}

bool retro_load_game(const struct retro_game_info *game)
{
    char game_dir[512] = { 0 };
    const char *system_dir = NULL;
    environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_dir);

    fill_pathname_join_special(game_dir, system_dir, "SyobonAction", 512);
    if (!path_is_directory(game_dir) || chdir(game_dir) == -1) {
        log_cb(RETRO_LOG_ERROR, "Missing game assets directory: %s\n", game_dir);
        return false;
    }
    game_ct = co_create(1024*1024, game_main);
    return true;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
}

void retro_deinit(void)
{
}

void retro_reset(void)
{
}

void retro_run(void)
{
    co_switch(game_ct);
    input_poll_cb();
    SDL_libretro_video_refresh();
}

size_t retro_serialize_size(void)
{
    return 0;
}

bool retro_serialize(void *data, size_t size)
{
    return false;
}

bool retro_unserialize(const void *data, size_t size)
{
    return false;
}

void retro_cheat_reset(void) {}
void retro_cheat_set(unsigned index, bool enabled, const char *code) {}

bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info)
{
    return false;
}

void retro_unload_game(void)
{
    maint = 3;
    co_switch(game_ct);
    deinit();
    co_delete(game_ct);
}

unsigned retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

void *retro_get_memory_data(unsigned id)
{
    return 0;
}

size_t retro_get_memory_size(unsigned id)
{
    return 0;
}
