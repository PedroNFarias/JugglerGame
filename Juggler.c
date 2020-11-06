#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

//Screen resolution
#define HEIGHT 600 //altura
#define WIDTH 600 //largura
#define FPS 60

//Plataform
#define LENGTH 75 //Comprimento
#define PLATAFORM_BASE 555 //altura
#define PLATAFOR_WIDTH 25 //largura

//Ball
#define RADIUS 15
#define SCORE 20

typedef struct {
    ALLEGRO_DISPLAY *window;
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_EVENT event;
    ALLEGRO_TIMER *timer;
    ALLEGRO_FONT *font;
    ALLEGRO_FONT *smallfont;
    ALLEGRO_FONT *selectedsmallfont;
    ALLEGRO_FONT *mediumfont;
    ALLEGRO_FONT *selectfont;
    ALLEGRO_FONT *bigfont;
    ALLEGRO_KEYBOARD_STATE keyboardState;
    FILE *file;
    ALLEGRO_BITMAP *ScoreboardImage;
    ALLEGRO_BITMAP *GameImage;
    ALLEGRO_BITMAP *HelpImage;
    ALLEGRO_BITMAP *MenuImage;
    ALLEGRO_BITMAP *HeartImage;
    ALLEGRO_BITMAP *GoldTrophy;
    ALLEGRO_BITMAP *SilverTrophy;
    ALLEGRO_BITMAP *BronzeTrophy;
    ALLEGRO_BITMAP *Medal;
} ALLEGRO_POINTERS_t;

typedef struct {
    int level;
    int life;
    char life_text[10];
    int score;
    char score_text[13];
    float velocity;
    char name[10];
    int ballAmount;
    int plataform;
    float gravity;
    bool exit;
    bool pause;
    bool time;
} CONFIG_t;

typedef struct {
    char name[10];
    int highScore;
} SCORE_t;

typedef struct {
    double initialTime;
    double actualTime;
    double pausedTimeInit;
    double pausedTimeTotal;
    char time_text[11];
    bool getpausedTimeInit;
    double timeSpawnBall;
} TIME_t;

typedef struct {
    int height;
    int width;
} DISPLAY_t;

typedef struct {
    int top;
    int left;
    int right;
    int down;
} DISPLAY_SIDES_t;

typedef struct {
    int XMousePlatform;
} PLATAFORM_t;

typedef struct {
    bool check;
    float X_ball;
    float Y_ball;
    float X_way;
    float Y_way;
    float velocity;
    float gravity;
} BALL_t;

typedef struct{
    int display;
} MENU_t;

typedef struct {
    int mouseClick;
    int mouseX;
    int mouseY;
    int mousedX;
    int mousedY;
} MOUSE_t;


bool initialize(ALLEGRO_POINTERS_t *al_pointer, DISPLAY_t *display);
void defaultConfiguration(CONFIG_t *config, MENU_t *menu);
void controlMenu(ALLEGRO_POINTERS_t *al_pointer, MOUSE_t *mouse, MENU_t *menu, CONFIG_t *config);
void returnMenu (MOUSE_t *mouse, MENU_t *menu);
void setPointers(ALLEGRO_POINTERS_t *al_pointer);
void setDisplay(DISPLAY_t *display, DISPLAY_SIDES_t *display_sides);
void setConfiguration(CONFIG_t *config, BALL_t *ball, bool *spawnBall2);
void readScoreFile (SCORE_t *scoreFile, ALLEGRO_POINTERS_t *al_pointer);
void drawTime (TIME_t *time, ALLEGRO_POINTERS_t *al_pointer);
void drawMenu(ALLEGRO_POINTERS_t *al_pointer);
void drawPlataform(PLATAFORM_t *plataform, CONFIG_t *config);
void drawScore(CONFIG_t *config, ALLEGRO_POINTERS_t *al_pointer);
void drawScoreboard(SCORE_t *scoreFile, ALLEGRO_POINTERS_t * al_pointer);
void drawLifes (CONFIG_t *config, ALLEGRO_POINTERS_t *al_pointer);
void drawHelp (ALLEGRO_POINTERS_t *al_pointer);
void drawBall(BALL_t *ball, CONFIG_t *config);
void paintBackGround();
void spawnBall (BALL_t *ball, CONFIG_t *config, int i);
void reorganizeScore(CONFIG_t *config, SCORE_t *scoreFile, int posicao);
void ballDisplacement (BALL_t *ball, CONFIG_t *config);
void ballToutchingDisplayBorder(BALL_t *ball, DISPLAY_SIDES_t *display_sides, CONFIG_t *config, SCORE_t *scoreFile, MENU_t *menu, ALLEGRO_POINTERS_t *al_pointer);
void scoreUp (BALL_t *ball, PLATAFORM_t *plataform, CONFIG_t *config, MOUSE_t *mouse, TIME_t *time, bool *spawnBall2);
void checkScore(CONFIG_t *config, BALL_t *ball, TIME_t *time, bool *spawnBall2);
void drawPauseMenu(CONFIG_t *config, ALLEGRO_POINTERS_t *al_pointer);
void controlPauseMenu(CONFIG_t *config, ALLEGRO_POINTERS_t *al_pointer, MOUSE_t *mouse, MENU_t *menu, TIME_t *time);
void initializeTimer(TIME_t *time);
void checkLife (CONFIG_t *config, SCORE_t *scoreFile, MENU_t *menu, ALLEGRO_POINTERS_t *al_pointer);
void checkGameOver (CONFIG_t *config, SCORE_t *scoreFile, MENU_t *menu, ALLEGRO_POINTERS_t *al_pointer);
void writeScoreFile (SCORE_t *scoreFile, ALLEGRO_POINTERS_t *al_pointer);
void writeName(ALLEGRO_POINTERS_t *al_pointer, CONFIG_t *config, MENU_t *menu);
void plataformDirection(MOUSE_t *mouse, ALLEGRO_POINTERS_t *al_pointer, BALL_t *ball);
void drawGameBackground(ALLEGRO_POINTERS_t *al_pointer);
void menuControls(ALLEGRO_POINTERS_t *al_pointer, MENU_t *menu, MOUSE_t *mouse, PLATAFORM_t *plataform, bool *shouldRedraw, CONFIG_t *config, TIME_t *time);
void desactiveBall(BALL_t *ball);
void synchronizeBalls(TIME_t *time, BALL_t *ball, CONFIG_t *config, bool *spawnBall2);

int main () {
    bool spawnBall2 = false;
    bool shouldRedraw;
    ALLEGRO_POINTERS_t al_pointer;
    DISPLAY_t display;
    DISPLAY_SIDES_t display_sides;
    CONFIG_t config;
    MENU_t menu;
    SCORE_t scoreFile[11];
    TIME_t time;
    PLATAFORM_t plataform;
    BALL_t ball[3];
    MOUSE_t mouse;

    defaultConfiguration(&config, &menu);
    setPointers(&al_pointer);
    setDisplay(&display, &display_sides);
    setConfiguration(&config, ball, &spawnBall2);
    readScoreFile(scoreFile,&al_pointer);
    desactiveBall(ball);

    if (!initialize(&al_pointer, &display)) {
        return false;
    }

    while(!config.exit) {

        al_wait_for_event(al_pointer.event_queue, &al_pointer.event);
        menuControls(&al_pointer, &menu, &mouse, &plataform, &shouldRedraw, &config, &time);
        if(menu.display == 0){
            if(shouldRedraw && al_is_event_queue_empty(al_pointer.event_queue)){
                shouldRedraw = false;
                paintBackGround();
                controlMenu(&al_pointer, &mouse, &menu, &config);
                drawMenu(&al_pointer);
                al_flip_display();
            }
        }else if (menu.display == 1) {
          if(config.time == 1){
            config.time = 0;
            desactiveBall(ball);
            desactiveBall(ball);
            config.life = 5;
            config.score = 0;
            config.level = 1;
            setConfiguration(&config, ball, &spawnBall2);
            initializeTimer(&time);
          }
          if(shouldRedraw && al_is_event_queue_empty(al_pointer.event_queue)){
              shouldRedraw = false;
              if(config.pause == 0){
                synchronizeBalls(&time, ball, &config, &spawnBall2);
                paintBackGround();
                drawGameBackground(&al_pointer);
                drawTime(&time, &al_pointer);
                drawPlataform(&plataform, &config);
                drawBall(ball, &config);
                ballDisplacement(ball,&config);
                ballToutchingDisplayBorder(ball, &display_sides, &config, scoreFile, &menu, &al_pointer);
                scoreUp(ball, &plataform, &config, &mouse, &time, &spawnBall2);
                drawScore(&config, &al_pointer);
                drawLifes(&config, &al_pointer);
              }else if(config.pause == 1){
                drawPauseMenu(&config, &al_pointer);
                controlPauseMenu(&config, &al_pointer, &mouse, &menu, &time);
              }
              al_flip_display();
          }
        }else if(menu.display == 2){
            if(shouldRedraw && al_is_event_queue_empty(al_pointer.event_queue)){
                shouldRedraw = false;
                paintBackGround();
                drawHelp(&al_pointer);
                returnMenu(&mouse, &menu);
                al_flip_display();
            }
        }else if(menu.display == 3){
            if(shouldRedraw && al_is_event_queue_empty(al_pointer.event_queue)){
                shouldRedraw = false;
                paintBackGround();
                readScoreFile(scoreFile, &al_pointer);
                drawScoreboard(scoreFile, &al_pointer);
                returnMenu(&mouse, &menu);
                al_flip_display();
            }
        }
        plataformDirection(&mouse, &al_pointer, ball);
    }
    al_destroy_display(al_pointer.window);
    al_destroy_event_queue(al_pointer.event_queue);
    al_destroy_bitmap(al_pointer.ScoreboardImage);
  return 0;
}

void setPointers (ALLEGRO_POINTERS_t *al_pointer) {
    al_pointer->window = NULL;
    al_pointer->event_queue = NULL;
    al_pointer->timer = NULL;
    al_pointer->font = NULL;
    al_pointer->smallfont = NULL;
    al_pointer->selectedsmallfont = NULL;
    al_pointer->file = NULL;
    al_pointer->mediumfont = NULL;
    al_pointer->bigfont = NULL;
    al_pointer->selectfont = NULL;
    al_pointer->ScoreboardImage = NULL;
    al_pointer->GameImage = NULL;
    al_pointer->MenuImage = NULL;
    al_pointer->HelpImage = NULL;
    al_pointer->HeartImage = NULL;
    al_pointer->BronzeTrophy = NULL;
    al_pointer->SilverTrophy = NULL;
    al_pointer->GoldTrophy = NULL;
    al_pointer->Medal = NULL;
}

void desactiveBall(BALL_t *ball){
    for ( int i = 0; i < 3; i++){
        ball[i].Y_ball = 300;
        ball[i].X_ball = 800;
        ball[i].Y_way = 0;
        ball[i].X_way = 0;
        ball[i].gravity = 0;
    }
}

void setDisplay (DISPLAY_t *display, DISPLAY_SIDES_t *display_sides) {
    display->height = HEIGHT;
    display->width = WIDTH;
    display_sides->top = 0;
    display_sides->down = HEIGHT;
    display_sides->left = 0;
    display_sides->right = WIDTH;
}

void readScoreFile (SCORE_t *scoreFile, ALLEGRO_POINTERS_t *al_pointer) {
    int i = 0;
    al_pointer->file = fopen("score.txt","r");
    if (al_pointer->file == NULL) {
        printf("error to load SCORE FILE\n");
        al_destroy_display(al_pointer->window);
    }else {
        while( (fscanf(al_pointer->file,"%s %i\n", scoreFile[i].name, &scoreFile[i].highScore))!=EOF )
            i++;

        fclose(al_pointer->file);
    }
}

void drawScoreboard(SCORE_t *scoreFile, ALLEGRO_POINTERS_t * al_pointer){
    int pos = 160, i;
    char pontText[10], posText[3];

    al_draw_bitmap(al_pointer->ScoreboardImage, -13, -13, 0);
    al_draw_text(al_pointer->bigfont, al_map_rgb(0, 0, 0), 300, 45, ALLEGRO_ALIGN_CENTER, "SCOREBOARD");
    al_draw_text(al_pointer->font, al_map_rgb(0, 0, 0), 300, 130, ALLEGRO_ALIGN_CENTER, "Top 10 jogadores");
    for( i = 0; i < 10; i++){
        if(i > 2)
            al_draw_bitmap(al_pointer->Medal, 150, pos - 10, 0);
        sprintf(pontText, "%d", scoreFile[i].highScore);
        sprintf(posText, "%d", i + 1);
        al_draw_text(al_pointer->font, al_map_rgb(0, 0, 0), 180, pos, ALLEGRO_ALIGN_LEFT, posText);
        al_draw_text(al_pointer->font, al_map_rgb(0, 0, 0), 210, pos, ALLEGRO_ALIGN_LEFT, scoreFile[i].name);
        al_draw_text(al_pointer->font, al_map_rgb(0, 0, 0), 300, pos + 5, ALLEGRO_ALIGN_CENTER, "__________________________________");
        al_draw_text(al_pointer->font, al_map_rgb(0, 0, 0), 380, pos, ALLEGRO_ALIGN_LEFT, pontText);
        pos += 33;
    }
    al_draw_filled_rounded_rectangle(500, 550, 580, 560, 7, 7, al_map_rgb(50, 0, 138));
    al_draw_line(505, 555, 540, 530, al_map_rgb(50, 0, 138), 10);
    al_draw_line(505, 555, 540, 580, al_map_rgb(50, 0, 138), 10);
    al_draw_bitmap(al_pointer->GoldTrophy, 150, 150, 0);
    al_draw_bitmap(al_pointer->SilverTrophy, 150, 183, 0);
    al_draw_bitmap(al_pointer->BronzeTrophy, 150, 216, 0);
}

void drawTime (TIME_t *time, ALLEGRO_POINTERS_t *al_pointer) {
    time->actualTime = al_get_time() - time->initialTime - time->pausedTimeTotal;
    sprintf(time->time_text, "TEMPO %.2lf", time->actualTime);
    al_draw_text(al_pointer->font, al_map_rgb(255,255,0), 10, 20, 0, time->time_text);
}

void defaultConfiguration(CONFIG_t *config,MENU_t *menu) {
    config->level = 1;
    menu->display = 0;
    config->score = 0;
}

void setConfiguration (CONFIG_t *config, BALL_t *ball, bool *spawnBall2) {
    int i;
    switch (config->level) {
        case 1 :
            config->life = 5;
            config->score = 0;
            config->plataform = 0;
            config->ballAmount = 2;
            for(i = 0; i < config->ballAmount; i++)
                config->velocity = ball[i].velocity = 7;
                ball[i].gravity = 0.07;
            config->gravity = 0.07;
            break;
        case 2 :
            *spawnBall2 = true;
            config->ballAmount = 3;
            break;
        case 3 :
            config->velocity = 9;
            config->gravity = 0.10;
            break;
        case 4 :
            for(i = 0; i < config->ballAmount; i++)
            config->plataform = -20;
            break;
    }
}

bool initialize (ALLEGRO_POINTERS_t *al_pointer, DISPLAY_t *display) {
    if (!al_init()) {
        fprintf(stderr, "Fail to initialize ALLEGRO.\n");
        return false;
    }
    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Fail to initialize PRIMITIVE'S add-on.\n");
        return false;
    }

    al_pointer->window = al_create_display(display->width, display->height);
    if (!al_pointer->window)  {
        fprintf(stderr, "Fail to create a WINDOW.\n");
        return false;
    }
    al_set_window_title(al_pointer->window, "JUGGLER");

    if (!al_install_mouse()) {
        fprintf(stderr, "Fail to initialize MOUSE.\n");
        al_destroy_display(al_pointer->window);
        return false;
    }

    if(!al_install_keyboard()) {
      fprintf(stderr, "Fail to initialize KEYBOARD.\n");
      al_destroy_display(al_pointer->window);
      return false;
    }

    al_init_font_addon();
    al_init_image_addon();

    al_pointer->ScoreboardImage = al_load_bitmap("Imagens/Scoreboard1.jpg");
    al_pointer->GameImage = al_load_bitmap("Imagens/game.jpg");
    al_pointer->HeartImage = al_load_bitmap("Imagens/heart.png");
    al_pointer->MenuImage = al_load_bitmap("Imagens/Menu.jpg");
    al_pointer->HelpImage= al_load_bitmap("Imagens/Help.bmp");
    al_pointer->GoldTrophy= al_load_bitmap("Imagens/GoldMedal.png");
    al_pointer->SilverTrophy= al_load_bitmap("Imagens/SilverMedal.png");
    al_pointer->BronzeTrophy= al_load_bitmap("Imagens/BronzeMedal.png");
    al_pointer->Medal = al_load_bitmap("Imagens/Medal.png");

    if (!al_pointer->ScoreboardImage || !al_pointer->GameImage || !al_pointer->HeartImage || !al_pointer->MenuImage || !al_pointer->HelpImage) {
      fprintf(stderr, "Fail to load IMAGES.\n");
      al_destroy_display(al_pointer->window);
      return false;
    }

    if (!al_pointer->BronzeTrophy || !al_pointer->SilverTrophy || !al_pointer->GoldTrophy || !al_pointer->Medal) {
      fprintf(stderr, "Fail to load ICONS.\n");
      al_destroy_display(al_pointer->window);
      return false;
    }

    if (!al_init_ttf_addon()) {
      fprintf(stderr, "Fail to initialize TTF ADON.\n");
      al_destroy_display(al_pointer->window);
      return false;
    }

    al_pointer->font = al_load_font("Exo-Bold.ttf", 15, 0);
    if (!al_pointer->font) {
      fprintf(stderr, "Fail to load FONT.\n");
      al_destroy_display(al_pointer->window);
      return false;
    }

    al_pointer->bigfont = al_load_font("Exo-Bold.ttf", 70, 0);
    if (!al_pointer->bigfont) {
      fprintf(stderr, "Fail to load FONT.\n");
      al_destroy_display(al_pointer->window);
      return false;
    }

    al_pointer->mediumfont = al_load_font("Exo-Bold.ttf", 40, 0);
    if (!al_pointer->mediumfont) {
      fprintf(stderr, "Fail to load FONT.\n");
      al_destroy_display(al_pointer->window);
      return false;
    }

    al_pointer->selectfont = al_load_font("Exo-Bold.ttf", 44, 0);
    if (!al_pointer->selectfont) {
      fprintf(stderr, "Fail to load FONT.\n");
      al_destroy_display(al_pointer->window);
      return false;
    }

    al_pointer->smallfont = al_load_font("Exo-Bold.ttf", 25, 0);
    if (!al_pointer->smallfont) {
      fprintf(stderr, "Fail to load FONT.\n");
      al_destroy_display(al_pointer->window);
      return false;
    }

    al_pointer->selectedsmallfont = al_load_font("Exo-Bold.ttf", 26, 0);
    if (!al_pointer->selectedsmallfont) {
      fprintf(stderr, "Fail to load FONT.\n");
      al_destroy_display(al_pointer->window);
      return false;
    }

    al_pointer->timer = al_create_timer(1.0 / FPS);
    if (!al_pointer->timer) {
        fprintf(stderr, "Fail to initialize TIMER.\n");
        return false;
    }

    al_pointer->event_queue = al_create_event_queue();
    if (!al_pointer->event_queue) {
        fprintf(stderr, "Fail to initialize EVENT QUEUE.\n");
        return false;
    }

    al_register_event_source(al_pointer->event_queue, al_get_keyboard_event_source());
    al_register_event_source(al_pointer->event_queue, al_get_mouse_event_source());
    al_register_event_source(al_pointer->event_queue, al_get_timer_event_source(al_pointer->timer));
    al_start_timer(al_pointer->timer);

    return true;
}

void initializeTimer(TIME_t *time){
    time->initialTime = al_get_time();
    time->pausedTimeTotal = 0;
    time->getpausedTimeInit = 1;
}

void paintBackGround () {
    al_clear_to_color(al_map_rgb(18,161,209));
}

void drawPlataform (PLATAFORM_t *plataform, CONFIG_t *config) {
    al_draw_filled_rectangle(plataform->XMousePlatform - LENGTH - config->plataform, PLATAFORM_BASE,
      plataform->XMousePlatform + LENGTH + config->plataform, PLATAFORM_BASE + PLATAFOR_WIDTH,al_map_rgb(50, 0, 138));
}

void spawnBall (BALL_t *ball, CONFIG_t *config, int i) {
    int side;

    srand(time(NULL));
    ball[i].Y_ball = 545;
    ball[i].X_ball = 100;
    ball[i].Y_way = 1;
    side = rand() % 2;
    if(side == 1){
        ball[i].X_way = 0.15;
    }else{
        ball[i].X_way = -0.15;
    }
    ball[i].gravity = config->gravity;
}

void ballDisplacement (BALL_t *ball, CONFIG_t *config) {
    for(int i = 0; i < 3; i++){
     ball[i].Y_ball -= ball[i].velocity * ball[i].Y_way;
     ball[i].velocity -= ball[i].gravity;
     ball[i].X_ball += config->velocity * ball[i].X_way;
    }
}

void drawBall (BALL_t *ball, CONFIG_t *config) {
    for(int i = 0; i < config->ballAmount; i++) {
        if (i == 0) {
            al_draw_filled_circle (ball[i].X_ball, ball[i].Y_ball, RADIUS , al_map_rgb (1, 95, 142));
        }
        if (i == 1) {
            al_draw_filled_circle (ball[i].X_ball, ball[i].Y_ball, RADIUS , al_map_rgb (5, 99, 7));
        }
        if (i == 2) {
            al_draw_filled_circle (ball[i].X_ball, ball[i].Y_ball, RADIUS , al_map_rgb (189, 209, 8));
        }
    }
}

void scoreUp (BALL_t *ball, PLATAFORM_t *plataform, CONFIG_t *config, MOUSE_t *mouse, TIME_t *time, bool *spawnBall2) {
    for( int i = 0; i < config->ballAmount; i++){
        if ((ball[i].X_ball >= plataform->XMousePlatform - LENGTH - config->plataform && (ball[i].Y_ball >= PLATAFORM_BASE - 8 &&  ball[i].Y_ball <= PLATAFORM_BASE + 2) )&&
            (ball[i].X_ball <= plataform->XMousePlatform + LENGTH + config->plataform && (ball[i].Y_ball >= PLATAFORM_BASE - 8 && ball[i].Y_ball <= PLATAFORM_BASE + 2)) || ball->check ){
            ball[i].velocity = config->velocity;
            ball[i].gravity = config->gravity;
            config->score += SCORE;
            ball[i].X_way += (float)mouse->mousedX * 0.0625;
            checkScore(config, ball, time, spawnBall2);
        }
    }
}

void drawScore (CONFIG_t *config, ALLEGRO_POINTERS_t *al_pointer) {
  sprintf(config->score_text, "PONTUAÇÃO %d", config->score);
  al_draw_text(al_pointer->font, al_map_rgb(18,163,8), 10, 40, 0, config->score_text);
}

void drawLifes (CONFIG_t *config, ALLEGRO_POINTERS_t *al_pointer) {
  sprintf(config->life_text, "%d", config->life);
  al_draw_text(al_pointer->selectfont, al_map_rgb(0,0,0), 570, 28, ALLEGRO_ALIGN_CENTER, config->life_text);
  al_draw_text(al_pointer->mediumfont, al_map_rgb(255,0,0), 570, 30, ALLEGRO_ALIGN_CENTER, config->life_text);

  if (config->life == 0) {
      al_draw_text(al_pointer->bigfont, al_map_rgb(255, 50, 50), 300, 300, ALLEGRO_ALIGN_CENTER, "GAME OVER!");
  }else{
      al_draw_bitmap(al_pointer->HeartImage, 490, 20, 0);
  }
}

void checkScore (CONFIG_t *config, BALL_t *ball, TIME_t *time, bool *spawnBall2) {
    switch(config->score ) {
        case 200:
            config->level = 2;
            setConfiguration(config, ball, spawnBall2);
            time->timeSpawnBall = al_get_time();
            break;
        case 500:
            config->level = 3;
            setConfiguration(config, ball, spawnBall2);
            break;
        case 700:
            config->level = 4;
            setConfiguration(config, ball, spawnBall2);
            break;
    }
}

void ballToutchingDisplayBorder(BALL_t *ball, DISPLAY_SIDES_t *display_sides, CONFIG_t *config, SCORE_t *scoreFile, MENU_t *menu, ALLEGRO_POINTERS_t *al_pointer) {
    checkLife(config, scoreFile, menu, al_pointer);
    for(int i = 0; i < config->ballAmount; i++){
        if (ball[i].X_ball + RADIUS >= display_sides->right - 3 && ball[i].X_ball + RADIUS <= display_sides->right + 10) {
            ball[i].X_way = ball[i].X_way * -1;
        }
        if (ball[i].X_ball - RADIUS >= display_sides->left - 10 && ball[i].X_ball - RADIUS <= display_sides->left + 3 ) {
            ball[i].X_way = ball[i].X_way * -1;
        }
        if (ball[i].Y_ball - RADIUS <= display_sides->top - 3 && ball[i].Y_ball - RADIUS <= display_sides->top + 10) {
            ball[i].Y_way = ball[i].Y_way * -1;
        }
        if (ball[i].Y_ball + RADIUS >= display_sides->down && ball[i].Y_ball - RADIUS >= display_sides->down + 2) {
            config->life--;
            spawnBall(ball, config, i);
            ball[i].velocity = config->velocity;
        }
    }
}

void drawMenu(ALLEGRO_POINTERS_t *al_pointer){
    al_draw_text(al_pointer->bigfont, al_map_rgb(5, 41, 99), 300, 50, ALLEGRO_ALIGN_CENTER, "JUGGLER");
    al_draw_text(al_pointer->mediumfont, al_map_rgb(5, 41, 99), 300, 180, ALLEGRO_ALIGN_CENTER, "START");
    al_draw_text(al_pointer->mediumfont, al_map_rgb(5, 41, 99), 300, 240, ALLEGRO_ALIGN_CENTER, "HELP");
    al_draw_text(al_pointer->mediumfont, al_map_rgb(5, 41, 99), 300, 300, ALLEGRO_ALIGN_CENTER, "SCOREBOARD");
    al_draw_text(al_pointer->mediumfont, al_map_rgb(5, 41, 99), 300, 360, ALLEGRO_ALIGN_CENTER, "QUIT");
}

void controlMenu(ALLEGRO_POINTERS_t *al_pointer, MOUSE_t *mouse, MENU_t *menu, CONFIG_t *config) {
    al_draw_bitmap(al_pointer->MenuImage, 0, -200, 0);
    if(mouse->mouseX > 200 && mouse->mouseX < 400){
        if(mouse->mouseY > 170 && mouse->mouseY < 230){
            al_draw_text(al_pointer->selectfont, al_map_rgb(15, 141, 199), 300, 180, ALLEGRO_ALIGN_CENTER, "START");
            if(mouse->mouseClick == 1){
                menu->display = 1;
                config->time = 1;
            }
        }else if(mouse->mouseY > 230 && mouse->mouseY < 290){
            al_draw_text(al_pointer->selectfont, al_map_rgb(15, 141, 199), 300, 240, ALLEGRO_ALIGN_CENTER, "HELP");
            if(mouse->mouseClick == 1){
                menu->display = 2;
            }
        }else if(mouse->mouseY > 290 && mouse->mouseY < 350){
            al_draw_text(al_pointer->selectfont, al_map_rgb(15, 141, 199), 300, 300, ALLEGRO_ALIGN_CENTER, "SCOREBOARD");
            if(mouse->mouseClick == 1){
                menu->display = 3;
            }
        }else if(mouse->mouseY > 350 && mouse->mouseY < 410){
            al_draw_text(al_pointer->selectfont, al_map_rgb(255, 50, 50), 300, 360, ALLEGRO_ALIGN_CENTER, "QUIT");
            al_draw_text(al_pointer->selectfont, al_map_rgb(255, 50, 50), 380, 360, ALLEGRO_ALIGN_CENTER, "=(");
            if(mouse->mouseClick == 1){
                config->exit = true;
            }
        }
    }
}

void drawHelp (ALLEGRO_POINTERS_t *al_pointer){
    al_draw_bitmap(al_pointer->HelpImage, -223.5, 0, ALLEGRO_ALIGN_CENTER);
    al_draw_text(al_pointer->bigfont, al_map_rgb(255, 255, 255), 300, 50, ALLEGRO_ALIGN_CENTER, "HELP");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 150, ALLEGRO_ALIGN_CENTER, "Como jogar:");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 180, ALLEGRO_ALIGN_CENTER, "Seu objetivo é impedir que as bolas toquem no chão, mantendo-as no");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 210, ALLEGRO_ALIGN_CENTER, "ar com o a plataforma controlada pelo mouse.");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 240, ALLEGRO_ALIGN_CENTER, "Quanto mais tempo você mantê-las no ar, mais pontos terá.");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 270, ALLEGRO_ALIGN_CENTER, "Com o aumento da pontuação, a dificuldade aumenta, aumentando a quan-");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 300, ALLEGRO_ALIGN_CENTER, "tidade de bolas na tela, a velocidade delas e também diminuindo o ");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 330, ALLEGRO_ALIGN_CENTER, "tamanho da plataforma.");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 360, ALLEGRO_ALIGN_CENTER, "Caso tenha uma pontuação melhor que a dos antigos jogadores, ao");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 390, ALLEGRO_ALIGN_CENTER, "final do jogo você poderá registrar seu nome no scoreboard.");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 420, ALLEGRO_ALIGN_CENTER, "Resumindo: mexa o mouse, não deixe as bolas tocar no chão, consiga");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 450, ALLEGRO_ALIGN_CENTER, "pontos e boa sorte!!!");
    al_draw_text(al_pointer->font, al_map_rgb(255, 255, 255), 300, 480, ALLEGRO_ALIGN_CENTER, "Para voltar ao menu principal, clique na seta abaixo ou pressione ESC.");
    al_draw_filled_rounded_rectangle(500, 550, 580, 560, 7, 7, al_map_rgb(255, 50, 255));
    al_draw_line(505, 555, 540, 530, al_map_rgb(255, 50, 255), 10);
    al_draw_line(505, 555, 540, 580, al_map_rgb(255, 50, 255), 10);
}

void returnMenu (MOUSE_t *mouse, MENU_t *menu){
    if(mouse->mouseX > 500 && mouse->mouseX < 580){
        if(mouse->mouseY > 530 && mouse->mouseY < 580){
            al_draw_filled_rounded_rectangle(500, 550, 580, 560, 7, 7, al_map_rgb(0, 0, 0));
            al_draw_line(505, 555, 540, 530, al_map_rgb(0, 0, 0), 10);
            al_draw_line(505, 555, 540, 580, al_map_rgb(0, 0, 0), 10);
            if(mouse->mouseClick == 1){
                menu->display = 0;
            }
        }
    }
}

void drawPauseMenu(CONFIG_t *config, ALLEGRO_POINTERS_t *al_pointer){
    al_draw_text(al_pointer->mediumfont, al_map_rgb(0, 0, 0), 300, 220, ALLEGRO_ALIGN_CENTER, "PAUSED");
    al_draw_text(al_pointer->smallfont, al_map_rgb(0, 0, 0), 300, 280, ALLEGRO_ALIGN_CENTER, "Resume");
    al_draw_text(al_pointer->smallfont, al_map_rgb(0, 0, 0), 300, 320, ALLEGRO_ALIGN_CENTER, "Menu");
    al_draw_text(al_pointer->smallfont, al_map_rgb(0, 0, 0), 300, 360, ALLEGRO_ALIGN_CENTER, "Quit");
}

void controlPauseMenu(CONFIG_t *config, ALLEGRO_POINTERS_t *al_pointer, MOUSE_t *mouse, MENU_t *menu, TIME_t *time){
    if(time->getpausedTimeInit == 1){
        time->pausedTimeInit = al_get_time();
        time->getpausedTimeInit = 0;
    }

    if(mouse->mouseX > 250 && mouse->mouseX < 350){
        if(mouse->mouseY > 280 && mouse->mouseY < 320){
            al_draw_text(al_pointer->smallfont, al_map_rgb(150, 150, 150), 300, 280, ALLEGRO_ALIGN_CENTER, "Resume");
            if(mouse->mouseClick == 1){
                time->pausedTimeTotal = time->pausedTimeTotal + (al_get_time() - time->pausedTimeInit);
                config->pause = 0;
                time->getpausedTimeInit = 1;
            }
        }else if(mouse->mouseY > 320 && mouse->mouseY < 360){
            al_draw_text(al_pointer->smallfont, al_map_rgb(150, 150, 150), 300, 320, ALLEGRO_ALIGN_CENTER, "Menu");
            if(mouse->mouseClick == 1){
                menu->display = 0;
                config->pause = 0;
                time->getpausedTimeInit = 1;
                al_rest(0.25);
            }
        }else if(mouse->mouseY > 360 && mouse->mouseY < 400){
            al_draw_text(al_pointer->smallfont, al_map_rgb(150, 150, 150), 300, 360, ALLEGRO_ALIGN_CENTER, "Quit");
            if(mouse->mouseClick == 1)
                config->exit = 1;
                time->getpausedTimeInit = 1;
        }
    }
}

void checkLife (CONFIG_t *config, SCORE_t *scoreFile, MENU_t *menu, ALLEGRO_POINTERS_t *al_pointer) {
    if (config->life == 0) {
        menu->display = 3;
        checkGameOver(config, scoreFile, menu, al_pointer);
    }

}

void checkGameOver (CONFIG_t *config, SCORE_t *scoreFile, MENU_t *menu, ALLEGRO_POINTERS_t *al_pointer) {
    if (config->score > scoreFile[9].highScore) {
        for (int i = 0; i < 10; i++) {
            if (config->score > scoreFile[i].highScore) {
                writeName(al_pointer, config, menu);
                reorganizeScore(config, scoreFile, i);
                break;
            }
        }
        writeScoreFile(scoreFile, al_pointer);
        }else {
          al_rest(5);
          menu->display = 0;
        }
}

void reorganizeScore(CONFIG_t *config, SCORE_t *scoreFile, int posicao) {
    for (int i = 9; i > posicao; i--) {
        scoreFile[i]  = scoreFile[i -1];
    }
    scoreFile[posicao].highScore = config->score;
    strcpy(scoreFile[posicao].name, config->name);
}

void writeScoreFile (SCORE_t *scoreFile, ALLEGRO_POINTERS_t *al_pointer) {
    int i = 0;
    al_pointer->file = fopen("score.txt","w");
    if (al_pointer->file == NULL) {
        printf("error to load SCORE FILE\n");
        al_destroy_display(al_pointer->window);
      }else {
        while( i < 10) {
            fprintf(al_pointer->file, "%s %i\n", scoreFile[i].name, scoreFile[i].highScore);
            i++;
        }
        fclose(al_pointer->file);
    }
}

void writeName(ALLEGRO_POINTERS_t *al_pointer, CONFIG_t *config, MENU_t *menu){
    bool sair = 0;
    config->name[0] = '\0';
    while(sair == 0){
        al_wait_for_event(al_pointer->event_queue, &al_pointer->event);
        if (al_pointer->event.type == ALLEGRO_EVENT_KEY_CHAR){
            if (strlen(config->name) <= 10){
                char temp[] = {al_pointer->event.keyboard.unichar, '\0'};
                if (al_pointer->event.keyboard.unichar >= '0' && al_pointer->event.keyboard.unichar <= '9'){
                    strcat(config->name, temp);
                }else if (al_pointer->event.keyboard.unichar >= 'A' && al_pointer->event.keyboard.unichar <= 'Z'){
                    strcat(config->name, temp);
                }else if (al_pointer->event.keyboard.unichar >= 'a' && al_pointer->event.keyboard.unichar <= 'z'){
                    strcat(config->name, temp);
                }
            }
            if (al_pointer->event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(config->name) != 0)
            {
                config->name[strlen(config->name) - 1] = '\0';
            }
        }
        if(al_pointer->event.type == ALLEGRO_EVENT_KEY_DOWN && al_pointer->event.keyboard.keycode == ALLEGRO_KEY_ENTER){
            sair = 1;
        }
        al_draw_bitmap(al_pointer->ScoreboardImage, -13, -13, 0);
        al_draw_bitmap(al_pointer->HeartImage, 490, 20, 0);
        al_draw_line(490, 15, 554, 79, al_map_rgb(0, 0, 0), 10);
        al_draw_line(490, 79, 554, 15, al_map_rgb(0, 0, 0), 10);
        al_draw_text(al_pointer->mediumfont, al_map_rgb(0, 0, 0), 300, 150, ALLEGRO_ALIGN_CENTER, "Parabéns!!!");
        al_draw_text(al_pointer->smallfont, al_map_rgb(0, 0, 0), 300, 220, ALLEGRO_ALIGN_CENTER, "Você bateu um recorde!!!");
        al_draw_text(al_pointer->smallfont, al_map_rgb(0, 0, 0), 300, 260, ALLEGRO_ALIGN_CENTER, "Digite o seu nome:");
        al_draw_text(al_pointer->mediumfont, al_map_rgb(0, 0, 0), 300, 300, ALLEGRO_ALIGN_CENTER, config->name);
        al_draw_text(al_pointer->smallfont, al_map_rgb(0, 0, 0), 300, 360, ALLEGRO_ALIGN_CENTER, config->score_text);
        al_flip_display();
    }
    al_flip_display();
}

void plataformDirection(MOUSE_t *mouse, ALLEGRO_POINTERS_t *al_pointer, BALL_t *ball){
    if (al_pointer->event.type == ALLEGRO_EVENT_MOUSE_AXES){
        mouse->mousedX = al_pointer->event.mouse.dx;
        if(mouse->mousedX > 10)
            mouse->mousedX = 10;
        else if(mouse->mousedX < -10)
            mouse->mousedX = -10;
    }else{
        mouse->mousedX = al_pointer->event.mouse.dx;
    }
    for(int i = 0; i < 3; i++){
        if(ball[i].X_way > 1){
            ball[i].X_way = 1;
        }else if(ball[i].X_way < -1){
            ball[i].X_way = -1;
        }
    }
}

void drawGameBackground(ALLEGRO_POINTERS_t *al_pointer){
    al_draw_bitmap(al_pointer->GameImage, 0, 0, 0);
}

void menuControls(ALLEGRO_POINTERS_t *al_pointer, MENU_t *menu, MOUSE_t *mouse, PLATAFORM_t *plataform, bool *shouldRedraw, CONFIG_t *config, TIME_t *time){
    if(al_pointer->event.type == ALLEGRO_EVENT_MOUSE_AXES){
            plataform->XMousePlatform = mouse->mouseX = al_pointer->event.mouse.x;
            mouse->mouseY = al_pointer->event.mouse.y;
        }else if(al_pointer->event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
            mouse->mouseClick = 1;
        }else if(al_pointer->event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            mouse->mouseClick = 0;
        }else if(al_pointer->event.type == ALLEGRO_EVENT_TIMER){
            *shouldRedraw = true;
        }else if(al_pointer->event.type == ALLEGRO_EVENT_KEY_DOWN){
             al_get_keyboard_state(&al_pointer->keyboardState);
             switch(al_pointer->event.keyboard.keycode) {
                 case ALLEGRO_KEY_ESCAPE:
                    if(menu->display == 0)
                        config->exit = true;
                    else if(menu->display == 1 && config->pause == 0){
                        time->pausedTimeInit = al_get_time();
                        config->pause = true;
                    }else if(menu->display == 1 && config->pause == 1){
                        config->pause = false;
                        time->pausedTimeTotal = time->pausedTimeTotal + (al_get_time() - time->pausedTimeInit);
                        time->getpausedTimeInit = 1;
                    }else if(menu->display == 2 || menu->display == 3)
                        menu->display = 0;
                    break;
             }
        }
}

void synchronizeBalls(TIME_t *time, BALL_t *ball, CONFIG_t *config, bool *spawnBall2){
    if((time->actualTime = al_get_time() - time->initialTime - time->pausedTimeTotal) < 2.3){
        if((time->actualTime = al_get_time() - time->initialTime - time->pausedTimeTotal) > 1 && (time->actualTime = al_get_time() - time->initialTime - time->pausedTimeTotal) < 1.1){
            spawnBall(ball, config, 0);
        }else if(ball[0].Y_ball > 300 && ball[0].Y_ball < 305){
            spawnBall(ball, config, 1);
            ball[1].velocity = config->velocity;
        }
    }
    if(config->level == 2 && ball[1].Y_ball < 205 && *spawnBall2 == true){
        spawnBall(ball, config, 2);
        ball[2].velocity = config->velocity;
        *spawnBall2 = false;
    }
}

void credits(){
    printf("Icons made by Freepik, from Flaticon");
}
