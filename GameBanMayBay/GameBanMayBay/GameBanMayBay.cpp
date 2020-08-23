#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include "Dependencies/glew/glew.h"
#include "Dependencies/freeglut/freeglut.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <sys/timeb.h>
#include <sys/utime.h>

#define STEPS 40
#define PI 3.14
const int screenWidth = 1200; //chiều dài màn hình
const int screenHeight = 650; // chiều cao màn hình

int radius = 8; //Bán kinh có hình tròn vẽ planeThreat
int start = 0; //game track
int gv = 0; //Trang thái hiển trị màn hình nào, như mới vào game, game chạy, thua game, trợ giúp
int level = 0;
int score = 0; //Điểm của người chơi

//Để tính frame 
int cframe = 0;
int time = 0;
int timebase = 0;

//Tọa độ của plain main
int planeMain_x = (screenWidth / 2) - (94 / 2);;//vị trí bên trái theo trục x 
int planeMain_y = 0; //vị trí từ dưới lên theo y
const int speedPlane = 20; // tốc độ di chuyển của máy bay
int health = 3;

//Tọa độ viên đạn và trạng thái viên đạn
int shootX = 0;
int shootY = 0;

//Cấu trúc của 1 viên đạn
struct bullet {
    int x = shootX;
    int y = shootY;
    bool is_move = false; //Trang thái di chuyển
    //Vẽ viên đạn
    void Init() {
        glPointSize(5); //
        glBegin(GL_POINTS);//front shooter
        glColor3f(255, 255, 255);
        glVertex2f(x, y); // Vị trí xuất hiện
        glEnd();
    }
    //Di chuyển của viên đạn
    void handleMove() {
        y += 5;
        if (y > screenHeight) {
            is_move = false;
        }
    }
};
std::vector<bullet> bulletList; //Danh sách chứa đạn bắn ra

//Tọa độ của các plane threat
int planeThreat1_y = screenHeight;
int planeThreat1_x = rand() % 928;;
int planeThreat2_y = screenHeight + 140;
int planeThreat2_x = rand() % 928;;
int planeThreat3_y = screenHeight + 280;
int planeThreat3_x = rand() % 928;;
int speedThreat = 1; //Tốc độ của planeThreat

//Font chữ
const int font1 = (int)GLUT_BITMAP_TIMES_ROMAN_24;
const int font2 = (int)GLUT_BITMAP_HELVETICA_18;
const int font3 = (int)GLUT_BITMAP_8_BY_13;

void countFrames(void);
void TangSpeedThreat(void);//Khai báo hàm để gọi tăng speet
//------------------------Mở renderBitmapString--------------------//
//Hàm để hiển thị các thông báo lên màn hình
void renderBitmapString(float x, float y, void* font, const char* string) {
    const char* c;
    glRasterPos2f(x, y); //Vị trí bắt đầu của dòng kí tự
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}
//------------------------Đóng renderBitmapString--------------------//

//------------------------Mở glCircle--------------------//
//Hàm vẽ hình tròn của plane threat
void glCircle(GLint x, GLint y, GLint radius) {
    GLfloat twicePi = (GLfloat)2.0f * PI;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2i(x, y);
    for (int i = 0; i <= STEPS; i++) {
        glVertex2i((GLint)(x + (radius * cos(i * twicePi / STEPS)) + 0.5),
            (GLint)(y + (radius * sin(i * twicePi / STEPS)) + 0.5));
    }
    glEnd();
}
//------------------------Đóng glCircle--------------------//

int getMilliCount() {
    timeb tb;
    ftime(&tb);
    int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return nCount;
}
void sleep(int sleeptime)
{
    int count = 0;
    int beginsleep = getMilliCount();
    while (getMilliCount() - beginsleep < sleeptime)
    {
        count++;
    }
}
//------------------------Mở firstDesign--------------------//
void firstDesign() {
    glClearColor(0.0, 0.0, 0.0, 1); // Hiển thị màu nền
    //Thông báo khi game over
    if (gv == 1) {
        glColor3f(1.0, 0.0, 0.0);
        renderBitmapString(screenWidth / 2 - 100, 400, (void*)font1, "GAME OVER");
        glColor3f(1.0, 1.0, 1.0);
        char buffer2[50];
        sprintf(buffer2, "Your Score is : %d", score);
        renderBitmapString(screenWidth / 2 - 100, 400 - 30, (void*)font1, buffer2);
        renderBitmapString(screenWidth / 2 - 100, 400 - 60, (void*)font2, "SPACE - Choi lai");
        renderBitmapString(screenWidth / 2 - 100, 400 - 90, (void*)font2, "B - Ve Man Hinh Chinh");
        renderBitmapString(screenWidth / 2 - 100, 400 - 120, (void*)font2, "Q - Thoat");
    }
    else if (gv == 2)
    {
        glColor3f(1.0, 1.0, 1.0);
        renderBitmapString(screenWidth / 2 - 100, 400, (void*)font2, "W - Di Len");
        renderBitmapString(screenWidth / 2 - 100, 400 - 30, (void*)font2, "S - Di xuong");
        renderBitmapString(screenWidth / 2 - 100, 400 - 60, (void*)font2, "D - Di Sang Phai");
        renderBitmapString(screenWidth / 2 - 100, 400 - 90, (void*)font2, "A - Di Sang Trai");
        renderBitmapString(screenWidth / 2 - 100, 400 - 120, (void*)font2, "B - Ve Man Hinh Chinh");
    }
    else
    {
        glColor3f(1.0, 1.0, 1.0);
        renderBitmapString(screenWidth / 2 - 100, 400, (void*)font1, "GAME MAY BAY ");

        glColor3f(1.0, 1.0, 1.0);
        renderBitmapString(screenWidth / 2 - 100, 400 - 50, (void*)font2, "SPACE - Bat Dau");
        renderBitmapString(screenWidth / 2 - 100, 400 - 80, (void*)font2, "H - Tro Giup");
        renderBitmapString(screenWidth / 2 - 100, 400 - 110, (void*)font2, "Q - Thoat");

    }

}
//------------------------Đóng firstDesign--------------------//

//------------------------Mở startGame--------------------//
void startGame() {
    int beginFrame = getMilliCount();

    //MÁY BAY CHÍNH
    //Thân máy bay
    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2f(planeMain_x + 38, planeMain_y + 12);
    glVertex2f(planeMain_x + 38, planeMain_y + 32);
    glColor3f(1, 0, 0);

    glVertex2f(planeMain_x + 46, planeMain_y + 40);
    glVertex2f(planeMain_x + 54, planeMain_y + 32);
    glVertex2f(planeMain_x + 54, planeMain_y + 12);
    glEnd();

    //Cánh phải
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeMain_x + 54, planeMain_y + 12);
    glVertex2f(planeMain_x + 54, planeMain_y + 32);
    glVertex2f(planeMain_x + 94, planeMain_y + 4);

    //Cánh trái
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeMain_x + 38, planeMain_y + 12);
    glVertex2f(planeMain_x + 38, planeMain_y + 32);
    glVertex2f(planeMain_x, planeMain_y + 4);
    glEnd();

    //Động cơ phía sau
    glPointSize(12);
    glBegin(GL_POINTS);
    glColor3f(1, 1, 1);
    glVertex2f(planeMain_x + 38, planeMain_y + 8);
    glVertex2f(planeMain_x + 54, planeMain_y + 8);
    glEnd();

    //CÁC MÁY BAY TRỞ NGẠI - PLANETHREAT
    //         ******* BẮT ĐẦU MÁY BAY 1 *********
    //Cạnh Trái Trên
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat1_x + 36, planeThreat1_y - 20);
    glVertex2f(planeThreat1_x + 36, planeThreat1_y - 48);
    glVertex2f(planeThreat1_x, planeThreat1_y);
    glEnd();
    //Cánh Phải Trên
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat1_x + 36, planeThreat1_y - 20);
    glVertex2f(planeThreat1_x + 36, planeThreat1_y - 48);
    glVertex2f(planeThreat1_x + 72, planeThreat1_y);
    glEnd();
    //Cánh Trái Dưới
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat1_x + 36, planeThreat1_y - 20);
    glVertex2f(planeThreat1_x + 36, planeThreat1_y);
    glVertex2f(planeThreat1_x, planeThreat1_y - 40);
    glEnd();
    //Cánh Phải Dưới
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat1_x + 36, planeThreat1_y - 25);
    glVertex2f(planeThreat1_x + 36, planeThreat1_y);
    glVertex2f(planeThreat1_x + 72, planeThreat1_y - 40);
    glEnd();
    //Thân Tròn
    glColor3f(0.000, 0.000, 0.000);
    glCircle(planeThreat1_x + 36, planeThreat1_y - 25, radius);
    //XỬ LÍ DI CHUYỂN
    planeThreat1_y -= speedThreat;//Hướng di chuyển từ trên xuống và tốc độ của planeThreat
    //Khi đi đến cuối thì đưa về vị trí mới, cộng đểm và random vi tri moi
    if (planeThreat1_y < 0) {
        planeThreat1_y = screenHeight;
        score += 1;
        planeThreat1_x = rand() % 928;
        TangSpeedThreat();
    }
    //XỬ LÝ VA CHAM
    // Cần có 2 điều kiện để 2 máy bay va chạm 
    // 1: Kiểm tra xem tọa độ x của máy bay chính TRỪ cho tọa độ x của planeThreat mà nhỏ hơn độ dài của planeThreat thì tức là 2 máy bay đang so le với nhau
    // 2: Nếu Tọa độ y của planeThread CỘNG với độ dài của nó(42)-Nhưng chọn 20 cho hiệu ứng chạm nhìn đẹp- mà NHỎ hơn độ dài của máy bay chính thì 2 máy bay chạm nhau
    if ((abs(planeMain_x - planeThreat1_x) < 90) && (abs(planeThreat1_y - 20 - planeMain_y) < 40))
    {
        health -= 1; //Mất 1 máu
        planeThreat1_y = screenHeight;
        planeThreat1_x = rand() % 928;
    }
    //         ******* HẾT MÁY BAY 1 *********

    //         ******* BẮT ĐẦU MÁY BAY 2 *********
    //Cạnh Trái Trên
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat2_x + 36, planeThreat2_y - 20);
    glVertex2f(planeThreat2_x + 36, planeThreat2_y - 48);
    glVertex2f(planeThreat2_x, planeThreat2_y);
    glEnd();
    //Cánh Phải Trên
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat2_x + 36, planeThreat2_y - 20);
    glVertex2f(planeThreat2_x + 36, planeThreat2_y - 48);
    glVertex2f(planeThreat2_x + 72, planeThreat2_y);
    glEnd();
    //Cánh Trái Dưới
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat2_x + 36, planeThreat2_y - 20);
    glVertex2f(planeThreat2_x + 36, planeThreat2_y);
    glVertex2f(planeThreat2_x, planeThreat2_y - 40);
    glEnd();
    //Cánh Phải Dưới
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat2_x + 36, planeThreat2_y - 25);
    glVertex2f(planeThreat2_x + 36, planeThreat2_y);
    glVertex2f(planeThreat2_x + 72, planeThreat2_y - 40);
    glEnd();
    //Thân Tròn
    glColor3f(0.000, 0.000, 0.000);
    glCircle(planeThreat2_x + 36, planeThreat2_y - 25, radius);
    //XỬ LÍ DI CHUYỂN
    planeThreat2_y -= speedThreat;//Hướng di chuyển từ trên xuống và tốc độ của planeThreat
    //Khi đi đến cuối thì đưa về vị trí mới, cộng đểm và random vi tri moi
    if (planeThreat2_y < 0) {
        planeThreat2_y = screenHeight;
        score += 1;
        planeThreat2_x = rand() % 928;
        TangSpeedThreat();
    }
    //XỬ LÝ VA CHAM
    // Cần có 2 điều kiện để 2 máy bay va chạm 
    // 1: Kiểm tra xem tọa độ x của máy bay chính TRỪ cho tọa độ x của planeThreat mà nhỏ hơn độ dài của planeThreat thì tức là 2 máy bay đang so le với nhau
    // 2: Nếu Tọa độ y của planeThread CỘNG với độ dài của nó(42)-Nhưng chọn 20 cho hiệu ứng chạm nhìn đẹp- mà NHỎ hơn độ dài của máy bay chính thì 2 máy bay chạm nhau
    if ((abs(planeMain_x - planeThreat2_x) < 90) && (abs(planeThreat2_y - 20 - planeMain_y) < 40))
    {
        health -= 1; //Mất 1 máu
        planeThreat2_y = screenHeight;
        planeThreat2_x = rand() % 928;
    }
    //         ******* HẾT MÁY BAY 2 *********

    //         ******* BẮT ĐẦU MÁY BAY 3 *********
    //Cạnh Trái Trên
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat3_x + 36, planeThreat3_y - 20);
    glVertex2f(planeThreat3_x + 36, planeThreat3_y - 48);
    glVertex2f(planeThreat3_x, planeThreat3_y);
    glEnd();
    //Cánh Phải Trên
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat3_x + 36, planeThreat3_y - 20);
    glVertex2f(planeThreat3_x + 36, planeThreat3_y - 48);
    glVertex2f(planeThreat3_x + 72, planeThreat3_y);
    glEnd();
    //Cánh Trái Dưới
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat3_x + 36, planeThreat3_y - 20);
    glVertex2f(planeThreat3_x + 36, planeThreat3_y);
    glVertex2f(planeThreat3_x, planeThreat3_y - 40);
    glEnd();
    //Cánh Phải Dưới
    glBegin(GL_TRIANGLES);
    glColor3f(1, 0, 0);
    glVertex2f(planeThreat3_x + 36, planeThreat3_y - 25);
    glVertex2f(planeThreat3_x + 36, planeThreat3_y);
    glVertex2f(planeThreat3_x + 72, planeThreat3_y - 40);
    glEnd();
    //Thân Tròn
    glColor3f(0.000, 0.000, 0.000);
    glCircle(planeThreat3_x + 36, planeThreat3_y - 25, radius);
    //XỬ LÍ DI CHUYỂN
    planeThreat3_y -= speedThreat;//Hướng di chuyển từ trên xuống và tốc độ của planeThreat
    //Khi đi đến cuối thì đưa về vị trí mới, cộng đểm và random vi tri moi
    if (planeThreat3_y < 0) {
        planeThreat3_y = screenHeight;
        score += 1;
        planeThreat3_x = rand() % 928;
        TangSpeedThreat();
    }
    //XỬ LÝ VA CHAM
    // Cần có 2 điều kiện để 2 máy bay va chạm 
    // 1: Kiểm tra xem tọa độ x của máy bay chính TRỪ cho tọa độ x của planeThreat mà nhỏ hơn độ dài của planeThreat thì tức là 2 máy bay đang so le với nhau
    // 2: Nếu Tọa độ y của planeThread CỘNG với độ dài của nó(42)-Nhưng chọn 20 cho hiệu ứng chạm nhìn đẹp- mà NHỎ hơn độ dài của máy bay chính thì 2 máy bay chạm nhau
    if ((abs(planeMain_x - planeThreat3_x) < 90) && (abs(planeThreat3_y - 20 - planeMain_y) < 40))
    {
        health -= 1; //Mất 1 máu
        planeThreat3_y = screenHeight;
        planeThreat3_x = rand() % 928;
    }
    //         ******* HẾT MÁY BAY 3 *********

    //MÁY BAY CHÍNH BẮN ĐẠN
    for (int i = 0; i < bulletList.size(); i++) {
        if (bulletList.at(i).is_move) {
            bulletList.at(i).Init();
            bulletList.at(i).handleMove();
            if ((abs(planeThreat1_x - bulletList.at(i).x) < 80) && (abs(planeThreat1_y - 20 - bulletList.at(i).y) < 35))
            {
                bulletList.at(i).is_move = false; //Khi bị va chạm thì đạn biến mất và máy bay sẽ xuất hiện ở vị trí mới
                planeThreat1_y = screenHeight + 100;
                planeThreat1_x = rand() % 928;
                score += 1;
                TangSpeedThreat();
                bulletList.erase(bulletList.begin() + i);
            }
            else if ((abs(planeThreat2_x - bulletList.at(i).x) < 80) && (abs(planeThreat2_y - 20 - bulletList.at(i).y) < 35))
            {
                bulletList.at(i).is_move = false;
                planeThreat2_y = screenHeight + 100;
                planeThreat2_x = rand() % 928;
                score += 1;
                TangSpeedThreat();
                bulletList.erase(bulletList.begin() + i);
            }
            else if ((abs(planeThreat3_x - bulletList.at(i).x) < 80) && (abs(planeThreat3_y - 20 - bulletList.at(i).y) < 35))
            {
                bulletList.at(i).is_move = false;
                planeThreat3_y = screenHeight + 100;
                planeThreat3_x = rand() % 928;
                score += 1;
                TangSpeedThreat();
                bulletList.erase(bulletList.begin() + i);
            }
        }
        else {
            bulletList.erase(bulletList.begin() + i); //Nếu đạn đi quá màn hình hay bắn trung threatplane thì xóa đạn
        }
    }
    //HẾT MẠNG THÌ DỪNG GAME
    if (health < 1) {
        start = 0;
        gv = 1;
    }

    //HIỂN TRỊ THÔNG SỐ ĐIỂM FPS, MẠNG
    //Print Score
    char buffer[50];
    sprintf(buffer, "SCORE: %d", score);
    glColor3f(0.000, 1.000, 0.000);
    renderBitmapString(screenWidth - 100, screenHeight - 20, (void*)font3, buffer);
    //Speed Print
    char buffer1[50];
    sprintf(buffer1, "SPEED:%dKm/h", speedThreat);
    glColor3f(0.000, 1.000, 0.000);
    renderBitmapString(screenWidth - 100, screenHeight - 40, (void*)font3, buffer1);
    //Tính FPS
    char buffer2[50];
    strcpy(buffer2, "FPS:");
    glColor3f(0.000, 1.000, 0.000);
    renderBitmapString(screenWidth - 100, screenHeight - 60, (void*)font3, buffer2);
    countFrames();
    //Lượt chơi của người chơi
    char buffer3[50];
    sprintf(buffer3, "HEALTH: %d", health);
    glColor3f(0.000, 1.000, 0.000);
    renderBitmapString(screenWidth - 100, screenHeight - 90, (void*)font3, buffer3);

    glutPostRedisplay();
    int timeDiff = getMilliCount() - beginFrame;
    if (timeDiff < 10)
    {
        sleep(10 - timeDiff);
    }
}
//------------------------Đóng startGame--------------------//

//------------------------Mở TangSpeedThreat--------------------//
void TangSpeedThreat(void) {
    //KHI ĐẠT ĐƯỢC 1 MỐC ĐIỂM TĂNG ĐỘ KHÓ CHO GAME
    if (score > 0 && score % 13 == 0) {
        speedThreat += 1;
    }
}
//------------------------Đóng TangSpeedThreat--------------------//

//------------------------Mở display--------------------//
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (start == 1) {
        glClearColor(0, 0, 0, 0);
        startGame();
    }
    else {
        firstDesign();
    }
    glFlush();
    glutSwapBuffers();
}
//------------------------Đóng display--------------------//

//------------------------Mở keyboardFunc--------------------//
//Hàm xử lí nhấn bàn phím
void keyboardFunc(unsigned char key, int x, int y) {
    if (' ' == key) {
        if (start == 0) {
            start = 1;
            gv = 0;
            score = 0;
            health = 3;
            speedThreat = 1;
            planeThreat1_y = screenHeight;
            planeThreat1_x = rand() % 928;;
            planeThreat2_y = screenHeight + 140;
            planeThreat2_x = rand() % 928;;
            planeThreat3_y = screenHeight + 280;
            planeThreat3_x = rand() % 928;
            planeMain_x = (screenWidth / 2) - (94 / 2);
            planeMain_y = 0;
        }
    }
    if ('h' == key || 'H' == key) {
        start = 0;
        gv = 2;
    }
    if ('b' == key || 'B' == key) {
        start = 0;
        gv = 0;
    }
    if ('q' == key || 'Q' == key) {
        exit(0);
    }
    if ('a' == key || 'A' == key) {
        if (planeMain_x >= 0) {
            planeMain_x -= speedPlane;
            if (planeMain_x < 0) {
                planeMain_x = 0;
            }
        }
    }
    if ('d' == key || 'D' == key) {
        if (planeMain_x <= screenWidth) {
            planeMain_x += speedPlane;
            if (planeMain_x + 94 > screenWidth) {
                planeMain_x = screenWidth - 94;
            }
        }
    }
    if ('s' == key || 'S' == key) {
        if (planeMain_y >= 0) {
            planeMain_y -= speedPlane;
            if (planeMain_y < 0) {
                planeMain_y = 0;
            }
        }
    }
    if ('w' == key || 'W' == key) {
        if (planeMain_y <= screenHeight) {
            planeMain_y += speedPlane;
            if (planeMain_y + 50 > screenHeight) {
                planeMain_y = screenHeight - 50;
            }
        }
    }
}
//------------------------Đóng keyboardFunc--------------------//

//------------------------Đóng mouseFunc--------------------//
void mouseFunc(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            shootX = planeMain_x + 94 / 2;
            shootY = planeMain_y + 50;

            bullet bullet1;
            bullet1.is_move = true;
            bulletList.push_back(bullet1);
        }
    }
}
//------------------------Đóng keyboardFunc--------------------//

//------------------------Mở countFrames--------------------//
void countFrames(void) {
    //FPS nay can sua lai theo thay
    time = glutGet(GLUT_ELAPSED_TIME);
    cframe++;
    if (time - timebase > 50) {
        float fps = cframe * 1000.0 / (time - timebase);
        char buffer2[50];
        sprintf(buffer2, "%f", fps);
        glColor3f(0.000, 1.000, 0.000);
        renderBitmapString(screenWidth - 60, screenHeight - 60, (void*)font3, buffer2);
        timebase = time;
        cframe = 0;
    }
}
//------------------------Đóng countFrames--------------------//

//------------------------Mở timer--------------------//
void timer(int) {
    glutPostRedisplay();
    glutTimerFunc(1000 / 100, timer, 0);
}
//------------------------Đóng timer--------------------//

//------------------------Mở reshape--------------------//
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
}
//------------------------Đóng reshape--------------------//

//------------------------Mở main--------------------//
//Hàm chính
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(screenWidth, screenHeight); //Chiều cao và rộng của màn hình
    glutInitWindowPosition(200, 20); //Vị trí xuất hiện của màn hình
    glutCreateWindow("Game May Bay");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseFunc);
    glutReshapeFunc(reshape);
    glutTimerFunc(1000, timer, 0);
    glutMainLoop();
    return 0;
}
//------------------------Đóng main--------------------//

//------------------------The End--------------------//

