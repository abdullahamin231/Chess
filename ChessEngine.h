
#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include "Board.h"
#include "Move.h"
#include <cstdlib>
#include <ctime> // Include <ctime> for time()

class ChessEngine {
  Board *board;

public:
  ChessEngine(Board *board) : board(board) {
    // Seed the random number generator only once
    srand(static_cast<unsigned int>(time(0)));
  };

  Move GenerateMove() {
    vector<Move> legalMoves = board->GenerateLegalMoves();
    cout << legalMoves.size() << endl;
    return legalMoves[rand() % legalMoves.size()];
  }
};

#endif // !CHESS_ENGINE_H
