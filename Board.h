
#ifndef BOARD_H
#define BOARD_H

#include "Move.h"
#include "Piece.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <exception>
#include <iostream>
using namespace std;
using namespace sf;

class Board {
public:
  // Player input handling
  bool pickPlayer(int startSquare, int &main_pickedPiece);
  bool placePlayer(int targetSquare, int &main_pickedPiece);
  // Confirms if move is legal
  bool validPlace(int targetSquare);
  bool isMoveLegal(int startSquare, int targetSquare);
  bool inCheck();
  int makeMove(int startSquare, int targetSquare);
  void undoMove(int movedPiece);

  vector<Move> GenerateAllLegalMoves(bool ofEnemy, bool checkForCheck);
  vector<Move> GenerateAllLegalMovesForPiece(int piece);

  void GenerateSlidingMoves(vector<Move> &moves, int startSquare, int piece,
                            int friendly_color, bool alsoCheck = 0);
  void GenerateKnightMoves(vector<Move> &Moves, int startSquare, int piece,
                           int friendly_color, bool alsoCheck = 0);
  void GeneratePawnMoves(vector<Move> &Moves, int startSquare, int piece,
                         int friendly_color, bool alsoCheck = 0);
  void GenerateKingMoves(vector<Move> &Moves, int startSquare, int piece,
                         int correct_color, bool alsoCheck = 0);

  Board();
  void PrecomputedMoveData();
  void printMove(int startSquare, int targetSquare);
  void updateCopy();
  void copyFromCopy();

  void print() {}
  void changeTurn() { turn = !turn; }
  int *getPickedPieceCords() { return this->pickedPieceCords; }
  int indexFromXY(int x, int y) { return y * 8 + x; }
  bool validTurn();
  int getColor(int piece) { return piece & 24; }
  int getPickedPiece() { return this->pickedPiece; }
  bool isWhiteTurn() { return turn == 0; }
  bool isWhitePiece(int piece) { return (piece & 24) == 8; }
  int *getSquares() { return this->Squares; }
  void loadFromFen(string fen);

public:
  int pickedPiece = Piece.None;
  int pickedPieceSquare = Piece.None;
  int pickedPieceCords[2];

  int whiteKingPos = 60, blackKingPos = 4, fakeKingPos = -1;
  bool posChanged = false;

  int enpassantSquare = -1;

  int Squares[64] = {0};
  int copySquares[64] = {0};
  int turn = 0;
  int directionOffsets[8] = {8, -8, -1, 1, 7, -7, 9, -9};
  int NumSquaresToEdge[64][8];

  unordered_map<int, char> rankfile;
  vector<int> capturedPieces;
};

inline bool Board::pickPlayer(int startSquare, int &main_pickedPiece) {

  this->pickedPiece = Squares[startSquare];
  if (!validTurn()) {
    this->pickedPiece = Piece.None;
    return false;
  }

  this->pickedPieceSquare = startSquare;
  this->pickedPieceCords[0] = startSquare % 8,
  this->pickedPieceCords[1] = startSquare / 8;
  main_pickedPiece = this->pickedPiece;

  this->updateCopy();
  return true;
}

inline bool Board::placePlayer(int targetSquare, int &main_pickedPiece) {
  if (!validPlace(targetSquare)) {
    pickedPiece = 0;
    pickedPieceSquare = -1;
    pickedPieceCords[0] = -1, pickedPieceCords[1] = -1;
    return false;
  }

  bool isWhite = isWhitePiece(pickedPiece);

  // Remove picked piece from board
  Squares[pickedPieceSquare] = Piece.None;

  int capturedPiece = Squares[targetSquare];
  if (capturedPiece != Piece.None) {
    capturedPieces.push_back(capturedPiece);
  }

  // Place picked piece on board
  Squares[targetSquare] = this->pickedPiece;

  if (Piece.isPawn(pickedPiece)) {
    if (targetSquare == enpassantSquare) {
      int enpassantCapturedSquare =
          isWhite ? enpassantSquare + 8 : enpassantSquare - 8;
      capturedPiece = Squares[enpassantCapturedSquare];
      if (capturedPiece != Piece.None) {
        capturedPieces.push_back(capturedPiece);
      }
      Squares[enpassantCapturedSquare] = Piece.None;
    }
  }

  // Store capturedPieces
  if (Squares[targetSquare] != Piece.None) {
    capturedPieces.push_back(Squares[targetSquare]);
  }

  // Setting kingPos to be used later in inCheck
  if (Piece.isKing(this->pickedPiece)) {
    if (isWhite)
      whiteKingPos = targetSquare;
    else
      blackKingPos = targetSquare;
  }

  // if pawn moves two squares, set then enpassantSquare
  if (Piece.isPawn(this->pickedPiece)) {
    if (abs(targetSquare - pickedPieceSquare) == 16) {
      enpassantSquare = isWhite ? targetSquare + 8 : targetSquare - 8;
    } else {
      enpassantSquare = -1;
    }
  } else {
    enpassantSquare = -1;
  }

  // so nothing is shown floating
  main_pickedPiece = this->pickedPiece = Piece.None;
  this->pickedPieceSquare = Piece.None;

  updateCopy();
  changeTurn();

  return true;
}

inline bool Board::validPlace(int targetSquare) {
  // didn't place anywhere
  if (targetSquare == this->pickedPieceSquare)
    return false;

  // calculate all legal moves, if where player is placing
  // is found in legal moves then it is ok.
  vector<Move> possible_moves = GenerateAllLegalMoves(false, true);
  for (auto &move : possible_moves) {
    if (move.targetSquare == targetSquare) {
      return true;
    }
  }

  return false;
}

inline bool Board::isMoveLegal(int startSquare, int targetSquare) {
  int movedPiece = makeMove(startSquare, targetSquare);
  if (inCheck()) {
    undoMove(movedPiece);
    return false;
  }
  undoMove(movedPiece);
  return true;
}

inline bool Board::inCheck() {
  vector<Move> enemyMoves = GenerateAllLegalMoves(true, false);
  int ownKingColor = isWhiteTurn() ? whiteKingPos : blackKingPos;
  for (auto &em : enemyMoves) {
    if (em.targetSquare == ownKingColor)
      return true;
  }
  return false;
}

inline int Board::makeMove(int startSquare, int targetSquare) {
  updateCopy();
  int movedPiece = Squares[startSquare];
  Squares[startSquare] = Piece.None;

  if (Piece.isKing(movedPiece)) {
    posChanged = true;
    if (isWhitePiece(movedPiece)) {
      fakeKingPos = whiteKingPos;
      whiteKingPos = targetSquare;
    } else {
      fakeKingPos = blackKingPos;
      blackKingPos = targetSquare;
    }
  }
  Squares[targetSquare] = movedPiece;

  if (targetSquare == enpassantSquare) {
    Squares[isWhitePiece(movedPiece) ? enpassantSquare + 8
                                     : enpassantSquare - 8] = Piece.None;
  }

  return movedPiece;
}
inline void Board::undoMove(int movedPiece) {
  copyFromCopy();
  if (posChanged) {
    if (isWhitePiece(movedPiece)) {
      whiteKingPos = fakeKingPos;
    } else {
      blackKingPos = fakeKingPos;
    }
  }
  posChanged = false;
}

inline vector<Move> Board::GenerateAllLegalMoves(bool ofEnemy,
                                                 bool checkForCheck) {
  vector<Move> possible_moves;

  if (ofEnemy)
    changeTurn();

  int correct_color = (turn == 0) ? Piece.White : Piece.Black;

  for (int startSquare{0}; startSquare < 64; ++startSquare) {
    int piece = Squares[startSquare];

    // Only calculate moves for a piece if it is correct color
    if (Piece.sameColor(piece, correct_color)) {
      if (Piece.isSliding(piece)) {
        GenerateSlidingMoves(possible_moves, startSquare, piece, correct_color,
                             checkForCheck);
      } else if (Piece.isKing(piece)) {
        GenerateKingMoves(possible_moves, startSquare, piece, correct_color,
                          checkForCheck);
      } else if (Piece.isKnight(piece)) {
        GenerateKnightMoves(possible_moves, startSquare, piece, correct_color,
                            checkForCheck);
      } else if (Piece.isPawn(piece)) {
        GeneratePawnMoves(possible_moves, startSquare, piece, correct_color,
                          checkForCheck);
      }
    }
  }
  if (ofEnemy) {
    changeTurn();
  }
  return possible_moves;
};

inline vector<Move> Board::GenerateAllLegalMovesForPiece(int piece) {
  vector<Move> possible_moves;
  int correct_color = (turn == 0) ? Piece.White : Piece.Black;

  int startSquare = this->pickedPieceSquare;
  // Only calculate moves for a piece if it is correct color
  if (Piece.sameColor(piece, correct_color)) {
    if (Piece.isSliding(piece)) {
      GenerateSlidingMoves(possible_moves, startSquare, piece, correct_color,
                           true);
    } else if (Piece.isKing(piece)) {
      GenerateKingMoves(possible_moves, startSquare, piece, correct_color,
                        true);
    } else if (Piece.isKnight(piece)) {
      GenerateKnightMoves(possible_moves, startSquare, piece, correct_color,
                          true);
    } else if (Piece.isPawn(piece)) {
      GeneratePawnMoves(possible_moves, startSquare, piece, correct_color,
                        true);
    }
  }
  return possible_moves;
};

inline void Board::GenerateSlidingMoves(vector<Move> &Moves, int startSquare,
                                        int piece, int friendly_color,
                                        bool alsoCheck) {

  int enemy_color = friendly_color == 8 ? 16 : 8;
  int startDirIndex = Piece.isBishop(piece) ? 4 : 0;
  int endDirIndex = Piece.isRook(piece) ? 4 : 8;

  for (int directionIndex = startDirIndex; directionIndex < endDirIndex;
       directionIndex++) {
    for (int n = 0; n < NumSquaresToEdge[startSquare][directionIndex]; n++) {

      int targetSquare =
          startSquare + directionOffsets[directionIndex] * (n + 1);
      int pieceOnTargetSquare = Squares[targetSquare];

      if (Piece.sameColor(pieceOnTargetSquare, friendly_color)) {
        break;
      }

      if (alsoCheck) {
        if (isMoveLegal(startSquare, targetSquare)) {
          Moves.push_back(Move(startSquare, targetSquare));
        }
      } else {
        Moves.push_back(Move(startSquare, targetSquare));
      }
      // if(!isMoveLegal(startSquare, targetSquare))
      // Moves.pop_back();

      if (Piece.sameColor(pieceOnTargetSquare, enemy_color)) {
        break;
      };
    }
  }
};

inline void Board::GenerateKnightMoves(vector<Move> &Moves, int startSquare,
                                       int piece, int friendly_color,
                                       bool alsoCheck) {

  int dx[] = {2, 2, -2, -2, 1, 1, -1, -1};
  int dy[] = {1, -1, 1, -1, 2, -2, 2, -2};

  int enemy_color = friendly_color == 8 ? 16 : 8;

  for (int i = 0; i < 8; ++i) {
    int newRow = startSquare / 8 + dx[i];
    int newCol = startSquare % 8 + dy[i];
    if (newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
      int targetSquare = newRow * 8 + newCol;
      if (Piece.sameColor(Squares[targetSquare], friendly_color)) {
        continue;
      }
      if (alsoCheck) {
        if (isMoveLegal(startSquare, targetSquare)) {
          Moves.push_back(Move(startSquare, newRow * 8 + newCol));
        }
      } else
        Moves.push_back(Move(startSquare, newRow * 8 + newCol));

      if (Piece.sameColor(Squares[targetSquare], enemy_color)) {
        continue;
      }
    }
  }
}

inline void Board::GeneratePawnMoves(vector<Move> &Moves, int startSquare,
                                     int piece, int friendly_color,
                                     bool alsoCheck) {
  int enemy_color = friendly_color == 8 ? 16 : 8;
  bool isWhite = friendly_color == 8;
  int direction = isWhite ? -8 : 8;
  int firstRow = isWhite ? 6 : 1;
  int enpassantRow = isWhite ? 3 : 4;

  int currentRow = startSquare / 8;
  int targetSquare = startSquare + direction;
  // One space forward + If first move then 2 space forward
  if (Squares[targetSquare] == Piece.None) {
    if (alsoCheck) {
      if (isMoveLegal(startSquare, targetSquare))
        Moves.push_back(Move(startSquare, targetSquare));
    } else {
      Moves.push_back(Move(startSquare, targetSquare));
    }
    if (currentRow == firstRow) {
      targetSquare += direction;
      if (Squares[targetSquare] == Piece.None) {
        if (alsoCheck) {
          if (isMoveLegal(startSquare, targetSquare)) {
            Moves.push_back(Move(startSquare, targetSquare));
          }

        } else {
          Moves.push_back(Move(startSquare, targetSquare));
        }
      }
    }
  }

  int directions[2] = {-1, 1};
  if (currentRow == firstRow)
    targetSquare -= direction;
  for (int i : directions) {
    targetSquare += i;
    if (Piece.sameColor(Squares[targetSquare], enemy_color)) {
      if (alsoCheck) {
        if (isMoveLegal(startSquare, targetSquare))
          Moves.push_back(Move(startSquare, targetSquare));
      } else {
        Moves.push_back(Move(startSquare, targetSquare));
      }
    }
    targetSquare -= i;
  }

  if (currentRow == enpassantRow) {
    if (abs(startSquare % 8 - enpassantSquare % 8) == 1) {
      if (Squares[enpassantSquare] == Piece.None ||
          Piece.sameColor(Squares[enpassantSquare], enemy_color)) {
        if (alsoCheck) {
          if (isMoveLegal(startSquare, enpassantSquare)) {
            Moves.push_back(Move(startSquare, enpassantSquare));
          }
        } else {
          Moves.push_back(Move(startSquare, enpassantSquare));
        }
      }
    }
  }
};

inline void Board::GenerateKingMoves(vector<Move> &Moves, int startSquare,
                                     int piece, int friendly_color,
                                     bool alsoCheck) {

  int enemy_color = friendly_color == 8 ? 16 : 8;
  int ownCol = startSquare % 8, ownRow = startSquare / 8;
  for (int directionIndex = 0; directionIndex < 8; ++directionIndex) {
    int targetSquare = startSquare + directionOffsets[directionIndex];

    // Check if targetSquare is within board limits
    if (targetSquare < 0 || targetSquare >= 64) {
      continue; // Skip invalid squares
    }

    int targetCol = targetSquare % 8, targetRow = targetSquare / 8;

    if (abs(targetCol - ownCol) > 1)
      continue;

    int pieceOnTargetSquare = Squares[targetSquare];

    if (pieceOnTargetSquare == Piece.None ||
        Piece.sameColor(pieceOnTargetSquare, enemy_color)) {
      if (alsoCheck) {
        if (isMoveLegal(startSquare, targetSquare))
          Moves.push_back(Move(startSquare, targetSquare));
      } else {
        Moves.push_back(Move(startSquare, targetSquare));
      }
    }
  }
};

inline bool Board::validTurn() {
  int pickColor = this->pickedPiece & 24;
  if ((isWhiteTurn() && pickColor == Piece.White) ||
      (!isWhiteTurn() && pickColor == Piece.Black)) {
    return true;
  }
  return false;
}
inline Board::Board() {
  PrecomputedMoveData();
  char files[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
  // Assign values 0 to 7 to each file
  for (int i = 0; i < 8; ++i) {
    rankfile[i] = files[i];
  }
}
inline void Board::PrecomputedMoveData() {
  for (int file = 0; file < 8; file++) {
    for (int rank = 0; rank < 8; rank++) {
      int squareIndex = rank * 8 + file;

      int numNorth = 7 - rank;
      int numSouth = rank;
      int numWest = file;
      int numEast = 7 - file;

      NumSquaresToEdge[squareIndex][0] = numNorth;               // North
      NumSquaresToEdge[squareIndex][1] = numSouth;               // South
      NumSquaresToEdge[squareIndex][2] = numWest;                // West
      NumSquaresToEdge[squareIndex][3] = numEast;                // East
      NumSquaresToEdge[squareIndex][4] = min(numNorth, numWest); // North-West
      NumSquaresToEdge[squareIndex][5] = min(numSouth, numEast); // South-East
      NumSquaresToEdge[squareIndex][6] = min(numNorth, numEast); // North-East
      NumSquaresToEdge[squareIndex][7] = min(numSouth, numWest); // South-West
    }
  }
}
inline void Board::updateCopy() {
  for (int i = 0; i < 64; ++i) {
    copySquares[i] = Squares[i];
  }
}

inline void Board::copyFromCopy() {
  for (int i = 0; i < 64; ++i) {
    Squares[i] = copySquares[i];
  }
}
inline void Board::printMove(int startSquare, int targetSquare) {
  Piece.name(Squares[startSquare]);
  cout << " " << rankfile[startSquare % 8] << startSquare / 8
       << rankfile[targetSquare % 8] << targetSquare / 8 << " - attacking: ";
  Piece.name(Squares[targetSquare]);
  cout << endl;
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
