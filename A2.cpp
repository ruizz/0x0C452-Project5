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
	Point(){}
	Point(int _x, int _y) : x(_x), y(_y) {

		if (rectCount == 3)
			type = START_POINT;
		else if (rectCount == 4)
			type = FINISH_POINT;
		else
			type = 2;
	}
};

struct Line {

	int x1, y1;
	int x2, y2;
	
	Line(int _x1, int _y1, int _x2, int _y2) : x1(_x1), y1(_y1),x2(_x2), y2(_y2) {

	}
};
struct Node	//for dijkstra
{
	Point pos;
	Node* from;
	double curDist;
	vector<Node*> neighbors;
	vector<double> dist;

	Node(int _x, int _y):pos(Point(_x,_y))
	{
		from = NULL;
		curDist = 500*500+500*500;	//just a big enough number
	}
};
vector<Square> squares; // Holds 3 squares
vector<Point> points; // Holds 2 points
vector<Line> lines;
vector<Line> connections;	//just for display
vector<Node*> dijkstra;
vector<Line> path;
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
	else
		glColor3f(0.8, 0.7, 0.4);
	glBegin(GL_POINTS);
	glVertex2f(p.x, p.y);
	glEnd();
	glPopMatrix();
}

void drawLine(Line l)
{
	glColor3f(0.8, 0.8, 0.4);
	glBegin(GL_LINES);
	glVertex2d(l.x1, l.y1);
	glVertex2d(l.x2, l.y2);
	glEnd();
}

void drawConnections(Line l)
{
	glColor3f(0.4, 0.4, 0.4);
	glBegin(GL_LINES);
	glVertex2d(l.x1, l.y1);
	glVertex2d(l.x2, l.y2);
	glEnd();
}
void drawPath()
{
	glColor3f(1, 0, 0);
	
	glBegin(GL_LINES);
	for(int i=0; i<path.size(); i++)
	{
		glVertex2d(path[i].x1, path[i].y1);
		glVertex2d(path[i].x2, path[i].y2);
		
	}
	glEnd();
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
	for(int i=0; i<lines.size(); i++)
	{
		drawLine(lines[i]);	
	}
	for(int i=0; i<connections.size(); i++)
	{
		drawConnections(connections[i]);
	}
	drawPath();
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
void getLines()
{
	for(int i=0; i<squares.size(); i++)
	{
		
		int uplimitleft = 0;
		int lolimitleft = 500;
		int uplimitright = 0;
		int lolimitright = 500;

		if(squares[i].y1 < 0)
		{
			uplimitleft = -1;
			uplimitright = -1;
		}
		if(squares[i].y2 > 500)
		{
			lolimitleft = -1;
			lolimitright = -1;
		}
		if(squares[i].x1 <0)
		{
			uplimitleft = -1;
			lolimitleft = -1;
		}
		if(squares[i].x2 >500)
		{
			uplimitright = -1;
			lolimitright = -1;
		}
		//printf("%d %d %d %d\n", squares[i].x1, squares[i].y1, squares[i].x2, squares[i].y2);
		for(int j=0; j<squares.size(); j++)
		{
			if(i!=j)
			{

				//printf("%d %d\n", i, j);
				
				if(squares[i].x1 >= squares[j].x1 && squares[i].x1 <= squares[j].x2)
				{
					if(squares[i].y1 > squares[j].y2 && squares[i].y1 > squares[j].y1 && uplimitleft != -1 && uplimitleft < squares[j].y2)
					{
						//upper limit
						uplimitleft = squares[j].y2;
					}
					else if(squares[i].y1 > squares[j].y1 && squares[i].y1 < squares[j].y2)
					{
						uplimitleft = -1;
					}
					if(squares[i].y2 < squares[j].y1 && squares[i].y2 < squares[j].y2 && lolimitleft != -1 && lolimitleft > squares[j].y1)
					{
						//upper limit
						lolimitleft = squares[j].y1;
					}
					else if(squares[i].y2 > squares[j].y1 && squares[i].y2 < squares[j].y2)
					{
						lolimitleft = -1;
					}
				}
				
				if(squares[i].x2 >= squares[j].x1 && squares[i].x2 <= squares[j].x2)
				{
					if(squares[i].y1 > squares[j].y2 && squares[i].y1 > squares[j].y1 && uplimitright != -1 && uplimitright < squares[j].y2)
					{
						//upper limit
						uplimitright = squares[j].y2;
					}
					else if(squares[i].y1 > squares[j].y1 && squares[i].y1 < squares[j].y2)
					{
						uplimitright = -1;
					}
					if(squares[i].y2 < squares[j].y1 && squares[i].y2 < squares[j].y2 && lolimitright != -1 && lolimitright > squares[j].y1)
					{
						//upper limit
						lolimitright = squares[j].y1;
					}
					else if(squares[i].y2 > squares[j].y1 && squares[i].y2 < squares[j].y2)
					{
						lolimitright = -1;
					}
				}

			}
		}
		if(uplimitleft != -1)
			lines.push_back(Line(squares[i].x1, squares[i].y1, squares[i].x1, uplimitleft));
		if(lolimitleft != -1)
			lines.push_back(Line(squares[i].x1, lolimitleft, squares[i].x1, squares[i].y2));
		if(uplimitright != -1)
			lines.push_back(Line(squares[i].x2, squares[i].y1, squares[i].x2, uplimitright));
		if(lolimitright != -1)
			lines.push_back(Line(squares[i].x2, lolimitright, squares[i].x2, squares[i].y2));
	}

	
}
bool ableToConnect(Point p1, Point p2)
{
	if(p1.x == p2.x && p1.y == p2.y)
		return false;
	if(p1.x == p2.x)
	{
		for(int k=0; k<squares.size(); k++)
		{
			if(squares[k].x1 <= p1.x && p1.x <= squares[k].x2)
			{
				int up = p1.y;
				int down = p2.y;
				if(up> down)
				{
					down=p1.y;
					up=p2.y;
				}
				if((up <= squares[k].y1 && squares[k].y1 <= down) || (up <= squares[k].y2 && squares[k].y2 <= down))
				{
					return false;
				}
			}
		}
	}
	else
	{
		double slope = (double)(p1.y - p2.y)/(p1.x-p2.x);
		double c = p1.y - slope*p1.x;
		//printf("points : %d %d, %d %d\n", p1.x, p1.y, p2.x, p2.y);
		//printf("equation = %.1f*x + %.2f\n", slope, c);

		for(int k=0; k<squares.size(); k++)
		{
			int lx;
			int rx;
			if(p1.x < p2.x)
			{
				lx = p1.x;
				rx = p2.x;
			}
			else
			{
				lx = p2.x;
				rx = p1.x;
			}

			if(squares[k].x1 >= lx && squares[k].x1 <= rx)
			{
				double y = squares[k].x1 * slope + c;
				//printf("y = %f, y1 = %d, y2 = %d\n", y, squares[k].y1, squares[k].y2);
				if((double)squares[k].y1 <= y && y <= (double)squares[k].y2)
				{
					return false;
				}
			}
			if(squares[k].x2 >= lx && squares[k].x2 <= rx)
			{
				double y = squares[k].x2 * slope + c;
				//printf("y = %f, y1 = %d, y2 = %d\n", y, squares[k].y1, squares[k].y2);
				if((double)squares[k].y1 <= y && y <= (double)squares[k].y2)
				{
					return false;
				}
			}

			int up = p1.y;	//upper point
			int down = p2.y;	//lower point
			if(up> down)
			{
				down=p1.y;
				up=p2.y;
			}
			if(squares[k].y1 >= up && squares[k].y1 <= down)
			{
				double x = (squares[k].y1-c) / slope;
				//printf("x = %f, x1 = %d, x2 = %d\n", x, squares[k].x1, squares[k].x2);
				if((double)squares[k].x1 <= x && x <= (double)squares[k].x2)
				{
					return false;
				}
			}
			if(squares[k].y2 >= up && squares[k].y2 <= down)
			{
				double x = (squares[k].y2-c) / slope;
				//printf("x = %f, x1 = %d, x2 = %d\n", x, squares[k].x1, squares[k].x2);
				if((double)squares[k].x1 <= x && x <= (double)squares[k].x2)
				{
					return false;
				}
			}
		}
	}
	return true;
}
void calcPath()
{
	for(int i=0; i<points.size(); i++)
	{
		dijkstra.push_back(new Node(points[i].x, points[i].y));
	}
	dijkstra[0]->curDist = 0;
	for(int i=0; i<dijkstra.size()-1; i++)
	{
		for(int j=i+1; j<dijkstra.size(); j++)
		{
			if(i!=j)
			{
				if(ableToConnect(dijkstra[i]->pos, dijkstra[j]->pos))
				{
					connections.push_back(Line(dijkstra[i]->pos.x, dijkstra[i]->pos.y, dijkstra[j]->pos.x, dijkstra[j]->pos.y));
					dijkstra[i]->neighbors.push_back(dijkstra[j]);
					double d = sqrt(pow(dijkstra[i]->pos.x-dijkstra[j]->pos.x,2.0) + pow(dijkstra[i]->pos.y-dijkstra[j]->pos.y,2.0));
					//dijkstra[i]->dist.push_back(d);
					if(dijkstra[j]->curDist > dijkstra[i]->curDist + d)
					{
						dijkstra[j]->curDist = dijkstra[i]->curDist + d;
						dijkstra[j]->from = dijkstra[i];
					}
				}
			}
		}
		
	}
	printf("dist %f from %d %d\n", dijkstra[dijkstra.size()-1]->curDist, dijkstra[dijkstra.size()-1]->pos.x, dijkstra[dijkstra.size()-1]->pos.y);
	Node* cur = dijkstra[dijkstra.size()-1];
	while(cur->from != NULL)
	{
		path.push_back(Line(cur->pos.x, cur->pos.y, cur->from->pos.x, cur->from->pos.y));
		cur = cur->from;
	}
}
void getPoints()
{
	for(int i=0; i<lines.size(); i++)
	{
		//printf("A%d %d %d %d\n", lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2);
		points.push_back(Point(lines[i].x1, (lines[i].y1 + lines[i].y2)/2.0 ));
	}
}
void addObject(int x, int y){

	if (rectCount == 0) // Big square
		squares.push_back(Square(x, y));

	else if (rectCount == 1) // Medium square;
		squares.push_back(Square(x, y));

	else if (rectCount == 2) // Small square
	{
		squares.push_back(Square(x, y));
		//Display lines
		getLines();
	}
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
		rectCount=5;
		getPoints();
		rectCount=4;
		points.push_back(Point(x, y));
		calcPath();
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
