/*
 * en47_vis.cpp
 *
 * Course: EN47
 * Last updated: July 11, 2001
 *
 * Visual functions for use with polygons.cpp.
 */

#include <limits.h>
#include <iostream>
#include <LEDA/graphics/window.h>
#include <LEDA/geo/polygon.h>
#include <LEDA/core/list.h>
#include "en47_vis_txt.h"

using namespace leda;
using namespace std;

int max(int a, int b) {
    if (a > b) return a;
    return b;
}

window *W;

void en47_display() { 
    W = new window(400, 400, "EN47");
    W->init(0, 100, 0);
    W->display(); 
}

void en47_display(int width, int height) {
    W = new window(width * 4, height * 4, "EN47"); 
    W->init(0, width, 0);
    W->display(); 
}

void en47_close() { W->close(); }
void en47_clear() { W->clear(); }

void en47_draw_trapezoid(int x1, int y1, int x2, int y2) {

    list<point> point_list;
    point p;

    if (x1 < x2) {
	p = point(x1, 0);
	point_list.push(p);
	p = point(x2, 0);
	point_list.push(p);
	p = point(x2, y2);
	point_list.push(p);
	p = point(x1, y1);
	point_list.push(p);

	W->draw_filled_polygon(polygon(point_list), red);
	W->set_line_width(4);
	W->draw_segment(point(x1, y1), point(x2, y2), black);
	
    } else {
	p = point(x2, 0);
	point_list.push(p);
	p = point(x1, 0);
	point_list.push(p);
	p = point(x1, y1);
	point_list.push(p);
	p = point(x2, y2);
	point_list.push(p);

	W->draw_filled_polygon(polygon(point_list), white);
	W->set_line_width(4);
	W->draw_segment(point(x1, y1), point(x2, y2), black);
    }
}

void en47_draw_upper_histogram(int x, int y, int type) {

    list<point> point_list;
    point p;

    p = point(5*x+1, 50);
    point_list.push(p);
    p = point(5*x+4, 50);
    point_list.push(p);
    p = point(5*x+4, (y+100)/2 );
    point_list.push(p);
    p = point(5*x+1, (y+100)/2 );
    point_list.push(p);

    if (type == ERASE)
	W->draw_filled_polygon(polygon(point_list), white);
    else
	W->draw_filled_polygon(polygon(point_list), blue + type);
}

void en47_draw_lower_histogram(int x, int y, int type) {

    list<point> point_list;
    point p;

    p = point(5*x+1, 0);
    point_list.push(p);
    p = point(5*x+4, 0);
    point_list.push(p);
    p = point(5*x+4, y/2);
    point_list.push(p);
    p = point(5*x+1, y/2);
    point_list.push(p);

    if (type == ERASE)
	W->draw_filled_polygon(polygon(point_list), white);
    else
	W->draw_filled_polygon(polygon(point_list), blue + type);
}

void en47_draw_histogram(int x, int y, int type) {

    list<point> point_list;
    point p;

    p = point(5*x+1, 0);
    point_list.push(p);
    p = point(5*x+4, 0);
    point_list.push(p);
    p = point(5*x+4, y);
    point_list.push(p);
    p = point(5*x+1, y);
    point_list.push(p);

    if (type == ERASE)
	W->draw_filled_polygon(polygon(point_list), white);
    else
	W->draw_filled_polygon(polygon(point_list), blue + type);
}

void en47_draw_towers(int poles[3][21]) {

    int pole_width = 10,			// the width of the poles
        big_disk,				// the number of the largest disk
        pole_loc[3],				// the x-coordinates of the poles
	big_rad,				// the radius of the largest disk
	small_rad = pole_width,			// the radius of the smallest disk
	disk_height;				// the height of the disks

    // find the largest disk
    big_disk = max(poles[0][0], max(poles[1][0], poles[2][0]));

    // find the location of the poles
    pole_loc[0] = int(W->xmax() / 6);
    pole_loc[1] = int(W->xmax() / 2);
    pole_loc[2] = int(W->xmax() * 5 / 6);

    // draw the poles
    W->clear();
    W->set_line_width(pole_width);
    W->set_color(black);
    W->draw_segment(pole_loc[0], 0, pole_loc[0], W->ymax() * 4 / 5);
    W->draw_segment(pole_loc[1], 0, pole_loc[1], W->ymax() * 4 / 5);
    W->draw_segment(pole_loc[2], 0, pole_loc[2], W->ymax() * 4 / 5);
    W->draw_ctext(pole_loc[0], W->ymax() * 4 / 5 + 5, "0");
    W->draw_ctext(pole_loc[1], W->ymax() * 4 / 5 + 5, "1");
    W->draw_ctext(pole_loc[2], W->ymax() * 4 / 5 + 5, "2");
    for (int i = 0; i < 3; i++)
	W->draw_circle(pole_loc[i], W->ymax() * 4 / 5 + 5, 5);
   
    // find the radius of the largest disk
    big_rad = int(W->xmax() * 9 / 60);		// i.e. (width / 6) * 0.9

    // find the height of the disks
    disk_height = int(float(W->ymax()) * 4 / 5 / (big_disk + 1)) + 9; 

    // draw all the disks
    int j,
	curr_rad,				// radius of current disk
        curr_height;				// height of current disk

    W->set_line_width(disk_height);

    for (int i = 0; i < 3; i++) {
	j = 0;

	while (poles[i][j] != 0) {
            curr_rad = int(float(big_rad - small_rad) * (poles[i][j] - 1) / (big_disk - 1) + small_rad);
	    curr_height = (j + 1) * (disk_height - 9);

	    W->set_color((poles[i][j] % 9) + 2);
	    W->draw_segment(pole_loc[i], curr_height, pole_loc[i] + curr_rad, curr_height);
	    W->draw_segment(pole_loc[i], curr_height, pole_loc[i] - curr_rad, curr_height);
	    j++;
	}
    }
}

void en47_wait() { 
     sleep (2); 
}

void en47_mouse_wait() { 
     W->read_mouse(); 
}
