#ifndef GAME_H
#define GAME_H

#define SUDOKU_QUADRANT 3
#define SUDOKU_NUM_GROUP_ITEMS (SUDOKU_QUADRANT * SUDOKU_QUADRANT)
#define SUDOKU_NUM_ITEMS (SUDOKU_NUM_GROUP_ITEMS * SUDOKU_NUM_GROUP_ITEMS)

#define MENU_OPT_EXIT 0
#define MENU_OPT_GEN_FILE 1
#define MENU_OPT_START_GAME 2

typedef struct SUDOKU_NODE {
  int value;
  int row; 
  int column;
  struct SUDOKU_NODE** horizontal;
  struct SUDOKU_NODE** vertical;
  struct SUDOKU_NODE** group;
} SUDOKU_NODE;

typedef struct SUDOKU {
  SUDOKU_NODE nodes[SUDOKU_NUM_ITEMS];
  SUDOKU_NODE** horizontals[SUDOKU_NUM_GROUP_ITEMS];
  SUDOKU_NODE** verticals[SUDOKU_NUM_GROUP_ITEMS];
  SUDOKU_NODE** groups[SUDOKU_NUM_GROUP_ITEMS];
} SUDOKU;

SUDOKU* new_sudoku();
void free_sudoku(SUDOKU* sudoku);
void show_sudoku(const SUDOKU* sudoku);
int generate_sudoku_file();
int read_sudoku_from_file(SUDOKU* sudoku);
int get_node_possibilities(const SUDOKU_NODE* node, int* array_possibilities);
SUDOKU_NODE* get_node_from_coordinates(SUDOKU* sudoku, int row, int column);

int menu();
int gameloop();

#endif