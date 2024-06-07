#include "Board.h"
#include "GUI.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

int main() {

  RenderWindow window(VideoMode(960, 960), "Chess");
  window.setPosition(Vector2i(400, 0));
  GUI gui;

  Board board;

  string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
  board.loadFromFen(startingFEN);

  bool picked = false;

  int x = -1, y = -1, temp_picked = Piece.None;

  while (window.isOpen()) {
    Event e;
    while (window.pollEvent(e)) {
      if (e.type == Event::Closed) {
        window.close();
      }
      if (e.type == Event::KeyPressed) {
        if (e.key.code == Keyboard::P) {
          board.print();
        } else if (e.key.code == Keyboard::O) {
          board.checkStatus();
        }
      }
      if (e.type == Event::MouseButtonPressed && !board.hasWon()) {
        x = e.mouseButton.x / gui.block_size;
        y = e.mouseButton.y / gui.block_size;
        if (!picked) {
          picked = board.pickup(x, y, temp_picked);
        } else {
          board.place(x, y), picked = false;
        }
      }
    }

    window.clear();
    gui.draw(window, board.getSquares());

    if (picked && !board.hasWon()) {
      gui.show_picked(window, temp_picked, Mouse::getPosition(window).x,
                      Mouse::getPosition(window).y);
      gui.show_legal_moves(window,
                           board.GenerateForPiece(temp_picked, y * 8 + x));
    }
    window.display();
  }
}
