#include <SDL2/SDL.h>
#include <psp2/kernel/processmgr.h>
#include <math.h>

#define W 960
#define H 544

typedef struct {
    float x, y;
    int hp;
    int facing;
    float attack;
} Fighter;

static void draw_fighter(SDL_Renderer *r, Fighter *f, int player) {
    SDL_Rect shadow = {(int)f->x-17, (int)f->y+20, 34, 8};
    SDL_SetRenderDrawColor(r, 0, 0, 0, 110);
    SDL_RenderFillRect(r, &shadow);

    SDL_Rect body = {(int)f->x-13, (int)f->y-22, 26, 38};
    if (player) SDL_SetRenderDrawColor(r, 35, 190, 175, 255);
    else SDL_SetRenderDrawColor(r, 210, 65, 100, 255);
    SDL_RenderFillRect(r, &body);

    SDL_Rect head = {(int)f->x-11, (int)f->y-43, 22, 21};
    SDL_SetRenderDrawColor(r, 235, 190, 145, 255);
    SDL_RenderFillRect(r, &head);

    SDL_Rect hair = {(int)f->x-12, (int)f->y-46, 24, 8};
    SDL_SetRenderDrawColor(r, player ? 70 : 35, 35, 55, 255);
    SDL_RenderFillRect(r, &hair);

    SDL_Rect legs = {(int)f->x-12, (int)f->y+16, 10, 13};
    SDL_SetRenderDrawColor(r, 225, 225, 235, 255);
    SDL_RenderFillRect(r, &legs);
    legs.x += 14;
    SDL_RenderFillRect(r, &legs);

    if (f->attack > 0) {
        SDL_Rect fist = {(int)(f->x + f->facing*18), (int)f->y-17, 18, 12};
        SDL_SetRenderDrawColor(r, 255, 225, 80, 255);
        SDL_RenderFillRect(r, &fist);
    }
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        sceKernelExitProcess(1);
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Neon Brawl - PS Vita",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W, H, SDL_WINDOW_SHOWN);
    if (!window) { SDL_Quit(); sceKernelExitProcess(1); return 1; }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window); SDL_Quit(); sceKernelExitProcess(1); return 1;
    }

    SDL_GameController *pad = NULL;
    if (SDL_NumJoysticks() > 0 && SDL_IsGameController(0))
        pad = SDL_GameControllerOpen(0);

    Fighter player = {180, 350, 100, 1, 0};
    Fighter enemy = {650, 350, 50, -1, 0};
    int running = 1;
    Uint32 previous = SDL_GetTicks();
    SDL_Event event;

    while (running) {
        Uint32 now = SDL_GetTicks();
        float dt = (now - previous) / 1000.0f;
        previous = now;
        if (dt > 0.05f) dt = 0.05f;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_CONTROLLERBUTTONDOWN &&
                (event.cbutton.button == SDL_CONTROLLER_BUTTON_B ||
                 event.cbutton.button == SDL_CONTROLLER_BUTTON_X)) {
                player.attack = 0.18f;
                if (fabsf(enemy.x-player.x) < 95 && fabsf(enemy.y-player.y) < 42) {
                    enemy.hp -= 10;
                    if (enemy.hp <= 0) { enemy.hp = 50; enemy.x = 700; }
                }
            }
        }

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        float mx = 0, my = 0;
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) mx -= 1;
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) mx += 1;
        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) my -= 1;
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) my += 1;
        if (pad) {
            Sint16 ax = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX);
            Sint16 ay = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY);
            if (abs(ax) > 7000) mx = ax / 32767.0f;
            if (abs(ay) > 7000) my = ay / 32767.0f;
        }
        if (mx != 0) player.facing = mx > 0 ? 1 : -1;
        player.x += mx * 190.0f * dt;
        player.y += my * 150.0f * dt;
        if (player.x < 25) player.x = 25;
        if (player.x > W-25) player.x = W-25;
        if (player.y < 260) player.y = 260;
        if (player.y > 465) player.y = 465;
        if (player.attack > 0) player.attack -= dt;

        if (enemy.x > player.x + 55) enemy.x -= 45.0f * dt;
        else if (enemy.x < player.x - 55) enemy.x += 45.0f * dt;
        enemy.y += (player.y-enemy.y) * 0.35f * dt;
        enemy.facing = enemy.x > player.x ? -1 : 1;

        SDL_SetRenderDrawColor(renderer, 15, 12, 32, 255);
        SDL_RenderClear(renderer);
        SDL_Rect skyline = {0, 90, W, 175};
        SDL_SetRenderDrawColor(renderer, 30, 24, 58, 255);
        SDL_RenderFillRect(renderer, &skyline);
        for (int i=0; i<12; i++) {
            SDL_Rect building = {i*86, 110+(i%3)*22, 62, 150-(i%3)*22};
            SDL_SetRenderDrawColor(renderer, 42, 32, 76, 255);
            SDL_RenderFillRect(renderer, &building);
        }
        SDL_Rect floor = {0, 260, W, 284};
        SDL_SetRenderDrawColor(renderer, 24, 22, 38, 255);
        SDL_RenderFillRect(renderer, &floor);
        SDL_Rect line = {0, 480, W, 3};
        SDL_SetRenderDrawColor(renderer, 0, 210, 240, 255);
        SDL_RenderFillRect(renderer, &line);

        draw_fighter(renderer, &enemy, 0);
        draw_fighter(renderer, &player, 1);

        SDL_Rect hpback = {30, 25, 220, 14};
        SDL_SetRenderDrawColor(renderer, 70, 45, 65, 255);
        SDL_RenderFillRect(renderer, &hpback);
        SDL_Rect hp = {30, 25, player.hp*2, 14};
        SDL_SetRenderDrawColor(renderer, 30, 230, 180, 255);
        SDL_RenderFillRect(renderer, &hp);
        SDL_Rect ehpback = {710, 25, 220, 14};
        SDL_SetRenderDrawColor(renderer, 70, 45, 65, 255);
        SDL_RenderFillRect(renderer, &ehpback);
        SDL_Rect ehp = {710, 25, enemy.hp*4, 14};
        SDL_SetRenderDrawColor(renderer, 245, 65, 115, 255);
        SDL_RenderFillRect(renderer, &ehp);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (pad) SDL_GameControllerClose(pad);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    sceKernelExitProcess(0);
    return 0;
}
