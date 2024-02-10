#include <SDL_libretro.h>
#include <sdl/src/events/SDL_events_c.h>
#include <sdl/src/video/dummy/SDL_nullevents_c.h>


static void PumpKeyboardEvents(void)
{
    static int16_t keys[RETROK_LAST] = {0};
    static SDL_keysym sym;
    for (int i = 0; i < RETROK_LAST; ++i) {
        int16_t state = SDL_libretro_input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0, i);
        if (keys[i] != state) {
            keys[i] = state;
            sym.scancode = i;
            sym.sym = i;
            SDL_PrivateKeyboard(state ? SDL_PRESSED : SDL_RELEASED, &sym);
        }
    }

    static int16_t buttons[16] = {0};
    static const int bkeys[16] = {
        [RETRO_DEVICE_ID_JOYPAD_UP]     = SDLK_UP,
        [RETRO_DEVICE_ID_JOYPAD_DOWN]   = SDLK_DOWN,
        [RETRO_DEVICE_ID_JOYPAD_LEFT]   = SDLK_LEFT,
        [RETRO_DEVICE_ID_JOYPAD_RIGHT]  = SDLK_RIGHT,
        [RETRO_DEVICE_ID_JOYPAD_A]      = SDLK_z,
        [RETRO_DEVICE_ID_JOYPAD_B]      = SDLK_SPACE,
        [RETRO_DEVICE_ID_JOYPAD_X]      = SDLK_o,
        [RETRO_DEVICE_ID_JOYPAD_Y]      = SDLK_RETURN,
        [RETRO_DEVICE_ID_JOYPAD_SELECT] = SDLK_F1,
        [RETRO_DEVICE_ID_JOYPAD_START]  = SDLK_0,
        [RETRO_DEVICE_ID_JOYPAD_L]      = SDLK_s,
        [RETRO_DEVICE_ID_JOYPAD_R]      = SDLK_1,
        [RETRO_DEVICE_ID_JOYPAD_L2]     = SDLK_2,
        [RETRO_DEVICE_ID_JOYPAD_R2]     = SDLK_3,
        [RETRO_DEVICE_ID_JOYPAD_L3]     = SDLK_4,
        [RETRO_DEVICE_ID_JOYPAD_R3]     = SDLK_5,
    };
    for (int i = 0; i < 16; ++i) {
        int16_t state = SDL_libretro_input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i);
        int k = bkeys[i];
        if (k && buttons[i] != state) {
            buttons[i] = state;
            sym.scancode = k;
            sym.sym = k;
            SDL_PrivateKeyboard(state ? SDL_PRESSED : SDL_RELEASED, &sym);
        }
    }
}

void DUMMY_PumpEvents(_THIS)
{
    PumpKeyboardEvents();
}

void DUMMY_InitOSKeymap(_THIS)
{
}
