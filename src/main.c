#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bitBoard.h"
#include "minimax.h"

ul next_put(Board board)
{
  int max_depth = 6;
  ul put;
  put = alpha_beta(board, max_depth);
  // put = minimax(board, max_depth);
  return put;
}

int game(int verbose)
{
  Board board = init_board();
  board.turn *= 1;
  History hist;
  hist.board[0] = board;
  hist.index = 1;

  int human_side = 0;

  while (1) {
    if (verbose) print_board(board);

    if (is_pass(board)) {
      if (verbose) printf("turn = %2d, move = Pass\n", board.turn);
      board.turn *= -1;
      continue;
    }
    if (is_finished(board)) break;

    ul put;
    if (board.turn == human_side) {
      char buf[1000];
      while (1) { // 入力
        printf("Where? ");
        scanf("%s", buf);
        if (buf[0] == 'u') break;
        if (buf[0] == 'l') {
          print_mask(generate_all_legal_moves(board));
          continue;
        }
        put = to_bit_board(buf[0]-'a', buf[1]-'1');
        if (generate_all_legal_moves(board) & put) break;
        printf("%s is not a legal move!\n\n", buf);
      }
      if (buf[0] == 'u') { // 自分のターンに戻るまでundo()
        do {
          hist.index--;
          board = hist.board[hist.index];
        } while (hist.index != 0 && board.turn != human_side);
        continue;
      }
    }

    else {
      if (board.turn == -2) {
        ul legal = generate_all_legal_moves(board);
        put = get_random_move(legal);
      }
      else {
        put = next_put(board);
      }
    }

    if (verbose) printf("turn = %2d, put = %#017lx\n", board.turn, put);
    board = reverse(board, put);
    hist.board[hist.index++] = board;
  }

  int black_count = bit_count(board.black_board);
  int white_count = bit_count(board.white_board);
  if (verbose) {
    if (black_count == white_count) printf("%d - %d  Draw.\n", black_count, white_count);
    if (black_count > white_count) printf("%d - %d  Black Win!\n", black_count, white_count);
    if (black_count < white_count) printf("%d - %d  White Win!\n", black_count, white_count);
  }
  return black_count-white_count;
}

int main(void)
{
  srand((unsigned int)time(NULL));
  game(1);
}

