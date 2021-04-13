#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bitBoard.h"

Board init_board()
{
  Board board;
  board.black_board = 0x0000000810000000; board.white_board = 0x0000001008000000;
  // board.black_board = 0x0000001008000000; board.white_board = 0x0000000810000000;
  board.turn = 1;
  return board;
}


void print_mask(ul mask)
{
  int x, y;
  puts("\n  a b c d e f g h");
  for (y = 0; y < 8; y++) {
    printf("%d ", y + 1);
    for (x = 0; x < 8; x++) {
      const ul s = 1L << (63-8*y-x);
      char c = '-';
      if (mask & s) c = '1';
      printf("%c ", c);
    }
    putchar('\n');
  }
  putchar('\n');  
}


void print_board(Board board)
{
  int x, y;
  puts("\n  a b c d e f g h");
  for (y = 0; y < 8; y++) {
    printf("%d ", y + 1);
    for (x = 0; x < 8; x++) {
      const ul s = 1L << (63-8*y-x);
      char c = '-';
      if (board.black_board & s) c = 'B';
      if (board.white_board & s) c = 'W';
      printf("%c ", c);
    }
    putchar('\n');
  }
  putchar('\n');
}

int bit_count(ul a)
{
  a = (a & 0x5555555555555555) + ((a & 0xaaaaaaaaaaaaaaaa) >> 1);
  a = (a & 0x3333333333333333) + ((a & 0xcccccccccccccccc) >> 2);
  a = (a & 0x0f0f0f0f0f0f0f0f) + ((a & 0xf0f0f0f0f0f0f0f0) >> 4);
  a = (a & 0x00ff00ff00ff00ff) + ((a & 0xff00ff00ff00ff00) >> 8);
  a = (a & 0x0000ffff0000ffff) + ((a & 0xffff0000ffff0000) >> 16);
  a = (a & 0x00000000ffffffff) + ((a & 0xffffffff00000000) >> 32);
  return (int)a;
}


// 全合法手を生成する．
ul generate_all_legal_moves(Board board)
{
  ul player_bit_board = 
    (board.turn == 1) ? board.black_board : board.white_board;
  ul opponent_bit_board = 
    (board.turn == 1) ? board.white_board : board.black_board;

  ul bits, tmp, mask;
  ul blank = ~(player_bit_board | opponent_bit_board);
  ul legal = 0;

  // 左
  mask = 0x7e7e7e7e7e7e7e7e; // シフトで外に出た分を消すマスク
  bits = opponent_bit_board & mask;
  tmp = bits & (player_bit_board << 1);
  tmp |= bits & (tmp << 1);
  tmp |= bits & (tmp << 1);
  tmp |= bits & (tmp << 1);
  tmp |= bits & (tmp << 1);
  tmp |= bits & (tmp << 1);
  legal |= blank & (tmp << 1);

  // 右
  tmp = bits & (player_bit_board >> 1);
  tmp |= bits & (tmp >> 1);
  tmp |= bits & (tmp >> 1);
  tmp |= bits & (tmp >> 1);
  tmp |= bits & (tmp >> 1);
  tmp |= bits & (tmp >> 1);
  legal |= blank & (tmp >> 1);

  // 上
  mask = 0x00ffffffffffff00;
  bits = opponent_bit_board & mask;
  tmp = bits & (player_bit_board << 8);
  tmp |= bits & (tmp << 8);
  tmp |= bits & (tmp << 8);
  tmp |= bits & (tmp << 8);
  tmp |= bits & (tmp << 8);
  tmp |= bits & (tmp << 8);
  legal |= blank & (tmp << 8);

  // 下
  tmp = bits & (player_bit_board >> 8);
  tmp |= bits & (tmp >> 8);
  tmp |= bits & (tmp >> 8);
  tmp |= bits & (tmp >> 8);
  tmp |= bits & (tmp >> 8);
  tmp |= bits & (tmp >> 8);
  legal |= blank & (tmp >> 8);

  // 左上
  mask = 0x007e7e7e7e7e7e00;
  bits = opponent_bit_board & mask;
  tmp = bits & (player_bit_board << 9);
  tmp |= bits & (tmp << 9);
  tmp |= bits & (tmp << 9);
  tmp |= bits & (tmp << 9);
  tmp |= bits & (tmp << 9);
  tmp |= bits & (tmp << 9);
  legal |= blank & (tmp << 9);

  // 右上
  tmp = bits & (player_bit_board << 7);
  tmp |= bits & (tmp << 7);
  tmp |= bits & (tmp << 7);
  tmp |= bits & (tmp << 7);
  tmp |= bits & (tmp << 7);
  tmp |= bits & (tmp << 7);
  legal |= blank & (tmp << 7);

  // 右下
  tmp = bits & (player_bit_board >> 9);
  tmp |= bits & (tmp >> 9);
  tmp |= bits & (tmp >> 9);
  tmp |= bits & (tmp >> 9);
  tmp |= bits & (tmp >> 9);
  tmp |= bits & (tmp >> 9);
  legal |= blank & (tmp >> 9);

  // 左下
  tmp = bits & (player_bit_board >> 7);
  tmp |= bits & (tmp >> 7);
  tmp |= bits & (tmp >> 7);
  tmp |= bits & (tmp >> 7);
  tmp |= bits & (tmp >> 7);
  tmp |= bits & (tmp >> 7);
  legal |= blank & (tmp >> 7);

  return legal;
}

// 石を8方向に動かす．左から時計回り．
ul transfer(ul put, int k)
{
  switch(k) {
    case(0): // 左
      return (put << 1) & 0xfefefefefefefefe;
    case(1): // 左上
      return (put << 9) & 0xfefefefefefefe00;
    case(2): // 上
      return (put << 8) & 0xffffffffffffff00;
    case(3): // 右上
      return (put << 7) & 0x7f7f7f7f7f7f7f00;
    case(4): // 右
      return (put >> 1) & 0x7f7f7f7f7f7f7f7f;
    case(5): // 右下
      return (put >> 9) & 0x007f7f7f7f7f7f7f;
    case(6): // 下
      return (put >> 8) & 0x00ffffffffffffff;
    case(7): // 左下
      return (put >> 7) & 0x00fefefefefefefe;
    default:
      return 0;
  }
}

// turnの側に石を置き，反転する石の位置を返す．
ul reverse_(Board board, ul put) 
{
  ul player_bit_board = 
    (board.turn == 1) ? board.black_board : board.white_board;
  ul opponent_bit_board = 
    (board.turn == 1) ? board.white_board : board.black_board;

  ul rev = 0;
  for (int k = 0; k < 8; ++k) {
    ul rev_ = 0;
    ul mask = transfer(put, k);
    while ((mask != 0) && ((mask & opponent_bit_board) != 0)) {
      rev_ |= mask;
      mask = transfer(mask, k);
    }
    if (mask & player_bit_board) 
      rev |= rev_;
  }
  return rev;
}

// Board.turn側に石を置く．
Board reverse(Board board, ul put)
{
  ul player_bit_board = 
    (board.turn == 1) ? board.black_board : board.white_board;
  ul opponent_bit_board = 
    (board.turn == 1) ? board.white_board : board.black_board;

  ul rev = reverse_(board, put);
  player_bit_board ^= put | rev;
  opponent_bit_board ^= rev;

  board.black_board = (board.turn == 1) ? player_bit_board : opponent_bit_board;
  board.white_board = (board.turn == 1) ? opponent_bit_board : player_bit_board;
  board.turn *= -1;
  return board;
}

int is_pass(Board board)
{
  ul player_legal = generate_all_legal_moves(board);
  board.turn *= -1;
  ul opponent_legal = generate_all_legal_moves(board);
  return (player_legal == 0UL) && (opponent_legal != 0UL);
}

int is_finished(Board board)
{
  ul player_legal = generate_all_legal_moves(board);
  board.turn *= -1;
  ul opponent_legal = generate_all_legal_moves(board);
  return (player_legal == 0UL) && (opponent_legal == 0UL);
}

ul to_bit_board(int col, int row)
{
  return 1UL << (63-col-row*8);
}

ul get_random_move(ul moves)
{
  int n = bit_count(moves);
  int k = rand() % n;

  long lmoves = (long)moves;
  ul put = 0UL;
  while (lmoves != 0UL) {
    put = (ul)(lmoves & -lmoves);
    lmoves ^= put;
    // printf("%#18lx %#18lx\n",put,lmoves);
    if (k-- == 0) return put;
  }
  return 0UL;
}

// 適当に拾ってきた評価値
// int weight[8][8] = {
//   {30 , -12, 0 , -1, -1, 0 , -12, 30 },
//   {-12, -15, -3, -3, -3, -3, -15, -12},
//   {0  , -3 , 0 , -1, -1, 0 , -3 , 0  },
//   {-1 , -3 , -1, -1, -1, -1, -3 , -1 },
//   {-1 , -3 , -1, -1, -1, -1, -3 , -1 },
//   {0  , -3 , 0 , -1, -1, 0 , -3 , 0  },
//   {-12, -15, -3, -3, -3, -3, -15, -12},
//   {30 , -12, 0 , -1, -1, 0 , -12, 30 }
// };

// https://www.info.kindai.ac.jp/~takasi-i/thesis/2012_09-1-037-0133_S_Shiota_thesis.pdf
int weight[8][8] = {
  { 45, -11,  4, -1, -1,  4, -11,  45},
  {-11, -16, -1, -3, -3, -1, -16, -11},
  {  4,  -1,  2, -1, -1,  2,  -1,   4},
  {-1 , -3 , -1,  0,  0, -1,  -3,  -1},
  {-1 , -3 , -1,  0,  0, -1,  -3,  -1},
  {  4,  -1,  2, -1, -1,  2,  -1,   4},
  {-11, -16, -1, -3, -3, -1, -16, -11},
  { 45, -11,  4, -1, -1,  4, -11,  45}
};

int eval(Board board)
{
  int score = 0;

  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j) {
      if ((1UL << (8*i+j)) & board.black_board) score += weight[i][j];
      if ((1UL << (8*i+j)) & board.white_board) score -= weight[i][j];
    }

  score *= 2;

  board.turn = 1;
  score += bit_count(generate_all_legal_moves(board));
  board.turn = -1;
  score -= bit_count(generate_all_legal_moves(board));

  return score;
}


void ul_to_str(ul put, char *str)
{
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j) {
      if (put & (1UL << (63-j-i*8))) {
        str[0] = 'a' + j;
        str[1] = '1' + i;
        str[2] = '\0';
      }
    }
}