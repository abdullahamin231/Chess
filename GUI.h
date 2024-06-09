
#ifndef GUI_H
#define GUI_H

#include "Piece.h" // Include the Piece header file here
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>

#include "Move.h"
using namespace std;
using namespace sf;

class GUI {
  RectangleShape graphical_board[8][8];
  Texture textures[12];

  //  Color selectedColor(217, 109, 34);

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
                                                file * block_size);
      }
    }
  }

  void show_picked(RenderWindow &window, int picked, int x, int y);

  void show_legal_moves(RenderWindow &window, const vector<Move> &Moves);

  void draw(RenderWindow &window, int *Squares, int ox, int oy);
};

inline void GUI::show_picked(RenderWindow &window, int picked, int x, int y) {
  // Shows dimmed piece that follows the mouse
  if (picked == 0)
    return;
  Sprite piece;
  int type = picked & 7;
  int color = picked & 24;

  piece.setTexture(textures[(type - 1) + (color == Piece.White ? 0 : 6)]);
  piece.setPosition(x - 50, y - 20);
  piece.setColor(Color(255, 255, 255, 220));
  window.draw(piece);
}

inline void GUI::show_legal_moves(RenderWindow &window,
                                  const vector<Move> &Moves) {
  for (auto &move : Moves) {
    RectangleShape legal;
    legal.setSize(Vector2f(block_size, block_size));
    int rank = move.targetSquare / 8;
    int file = move.targetSquare % 8;
    legal.setFillColor(Color(200, 0, 0, 180));
    legal.setPosition(file * block_size, rank * block_size);
    window.draw(legal);
  }
}

inline void GUI::draw(RenderWindow &window, int *Squares, int ox, int oy) {
  for (int file = 0; file < 8; file++) {
    for (int rank = 0; rank < 8; rank++) {
      graphical_board[file][rank].setPosition(
          rank * block_size, file * block_size); // Position the square
      window.draw(graphical_board[file][rank]);  // Draw the square
    }
  }

  // purpose is to draw the selected box;
  RectangleShape chosen;
  chosen.setFillColor(Color(222, 111, 35));
  chosen.setSize(Vector2f(block_size, block_size));
  chosen.setPosition(ox * block_size, oy * block_size);
  window.draw(chosen);

  for (int file = 0; file < 8; file++) {
    for (int rank = 0; rank < 8; rank++) {
      int index = file * 8 + rank;
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
