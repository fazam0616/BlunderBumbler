#ifndef HEADER_CHESS
#define HEADER_CHESS

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "List.h"
#include "Hashmap.h"

#define PAWN        0
#define BISHOP        1
#define ROOK      2
#define KNIGHT      3
#define QUEEN       4
#define KING        5
#define PROMOTED    6
#define WHITE       7
#define PIECE_TYPE_COUNT 7
#define BITBOARD_COUNT 8

typedef int Piece;

typedef enum Turn {
    BlackTurn, WhiteTurn
} Turn;

typedef struct GameBoard{
    uint64_t bitboards[BITBOARD_COUNT];
} GameBoard;

typedef struct GameBranch{
    GameBoard board;
    int score;
    int board_score;
    int alpha;
    int beta;
    struct GameBranch * prev_branch;
    Tree* possible_branches;
    struct GameBranch* best;
    signed long depth;

    /*
    Turn being WhiteTurn implies the board represents a state
    where White should move a piece, but hasn't done so yet.
    */
    Turn turn;

    // LRU tracking
    struct GameBranch* lru_prev;
    struct GameBranch* lru_next;
} GameBranch;

void toggleBitBoardBit(uint64_t* G, int i, int j);
bool getBitBoardBit(uint64_t* G, int i, int j);
uint64_t getBlackMask(GameBoard* G);
uint64_t getWhiteMask(GameBoard* G);
int getPieceType(GameBoard* G, int i, int j);
bool toggleGameBoardBit(GameBoard* G, int i, int j);
void setBitBoardBit(uint64_t* G, int i, int j, uint64_t val);
void setPieceType(GameBoard* G, Piece type, int i, int j);

int compare(void* A, void* B);
uint64_t hash(void* G, unsigned long prime);

void initializeBoard(GameBoard* G);
bool getMoves(GameBoard* G, uint64_t* moves,int i, int j);
bool pawnMove(GameBoard* G, uint64_t* moves, int i, int j);
bool bishopMove(GameBoard* G, uint64_t* moves, int i, int j);
bool rookMove(GameBoard* G, uint64_t* moves, int i, int j);
bool queenMove(GameBoard* G, uint64_t* moves, int i, int j);
bool kingMove(GameBoard* G, uint64_t* moves, int i, int j);
bool promotedMove(GameBoard* G, uint64_t* moves, int i, int j);
bool knightMove(GameBoard* G, uint64_t* moves, int i, int j);
bool isCheck(GameBoard* G, bool is_white);
bool isCheckMate(GameBoard* G, bool is_white);
Piece movePiece(GameBoard* G, GameBoard* captured, int* captured_num,int s_i, int s_j, int d_i, int d_j);
void capturePiece(GameBoard* captured, int* captured_count, Piece type, bool is_white);
int positionalScore(GameBoard* G, bool is_white);
int getScore(GameBoard* prev, GameBoard* after, int s_i, int s_j, int d_i, int d_j);
GameBranch* search(Hashmap* H, GameBranch* G,bool is_white, int max_depth);
Piece find_move(GameBoard* before, GameBoard* after, int* s_i, int* s_j, int* d_i, int* d_j);
void printHistory(GameBranch* B, int depth);
void printBoard(GameBoard b);
GameBranch* addToPool(Hashmap* H, GameBoard* G, Turn T);
void deleteGameBranch(GameBranch* g);
GameBranch* newGameBranch(Turn t);
GameBranch* emptyGameBranch();
#endif