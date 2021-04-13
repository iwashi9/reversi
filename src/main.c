#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bitBoard.h"
#include "minimax.h"
#include "mcts.h"

// alpha_beta(6) == mcts(500,15)

int human_side;

ul random_put(Board board)
{
  ul legal = generate_all_legal_moves(board);
  ul put = get_random_move(legal);
  return put;
}

ul next_put_b(Board board)
{
  // return random_put(board);

  // return alpha_beta(board, 6);

  // if (bit_count(~(board.black_board | board.white_board)) <= 14)
  //   return alpha_beta(board, 14);
  // return mcts(board, 1000, 15);

  // int rest = bit_count(~(board.black_board | board.white_board));
  // printf("%d\n",rest);
  // if (rest <= 16)
  //   return alpha_beta(board, 16);
  // return mcts(board, 50000, 15);

  // return alpha_beta(board, 8);
  return mcts(board, 5000, 15);
}

ul next_put_w(Board board)
{
  // return random_put(board);
  // return alpha_beta(board, 8);
  // if (bit_count(~(board.black_board | board.white_board)) <= 14)
  //   return alpha_beta(board, 14);
  // return random_put(board);
  return mcts(board, 5000, 15);
}

int game(int verbose)
{
  Board board = init_board();
  History hist;
  hist.board[0] = board;
  hist.index = 1;

  while (1) {
    if (verbose) 
      print_board(board);

    if (is_pass(board)) {
      if (verbose) 
        printf("turn = %2d, move = Pass\n", board.turn);
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
        if (buf[0] == 'q') exit(0);
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
      if (board.turn == 1) {
        put = next_put_b(board);
      }
      else {
        put = next_put_w(board);
      }
    }

    if (verbose) {
      char buf[10];
      ul_to_str(put, buf);
      printf("turn = %2d, move = %s\n", board.turn, buf);
    }
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

int main(int argc, char **argv)
{
  if (argc > 1)
    human_side = atoi(argv[1]);
  else
    human_side = 0;

  unsigned seed = (unsigned int)time(NULL);
  printf("%u\n",seed);
  srand(seed);
  // srand(811);
  // game(1);

  int s = 0;
  int n = 10000;
  clock_t start = clock();
  for (int i = 0; i < n; ++i) {
    s += (game(0) > 0) ? 1 : 0; 
    printf("\033[2K\033[G%d/%d  black-white: %d-%d  %3.3lf %%",i+1,n,s,i+1-s,100*(double)s/(i+1));
    fflush(stdout);
  }
    printf("\n");
  clock_t end = clock();
  printf("elapsed time: %lf\n", (double)(end-start) / CLOCKS_PER_SEC);
  printf("black %lf %%\n",100 * (double)s/n);
  printf("white %lf %%\n",100 * (double)(n-s)/n);

  return 0;
}

