#ifndef MCTS_H
#define MCTS_H

#include "bitBoard.h"

typedef struct mcts_node {
  Board board;
  ul *legal_moves;
  int n_legals;
  int n_all;
  int *n_playout;
  double *rewards;
  struct mcts_node **nodes;
} mctsNode;

mctsNode init_node(Board board);
int ucb_choice(mctsNode *node);
double search(mctsNode *node, int threshold);
double random_search(mctsNode *node, int parent_turn);
double rollout(Board board);
ul mcts(Board board, int n, int threshold);
ul mcts_t(Board board, double t, int threshold);
ul mcts_random(Board board, int n);
void print_node(mctsNode *node);

#endif