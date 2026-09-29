#include <stdlib.h>
#include <stdio.h>
#include "game.h"

SUDOKU* new_sudoku() {
  SUDOKU* sudoku = malloc(sizeof(SUDOKU));
  SUDOKU_NODE* node;
  sudoku->num_empty_nodes = SUDOKU_NUM_GROUP_ITEMS;
  
  for (int i=0; i<SUDOKU_NUM_ITEMS; i++) {
    sudoku->indexes_empty_nodes[i] = i;
    
    node = &sudoku->nodes[i];
    node->row = i / SUDOKU_NUM_GROUP_ITEMS;
    node->column = i % SUDOKU_NUM_GROUP_ITEMS;
    node->index = i;
    node->num_possibilities = SUDOKU_NUM_GROUP_ITEMS;
    node->value = 0;

    for (int j=0; j<SUDOKU_NUM_GROUP_ITEMS; j++) {
      node->possibilities[j] = j+1;
      node->possibilities_by_index[j] = 1;
    }
    
  }

  for (int i=0; i<SUDOKU_NUM_GROUP_ITEMS; i++) {
    sudoku->horizontals[i] = malloc(sizeof(SUDOKU_NODE*) * SUDOKU_NUM_GROUP_ITEMS);
    sudoku->verticals[i] = malloc(sizeof(SUDOKU_NODE*) * SUDOKU_NUM_GROUP_ITEMS);
    sudoku->groups[i] = malloc(sizeof(SUDOKU_NODE*) * SUDOKU_NUM_GROUP_ITEMS);

    for (int j=0; j<SUDOKU_NUM_GROUP_ITEMS; j++) {
      // populating horizontal values
      node = &sudoku->nodes[i * SUDOKU_NUM_GROUP_ITEMS + j];
      sudoku->horizontals[i][j] = node;
      node->horizontal = sudoku->horizontals[i];

      // populating vertical values
      node = &sudoku->nodes[j * SUDOKU_NUM_GROUP_ITEMS + i];
      sudoku->verticals[i][j] = node;
      node->vertical = sudoku->verticals[i];
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
        
          node = &sudoku->nodes[index];
          sudoku->groups[i][j] = node;
          node->group = sudoku->groups[i];
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
  
  for (int i=0; i<SUDOKU_NUM_ITEMS; i++)
    update_node_possibilities(&sudoku->nodes[i]);

  update_empty_nodes(sudoku);
  fclose(file);
  return 1;
}

void update_empty_nodes(SUDOKU *sudoku)
{
  sudoku->num_empty_nodes = 0;
  for (int i=0, array_index=0; i<SUDOKU_NUM_ITEMS; i++)
    if (!sudoku->nodes[i].value) {
      sudoku->indexes_empty_nodes[array_index++] = i;
      sudoku->num_empty_nodes++;
    }
}

void update_node_possibilities(SUDOKU_NODE *node)
{
  if (node->value)
    return;

  // int count_possibility_numbers[SUDOKU_NUM_GROUP_ITEMS] = {0};

  for (int i=0; i<SUDOKU_NUM_GROUP_ITEMS; i++) {
    SUDOKU_NODE* group_parent = node->group[i];
    int
      g_val = group_parent->value,
      v_val = node->vertical[i]->value,
      h_val = node->horizontal[i]->value;

    if (g_val) node->possibilities_by_index[g_val-1] = 0;
    // else for (int j=0; j<group_parent->num_possibilities; j++)
    //   count_possibility_numbers[group_parent->possibilities[j]-1]++;
    
    if (v_val) node->possibilities_by_index[v_val-1] = 0;
    if (h_val) node->possibilities_by_index[h_val-1] = 0;
  }

  int count = 0;
  for (int i=0; i<SUDOKU_NUM_GROUP_ITEMS; i++) {
    if (node->possibilities_by_index[i])
      node->possibilities[count++] = i+1;
  }

  // TODO: otimizar loops
  // for (int i=0; i<SUDOKU_NUM_GROUP_ITEMS; i++) {
  //   if (count_possibility_numbers[i] == 1 && node->possibilities_by_index[i]) {
  //     node->num_possibilities = 1;
  //     node->possibilities[0] = i+1;
  //     return;
  //   }
  // }
  
  node->num_possibilities = count;
}

int solve_sudoku(SUDOKU* sudoku, SUDOKU_NODE** out_updates)
{
  int array_updates = 0,
    changes_found,
    indexes_updated[SUDOKU_NUM_ITEMS] = {0};

  do {
    changes_found = 0;

    for (int i=0; i<sudoku->num_empty_nodes; i++) {
      if (indexes_updated[i])
        continue;
      SUDOKU_NODE* node = &sudoku->nodes[sudoku->indexes_empty_nodes[i]];

      if (node->num_possibilities > 1)
        update_node_possibilities(node);
      
      if (node->num_possibilities == 1) {
        if (out_updates) {
          out_updates[array_updates] = node;
        }

        changes_found = 1;
        array_updates++;
        node->value = node->possibilities[0];
        indexes_updated[i] = 1;

        // printf("solved: <Node row=%d column=%d value=%d>\n", node->row+1, node->column+1, node->value);
      }
      // else {
        // printf("pending: <Node row=%d column=%d possibilities(%d)=[", node->row+1, node->column+1, node->num_possibilities);
        // for (int j=0; j<node->num_possibilities; j++)
        //   printf("%d ", node->possibilities[j]);
        // printf("]>\n");
      // }
    }
  } while (changes_found);

  // updating empty node variables
  if (array_updates)
    update_empty_nodes(sudoku);

  return array_updates;
}

const SUDOKU_NODE* get_node_from_coordinates(const SUDOKU* sudoku, int row, int column)
{
  int index = (row-1) * SUDOKU_NUM_GROUP_ITEMS + (column-1);
  if (index >= SUDOKU_NUM_ITEMS)
    return NULL;
  return &sudoku->nodes[index];
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
  
  SUDOKU* sudoku = new_sudoku();
  if (!sudoku) {
    return 0;
  }
  
  if (!read_sudoku_from_file(sudoku)) {
    free_sudoku(sudoku);
    return 0;
  }
  
  system("cls");
  show_sudoku(sudoku);
  printf("\n  1. See possibilities by position\n  2. Auto solve\n  3. Test\n  0. Back to menu\n\nOption: ");
  
  // getting the option
  int option;
  scanf("%d", &option);
  while (!(option == 1 || option == 2 || option == 3 || option == 0)) {
    printf("\033[A\033[2KOption: ");
    scanf("%d", &option);
  }

  if (option == 1) {
    int row = 0, col = 0;
  
    for (;;) {
  
      // not first loop, go cursors up and clean the line 5 times
      if (row != 0)
        printf("\033[A\033[2K\033[A\033[2K\033[A\033[2K\033[A\033[2K\033[A\033[2K");
  
      printf("Row: ");
      scanf("%d", &row);
    
      if (!row)
        break;
    
      printf("Column: ");
      scanf("%d", &col);
  
      if (row < 1 || col < 1 || row > SUDOKU_NUM_GROUP_ITEMS || col > SUDOKU_NUM_GROUP_ITEMS) {
        printf("Invalid entries\n\n");
        system("PAUSE");
        continue;
      }
  
      const SUDOKU_NODE* node = get_node_from_coordinates(sudoku, row, col);
      if (node->value) {
        printf("The value %d is already set\n\n", node->value);
        system("PAUSE");
        continue;
      }
      
      printf("The possibilites to row %d and column %d are ", row, col);
      for (int i=0; i<node->num_possibilities; i++)
        printf("%d ", node->possibilities[i]);
  
      printf("\n\n");
      system("PAUSE");
    }
  }
  else if (option == 2) {
    solve_sudoku(sudoku, NULL);
    show_sudoku(sudoku);

    printf("\n");
    system("PAUSE");
  }
  else if (option == 3) {
    show_sudoku(sudoku);
    const SUDOKU_NODE* node = get_node_from_coordinates(sudoku, 5, 3);

    if (node->value) {
      printf("<Node row=%d column=%d value=%d>\n\n", node->row+1, node->column+1, node->value);
      system("PAUSE");
      free_sudoku(sudoku);
      return 1;
    }

    printf("<Node row=%d column=%d possibilities(%d)=[", node->row+1, node->column+1, node->num_possibilities);
    for (int j=0; j<node->num_possibilities; j++)
      printf("%d ", node->possibilities[j]);
    printf("]>\n");

    int count_possibility_numbers[SUDOKU_NUM_GROUP_ITEMS] = {0};

    for (int i=0; i<SUDOKU_NUM_GROUP_ITEMS; i++) {
      SUDOKU_NODE* group_parent = node->group[i];
      if (!group_parent->value) {
        printf("Index: %d [", i);
        for (int j=0; j<group_parent->num_possibilities; j++) {
          count_possibility_numbers[group_parent->possibilities[j]-1]++;
          printf("%d ", group_parent->possibilities[j]);
        }
        printf("]\n");
      }
    }

    printf("<Group count_possibility_numbers=[");
    for (int j=0; j<SUDOKU_NUM_GROUP_ITEMS; j++)
      printf("%d ", count_possibility_numbers[j]);
    printf("]>\n");

    for (int i=0; i<SUDOKU_NUM_GROUP_ITEMS; i++) {
      if (count_possibility_numbers[i] == 1) {
        printf("the number %d has just one valid place\n\n", i+1);
      }
    }

    system("PAUSE");
  }

  free_sudoku(sudoku);
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