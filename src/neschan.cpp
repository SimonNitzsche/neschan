// neschan.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <vector>

#include "neschan.h"
#include <nes_system.h>
#include <nes_ppu.h>
#include <trace.h>

#include <sdl.h>

using namespace std;

uint32_t make_argb(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 16) | (g << 8) | b;
}

int WINAPI wWinMain(
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    PWSTR       lpCmdLine,
    int         nCmdShow)
{
    //The window we'll be rendering to
    SDL_Window* window = NULL;

    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        return -1;
    }

    //Create window
    SDL_Window *sdlWindow;
    SDL_Renderer *sdlRenderer;
    SDL_CreateWindowAndRenderer(PPU_SCREEN_X * 2, PPU_SCREEN_Y * 2, SDL_WINDOW_SHOWN, &sdlWindow, &sdlRenderer);

    SDL_SetWindowTitle(sdlWindow, "NESChan v0.1 by yizhang82");

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
    SDL_RenderSetLogicalSize(sdlRenderer, PPU_SCREEN_X, PPU_SCREEN_Y);

    SDL_Texture *sdlTexture = SDL_CreateTexture(sdlRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        PPU_SCREEN_X, PPU_SCREEN_Y);

    INIT_TRACE("neschan.log");

    nes_system system;

    system.power_on();

    system.load_rom("../test/roms/color_test/color_test.nes", nes_rom_exec_mode_reset);

    vector<uint32_t> pixels(PPU_SCREEN_Y * PPU_SCREEN_X);

    static uint32_t palette[] = {
        make_argb(84,  84,  84),    make_argb(0,  30, 116),    make_argb(8,  16, 144),    make_argb(48,   0, 136),   make_argb(68,   0, 100),   make_argb(92,   0,  48),   make_argb(84,   4,   0),   make_argb(60,  24,   0),   make_argb(32,  42,   0),   make_argb(8,  58,   0),   make_argb(0,  64,   0),    make_argb(0,  60,   0),    make_argb(0,  50,  60),    make_argb(0,   0,   0),   make_argb(0, 0, 0), make_argb(0, 0, 0),
        make_argb(152, 150, 152),   make_argb(8,  76, 196),    make_argb(48,  50, 236),   make_argb(92,  30, 228),   make_argb(136,  20, 176),  make_argb(160,  20, 100),  make_argb(152,  34,  32),  make_argb(120,  60,   0),  make_argb(84,  90,   0),   make_argb(40, 114,   0),  make_argb(8, 124,   0),    make_argb(0, 118,  40),    make_argb(0, 102, 120),    make_argb(0,   0,   0),   make_argb(0, 0, 0), make_argb(0, 0, 0),
        make_argb(236, 238, 236),   make_argb(76, 154, 236),   make_argb(120, 124, 236),  make_argb(176,  98, 236),  make_argb(228,  84, 236),  make_argb(236,  88, 180),  make_argb(236, 106, 100),  make_argb(212, 136,  32),  make_argb(160, 170,   0),  make_argb(116, 196,   0), make_argb(76, 208,  32),   make_argb(56, 204, 108),   make_argb(56, 180, 204),   make_argb(60,  60,  60),  make_argb(0, 0, 0), make_argb(0, 0, 0),
        make_argb(236, 238, 236),   make_argb(168, 204, 236),  make_argb(188, 188, 236),  make_argb(212, 178, 236),  make_argb(236, 174, 236),  make_argb(236, 174, 212),  make_argb(236, 180, 176),  make_argb(228, 196, 144),  make_argb(204, 210, 120),  make_argb(180, 222, 120), make_argb(168, 226, 144),  make_argb(152, 226, 180),  make_argb(160, 214, 228),  make_argb(160, 162, 160), make_argb(0, 0, 0), make_argb(0, 0, 0)
    };
    assert(sizeof(palette) == sizeof(uint32_t) * 0x40);

    int cpu_clock = 21477272;
    int tick = 21477272 / 60;

    SDL_Event sdlEvent;

    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&sdlEvent) != 0)
        {
            if (sdlEvent.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        for (int i = 0; i < tick; ++i)
            system.step(nes_cycle_t(1));

        uint32_t *cur_pixel = pixels.data();
        uint8_t *frame_buffer = system.ppu()->frame_buffer();
        for (int y = 0; y < PPU_SCREEN_Y; ++y)
        {
            for (int x = 0; x < PPU_SCREEN_X; ++x)
            {
                *cur_pixel = palette[(*frame_buffer & 0xff)];
                frame_buffer++;
                cur_pixel++;
            }
        }

        SDL_UpdateTexture(sdlTexture, NULL, pixels.data(), PPU_SCREEN_X * sizeof (uint32_t));

        SDL_RenderClear(sdlRenderer);
        SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(sdlRenderer);
    }

    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}
