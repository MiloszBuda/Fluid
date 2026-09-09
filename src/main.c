#include <SDL3/SDL.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_BLUE 0x34c3eb
#define COLOR_GREY 0x1f1f1f1f

#define CELL_SIZE 5
#define LINE_WIDTH 1
#define COLUMNS (SCREEN_WIDTH / CELL_SIZE)
#define ROWS (SCREEN_HEIGHT / CELL_SIZE)

#define AIR_TYPE 0
#define SOLID_TYPE 1

#define TOOL_WATER 0
#define TOOL_SOLID 1
#define TOOL_ERASER 2

#define MAX_BRUSH_RADIUS 8
#define MIN_BRUSH_RADIUS 2

#define SUB_STEPS 4
#define SEARCH_RANGE 5 // Zasięg szukania w bok (np. 5 kratek)

struct CellFlow {
    double flow_down;
    int target_left;
    double flow_left;
    int target_right;
    double flow_right;
};

struct Cell {
    int type;
    double fill_state;
    int x;
    int y;
};



void draw_grid(SDL_Surface *screen) {
    for (int x = 0; x < COLUMNS; x++) {
        SDL_Rect column = (SDL_Rect){x * CELL_SIZE, 0, LINE_WIDTH, SCREEN_HEIGHT};
        SDL_FillSurfaceRect(screen, &column, COLOR_GREY);
    }
    for (int y = 0; y < ROWS; y++) {
        SDL_Rect row = (SDL_Rect){0, y * CELL_SIZE, SCREEN_WIDTH, LINE_WIDTH};
        SDL_FillSurfaceRect(screen, &row, COLOR_GREY);
    }
}

void draw_cell(SDL_Surface *screen, struct Cell cell) {
    int pixel_x = cell.x * CELL_SIZE;
    int pixel_y = cell.y * CELL_SIZE;
    SDL_Rect cell_rect = (SDL_Rect){pixel_x, pixel_y, CELL_SIZE, CELL_SIZE};

    SDL_FillSurfaceRect(screen, &cell_rect, COLOR_BLACK);

    if (cell.type == SOLID_TYPE) {
        SDL_FillSurfaceRect(screen, &cell_rect, COLOR_WHITE);
    } else if (cell.type == AIR_TYPE && cell.fill_state > 0.001) {
        int water_height = (int)(cell.fill_state * CELL_SIZE);
        if (water_height > CELL_SIZE) water_height = CELL_SIZE;
        if (water_height < 1) water_height = 1;

        SDL_Rect water_rect = (SDL_Rect){
            pixel_x,
            pixel_y + CELL_SIZE - water_height,
            CELL_SIZE,
            water_height
        };
        SDL_FillSurfaceRect(screen, &water_rect, COLOR_BLUE);
    }
}

void initialize_environment(struct Cell environment[ROWS * COLUMNS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            environment[j + COLUMNS * i] = (struct Cell){AIR_TYPE, 0.0, j, i};
        }
    }
}

void draw_environment(SDL_Surface *screen, struct Cell environment[ROWS * COLUMNS]) {
    for (int i = 0; i < ROWS * COLUMNS; i++) {
        draw_cell(screen, environment[i]);
    }
}

int find_horizontal_target(struct Cell *env, int start_x, int start_y, int dir, double water_level) {
    for (int d = 1; d <= SEARCH_RANGE; d++) {
        int nx = start_x + d * dir;
        if (nx < 0 || nx >= COLUMNS) break;

        int n_idx = nx + COLUMNS * start_y;
        if (env[n_idx].type == SOLID_TYPE) {
            break; // Ściana blokuje dalszy przepływ
        }

        // 1. Wykrywanie przepaści/odpływu – woda priorytetowo płynie w stronę dziury
        if (start_y < ROWS - 1) {
            int below_idx = nx + COLUMNS * (start_y + 1);
            if (env[below_idx].type == AIR_TYPE && env[below_idx].fill_state < 0.99) {
                return n_idx;
            }
        }

        // 2. Pierwsza komórka o wyraźnie niższym poziomie wody
        if (env[n_idx].fill_state < water_level - 0.01) {
            return n_idx;
        }

        // Jeśli komórka ma tyle samo lub więcej wody, pętla szuka dalej (woda przepływa przez nią)
    }
    return -1;
}


void simulation_step(struct Cell environment[ROWS * COLUMNS]) {
    static struct CellFlow flows[ROWS * COLUMNS];
    for (int i = 0; i < ROWS * COLUMNS; i++) {
        flows[i] = (struct CellFlow){0.0, -1, 0.0, -1, 0.0};
    }

    // 1. Obliczanie przepływów
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            int idx = j + COLUMNS * i;
            struct Cell curr = environment[idx];

            if (curr.type != AIR_TYPE || curr.fill_state <= 0.001) {
                continue;
            }

            double remaining_water = curr.fill_state;

            // KROK 1: Grawitacja w dół (najwyższy priorytet)
            if (i < ROWS - 1) {
                int below_idx = j + COLUMNS * (i + 1);
                struct Cell below = environment[below_idx];

                if (below.type == AIR_TYPE) {
                    double space_below = 1.0 - below.fill_state;
                    if (space_below > 0.0) {
                        double flow = fmin(remaining_water, space_below);
                        flows[idx].flow_down = flow;
                        remaining_water -= flow;
                    }
                }
            }

            // KROK 2: Błyskawiczne poziomowanie na boki (gdy woda spoczywa na podłożu)
            if (remaining_water > 0.001) {
                int target_l = find_horizontal_target(environment, j, i, -1, remaining_water);
                int target_r = find_horizontal_target(environment, j, i,  1, remaining_water);

                flows[idx].target_left = target_l;
                flows[idx].target_right = target_r;

                if (target_l != -1 && target_r != -1) {
                    double diff_l = remaining_water - environment[target_l].fill_state;
                    double diff_r = remaining_water - environment[target_r].fill_state;
                    flows[idx].flow_left = diff_l / 3.0;
                    flows[idx].flow_right = diff_r / 3.0;
                } else if (target_l != -1) {
                    double diff_l = remaining_water - environment[target_l].fill_state;
                    flows[idx].flow_left = diff_l / 2.0;
                } else if (target_r != -1) {
                    double diff_r = remaining_water - environment[target_r].fill_state;
                    flows[idx].flow_right = diff_r / 2.0;
                }
            }
        }
    }

    // 2. Aplikowanie przepływów (zachowanie 100% masy)
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            int idx = j + COLUMNS * i;
            struct CellFlow f = flows[idx];

            environment[idx].fill_state -= (f.flow_down + f.flow_left + f.flow_right);

            if (f.flow_down > 0.0) {
                environment[j + COLUMNS * (i + 1)].fill_state += f.flow_down;
            }
            if (f.flow_left > 0.0 && f.target_left != -1) {
                environment[f.target_left].fill_state += f.flow_left;
            }
            if (f.flow_right > 0.0 && f.target_right != -1) {
                environment[f.target_right].fill_state += f.flow_right;
            }
        }
    }

    // Czyszczenie resztek numerycznych
    for (int i = 0; i < ROWS * COLUMNS; i++) {
        if (environment[i].fill_state < 0.0001) {
            environment[i].fill_state = 0.0;
        }
    }
}

void apply_brush(struct Cell *environment, int center_x, int center_y, int radius, int tool) {
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius) {
                int cx = center_x + dx;
                int cy = center_y + dy;

                if (cx >= 0 && cx < COLUMNS && cy >= 0 && cy < ROWS) {
                    int idx = cy * COLUMNS + cx;
                    if (tool == TOOL_WATER) {
                        environment[idx] = (struct Cell){AIR_TYPE, 1.0, cx, cy};
                    } else if (tool == TOOL_SOLID) {
                        environment[idx] = (struct Cell){SOLID_TYPE, 0.0, cx, cy};
                    } else if (tool == TOOL_ERASER) {
                        environment[idx] = (struct Cell){AIR_TYPE, 0.0, cx, cy};
                    }
                }
            }
        }
    }
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Test fluid", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Surface *screen = SDL_GetWindowSurface(window);

    struct Cell *environment = malloc(sizeof(struct Cell) * ROWS * COLUMNS);
    initialize_environment(environment);

    int running = 1;
    SDL_Event event;
    int active_tool = TOOL_WATER;
    int brush_radius = 0;

    while (running) {
        // 1. Obsługa pojedynczych zdarzeń (okno, klawiatura, kółko myszy)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }

            // Rozmiar pędzla kółkiem
            if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                if (event.wheel.y > 0.0f && brush_radius < MAX_BRUSH_RADIUS) {
                    brush_radius++;
                } else if (event.wheel.y < 0.0f && brush_radius > MIN_BRUSH_RADIUS) {
                    brush_radius--;
                }
            }

            // Przełączanie narzędzi
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_SPACE) {
                    active_tool = (active_tool == TOOL_WATER) ? TOOL_SOLID : TOOL_WATER;
                } else if (event.key.key == SDLK_X) {
                    active_tool = TOOL_ERASER;
                }
            }
        }

        // 2. Ciągła obsługa myszy (działa także przy trzymaniu w bezruchu)
        float mx, my;
        Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);

        int cx = (int)(mx / CELL_SIZE);
        int cy = (int)(my / CELL_SIZE);

        if (mouse_buttons & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) {
            apply_brush(environment, cx, cy, brush_radius, TOOL_ERASER);
        } else if (mouse_buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
            apply_brush(environment, cx, cy, brush_radius, active_tool);
        }

        // 3. Fizyka z sub-steppingiem (wielokrotny krok symulacji)
        for (int step = 0; step < SUB_STEPS; step++) {
            simulation_step(environment);
        }

        // 4. Renderowanie (raz na klatkę)
        draw_environment(screen, environment);
        draw_grid(screen);
        SDL_UpdateWindowSurface(window);

        // Stabilne ~60 FPS
        SDL_Delay(16);
    }

    free(environment);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}