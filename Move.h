#ifndef MOVE_H
#define MOVE_H
struct Move {
  int startSquare;
  int targetSquare;

  int tf, tr;

  Move(int x, int y) : startSquare(x), targetSquare(y) {}
};

#endif // !MOVE_H
