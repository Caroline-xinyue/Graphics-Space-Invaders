// hw2.h
#ifndef HW2_H
#define HW2_H
#include <stdlib.h>
#include <stdio.h>
#ifdef __APPLE__
#include <GLFW/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif
#include <GL/glu.h>
#include "time.h"
#include "math.h"
#include "stdlib.h"

typedef enum {DIED, DYING, ALIVE} status;
typedef enum {PAUSED, RESUME, WIN, LOSE} gameState;
typedef enum {FALSE, TRUE} boolean;
typedef enum {LEFT=-1, RIGHT=1} direction;

#define WIDTH 800
#define HEIGHT 800
#define WINDOW_SIZE 2
#define NUM_ENEMIES_X 11
#define NUM_ENEMIES_Y 5
#define NUM_MISSLES 20
#define NUM_BOMBS 50
#define SHIP_LIVES 3
#define TIME_DURATION 0.02
#define BOMB_DURATION 0.15
#define INTERVAL_SIZE_RATIO 0.7

// The steps below are the speed for each object when the game proceed one state
#define ENEMY_STEP 2.0 / WIDTH
#define SHIP_STEP 8.0 / WIDTH
#define MISSLE_STEPX 0.01
#define MISSLE_STEPY 0.05
#define ANGLE_STEP 1
#define DYING_STEP 0.95
#define ENEMY_ACCELERATION 1.2

typedef struct _coordinate {
  double x;
  double y;
} Coordinate;

typedef struct _enemies {
  status enemy_status[11][5];
  double enemy_size;
  double enemy_circ_radius;
  double enemy_translateX;
  double enemy_translateY;
  direction direction;
  double enemy_step;
  double angle;
  double boarderR;
  double boarderL;
  boolean enemy_hit_boarderR;
  boolean enemy_hit_boarderL;
  double dying_scale_factors[11][5];
  double dying_angle;
} Enemies;

typedef struct _ship {
  status ship_status;
  int ship_lives;
  double ship_width;
  double ship_height;
  double ship_step;
  double ship_translateX;
  double ship_translateY;
  double ship_dying_scale_factor;
  double ship_dying_angle;
} Ship;

typedef struct _missle {
  Coordinate missle_translate[NUM_MISSLES];
  int missle_index;
} Missle;

typedef struct _bomb {
  Coordinate bomb_translate[NUM_BOMBS];
  int bomb_index;
} Bomb;

double start = 0.0f;
double last = 0.0f;
double bomb_start = 0.0f;
double bomb_last = 0.0f;

gameState game_state = RESUME;
boolean rightKeyPressed = FALSE;
boolean leftKeyPressed = FALSE;
double missle_stepX = 0.0f;

Enemies enemies;
Ship ship;
Missle missle;
Bomb bomb;

void init();
void init_enemies();
void init_ship();
void init_missles();
void init_bombs();
void init_enemy_status();
void init_dying_scale_factors();
void init_missle_coordinates();
void init_bomb_coordinates();

void draw();
void keyboard(GLFWwindow *w, int key, int scancode, int action, int mods);
void cursor(GLFWwindow* window, double xpos, double ypos);
void mouse(GLFWwindow* window, int button, int action, int mods);
void reshape(GLFWwindow *w, int width, int height);
void resize(int width, int height);
void framebuffer_size_callback(GLFWwindow *w, int width, int height);

void fire_missle();
void draw_rectangle(double centerX, double centerY, double width, double height);
void draw_circle(double cx, double cy, double radius);
void draw_point();
void draw_missles();
void draw_bombs();
void draw_robot_ship();
void draw_ship();
void draw_dying_ship();
void draw_enemies();
void draw_dying_enemy(int indexX, int indexY);
void draw_lives();
void check_win();
void checkRightBoarder();
void checkLeftBoarder();
void check_boarders();
void update_direction();
double get_distance(double enemy_centerX, double enemy_centerY, double missle_X, double missle_Y);
int is_in_window(Coordinate coordinate);
int collision_detection();
void bomb_detection();
void missle_enemy_collision();
void update_game_state();
void create_bombs();
void exit_game();

void update_enemies();
void update_ship();
void update_missle();
void update_bomb();

void  print_debug_info();
void  print_enemies_loc();
void  print_ship_loc();
void  print_missles_loc();
void  print_bomb_loc();
#endif
