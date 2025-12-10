/*
 * en47_vis_txt.h
 *
 * Course: EN47
 * Last updated: July 27, 2001  REVISED November 1, 2023
 *
 */

#ifndef EN47_VIS_TXT_H 
#define EN47_VIS_TXT_H 

#define ERASE -1
#define DRAW 0
#define HIGHLIGHT 1

#define WHITE 	0
#define BLACK	1
#define RED	2
#define GREEN	3
#define BLUE	4
#define YELLOW	5
#define VIOLET	6
#define ORANGE	7

using namespace std;

struct my_point {
    int x, y;
    int ID;
};

void en47_display();				// open the window
void en47_display(int width, int height);	// open the window and specify size
void en47_display(int minx, int maxx, int miny, int maxy);
						// open the window and specify:
						    // minx, maxx, miny, maxy
void en47_close(); 				// close the window
void en47_clear(); 				// clear the window
void en47_draw_point(int x, int y);		// draw a black point
void en47_draw_point(int x, int y, int col);	// draw a colored point
void en47_draw_segment(int x1, int y1, int x2, int y2);	
						// draw a black line segment
void en47_draw_segment(int x1, int y1, int x2, int y2, int col);
						// draw a colored line segment
void en47_draw_trapezoid(int, int, int, int);
void en47_draw_upper_histogram(int, int, int);
void en47_draw_lower_histogram(int, int, int);
void en47_draw_histogram(int, int, int);
void en47_draw_towers(int[3][21]);
void en47_wait();
void en47_mouse_wait();
void en47_wait(int milliseconds);

#endif
