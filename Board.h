
#ifndef BOARD_H
#define BOARD_H

#include "Move.h"
#include "Piece.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;

class Board {
public:
  Board() { PrecomputedMoveData(); }

  void print() { cout << "Turn: " << (turn == 0 ? "White" : "Black") << endl; }
  void checkStatus() {
    bool w = this->hasWon();
    cout << "Won: " << w << endl;
  }

  void PrecomputedMoveData();

  bool isWhiteTurn() { return turn == 0; }
  bool isWhitePiece(int piece) { return (piece & 24) == 8; }
  int getPickedPiece() { return picked[1] * 8 + picked[0]; }
  int getPicked() { return picked_piece; }
  void can_castle(int color, bool &can_left, bool &can_right);

  bool in_check(bool otherColor = 0);
  bool isMoveLegal(int startSquare, int targetSquare);

  void GenerateSlidingMovesN(vector<Move> &Moves, int startSquare, int piece);
  void GenerateSlidingMovesC(vector<Move> &Moves, int startSquare, int piece);

  void GeneratePawnMovesN(vector<Move> &Moves, int startSquare, int piece);
  void GeneratePawnMovesC(vector<Move> &Moves, int startSquare, int piece);

  void GenerateKingMovesN(vector<Move> &Moves, int startSquare, int piece);
  void GenerateKingMovesC(vector<Move> &Moves, int startSquare, int piece);

  void GenerateKnightMovesN(vector<Move> &Moves, int startSquare, int piece);
  void GenerateKnightMovesC(vector<Move> &Moves, int startSquare, int piece);

  vector<Move> GenerateAllMoves(bool otherColor = 0);
  vector<Move> GenerateForPiece(int piece, int startSquare);

  bool pickup(int x, int y, int &temp_picked);
  void place(int x, int y);
  bool legalPlace(int x, int y);
  bool hasWon();

  int *getSquares() { return this->Squares; }
  void loadFromFen(string fen);

public:
  int Squares[64] = {0};
  int copySquares[64] = {0};
  int picked_piece = Piece.None;
  int turn = 0; // 0 for white's turn, 1 for black's turn
  int picked[2] = {-1, -1};

  int directionOffsets[8] = {8, -8, -1, 1, 7, -7, 9, -9};
  int NumSquaresToEdge[64][8];

  int enpassant_square = -1;
  int white_castle[5] = {0, 0, 0, 58, 62};
  int black_castle[5] = {0, 0, 0, 2, 6};

  int white_king_square = 60, black_king_square = 4;

  bool won = false;
  int moveCount = 0;

  void updateCopy() {
    for (int i = 0; i < 64; ++i) {
      copySquares[i] = Squares[i];
    }
  }

  void copyFromCopy() {
    for (int i = 0; i < 64; ++i) {
      Squares[i] = copySquares[i];
    }
  }
};

inline void Board::PrecomputedMoveData() {
  for (int file = 0; file < 8; file++) {
    for (int rank = 0; rank < 8; rank++) {
      int squareIndex = rank * 8 + file;

      int numNorth = 7 - rank;
      int numSouth = rank;
      int numWest = file;
      int numEast = 7 - file;

      NumSquaresToEdge[squareIndex][0] = numNorth; // North
      NumSquaresToEdge[squareIndex][1] = numSouth; // South
      NumSquaresToEdge[squareIndex][2] = numWest;  // West
      NumSquaresToEdge[squareIndex][3] = numEast;  // East
      NumSquaresToEdge[squareIndex][4] =
          std::min(numNorth, numWest); // North-West
      NumSquaresToEdge[squareIndex][5] =
          std::min(numSouth, numEast); // South-East
      NumSquaresToEdge[squareIndex][6] =
          std::min(numNorth, numEast); // North-East
      NumSquaresToEdge[squareIndex][7] =
          std::min(numSouth, numWest); // South-West
    }
  }
}

inline void Board::GenerateSlidingMovesC(vector<Move> &Moves, int startSquare,
                                         int piece) {

  int startDirIndex = Piece.isBishop(piece) ? 4 : 0;
  int endDirIndex = Piece.isRook(piece) ? 4 : 8;

  for (int directionIndex = startDirIndex; directionIndex < endDirIndex;
       directionIndex++) {
    for (int n = 0; n < NumSquaresToEdge[startSquare][directionIndex]; n++) {
      int targetSquare =
          startSquare + directionOffsets[directionIndex] * (n + 1);

      // Additional check for horizontal moves to ensure they don't wrap around
      // rows
      int pieceOnTargetSquare = Squares[targetSquare];

      // If the target square is occupied by a piece of the same color, stop
      if (Piece.isCorrectColor(pieceOnTargetSquare, turn)) {
        break;
      }

      // Add the move to the list of possible moves
      Moves.push_back(Move(startSquare, targetSquare));

      bool legal_move = isMoveLegal(startSquare, targetSquare);
      if (!legal_move)
        Moves.pop_back();

      // If the target square is occupied by an opponent's piece, stop
      if (Piece.isCorrectColor(pieceOnTargetSquare, !turn)) {
        break;
      }
    }
  }
}
inline void Board::GenerateSlidingMovesN(vector<Move> &Moves, int startSquare,
                                         int piece) {
  int startDirIndex = Piece.isBishop(piece) ? 4 : 0;
  int endDirIndex = Piece.isRook(piece) ? 4 : 8;

  for (int directionIndex = startDirIndex; directionIndex < endDirIndex;
       directionIndex++) {
    for (int n = 0; n < NumSquaresToEdge[startSquare][directionIndex]; n++) {
      int targetSquare =
          startSquare + directionOffsets[directionIndex] * (n + 1);

      // Additional check for horizontal moves to ensure they don't wrap around
      // rows
      int pieceOnTargetSquare = Squares[targetSquare];

      // If the target square is occupied by a piece of the same color, stop
      if (Piece.isCorrectColor(pieceOnTargetSquare, turn)) {
        break;
      }

      // Add the move to the list of possible moves
      Moves.push_back(Move(startSquare, targetSquare));

      // bool legal_move = isMoveLegal(startSquare, targetSquare);
      //  if(!legal_move) Moves.pop_back();

      // If the target square is occupied by an opponent's piece, stop
      if (Piece.isCorrectColor(pieceOnTargetSquare, !turn)) {
        break;
      }
    }
  }
}

inline void Board::GeneratePawnMovesC(vector<Move> &Moves, int startSquare,
                                      int piece) {

  bool isWhite = isWhitePiece(piece);
  int direction = isWhite ? -8 : 8;
  int startRow = isWhite ? 6 : 1;
  int enPassantRow = isWhite ? 3 : 4;

  // Normal one square move
  int targetSquare = startSquare + direction;
  if (Squares[targetSquare] == Piece.None) {

    Moves.push_back(Move(startSquare, targetSquare));
    bool legal_move = isMoveLegal(startSquare, targetSquare);

    if (!legal_move && !Moves.empty())
      Moves.pop_back();
    // Double move on first move
    if ((startSquare / 8) == startRow) {
      targetSquare += direction;
      if (Squares[targetSquare] == Piece.None) {
        Moves.push_back(Move(startSquare, targetSquare));
        legal_move = isMoveLegal(startSquare, targetSquare);
        if (!legal_move && !Moves.empty())
          Moves.pop_back();
      }
    }
  }

  // Diagonal Move
  int captureOffsets[2] = {direction + 1, direction - 1};
  for (int offset : captureOffsets) {
    targetSquare = startSquare + offset;

    if (targetSquare >= 0 && targetSquare < 64 &&
        (startSquare % 8 != 0 || offset != direction - 1) &&
        (startSquare % 8 != 7 || offset != direction + 1)) {
      if (Piece.isCorrectColor(Squares[targetSquare], !turn)) {
        Moves.push_back(Move(startSquare, targetSquare));
        bool legal_move = isMoveLegal(startSquare, targetSquare);
        if (!legal_move && !Moves.empty())
          Moves.pop_back();
      }
    }
  }

  if ((startSquare / 8) == enPassantRow) {
    if (abs((startSquare % 8) - (enpassant_square % 8)) == 1) {
      if (Squares[enpassant_square] == Piece.None ||
          Piece.isCorrectColor(Squares[enpassant_square], !turn)) {
        Moves.push_back(Move(startSquare, enpassant_square));
        bool legal_move = isMoveLegal(startSquare, targetSquare);
        if (!legal_move && !Moves.empty())
          Moves.pop_back();
      }
    }
  }
}
inline void Board::GeneratePawnMovesN(vector<Move> &Moves, int startSquare,
                                      int piece) {

  bool isWhite = isWhitePiece(piece);
  int direction = isWhite ? -8 : 8;
  int startRow = isWhite ? 6 : 1;
  int enPassantRow = isWhite ? 3 : 4;

  // Normal one square move
  int targetSquare = startSquare + direction;
  if (Squares[targetSquare] == Piece.None) {
    Moves.push_back(Move(startSquare, targetSquare));
    // Double move on first move
    if ((startSquare / 8) == startRow) {
      targetSquare += direction;
      if (Squares[targetSquare] == Piece.None) {
        Moves.push_back(Move(startSquare, targetSquare));
      }
    }
  }

  // Diagonal Move
  int captureOffsets[2] = {direction + 1, direction - 1};
  for (int offset : captureOffsets) {
    targetSquare = startSquare + offset;

    if (targetSquare >= 0 && targetSquare < 64 &&
        (startSquare % 8 != 0 || offset != direction - 1) &&
        (startSquare % 8 != 7 || offset != direction + 1)) {
      if (Piece.isCorrectColor(Squares[targetSquare], !turn))
        Moves.push_back(Move(startSquare, targetSquare));
    }
  }

  if ((startSquare / 8) == enPassantRow) {
    if (abs((startSquare % 8) - (enpassant_square % 8)) == 1) {
      if (Squares[enpassant_square] == Piece.None ||
          Piece.isCorrectColor(Squares[enpassant_square], !turn)) {
        Moves.push_back(Move(startSquare, enpassant_square));
      }
    }
  }
}

inline void Board::GenerateKingMovesC(vector<Move> &Moves, int startSquare,
                                      int piece) {
  for (int directionIndex = 0; directionIndex < 8; directionIndex++) {
    int n = 1; // King moves only one square

    int targetSquare = startSquare + directionOffsets[directionIndex] * n;
    int pieceOnTargetSquare = Squares[targetSquare];

    // Check if the target square is within the board bounds
    if (targetSquare >= 0 && targetSquare < 64) {
      // If the target square is empty or occupied by an opponent's piece
      if (Piece.isCorrectColor(pieceOnTargetSquare, !turn) ||
          pieceOnTargetSquare == Piece.None) {

        Moves.push_back(Move(startSquare, targetSquare));
        bool legal_move = isMoveLegal(startSquare, targetSquare);
        if (!legal_move)
          Moves.pop_back();
      }
    }
  }

  // 56 57 58 59 60 61 62 63
  bool isWhite = isWhitePiece(piece);
  bool can_left, can_right;
  // Castling logic
  if (isWhite) {
    can_castle(Piece.White, can_left, can_right);
    if (can_left) {
      Moves.push_back(Move(startSquare, 58));
    }
    if (can_right) {
      Moves.push_back(Move(startSquare, 62));
    }
  } else {
    can_castle(Piece.Black, can_left, can_right);
    if (can_left) {
      Moves.push_back(Move(startSquare, 2));
    }
    if (can_right) {
      Moves.push_back(Move(startSquare, 6));
    }
  }
}
inline void Board::GenerateKingMovesN(vector<Move> &Moves, int startSquare,
                                      int piece) {
  for (int directionIndex = 0; directionIndex < 8; directionIndex++) {
    int n = 1; // King moves only one square

    int targetSquare = startSquare + directionOffsets[directionIndex] * n;
    int pieceOnTargetSquare = Squares[targetSquare];

    // Check if the target square is within the board bounds
    if (targetSquare >= 0 && targetSquare < 64) {
      // If the target square is empty or occupied by an opponent's piece
      if (Piece.isCorrectColor(pieceOnTargetSquare, !turn) ||
          pieceOnTargetSquare == Piece.None) {

        Moves.push_back(Move(startSquare, targetSquare));
      }
    }
  }

  // No need to check for castling when in normal move generator.
  // 56 57 58 59 60 61 62 63
  /*bool isWhite = isWhitePiece(piece);
  bool can_left, can_right;
  // Castling logic
  if (isWhite) {
    can_castle(Piece.White, can_left, can_right);
    if (can_left) {
      Moves.push_back(Move(startSquare, 58));
    }
    if (can_right) {
      Moves.push_back(Move(startSquare, 62));
    }
  } else {
    can_castle(Piece.Black, can_left, can_right);
    if (can_left) {
      Moves.push_back(Move(startSquare, 2));
    }
    if (can_right) {
      Moves.push_back(Move(startSquare, 6));
    }
  }*/
}

inline void Board::can_castle(int color, bool &can_left, bool &can_right) {

  if (color == Piece.White) {
    bool left_rook_moved = white_castle[0];
    bool king_moved = white_castle[1];
    bool right_rook_moved = white_castle[2];
    bool left_b1_empty = Squares[59] == Piece.None;
    bool left_c1_empty = Squares[58] == Piece.None;
    bool left_d1_empty = Squares[57] == Piece.None;
    bool right_f1_empty = Squares[61] == Piece.None;
    bool right_g1_empty = Squares[62] == Piece.None;

    vector<Move> enemy_moves = GenerateAllMoves(true);

    for (auto &move : enemy_moves) {
      if (move.targetSquare == 57)
        left_d1_empty = false;
      if (move.targetSquare == 58)
        left_c1_empty = false;
      if (move.targetSquare == 59)
        left_b1_empty = false;
      if (move.targetSquare == 61)
        right_f1_empty = false;
      if (move.targetSquare == 62)
        right_g1_empty = false;
    }

    can_left = !left_rook_moved && !king_moved && left_b1_empty &&
               left_c1_empty && left_d1_empty;

    can_right =
        !king_moved && !right_rook_moved && right_f1_empty && right_g1_empty;

  } else {
    bool left_rook_moved = black_castle[0];
    bool king_moved = black_castle[1];
    bool right_rook_moved = black_castle[2];
    bool left_b8_empty = Squares[3] == Piece.None;
    bool left_c8_empty = Squares[2] == Piece.None;
    bool left_d8_empty = Squares[1] == Piece.None;
    bool right_f8_empty = Squares[5] == Piece.None;
    bool right_g8_empty = Squares[6] == Piece.None;
    vector<Move> enemy_moves = GenerateAllMoves(true);

    for (auto &move : enemy_moves) {
      if (move.targetSquare == 1)
        left_d8_empty = false;
      if (move.targetSquare == 2)
        left_c8_empty = false;
      if (move.targetSquare == 3)
        left_b8_empty = false;
      if (move.targetSquare == 5)
        right_f8_empty = false;
      if (move.targetSquare == 6)
        right_g8_empty = false;
    }
    can_left = !left_rook_moved && !king_moved && left_b8_empty &&
               left_c8_empty && left_d8_empty;

    can_right =
        !king_moved && !right_rook_moved && right_f8_empty && right_g8_empty;
  }
}

inline void Board::GenerateKnightMovesC(vector<Move> &Moves, int startSquare,
                                        int piece) {

  int dx[] = {2, 2, -2, -2, 1, 1, -1, -1};
  int dy[] = {1, -1, 1, -1, 2, -2, 2, -2};

  for (int i = 0; i < 8; ++i) {
    int newRow = startSquare / 8 + dx[i];
    int newCol = startSquare % 8 + dy[i];
    if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
      int targetSquare = newRow * 8 + newCol;
      if (Piece.isCorrectColor(Squares[targetSquare], turn)) {
        continue;
      }
      Moves.push_back(Move(startSquare, newRow * 8 + newCol));
      bool legal_move = isMoveLegal(startSquare, targetSquare);
      if (!legal_move)
        Moves.pop_back();
      if (Piece.isCorrectColor(Squares[targetSquare], !turn)) {
        continue;
      }
    }
  }
}
inline void Board::GenerateKnightMovesN(vector<Move> &Moves, int startSquare,
                                        int piece) {

  int dx[] = {2, 2, -2, -2, 1, 1, -1, -1};
  int dy[] = {1, -1, 1, -1, 2, -2, 2, -2};

  for (int i = 0; i < 8; ++i) {
    int newRow = startSquare / 8 + dx[i];
    int newCol = startSquare % 8 + dy[i];
    if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
      int targetSquare = newRow * 8 + newCol;
      if (Piece.isCorrectColor(Squares[targetSquare], turn)) {
        continue;
      }
      Moves.push_back(Move(startSquare, newRow * 8 + newCol));
      if (Piece.isCorrectColor(Squares[targetSquare], !turn)) {
        continue;
      }
    }
  }
}

inline vector<Move> Board::GenerateAllMoves(bool ofEnemy) {

  vector<Move> Moves;
  // if otherColor == true, turn is switched in order to find all possible moves
  // of enemy this is used in calculating whether player is in check or not
  if (ofEnemy) {
    turn = !turn;
  }
  for (int startSquare = 0; startSquare < 64; startSquare++) {
    int piece = Squares[startSquare];

    if (Piece.isCorrectColor(piece, turn)) {
      if (Piece.isSliding(piece)) {
        GenerateSlidingMovesN(Moves, startSquare, piece);
      } else if (Piece.isKing(piece)) {
        GenerateKingMovesN(Moves, startSquare, piece);
      } else if (Piece.isKnight(piece)) {
        GenerateKnightMovesN(Moves, startSquare, piece);
      } else if (Piece.isPawn(piece)) {
        GeneratePawnMovesN(Moves, startSquare, piece);
      }
    }
  }
  if (ofEnemy) {
    turn = !turn;
  }

  return Moves;
}

inline vector<Move> Board::GenerateForPiece(int piece, int startSquare) {
  vector<Move> Moves;
  bool isKingInCheck = in_check();
  if (Piece.isSliding(piece)) {
    GenerateSlidingMovesC(Moves, startSquare, piece);
  } else if (Piece.isPawn(piece)) {
    GeneratePawnMovesC(Moves, startSquare, piece);
  } else if (Piece.isKing(piece)) {
    GenerateKingMovesC(Moves, startSquare, piece);
  } else if (Piece.isKnight(piece)) {
    GenerateKnightMovesC(Moves, startSquare, piece);
  }
  return Moves;
}

inline bool Board::isMoveLegal(int startSquare, int targetSquare) {
  updateCopy(); // save board state
  Squares[startSquare] = Piece.None;
  Squares[targetSquare] = picked_piece;

  int king_moved = -1;
  if (Piece.isKing(picked_piece)) {
    if (isWhitePiece(picked_piece)) {
      king_moved = white_king_square;
      white_king_square = targetSquare;
    } else {
      king_moved = black_king_square;
      black_king_square = targetSquare;
    }
  }

  bool still_in_check = in_check();
  copyFromCopy(); // reload board state

  if (king_moved != -1) {
    if (isWhitePiece(picked_piece)) {
      white_king_square = king_moved;
    } else {
      black_king_square = king_moved;
    }
  }

  return !still_in_check;
};
inline bool Board::in_check(bool otherColor) {

  // Generate all possible moves for the ENEMY (i.e don't check for enemy being
  // in check);
  vector<Move> enemy_moves = GenerateAllMoves(true);
  for (auto &move : enemy_moves) {
    if (move.targetSquare ==
        (turn == 0 ? white_king_square : black_king_square)) {
      return true;
    }
  }
  return false;
}

inline bool Board::pickup(int x, int y, int &temp_picked) {
  won = hasWon();
  int index = y * 8 + x;
  this->picked_piece = Squares[index];
  int pickColor = picked_piece & 24;

  // Check if it's the correct player's turn
  if ((isWhiteTurn() && pickColor != Piece.White) ||
      (!isWhiteTurn() && pickColor != Piece.Black)) {
    return false;
  }

  picked[0] = x;
  picked[1] = y;
  temp_picked = picked_piece;
  updateCopy();

  // Update the in_check status after picking up the piece
  this->print();

  return true;
}

inline void Board::place(int x, int y) {

  // Illegal place or placed on startSquare
  if (!legalPlace(x, y) || (y == picked[1] && x == picked[0])) {
    // Invalid move, restore the picked piece to its original place
    Squares[picked[1] * 8 + picked[0]] = picked_piece;
    if (Piece.isKing(picked_piece)) {
      if (isWhitePiece(picked_piece)) {
        white_king_square = picked[1] * 8 + picked[0];
      } else {
        black_king_square = picked[1] * 8 + picked[0];
      }
    }
    return;
  }

  int index = y * 8 + x;
  bool isWhite = isWhitePiece(picked_piece);

  // Handle castling
  if (Piece.isKing(picked_piece)) {
    bool can_left, can_right;
    if (isWhite) {
      can_castle(Piece.White, can_left, can_right);
      if (index == 58 && can_left) { // Castling queenside
        cout << "Castling queenside for white king" << endl;
        white_king_square = 58;
        Squares[58] = Piece.King | Piece.White;
        Squares[59] = Piece.Rook | Piece.White;
        Squares[60] = Piece.None;
        Squares[56] = Piece.None;
        white_castle[0] = white_castle[1] = 1;
        turn = !turn;
        this->moveCount++;
        return;
      } else if (index == 62 && can_right) { // Castling kingside
        cout << "Castling kingside for white king" << endl;
        white_king_square = 62;
        Squares[62] = Piece.King | Piece.White;
        Squares[61] = Piece.Rook | Piece.White;
        Squares[60] = Piece.None;
        Squares[63] = Piece.None;
        white_castle[1] = white_castle[2] = 1;
        this->moveCount++;
        turn = !turn;
        return;
      }
    } else {
      can_castle(Piece.Black, can_left, can_right);
      if (index == 2 && can_left) { // Castling queenside
        cout << "Castling queenside for black king" << endl;
        black_king_square = 2;
        Squares[2] = Piece.King | Piece.Black;
        Squares[3] = Piece.Rook | Piece.Black;
        Squares[4] = Piece.None;
        Squares[0] = Piece.None;
        black_castle[0] = black_castle[1] = 1;
        turn = !turn;
        this->moveCount++;
        return;
      } else if (index == 6 && can_right) { // Castling kingside
        cout << "Castling kingside for black king" << endl;
        black_king_square = 6;
        Squares[6] = Piece.King | Piece.Black;
        Squares[5] = Piece.Rook | Piece.Black;
        Squares[4] = Piece.None;
        turn = !turn;
        this->moveCount++;
      }
    }
  }

  // Capture piece if enpassant
  if (Piece.isPawn(picked_piece)) {
    if ((index) == enpassant_square)
      Squares[isWhite ? enpassant_square + 8 : enpassant_square - 8] =
          Piece.None;
  }

  // Place the picked_piece onto correct Index in Squares
  Squares[index] = picked_piece;
  this->moveCount++;
  if (index != (picked[1] * 8 + picked[0]))
    Squares[picked[1] * 8 + picked[0]] = Piece.None;

  if (Piece.isKing(picked_piece)) {
    if (isWhite)
      white_king_square = index;
    else
      black_king_square = index;
  }

  // if piece is rook and function comes til here, then it moved.
  if (Piece.isRook(picked_piece)) {
    if (isWhite) {
      if (picked[1] * 8 + picked[0] == 56)
        white_castle[0] = 1;
      else if (picked[1] * 8 + picked[0] == 63)
        white_castle[2] = 1;
    } else {
      if (picked[1] * 8 + picked[0] == 0)
        black_castle[0] = 1;
      else if (picked[1] * 8 + picked[0] == 7)
        black_castle[2] = 1;
    }
  }

  // if piece placed is pawn then set enpassant square
  if (Piece.isPawn(picked_piece)) {
    if (abs(index - (picked[1] * 8 + picked[0])) == 16) {
      enpassant_square = turn == 0 ? index + 8 : index - 8;
    } else {
      enpassant_square = -1;
    }
  } else {
    enpassant_square = -1;
  }

  turn = !turn; // Change turn
  updateCopy();
}

inline bool Board::hasWon() {
  int king_square = turn == 0 ? white_king_square : black_king_square;
  vector<Move> Possible_Legal_Moves =
      GenerateForPiece(Squares[king_square], king_square);
  bool status = false;
  if (Possible_Legal_Moves.empty()) {
    status = (moveCount > 3);
  }
  if (status) {
    cout << (turn == 0 ? "Black won\n" : "White won\n");
    cout << "Move count = " << moveCount << endl;
  }
  return status;
}

inline bool Board::legalPlace(int x, int y) {

  int destination = y * 8 + x;
  int startSquare = picked[1] * 8 + picked[0];
  vector<Move> legal_moves;

  legal_moves = GenerateForPiece(picked_piece, startSquare);
  bool legal = false;
  // loop over all the legal moves, if our destination matches any then it is
  // also legal
  for (auto &move : legal_moves) {
    if (move.targetSquare == destination) {
      legal = true;
      break;
    }
  }

  return legal;
}

inline void Board::loadFromFen(string fen) {

  map<char, int> pieceMap;
  pieceMap['k'] = Piece.King;
  pieceMap['p'] = Piece.Pawn;
  pieceMap['b'] = Piece.Bishop;
  pieceMap['n'] = Piece.Knight;
  pieceMap['r'] = Piece.Rook;
  pieceMap['q'] = Piece.Queen;

  int file = 0, rank = 0;

  for (char c : fen) {
    if (c == '/') {
      file = 0;
      rank++;
    } else {
      if (isdigit(c)) {
        file += (c - '0');
      } else {
        int pieceColor = isupper(c) ? Piece.White : Piece.Black;
        int pieceType = pieceMap[isupper(c) ? tolower(c) : c];
        Squares[rank * 8 + file] = pieceType | pieceColor;
        file++;
      }
    }
  }
  updateCopy();
}

#endif // BOARD_H
