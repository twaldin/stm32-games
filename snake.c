#include "games.h"

#define WIDTH 16
#define HEIGHT 12
#define SNW 20

void delay_ms(uint32_t ms);
void draw_game(bool snake[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT]);
void move_snake(uint8_t *headx, uint8_t *heady, uint8_t *tailx, uint8_t *taily, uint8_t direction, bool snake[WIDTH][HEIGHT], bool *alive, uint8_t tail_dir[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT]);
void new_tail(uint8_t *tailx, uint8_t *taily, uint8_t tail_dir[WIDTH][HEIGHT]); 
void spawn_apple(bool snake[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT]);

uint64_t systick_count = 0;

void sys_tick_handler()
{
    systick_count++;
}

void snake_game(void){

    // initialization
    bool alive = 1;
    bool snake[WIDTH][HEIGHT] = {0}; // 1 = snake segment
    bool apples[WIDTH][HEIGHT] = {0}; // 1 = apple

    // 0 = right, 1 = up, 2 = left, 3 = down
    uint8_t tail_dir[WIDTH][HEIGHT] = {0}; 
    uint8_t direction = 0;    

    // starting location
    uint8_t headx = 8;
    uint8_t heady = 6;
    uint8_t tailx = 8;
    uint8_t taily = 6;

    // background
    st_fill_screen(ST_COLOR_BLACK);


    // first 2 snake segments
    snake[headx][heady] = true;
    snake[headx+=1][heady] = true;

    // srand for apples
    srand(systick_count);

    //game loop
    while(alive){
        //changing direction based on input
        if(is_right_pressed() && direction != 2){
            direction = 0;
        }
        if(is_up_pressed() && direction != 3){
            direction = 1;
        }
        if(is_left_pressed() && direction != 0){
            direction = 2;
        }
        if(is_down_pressed() && direction != 1){
            direction = 3;
        }
        //game logic
        if(!(systick_count % 1000)){
            draw_game(snake, apples);
            move_snake(&headx, &heady, &tailx, &taily, direction, snake, &alive, tail_dir, apples);
        }


        //new apple location
        if(!(systick_count % 4000)){ 
            spawn_apple(snake, apples);
        } 
    }
    st_fill_screen(ST_COLOR_RED);
    st_draw_string(30, 30, "YOU LOSE", ST_COLOR_BLACK, &font_ubuntu_mono_24);
}

void draw_game(bool snake[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT]){
    uint8_t i, j;
    st_fill_screen(ST_COLOR_BLACK);
    for(i = 0; i < WIDTH; i++){
        for(j = 0; j < HEIGHT; j++){
            if(snake[i][j]){ 
                st_fill_rect_fast(i*SNW, j*SNW, SNW, SNW, ST_COLOR_WHITE);
            }
            else if(apples[i][j]){
                st_fill_rect_fast(i*SNW+5, j*SNW+5, SNW-10, SNW-10, ST_COLOR_RED);
            }
        }
    }
}

void spawn_apple(bool snake[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT]){
    uint8_t applex;
    uint8_t appley;

    do{
        applex = rand() % WIDTH;
        appley = rand() % HEIGHT;
    }while(snake[applex][appley] || apples[applex][appley]);

    apples[applex][appley] = 1;
}

void move_snake(uint8_t *headx, uint8_t *heady,uint8_t *tailx, uint8_t *taily, uint8_t direction, bool snake[WIDTH][HEIGHT], bool *alive, uint8_t tail_dir[WIDTH][HEIGHT], bool apples[WIDTH][HEIGHT]){ 

    if(direction == 0){
        if(*headx+1 >= WIDTH || snake[*headx + 1][*heady]){
            *alive = false;
            return;
        }
        else{
            tail_dir[*headx][*heady] = 0;
            *headx += 1;
            snake[*headx][*heady] = true;
        }
    }

    else if(direction == 1){
        if(*heady+1 >= HEIGHT || snake[*headx][*heady + 1]){
            *alive = false;
            return;
        }
        else{
            tail_dir[*headx][*heady] = 1;
            *heady += 1;
            snake[*headx][*heady] = true;
        }
    }
    else if(direction == 2){

        if(*headx == 0){
            *alive = false;
            return;
        }
        else if(snake[*headx - 1][*heady] == 1){
            *alive = false;
            return;
        }
        else{
            tail_dir[*headx][*heady] = 2;
            *headx -= 1;
            snake[*headx][*heady] = true;
        }
    }
    else if(direction == 3){
        if(*heady == 0){
            *alive = false;
            return;
        }
        else if(snake[*headx][*heady - 1] == 1){
            *alive = false;
            return;
        }
        else{
            tail_dir[*headx][*heady] = 3;
            *heady -= 1;
            snake[*headx][*heady] = true;
        }
    }
    if(!apples[*headx][*heady]){
        snake[*tailx][*taily] = false; 
        new_tail(tailx, taily, tail_dir);
    }
}


void new_tail(uint8_t *tailx, uint8_t *taily, uint8_t tail_dir[WIDTH][HEIGHT]){ 
    switch(tail_dir[*tailx][*taily]){ //new tail goes in dir of current tail_dir
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

void delay_ms(uint32_t ms)
{
    // Assuming a specific clock frequency (adjust as needed)
    // This is very approximate and depends on your clock speed
    uint32_t i, j;

    for (i = 0; i < ms; i++) {
        for (j = 0; j < (20000); j++) {
            __asm__("nop");
        }
    }
}


