#pragma GCC diagnostic ignored "-Wnarrowing"

#include <csignal>
#include <random>
#include <utility>
#include <vector>

#include <iostream>

extern "C" {
#include <ncurses.h>
#include <unistd.h>
}

constexpr char empty{' '}, alive{'X'};
constexpr int times{ 100 };

using Row = std::vector<char>;
using Board = std::vector<Row>;

void term(int);
int checkNeighbors(const Board&, int, int);
void iterateBoard(Board&);
void printBoard(const Board&);

int main(int argc, char **argv) {
	signal(SIGTERM, term);
	initscr();
	Board board;

	int y, x;
	getmaxyx(stdscr, y, x);
	y = 30; x = 120;
	board.resize(y);
	for (auto& row : board)
		row.resize(x);

	std::random_device rd;
	std::mt19937 mt{ rd() };
	std::uniform_int_distribution<int> dist{ 0, 1 };
	for (auto& row : board)
		for (auto& cell : row)
			cell = dist(mt) == 1 ? alive : empty;

	printBoard(board);
	for (int i{ 0 }; i < times; ++i) {
		sleep(1);
		iterateBoard(board);
		printBoard(board);
	}
	getch();
	endwin();
	return 0;
}

void term(int sig) {
	endwin();
	std::exit(1);
}

int countNeighbors(const Board& board, int x, int y) {
	int n{ 0 };
	static constexpr int neighbors[9][2]{
			{-1,-1},{-1,0},{-1,1},
			{ 0,-1},{ 0,0},{ 0,1},
			{ 1,-1},{ 1,0},{ 1,1}
	};
	static const auto bound = [](int x, int lower, int upper) -> int {
		if (x < lower)
			return lower;
		else if (x > upper)
			return upper;
		else
			return x;
	};
	int ymax = board.size(), xmax = board[0].size();
	for (const auto& neighbor : neighbors) {
		if (board[bound(y + neighbor[1], 0, ymax - 1)]
				[bound(x + neighbor[0], 0, xmax - 1)] == alive)
			++n;
	}

	return n;
}

void iterateBoard(Board& board) {
	static Board stage;
	int ymax{ board.size() }, xmax{ board[0].size() };

	if (stage.size() != ymax)
		stage.resize(ymax);
	if (stage[0].size() != xmax)
		for (auto& row : stage)
			row.resize(xmax);

	int n;
	for (int y{ 0 }; y < ymax; ++y) {
		for (int x{ 0 }; x < xmax; ++x) {
			n = countNeighbors(board, x, y);
			if (n == 3)
				stage[y][x] = alive;
			else if (n == 2)
				stage[y][x] = board[y][x];
			else
				stage[y][x] = empty;
		}
	}
	std::swap(board, stage);
}

void printBoard(const Board& board) {
	move(0, 0);
	for (int y{ 0 }; y < board.size(); ++y) {
		for (int x{ 0 }; x < board[0].size(); ++x) {
			addch(board[y][x]);
		}
		move(y, 0);
	}
	refresh();
}
