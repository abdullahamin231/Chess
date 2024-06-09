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

  bool sameColor(int piece, int color) {
    int piece_color = piece & 24;
    return piece_color == color;
  }
  bool isCorrectColor(int piece, int turn) {
    if (((piece & 24) == White) && (turn == 0))
      return true;
    if (((piece & 24) == Black) && (turn == 1))
      return true;

    return false;
  }

  void name(int piece) {
    string color = (piece & 24) == White   ? "White "
                   : (piece & 24) == Black ? "Black"
                                           : "None";
    string pieceName;

    if (isBishop(piece)) {
      pieceName = "Bishop";
    } else if (isRook(piece)) {
      pieceName = "Rook";
    } else if (isKing(piece)) {
      pieceName = "King";
    } else if (isPawn(piece)) {
      pieceName = "Pawn";
    } else if (isQueen(piece)) {
      pieceName = "Queen";
    }

    cout << color << pieceName;
  }
  bool isQueen(int piece) { return ((piece & 7) == Queen); }
  bool isBishop(int piece) { return ((piece & 7) == Bishop); }
  bool isRook(int piece) { return ((piece & 7) == Rook); }
  bool isKing(int piece) { return (piece & 7) == King; }
  bool isKnight(int piece) { return (piece & 7) == Knight; }
  bool isPawn(int piece) { return (piece & 7) == Pawn; }

  bool isSliding(int piece) {
    return ((piece & 7) == Bishop) || ((piece & 7) == Rook) ||
           ((piece & 7) == Queen);
  }
};

inline Piece Piece;

#endif // !PIECE_H
