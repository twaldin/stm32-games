#include "games.h"
#define WIDTH 16
#define HEIGHT 12
#define SNW 20

void delay_ms(uint32_t ms);
static void st_fill_semicircle(int16_t x0, int16_t y0, int16_t r, uint8_t dir, uint16_t color);
static void draw_rounded_end(uint8_t i, uint8_t j, uint8_t dir, uint16_t color);
void draw_game(bool snake[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT], uint8_t headx, uint8_t heady, uint8_t tailx, uint8_t taily, uint8_t head_direction, uint8_t tail_dir[WIDTH][HEIGHT]);
void move_snake(uint8_t *headx, uint8_t *heady, uint8_t *tailx, uint8_t *taily,
                uint8_t direction, bool snake[WIDTH][HEIGHT], bool *alive,
                uint8_t tail_dir[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT],
                uint16_t *score);
void new_tail(uint8_t *tailx, uint8_t *taily, uint8_t tail_dir[WIDTH][HEIGHT]);
void spawn_apple(bool snake[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT]);
uint32_t calculate_game_speed(uint16_t score);
void handle_apple_eaten(uint8_t x, uint8_t y, bool apples[WIDTH][HEIGHT],
                        uint16_t *score);
static void st_fill_quarter_circle(int16_t cx, int16_t cy, int16_t r, uint8_t quadrant, uint16_t color);
static void draw_corner_segment(uint8_t i, uint8_t j, bool from_left, bool from_right, bool from_up, bool from_down, uint16_t color);

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
      draw_game(snake, apples, headx, heady, tailx, taily, direction, tail_dir);
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

void draw_game(bool snake[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT], uint8_t headx, uint8_t heady, uint8_t tailx, uint8_t taily, uint8_t head_direction, uint8_t tail_dir[WIDTH][HEIGHT]) {
  uint8_t i, j;
  st_fill_screen(ST_COLOR_BLACK);
  for (i = 0; i < WIDTH; i++) {
    for (j = 0; j < HEIGHT; j++) {
      if (snake[i][j]) {
        if (i == headx && j == heady) {
            draw_rounded_end(i, j, head_direction, ST_COLOR_DARKGREEN);
        } else if (i == tailx && j == taily) {
            uint8_t tail_end_dir = (tail_dir[i][j] + 2) % 4;
            draw_rounded_end(i, j, tail_end_dir, ST_COLOR_DARKGREEN);
        } else {
            bool n_left = (i > 0) && snake[i-1][j];
            bool n_right = (i < WIDTH-1) && snake[i+1][j];
            bool n_up = (j > 0) && snake[i][j-1];
            bool n_down = (j < HEIGHT-1) && snake[i][j+1];
            int n_count = n_left + n_right + n_up + n_down;

            if (n_count == 2 && !((n_left && n_right) || (n_up && n_down))) {
                draw_corner_segment(i, j, n_left, n_right, n_up, n_down, ST_COLOR_DARKGREEN);
            } else {
                st_fill_rect_fast(i * SNW, j * SNW, SNW, SNW, ST_COLOR_DARKGREEN);
            }
        }
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

static void st_fill_quarter_circle(int16_t cx, int16_t cy, int16_t r, uint8_t quadrant, uint16_t color) {
    for (int16_t py = 0; py <= r; py++) {
        for (int16_t px = 0; px <= r; px++) {
            if (px*px + py*py > r*r) continue;

            if (quadrant == 0) { // top-left
                st_draw_pixel(cx - px, cy - py, color);
            } else if (quadrant == 1) { // top-right
                st_draw_pixel(cx + px, cy - py, color);
            } else if (quadrant == 2) { // bottom-left
                st_draw_pixel(cx - px, cy + py, color);
            } else if (quadrant == 3) { // bottom-right
                st_draw_pixel(cx + px, cy + py, color);
            }
        }
    }
}

static void draw_corner_segment(uint8_t i, uint8_t j, bool from_left, bool from_right, bool from_up, bool from_down, uint16_t color) {
    uint16_t x = i * SNW;
    uint16_t y = j * SNW;
    uint16_t r = SNW;
    uint16_t bg_color = ST_COLOR_BLACK;

    st_fill_rect_fast(x, y, SNW, SNW, bg_color);

    if (from_right && from_down) { // Snake is coming from right and going down, open corner is top-left
        st_fill_quarter_circle(x + SNW, y + SNW, r, 0, color); // Center bottom-right, carve top-left
    } else if (from_left && from_down) { // Snake is coming from left and going down, open corner is top-right
        st_fill_quarter_circle(x, y + SNW, r, 1, color);       // Center bottom-left, carve top-right
    } else if (from_right && from_up) { // Snake is coming from right and going up, open corner is bottom-left
        st_fill_quarter_circle(x + SNW, y, r, 2, color);     // Center top-right, carve bottom-left
    } else if (from_left && from_up) { // Snake is coming from left and going up, open corner is bottom-right
        st_fill_quarter_circle(x, y, r, 3, color);           // Center top-left, carve bottom-right
    }
}

static void st_fill_semicircle(int16_t x0, int16_t y0, int16_t r, uint8_t dir, uint16_t color) {
    for (int16_t py = -r; py <= r; py++) {
        for (int16_t px = -r; px <= r; px++) {
            if (px*px + py*py > r*r) continue;

            if (dir == 0 && px >= 0) { // right
                st_draw_pixel(x0 + px, y0 + py, color);
            } else if (dir == 1 && py >= 0) { // down
                 st_draw_pixel(x0 + px, y0 + py, color);
            } else if (dir == 2 && px <= 0) { // left
                 st_draw_pixel(x0 + px, y0 + py, color);
            } else if (dir == 3 && py <= 0) { // up
                 st_draw_pixel(x0 + px, y0 + py, color);
            }
        }
    }
}

static void draw_rounded_end(uint8_t i, uint8_t j, uint8_t dir, uint16_t color) {
    uint16_t x = i * SNW;
    uint16_t y = j * SNW;
    uint16_t r = SNW / 2;

    if (dir == 0) { // right
        st_fill_rect_fast(x, y, r, SNW, color);
    } else if (dir == 1) { // down
        st_fill_rect_fast(x, y, SNW, r, color);
    } else if (dir == 2) { // left
        st_fill_rect_fast(x + r, y, r, SNW, color);
    } else if (dir == 3) { // up
        st_fill_rect_fast(x, y + r, SNW, r, color);
    }

    int16_t cx = x + r;
    int16_t cy = y + r;
    st_fill_semicircle(cx, cy, r, dir, color);
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
