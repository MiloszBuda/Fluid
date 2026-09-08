#include <SDL3/SDL.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_BLUE 0x34c3eb
#define COLOR_GREY 0x1f1f1f1f
#define CELL_SIZE 30
#define LINE_WIDTH 2
#define COLUMNS SCREEN_WIDTH / CELL_SIZE
#define ROWS SCREEN_HEIGHT / CELL_SIZE
#define SOLID_TYPE 0
#define WATER_TYPE 1
#define VOID_TYPE 2

struct Cell {
    int type;
    int fill_state;
    int x;
    int y;
};

void draw_grid(SDL_Surface *screen) {
    for (int x = 0; x < COLUMNS; x++) {
        SDL_Rect column = (SDL_Rect){x * CELL_SIZE,0,LINE_WIDTH , SCREEN_HEIGHT};
        SDL_FillSurfaceRect(screen, &column, COLOR_GREY);

    }
    for (int y = 0; y < ROWS; y++) {
        SDL_Rect row = (SDL_Rect){0,y * CELL_SIZE,SCREEN_WIDTH ,LINE_WIDTH };
        SDL_FillSurfaceRect(screen, &row, COLOR_GREY);

    }
}

void draw_cell(SDL_Surface *screen, struct Cell cell) {
    int pixel_x = cell.x * CELL_SIZE;
    int pixel_y = cell.y * CELL_SIZE;
    SDL_Rect cell_rect = (SDL_Rect){pixel_x, pixel_y,CELL_SIZE , CELL_SIZE};
    Uint32 color = COLOR_WHITE;
    if (cell.type == WATER_TYPE) {
        color = COLOR_BLUE;
    }
    if (cell.type == VOID_TYPE) {
        color = COLOR_BLACK;
    }
    SDL_FillSurfaceRect(screen, &cell_rect, color);
}

int main(void)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Test fluid",SCREEN_WIDTH ,SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);

    SDL_Surface *screen = SDL_GetWindowSurface(window);
    draw_grid(screen);

    int running = 1;
    SDL_Event event;
    int current_type = SOLID_TYPE;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }

            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                if (event.motion.state != 0) {
                    int cell_x = event.motion.x / CELL_SIZE;
                    int cell_y = event.motion.y / CELL_SIZE;
                    struct Cell cell = (struct Cell){current_type, 0, cell_x,cell_y,};
                    draw_cell(screen, cell);
                }
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_SPACE) {
                    current_type = (current_type + 1) % 3;
                }
            }
        }

        SDL_UpdateWindowSurface(window);
        SDL_Delay(10);

    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}