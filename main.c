#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "move.h"

static void undo_board(int n);
static void init_board();
static void print_board();

int board[8][8];  // 8x8 squares: 0 -> empty; 1 -> black; -1 -> white
IntPair moves_history[60];
int history_index;

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

static int evaluate(int turn)
{
  int score = 0;
  int x, y;
  for (y = 0; y < 8; y++)
    for (x = 0; x < 8; x++) {
      if (board[x][y] == turn) score += weight[x][y];
    }
  return score;
}

IntPair best_move;
static int negamax(int depth, int max_depth, int turn)
{
  if (depth == max_depth)
    return evaluate(turn) * turn;

  int best = -1e9;
  IntPair legal_moves[60];
  const int nmoves = generate_all_legal_moves(turn, legal_moves);
  for (int i = 0; i < nmoves; ++i) {
    // printf("%d %d\n", legal_moves[i].first, legal_moves[i].second);
    place_disk(turn, legal_moves[i]); 
    moves_history[history_index++] = legal_moves[i];
    // print_board();
    int v = -negamax(depth+1, max_depth, -turn);
    if (v > best) {
      best = v;
      if (depth == 0) 
        best_move = legal_moves[i];
    }
    undo_board(1);
  }
  return best;
}

static void init_board()
{
  int x, y;
  for (y = 0; y < 8; y++) 
    for (x = 0; x < 8; x++)
      board[x][y] = 0;

  board[4][3] = board[3][4] =  1;  // black disks
  board[3][3] = board[4][4] = -1;  // white disks
}

static void print_board()
{
  int x, y;
  puts("\n  a b c d e f g h");
  for (y = 0; y < 8; y++) {
    printf("%d ", y + 1);
    for (x = 0; x < 8; x++) {
      const int d = board[x][y];
      char c = '-';
      if (d ==  1) c = 'B';
      if (d == -1) c = 'W';
      printf("%c ", c);
    }
    putchar('\n');
  }
  putchar('\n');
}

static void undo_board(int n) // 履歴を初めから辿ってn手局面を巻き戻し
{
  init_board();
  history_index -= n;
  if (history_index < 0) 
    history_index = 0;

  int turn = -1;
  int passed = 0;
  for (int i = 0; i < history_index; ++i) {
    if (!passed) turn *= -1;
    IntPair legal_moves[60];
    const int nmoves = generate_all_legal_moves(turn, legal_moves);
    if (nmoves == -1) break;     // no empty square
    if (nmoves ==  0) { // pass
      if (passed == 0) {passed = 1; continue;}
      else break;
    }
    place_disk(turn, moves_history[i]);
    passed = 0;
  }
}

static void win_loss_judge()
{
  int black_count = 0;
  int white_count = 0;
  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++) {
      if (board[x][y] == 1) black_count++;
      else if (board[x][y] == -1) white_count++;
    }

  if (black_count == white_count) printf("Draw.\n");
  else if (black_count < white_count) printf("White win!\n");
  else if (black_count > white_count) printf("Black win!\n");
}

int main(int argc, char **argv)
{
  const int human_side = (argc >= 2) ? atoi(argv[1]) : 0;

  init_board();
  srand((unsigned int)time(NULL));

  int passed = 0; // 両者パスの判定用
  int undo = 0;
  int turn;
  for (turn = 1;; turn *= -1) {
    print_board();

    IntPair legal_moves[60];
    const int nmoves = generate_all_legal_moves(turn, legal_moves);
    if (nmoves == -1) break;     // no empty square
    if (nmoves ==  0) { // pass
      printf("turn = %d, move = Pass\n", turn);
      if (passed == 0) {passed = 1; continue;}
      else break;
    }

    IntPair move;
    if (turn == human_side) {
      while (1) {
  printf("Where? ");
  char buf[1000];
  scanf("%s", buf);

  if (buf[0] == 'z') {
    undo_board(2);
    undo = 1;
    break;
  }

  move.first  = buf[0] - 'a';
  move.second = buf[1] - '1';

  if (is_legal_move(turn, move)) break;
  printf("%s is not a legal move!\n\n", buf);
      } 
    } else {
      // move = legal_moves[0];  // choose the first legal move
      // move = legal_moves[rand() % nmoves];  // choose random legal move
      negamax(0,3,turn);
      move = best_move;
      printf("turn = %d, move = %c%c\n", 
       turn, 'a' + move.first, '1' + move.second);
    }

    if (undo) {undo = 0; continue;}

    place_disk(turn, move);
    passed = 0;
    moves_history[history_index++] = move;
  }

  win_loss_judge();

  return 0;
}
