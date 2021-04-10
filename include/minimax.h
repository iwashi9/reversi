#ifndef MINIMAX_H
#define MINIMAX_H
#include "bitBoard.h"

int max_node(Board board, int depth, int max_depth, ul* best_put);
int min_node(Board board, int depth, int max_depth, ul* best_put);
ul minimax(Board board, int max_depth);

int negamax_(Board board, int depth, int max_depth, ul* best_put);
ul negamax(Board board, int max_depth);

int max_node_alpha_beta(Board board, int depth, int max_depth, ul* best_put, int a, int b);
int min_node_alpha_beta(Board board, int depth, int max_depth, ul* best_put, int a, int b);
ul alpha_beta(Board board, int max_depth);

#endif
