/* Syeda Batool Jafri 22401327 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600;
const int TIMEPERIOD = 16;
#define RADIANS 0.01745329252 

bool game_on = false;
bool paused = false;

int score = 0;
int last_point = 0;
int destroyed = 0;
int burst = 0;
float time_left = 30;

float weapon_y = 0;

bool bullet_on_screen = false;
float bullet_x, bullet_y;

#define NUM 5

struct Target {
    float x, y;
    float speed;
    bool on_screen;
};

Target targets[NUM];

void vprint(int x, int y, void* font, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
        glutBitmapCharacter(font, str[i]);
}

void circle(float x, float y, float r) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 3) {
        glVertex2f(x + r * cos(i * RADIANS), y + r * sin(i * RADIANS));
    }
    glEnd();
}

void drawScoreboard() {
    glColor3ub(255, 255, 255);
    vprint(360, 310, GLUT_BITMAP_8_BY_13, "REMAINING");
    vprint(360, 290, GLUT_BITMAP_8_BY_13, "TIME");
    glColor3ub(0, 255, 0);
    vprint(380, 260, GLUT_BITMAP_8_BY_13, "%.2f", time_left);

    glColor3ub(255, 255, 255);
    vprint(360, 210, GLUT_BITMAP_8_BY_13, "TOTAL");
    vprint(360, 190, GLUT_BITMAP_8_BY_13, "ROCKETS");
    glColor3ub(255, 255, 0);
    vprint(380, 160, GLUT_BITMAP_8_BY_13, "%02d", destroyed);

    glColor3ub(255, 255, 255);
    vprint(360, 110, GLUT_BITMAP_8_BY_13, "BURST");
    vprint(360, 90, GLUT_BITMAP_8_BY_13, "ROCKETS");
    glColor3ub(255, 255, 0);
    vprint(380, 60, GLUT_BITMAP_8_BY_13, "%02d", burst);

    glColor3ub(255, 255, 255);
    vprint(360, 10, GLUT_BITMAP_8_BY_13, "LAST");
    vprint(360, -10, GLUT_BITMAP_8_BY_13, "POINTS");
    glColor3ub(255, 255, 0);
    vprint(380, -40, GLUT_BITMAP_8_BY_13, "%02d", last_point);

    glColor3ub(255, 255, 255);
    vprint(360, -90, GLUT_BITMAP_8_BY_13, "TOTAL");
    vprint(360, -110, GLUT_BITMAP_8_BY_13, "POINTS");
    glColor3ub(255, 0, 0);
    vprint(380, -140, GLUT_BITMAP_8_BY_13, "%02d", score);
}

void drawControls() {
    glColor3ub(255, 255, 0);
    vprint(-220, -340, GLUT_BITMAP_8_BY_13, "<SpaceBar> Fire, <F1> Start/Restart/Pause, <ESC> Pause");
}

void drawBackground() {
    float height = 670.0;
    float step = 10.0;

    glBegin(GL_QUADS);
    for (float y = -320.0; y < 350.0; y += step) {
        float nextY = y + step;

        float ratio1 = (y + 320.0) / height;
        float ratio2 = (nextY + 320.0) / height;

        glColor3f(0.0, 0.0, 0.3 * ratio1);
        glVertex2f(-500, y);
        glVertex2f(350, y);

        glColor3f(0.0, 0.0, 0.3 * ratio2);
        glVertex2f(350, nextY);
        glVertex2f(-500, nextY);
    }
    glEnd();
}

void drawRocket(float x, float y) {
    glColor3ub(180, 180, 180);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 60);
    glVertex2f(x - 10, y + 40);
    glVertex2f(x + 10, y + 40);
    glEnd();

    glColor3ub(200, 200, 200);
    glRectf(x - 10, y + 40, x + 10, y - 40);

    glColor3ub(150, 150, 150);
    glBegin(GL_POLYGON);
    glVertex2f(x - 10, y);
    glVertex2f(x - 20, y - 40);
    glVertex2f(x - 10, y - 40);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(x + 10, y);
    glVertex2f(x + 20, y - 40);
    glVertex2f(x + 10, y - 40);
    glEnd();
 
    glColor3ub(100, 100, 100);
    glRectf(x - 8, y - 40, x + 8, y - 45);

    glColor3ub(255, 120, 0);
    circle(x, y - 45, 9);
    circle(x - 5, y - 48, 7);
    circle(x + 5, y - 48, 7);

    glColor3ub(255, 200, 0);
    circle(x, y - 50, 6);
}

void drawCannon(float x, float y) {
    glColor3ub(139, 69, 19);
    glBegin(GL_POLYGON);
    glVertex2f(x, y + 30);
    glVertex2f(x + 50, y + 20);
    glVertex2f(x + 50, y - 20);
    glVertex2f(x, y - 30);
    glEnd();

    glColor3ub(100, 100, 100);
    glRectf(x + 40, y + 15, x + 60, y - 15);

    glColor3ub(50, 50, 50);
    circle(x + 55, y, 6);
}

void drawBullet(float x, float y) {
    glColor3ub(255, 100, 0);
    glRectf(x, y + 5, x + 20, y - 5);

    glColor3ub(255, 255, 0);
    glBegin(GL_POLYGON);
    glVertex2f(x + 20, y + 4);
    glVertex2f(x + 30, y);
    glVertex2f(x + 20, y - 4);
    glEnd();
}

void drawGameArea() {
    drawBackground();

    glColor3ub(140, 140, 140);
    glRectf(-500, 350, -450, -350);

    glColor3ub(10, 10, 10);
    glRectf(350, 350, 500, -350);

    drawCannon(-450, weapon_y);

    if (bullet_on_screen) {
        drawBullet(bullet_x, bullet_y);
    }

    for (int i = 0; i < NUM; i++) {
        if (targets[i].on_screen) {
            drawRocket(targets[i].x, targets[i].y);
        }
    }
}

void initTargets() {
    for (int i = 0; i < NUM; i++) {
        targets[i].x = (rand() % 310) + 10;
        targets[i].y = -350 - (i * 150) - (rand() % 50);
        targets[i].speed = 2.5 + (rand() % 20) / 10.0;
        targets[i].on_screen = true;
    }
}

void checkCollision() {
    for (int i = 0; i < NUM; i++) {
        if (!targets[i].on_screen) continue;

        float dx = bullet_x - targets[i].x;
        float dy = bullet_y - targets[i].y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist < 60) {
            targets[i].y = -400 - (rand() % 100);
            targets[i].x = (rand() % 310) + 10;
            targets[i].speed = 2.5 + (rand() % 20) / 10.0;

            bullet_on_screen = false;
            bullet_x = -600;

            float from_center = fabs(dy);

            if (from_center < 15) last_point = 5;
            else if (from_center < 30) last_point = 4;
            else if (from_center < 50) last_point = 3;
            else last_point = 1;

            score += last_point;
            destroyed++;
            burst++; 
        }
    }
}

void display() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    drawGameArea();
    drawScoreboard();
    drawControls();

    glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y) {
    if (key == ' ' && game_on && !paused && !bullet_on_screen) {
        bullet_on_screen = true;
        bullet_x = -390;
        bullet_y = weapon_y;
    }

    if (key == 27) {
        if (game_on) {
            paused = !paused;
        }
    }
}

void onSpecialKeyDown(int key, int x, int y) {
    if (key == GLUT_KEY_F1) {
        if (!game_on) {
            game_on = true;
            paused = false;
            score = 0;
            last_point = 0;
            destroyed = 0;
            burst = 0;
            time_left = 30;
            bullet_on_screen = false;
            weapon_y = 0;
            initTargets();
        }
        else {
            paused = !paused;
        }
    }

    if (!game_on || paused) return;

    if (key == GLUT_KEY_UP)
        weapon_y += 10;

    if (key == GLUT_KEY_DOWN)
        weapon_y -= 10;

    if (weapon_y > 320) weapon_y = 320;
    if (weapon_y < -300) weapon_y = -300;
}

void onTimer(int v) {
    if (game_on && !paused) {
        time_left -= 0.016;
        if (time_left < 0) {
            time_left = 0;
            game_on = false;
        }

        if (bullet_on_screen) {
            bullet_x += 15;
            if (bullet_x > 350)
                bullet_on_screen = false;
        }

        for (int i = 0; i < NUM; i++) {
            if (targets[i].on_screen) {
                targets[i].y += targets[i].speed;

                if (targets[i].y > 400) {
                    targets[i].y = -400 - (rand() % 100);
                    targets[i].x = (rand() % 310) + 10;
                    targets[i].speed = 2.5 + (rand() % 20) / 10.0;
                }
            }
        }

        if (bullet_on_screen)
            checkCollision();
    }

    glutTimerFunc(TIMEPERIOD, onTimer, 0);
    glutPostRedisplay();
}

void onResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-500, 500, -350, 350, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("CTIS164 HW2 - 2025-2026 SPRING - Syeda Batool Jafri");
    glutDisplayFunc(display);
    glutReshapeFunc(onResize);
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);
    glutTimerFunc(TIMEPERIOD, onTimer, 0);
    glutMainLoop();
    return 0;
}