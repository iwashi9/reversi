#ifndef MOVE_H
#define MOVE_H

typedef struct intpair
{
  int first;
  int second;
} IntPair;

int is_legal_move(const int side, const IntPair square);
int place_disk(const int side, const IntPair square);
int generate_all_legal_moves(const int side, IntPair legal_moves[]);

#endif
