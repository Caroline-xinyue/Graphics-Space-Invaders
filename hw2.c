// hw2.c
#include "hw2.h"
#include "math.h"

int main(int argc, char **argv) {
  GLFWwindow* window;
  
  // Initializes random number generator
  srand(time(NULL));

  // Initialize the library
  if (!glfwInit())
    exit(EXIT_FAILURE);
  
  // Create a window and its OpenGL context
  window = glfwCreateWindow(WIDTH, HEIGHT, "Space Invaders", NULL, NULL);
  if (!window){
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetWindowPos(window, 100, 100);
  // Make the window's context current 
  glfwMakeContextCurrent(window);
  
  // Assign reshape() to be the function called whenever 
  // a reshape event occurs
  glfwSetWindowSizeCallback(window, reshape);
  
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  
  // Assign keyboard() to be the function called whenever
  // a key is pressed or released
  glfwSetKeyCallback(window, keyboard);
  
  // Assign mouse() to be the function called whenever
  // a mouse button is pressed or released
  glfwSetMouseButtonCallback(window, mouse);
  
  // Assign cursor() to be the function called whenever
  // a cursor moves
  glfwSetCursorPosCallback(window, cursor);
  
  init();
  
  start = glfwGetTime();
  bomb_start = glfwGetTime();
  
  // Loop until the user closes the window 
  while (!glfwWindowShouldClose(window)) {
    // clear to background color
    glClear(GL_COLOR_BUFFER_BIT);
    
    draw();
    
    // Swap front and back buffers 
    glfwSwapBuffers(window);
    
    // Poll for and process events
    glfwPollEvents();
  }
  
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

void draw() {
  glLoadIdentity();
  draw_enemies();
  draw_missles();
  draw_ship();
  draw_bombs();
  draw_lives();
  if(game_state == RESUME) {
    update_game_state();
  }
}

void update_game_state() {
  #if DEBUG
  print_debug_info();
  #endif
  
  missle_enemy_collision();
  if(ship.ship_status == ALIVE) {
    bomb_detection();
  }
  update_direction();
  check_win();
  exit_game();
  update_enemies();
  update_ship();
  update_missle();
  update_bomb();
}

void keyboard(GLFWwindow *w, int key, int scancode, int action, int mods) {
  if(key == GLFW_KEY_RIGHT) {
    if(action == GLFW_RELEASE) {
      rightKeyPressed = FALSE;
    } else {
      rightKeyPressed = TRUE;
    }
  } else if(key == GLFW_KEY_LEFT) {
    if(action == GLFW_RELEASE) {
      leftKeyPressed = FALSE;
    } else {
      leftKeyPressed = TRUE;
    }
  }
  if(action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch(key) {
    case GLFW_KEY_SPACE: {
      if(game_state == RESUME && ship.ship_status == ALIVE) {
        fire_missle();
      }
      break;
    }
    case 'r':
    case 'R':
      if(game_state == PAUSED) {
        game_state = RESUME;
      }
      break;
    case 'd':
    case 'D':
      if(game_state == RESUME) {
        game_state = PAUSED;
      }
      if(game_state == PAUSED) {
        update_game_state();
      }
      break;
    case 'z':
    case 'Z':
      missle_stepX -= MISSLE_STEPX;
      break;
    case 'c':
    case 'C':
      missle_stepX += MISSLE_STEPX;
      break;
    case GLFW_KEY_ESCAPE:
    case 'q':
    case 'Q':
      glfwSetWindowShouldClose(w, GL_TRUE);
      break;
    default:
      break;
    }
  }
}

void cursor(GLFWwindow* window, double xpos, double ypos) {
  double ship_width_original_dimension = ship.ship_width * WIDTH / WINDOW_SIZE;
  if(game_state == RESUME && xpos <= WIDTH - 0.5 * ship_width_original_dimension && xpos >= 0.5 *ship_width_original_dimension) {
    ship.ship_translateX = xpos * WINDOW_SIZE / WIDTH;
  }
}

void mouse(GLFWwindow* window, int button, int action, int mods) {
  if(action == GLFW_RELEASE) {
    switch(button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      if(game_state == RESUME && ship.ship_status == ALIVE) {
        fire_missle();
      }
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      if(game_state == RESUME) {
	game_state = PAUSED;
      }
      if(game_state == PAUSED) {
	update_game_state();
      }
      break;
    default:
      break;
    }
  }
}

void fire_missle() {
  int k = missle.missle_index % NUM_MISSLES;
  missle.missle_translate[k].x = ship.ship_translateX;
  missle.missle_translate[k].y = ship.ship_translateY;
  missle.missle_index++;
}

void draw_missles() {
  glColor3f(238.0f/256, 130.0f/256, 238.0f/256);
  for(int k = 0; k < NUM_MISSLES; k++) {
    if(is_in_window(missle.missle_translate[k])) {
      glPushMatrix();
      glPointSize(12);
      glTranslatef(missle.missle_translate[k].x, missle.missle_translate[k].y, 0);
      draw_point();
      glPopMatrix();
    }
  }
}

void draw_bombs() {
  glColor3f(64.0f/256, 224.0f/256, 208.0f/256);
  for(int k = 0; k < NUM_BOMBS; k++) {
    if(is_in_window(bomb.bomb_translate[k])) {
      glPushMatrix();
      glPointSize(12);
      glTranslatef(bomb.bomb_translate[k].x, bomb.bomb_translate[k].y, 0);
      draw_point();
      glPopMatrix();
    }
  }
}

void create_bombs() {
  int k = bomb.bomb_index % NUM_BOMBS;
  int indexX = rand() % NUM_ENEMIES_X;
  int indexY = rand() % NUM_ENEMIES_Y;
  if(enemies.enemy_status[indexX][indexY] != DIED) {
    bomb.bomb_translate[k].x = enemies.enemy_translateX + (1 + INTERVAL_SIZE_RATIO) * indexX * enemies.enemy_size;
    bomb.bomb_translate[k].y = enemies.enemy_translateY + (1 + INTERVAL_SIZE_RATIO) * indexY * enemies.enemy_size;
    bomb.bomb_index++;
  }
}

void draw_robot_ship() {
  draw_circle(-1, -1 - 0.2 * ship.ship_height, 0.5 * 0.6 * ship.ship_width);
  draw_rectangle(-1 + ship.ship_width * 0.45, -1, 0.2 * ship.ship_width, 0.3 * ship.ship_height);
  draw_rectangle(-1 - ship.ship_width * 0.45, -1, 0.2 * ship.ship_width, 0.3 * ship.ship_height);
  draw_rectangle(-1, -1, 0.6 * ship.ship_width, 0.4 * ship.ship_height);
  draw_rectangle(-1 - 0.12 * ship.ship_width, -1 + 0.35 * ship.ship_height, 0.2 * ship.ship_width, 0.3 * ship.ship_height);
  draw_rectangle(-1 + 0.12 * ship.ship_width, -1 + 0.35 * ship.ship_height, 0.2 * ship.ship_width, 0.3 * ship.ship_height);
}

void draw_ship() {
  glColor3f(100.0f/256, 149.0f/256, 237.0f/256);
  if(ship.ship_status == ALIVE) {
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(ship.ship_translateX, ship.ship_translateY, 0);
    draw_robot_ship();
    glPopMatrix();
  } else if(ship.ship_status == DYING) {
    draw_dying_ship();
    if(ship.ship_dying_scale_factor <= 0.01) {
      ship.ship_status = ALIVE;
      ship.ship_dying_angle = 0.0f;
      ship.ship_dying_scale_factor = 1.0f;
    }
  }
}

void draw_lives() {
  glColor3f(100.0f/256, 149.0f/256, 237.0f/256);
  for(int i = 0; i < ship.ship_lives; i++) {
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(12.0f * WINDOW_SIZE / 13 - i * (ship.ship_width * 3 / 2.0f), WINDOW_SIZE / 15.0f, 0);
    draw_robot_ship();
    glPopMatrix();
  }
}

void draw_dying_ship() {
  glLoadIdentity();
  glPushMatrix();
  glTranslatef(ship.ship_translateX, ship.ship_translateY, 0);
  glTranslatef(-1, -1, 0);
  glRotatef(ship.ship_dying_angle, 0, 0, 1);
  glScalef(ship.ship_dying_scale_factor, ship.ship_dying_scale_factor, 1);
  glTranslatef(1, 1, 0);
  draw_robot_ship();
  glPopMatrix();
}

void draw_enemies() {
  double enemy_centerX = 0.0f;
  double enemy_centerY = 0.0f;
  glColor3f(255.0f/256, 218.0f/256, 185.0f/256);
  for(int j = 0; j < NUM_ENEMIES_Y; j++) {
    for(int i = 0; i < NUM_ENEMIES_X; i++) {
      if(enemies.enemy_status[i][j] == ALIVE) {
        enemy_centerX = (1 + INTERVAL_SIZE_RATIO) * i * enemies.enemy_size - 1;
        enemy_centerY = (1 + INTERVAL_SIZE_RATIO) * j * enemies.enemy_size - 1;
        glLoadIdentity();
        glPushMatrix();
        glTranslatef(enemies.enemy_translateX, enemies.enemy_translateY, 0);
        glTranslatef(enemy_centerX, enemy_centerY, 0);
        glRotatef(enemies.angle, 0, 0, 1);
        glTranslatef(-enemy_centerX, -enemy_centerY, 0);
        draw_rectangle(enemy_centerX, enemy_centerY, enemies.enemy_size, enemies.enemy_size);
        glPopMatrix();
      } else if(enemies.enemy_status[i][j] == DYING) {
        draw_dying_enemy(i, j);
        if(enemies.dying_scale_factors[i][j] <= 0.01) {
          enemies.enemy_status[i][j] = DIED;
          enemies.dying_angle = 0.0f;
          enemies.dying_scale_factors[i][j] = 1.0f;
        }
      }
    }
  }
}

void draw_dying_enemy(int indexX, int indexY) {
  double enemy_centerX = (1 + INTERVAL_SIZE_RATIO) * indexX * enemies.enemy_size - 1;
  double enemy_centerY = (1 + INTERVAL_SIZE_RATIO) * indexY * enemies.enemy_size - 1;
  glLoadIdentity();
  glPushMatrix();
  glTranslatef(enemies.enemy_translateX, enemies.enemy_translateY, 0);
  glTranslatef(enemy_centerX, enemy_centerY, 0);
  glRotatef(enemies.dying_angle, 0, 0, 1);
  glScalef(enemies.dying_scale_factors[indexX][indexY], enemies.dying_scale_factors[indexX][indexY], 1);
  glTranslatef(-enemy_centerX, -enemy_centerY, 0);
  draw_rectangle(enemy_centerX, enemy_centerY, enemies.enemy_size, enemies.enemy_size);
  glPopMatrix();
}

void draw_rectangle(double cx, double cy, double width, double height) {
  glBegin(GL_POLYGON);
  // Specify verticies in quad in ccw order
  glVertex2f(cx + width/2, cy - height/2);
  glVertex2f(cx - width/2, cy - height/2);
  glVertex2f(cx - width/2, cy + height/2);
  glVertex2f(cx + width/2, cy + height/2);
  glEnd();
}

void draw_circle(double cx, double cy, double radius) {
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(cx, cy);
  for(int i = 10; i <= 20; i++) {
    glVertex2f(cx + (radius * cos(i * 2.0f * M_PI / 20)), cy + (radius * sin(i * 2.0f * M_PI / 20)));
  }
  glEnd();
}

void draw_point() {
  glBegin(GL_POINTS);
  glVertex2f(-1, -1);
  glEnd();
}

void check_win() {
  for(int j = NUM_ENEMIES_Y - 1; j >= 0; j--) {
    for(int i = 0; i < NUM_ENEMIES_X; i++) {
      if(enemies.enemy_status[i][j] != DIED) {
	if(-1 + enemies.enemy_translateY + (1 + INTERVAL_SIZE_RATIO) * j * enemies.enemy_size >= -1 + ship.ship_translateY - 0.5 * ship.ship_height) {
	  game_state = LOSE;
	}
	return;
      }
    }
  }
  game_state = WIN;
}

void checkRightBoarder() {
  double enemy_centerX = 0.0f;
  for(int i = NUM_ENEMIES_X - 1; i >= 0; i--) {
    for(int j = 0; j < NUM_ENEMIES_Y; j++) {
      if(enemies.enemy_status[i][j] == ALIVE) {
        enemy_centerX = (1 + INTERVAL_SIZE_RATIO) * i * enemies.enemy_size - 1;
        enemies.boarderR = enemy_centerX + enemies.enemy_translateX + enemies.enemy_size/2.0;
        return;
      }
    }
  }
}

void checkLeftBoarder() {
  double enemy_centerX = 0.0f;
  for(int i = 0; i < NUM_ENEMIES_X; i++) {
    for(int j = 0; j < NUM_ENEMIES_Y; j++) {
      if(enemies.enemy_status[i][j] == ALIVE) {
        enemy_centerX = (1 + INTERVAL_SIZE_RATIO) * i * enemies.enemy_size - 1;
        enemies.boarderL = enemy_centerX + enemies.enemy_translateX - enemies.enemy_size/2.0;
        return;
      }
    }
  }
}

void check_boarders() {
  checkRightBoarder();
  checkLeftBoarder();
}

void update_direction() {
  check_boarders();
  if(enemies.enemy_hit_boarderR && enemies.boarderR >= 1.0f) {
    enemies.direction = LEFT;
    enemies.enemy_translateY += enemies.enemy_size;
    enemies.enemy_hit_boarderR = FALSE;
    enemies.enemy_hit_boarderL = TRUE;
  } else if(enemies.enemy_hit_boarderL && enemies.boarderL <= -1.0f) {
    enemies.direction = RIGHT;
    enemies.enemy_translateY += enemies.enemy_size;
    enemies.enemy_hit_boarderL = FALSE;
    enemies.enemy_hit_boarderR = TRUE;
  }
}

void missle_enemy_collision() {
  for(int j = NUM_ENEMIES_Y - 1; j >= 0; j--) {
    for(int i = 0; i < NUM_ENEMIES_X; i++) {
      if(enemies.enemy_status[i][j] == ALIVE) {
	if(collision_detection(i, j)) {
	  return;
	}
      }
    }
  }
}

// parameters: enemy index in the enemy army
int collision_detection(int indexX, int indexY) {
  double enemyX = enemies.enemy_translateX + (1 + INTERVAL_SIZE_RATIO) * indexX * enemies.enemy_size;
  double enemyY = enemies.enemy_translateY + (1 + INTERVAL_SIZE_RATIO) * indexY * enemies.enemy_size;
  for(int k = 0; k < NUM_MISSLES; k++) {
    if(is_in_window(missle.missle_translate[k])) {
      if(get_distance(enemyX, enemyY, missle.missle_translate[k].x, missle.missle_translate[k].y) <= enemies.enemy_circ_radius) {
	enemies.enemy_status[indexX][indexY] = DYING;
	missle.missle_translate[k].x = -2;
	missle.missle_translate[k].y = -2;
	return TRUE;
      }
    }
  }
  return FALSE;
}

void bomb_detection() {
  for(int k = 0; k < NUM_BOMBS; k++) {
    if(is_in_window(bomb.bomb_translate[k])) {
      double bombX = bomb.bomb_translate[k].x;
      double bombY = bomb.bomb_translate[k].y;
      if(bombX >= ship.ship_translateX - ship.ship_width / 2.0 && bombX <= ship.ship_translateX + ship.ship_width / 2.0f && bombY >= ship.ship_translateY - ship.ship_height / 2.0f && bombY <= ship.ship_translateY + ship.ship_height / 2.0f) {
	if(ship.ship_lives > 1) {
	  ship.ship_status = DYING;
	  ship.ship_lives--;
	} else {
	  ship.ship_lives = 0;
	  ship.ship_status = DIED;
	  game_state = LOSE;
	}
	bomb.bomb_translate[k].x = -2;
	bomb.bomb_translate[k].y = -2;
	return;
      }
    }
  }
}

double get_distance(double enemy_centerX, double enemy_centerY, double missle_X, double missle_Y) {
  return sqrt((enemy_centerX - missle_X) * (enemy_centerX - missle_X) + (enemy_centerY - missle_Y) * (enemy_centerY - missle_Y));
}

int is_in_window(Coordinate coordinate) {
  return (coordinate.x - 1 <= 1 && coordinate.x - 1 >= -1 && coordinate.y - 1 <= 1 && coordinate.y - 1 >= -1);
}

void init_enemies() {
  init_enemy_status();
  enemies.enemy_size = (2 * 3 / 5.0f) / (NUM_ENEMIES_X + (NUM_ENEMIES_X - 1) * INTERVAL_SIZE_RATIO);
  enemies.enemy_step = ENEMY_STEP;
  enemies.enemy_translateX = WINDOW_SIZE / 5.0f;
  enemies.enemy_translateY = WINDOW_SIZE / 7.0f;
  enemies.angle = 0.0f;
  enemies.direction = RIGHT;
  enemies.boarderR = 1.0f;
  enemies.boarderL = 1.0f;
  enemies.enemy_hit_boarderR = TRUE;
  enemies.enemy_hit_boarderL = TRUE;
  enemies.enemy_circ_radius = sqrt(2) * enemies.enemy_size / 2.0f;
  init_dying_scale_factors();
  enemies.dying_angle = 0.0f;
}

void init_enemy_status() {
  for(int j = 0; j < NUM_ENEMIES_Y; j++) {
    for(int i = 0; i < NUM_ENEMIES_X; i++) {
      enemies.enemy_status[i][j] = ALIVE;
    }
  }
}

void init_dying_scale_factors() {
  for(int j = 0; j < NUM_ENEMIES_Y; j++) {
    for(int i = 0; i < NUM_ENEMIES_X; i++) {
      enemies.dying_scale_factors[i][j] = 1.0f;
    }
  }
}

void init_ship() {
  ship.ship_status = ALIVE;
  ship.ship_lives = SHIP_LIVES;
  ship.ship_width = 1.8 * enemies.enemy_size;
  ship.ship_height = 1.5 * enemies.enemy_size;
  ship.ship_step = SHIP_STEP;
  ship.ship_translateX = WINDOW_SIZE / 2.0f;
  ship.ship_translateY = WINDOW_SIZE * 14 / 15.0f;
  ship.ship_dying_scale_factor = 1.0f;
  ship.ship_dying_angle = 0.0f;
}

void init_missles() {
  missle.missle_index = 0;
  init_missle_coordinates();
}

void init_missle_coordinates() {
  for(int k = 0; k < NUM_MISSLES; k++) {
    missle.missle_translate[k].x = -2;
    missle.missle_translate[k].y = -2;
  }
}

void init_bombs() {
  bomb.bomb_index = 0;
  init_bomb_coordinates();
}

void init_bomb_coordinates() {
  for(int k = 0; k < NUM_BOMBS; k++) {
    bomb.bomb_translate[k].x = -2;
    bomb.bomb_translate[k].y = -2;
  }
}

void init() {
  // Set the clear color to black
  glClearColor(245.0f/256, 255.0f/256, 250.0f/256, 256.0f/256);
  resize(WIDTH, HEIGHT);
  init_enemies();
  init_ship();
  init_missles();
  init_bombs();
}

void exit_game() {
  if(game_state == WIN) {
    printf("CONGRATULATIONS! You win the space invaders game!\n");
  } else if(game_state == LOSE) {
    printf("SORRY! You lose but keep the sprit up and try again!\n");
  }
}

void reshape(GLFWwindow *w, int width, int height) {
  resize(width, height);
}

void resize(int width, int height) {
  // Switch to projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // Viewing area set to window size
  gluOrtho2D(-1.0f, 1.0f, 1.0f, -1.0f);
  // Return to modelview mode
  glMatrixMode(GL_MODELVIEW);
}

void framebuffer_size_callback(GLFWwindow *w, int width, int height) {
  // Set the new viewport size
  glViewport(0, 0, width, height);
}

void update_enemies() {
  last = glfwGetTime();
  if(last - start > TIME_DURATION) {
    enemies.enemy_translateX = enemies.enemy_translateX + enemies.direction * ENEMY_ACCELERATION * enemies.enemy_step;
    enemies.angle += ANGLE_STEP;
    start = last;
  }
  for(int j = 0; j < NUM_ENEMIES_Y; j++) {
    for(int i = 0; i < NUM_ENEMIES_X; i++) {
      if(enemies.enemy_status[i][j] == DYING) {
	enemies.dying_scale_factors[i][j] *= DYING_STEP;
      }
    }
  }
  enemies.dying_angle += 5 * ANGLE_STEP;
  if(enemies.angle > 360) {
    enemies.angle -= 360;
  } else if(enemies.dying_angle > 360) {
    enemies.dying_angle -= 360;
  }
}

void update_bomb() {
  bomb_last = glfwGetTime();
  if(bomb_last - bomb_start > BOMB_DURATION) {
    create_bombs();
    bomb_start = bomb_last;
  }
  for(int i = 0; i < NUM_BOMBS; i++) {
    if(is_in_window(bomb.bomb_translate[i])) {
      bomb.bomb_translate[i].y += 0.5 * MISSLE_STEPY;
    }
  }
}

void update_missle() {
  for(int i = 0; i < NUM_MISSLES; i++) {
    if(is_in_window(missle.missle_translate[i])) {
      missle.missle_translate[i].x += missle_stepX;
      missle.missle_translate[i].y -= MISSLE_STEPY;
    }
  }
}

void update_ship() {
  if(rightKeyPressed && game_state == RESUME && ship.ship_status == ALIVE) {
    if(ship.ship_translateX >= WINDOW_SIZE - 0.5 * ship.ship_width) {
      ship.ship_translateX = WINDOW_SIZE - 0.5 * ship.ship_width;
    } else {
      ship.ship_translateX += ship.ship_step;
    }
  }
  if(leftKeyPressed && game_state == RESUME && ship.ship_status == ALIVE) {
    if(ship.ship_translateX - ship.ship_step <= 0.5 * ship.ship_width) {
      ship.ship_translateX = 0.5 * ship.ship_width;
    } else {
      ship.ship_translateX -= ship.ship_step;
    }
  }
  if(ship.ship_status == DYING) {
    ship.ship_dying_scale_factor *= DYING_STEP;
    ship.ship_dying_angle += 10 * ANGLE_STEP;
  }
  if(ship.ship_dying_angle > 360) {
    ship.ship_dying_angle -= 360;
  }
}

void  print_debug_info() {
  print_enemies_loc();
  print_ship_loc();
  print_missles_loc();
  print_bomb_loc();
}


void print_missles_loc() {
  for(int i = 0; i < NUM_MISSLES; i++) {
    printf("%dth Missle Position x: %f, y: %f\n", i, -1 + missle.missle_translate[i].x, -1 + missle.missle_translate[i].y);
  }
  printf("\n");
}

void print_bomb_loc() {
  for(int i = 0; i < NUM_BOMBS; i++) {
    printf("%dth Bomb Position x: %f, y: %f\n", i, -1 + bomb.bomb_translate[i].x, -1 + bomb.bomb_translate[i].y);
  }
  printf("\n");
}

void print_ship_loc() {
  printf("Ship Position x: %f, y: %f\n", -1 + ship.ship_translateX, -1 + ship.ship_translateY);
}

void print_enemies_loc() {
  printf("Enemies Position(Upper Left Corner) x: %f, y: %f\n", -1 + enemies.enemy_translateX, -1 + enemies.enemy_translateY);
}
