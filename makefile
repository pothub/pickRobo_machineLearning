all:main.cpp
	g++ main.cpp -std=c++11 -o out `pkg-config opencv --cflags --libs`
	./out
