#include "Chess.h"
#include "GUI.h"
#include <stdio.h>
#include <stdlib.h>
#include "LRUList.h"

void handle_events(int* mouseX, int* mouseY, bool* pressed, bool*  isMoving, bool* running){
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *running = false;
                break;

            // Mouse movement
            case SDL_MOUSEMOTION:
                *isMoving = true;
                *mouseX = event.motion.x;
                *mouseY = event.motion.y;
                break;

            // Mouse button down
            case SDL_MOUSEBUTTONDOWN:
                // Start drag if left button is pressed
                if (event.button.button == SDL_BUTTON_LEFT) {
                    *pressed = true;
                }
                break;

            // Mouse button up
            case SDL_MOUSEBUTTONUP:
                // End drag if left button is released
                if (event.button.button == SDL_BUTTON_LEFT && *pressed) {
                    *pressed = false;
                }
                break;

            default:
                break;
        }
    }
}

void startMovingPiece(GUI *gui, int* selected, int picked_coord, Piece* dragging){
    int posx = *selected % 8;
    int posy = *selected / 8;
    int row = picked_coord / 8;
    int column = picked_coord % 8;
    *dragging = getPieceType(&gui->current_board, row, column);
    
    //Actual piece grabbed
    if(*dragging >= 0){
        
        toggleBitBoardBit(&gui->display_board.bitboards[*dragging], row, column);
        toggleBitBoardBit(&gui->display_board.bitboards[WHITE], row, column);
        
        // setBitBoardBit(&gui->selected_square, row, column, 1);
        toggleBitBoardBit(&gui->selected_square, posy, posx);

        //Clicking for first time in a row
        if (posx != column || posy != row && *selected >= 0){
            gui->highlighted_square = 0;
            setBitBoardBit(&gui->selected_square, posy, posx, 0);
            setBitBoardBit(&gui->selected_square, row, column, 1);
        }else{
            //Clicking second time in a row
            if (getBitBoardBit(&gui->selected_square, row, column)){
                getMoves(&gui->current_board, &gui->highlighted_square, row, column);
            }
        }
        if (getBitBoardBit(&gui->selected_square, row, column)){
            getMoves(&gui->current_board, &gui->highlighted_square, row, column);
        }

        *selected = picked_coord;
        
    }
}

int main(void** args){
    bool running = true;
    GUI* gui = (GUI*)malloc(sizeof(GUI));   
    int mouseX;
    int mouseY;
    bool clicked;
    bool moving;
    bool isPlayerTurn = true;
    int selected = -1;
    Piece dragging = -1;
    bool turnColor = true;
    char text[30];
    int inCheck = 0;
    Hashmap* SearchTree = new_hashmap(&hash, &compare, 100);
    GameBranch* new_branch = NULL;
    int SEARCH_DEPTH = 6;

    if (!init_GUI(gui)) {
        printf("Failed to initialize GUI.\n");
        return -1;
    }
    
    //Initial empty board;

    printf("Beginning frame loop:\n");
    while (running) {
        moving = false;
        handle_events(&mouseX, &mouseY, &clicked, &moving, &running);

        gui->hover_square = 0;

        int row = mouseY / TILE_SIZE;
        int column = mouseX / TILE_SIZE;

        //Mouse is over chess area
        if (row >= 0 && row < 8 && column >= 0 && column < 8){
            toggleBitBoardBit(&gui->hover_square, row, column);

            //Attempting to pick up a piece (ensure square contains a piece, of the correct color)
            if (clicked && dragging < 0 && getBitBoardBit(&gui->current_board.bitboards[WHITE], row, column) == turnColor){
                startMovingPiece(gui, &selected, row*8 + column, &dragging);
            }
            //Dropping a held piece
            else if (!clicked && dragging >= 0){
                int posx = selected % 8;
                int posy = selected / 8;

                //Piece is dropped over valid spot
                if ((posx != column || posy != row) && getBitBoardBit(&gui->highlighted_square, row, column)){

                    toggleBitBoardBit(&gui->display_board.bitboards[dragging], row, column);
                    toggleBitBoardBit(&gui->display_board.bitboards[WHITE], row, column);
                    
                    int score;
                    memcpy(&gui->display_board, &gui->current_board, sizeof(GameBoard)); 
                    movePiece(&gui->current_board, &gui->captured_pieces, &gui->captured_num, posy, posx, row, column);
                    printf("That move scored: %d\n", (score = getScore(&gui->display_board, &gui->current_board, posy, posx, row, column)));
                    memcpy(&gui->display_board, &gui->current_board, sizeof(GameBoard));
                    
                    gui->selected_square = 0;
                    selected = -1;
                    gui->highlighted_square = 0;

                    turnColor = !turnColor;

                    if (isCheck(&gui->current_board, turnColor)){
                        if (isCheckMate(&gui->current_board, turnColor)){
                            // printf("In check mate\n");
                            inCheck = 2;
                        }else{
                            // printf("In check\n");
                            inCheck = 1;
                        }
                    }else{
                        inCheck = 0;
                    }

                    GameBranch* t = new_branch;

                    new_branch = addToPool(SearchTree, &gui->current_board, isPlayerTurn ? WhiteTurn : BlackTurn);
                    if (new_branch->depth < 0){
                        new_branch->prev_branch = t;
                        new_branch->score = score;
                        new_branch->best = NULL;
                        
                        if (t)
                            new_branch->depth = t->depth+1;
                        else    
                            new_branch->depth = 0;
                    }
                    pruneCheck(t, new_branch, SearchTree);

                    GameBranch* best = search(SearchTree,new_branch, true, SEARCH_DEPTH );
                    printHistory(new_branch, 1);
                    if (best){
                        printf("Best found so far:%d\n", best->score);
                        printf("Explored branches: %ld\n\n", SearchTree->size);

                        GameBranch* current = best;
                        t = best;
                    }else{
                        printf("No \"best\" found\n");
                    }
                    isPlayerTurn = !isPlayerTurn;
                }
                //Piece is dropped over invalid spot
                else{
                    toggleBitBoardBit(&gui->display_board.bitboards[dragging], posy, posx);
                    toggleBitBoardBit(&gui->display_board.bitboards[WHITE], posy, posx);
                }

                //If source square is unselected, remove highlight after dropping
                if (!getBitBoardBit(&gui->selected_square, posy, posx)){
                    gui->highlighted_square = 0;
                }
                dragging = -1;
            }
        }

        

        //Render Game
        draw_chessboard(gui);
        draw_captured_pieces(gui);

        //Render piece in hand
        if (dragging >= 0){
            SDL_Rect piece_rect = {mouseX - TILE_SIZE/2, mouseY - TILE_SIZE/2, TILE_SIZE, TILE_SIZE};
            SDL_RenderCopy(gui->renderer, gui->textures[
                getBitBoardBit(&gui->current_board.bitboards[WHITE], selected / 8, selected%8) ? 1 : 0
                ][dragging], NULL, &piece_rect);
        }

        //Render text
        memset(text, 0, sizeof(text));
        snprintf(text, sizeof(text), "%ss turn%s", turnColor ? "White" : "Black", inCheck > 0 ? (inCheck == 2 ? ", in checkmate": ", in check") : " ");
        SDL_Color color = {0, 0, 0, 255};
        SDL_Surface* textSurface = TTF_RenderText_Blended(gui->font, text, color);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gui->renderer, textSurface);
        SDL_FreeSurface(textSurface);

        SDL_Rect textRect = {TILE_SIZE*.25, (BOARD_SIZE+.25)*TILE_SIZE, textSurface->w, textSurface->h};

        SDL_RenderCopy(gui->renderer, textTexture, NULL, &textRect);
        SDL_DestroyTexture(textTexture);
        
        SDL_RenderPresent(gui->renderer);
    }
    destroy_GUI(gui);

    return 0;
}