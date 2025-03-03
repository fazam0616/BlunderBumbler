#include "GUI.h"

// Initialize the GUI
bool init_GUI(GUI* gui) {
    gui->selected_square = 0;
    gui->highlighted_square = 0;
    gui->hover_square = 0;
    memset(&gui->captured_pieces, 0, sizeof(GameBoard));
    memset(&gui->current_board, 0, sizeof(GameBoard));
    memset(&gui->display_board, 0, sizeof(GameBoard));
    gui->captured_num = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    
    if (TTF_Init() == -1) {
        printf("Failed to initialize SDL_ttf: %s\n", TTF_GetError());
        return -1;
    }

    gui->font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!gui->font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return -1;
    }

    gui->window = SDL_CreateWindow(
        "Chess GUI",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!gui->window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    gui->renderer = SDL_CreateRenderer(gui->window, -1, SDL_RENDERER_SOFTWARE);
    if (!gui->renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRenderDrawBlendMode(gui->renderer, SDL_BLENDMODE_BLEND);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }
    
    load_textures(gui, "./textures");


    initializeBoard(&gui->current_board);
    initializeBoard(&gui->display_board);

    printf("Succesfully initialized GUI obj\n");
    return true;
}

// Load textures for pieces
void load_textures(GUI* gui, const char* texture_path) {
    const char* piece_names[PIECE_TYPE_COUNT+1] = {"pawn", "bishop", "rook", "knight", "queen", "king", "promoted"};
    const char* colors[2] = {"black", "white"};

    for (int color = 0; color < 2; color++) {
        for (int piece = 0; piece < PIECE_TYPE_COUNT; piece++) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "%s/%s/%s.png", texture_path, colors[color], piece_names[piece]);
            SDL_Surface* surface = IMG_Load(filepath);
            if (!surface) {
                printf("Failed to load image %s! IMG_Error: %s\n", filepath, IMG_GetError());
                continue;
            }
            gui->textures[color][piece] = SDL_CreateTextureFromSurface(gui->renderer, surface);
            SDL_FreeSurface(surface);
        }
    }
}

// Draw the chessboard and pieces
void draw_chessboard(GUI* gui) {
    GameBoard* board = &gui->display_board;
    SDL_SetRenderDrawColor(gui->renderer, 255, 255, 255, 255); // White background
    SDL_RenderClear(gui->renderer);

    // Draw chessboard tiles
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            SDL_Rect tile = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            
            if ((i + j) % 2 == 0) {
                SDL_SetRenderDrawColor(gui->renderer, 240, 217, 181, 255); // Light tile
            } else {
                SDL_SetRenderDrawColor(gui->renderer, 181, 136, 99, 255); // Dark tile
            }
            SDL_RenderFillRect(gui->renderer, &tile);

            if (getBitBoardBit(&gui->selected_square, i, j)) {
                SDL_SetRenderDrawColor(gui->renderer, 255, 255, 0, 255); // Yellow border

                // Thicker border effect (draw multiple rectangles with offsets)
                int border_thickness = 5; // Adjust thickness here

                for (int offset = 0; offset < border_thickness; offset++) {
                    SDL_Rect border_rect = {
                        tile.x + offset, 
                        tile.y + offset, 
                        tile.w - 2 * offset, 
                        tile.h - 2 * offset
                    };
                    SDL_RenderDrawRect(gui->renderer, &border_rect);
                }
            }

            // Draw piece if present
            Piece type = getPieceType(board, i, j);
            if (type >= 0) {
                int color = getBitBoardBit(&board->bitboards[WHITE], i, j) ; // White or Black
                SDL_Rect piece_rect = {j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE};
                SDL_RenderCopy(gui->renderer, gui->textures[color][type], NULL, &piece_rect);
            }

            if (getBitBoardBit(&gui->hover_square, i, j)){
                SDL_SetRenderDrawColor(gui->renderer, 112, 112, 112, 100);
                SDL_RenderFillRect(gui->renderer, &tile);
            }

            if (getBitBoardBit(&gui->highlighted_square, i, j)){
                SDL_SetRenderDrawColor(gui->renderer, 59, 227, 104, 150); // highlighted tile
                SDL_RenderFillRect(gui->renderer, &tile);
            }
        }
    }
}

/*
TODO: Change gui to store bitboard of all captured pieces, and render from that rather than a list
*/

// Draw captured pieces
void draw_captured_pieces(GUI* gui) {
    int x = BOARD_SIZE * TILE_SIZE + 10; // Starting x position
    int y = 10; // Starting y position


    for (int i = 0; i < gui->captured_num; i++){
        Piece type = getPieceType(&gui->captured_pieces, i / 8, i % 8);
        if (type >= 0){
            int color = getBitBoardBit(&gui->captured_pieces.bitboards[WHITE], i / 8, i%8) ? 1 : 0;
            SDL_Rect mini_piece_rect = {x, y, MINI_PIECE_SIZE, MINI_PIECE_SIZE};
            SDL_RenderCopy(gui->renderer, gui->textures[color][type], NULL, &mini_piece_rect);

            y += MINI_PIECE_SIZE + 5; // Move down for the next piece
            if (y + MINI_PIECE_SIZE > WINDOW_HEIGHT) {
                y = 10;
                x += MINI_PIECE_SIZE + 5; // Move to the next column
            }
        }
    }
}

// Destroy the GUI
void destroy_GUI(GUI* gui) {
    for (int color = 0; color < 2; color++) {
        for (int piece = 0; piece < 6; piece++) {
            if (gui->textures[color][piece]) {
                SDL_DestroyTexture(gui->textures[color][piece]);
            }
        }
    }
    TTF_CloseFont(gui->font);

    SDL_DestroyRenderer(gui->renderer);
    SDL_DestroyWindow(gui->window);
    IMG_Quit();
    TTF_Quit(); 
    SDL_Quit();
}





