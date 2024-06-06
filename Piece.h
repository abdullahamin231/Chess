#ifndef PIECE_H
#define PIECE_H
#include <iostream>
using namespace std;

class Piece {
public:
  int None = 0;
  int King = 1;
  int Pawn = 2;
  int Knight = 3;
  int Bishop = 4;
  int Rook = 5;
  int Queen = 6;

  int White = 8;
  int Black = 16;

  bool isCorrectColor(int piece, int turn) {
    if ((piece & 24) == White && turn == 0)
      return true;
    if ((piece & 24) == Black && turn == 1)
      return true;

    return false;
  }

  bool isBishop(int piece) { return ((piece & 7) == Bishop); }
  bool isRook(int piece) { return ((piece & 7) == Rook); }
  bool isKing(int piece) { return (piece & 7) == King; }
  bool isKnight(int piece) { return (piece & 7) == Knight; }
  bool isPawn(int piece) { return (piece & 7) == Pawn; }

  bool isSliding(int piece) { return (piece & 7) > Knight && (piece & 7) < 7; }
};

inline Piece Piece;

#endif // !PIECE_H
