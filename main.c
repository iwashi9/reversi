#include <stdio.h>
#include <stdlib.h>
#include "move.h"

int board[8][8];  // 8x8 squares: 0 -> empty; 1 -> black; -1 -> white

static void init_board()
{
  int x, y;
  for (y = 0; y < 8; y++) 
    for (x = 0; x < 8; x++)
      board[x][y] = 0;

  board[4][3] = board[3][4] =  1;  // black disks
  board[3][3] = board[4][4] = -1;  // white disks
}

static void print_board()
{
  int x, y;
  puts("\n  a b c d e f g h");
  for (y = 0; y < 8; y++) {
    printf("%d ", y + 1);
    for (x = 0; x < 8; x++) {
      const int d = board[x][y];
      char c = '-';
      if (d ==  1) c = 'B';
      if (d == -1) c = 'W';
      printf("%c ", c);
    }
    putchar('\n');
  }
  putchar('\n');
}

int main(int argc, char **argv)
{
  const int human_side = (argc >= 2) ? atoi(argv[1]) : 0;

  init_board();

  int turn;
  for (turn = 1;; turn *= -1) {
    print_board();

    IntPair legal_moves[60];
    const int nmoves = generate_all_legal_moves(turn, legal_moves);
    if (nmoves == -1) break;     // no empty square
    if (nmoves ==  0) { // pass
      printf("turn = %d, move = Pass\n", turn);
      continue;  
    }

    IntPair move;
    if (turn == human_side) {
      while (1) {
	printf("Where? ");
	char buf[1000];
	scanf("%s", buf);
	move.first  = buf[0] - 'a';
	move.second = buf[1] - '1';
	if (is_legal_move(turn, move)) break;
	printf("%s is not a legal move!\n\n", buf);
      } 
    } else {
      move = legal_moves[0];  // choose the first legal move
      printf("turn = %d, move = %c%c\n", 
	     turn, 'a' + move.first, '1' + move.second);
    }
    place_disk(turn, move);
  }

  int black_count = 0;
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++)
      if (board[y][x] == 1) black_count++;

  if (black_count == 32) printf("Draw.\n");
  else if (black_count < 32) printf("White win!\n");
  else printf("Black win!\n");

  return 0;
}
