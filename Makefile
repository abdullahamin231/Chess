run:
	g++ main.cpp Board.h Piece.h GUI.h -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
	./sfml-app
