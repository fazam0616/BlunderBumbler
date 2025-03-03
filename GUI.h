#ifndef HEADER_GUI
#define HEADER_GUI

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "Chess.h"
#include "List.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BOARD_SIZE 8
#define TILE_SIZE 64
#define SIDE_PANEL_WIDTH 200
#define MINI_PIECE_SIZE 32

typedef struct GUI {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* textures[2][PIECE_TYPE_COUNT]; // [0: black, 1: white][PAWN, BISHOP, ...]
    uint64_t selected_square;
    uint64_t highlighted_square;
    uint64_t hover_square;
    GameBoard captured_pieces;
    GameBoard current_board;
    GameBoard display_board;
    int captured_num;
    TTF_Font* font;
    
} GUI;

// Function prototypes
bool init_GUI(GUI* gui);
void load_textures(GUI* gui, const char* texture_path);
void draw_chessboard(GUI* gui);
void draw_captured_pieces(GUI* gui);
void destroy_GUI(GUI* gui);

#endif
