
#ifndef BOARD_H
#define BOARD_H

#include "Move.h"
#include "Piece.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cctype>
#include <iostream>
#include <map>
using namespace std;
using namespace sf;

class Board {
public:
  int Squares[64] = {0};
  int picked_piece = Piece.None;
  int turn = 0; // 0 for white's turn, 1 for black's turn
  int picked[2] = {-1, -1};

  int directionOffsets[8] = {8, -8, 1, -1, 7, -7, 9, -9};
  int NumSquaresToEdge[64][8];

  int enpassant_square = -1;
  int white_castle[5] = {0, 0, 0, 58, 62};
  int black_castle[5] = {0, 0, 0, 2, 6};

  int white_king_square = 60, black_king_square = 4;
  bool isInCheck = false;

public:
  Board() { PrecomputedMoveData(); }

  void print() {
    cout << (turn == 0 ? "White" : "Black")
         << " in check: " << (in_check() ? "True" : "False")
         << ", white_king_square" << white_king_square << "\n black_king_square"
         << black_king_square << endl;
  }

  void PrecomputedMoveData() {
    for (int file = 0; file < 8; file++) {
      for (int rank = 0; rank < 8; rank++) {

        int numNorth = 7 - rank;
        int numSouth = rank;
        int numWest = file;
        int numEast = 7 - file;

        int squareIndex = rank * 8 + file;
        NumSquaresToEdge[squareIndex][0] = numNorth;
        NumSquaresToEdge[squareIndex][1] = numSouth;
        NumSquaresToEdge[squareIndex][2] = numWest;
        NumSquaresToEdge[squareIndex][3] = numEast;
        NumSquaresToEdge[squareIndex][4] = min(numNorth, numWest);
        NumSquaresToEdge[squareIndex][5] = min(numSouth, numEast);
        NumSquaresToEdge[squareIndex][6] = min(numNorth, numEast);
        NumSquaresToEdge[squareIndex][7] = min(numSouth, numWest);
      }
    }
  }

  bool isWhiteTurn() { return turn == 0; }
  bool isWhitePiece(int piece) { return (piece & 24) == 8; }
  int getPicked() { return picked_piece; }

  void GenerateSlidingMoves(vector<Move> &Moves, int startSquare, int piece) {
    int startDirIndex = Piece.isBishop(piece) ? 4 : 0;
    int endDirIndex = Piece.isRook(piece) ? 4 : 8;

    for (int directionIndex = startDirIndex; directionIndex < endDirIndex;
         directionIndex++) {
      for (int n = 0; n < NumSquaresToEdge[startSquare][directionIndex]; n++) {
        int targetSquare =
            startSquare + directionOffsets[directionIndex] * (n + 1);

        int pieceOnTargetSquare = Squares[targetSquare];

        // own colored piece
        if (Piece.isCorrectColor(pieceOnTargetSquare, turn)) {
          break;
        }

        Moves.push_back(Move(startSquare, targetSquare));

        // enemey piece
        if (Piece.isCorrectColor(pieceOnTargetSquare, !turn)) {
          break;
        }
      }
    }
  }

  void GeneratePawnMoves(vector<Move> &Moves, int startSquare, int piece) {
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
  };

  void GenerateKingMoves(vector<Move> &Moves, int startSquare, int piece) {
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

  void can_castle(int color, bool &can_left, bool &can_right) {
    if (color == Piece.White) {
      can_left = !white_castle[0] && !white_castle[1] &&
                 Squares[59] == Piece.None && Squares[58] == Piece.None &&
                 Squares[57] == Piece.None;
      can_right = !white_castle[1] && !white_castle[2] &&
                  Squares[61] == Piece.None && Squares[62] == Piece.None;
    } else {
      can_left = !black_castle[0] && !black_castle[1] &&
                 Squares[3] == Piece.None && Squares[2] == Piece.None &&
                 Squares[1] == Piece.None;
      can_right = !black_castle[1] && !black_castle[2] &&
                  Squares[5] == Piece.None && Squares[6] == Piece.None;
    }
  }

  void GenerateKnightMoves(vector<Move> &Moves, int startSquare, int piece) {
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
  };

  vector<Move> GenerateAllMoves(bool otherColor = 0) {
    vector<Move> Moves;
    if (otherColor) {
      turn = !turn;
    }
    for (int startSquare = 0; startSquare < 64; startSquare++) {
      int piece = Squares[startSquare];

      if (Piece.isCorrectColor(piece, turn)) {
        if (Piece.isSliding(piece)) {
          GenerateSlidingMoves(Moves, startSquare, piece);
        } else if (Piece.isKing(piece)) {
          GenerateKingMoves(Moves, startSquare, piece);
        } else if (Piece.isKnight(piece)) {
          GenerateKnightMoves(Moves, startSquare, piece);
        } else if (Piece.isPawn(piece)) {
          GeneratePawnMoves(Moves, startSquare, piece);
        }
      }
    }
    if (otherColor) {
      turn = !turn;
    }

    return Moves;
  }

  vector<Move> GenerateForPiece(int piece, int startSquare) {
    vector<Move> moves;
    if (Piece.isSliding(piece)) {
      GenerateSlidingMoves(moves, startSquare, piece);
    } else if (Piece.isKing(piece)) {
      GenerateKingMoves(moves, startSquare, piece);
    } else if (Piece.isKnight(piece)) {
      GenerateKnightMoves(moves, startSquare, piece);
    } else if (Piece.isPawn(piece)) {
      GeneratePawnMoves(moves, startSquare, piece);
    }
    return moves;
  }

  bool in_check() {
    vector<Move> possible_moves = GenerateAllMoves(true);
    for (auto &move : possible_moves) {
      if (move.targetSquare ==
          (turn == 0 ? white_king_square : black_king_square)) {
        return true;
      }
    }
    return false;
  }

  bool pickup(int x, int y, int &temp_picked) {
    int index = y * 8 + x;
    picked_piece = Squares[index];
    int pickColor = picked_piece & 24;

    // Check if it's the correct player's turn
    if ((isWhiteTurn() && pickColor != Piece.White) ||
        (!isWhiteTurn() && pickColor != Piece.Black)) {
      return false;
    }

    picked[0] = x;
    picked[1] = y;
    Squares[index] = Piece.None; // Temporarily remove the piece
    temp_picked = picked_piece;
    return true;
  }

  void place(int x, int y) {
    // Illegal place or placed on startSquare
    if (!legalPlace(x, y) || (y == picked[1] && x == picked[0])) {
      // Invalid move, restore the picked piece to its original place
      Squares[picked[1] * 8 + picked[0]] = picked_piece;
      return;
    }

    int index = y * 8 + x;
    bool isWhite = isWhitePiece(picked_piece);

    // Handle castling
    if (Piece.isKing(picked_piece)) {
      if (isWhite) {
        if (index == 58) { // Castling queenside
          white_king_square = 58;
          Squares[58] = Piece.King | Piece.White;
          Squares[59] = Piece.Rook | Piece.White;
          Squares[60] = Piece.None;
          Squares[56] = Piece.None;
          white_castle[1] = white_castle[2] = 1;
          return;
        } else if (index == 62) { // Castling kingside
          white_king_square = 62;
          Squares[62] = Piece.King | Piece.White;
          Squares[61] = Piece.Rook | Piece.White;
          Squares[60] = Piece.None;
          Squares[63] = Piece.None;
          white_castle[1] = white_castle[2] = 1;
          return;
        }
      } else {
        if (index == 2) { // Castling queenside
          black_king_square = 2;
          Squares[2] = Piece.King | Piece.Black;
          Squares[3] = Piece.Rook | Piece.Black;
          Squares[4] = Piece.None;
          Squares[0] = Piece.None;
          black_castle[1] = black_castle[2] = 1;
          return;
        } else if (index == 6) { // Castling kingside
          black_king_square = 6;
          Squares[6] = Piece.King | Piece.Black;
          Squares[5] = Piece.Rook | Piece.Black;
          Squares[4] = Piece.None;
          Squares[7] = Piece.None;
          black_castle[1] = black_castle[2] = 1;
          return;
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
  }

  bool legalPlace(int x, int y) {
    int index = y * 8 + x;
    int destinationPiece = Squares[index];
    int destinationColor = destinationPiece & 24;

    vector<Move> legal_moves;
    if (Piece.isSliding(picked_piece)) {
      GenerateSlidingMoves(legal_moves, picked[1] * 8 + picked[0],
                           picked_piece);
    } else if (Piece.isKing(picked_piece)) {
      GenerateKingMoves(legal_moves, picked[1] * 8 + picked[0], picked_piece);
    } else if (Piece.isKnight(picked_piece)) {
      GenerateKnightMoves(legal_moves, picked[1] * 8 + picked[0], picked_piece);
    } else if (Piece.isPawn(picked_piece)) {
      GeneratePawnMoves(legal_moves, picked[1] * 8 + picked[0], picked_piece);
    }

    bool legal = false;
    int destination = y * 8 + x;
    for (auto &move : legal_moves) {
      if (move.targetSquare == destination) {
        legal = true;
        break;
      }
    }

    return legal;
  }
  void loadFromFen(string fen) {
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
  }

  int *getSquares() { return this->Squares; }
};

#endif // BOARD_H