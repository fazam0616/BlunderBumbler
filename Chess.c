#include "Chess.h"

bool (*moveFcns[PIECE_TYPE_COUNT])(GameBoard* ,uint64_t*,int,int) = {&pawnMove, &bishopMove, &rookMove, &knightMove, &queenMove, &kingMove,&promotedMove};
int pieceValues[PIECE_TYPE_COUNT] = {1, 3, 5, 3, 9, 9, 9};
int position_scores[PIECE_TYPE_COUNT][64] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6 
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 1, 1, 2, 2, 1, 1, 0,
        0, 1, 2, 3, 3, 2, 1, 0,
        0, 1, 2, 3, 3, 2, 1, 0,
        0, 1, 1, 2, 2, 1, 1, 0,
        0, 0, 1, 1, 1, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 2, 2, 2, 2, 1, 0,
        0, 1, 2, 3, 3, 2, 1, 0,
        0, 1, 2, 3, 3, 2, 1, 0,
        0, 1, 2, 2, 2, 2, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 4, 4, 4, 4, 1, 0,
        0, 1, 3, 3, 3, 3, 1, 0,
        0, 1, 3, 3, 3, 3, 1, 0,
        0, 1, 4, 4, 4, 4, 1, 0,
        0, 1, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 2, 2, 2, 2, 2, 2, 1,
        1, 3, 3, 3, 3, 3, 3, 1,
        1, 2, 2, 2, 2, 2, 2, 1,
        1, 2, 2, 2, 2, 2, 2, 1,
        1, 3, 3, 3, 3, 3, 3, 1,
        2, 2, 2, 2, 2, 2, 2, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    },
    {
        3, 3, 3, 3, 3, 3, 3, 3,
        2, 2, 2, 2, 2, 2, 2, 2,
        1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    },{
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 2, 2, 2, 2, 2, 2, 1,
        1, 3, 3, 3, 3, 3, 3, 1,
        1, 2, 2, 2, 2, 2, 2, 1,
        1, 2, 2, 2, 2, 2, 2, 1,
        1, 3, 3, 3, 3, 3, 3, 1,
        2, 2, 2, 2, 2, 2, 2, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    }
    };


inline void toggleBitBoardBit(uint64_t* G, int i, int j){
    uint64_t mask = (uint64_t)1 << (i*8 + j);

    *G ^= mask;
}

inline bool getBitBoardBit(uint64_t* G, int i, int j){
    uint64_t mask = ((uint64_t)1 << (i * 8 + j)); 
    return ((*G & mask) != 0) ? 1 : 0;
}

inline int getPieceType(GameBoard* G, int i, int j) {
    int pos = i * 8 + j;
    uint64_t mask = 1ULL << pos;
    if (G->bitboards[PAWN]   & mask) return PAWN;
    if (G->bitboards[BISHOP] & mask) return BISHOP;
    if (G->bitboards[KNIGHT] & mask) return KNIGHT;
    if (G->bitboards[ROOK]   & mask) return ROOK;
    if (G->bitboards[KING]   & mask) return KING;
    if (G->bitboards[QUEEN]  & mask) return QUEEN;
    if (G->bitboards[PROMOTED]  & mask) return PROMOTED;
    return -1;
}



inline uint64_t getBlackMask(GameBoard* G){
    uint64_t all = 0;

    for (int i = 0; i < PIECE_TYPE_COUNT; i++){
        all |= G->bitboards[i];
    } 

    return all & ~G->bitboards[WHITE];
}


inline uint64_t getWhiteMask(GameBoard* G){
    uint64_t all = 0;

    for (int i = 0; i < PIECE_TYPE_COUNT; i++){
        all |= G->bitboards[i];
    } 

    return all & G->bitboards[WHITE];
}

inline bool toggleGameBoardBit(GameBoard* G, int i, int j){
    Piece type = getPieceType(G, i, j);

    if (type >= 0){
        toggleBitBoardBit(&G->bitboards[type], i, j);
        return true;
    }

    return false;
}

inline void setBitBoardBit(uint64_t* G, int i, int j, uint64_t val){
    uint64_t mask = ~((uint64_t)1 << (i*8 + j));
    uint64_t cleared = *G & mask;
    *G = cleared | (val << (i*8 + j));
}

inline void setPieceType(GameBoard* G, Piece type, int i, int j){
    for (int t = 0; t<PIECE_TYPE_COUNT;t++){
        setBitBoardBit(&G->bitboards[t], i, j, t == type);
    }
}

void initializeBoard(GameBoard* G){
    G->bitboards[PAWN] =    0x00FF00000000FF00;
    G->bitboards[BISHOP] =  0x2400000000000024;
    G->bitboards[ROOK] =    0x8100000000000081;
    G->bitboards[KNIGHT] =  0x4200000000000042;
    G->bitboards[QUEEN] =   0x0800000000000008;
    G->bitboards[KING] =    0x1000000000000010;
    G->bitboards[PROMOTED]= 0x0000000000000000;
    G->bitboards[WHITE] =   0xFFFF000000000000;
}

int compare(void* A, void* B){
    if (A && B){
        signed int diff;
        for(int i = 0; i<BITBOARD_COUNT;i++){
            diff = ((GameBranch*)A)->board.bitboards[i] - ((GameBranch*)B)->board.bitboards[i];
            if (diff != 0)
                return diff;
        }
        //Compare the turn at the game state
        if (((GameBranch*)A)->turn != ((GameBranch*)B)->turn){
            if (((GameBranch*)A)->turn == WhiteTurn)
                return 1;
            else   
                return -1;
        }
        else
            return 0;
    }else{
        return A ? -1 : (B ? 1 : 0);
    }
}

uint64_t hash(void* G, unsigned long prime) {
    uint64_t h = 0;  // Initialize hash value
    uint64_t seed = prime;  // Use the prime as a seed
    uint64_t c1 = 0x87c37b91114253d5;
    uint64_t c2 = 0x4cf5ad432745937f;
    GameBranch* branch = (GameBranch*) G;

    for (int i = 0; i < BITBOARD_COUNT; i++) {
        uint64_t k = branch->board.bitboards[i];

        // Mix the key
        k *= c1;
        k = (k << 31) | (k >> (64 - 31));  // Rotate left by 31
        k *= c2;

        // Combine into hash
        h ^= k;
        h = (h << 27) | (h >> (64 - 27));  // Rotate left by 27
        h = h * 5 + 0x52dce729;
    }

    // Incorporate the turn into the hash:
    // Use one constant if it's WhiteTurn and another if it's BlackTurn.
    uint64_t turnConst = (branch->turn == WhiteTurn) 
        ? 0xA5A5A5A5A5A5A5A5ULL 
        : 0x5A5A5A5A5A5A5A5ULL;
    h ^= turnConst;

    // Finalize hash
    h ^= seed;
    h ^= (h >> 33);
    h *= 0xff51afd7ed558ccdULL;
    h ^= (h >> 33);
    h *= 0xc4ceb9fe1a85ec53ULL;
    h ^= (h >> 33);

    return h;
}


bool pawnMove(GameBoard* G, uint64_t* moves, int i, int j){
    int is_white = getBitBoardBit(&G->bitboards[WHITE], i, j);
    bool isBegin = is_white ? (i == 6) : (i == 1);
    bool returnVal = false;
    int dir = is_white ? -1 : 1;
    int length = (isBegin ? 3:2);
    bool front_blocked = false;

    
    for (int q = 1; q < length; q++){
        int posy = i + dir*q;
        if (posy >= 0 && posy < 8){
            if ((getPieceType(G, posy, j) < 0) && !front_blocked){
                setBitBoardBit(moves, posy, j, 1);
                returnVal = true;
            }
            else if (q == 1){
                front_blocked = true;
            }
            
            if (q == 1){
                for (int side = -1; side < 2; side += 2){
                    int j_side = j+side;
                    if (j_side >= 0 && j_side < 8){
                        if (getBitBoardBit(&G->bitboards[WHITE], posy, j_side) != is_white && getPieceType(G, posy, j_side) >= 0){
                            setBitBoardBit(moves, posy, j_side, 1);
                            returnVal = true;
                        }
                    }
                }
            }
        }
    }

    // for (int q = 1; q < length; q++){
    //     int posy = i + dir*q;
    //     if (posy >= 0 && posy < 8){
    //         if ((type = getPieceType(G, posy, j)) < 0 || getBitBoardBit(&G->bitboards[WHITE], posy, j) != is_white){
    //             //Nothing in front
    //             if (type < 0){
    //                 setBitBoardBit(moves, posy, j, 1);
    //                 returnVal = true;
    //             }else{

    //             }
                
    //             if (q == 1){// Can't kill people when double stepping
    //                 for (int side = -1; side < 2; side += 2){
    //                     int j_side = j+side;
    //                     if (j_side >= 0 && j_side < 8){
    //                         if (getBitBoardBit(&G->bitboards[WHITE], posy, j_side) != is_white && getPieceType(G, posy, j_side) >= 0){
    //                             setBitBoardBit(moves, posy, j_side, 1);
    //                             returnVal = true;
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //         else{
    //             break;
    //         }
    //     }
    // }

    return returnVal;
}

bool bishopMove(GameBoard* G, uint64_t* moves, int i, int j){
    int is_white = getBitBoardBit(&G->bitboards[WHITE], i, j);
    int dirx;
    int diry;
    bool returnVal = false;
    Piece type;

    for (int z = 0; z<4;z++){
        for (int q = 1; q < 8; q++){
            dirx = z < 2 ? 1 : -1;
            diry = (z % 2 == 1) ? 1 : -1;
            int posx = j + dirx*q;
            int posy = i + diry*q;
            if (posy >= 0 && posy < 8 && posx >= 0 && posx < 8){
                if ((type = getPieceType(G, posy, posx)) < 0 || getBitBoardBit(&G->bitboards[WHITE], posy, posx) != is_white){
                    setBitBoardBit(moves, posy, posx, 1);
                    returnVal = true;
                    if (type >= 0){
                        break;
                    }
                }else{
                    break;
                }
            }else{
                break;
            }
        }
    }
    return returnVal;
}

bool rookMove(GameBoard* G, uint64_t* moves, int i, int j){
    int is_white = getBitBoardBit(&G->bitboards[WHITE], i, j);
    int dir;
    bool returnVal = false;
    Piece type;

    for (int z = 0; z<4;z++){
        for (int q = 1; q < 8; q++){
            dir = z < 2 ? 1 : -1;
            int posx = j;
            int posy = i;
            if (z % 2 == 1){
                posx += dir*q;
            }else{
                posy += dir*q;
            }


            if (posy >= 0 && posy < 8 && posx >= 0 && posx < 8){
                if ((type = getPieceType(G, posy, posx)) < 0 || getBitBoardBit(&G->bitboards[WHITE], posy, posx) != is_white){
                    setBitBoardBit(moves, posy, posx, 1);
                    returnVal = true;
                    if (type >= 0){
                        break;
                    }
                }else{
                    break;
                }
            }else{
                break;
            }
        }
    }
    return returnVal;
}

bool kingMove (GameBoard* G, uint64_t* moves, int i, int j){
    int offsets[8][2] = {
        {-1, -1}, {-1, 0}, // Up two, left/right one
        {-1, 1}, {0, 1},   // Down two, left/right one
        {1, 1}, {1, 0}, // Up one, left/right two
        {1, -1}, {0, -1}    // Down one, left/right two
    };
    int is_white = getBitBoardBit(&G->bitboards[WHITE], i, j);
    int dir;
    bool returnVal = false;

    for (int q = 0; q<8;q++){  
        int posy = i + offsets[q][0];
        int posx = j + offsets[q][1];

        if (posy >= 0 && posy < 8 && posx >= 0 && posx < 8){
            if (getPieceType(G, posy, posx) < 0 || getBitBoardBit(&G->bitboards[WHITE], posy, posx) != is_white){
                setBitBoardBit(moves, posy, posx, 1);
                returnVal = true;
            }
        }
    }

    return returnVal;
}

bool queenMove(GameBoard* G, uint64_t* moves, int i, int j){
    bool returnVal = rookMove(G, moves, i, j);

    return returnVal | bishopMove(G, moves, i, j);
}

bool promotedMove(GameBoard* G, uint64_t* moves, int i, int j){
    return queenMove(G, moves, i, j);
}

bool knightMove(GameBoard* G, uint64_t* moves, int i, int j){
    // Define all possible knight moves as pairs of (row offset, col offset)
    int offsets[8][2] = {
        {-2, -1}, {-2, 1}, // Up two, left/right one
        {2, -1}, {2, 1},   // Down two, left/right one
        {-1, -2}, {-1, 2}, // Up one, left/right two
        {1, -2}, {1, 2}    // Down one, left/right two
    };

    int sign;
    int is_white = getBitBoardBit(&G->bitboards[WHITE], i, j);
    bool returnVal = false;

    for (int q = 0; q < 8; q++){        
        int posy = i + offsets[q][0];
        int posx = j + offsets[q][1];

        if (posx >= 0 && posx < 8 && posy >= 0 && posy < 8){
            if (getPieceType(G, posy, posx) < 0 || getBitBoardBit(&G->bitboards[WHITE], posy, posx) != is_white){
                setBitBoardBit(moves, posy, posx, 1);
                returnVal = true;
            }
        }   
    }
    
    return returnVal;
}

bool getMoves(GameBoard* G, uint64_t* moves,int i, int j){
    Piece type = getPieceType(G, i, j);

    if (type < 0)
        return false;

    return moveFcns[type](G, moves, i, j);
}

void capturePiece(GameBoard* captured, int* captured_count, Piece type, bool is_white){
    setBitBoardBit(&captured->bitboards[type], *captured_count / 8, *captured_count % 8, 1);
    if (is_white)
        setBitBoardBit(&captured->bitboards[WHITE], *captured_count / 8, *captured_count % 8, 1);
    else
        setBitBoardBit(&captured->bitboards[WHITE], *captured_count / 8, *captured_count % 8, 0);

    *captured_count += 1;
}

Piece movePiece(GameBoard* G, GameBoard* captured, int* captured_num,int s_i, int s_j, int d_i, int d_j){
    Piece type1 = getPieceType(G, s_i, s_j);
    Piece type2 = getPieceType(G, d_i, d_j);
    bool s_white = getBitBoardBit(&G->bitboards[WHITE], s_i, s_j);
    bool promoted = type1 == PAWN && (s_white ? d_i == 0 : d_i == 7);

    if (type1 >= 0){
        setBitBoardBit(&G->bitboards[type1], s_i, s_j, 0);
        if (s_white)
            setBitBoardBit(&G->bitboards[WHITE], s_i, s_j, 0);
    }

    if (type2 >= 0){
        setBitBoardBit(&G->bitboards[type2], d_i, d_j, 0);
        capturePiece(captured, captured_num,type2, !s_white);
    }

    setBitBoardBit(&G->bitboards[type1], d_i, d_j, 1);
    if (s_white)
        setBitBoardBit(&G->bitboards[WHITE], d_i, d_j, 1);
    else
        setBitBoardBit(&G->bitboards[WHITE], d_i, d_j, 0);

    //Promote pawn to queen
    if (promoted){
        setBitBoardBit(&G->bitboards[PAWN], d_i, d_j, 0);
        setBitBoardBit(&G->bitboards[PROMOTED], d_i, d_j, 1);
    }

    return type2;
}


void printBoard(GameBoard b){
    printf("--------\n");
    for (int i = 0; i<8; i++){
        for (int j = 0; j< 8; j++){
            if (getBitBoardBit(&b.bitboards[WHITE], i, j)){
                switch (getPieceType(&b,i, j))
                {
                case -1:
                    printf(" ");
                    break;
                
                case PAWN:
                    printf("P");
                    break;
                case BISHOP:
                    printf("B");
                    break;
                case ROOK:
                    printf("R");
                    break;
                case KNIGHT:
                    printf("H");
                    break;
                case KING:
                    printf("K");
                    break;
                case PROMOTED:
                case QUEEN:
                    printf("Q");
                    break;
                }
            }else{
                switch (getPieceType(&b,i, j))
                {
                case -1:
                    printf(" ");
                    break;
                
                case PAWN:
                    printf("p");
                    break;
                case BISHOP:
                    printf("b");
                    break;
                case ROOK:
                    printf("r");
                    break;
                case KNIGHT:
                    printf("h");
                    break;
                case KING:
                    printf("k");
                    break;
                case PROMOTED:
                case QUEEN:
                    printf("q");
                    break;
                }
            }
        }
        printf("\n");
    }
    printf("--------\n");
}

Piece find_move(GameBoard* before, GameBoard* after, int* s_i, int* s_j, int* d_i, int* d_j) {
    // Initialize source and destination coordinates
    *s_i = *s_j = *d_i = *d_j = -1;

    // Combine all bitboards to get the occupied squares for both before and after states
    uint64_t before_occupied = 0;
    uint64_t after_occupied = 0;
    GameBoard diff;
    uint64_t changed_positions = 0;
    int moved_type = -1;
    Piece captured = -1;


    for (int i = 0; i < PIECE_TYPE_COUNT; i++) {
        diff.bitboards[i] = before->bitboards[i] ^ after->bitboards[i];
        changed_positions |= diff.bitboards[i];
        after_occupied |= after->bitboards[i];
        before_occupied |= before->bitboards[i];
    }
    // printBoard(*after);
    // printBoard(*before);

    // XOR of before and after occupied squares gives changed positions
    // uint64_t changed_positions = before_occupied ^ after_occupied;

    // Find destination square (bit set in after but not in before)
    uint64_t added_positions = after_occupied & changed_positions;

    // Find source square (bit set in before but not in after)
    uint64_t removed_positions = before_occupied & changed_positions;

    
    if (__builtin_popcountll(added_positions) > 0) {
        int pos = __builtin_ffsll(added_positions) - 1;  // Find the first set bit (0-indexed)
        *d_i = pos / 8;
        *d_j = pos % 8;
        moved_type = getPieceType(after, *d_i, *d_j);
    }

    //Check if a piece captured a instance of it's own class, in a different colour
    else {
        // Compute the XOR of the white bitboards to detect differences.
        uint64_t white_diff = before->bitboards[WHITE] ^ after->bitboards[WHITE];
        if (__builtin_popcountll(white_diff) > 0) {
            int pos = __builtin_ffsll(white_diff) - 1;
            *d_i = pos / 8;
            *d_j = pos % 8;
            captured = getPieceType(after, *d_i, *d_j);
        }
    }

    if (__builtin_popcountll(removed_positions) > 0) {
        int pos = __builtin_ffsll(removed_positions) - 1;  // Find the first set bit (0-indexed)
        *s_i = pos / 8;
        *s_j = pos % 8;
    }

    // Check for capture: Find the piece type removed at the destination square
    for (int i = 0; i < PIECE_TYPE_COUNT && captured < 0; i++) {
        if ((before->bitboards[i] & added_positions) != 0) {
            captured = i;  // Return the type of the piece captured
        }
    }

    return captured;
}

bool isCheckMate(GameBoard* G, bool is_white) {
    // Build a bitboard of defending pieces.
    // For white, use the white mask directly.
    // For black, assume the union of all pieces minus the white mask.
    uint64_t defending;
    uint64_t allPieces = 0;
    for (int p = PAWN; p < PIECE_TYPE_COUNT; p++) {
        allPieces |= G->bitboards[p];
    }
    if (is_white) {
        defending = allPieces & G->bitboards[WHITE];
    } else {  
        defending = allPieces & ~(G->bitboards[WHITE]);
    }

    GameBoard simulated_board;
    GameBoard temp_board;
    int temp_num;

    // Iterate over each defending piece using __builtin_ffsll.
    while (defending) {
        // __builtin_ffsll returns 1-indexed position of least significant set bit.
        int src_index = __builtin_ffsll(defending) - 1; 
        // Clear the bit we just processed.
        defending &= defending - 1; 
        
        int s_i = src_index / 8;
        int s_j = src_index % 8;
        
        uint64_t moves = 0;
        if (getMoves(G, &moves, s_i, s_j)) {
            uint64_t moves_copy = moves;
            // Iterate over all moves available from this piece.
            while (moves_copy) {
                int move_index = __builtin_ffsll(moves_copy) - 1;
                moves_copy &= moves_copy - 1;
                
                int d_i = move_index / 8;
                int d_j = move_index % 8;
                
                // Simulate the move.
                memcpy(&simulated_board, G, sizeof(GameBoard));
                movePiece(&simulated_board, &temp_board, &temp_num, s_i, s_j, d_i, d_j);
                
                // If a move exists that escapes check, it's not checkmate.
                if (!isCheck(&simulated_board, is_white))
                    return false;
            }
        }
    }
    return true;
}


/*
Reads board as is, and sees if [is_white ? White : Black ] is currently in check
Normally assumes that the current turn has not taken place yet, although checkmate
uses it to check the validity of a post-check move
*/
bool isCheck(GameBoard* G, bool is_white) {
    // The defending king’s position (only the king of the defending color)
    uint64_t def_color_mask = is_white ? getWhiteMask(G) : getBlackMask(G);
    uint64_t def_king = G->bitboards[KING] & def_color_mask;
    
    // Build a bitboard of all pieces (excluding the WHITE color bitboard which is only for color info).
    // Here we assume your piece types range from PAWN up to PIECE_TYPE_COUNT.
    uint64_t allPieces = G->bitboards[PAWN] | G->bitboards[ROOK] | G->bitboards[BISHOP] | G->bitboards[KNIGHT]
                         | G->bitboards[KING] | G->bitboards[QUEEN] | G->bitboards[PROMOTED];
    
    // Determine the attacker mask:
    // If the defending side is white (is_white == true), attackers are black pieces (i.e. not set in G->bitboards[WHITE]).
    // Otherwise, attackers are white pieces.
    uint64_t attackers;
    attackers = ~def_color_mask;
    
    uint64_t moves = 0;
    uint64_t tmp;
    
    // Iterate over every set bit in the 'attackers' bitboard using __builtin_ffsll.
    // __builtin_ffsll returns the position (1-indexed) of the least significant set bit.
    // Subtract 1 to get a 0-indexed square number.
    while (attackers) {
        int index = __builtin_ffsll(attackers) - 1;  // Find the first set bit
        // Clear that bit from attackers so we don't process it again.
        attackers &= attackers - 1;
        
        int pos_i = index / 8;
        int pos_j = index % 8;
        
        tmp = 0;
        if (getMoves(G, &tmp, pos_i, pos_j)) {
            moves |= tmp;
            // If the defending king’s square is among these moves, return immediately.
            if ((def_king & moves) != 0)
                return true;
        }
    }
    
    return (def_king & moves) != 0;
}


int calc_score(GameBoard* prev, GameBoard* after, int s_i, int s_j, int d_i, int d_j, bool is_white){
    int score = 0;
    GameBoard simulate = *prev;
    GameBoard sim_captured;
    memset(&sim_captured, 0, sizeof(GameBoard));
    int sim_captured_num;
    int type1;
    int type2;
    bool in_check = false;
    int min_max = is_white ? 1 : -1;

    if (isCheck(after, is_white)){
        // printf("In check\n");
        return -500 * min_max;
    }
    if (isCheck(prev, is_white)){
        //Implies we were in check, but the move get's us out of it
        score += 50 * (min_max);
        in_check = true;
    }


    
    // Piece captured = movePiece(&simulate, &sim_captured, &sim_captured_num, s_i, s_j, d_i, d_j);
    // if (captured >= 0)
    //     // if (getBitBoardBit(&prev->bitboards[WHITE], d_i, d_j) != is_white)
    //         score -= pieceValues[captured] * min_max;
    //     // else
    //     //     score -= pieceValues[captured];

    if (!in_check){
        score += positionalScore(after, is_white) * min_max;

        //Reward putting opponent in check / checkmate
        if (isCheck(after, !is_white)){
            if (isCheckMate(after, !is_white)){
                score += 50000 * min_max;
            }else{
                score += 25 * min_max;
            }
        }

        //Reward promoting pawns
        // type1 = getPieceType(prev, s_i, s_j);
        // if (type1 == PAWN && (type2 = getPieceType(after, d_i, d_j)) == PROMOTED){
        //     if (getBitBoardBit(&prev->bitboards[WHITE], d_i, d_j) == is_white)
        //         score += 10 * min_max;
        //     else   
        //         score -= 10 * min_max;
        // }
    }

    return score;
}

int positionalScore(GameBoard* G, bool is_white){
    int score = 0;
    Piece type;

    for (int i = 0; i<8;i++){
        for (int j = 0; j<8;j++){
            if ((type=getPieceType(G, i, j)) >= 0 && getBitBoardBit(&G->bitboards[WHITE], i, j) == is_white){
                score += pieceValues[type]; //+ (is_white ?position_scores[type][(7-i)*8+j] : position_scores[type][i*8+j]);
            }
        }
    }

    return score;
}



int getScore(GameBoard* prev, GameBoard* after, int s_i, int s_j, int d_i, int d_j){
    bool is_white = getBitBoardBit(&prev->bitboards[WHITE], s_i, s_j);
    return calc_score(prev, after, s_i, s_j, d_i, d_j, is_white); //- calc_score(prev, after, s_i, s_j, d_i, d_j, !is_white);
}

GameBranch* emptyGameBranch(){
    GameBranch* g = malloc(sizeof(GameBranch));
    
    return g;
}

int score_compare(void* A, void* B){
    GameBranch* b_a = A;
    GameBranch* b_b = B;

    return b_b->score - b_a->score;
}

GameBranch* newGameBranch(Turn t){
    GameBranch* g = emptyGameBranch();
    if (!g){
        printf("Failed to allocate new game branch\n");
        return NULL;
    }
    g ->possible_branches = new_tree(&score_compare, NULL);
    g->turn = t;
    g->depth = 0;
    g->beta = INT_MAX;
    g->alpha = INT_MIN;
    g->best = NULL;
    g->lru_prev = NULL;
    g->lru_next = NULL;
    return g;
}

void deleteGameBranch(GameBranch* g){
    if (g){
        delete_tree(g->possible_branches);
        if (g->lru_prev)
            g->lru_prev->lru_next = g->lru_next;
        if (g->lru_next)
            g->lru_next->lru_prev = g->lru_prev;

        free(g);
    }
}

GameBranch* addToPool(Hashmap* H, GameBoard* G, Turn t){
    GameBranch* empty = newGameBranch(t);
    
    memcpy(&empty->board, G, sizeof(GameBoard));
    empty->turn = t;
    GameBranch* stored = NULL;

    if (!(stored = hash_find(H, empty))){
        stored = empty;
        empty->depth = -1;
        hash_insert(H, empty);
    }else{
        deleteGameBranch(empty);
    }
    return stored;
}


/*
H           The hashmap that stores all the game branches
G           The gamebranch to search from
is_white    Whether WHITE is the Max turn (true) or the min term (false)
*/
GameBranch* search(Hashmap* H, GameBranch* G,bool is_white, int max_depth){
    uint64_t color_mask = G->turn == WhiteTurn ? getWhiteMask(&G->board) : getBlackMask(&G->board);
    uint64_t all = 0;
    uint64_t moves;
    Piece type;
    GameBoard simulated_board;
    GameBoard simulated_captured;
    int captured_num;
    GameBranch* new_branch;
    GameBranch *stored_branch;
    int added_branches = 0;
    int revisited_branch = 0;
    bool MAX;

    if (max_depth <= 0)
        return G;
    
        
    if (!G->possible_branches)
        G->possible_branches = new_tree(&score_compare, NULL);

    if (is_white){ 
        MAX = G->turn == WhiteTurn;
    }else{
        MAX = G->turn == BlackTurn;
    }
    bool generate_moves = true;
    int tries = 10;
    
    if (count_nodes(G->possible_branches) <= 1){
        for (int s_j = 0; s_j < 8 && generate_moves; s_j++) { 
            for (int s_i = 0; s_i < 8 && generate_moves; s_i++) {
                bool is_right_colour = getBitBoardBit(&color_mask, s_i, s_j);
                if ((type = getPieceType(&G->board, s_i, s_j)) >= 0 && is_right_colour) {
                    uint64_t moves = 0;
                    getMoves(&G->board, &moves, s_i, s_j);

                    if (moves != 0) {
                        for (int d_j = 0; d_j < 8 && generate_moves; d_j++) {
                            for (int d_i = 0; d_i < 8 && generate_moves; d_i++) {
                                if (getBitBoardBit(&moves, d_i, d_j)) {
                                    GameBoard simulated_board;
                                    GameBoard simulated_captured;
                                    int captured_num=0;
                                    
                                    memcpy(&simulated_board, &G->board, sizeof(GameBoard));
                                    movePiece(&simulated_board, &simulated_captured, &captured_num, s_i, s_j, d_i, d_j);
                                    
                                    // memset(&lookup_branch, 0, sizeof(GameBranch));

                                    stored_branch = addToPool(H, &simulated_board, G->turn == WhiteTurn ? BlackTurn : WhiteTurn);
                                    if (stored_branch->depth < 0){
                                        stored_branch->depth = G->depth + 1;
                                        stored_branch->prev_branch = G;
                                        //Set score before searching in case depth-floor is reached
                                        stored_branch->board_score = getScore(&G->board, &simulated_board, s_i, s_j, d_i, d_j);
                                        stored_branch->score = stored_branch->board_score + search(H, stored_branch, is_white, max_depth - 1)->score;
                                        stored_branch->alpha = G->alpha;
                                        stored_branch->beta = G->beta;
                                        added_branches++;
                                    }else{
                                        revisited_branch++;
                                    }
                                    tree_insert(G->possible_branches, stored_branch);

                                    

                                    if (MAX){
                                        if (!G->best || G->best->score < stored_branch->score){
                                            G->best = stored_branch;
                                            G->score = G->board_score + G->best->score;
                                        }

                                        G->alpha = fmax(G->alpha, G->best->score);
                        
                                        //Prune rest of tree
                                        if (G->best->score >= G->beta){
                                            G->best = stored_branch;
                                            G->score = G->board_score + G->best->score;
                                            // printf("Alpha prune\n");
                                            return G->best;
                                        }
                                    }
                                    else{
                                        if (!G->best || G->best->score > stored_branch->score){
                                            G->best = stored_branch;
                                            G->score = G->board_score + stored_branch->score;
                                        }
                                        
                                        G->beta = fmin( G->beta, G->best->score);
                        
                                        //Prune rest of tree
                                        if (G->best->score <= G->alpha){
                                            G->best = stored_branch;
                                            G->score = stored_branch->board_score + stored_branch->score;
                                            // printf("Beta prune\n");
                                            return G->best;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    }
    // printf("Added %d new branches\n", added_branches);
    // printf("Revisited %d old branches\n\n", revisited_branch); 

    if (!G->best) {
        Tree* succ = G->possible_branches;
        if (MAX)
            while (succ->right != NULL&& succ->right->data != NULL)
                succ = succ->right;
        else 
            while (succ->left != NULL && succ->left->data != NULL)
                succ = succ->left;

        G->best = succ->data;
        G->score = stored_branch->board_score + G->best->score;
    }
    return G->best;
    
}

const char* piece_names[PIECE_TYPE_COUNT+1] = {"pawn", "bishop", "rook", "knight", "queen", "king", "promoted"};

void printHistory(GameBranch* B, int depth){
    int s_i;int s_j; int d_i; int d_j;
    Piece taken;
    Piece moved;
    if (B){
        if (B->turn == WhiteTurn)
            printf("White/Capital (bottom) turn\n");
        else
            printf("Black/Lower-Case (top) turn\n");

        printBoard(B->board);
        if (depth > 0 && B){
            if(B->best){
                taken = find_move(&B->board, &B->best->board, &s_i, &s_j, &d_i, &d_j);
                moved = getPieceType(&B->board, s_i, s_j);
                printf("%s(%d, %d) to (%d, %d)", piece_names[moved], s_j, s_i, d_j, d_i);
                if (taken >= 0){
                    printf(", capturing a %s\n", piece_names[taken]);
                }else{
                    printf("\n");
                }
            }
            printHistory(B->best, depth - 1);
        }
    }
}