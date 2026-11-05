#include <SDL2/SDL.h>
#include <psp2/kernel/processmgr.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        sceKernelExitProcess(1);
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Neon Brawl - PS Vita",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        960, 544,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        SDL_Quit();
        sceKernelExitProcess(1);
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED
    );

    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        sceKernelExitProcess(1);
        return 1;
    }

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 12, 10, 30, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 220, 255, 255);
        SDL_Rect titleBar = { 120, 150, 720, 8 };
        SDL_RenderFillRect(renderer, &titleBar);

        SDL_SetRenderDrawColor(renderer, 255, 40, 150, 255);
        SDL_Rect accent = { 120, 386, 720, 8 };
        SDL_RenderFillRect(renderer, &accent);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    sceKernelExitProcess(0);
    return 0;
}
