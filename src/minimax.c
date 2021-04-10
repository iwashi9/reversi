#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "minimax.h"
#include "bitBoard.h"


int max_node(Board board, int depth, int max_depth, ul* best_put)
{
  if (depth == max_depth)
    return eval(board);
  if (is_finished(board))
    return bit_count(board.black_board) - bit_count(board.white_board);
  if (is_pass(board)) {
    board.turn *= -1;
    return min_node(board, depth+1, max_depth, NULL);
  }

  int best = -1e9;
  ul legal = generate_all_legal_moves(board);
  int n = bit_count(legal);
  for (int i = 0; i < 64; ++i) {
    ul put = 1UL << i;
    if (legal & put) {
      Board new_board = reverse(board, put);
      int v = min_node(new_board, depth+1, max_depth, NULL);
      if (v > best) {
        best = v;
        if (depth == 0) 
          *best_put = put;
      }
      n--;
      if (n <= 0) break;
    }
  }
  return best;
}

int min_node(Board board, int depth, int max_depth, ul* best_put)
{
  if (depth == max_depth)
    return eval(board);
  if (is_finished(board))
    return bit_count(board.black_board) - bit_count(board.white_board);
  if (is_pass(board)) {
    board.turn *= -1;
    return max_node(board, depth+1, max_depth, NULL);
  }

  int best = 1e9;
  ul legal = generate_all_legal_moves(board);
  int n = bit_count(legal);
  for (int i = 0; i < 64; ++i) {
    ul put = 1UL << i;
    if (legal & put) {
      Board new_board = reverse(board, put);
      int v = max_node(new_board, depth+1, max_depth, NULL);
      if (v < best) {
        best = v;
        if (depth == 0) 
          *best_put = put;
      }
      n--;
      if (n <= 0) break;
    }
  }
  return best;
}

ul minimax(Board board, int max_depth)
{
  ul best_put;
  if (board.turn == 1)
    max_node(board, 0, max_depth, &best_put);
  else
    min_node(board, 0, max_depth, &best_put);
  return best_put;
}




int negamax_(Board board, int depth, int max_depth, ul* best_put)
{
  if (depth == max_depth)
    return eval(board)*board.turn;
  if (is_pass(board)) {
    board.turn *= -1;
    return -negamax_(board, depth+1, max_depth, NULL);
  }
  if (is_finished(board))
    return bit_count(board.black_board) - bit_count(board.white_board);

  int best = -1e9;
  ul legal = generate_all_legal_moves(board);
  int n = bit_count(legal);
  for (int i = 0; i < 64; ++i) {
    ul put = 1UL << i;
    if (legal & put) {
      Board new_board = reverse(board, put);
      int v = -negamax_(new_board, depth+1, max_depth, NULL);
      if (v > best) {
        best = v;
        if (depth == 0) 
          *best_put = put;
      }
      n--;
      if (n <= 0) break;
    }
  }
  return best;
}

ul negamax(Board board, int max_depth)
{
  ul best_put;
  negamax_(board, 0, max_depth, &best_put);
  return best_put;
}



int max_node_alpha_beta(Board board, int depth, int max_depth, ul* best_put, int a, int b)
{
  if (depth == max_depth)
    return eval(board);
  if (is_finished(board))
    return bit_count(board.black_board) - bit_count(board.white_board);
  if (is_pass(board)) {
    board.turn *= -1;
    return min_node_alpha_beta(board, depth+1, max_depth, NULL, a, b);
  }

  ul legal = generate_all_legal_moves(board);
  int n = bit_count(legal);
  for (int i = 0; i < 64; ++i) {
    ul put = 1UL << i;
    if (legal & put) {
      Board new_board = reverse(board, put);
      int v = min_node_alpha_beta(new_board, depth+1, max_depth, NULL, a, b);
      if (v > a) {
        a = v;
        if (depth == 0) 
          *best_put = put;
        if (a >= b)
          break;
      }
      n--;
      if (n <= 0) break;
    }
  }
  return a;
}

int min_node_alpha_beta(Board board, int depth, int max_depth, ul* best_put, int a, int b)
{
  if (depth == max_depth)
    return eval(board);
  if (is_finished(board))
    return bit_count(board.black_board) - bit_count(board.white_board);
  if (is_pass(board)) {
    board.turn *= -1;
    return max_node_alpha_beta(board, depth+1, max_depth, NULL, a, b);
  }

  ul legal = generate_all_legal_moves(board);
  int n = bit_count(legal);
  for (int i = 0; i < 64; ++i) {
    ul put = 1UL << i;
    if (legal & put) {
      Board new_board = reverse(board, put);
      int v = max_node_alpha_beta(new_board, depth+1, max_depth, NULL, a, b);
      if (v < b) {
        b = v;
        if (depth == 0) 
          *best_put = put;
        if (a >= b)
          break;
      }
      n--;
      if (n <= 0) break;
    }
  }
  return b;
}

ul alpha_beta(Board board, int max_depth)
{
  ul best_put;
  if (board.turn == 1)
    max_node_alpha_beta(board, 0, max_depth, &best_put, -1e9, 1e9);
  else
    min_node_alpha_beta(board, 0, max_depth, &best_put, -1e9, 1e9);
  return best_put;
}
