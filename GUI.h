#ifndef GUI_H
#define GUI_H

#include "Piece.h" // Include the Piece header file here
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Move.h"
using namespace std;
using namespace sf;

class GUI {
  RectangleShape graphical_board[8][8];
  Texture textures[12];

public:
  int block_size = 120;
  GUI() {
    string pieces[6] = {"king", "pawn", "knight", "bishop", "rook", "queen"};
    string colors[2] = {"white", "black"};

    int index = 0;
    for (string color : colors) {
      for (string piece : pieces) {
        textures[index].loadFromFile("./assets/" + color + "-" + piece +
                                     ".png");
        index++;
      }
    }

    for (int file = 0; file < 8; file++) {
      for (int rank = 0; rank < 8; rank++) {
        bool isLight = (file + rank) % 2 == 0;

        Color light(239, 210, 185);
        Color dark(158, 110, 89);
        graphical_board[file][rank].setSize(Vector2f(block_size, block_size));
        graphical_board[file][rank].setFillColor(isLight ? light : dark);
        graphical_board[file][rank].setPosition(rank * block_size,
                                                rank * block_size);
      }
    }
  }

  void show_picked(RenderWindow &window, int picked, int x, int y);

  void show_legal_moves(RenderWindow &window, const vector<Move> &Moves);

  void draw(RenderWindow &window, int *Squares);
};

inline void GUI::show_picked(RenderWindow &window, int picked, int x, int y) {

  Sprite piece;
  int type = picked & 7;
  int color = picked & 24;
  piece.setTexture(textures[(type - 1) + (color == Piece.White ? 0 : 6)]);
  piece.setPosition(x - 100, y - 100);
  window.draw(piece);
}
inline void GUI::show_legal_moves(RenderWindow &window,
                                  const vector<Move> &Moves) {

  for (auto &move : Moves) {
    RectangleShape legal;
    legal.setSize(Vector2f(block_size, block_size));
    int rank = move.targetSquare / 8;
    int file = move.targetSquare % 8;
    legal.setFillColor(Color(200, 0, 0, 150));
    legal.setPosition(file * block_size, rank * block_size);
    window.draw(legal);
  }
}

inline void GUI::draw(RenderWindow &window, int *Squares) {

  for (int file = 0; file < 8; file++) {
    for (int rank = 0; rank < 8; rank++) {

      graphical_board[file][rank].setPosition(
          rank * block_size, file * block_size); // Position the square

      window.draw(graphical_board[file][rank]); // Draw the square
    }
  }

  for (int file = 0; file < 8; file++) {
    for (int rank = 0; rank < 8; rank++) {
      int index = file * 8 + rank; // Calculate the index in the Squares array
      int type = Squares[index] & 7;
      int color = Squares[index] & 24;

      if (type != Piece.None) {
        Sprite piece;
        piece.setTexture(textures[(type - 1) + (color == Piece.White ? 0 : 6)]);
        piece.setPosition(rank * block_size - 5, file * block_size - 5);
        window.draw(piece);
      }
    }
  }
}

#endif // !GUI_H
