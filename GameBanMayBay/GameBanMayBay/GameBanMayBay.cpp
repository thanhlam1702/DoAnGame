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
int score = 0; //Điểm của người chơi

//Để tính frame 
int frameCount = 0;
int time = 0;
int timebase = 0;
int maxFps = 60;
float fps;

//Tọa độ của plain main
int planeMain_x = (screenWidth / 2) - (94 / 2);;//vị trí bên trái theo trục x 
int planeMain_y = 0; //vị trí từ dưới lên theo y
const int speedPlane = 20; // tốc độ di chuyển của máy bay
int health = 3;
int k = 1;

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
int numPlaneThreat = 10; //Số lượng máy bay trở ngại
int* planeThreat_x= new int[numPlaneThreat]; //danh sách tọa độ x của các máy bay
int* planeThreat_y = new int[numPlaneThreat];//danh sách tọa độ y của các máy bay
int speedThreat = 1; //Tốc độ của planeThreat
double red[10];
double blue[10];
double green[10];

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
    for (int i = 0; i < numPlaneThreat; i++) {
        glBegin(GL_TRIANGLES);
        glColor3f(red[i], green[i], blue[i]);
        glVertex2f(planeThreat_x[i] + 36 * k, planeThreat_y[i] - 20 * k);
        glVertex2f(planeThreat_x[i] + 36 * k, planeThreat_y[i] - 48 * k);
        glVertex2f(planeThreat_x[i], planeThreat_y[i]);
        glEnd();
        //Cánh Phải Trên
        glBegin(GL_TRIANGLES);
        glColor3f(red[i], green[i], blue[i]);
        glVertex2f(planeThreat_x[i] + 36 * k, planeThreat_y[i] - 20 * k);
        glVertex2f(planeThreat_x[i] + 36 * k, planeThreat_y[i] - 48 * k);
        glVertex2f(planeThreat_x[i] + 72 * k, planeThreat_y[i]);
        glEnd();
        //Cánh Trái Dưới
        glBegin(GL_TRIANGLES);
        glColor3f(red[i], green[i], blue[i]);
        glVertex2f(planeThreat_x[i] + 36 * k, planeThreat_y[i] - 20 * k);
        glVertex2f(planeThreat_x[i] + 36 * k, planeThreat_y[i]);
        glVertex2f(planeThreat_x[i], planeThreat_y[i] - 40 * k);
        glEnd();
        //Cánh Phải Dưới
        glBegin(GL_TRIANGLES);
        glColor3f(red[i], green[i], blue[i]);
        glVertex2f(planeThreat_x[i] + 36 * k, planeThreat_y[i] - 25 * k);
        glVertex2f(planeThreat_x[i] + 36 * k, planeThreat_y[i]);
        glVertex2f(planeThreat_x[i] + 72 * k, planeThreat_y[i] - 40 * k);
        glEnd();
        //Thân Tròn
        glColor3f(0.000, 0.000, 0.000);
        glCircle(planeThreat_x[i] + 36 * k, planeThreat_y[i] - 25 * k, radius * k);
        //XỬ LÍ DI CHUYỂN
        planeThreat_y[i] -= speedThreat;//Hướng di chuyển từ trên xuống và tốc độ của planeThreat
    }

    //VẼ ĐẠN LÊN MÀN HÌNH
    for (int i = 0; i < bulletList.size(); i++) {
        bulletList.at(i).Init();
        bulletList.at(i).handleMove();
    }

    //HIỂN TRỊ THÔNG SỐ ĐIỂM FPS, MẠNG
    //Print Score
    char buffer[50];
    sprintf(buffer, "SCORE: %d", score);
    glColor3f(0.000, 1.000, 0.000);
    renderBitmapString(screenWidth - 100, screenHeight - 20, (void*)font3, buffer);
    //Speed Print
    char buffer1[50];
    sprintf(buffer1, "LEVEL:%d", speedThreat);
    glColor3f(0.000, 1.000, 0.000);
    renderBitmapString(screenWidth - 100, screenHeight - 40, (void*)font3, buffer1);
    //Tính FPS
    countFrames();
    char buffer2[50];
    sprintf(buffer2, "FPS:%f", fps);
    glColor3f(0.000, 1.000, 0.000);
    renderBitmapString(screenWidth - 100, screenHeight - 60, (void*)font3, buffer2);
    //Lượt chơi của người chơi
    char buffer3[50];
    sprintf(buffer3, "HEALTH: %d", health);
    glColor3f(0.000, 1.000, 0.000);
    renderBitmapString(screenWidth - 100, screenHeight - 90, (void*)font3, buffer3);
    //so luon may bay
    char buffer4[50];
    sprintf(buffer4, "Num: %d", numPlaneThreat);
    glColor3f(0.000, 1.000, 0.000);
    renderBitmapString(screenWidth - 100, screenHeight - 120, (void*)font3, buffer4);
    
}
//------------------------Đóng startGame--------------------//

//------------------------Mở setPositionPlaneThreat--------------------//
void setPositionPlaneThreat() { //Khởi tạo vị trí ngẫu nhiên cho các máy bay cản khi bắt đầu game
    for (int i = 0; i < numPlaneThreat; i++) {
        planeThreat_x[i] = rand() % 928;
        planeThreat_y[i] = screenHeight + (i * 140);
        red[i] = (rand() % 11) * 1.0 / 10;
        green[i] = (rand() % 11) * 1.0 / 10;
        blue[i] = (rand() % 11) * 1.0 / 10;
    }
}
//------------------------Đóng setPositionPlaneThreat--------------------//

//------------------------Mở TangSpeedThreat--------------------//
void TangSpeedThreat(void) {
    //KHI ĐẠT ĐƯỢC 1 MỐC ĐIỂM TĂNG ĐỘ KHÓ CHO GAME
    if (score > 0 && score % 13 == 0) {
        speedThreat += 1;
    }
}
//------------------------Đóng TangSpeedThreat--------------------//

//------------------------Mở display--------------------//
void render() {
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

//------------------------Mở update--------------------//
void update() {
    //XỬ LÝ CỦA MÁY BAY VỚI CÁC PLANETHREAT
    for (int i = 0; i < numPlaneThreat; i++) {
        if (i % 3 == 0 && planeThreat_y[i] < (screenHeight * 2 / 3 )) {
            int temp = rand() % 2;
            if (temp == 0) {
                planeThreat_x[i] += 4;
            }
            else
            {
                planeThreat_x[i] -= 4;
            }
        }
        //Khi đi đến cuối thì đưa về vị trí mới, cộng đểm và random vi tri moi
        if (planeThreat_y[i] < 0) {
            planeThreat_y[i] = screenHeight;
            score += 1;
            planeThreat_x[i] = rand() % 928;
            TangSpeedThreat();
        }
        //XỬ LÝ VA CHAM
       // Cần có 2 điều kiện để 2 máy bay va chạm 
       // 1: Kiểm tra xem tọa độ x của máy bay chính TRỪ cho tọa độ x của planeThreat mà nhỏ hơn độ dài của planeThreat thì tức là 2 máy bay đang so le với nhau
       // 2: Nếu Tọa độ y của planeThread CỘNG với độ dài của nó(42)-Nhưng chọn 20 cho hiệu ứng chạm nhìn đẹp- mà NHỎ hơn độ dài của máy bay chính thì 2 máy bay chạm nhau
        if ((abs(planeMain_x - planeThreat_x[i]) < 90*k) && (abs(planeThreat_y[i] - 20*k - planeMain_y) < 40*k))
        {
            health -= 1; //Mất 1 máu
            planeThreat_y[i] = screenHeight + 140;
            planeThreat_x[i] = rand() % 928;
        }
    }

    //MÁY BAY CHÍNH BẮN ĐẠN
    for (int j = 0; j < bulletList.size(); j++) {
        if (bulletList.at(j).is_move) {
            for (int i = 0; i < numPlaneThreat; i++) {
                if ((abs(planeThreat_x[i] - bulletList.at(j).x) < 80*k) && (abs(planeThreat_y[i] - 20*k - bulletList.at(j).y) < 35*k))
                {
                    bulletList.at(j).is_move = false; //Khi bị va chạm thì đạn biến mất và máy bay sẽ xuất hiện ở vị trí mới
                    planeThreat_y[i] = screenHeight + 140;
                    planeThreat_x[i] = rand() % 928;
                    score += 1;
                    TangSpeedThreat();
                    bulletList.erase(bulletList.begin() + j);
                    return;
                }
            }
        }
        else {
            bulletList.erase(bulletList.begin() + j); //Nếu đạn đi quá màn hình thì xóa đạn
        }
    }
    //HẾT MẠNG THÌ DỪNG GAME
    if (health < 1) {
        start = 0;
        gv = 1;
    }
    int beginFrame = getMilliCount();
    glutPostRedisplay();
    int timeDiff = getMilliCount() - beginFrame;
    int countK = 0;
    countK++;
    if (timeDiff < 1000 / maxFps)
    {
        sleep(1000 / maxFps - timeDiff);
    }
}
//------------------------Đóng update--------------------//

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
            setPositionPlaneThreat();
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
        health = 3;
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
    if ('9' == key) {
        numPlaneThreat += 1;
    }
    if ('0' == key) {
        numPlaneThreat -= 1;
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
    time = glutGet(GLUT_ELAPSED_TIME);
    frameCount++;
    if (time - timebase > 50) {
        fps = frameCount / ((time - timebase) / 1000.0);
        timebase = time;
        frameCount = 0;
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
    glMatrixMode(GL_MODELVIEW);
}
//------------------------Đóng reshape--------------------//

//------------------------Mở main--------------------//
//Hàm chính
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(screenWidth, screenHeight); //Chiều cao và rộng của màn hình
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-screenWidth)/2, (glutGet(GLUT_SCREEN_HEIGHT) - screenHeight) / 2); //Vị trí xuất hiện của màn hình
    glutCreateWindow("Game May Bay");
    glutDisplayFunc(render);
    //inputProcess
    glutKeyboardFunc(keyboardFunc);
    glutMouseFunc(mouseFunc);
    reshape(screenWidth,screenHeight);
    glutTimerFunc(1000, timer, 0);
    glutIdleFunc(update);
    glutMainLoop();
    return 0;
}
//------------------------Đóng main--------------------//

//------------------------The End--------------------//

