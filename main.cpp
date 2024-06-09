#include "Board.h"
#include "ChessEngine.h"
#include "GUI.h"
#include <cstdlib>

using namespace std;

int main() {

  GUI gui;

  Board board;
  ChessEngine engine(&board);
  string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
  string queenFEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R";
  board.loadFromFen(queenFEN);
  board.whiteKingPos = 24, board.blackKingPos = 39;
  int x = -1, y = -1;

  /*for (int i = 1; i < 4; ++i) {
    auto start = chrono::high_resolution_clock::now();
    int moveCount = board.MoveGenerationTest(i, true);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Depth " << i << ": " << moveCount << " moves"
         << ", Time: " << duration.count() * 1000 << " milliseconds" << endl;
  }*/

  RenderWindow window(VideoMode(1500, 960), "Chess");
  window.setPosition(Vector2i(400, 0));

  bool pieceIsPicked = false;
  int pickedPiece = Piece.None;
  while (window.isOpen()) {
    Event e;
    while (window.pollEvent(e)) {
      if (e.type == Event::Closed) {
        window.close();
      }
      if (e.type == Event::KeyPressed) {
        if (e.key.code == Keyboard::C) {
          system("clear");
        }
        if (e.key.code == Keyboard::P) {
          board.print();
        }
      }
      if (e.type == Event::MouseButtonPressed) {
        x = e.mouseButton.x / gui.block_size;
        y = e.mouseButton.y / gui.block_size;
        int chosenSquare = y * 8 + x;
        cout << x << " " << y << " " << chosenSquare << endl;
        if (!pieceIsPicked)
          pieceIsPicked = board.pickPlayer(chosenSquare, pickedPiece);
        else {
          board.placePlayer(chosenSquare, pickedPiece);
          pieceIsPicked = false;
        }
      }
    }

    window.clear(Color(200, 200, 200));
    gui.draw(window, board.getSquares(), board.getPickedPieceCords()[0],
             board.getPickedPieceCords()[1]);

    if (pieceIsPicked) {
      gui.show_picked(window, pickedPiece, Mouse::getPosition(window).x,
                      Mouse::getPosition(window).y);
      gui.show_legal_moves(window,
                           board.GenerateAllLegalMovesForPiece(pickedPiece));
    }
    window.display();
  }

  return 0;
}
