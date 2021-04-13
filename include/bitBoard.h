#ifndef BIT_BOARD_H
#define BIT_BOARD_H

typedef unsigned long ul;

typedef struct bit_board {
  ul black_board;
  ul white_board;
  int turn;
} Board;

typedef struct history {
  Board board[61];
  int index;
} History;

Board init_board();
void print_mask(ul mask);
void print_board(Board board);
int bit_count(ul a);

ul generate_all_legal_moves(Board board);
ul transfer(ul put, int k);
ul reverse_(Board board, ul put);
Board reverse(Board board, ul put);
int is_pass(Board board);
int is_finished(Board board);
ul to_bit_board(int col, int row);
ul get_random_move(ul moves);
int eval(Board board);
void ul_to_str(ul put, char *str);

#endif
