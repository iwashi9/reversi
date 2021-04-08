#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned long ul;

typedef struct history {
  int index;
  ul put[60];
  ul rev[60];
  int turn[60];
} History;

ul player_bit_board;
ul opponent_bit_board;
int turn;
History hist;

void init_board()
{
  player_bit_board = 0x0000000810000000;
  opponent_bit_board = 0x0000001008000000;
  turn = 1;
  hist.index = 0;
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

void print_board()
{
  int x, y;
  puts("\n  a b c d e f g h");
  for (y = 0; y < 8; y++) {
    printf("%d ", y + 1);
    for (x = 0; x < 8; x++) {
      const ul s = 1L << (63-8*y-x);
      char c = '-';
      if (player_bit_board & s) c = (turn == 1) ? 'B' : 'W';
      if (opponent_bit_board & s) c = (turn == 1) ? 'W' : 'B';
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
ul generate_all_legal_moves()
{
  int i;
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

// boardを入れ替える
void swap_board()
{
  player_bit_board ^= opponent_bit_board;
  opponent_bit_board ^= player_bit_board;
  player_bit_board ^= opponent_bit_board;
}

// player_bit_board側の石を置き，反転する石の位置を返す．
ul reverse_(ul put) 
{
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

// player_bit_board側の石を置く．
void reverse(ul put)
{
  ul rev = reverse_(put);
  player_bit_board ^= put | rev;
  opponent_bit_board ^= rev;

  hist.rev[hist.index] = rev;
  hist.put[hist.index] = put;
  hist.turn[hist.index] = turn;
  hist.index++;
  swap_board();
  turn *= -1;
}

void undo()
{
  if (hist.index == 0) return;
  hist.index--;
  if (turn != hist.turn[hist.index]) {swap_board(); turn *= -1;}
  player_bit_board ^= (hist.put[hist.index] | hist.rev[hist.index]);
  opponent_bit_board ^= hist.rev[hist.index];
}

int is_pass()
{
  ul player_legal = generate_all_legal_moves();
  swap_board();
  ul opponent_legal = generate_all_legal_moves();
  swap_board();
  return (player_legal == 0UL) && (opponent_legal != 0UL);
}

int is_finished()
{
  ul player_legal = generate_all_legal_moves();
  swap_board();
  ul opponent_legal = generate_all_legal_moves();
  swap_board();
  return (player_legal == 0UL) && (opponent_legal == 0UL);
}

void test()
{
  init_board();
  print_board();
  printf("%d\n", bit_count(player_bit_board));
  printf("%d\n", bit_count(opponent_bit_board));
  clock_t start = clock();
  for (int i = 0; i < 1000000; ++i) generate_all_legal_moves();
  clock_t end = clock();
  printf("%f\n", (double)(end-start) / CLOCKS_PER_SEC);
  ul legal = generate_all_legal_moves();
  print_mask(legal);
  ul put = 0x0000100000000000;
  reverse(put);
  print_board();
  undo();
  print_board();
}

ul to_bit_board(int col, int row)
{
  return 1UL << (63-col-row*8);
}

ul get_random_move(ul moves)
{
  int n = bit_count(moves);
  int k = rand() % n + 1;
  for (int i = 0; i < 64; ++i) {
    if (moves & (1UL << i)) {
      k--;
      if (k == 0) return (1UL << i);
    }
  }
  return 0UL;
}

int weight[8][8] = {
  {30, -12, 0, -1, -1, 0, -12, 30},
  {-12 -15 -3, -3, -3, -3, -15, -12},
  {0, -3, 0, -1, -1, 0, -3, 0},
  {-1, -3, 1, -1, -1, -1, -3, -1},
  {-1, -3, 1, -1, -1, -1, -3, -1},
  {0, -3, 0, -1, -1, 0, -3, 0},
  {-12 -15 -3, -3, -3, -3, -15, -12},
  {30, -12, 0, -1, -1, 0, -12, 30}
};

int eval()
{
  int score = 0;
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j) {
      score += weight[i][j] * bit_count((1 << (8*i+j)) & player_bit_board);
      score -= weight[i][j] * bit_count((1 << (8*i+j)) & opponent_bit_board);
    }
  return score;
}

ul best_put_;
ul negamax(int depth, int max_depth, int turn_)
{
  if (depth == max_depth)
    return eval() * turn_;
  if (is_pass()) 
    return -100*turn_;
  if (is_finished()) 
    return bit_count(player_bit_board) - bit_count(opponent_bit_board);

  int best = -1e9;
  ul legal = generate_all_legal_moves();
  int n = bit_count(legal);
  for (int i = 0; i < 64; ++i) {
    ul put = 1UL << i;
    if (legal & put) {
      reverse(put);
      int v = -negamax(depth+1, max_depth, -turn_);
      if (v > best) {
        best = v;
        if (depth == 0) 
          best_put_ = put;
      }
      undo();
    }
  }
  return best;
}

ul best_put()
{
  negamax(0, 3, turn);
  return best_put_;
}

void game()
{
  srand((unsigned int)time(NULL));
  init_board();
  int human_side = 1;
  while (1) {
    print_board();

    if (is_pass()) {
      printf("turn = %2d, move = Pass\n", turn);
      turn *= -1; 
      swap_board();
      continue;
    }
    if (is_finished()) break;

    ul put;
    if (turn == human_side) {
      char buf[1000];
      while (1) { // 入力
        printf("Where? ");
        scanf("%s", buf);
        if (buf[0] == 'u') break;
        put = to_bit_board(buf[0]-'a', buf[1]-'1');
        if (generate_all_legal_moves() & put) break;
        printf("%s is not a legal move!\n\n", buf);
      }
      if (buf[0] == 'u') { // 自分のターンに戻るまでundo()
        do undo(); while (turn != human_side && hist.index != 0);
        continue;
      }
    } 
    else {
      // ul legal = generate_all_legal_moves();
      // put = get_random_move(legal);
      put = best_put();
    }

    printf("turn = %2d, put = %#017lx\n", turn, put);
    reverse(put);
  }

  if (turn == -1) {turn *= -1; swap_board();}
  int black_count = bit_count(player_bit_board);
  int white_count = bit_count(opponent_bit_board);
  if (black_count == white_count) printf("%d - %d  Draw.\n", black_count, white_count);
  if (black_count > white_count) printf("%d - %d  Black Win!\n", black_count, white_count);
  if (black_count < white_count) printf("%d - %d  White Win!\n", black_count, white_count);
}

int main(void)
{
  // test();
  // clock_t start = clock();
  // for (int i = 0; i < 100000; ++i) game();
  // clock_t end = clock();
  // printf("%f\n", (double)(end-start) / CLOCKS_PER_SEC);
  game();
  return 0;
}


