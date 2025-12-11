/*
 * dnc.cpp
 * Justin Paik (jpaik03)
 * 11 December, 2025
 * Divide & Conquer Convex Hull Algorithm
 *
 * Contains function definitions for the divide and conquer convex hull
 * algorithm.
 */

/* Standard c++ Includes */
#include <iostream>
#include <algorithm>
#include "assert.h"

#include "dnc.h"

using namespace std;

/* Helper function declarations */
static void sortPoints(vector<my_point> &pts);
static vector<my_point> solveRecursive(vector<my_point> &pts);
static pair<int, int> getUpperBridge(const vector<my_point> &left, const vector<my_point> &right);
static pair<int, int> getLowerBridge(const vector<my_point> &left, const vector<my_point> &right);
static int cross_product(my_point o, my_point a, my_point b);

/* Define constants */
const int WAIT = 250;

/******** dnc ********
 *
 * Wrapper function for the Divide & Conquer convex hull algorithm.
 *
 * Parameters:
 *      vector<my_point> &pts:  Address to a vector of my_points.
 * Returns:
 *      vector<my_point>:       A vector containing the points of the convex
 *                              hull.
 * Expects:
 *      pts is not empty.
 * Notes:
 *      Throws a CRE if pts is empty.
 *      Calls helper functions to sort points and build convex hulls
 *        recursively.
 ************************/
vector<my_point> dnc(vector<my_point> &pts)
{
        if (pts.empty()) {
                cerr << "No points given.\n";
                assert(!pts.empty());
        }
        sortPoints(pts);
        return solveRecursive(pts);
}

/******** sortPoints ********
 *
 * Sorts a vector of my_points by ascending x-coordinate, breaking ties by
 * ascending y-coordinate.
 *
 * Parameters:
 *      vector<my_point> &pts:  Address to a vector of my_points.
 * Returns:
 *      None.
 * Expects:
 *      None.
 * Notes:
 *      Uses std::to sort points in O(nlogn) time.
 ************************/
static void sortPoints(vector<my_point> &pts)
{
        sort(pts.begin(), pts.end(), [](const my_point &a, const my_point &b) {
                /* Sort by ascending x-coordinate */
                if (a.x != b.x) {
                return a.x < b.x;
                }
                /* Break ties by sort by ascending y-coordinate */
                return a.y < b.y;
        });
}

/******** solveRecursive ********
 *
 * Recursively builds the convex hull of a set of sorted points by partitioning
 * the set, building sub-hulls, and merging them.
 *
 * Parameters:
 *      vector<my_point> &pts:  Address to a vector of sorted my_points.
 * Returns:
 *      vector<my_point>:       A vector containing the points of the sub-hull.
 * Expects:
 *      pts is sorted by x-coordinate (ties broken by y-coordinate).
 * Notes:
 *      Includes visualization calls.
 ************************/
static vector<my_point> solveRecursive(vector<my_point> &pts)
{
        /* Base (trivial) case: Convex hull of a point is itself */
        if (pts.size() <= 1) {
                return pts;
        }
        
        /* Divide the points into two subsets */
        int mid = pts.size() / 2;
        vector<my_point> lPts(pts.begin(), pts.begin() + mid);
        vector<my_point> rPts(pts.begin() + mid, pts.end());
        
        /* Recurse to find convex hull of subsets */
        vector<my_point> lHull = solveRecursive(lPts);
        vector<my_point> rHull = solveRecursive(rPts);
        
        /* Display sub-hulls for incremental visualization */
        my_point p;
        for(size_t i = 0; i < lHull.size(); i++) {
                p = lHull[i];
                en47_draw_segment(p.x, p.y,
                                lHull[(i + 1) % lHull.size()].x,
                                lHull[(i + 1) % lHull.size()].y,
                                BLACK);
        }
        for(size_t i = 0; i < rHull.size(); i++) {
                p = rHull[i];
                en47_draw_segment(p.x, p.y,
                                rHull[(i + 1) % rHull.size()].x,
                                rHull[(i + 1) % rHull.size()].y,
                                BLACK);
        }
        /* Wait between frames */
        en47_wait(WAIT);

        /* Merge (conquer) */
        pair<int, int> upper = getUpperBridge(lHull, rHull);
        pair<int, int> lower = getLowerBridge(lHull, rHull);

        vector<my_point> mergedHull;

        // Upper Bridge -> Lower Bridge (CW on Left Hull)
        int ind = upper.first;
        mergedHull.push_back(lHull[ind]);
        while (ind != lower.first) {
                ind = (ind + 1) % lHull.size();
                mergedHull.push_back(lHull[ind]);
        }
        
        // Lower Bridge -> Upper Bridge (CW on Right Hull)
        ind = lower.second;
        mergedHull.push_back(rHull[ind]);
        while (ind != upper.second) {
                ind = (ind + 1) % rHull.size();
                mergedHull.push_back(rHull[ind]);
        }
                
        en47_wait(500);

        // Clear & Restore
        en47_clear();
        // Re-draw the merged hull in black to leave it for the next step up
        for(size_t k=0; k<mergedHull.size(); k++)
                en47_draw_segment(mergedHull[k].x, mergedHull[k].y, mergedHull[(k+1)%mergedHull.size()].x, mergedHull[(k+1)%mergedHull.size()].y, BLACK);

        return mergedHull;
}

/******** getUpperBridge ********
 *
 * Finds the upper bridge between two separated convex hulls.
 *
 * Parameters:
 *      const vector<my_point> &left:   The left sub-hull (sorted CW).
 *      const vector<my_point> &right:  The right sub-hull (sorted CW).
 * Returns:
 *      pair<int, int>:  Indices {index in left, index in right} of the bridge.
 * Expects:
 *      Hulls are disjoint and sorted by x-coordinate relative to each other.
 * Notes:
 *      Visualizes the search process using GREEN lines.
 ************************/
static pair<int, int> getUpperBridge(const vector<my_point> &left,
                                     const vector<my_point> &right)
{
        int n1 = left.size();
        int n2 = right.size();
        
        int i = n1 - 1;
        int j = 0;
        
        bool done = false;
        while (!done) {
                done = true;
        
                en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, GREEN);
                en47_wait(100);
        
                while (cross_product(right[j], left[i], left[(i + 1) % n1]) > 0) {
                en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, WHITE);
                i = (i + 1) % n1;
                done = false;
                en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, GREEN);
                en47_wait(100);
                }
        
                while (cross_product(left[i], right[j], right[(j - 1 + n2) % n2]) < 0) {
                en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, WHITE);
                j = (j - 1 + n2) % n2;
                done = false;
                en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, GREEN);
                en47_wait(100);
                }
        
                en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, WHITE);
        }
        return {i, j};
}

/******** getLowerBridge ********
 *
 * Finds the lower bridge between two separated convex hulls.
 *
 * Parameters:
 *      const vector<my_point> &left:   The left sub-hull (sorted CW).
 *      const vector<my_point> &right:  The right sub-hull (sorted CW).
 * Returns:
 *      pair<int, int>:  Indices {index in Left, index in Right} of the bridge.
 * Expects:
 *      Hulls are disjoint and sorted by x-coordinate relative to each other.
 * Notes:
 *      Visualizes the search process using ORANGE lines.
 ************************/
static pair<int, int> getLowerBridge(const vector<my_point> &left,
                                     const vector<my_point> &right)
{
    int n1 = left.size();
    int n2 = right.size();
   
    int i = n1 - 1;
    int j = 0;
   
    bool done = false;
    while (!done) {
        done = true;
       
        en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, ORANGE);
        en47_wait(100);
       
        while (cross_product(right[j], left[i], left[(i - 1 + n1) % n1]) < 0) {
            en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, WHITE);
            i = (i - 1 + n1) % n1;
            done = false;
            en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, ORANGE);
            en47_wait(100);
        }
       
        while (cross_product(left[i], right[j], right[(j + 1) % n2]) > 0) {
            en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, WHITE);
            j = (j + 1) % n2;
            done = false;
            en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, ORANGE);
            en47_wait(100);
        }
       
        en47_draw_segment(left[i].x, left[i].y, right[j].x, right[j].y, WHITE);
    }
    return {i, j};
}

/******** cross_product ********
 *
 * Determine orientation of ordered triplet (o, a, b).
 *
 * Parameters:
 *      my_point o, a, b:       Three points in the plane.
 * Returns:
 *      Value > 0 if Counter-Clockwise (Left turn).
 *      Value < 0 if Clockwise (Right turn).
 *      Value = 0 if Collinear.
 * Expects:
 *      None.
 * Notes:
 *      None
 ************************/
static int cross_product(my_point o, my_point a, my_point b)
{
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}
