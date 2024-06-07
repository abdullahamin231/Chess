#include "Board.h"
#include "ChessEngine.h"
#include "GUI.h"
#include <chrono>
#include <cstdlib>
#include <iostream>

using namespace std;

int main() {

  GUI gui;

  Board board;
  ChessEngine engine(&board);
  string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
  board.loadFromFen(startingFEN);

  bool picked = false;
  bool checkmate = false;

  int x = -1, y = -1, temp_picked = Piece.None;

  for (int i = 0; i < 6; ++i) {
    auto start = chrono::high_resolution_clock::now();
    int moveCount = board.MoveGenerationTest(i, true);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Depth " << i << ": " << moveCount
         << " moves, Time: " << duration.count() * 1000 << " milliseconds"
         << endl;
  }

  /*RenderWindow window(VideoMode(960, 960), "Chess");
  window.setPosition(Vector2i(400, 0));
  while (window.isOpen()) { // Added checkmate condition
    Event e;
    while (window.pollEvent(e)) {
      if (e.type == Event::Closed) {
        window.close();
      }
      if (e.type == Event::KeyPressed) {
        if (e.key.code == Keyboard::C) {
          system("clear");
        } else if (e.key.code == Keyboard::P) {
          bool engine_placed;
          Move engine_move = engine.GenerateMove();
          engine_placed = board.playEngineMove(
              engine_move.startSquare, engine_move.targetSquare, temp_picked);

          if (engine_placed) {
            bool response_placed;
            Move response_move = engine.GenerateMove();
            response_placed =
                board.playEngineMove(response_move.startSquare,
                                     response_move.targetSquare, temp_picked);
          }
        }
      }
      if (e.type == Event::MouseButtonPressed) {
        x = e.mouseButton.x / gui.block_size;
        y = e.mouseButton.y / gui.block_size;
        if (!picked) {
          picked = board.pickup(x, y, temp_picked);
        } else {
          bool placed = false;
          placed = board.place(x, y), picked = false;
          if (board.calculateCheckmate()) { // Check for checkmate after each
            checkmate = true;
            cout << "Checkmate!\n";
          } else if (placed) {
            Move engine_move = engine.GenerateMove();
            bool engine_placed = board.playEngineMove(
                engine_move.startSquare, engine_move.targetSquare, temp_picked);
            while (!engine_placed) {
              engine_move = engine.GenerateMove();
              engine_placed =
                  board.playEngineMove(engine_move.startSquare,
                                       engine_move.targetSquare, temp_picked);
            }
          }
        }
      }
    }

    window.clear();
    gui.draw(window, board.getSquares(), board.getPickedPiece());

    if (picked) {
      gui.show_picked(window, temp_picked, Mouse::getPosition(window).x,
                      Mouse::getPosition(window).y);
      gui.show_legal_moves(window,
                           board.GenerateForPiece(temp_picked, y * 8 + x));
    }
    window.display();
  }*/

  return 0;
}
