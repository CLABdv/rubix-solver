#include "viz_col.h"
#include "sig_shared.h"
#include <SDL2/SDL.h>
#include <signal.h>
#include <stdio.h>

void *thread_wrap_show_col(void *_)
{
    (void)_;
    show_col();
    return NULL;
}

void show_col()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("%s\n", SDL_GetError());
    }
    SDL_Window *window =
        SDL_CreateWindow("colour viz", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H, 0);
    if(!window)
    {
        printf("%s\n", SDL_GetError());
        SDL_Quit();
        return;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer)
    {
        printf("%s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }
    SDL_Texture *tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, WIN_H, WIN_W);
    if(!tex)
    {
        printf("%s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    int *pix_tmp = NULL;
    int pitch = 4 * WIN_W;
    SDL_Event event;

    while (keep_running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                raise(SIGINT);
        }
        SDL_LockTexture(tex, NULL, (void **)&pix_tmp, &pitch);
        for (int i = 0; i < WIN_W * WIN_H; i++)
        {
            pix_tmp[i] = col_shared << 8;
        }
        SDL_UnlockTexture(tex);
        SDL_RenderCopy(renderer, tex, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
