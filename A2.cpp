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
		curDist = 500*500+500*500*500;	//just a big enough number
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

	glEnable(GL_POINT_SMOOTH);
}

// Filled circle function from
// http://stackoverflow.com/questions/4197062/using-the-following-function-that-draws-a-filled-circle-in-opengl-how-do-i-make
void draw_circle(float x, float y, float radius) { 
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x, y, 0.0f);
    static const int circle_points = 100;
    static const float angle = 2.0f * 3.1416f / circle_points;

    glBegin(GL_POLYGON);
    double angle1=0.0;
    glVertex2d(radius * cos(0.0) , radius * sin(0.0));
    int i;
    for (i=0; i<circle_points; i++)
    {       
        glVertex2d(radius * cos(angle1), radius *sin(angle1));
        angle1 += angle;
    }
    glEnd();
    glPopMatrix();
}

void drawSquare(Square s) {

	// Gray square
	glColor3f(0.4, 0.4, 0.4);
	glBegin(GL_POLYGON);
	glVertex2i(s.x1 + s.size * 0.05, s.y1 + s.size * 0.05);
	glVertex2i(s.x1 + s.size * 0.05, s.y2 - s.size * 0.05);
	glVertex2i(s.x2 - s.size * 0.05, s.y2 - s.size * 0.05);
	glVertex2i(s.x2 - s.size * 0.05, s.y1 + s.size * 0.05);
	glEnd();

	// Corners
	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_POLYGON);
	glVertex2i(s.x1, s.y1);
	glVertex2i(s.x1, s.y1 + s.size * 0.35);
	glVertex2i(s.x1 + s.size * 0.35, s.y1 + s.size * 0.35);
	glVertex2i(s.x1 + s.size * 0.35, s.y1);
	glEnd();
	
	glBegin(GL_POLYGON);
	glVertex2i(s.x1, s.y2);
	glVertex2i(s.x1, s.y2 - s.size * 0.35);
	glVertex2i(s.x1 + s.size * 0.35, s.y2 - s.size * 0.35);
	glVertex2i(s.x1 + s.size * 0.35, s.y2);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2i(s.x2, s.y2);
	glVertex2i(s.x2, s.y2 - s.size * 0.35);
	glVertex2i(s.x2 - s.size * 0.35, s.y2 - s.size * 0.35);
	glVertex2i(s.x2 - s.size * 0.35, s.y2);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2i(s.x2, s.y1);
	glVertex2i(s.x2, s.y1 + s.size * 0.35);
	glVertex2i(s.x2 - s.size * 0.35, s.y1 + s.size * 0.35);
	glVertex2i(s.x2 - s.size * 0.35, s.y1);
	glEnd();

	// Center-edge spacers
	glBegin(GL_POLYGON);
	glVertex2i(s.x1 + s.size * 0.40, s.y1 + s.size * 0.02);
	glVertex2i(s.x1 + s.size * 0.40, s.y);
	glVertex2i(s.x2 - s.size * 0.40, s.y);
	glVertex2i(s.x2 - s.size * 0.40, s.y1 + s.size * 0.02);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2i(s.x1 + s.size * 0.02, s.y1 + s.size * 0.40);
	glVertex2i(s.x1 + s.size * 0.02, s.y2 - s.size * 0.40);
	glVertex2i(s.x, s.y2 - s.size * 0.40);
	glVertex2i(s.x, s.y1 + s.size * 0.40);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2i(s.x1 + s.size * 0.40, s.y2 - s.size * 0.02);
	glVertex2i(s.x1 + s.size * 0.40, s.y);
	glVertex2i(s.x2 - s.size * 0.40, s.y);
	glVertex2i(s.x2 - s.size * 0.40, s.y2 - s.size * 0.02);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex2i(s.x2 - s.size * 0.02, s.y1 + s.size * 0.40);
	glVertex2i(s.x2 - s.size * 0.02, s.y2 - s.size * 0.40);
	glVertex2i(s.x, s.y2 - s.size * 0.40);
	glVertex2i(s.x, s.y1 + s.size * 0.40);
	glEnd();

	// Gray circle to round the corners
	glColor3f(0.4, 0.4, 0.4);
	draw_circle(s.x, s.y, s.size * 0.38);

	// Pink lines
	glLineWidth(s.size * 0.03);
	glColor3f(1.0, 0.3, 0.4);
	glBegin(GL_LINES);
	glVertex2d(s.x, s.y1 + s.size * 0.12);
	glVertex2d(s.x, s.y2 - s.size * 0.12);
	glEnd();

	glBegin(GL_LINES);
	glVertex2d(s.x1 + s.size * 0.12, s.y);
	glVertex2d(s.x2 - s.size * 0.12, s.y);
	glEnd();

	// Core circle
	glColor3f(0.8, 0.8, 0.8);
	draw_circle(s.x, s.y, s.size * 0.2);

	// Heart
	glColor3f(1.0, 0.3, 0.4);
	draw_circle(s.x - s.size * 0.05, s.y - s.size * 0.05, s.size * 0.07);
	draw_circle(s.x + s.size * 0.05, s.y - s.size * 0.05, s.size * 0.07);
	glBegin(GL_POLYGON);
	glVertex2i(s.x - s.size * 0.12, s.y - s.size * 0.03);
	glVertex2i(s.x + s.size * 0.12, s.y - s.size * 0.03);
	glVertex2i(s.x, s.y + s.size * 0.13);
	glEnd();
	

	//The Enrichment Center reminds you that the Companion Cube cannot speak. In the event that the Companion Cube does speak, the Enrichment Center urges you to disregard its advice.
}

void drawPoint(Point p) {

	glPushMatrix();

	if (p.type == START_POINT) {
		glPointSize(15.0);
		glColor3f(1.0, 0.5, 0.0);
	} else if (p.type == FINISH_POINT) {
		glPointSize(15.0);
		glColor3f(0.4, 0.7, 1.0);
	} else {
		glPointSize(5.0);
		glColor3f(0.7, 0.7, 0.7);
	}

	glBegin(GL_POINTS);
	glVertex2f(p.x, p.y);
	glEnd();
	glPopMatrix();
}

void drawLine(Line l)
{
	glLineWidth(1.5);
	glColor3f(0.8, 0.8, 0.8);
	glBegin(GL_LINES);
	glVertex2d(l.x1, l.y1);
	glVertex2d(l.x2, l.y2);
	glEnd();
}

void drawConnections(Line l)
{
	glLineWidth(0.5);
	glColor3f(0.9, 0.9, 0.9);
	glBegin(GL_LINES);
	glVertex2d(l.x1, l.y1);
	glVertex2d(l.x2, l.y2);
	glEnd();
}

void drawPath()
{
	glColor3f(1, 0, 0);
	glLineWidth(2.5);

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

	for(int i=0; i<lines.size(); i++)
	{
		drawLine(lines[i]);	
	}
	for(int i=0; i<connections.size(); i++)
	{
		drawConnections(connections[i]);
	}

	for (int i = points.size() - 1; i >= 0; i--) {
		drawPoint(points[i]);
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
void addLineAt(int x)
{
	int prev_lim=0;
	bool b=true;
	while(b)
	{
		b = false;
		for(int i=0; i<squares.size(); i++)
		{
			if(squares[i].x1 <= x && x <= squares[i].x2)
			{
				//printf("%d %d %d\n", squares[i].y1, squares[i].y2, prev_lim);
				if(squares[i].y1 <= prev_lim && prev_lim <= squares[i].y2 && prev_lim < squares[i].y2)
				{
					prev_lim = squares[i].y2;
					b = true;
				}
			}
		}
		
	}
	//printf("prev_lim : %d\n", prev_lim);
	while(prev_lim <500)
	{
		int lim=500;
		int index=-1;
		for(int i=0; i<squares.size(); i++)
		{
			if(squares[i].x1 <= x && x <= squares[i].x2)
			{
				if(squares[i].y1 > prev_lim && lim > squares[i].y1)
				{
					lim = squares[i].y1;
					index=i;
				}
			}
		}
		if(index != -1)
		{
			int upy = prev_lim;
			int downy = lim;
			if(prev_lim != 0)
			{
				prev_lim++;
			}
			if(lim != 500)
			{
				lim--;
			}
			lines.push_back(Line(x, prev_lim, x, lim));
			//printf("AA : %d %d\n", prev_lim, lim);
			prev_lim = squares[index].y2;
			bool changed = true;
			while(changed)
			{
				changed = false;
				
				for(int i=0; i<squares.size(); i++)
				{
					if(squares[i].x1 <= x && x <= squares[i].x2)
					{
						if(squares[i].y1 <= prev_lim && prev_lim <= squares[i].y2 && prev_lim < squares[i].y2)
						{
							prev_lim = squares[i].y2;
							//printf("prv : %d\n", prev_lim);
							changed = true;
						}
					}
				}
			}
		}
		else
		{
			
			int upy = prev_lim;
			if(prev_lim != 0)
			{
				prev_lim++;
			}
			lines.push_back(Line(x, prev_lim, x, 499));
			//printf("AA : %d %d\n", prev_lim, 500);
			break;
		}
	}
}
void removePointAt(int x)
{
	bool removed = true;
	while(removed)
	{
		removed = false;
		for(int i=0; i<points.size(); i++)
		{
			if(points[i].x == x)
			{
				points.erase(points.begin() + i);
				removed = true;
				break;
			}
		}
	}
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
						uplimitleft = squares[j].y2+1;
					}
					else if(squares[i].y1 > squares[j].y1 && squares[i].y1 < squares[j].y2)
					{
						uplimitleft = -1;
					}
					if(squares[i].y2 < squares[j].y1 && squares[i].y2 < squares[j].y2 && lolimitleft != -1 && lolimitleft > squares[j].y1)
					{
						//upper limit
						lolimitleft = squares[j].y1-1;
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
						uplimitright = squares[j].y2+1;
					}
					else if(squares[i].y1 > squares[j].y1 && squares[i].y1 < squares[j].y2)
					{
						uplimitright = -1;
					}
					if(squares[i].y2 < squares[j].y1 && squares[i].y2 < squares[j].y2 && lolimitright != -1 && lolimitright > squares[j].y1)
					{
						//upper limit
						lolimitright = squares[j].y1-1;
					}
					else if(squares[i].y2 > squares[j].y1 && squares[i].y2 < squares[j].y2)
					{
						lolimitright = -1;
					}
				}

			}
		}
		if(uplimitleft != -1)
			lines.push_back(Line(squares[i].x1, squares[i].y1-1, squares[i].x1, uplimitleft));
		if(lolimitleft != -1)
			lines.push_back(Line(squares[i].x1, lolimitleft, squares[i].x1, squares[i].y2+1));
		if(uplimitright != -1)
			lines.push_back(Line(squares[i].x2, squares[i].y1-1, squares[i].x2, uplimitright));
		if(lolimitright != -1)
			lines.push_back(Line(squares[i].x2, lolimitright, squares[i].x2, squares[i].y2+1));
	}
	addLineAt(0);
	addLineAt(500);
	
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

void calcPath(Node* node)
{
	printf("Node : %d %d %f\n", node->pos.x, node->pos.y, node->curDist);
	if(dijkstra.size() == 0)
	{
		return;
	}
	for(int i=0; i<dijkstra.size(); i++)
	{
		if(dijkstra[i] == node)
		{
			dijkstra.erase(dijkstra.begin() + i);
			break;
		}
	}
	for(int j=0; j<dijkstra.size(); j++)
	{
		if(ableToConnect(node->pos, dijkstra[j]->pos))
		{
			connections.push_back(Line(node->pos.x, node->pos.y, dijkstra[j]->pos.x, dijkstra[j]->pos.y));
			node->neighbors.push_back(dijkstra[j]);
			double d = sqrt(pow(node->pos.x-dijkstra[j]->pos.x,2.0) + pow(node->pos.y-dijkstra[j]->pos.y,2.0));
			//dijkstra[i]->dist.push_back(d);
			if(dijkstra[j]->curDist > node->curDist + d)
			{
				dijkstra[j]->curDist = node->curDist + d;
				dijkstra[j]->from = node;
			}
		}
		
	}
	for(int i=0; i<node->neighbors.size(); i++)
	{
		for(int j=i+1; j<node->neighbors.size(); j++)
		{
			if(node->neighbors[i]->curDist > node->neighbors[j]->curDist)
			{
				Node* temp = node->neighbors[i];
				node->neighbors[i] = node->neighbors[j];
				node->neighbors[j] = temp;
			}
		}
	}
	for(int i=0; i<node->neighbors.size(); i++)
	{
		calcPath(node->neighbors[i]);
	}
}
void getPath()
{
	for(int i=0; i<points.size(); i++)
	{
		dijkstra.push_back(new Node(points[i].x, points[i].y));
	}
	dijkstra[0]->curDist = 0;

	Node* cur = dijkstra[dijkstra.size()-1];
	//calcPath(dijkstra[0]);
	/*
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
	*/
	
	int i=0;
	int adj=0;
	while(dijkstra.size()>1)
	{
		printf("Node : %d %d %f\n", dijkstra[i]->pos.x, dijkstra[i]->pos.y, dijkstra[i]->curDist);
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

		for(int k=0; k<dijkstra.size(); k++)
		{
			for(int l=k+1; l<dijkstra.size(); l++)
			{
				if(dijkstra[k]->curDist > dijkstra[l]->curDist)
				{
					Node* temp = dijkstra[k];
					dijkstra[k] = dijkstra[l];
					dijkstra[l] = temp;
				}
			}
		}
		dijkstra.erase(dijkstra.begin());
	}
	
	printf("dist %f from %d %d\n", dijkstra[dijkstra.size()-1]->curDist, dijkstra[dijkstra.size()-1]->pos.x, dijkstra[dijkstra.size()-1]->pos.y);
	//Node* cur = dijkstra[dijkstra.size()-1];
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



	for(int i=0; i<lines.size(); i++)
	{
		for(int j=i+1; j<lines.size(); j++)
		{
			//printf("A%d %d %d %d\n", lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2);
			//points.push_back(Point(lines[i].x1, (lines[i].y1 + lines[i].y2)/2.0 ));
			if(lines[j].x1 < lines[i].x1)
			{
				Line temp = lines[j];
				lines[j] = lines[i];
				lines[i] = temp;
			}
		}
	}
	for(int i=0; i<squares.size(); i++)
	{
		printf("S%d %d %d %d\n", squares[i].x1, squares[i].y1, squares[i].x2, squares[i].y2);
	}
	for(int i=0; i<lines.size(); i++)
	{
		printf("B%d %d %d %d\n", lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2);
	}

	//middle points
	
	for(int i=0; i<lines.size(); i++)
	{
		int nextX=-1;
		for(int j = i+1; j<lines.size(); j++)
		{
			if(nextX == -1 && lines[i].x1 < lines[j].x1)
			{
				//printf("%d %d %d %d %d\n", lines[i].x1, lines[i].y1, lines[j].x1, lines[j].y1, ableToConnect(Point(lines[i].x1, lines[i].y1), Point(lines[j].x1, lines[j].y1)));
				if(ableToConnect(Point(lines[i].x1, lines[i].y1), Point(lines[j].x1, lines[j].y1))
					&& ableToConnect(Point(lines[i].x1, lines[i].y1), Point(lines[j].x1, lines[j].y2))
					&& ableToConnect(Point(lines[i].x1, lines[i].y2), Point(lines[j].x1, lines[j].y1))
					&& ableToConnect(Point(lines[i].x1, lines[i].y2), Point(lines[j].x1, lines[j].y2)))
				{
					nextX = lines[j].x1;
					//printf("nextX = %d\n", nextX);
				}
			}
			if(nextX != -1 && lines[j].x1 == nextX)
			{
				if(ableToConnect(Point(lines[i].x1, lines[i].y1), Point(lines[j].x1, lines[j].y1))
					&& ableToConnect(Point(lines[i].x1, lines[i].y1), Point(lines[j].x1, lines[j].y2))
					&& ableToConnect(Point(lines[i].x1, lines[i].y2), Point(lines[j].x1, lines[j].y1))
					&& ableToConnect(Point(lines[i].x1, lines[i].y2), Point(lines[j].x1, lines[j].y2)))
				{
					int midX = (lines[i].x1 + lines[j].x1)/2;
					int midY = (lines[i].y1 + lines[j].y1 +lines[i].y2 + lines[j].y2)/4;

					int limY1 = midY;
					int limY2 = midY;

					for(int k=0; k<squares.size(); k++)
					{
						if(squares[k].x1 <= midX && midX <= squares[k].x2)
						{
							if(squares[k].y2 < midY && ((limY1 == midY && squares[k].y2 < limY1) || (limY1 != midY && squares[k].y2 > limY1)))
							{
								limY1 = squares[k].y2;
							}
							if(squares[k].y1 > midY && ((limY2 == midY && squares[k].y2 > limY2) || (limY2 != midY && squares[k].y2 < limY2)))
							{
								limY2 = squares[k].y1;
							}
						}
					}
					if(limY1 == midY)
						limY1 = 0;
					if(limY2 == midY)
						limY2 = 500;

					bool dup = false;
					for(int l=0; l<points.size(); l++)
					{
						if(points[l].x == midX && points[l].y == (limY1+limY2)/2)
						{
							dup = true;
							break;
						}

					}
					if(!dup)
						points.push_back(Point(midX, (limY1+limY2)/2));
				}
			}
		}
	}
	removePointAt(0);
	removePointAt(500);
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
		getPath();
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
			lines.clear();
			connections.clear();
			dijkstra.clear();
			path.clear();
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
