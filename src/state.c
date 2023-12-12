#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  game_state_t* state = malloc(sizeof(game_state_t));
  state->num_rows = 18;
  state->num_snakes = 1;
  state->board = malloc(sizeof(char*) * state->num_rows);
  state->snakes = malloc(sizeof(snake_t) * state->num_snakes);

  // Initialize the board
  for(int i = 0; i < state->num_rows; i++) {
    state->board[i] = malloc(sizeof(char) * 21);
  }
  strcpy(state->board[0], "####################");
  strcpy(state->board[1], "#                  #");
  strcpy(state->board[2], "# d>D    *         #");
  strcpy(state->board[17], "####################");
  for (int i = 3; i < 17; i++) {
    strcpy(state->board[i], "#                  #");
  }

  // Initialize the snakes
  state->snakes[0].tail_row = 2;
  state->snakes[0].tail_col = 2;
  state->snakes[0].head_row = 2;
  state->snakes[0].head_col = 4;
  state->snakes[0].live = true;
  return state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
  for(int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for(int i = 0; i < state->num_rows; i++) {
    fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  if(c == 'w' || c == 'a' || c =='s' || c == 'd') {
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if(c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x') {
    return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  if (c == 'w' || c == 'a' || c =='s' || c == 'd' || c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == '^' || c == '<' || c == '>' || c == 'v') {
    return true;
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch (c) {
    case '^':
      return'w';
      break;
    case '<':
      return 'a';
      break;
    case 'v':
      return 's';
      break;
    case '>':
      return 'd';
      break;
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch (c) {
    case 'W':
      return '^';
      break;
    case 'A':
      return '<';
      break;
    case 'S':
      return 'v';
      break;
    case 'D':
      return '>';
      break;
  }
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c =='s' || c == 'S') {
    return cur_row + 1;
  }
  else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  }
  else if(c ==  '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/

static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  // get current head position
  unsigned int cur_row = state->snakes[snum].head_row;
  unsigned int cur_col = state->snakes[snum].head_col;

  // get the square the snake's head is currently on
  char cur_square = get_board_at(state, cur_row, cur_col);

  // get the square the snake is moving to
  unsigned int next_row = get_next_row(cur_row, cur_square);
  unsigned int next_col = get_next_col(cur_col, cur_square);
  char next_square = get_board_at(state, next_row, next_col);

  return next_square;
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  // get current head 
  unsigned int head_row = state->snakes[snum].head_row;
  unsigned int head_col = state->snakes[snum].head_col;
  char head_square = get_board_at(state, head_row, head_col);

  // get head's next position
  unsigned int head_next_row = get_next_row(head_row, head_square);
  unsigned int head_next_col = get_next_col(head_col, head_square);

  // set head's next position
  set_board_at(state, head_next_row, head_next_col, head_square);

  //
  set_board_at(state, head_row, head_col, head_to_body(head_square));

  // update snake's head
  state->snakes[snum].head_row = head_next_row;
  state->snakes[snum].head_col = head_next_col;
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  // get tail position
  unsigned int tail_row = state->snakes[snum].tail_row;
  unsigned int tail_col = state->snakes[snum].tail_col;
  char tail_square = get_board_at(state, tail_row, tail_col);

  if (is_tail(tail_square)) {
    // blank out the tail
    set_board_at(state, tail_row, tail_col, ' ');
  }
  
  // get tail's next position
  unsigned int tail_next_row = get_next_row(tail_row, tail_square);
  unsigned int tail_next_col = get_next_col(tail_col, tail_square);

  char new_tail_square = body_to_tail(get_board_at(state, tail_next_row, tail_next_col));
  set_board_at(state, tail_next_row, tail_next_col, new_tail_square);

  state->snakes[snum].tail_row = tail_next_row;
  state->snakes[snum].tail_col = tail_next_col;

  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  char new_square;
  for (unsigned int i = 0; i < state->num_snakes; i++) {

    // get the positon of the snake's head
    unsigned int head_row = state->snakes[i].head_row;
    unsigned int head_col = state->snakes[i].head_col;

    // get the square the snake is moving to
    new_square = next_square(state, i);
    if (state->snakes[i].live) {
      if (new_square == '*') {
        update_head(state, i);
        add_food(state);
      }
      else if (new_square == '#' || is_snake(new_square)) {
        set_board_at(state, head_row, head_col,'x');
        state->snakes[i].live = false;
      }
      else {
        update_head(state, i);
        update_tail(state, i);
      }
    }
  }
  return;
}

/* Task 5 */
game_state_t* load_board(FILE* fp) {
  // TODO: Implement this function.
  // Allocate memory for the state
  game_state_t* state = malloc(sizeof(game_state_t));

  // Initialize the snakes array
  state->num_snakes = 0;
  state->snakes = NULL;

  // Read the board from file


  // Initialize the board
  state->num_rows = 0;  
  state->board = malloc(sizeof(char*) * state->num_rows);
  int count = 0;

  // allocate memory for line
  char* line = malloc(sizeof(char) * 100005);
  while(1) {
    char* line_ptr = fgets(line, 100005, fp);
    if (line_ptr == NULL) {
      break;
    }
    else {
      state->num_rows++;
    }

    // reallocate the memory of the board
    state->board = realloc(state->board, sizeof(char*) * state->num_rows);

    // replace \n with \0
    line[strcspn(line, "\n")] = 0;

    // allocate memory for the row
    state->board[count] = malloc(sizeof(char) * strlen(line) + 1);

    // copy the row, use strcpy instead of = directly
    strcpy(state->board[count], line);
    count++;
  }
  return state;
}

/*
  Task 6.1

    Given a snake struct with the tail r
ow and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int tail_row = state->snakes[snum].tail_row;
  unsigned int tail_col = state->snakes[snum].tail_col;

  while (get_board_at(state, tail_row, tail_col) !=' ') {
    char block = get_board_at(state, tail_row, tail_col);
    if (is_head(block)) {
      state->snakes[snum].head_row = tail_row;
      state->snakes[snum].head_col = tail_col;
      return;
    }
    tail_row = get_next_row(tail_row, block);
    tail_col = get_next_col(tail_col, block);
  }
  return;
}

unsigned int count_snakes(game_state_t* state) {
  // TODO: Implement this function.
  unsigned int count = 0;
  unsigned int board_rows = state->num_rows;
  for(unsigned int i = 0; i < board_rows; i++) {
    for(unsigned int j = 0; state->board[i][j] != '\0'; j++) {
      if (is_head(state->board[i][j])) {
        count++;
      } 
    }
  }
  return count;
}

// static void find_tail_and_head(game_state_t* state) {
//   // TODO: Implement this function.
//   unsigned int board_rows = state->num_rows;
//   state->snakes = malloc(sizeof(snake_t) * count_snakes(state));
//   unsigned int snum = 0;
//   for(unsigned int i = 0; i < board_rows; i++) {
//     for(unsigned int j = 0; state->board[i][j]!= '\0'; j++) {
//       if (is_tail(state->board[i][j])) {
//         state->snakes[snum].live = true;
//         state->snakes[snum].tail_row = i;
//         state->snakes[snum].tail_col = j;
//         find_head(state, snum);
//         snum++;
//       }
//     }
//   }
//   return;
// }

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  unsigned int board_rows = state->num_rows;
  state->num_snakes = count_snakes(state);
  state->snakes = malloc(sizeof(snake_t) * state->num_snakes);
  unsigned int snum = 0;
  for(unsigned int i = 0; i < board_rows; i++) {
    for(unsigned int j = 0; state->board[i][j]!= '\0'; j++) {
      if (is_tail(state->board[i][j])) {
        state->snakes[snum].live = true;
        state->snakes[snum].tail_row = i;
        state->snakes[snum].tail_col = j;
        find_head(state, snum);
        snum++;
      }
    }
  }
  return state;
}
