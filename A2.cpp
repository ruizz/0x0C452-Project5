#include "stddef.h"
#include "glut.h"
#include <iostream>
#include <vector>
#define START_POINT 0
#define FINISH_POINT 1

using namespace std;

int rectCount = 0;

struct Square {

	int x, y; // center
	int x1, y1; // top-left
	int x2, y2; // bottom-right
	int size;
	Square(int _x, int _y) : x(_x), y(_y) {

		if (rectCount == 0) { // Big square
			x1 = x - 100;
			x2 = x + 100;
			y1 = y - 100;
			y2 = y + 100;
			size = 200;
		} else if (rectCount == 1) { // Medium square
			x1 = x - 75;
			x2 = x + 75;
			y1 = y - 75;
			y2 = y + 75;
			size = 150;
		} else if (rectCount == 2) { // Small square
			x1 = x - 50;
			x2 = x + 50;
			y1 = y - 50;
			y2 = y + 50;
			size = 100;
		}
	}
};

struct Point {

	int x, y;
	int type;
	Point(int _x, int _y) : x(_x), y(_y) {

		if (rectCount == 3)
			type = START_POINT;
		else if (rectCount == 4)
			type = FINISH_POINT;
	}
};

vector<Square> squares; // Holds 3 squares
vector<Point> points; // Holds 2 points

void init(void) {

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glShadeModel(GL_FLAT);
	glPointSize(10.0);

	// Set projection parameters.
	glMatrixMode (GL_PROJECTION);
	gluOrtho2D (0.0, 0.0, 0.0, 0.0);
}

void drawSquare(Square s) {

	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex2i(s.x1, s.y1);
	glVertex2i(s.x1, s.y2);
	glVertex2i(s.x2, s.y2);
	glVertex2i(s.x2, s.y1);
	glEnd();
	glPopMatrix();
}

void drawPoint(Point p) {

	glPushMatrix();

	if (p.type == START_POINT)
		glColor3f(1.0, 0.5, 0.0);
	else if (p.type == FINISH_POINT)
		glColor3f(0.4, 0.7, 1.0);

	glBegin(GL_POINTS);
	glVertex2f(p.x, p.y);
	glEnd();
	glPopMatrix();
}


// DRAW SCREEN OBJECTS HERE
void redraw(void) {

	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < squares.size(); i++) {
		drawSquare(squares[i]);
	}

	for (int i = 0; i < points.size(); i++) {
		drawPoint(points[i]);
	}

	glutSwapBuffers();
}

void reshape(int w, int h) {

	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 500.0, 500.0, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void addObject(int x, int y){

	if (rectCount == 0) // Big square
		squares.push_back(Square(x, y));

	else if (rectCount == 1) // Medium square;
		squares.push_back(Square(x, y));

	else if (rectCount == 2) // Small square
		squares.push_back(Square(x, y));

	else if (rectCount == 3) { // Start point

		for (int i = 0; i < squares.size(); i++) { // Check if within bounds of any squares. Return if so.
			if (x > squares[i].x1 && x < squares[i].x2 && y > squares[i].y1 && y < squares[i].y2)
				return;
		}
		points.push_back(Point(x, y));

	} else if (rectCount == 4) { // End point

		for (int i = 0; i < squares.size(); i++) { // Check if within bounds of any squares. Return if so.
			if (x > squares[i].x1 && x < squares[i].x2 && y > squares[i].y1 && y < squares[i].y2)
				return;
		}
		points.push_back(Point(x, y));
	}

	rectCount++;
	glutPostRedisplay();
}

void mouse (int button, int state, int x, int y) {
	switch(button) {
	case GLUT_LEFT_BUTTON: // Add object.
		if (state == GLUT_DOWN) {
			addObject(x, y);
			glutIdleFunc(NULL);
		}
		if (state == GLUT_UP) {
			glutIdleFunc(NULL);
		}
		break;
	case GLUT_RIGHT_BUTTON: // Clear objects on screen.
		if (state == GLUT_DOWN) {
			squares.clear();
			points.clear();
			rectCount = 0;
			glutPostRedisplay();
			glutIdleFunc(NULL);
		}
		if (state == GLUT_UP) {
			glutIdleFunc(NULL);
		}
		break;
	default:
		break;
	}
}

void calcLines(){

}

int main (int argc, char ** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("CSCE 452 P4");
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_BLEND );
	init();
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}
