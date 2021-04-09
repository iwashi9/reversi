#ifndef BIT_BOARD_H
#define BIT_BOARD_H

typedef unsigned long ul;
typedef struct history {
  int index;
  ul put[60];
  ul rev[60];
  int turn[60];
} History;

void init_board();
void print_mask(ul mask);
void print_board();
int bit_count(ul a);
ul generate_all_legal_moves();
ul transfer(ul put, int k);
void swap_board();
void reverse(ul put);
void undo();

#endif
