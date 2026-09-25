#include <stdlib.h>
#include <stdio.h>
#include "game.h"

SUDOKU* new_sudoku() {
  SUDOKU* sudoku = malloc(sizeof(SUDOKU));

  for (int i=0; i<SUDOKU_NUM_ITEMS; i++) {
    sudoku->nodes[i].value = 0;
    sudoku->nodes[i].row = i / SUDOKU_NUM_GROUP_ITEMS;
    sudoku->nodes[i].column = i % SUDOKU_NUM_GROUP_ITEMS;
  }

  for (int i=0; i<SUDOKU_NUM_GROUP_ITEMS; i++) {
    sudoku->horizontals[i] = malloc(sizeof(SUDOKU_NODE*) * SUDOKU_NUM_GROUP_ITEMS);
    sudoku->verticals[i] = malloc(sizeof(SUDOKU_NODE*) * SUDOKU_NUM_GROUP_ITEMS);
    sudoku->groups[i] = malloc(sizeof(SUDOKU_NODE*) * SUDOKU_NUM_GROUP_ITEMS);

    for (int j=0; j<SUDOKU_NUM_GROUP_ITEMS; j++) {
      // populating horizontal values
      sudoku->horizontals[i][j] = &sudoku->nodes[i * SUDOKU_NUM_GROUP_ITEMS + j];
      
      // populating vertical vallues
      sudoku->verticals[i][j] = &sudoku->nodes[j * SUDOKU_NUM_GROUP_ITEMS + i];
    }
  }

  // populating groups
  for (int group_row=0; group_row < SUDOKU_QUADRANT; group_row++) {
    for (int group_col=0; group_col < SUDOKU_QUADRANT; group_col++) {
      for (int cell_row=0; cell_row < SUDOKU_QUADRANT; cell_row++) {
        for (int cell_col=0; cell_col < SUDOKU_QUADRANT; cell_col++) {
          int 
            row = group_row * SUDOKU_QUADRANT + cell_row,
            col = group_col * SUDOKU_QUADRANT + cell_col,
            index = row * SUDOKU_NUM_GROUP_ITEMS + col,
            i = group_row * SUDOKU_QUADRANT + group_col,
            j = cell_row * SUDOKU_QUADRANT + cell_col;
          sudoku->groups[i][j] = &sudoku->nodes[index];
        }
      }
    }
  }

  return sudoku;
}

void free_sudoku(SUDOKU* sudoku) {
  for (int i=0; i<SUDOKU_NUM_GROUP_ITEMS; i++) {
    free(sudoku->horizontals[i]);
    free(sudoku->verticals[i]);
    free(sudoku->groups[i]);
  }

  free(sudoku);
}

void show_sudoku(const SUDOKU* sudoku) {
  printf("------------------------------\n\n");
  printf("            SUDOKU\n\n");
  printf("     1 2 3   4 5 6   7 8 9\n");
  printf("   -------------------------\n");

  for (int row=0; row < SUDOKU_NUM_GROUP_ITEMS; row++) {
    printf(" %d |", row+1);

    for (int col=0; col < SUDOKU_NUM_GROUP_ITEMS; col++) {
      const int val = sudoku->nodes[row * SUDOKU_NUM_GROUP_ITEMS + col].value;
      
      printf(val? " %d" : "  ", val);
      if ((col+1) % SUDOKU_QUADRANT == 0)
        printf(" |");
    }
    
    printf("\n");

    if ((row+1) % SUDOKU_QUADRANT == 0)
      printf("   -------------------------\n");
  }
  
  printf("\n\n------------------------------\n");
}

int generate_sudoku_file() {
  FILE* file = fopen("sudoku.txt", "w");

  if (file == NULL) {
    printf("it wasn't possible to create the sudoku file\n");
    return 0;
  }
  
  for (int i=0; i<9; i++)
    fprintf(file, "0 0 0 0 0 0 0 0 0\n");
  
  fclose(file);
  return 1;
}

int read_sudoku_from_file(SUDOKU* sudoku) {
  FILE* file = fopen("sudoku.txt", "r");

  if (file == NULL) {
    printf("it wasn't possible to open the file\n");
    return 0;
  }

  char buffer[256];
  
  // TODO: adaptar para suporte a sudokus com outras estruturas alem de 3x3
  for (int row=0; fgets(buffer, sizeof(buffer), file) != NULL && row<SUDOKU_NUM_GROUP_ITEMS; row++) {
    sscanf(buffer, "%d %d %d %d %d %d %d %d %d\n",
      &sudoku->horizontals[row][0]->value,
      &sudoku->horizontals[row][1]->value,
      &sudoku->horizontals[row][2]->value,
      &sudoku->horizontals[row][3]->value,
      &sudoku->horizontals[row][4]->value,
      &sudoku->horizontals[row][5]->value,
      &sudoku->horizontals[row][6]->value,
      &sudoku->horizontals[row][7]->value,
      &sudoku->horizontals[row][8]->value
    );
  }
   
  fclose(file);
  return 1;
}

int menu() {
  system("cls");
  printf("----------------------------------\n");
  printf("\nWelcome to your sudoku\n");
  printf("\n  1. Generate empty sudoku file\n");
  printf("  2. Start game\n");
  printf("  0. Exit\n");
  printf("\n----------------------------------\n");

  int option;
  printf("\nChoose an option: ");
  scanf("%d", &option);
  
  while (!(option == MENU_OPT_EXIT || option == MENU_OPT_GEN_FILE || option == MENU_OPT_START_GAME)) {
    printf("That is not valid! Choose an option: ");
    scanf("%d", &option);
  }

  return option;
}

int option_game() {
  system("cls");
  
  SUDOKU* sudoku = new_sudoku();
  if (!sudoku) {
    return 0;
  }

  if (!read_sudoku_from_file(sudoku)) {
    free_sudoku(sudoku);
    return 0;
  }

  show_sudoku(sudoku);
  free_sudoku(sudoku);
  system("PAUSE");

  return 1;
}

int gameloop() {
  int option;

  while (option = menu()) {
    if (option == MENU_OPT_GEN_FILE && !generate_sudoku_file())
      break;
    else if (option == MENU_OPT_START_GAME && !option_game())
      break;
  }
  
  return option != MENU_OPT_EXIT;
}