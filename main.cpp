#include "Board.h"
#include "GUI.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

int main() {

  RenderWindow window(VideoMode(960, 960), "Chess");
  GUI gui;

  Board board;

  string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
  board.loadFromFen(startingFEN);

  bool picked = false;

  int x, y, temp_picked;

  while (window.isOpen()) {
    Event e;
    while (window.pollEvent(e)) {
      if (e.type == Event::Closed) {
        window.close();
      }
      if (e.type == Event::KeyPressed) {
        if (e.key.code == Keyboard::P) {
          board.print();
        }
      }
      if (e.type == Event::MouseButtonPressed) {
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

    if (picked) {
      gui.show_picked(window, board.getPicked(), Mouse::getPosition().x,
                      Mouse::getPosition().y);
      gui.show_legal_moves(window,
                           board.GenerateForPiece(temp_picked, y * 8 + x));
    }
    window.display();
  }
}
