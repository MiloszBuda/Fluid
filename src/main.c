#include <SDL3/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_GRID 0x111115
#define COLOR_DARK_HUD 0x101015
#define COLOR_HUD_BORDER 0x3b3b4a

#define CELL_SIZE 5
#define LINE_WIDTH 1
#define COLUMNS (SCREEN_WIDTH / CELL_SIZE)
#define ROWS (SCREEN_HEIGHT / CELL_SIZE)

#define AIR_TYPE 0
#define SOLID_TYPE 1

#define TOOL_WATER 0
#define TOOL_SOLID 1
#define TOOL_ERASER 2

#define MAX_BRUSH_RADIUS 25
#define MIN_BRUSH_RADIUS 0

#define GRAVITY_SUB_STEPS 3
#define HORIZONTAL_SUB_STEPS 5

struct Cell {
    int type;
    double fill_state;
    int x;
    int y;
};

const uint8_t* get_glyph_5x7(char c) {
    if (c >= 'a' && c <= 'z') c -= 32;

    static const uint8_t empty[7] = {0};
    static const uint8_t g_space[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    static const uint8_t g_dash[7]  = {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00};
    static const uint8_t g_colon[7] = {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00};
    static const uint8_t g_slash[7] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00};

    static const uint8_t g_0[7] = {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E};
    static const uint8_t g_1[7] = {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E};
    static const uint8_t g_2[7] = {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F};
    static const uint8_t g_3[7] = {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E};
    static const uint8_t g_4[7] = {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02};
    static const uint8_t g_5[7] = {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E};
    static const uint8_t g_6[7] = {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E};
    static const uint8_t g_7[7] = {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08};
    static const uint8_t g_8[7] = {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E};
    static const uint8_t g_9[7] = {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C};

    static const uint8_t g_A[7] = {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
    static const uint8_t g_B[7] = {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E};
    static const uint8_t g_C[7] = {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E};
    static const uint8_t g_D[7] = {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C};
    static const uint8_t g_E[7] = {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F};
    static const uint8_t g_F[7] = {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10};
    static const uint8_t g_G[7] = {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F};
    static const uint8_t g_H[7] = {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11};
    static const uint8_t g_I[7] = {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E};
    static const uint8_t g_J[7] = {0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C};
    static const uint8_t g_K[7] = {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11};
    static const uint8_t g_L[7] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F};
    static const uint8_t g_M[7] = {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11};
    static const uint8_t g_N[7] = {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11};
    static const uint8_t g_O[7] = {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
    static const uint8_t g_P[7] = {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10};
    static const uint8_t g_Q[7] = {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D};
    static const uint8_t g_R[7] = {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11};
    static const uint8_t g_S[7] = {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E};
    static const uint8_t g_T[7] = {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
    static const uint8_t g_U[7] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E};
    static const uint8_t g_V[7] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04};
    static const uint8_t g_W[7] = {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11};
    static const uint8_t g_X[7] = {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11};
    static const uint8_t g_Y[7] = {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04};
    static const uint8_t g_Z[7] = {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F};

    switch (c) {
        case ' ': return g_space;
        case '-': return g_dash;
        case ':': return g_colon;
        case '/': return g_slash;
        case '0': return g_0; case '1': return g_1; case '2': return g_2;
        case '3': return g_3; case '4': return g_4; case '5': return g_5;
        case '6': return g_6; case '7': return g_7; case '8': return g_8;
        case '9': return g_9;
        case 'A': return g_A; case 'B': return g_B; case 'C': return g_C;
        case 'D': return g_D; case 'E': return g_E; case 'F': return g_F;
        case 'G': return g_G; case 'H': return g_H; case 'I': return g_I;
        case 'J': return g_J; case 'K': return g_K; case 'L': return g_L;
        case 'M': return g_M; case 'N': return g_N; case 'O': return g_O;
        case 'P': return g_P; case 'Q': return g_Q; case 'R': return g_R;
        case 'S': return g_S; case 'T': return g_T; case 'U': return g_U;
        case 'V': return g_V; case 'W': return g_W; case 'X': return g_X;
        case 'Y': return g_Y; case 'Z': return g_Z;
        default:  return empty;
    }
}

void draw_char(SDL_Surface *screen, int x, int y, char c, uint32_t color, int scale) {
    const uint8_t *glyph = get_glyph_5x7(c);
    for (int r = 0; r < 7; r++) {
        uint8_t row = glyph[r];
        for (int col = 0; col < 5; col++) {
            if ((row >> (4 - col)) & 1) {
                SDL_Rect p = { x + col * scale, y + r * scale, scale, scale };
                SDL_FillSurfaceRect(screen, &p, color);
            }
        }
    }
}

void draw_text(SDL_Surface *screen, int x, int y, const char *text, uint32_t color, int scale) {
    int cur_x = x;
    while (*text) {
        draw_char(screen, cur_x, y, *text, color, scale);
        cur_x += (5 + 1) * scale;
        text++;
    }
}

void draw_hud(SDL_Surface *screen, int active_tool, int brush_radius) {
    SDL_Rect border = { 20, 20, 360, 240 };
    SDL_FillSurfaceRect(screen, &border, COLOR_HUD_BORDER);

    SDL_Rect bg = { 23, 23, 354, 234 };
    SDL_FillSurfaceRect(screen, &bg, COLOR_DARK_HUD);

    draw_text(screen, 38, 36, "NARZEDZIE:", 0x9ca3af, 2);
    if (active_tool == TOOL_WATER) {
        draw_text(screen, 180, 36, "WODA", 0x4bc8f5, 2);
    } else if (active_tool == TOOL_SOLID) {
        draw_text(screen, 180, 36, "SCIANA", COLOR_WHITE, 2);
    } else {
        draw_text(screen, 180, 36, "GUMKA", 0xf87171, 2);
    }

    char rad_str[32];
    snprintf(rad_str, sizeof(rad_str), "PROMIEN:   %d", brush_radius);
    draw_text(screen, 38, 58, rad_str, 0xfacc15, 2);

    SDL_Rect sep = { 38, 82, 324, 2 };
    SDL_FillSurfaceRect(screen, &sep, COLOR_HUD_BORDER);

    draw_text(screen, 38, 98,  "LPM    - RYSUJ",          0xd1d5db, 2);
    draw_text(screen, 38, 118, "PPM    - GUMKA",          0xd1d5db, 2);
    draw_text(screen, 38, 138, "SPACJA - ZMIANA BLOKU",   0x93c5fd, 2);
    draw_text(screen, 38, 158, "X      - TRYB GUMKI",     0xfca5a5, 2);
    draw_text(screen, 38, 178, "KOLKO  - ROZMIAR PEDZLA", 0xfde047, 2);
    draw_text(screen, 38, 198, "ESC    - WYJSCIE",        0x6b7280, 2);
}

// RENDEROWANIE Z CIENIOWANIEM OD POZIOMU TAFLI
void draw_grid(SDL_Surface *screen) {
    for (int x = 0; x < COLUMNS; x++) {
        SDL_Rect column = { x * CELL_SIZE, 0, LINE_WIDTH, SCREEN_HEIGHT };
        SDL_FillSurfaceRect(screen, &column, COLOR_GRID);
    }
    for (int y = 0; y < ROWS; y++) {
        SDL_Rect row = { 0, y * CELL_SIZE, SCREEN_WIDTH, LINE_WIDTH };
        SDL_FillSurfaceRect(screen, &row, COLOR_GRID);
    }
}

void draw_environment(SDL_Surface *screen, struct Cell *env) {
    SDL_FillSurfaceRect(screen, NULL, COLOR_BLACK);
    draw_grid(screen);

    // Tablica poziomu tafli (surface_y) dla każdego bloku wody
    static int surface_y[ROWS * COLUMNS];
    for (int i = 0; i < ROWS * COLUMNS; i++) {
        surface_y[i] = -1;
    }

    // Wykrywanie komórek stykających się z powietrzem (otwarta tafla)
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            int idx = x + COLUMNS * y;
            if (env[idx].type == AIR_TYPE && env[idx].fill_state > 0.005) {
                if (y == 0 || (env[x + COLUMNS * (y - 1)].type == AIR_TYPE && env[x + COLUMNS * (y - 1)].fill_state <= 0.005)) {
                    surface_y[idx] = y;
                }
            }
        }
    }

    // Propagacja poziomu tafli w dół i omijanie przeszkód na boki
    for (int y = 0; y < ROWS; y++) {
        // Dziedziczenie w dół z komórki powyżej
        if (y > 0) {
            for (int x = 0; x < COLUMNS; x++) {
                int idx = x + COLUMNS * y;
                int above_idx = x + COLUMNS * (y - 1);
                if (env[idx].type == AIR_TYPE && env[idx].fill_state > 0.005) {
                    if (surface_y[above_idx] != -1) {
                        if (surface_y[above_idx] > surface_y[idx]) {
                            surface_y[idx] = surface_y[above_idx];
                        }
                    }
                }
            }
        }

        // Zamiatanie w prawo (omijanie przeszkód od lewej strony)
        for (int x = 1; x < COLUMNS; x++) {
            int idx = x + COLUMNS * y;
            int left_idx = (x - 1) + COLUMNS * y;
            if (env[idx].type == AIR_TYPE && env[idx].fill_state > 0.005) {
                if (surface_y[left_idx] != -1) {
                    if (surface_y[left_idx] > surface_y[idx]) {
                        surface_y[idx] = surface_y[left_idx];
                    }
                }
            }
        }

        // Zamiatanie w lewo (omijanie przeszkód od prawej strony)
        for (int x = COLUMNS - 2; x >= 0; x--) {
            int idx = x + COLUMNS * y;
            int right_idx = (x + 1) + COLUMNS * y;
            if (env[idx].type == AIR_TYPE && env[idx].fill_state > 0.005) {
                if (surface_y[right_idx] != -1) {
                    if (surface_y[right_idx] > surface_y[idx]) {
                        surface_y[idx] = surface_y[right_idx];
                    }
                }
            }
        }
    }

    // Właściwe rysowanie z płynną głębokością hydrostatyczną
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS; x++) {
            int idx = x + COLUMNS * y;
            struct Cell cell = env[idx];

            if (cell.type == SOLID_TYPE) {
                SDL_Rect rect = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                SDL_FillSurfaceRect(screen, &rect, COLOR_WHITE);
            } else if (cell.type == AIR_TYPE && cell.fill_state > 0.005) {
                int px = x * CELL_SIZE;
                int py = y * CELL_SIZE;
                int water_height = (int)(cell.fill_state * CELL_SIZE);
                if (water_height > CELL_SIZE) water_height = CELL_SIZE;
                if (water_height < 1) water_height = 1;

                SDL_Rect water_rect = {
                    px,
                    py + CELL_SIZE - water_height,
                    CELL_SIZE,
                    water_height
                };

                // Prawdziwa głębokość od poziomu tafli
                int sy = surface_y[idx];
                int depth = (sy != -1) ? (y - sy) : 0;
                if (depth < 0) depth = 0;

                // Gradient: Powierzchnia (depth 0) -> Głębia (depth 35+ kratek)
                float t = (float)depth / 35.0f;
                if (t > 1.0f) t = 1.0f;

                int r = (int)(75  + t * (12  - 75));
                int g = (int)(200 + t * (45  - 200));
                int b = (int)(245 + t * (115 - 245));
                uint32_t water_color = (uint32_t)((r << 16) | (g << 8) | b);

                SDL_FillSurfaceRect(screen, &water_rect, water_color);
            }
        }
    }
}

void initialize_environment(struct Cell *env) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            env[j + COLUMNS * i] = (struct Cell){ AIR_TYPE, 0.0, j, i };
        }
    }
}

// FIZYKA PŁYNU
void simulate_gravity(struct Cell *env) {
    for (int y = ROWS - 2; y >= 0; y--) {
        for (int x = 0; x < COLUMNS; x++) {
            int idx = x + COLUMNS * y;
            if (env[idx].type == AIR_TYPE && env[idx].fill_state > 0.0001) {
                int below_idx = x + COLUMNS * (y + 1);
                if (env[below_idx].type == AIR_TYPE && env[below_idx].fill_state < 1.0) {
                    double space = 1.0 - env[below_idx].fill_state;
                    double flow = fmin(env[idx].fill_state, space);
                    env[idx].fill_state -= flow;
                    env[below_idx].fill_state += flow;
                }
            }
        }
    }
}

void simulate_diagonals(struct Cell *env) {
    for (int y = ROWS - 2; y >= 0; y--) {
        for (int x = 0; x < COLUMNS; x++) {
            int idx = x + COLUMNS * y;
            if (env[idx].type != AIR_TYPE || env[idx].fill_state <= 0.001) continue;

            int below_idx = x + COLUMNS * (y + 1);
            if (env[below_idx].type == SOLID_TYPE || env[below_idx].fill_state >= 0.99) {
                double water = env[idx].fill_state;
                int left_open = (x > 0 && env[(x - 1) + COLUMNS * y].type == AIR_TYPE &&
                                env[(x - 1) + COLUMNS * (y + 1)].type == AIR_TYPE &&
                                env[(x - 1) + COLUMNS * (y + 1)].fill_state < 0.99);

                int right_open = (x < COLUMNS - 1 && env[(x + 1) + COLUMNS * y].type == AIR_TYPE &&
                                 env[(x + 1) + COLUMNS * (y + 1)].type == AIR_TYPE &&
                                 env[(x + 1) + COLUMNS * (y + 1)].fill_state < 0.99);

                if (left_open && right_open) {
                    int dl = (x - 1) + COLUMNS * (y + 1);
                    int dr = (x + 1) + COLUMNS * (y + 1);
                    double flow_each = water / 2.0;
                    double flow_l = fmin(flow_each, 1.0 - env[dl].fill_state);
                    double flow_r = fmin(flow_each, 1.0 - env[dr].fill_state);
                    env[idx].fill_state -= (flow_l + flow_r);
                    env[dl].fill_state += flow_l;
                    env[dr].fill_state += flow_r;
                } else if (left_open) {
                    int dl = (x - 1) + COLUMNS * (y + 1);
                    double flow_l = fmin(water, 1.0 - env[dl].fill_state);
                    env[idx].fill_state -= flow_l;
                    env[dl].fill_state += flow_l;
                } else if (right_open) {
                    int dr = (x + 1) + COLUMNS * (y + 1);
                    double flow_r = fmin(water, 1.0 - env[dr].fill_state);
                    env[idx].fill_state -= flow_r;
                    env[dr].fill_state += flow_r;
                }
            }
        }
    }
}

void simulate_horizontal(struct Cell *env) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLUMNS - 1; x++) {
            int curr = x + COLUMNS * y;
            int right = (x + 1) + COLUMNS * y;
            if (env[curr].type == AIR_TYPE && env[right].type == AIR_TYPE) {
                int supported = (y == ROWS - 1) ||
                                (env[x + COLUMNS * (y + 1)].type == SOLID_TYPE) ||
                                (env[x + COLUMNS * (y + 1)].fill_state >= 0.7);
                if (supported) {
                    double diff = env[curr].fill_state - env[right].fill_state;
                    if (diff > 0.001) {
                        double flow = fmin(diff / 2.0, 1.0 - env[right].fill_state);
                        env[curr].fill_state -= flow;
                        env[right].fill_state += flow;
                    }
                }
            }
        }
        for (int x = COLUMNS - 1; x > 0; x--) {
            int curr = x + COLUMNS * y;
            int left = (x - 1) + COLUMNS * y;
            if (env[curr].type == AIR_TYPE && env[left].type == AIR_TYPE) {
                int supported = (y == ROWS - 1) ||
                                (env[x + COLUMNS * (y + 1)].type == SOLID_TYPE) ||
                                (env[x + COLUMNS * (y + 1)].fill_state >= 0.7);
                if (supported) {
                    double diff = env[curr].fill_state - env[left].fill_state;
                    if (diff > 0.001) {
                        double flow = fmin(diff / 2.0, 1.0 - env[left].fill_state);
                        env[curr].fill_state -= flow;
                        env[left].fill_state += flow;
                    }
                }
            }
        }
    }
}

void clean_residuals(struct Cell *env) {
    for (int i = 0; i < ROWS * COLUMNS; i++) {
        if (env[i].type == AIR_TYPE && env[i].fill_state < 0.0005) {
            env[i].fill_state = 0.0;
        }
    }
}

// PĘDZEL Z INTERPOLACJĄ (Płynny ruch)
void apply_brush_circle(struct Cell *env, int center_x, int center_y, int radius, int tool) {
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius) {
                int cx = center_x + dx;
                int cy = center_y + dy;
                if (cx >= 0 && cx < COLUMNS && cy >= 0 && cy < ROWS) {
                    int idx = cy * COLUMNS + cx;
                    if (tool == TOOL_WATER) {
                        env[idx] = (struct Cell){ AIR_TYPE, 1.0, cx, cy };
                    } else if (tool == TOOL_SOLID) {
                        env[idx] = (struct Cell){ SOLID_TYPE, 0.0, cx, cy };
                    } else if (tool == TOOL_ERASER) {
                        env[idx] = (struct Cell){ AIR_TYPE, 0.0, cx, cy };
                    }
                }
            }
        }
    }
}

void apply_brush_line(struct Cell *env, int x0, int y0, int x1, int y1, int radius, int tool) {
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (1) {
        apply_brush_circle(env, x0, y0, radius, tool);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
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
    int active_tool = TOOL_SOLID;
    int brush_radius = 1;

    int prev_cx = -1;
    int prev_cy = -1;
    int was_drawing = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }

            if (event.type == SDL_EVENT_MOUSE_WHEEL) {
                if (event.wheel.y > 0.0f && brush_radius < MAX_BRUSH_RADIUS) brush_radius++;
                else if (event.wheel.y < 0.0f && brush_radius > MIN_BRUSH_RADIUS) brush_radius--;
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_SPACE) {
                    active_tool = (active_tool == TOOL_WATER) ? TOOL_SOLID : TOOL_WATER;
                } else if (event.key.key == SDLK_X) {
                    active_tool = TOOL_ERASER;
                } else if (event.key.key == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }

        float mx, my;
        Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);
        int cx = (int)(mx / CELL_SIZE);
        int cy = (int)(my / CELL_SIZE);

        int is_lmb = (mouse_buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0;
        int is_rmb = (mouse_buttons & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) != 0;

        if (is_lmb || is_rmb) {
            int current_tool = is_rmb ? TOOL_ERASER : active_tool;
            if (was_drawing) {
                apply_brush_line(environment, prev_cx, prev_cy, cx, cy, brush_radius, current_tool);
            } else {
                apply_brush_circle(environment, cx, cy, brush_radius, current_tool);
            }
            prev_cx = cx;
            prev_cy = cy;
            was_drawing = 1;
        } else {
            was_drawing = 0;
        }

        // Fizyka
        for (int g = 0; g < GRAVITY_SUB_STEPS; g++) {
            simulate_gravity(environment);
            simulate_diagonals(environment);
            for (int h = 0; h < HORIZONTAL_SUB_STEPS; h++) {
                simulate_horizontal(environment);
            }
        }
        clean_residuals(environment);

        // Renderowanie
        draw_environment(screen, environment);
        draw_hud(screen, active_tool, brush_radius);
        SDL_UpdateWindowSurface(window);

        SDL_Delay(16);
    }

    free(environment);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}