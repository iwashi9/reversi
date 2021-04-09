#include <assert.h>
#include <stdio.h>
#include "move.h"

const static IntPair directions[8] = {
  { -1, -1 }, {  0, -1 }, { 1, -1 }, 
  { -1,  0 },             { 1,  0 }, 
  { -1,  1 }, {  0,  1 }, { 1,  1 } 
};

extern int board[8][8];

static int can_flip(const int side, IntPair square, const IntPair direction)
{
  assert(board[square.first][square.second] == 0);
  int n = 0;
  do {
    square.first  += direction.first; 
    square.second += direction.second; 
    n++;
    if (square.first  < 0 || square.first >= 8 || 
	square.second < 0 || square.second >= 8) 
      return 0;
  } while (board[square.first][square.second] == -side);
  if (board[square.first][square.second] == 0) return 0;
  if (n <= 1) return 0;
  return 1;
}

int is_legal_move(const int side, const IntPair square)
{
  if (square.first  < 0 || square.first  >= 8) return 0;
  if (square.second < 0 || square.second >= 8) return 0;

  int i;
  if (board[square.first][square.second] != 0) return 0;
  for (i = 0; i < 8; i++) {
    if (can_flip(side, square, directions[i])) return 1;
  }
  return 0;
}

int place_disk(const int side, const IntPair square)
{
  assert(is_legal_move(side, square));
  int n = 0, i;
  for (i = 0; i < 8; i++) {
    const IntPair dir = directions[i];
    if (!can_flip(side, square, dir)) continue;
    int x = square.first  + dir.first;
    int y = square.second + dir.second;
    while (board[x][y] == -side) {
      board[x][y] = side;
      n++;
      x += dir.first;
      y += dir.second;
    }
  }
  board[square.first][square.second] = side;
  assert(n > 0);
  return n;
}

int generate_all_legal_moves(const int side, IntPair legal_moves[])
{
  int x, y;
  int nmoves = 0;  // number of generated moves
  int nempty = 0;  // number of empty squares
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      if (board[x][y] != 0) continue;
      nempty++;
      IntPair square;
      square.first = x; square.second = y;
      if (!is_legal_move(side, square)) continue;
      assert(nmoves < 60);
      legal_moves[nmoves++] = square;
    }
  }
  if (nempty == 0) return -1;
  return nmoves;
}

