#include "games.h"
#define WIDTH 16
#define HEIGHT 12
#define SNW 20

void delay_ms(uint32_t ms);
void draw_game(bool snake[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT]);
void move_snake(uint8_t *headx, uint8_t *heady, uint8_t *tailx, uint8_t *taily,
                uint8_t direction, bool snake[WIDTH][HEIGHT], bool *alive,
                uint8_t tail_dir[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT],
                uint16_t *score);
void new_tail(uint8_t *tailx, uint8_t *taily, uint8_t tail_dir[WIDTH][HEIGHT]);
void spawn_apple(bool snake[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT]);
uint32_t calculate_game_speed(uint16_t score);
void handle_apple_eaten(uint8_t x, uint8_t y, bool apples[WIDTH][HEIGHT],
                        uint16_t *score);

uint64_t systick_count = 0;

void sys_tick_handler() { systick_count++; }

void snake_game(void) {
  // initialization
  bool alive = 1;
  bool snake[WIDTH][HEIGHT] = {0};  // 1 = snake segment
  bool apples[WIDTH][HEIGHT] = {0}; // 1 = apple

  // 0 = right, 1 = up, 2 = left, 3 = down
  uint8_t tail_dir[WIDTH][HEIGHT] = {0};
  uint8_t direction = 0;

  // starting location
  uint8_t headx = 8;
  uint8_t heady = 6;
  uint8_t tailx = 8;
  uint8_t taily = 6;

  // game variables
  uint16_t score = 0;
  uint64_t last_game_update = 0;
  uint64_t last_apple_spawn = 0;
  const uint32_t APPLE_SPAWN_INTERVAL = 5000;

  // background
  st_fill_screen(ST_COLOR_BLACK);

  // first 2 snake segments
  snake[headx][heady] = true;
  snake[headx += 1][heady] = true;

  // srand for apples with hella entropy
  srand(systick_count ^ (tailx << 8) ^ (heady << 16) ^ (direction << 24));

  // game loop
  while (alive) {
    // changing direction based on input
    if (is_right_pressed() && direction != 2) {
      direction = 0;
    }
    if (is_up_pressed() && direction != 3) {
      direction = 1;
    }
    if (is_left_pressed() && direction != 0) {
      direction = 2;
    }
    if (is_down_pressed() && direction != 1) {
      direction = 3;
    }
    // game logic
    uint32_t current_game_speed = calculate_game_speed(score);

    if (systick_count - last_game_update >= current_game_speed) {
      draw_game(snake, apples);
      move_snake(&headx, &heady, &tailx, &taily, direction, snake, &alive,
                 tail_dir, apples, &score);
      last_game_update = systick_count;
    }
    // new apple location
    if (systick_count - last_apple_spawn >= APPLE_SPAWN_INTERVAL) {
      spawn_apple(snake, apples);
      last_apple_spawn = systick_count;
    }
  }
  st_fill_screen(ST_COLOR_RED);
  st_draw_string(30, 30, "YOU LOSE", ST_COLOR_BLACK, &font_ubuntu_mono_24);
}

void draw_game(bool snake[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT]) {
  uint8_t i, j;
  st_fill_screen(ST_COLOR_BLACK);
  for (i = 0; i < WIDTH; i++) {
    for (j = 0; j < HEIGHT; j++) {
      if (snake[i][j]) {
        st_fill_rect_fast(i * SNW, j * SNW, SNW, SNW, ST_COLOR_WHITE);
      } else if (apples[i][j]) {
        // Draw apple bitmap that perfectly fills the cell
        st_draw_bitmap(i * SNW, j * SNW, &apple_image);
      }
    }
  }
}

void spawn_apple(bool snake[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT]) {
  uint8_t applex = 0;
  uint8_t appley = 0;
  uint8_t attempts = 0;
  const uint8_t MAX_ATTEMPTS = 50;
  bool found = false;

  // First try random positions
  while (!found && attempts < MAX_ATTEMPTS) {
    applex = rand() % WIDTH;
    appley = rand() % HEIGHT;
    attempts++;

    if (!snake[applex][appley] && !apples[applex][appley]) {
      found = true;
    }
  }

  // If random attempts failed, scan the board systematically
  if (!found) {
    for (uint8_t x = 0; x < WIDTH && !found; x++) {
      for (uint8_t y = 0; y < HEIGHT && !found; y++) {
        if (!snake[x][y] && !apples[x][y]) {
          applex = x;
          appley = y;
          found = true;
        }
      }
    }
  }

  // Only place an apple if we found a valid position
  if (found) {
    apples[applex][appley] = 1;
  }
}

void move_snake(uint8_t *headx, uint8_t *heady, uint8_t *tailx, uint8_t *taily,
                uint8_t direction, bool snake[WIDTH][HEIGHT], bool *alive,
                uint8_t tail_dir[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT],
                uint16_t *score) {
  if (direction == 0) {
    if (*headx + 1 >= WIDTH || snake[*headx + 1][*heady]) {
      *alive = false;
      return;
    } else {
      tail_dir[*headx][*heady] = 0;
      *headx += 1;
      snake[*headx][*heady] = true;
    }
  }

  else if (direction == 1) {
    if (*heady + 1 >= HEIGHT || snake[*headx][*heady + 1]) {
      *alive = false;
      return;
    } else {
      tail_dir[*headx][*heady] = 1;
      *heady += 1;
      snake[*headx][*heady] = true;
    }
  } else if (direction == 2) {

    if (*headx == 0) {
      *alive = false;
      return;
    } else if (snake[*headx - 1][*heady] == 1) {
      *alive = false;
      return;
    } else {
      tail_dir[*headx][*heady] = 2;
      *headx -= 1;
      snake[*headx][*heady] = true;
    }
  } else if (direction == 3) {
    if (*heady == 0) {
      *alive = false;
      return;
    } else if (snake[*headx][*heady - 1] == 1) {
      *alive = false;
      return;
    } else {
      tail_dir[*headx][*heady] = 3;
      *heady -= 1;
      snake[*headx][*heady] = true;
    }
  }
  if (apples[*headx][*heady]) {
    // Apple eaten - increase score
    (*score)++;
    apples[*headx][*heady] = false;
    // Snake grows (don't remove tail)
  } else {
    // No apple eaten - remove tail segment
    snake[*tailx][*taily] = false;
    new_tail(tailx, taily, tail_dir);
  }
}

void new_tail(uint8_t *tailx, uint8_t *taily, uint8_t tail_dir[WIDTH][HEIGHT]) {
  switch (tail_dir[*tailx][*taily]) { // new tail goes in dir of current
                                      // tail_dir
  case 0:
    *tailx += 1;
    break;
  case 1:
    *taily += 1;
    break;
  case 2:
    *tailx -= 1;
    break;
  case 3:
    *taily -= 1;
    break;
  default:
    break;
  }
}

uint32_t calculate_game_speed(uint16_t score) {
  const uint32_t BASE_SPEED = 200;
  const uint32_t MIN_SPEED = 50;
  const uint32_t SPEED_DECREASE_RATE = 5;

  uint32_t calculated_speed = BASE_SPEED - (score * SPEED_DECREASE_RATE);
  return (calculated_speed > MIN_SPEED) ? calculated_speed : MIN_SPEED;
}

// not actually in ms, just random basically
void delay_ms(uint32_t ms) {
  // This is very approximate and depends on clock speed which idk
  uint32_t i, j;

  for (i = 0; i < ms; i++) {
    for (j = 0; j < (20000); j++) {
      __asm__("nop");
    }
  }
}
