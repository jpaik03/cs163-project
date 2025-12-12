/*
 *      dnc.cpp
 *      Justin Paik (jpaik03)
 *      11 December, 2025
 *      Divide & Conquer Convex Hull Algorithm
 *
 *      Contains function definitions for the divide and conquer convex hull
 *      algorithm.
 */

/* Standard c++ Includes */
#include <iostream>
#include <algorithm>
#include "assert.h"

#include "dnc.h"

using namespace std;

typedef pair<int, int> ptPair;

/* Helper function declarations */
static void sortPoints(Points &pts);
static Points solveRecursive(Points &pts, const Points &allPts,
                             const vector<Points> &prevHulls);
static ptPair getUpperBridge(const Points &left, const Points &right);
static ptPair getLowerBridge(const Points &left, const Points &right);
static Points merge(const Points &lHull, const Points &rHull, 
                    ptPair upper, ptPair lower);
static void drawPoints(const Points &pts);
static void drawAllHulls(const vector<Points> &hulls);
static void drawSubHulls(const Points &lHull, const Points &rHull,
                         const Points &allPts, const vector<Points> &prevHulls);
static void drawMergedHull(const Points &hull, const Points &allPts,
                           const vector<Points> &prevHulls);
static void drawHull(const Points &hull, int color);
static void drawBridgeLine(const my_point &p1, const my_point &p2, int color);
static int findRightmost(const Points &hull, bool higherY);
static int findLeftmost(const Points &hull, bool higherY);
static int crossProduct(const my_point &o, const my_point &a,
                        const my_point &b);

/* Define constants and macros */
const int WAIT = 10;    /* Length of frames in milliseconds */
#define STORED GREEN
#define SUBHULL BLACK
#define SEARCH ORANGE
#define ERASED WHITE

/******** dnc ********
 *
 * Wrapper function for the Divide & Conquer convex hull algorithm.
 *
 * Parameters:
 *      Points &pts:    The set of points whose hull will be calculated.
 * Returns:
 *      A Points object containing the points of the convex hull.
 * Expects:
 *      pts is not empty.
 * Notes:
 *      Throws a CRE if pts is empty.
 *      Calls helper functions to sort points and build convex hulls
 *        recursively.
 ************************/
Points dnc(Points &pts)
{
        if (pts.empty()) {
                cerr << "No points given.\n";
                exit(EXIT_FAILURE);
        }
        sortPoints(pts);
        vector<Points> emptyHulls;
        return solveRecursive(pts, pts, emptyHulls);
}

/******** sortPoints ********
 *
 * Sorts a Points object by ascending x-coordinate, breaking ties by ascending
 * y-coordinate.
 *
 * Parameters:
 *      Points &pts:    The set of points to sort.
 * Returns:
 *      None.
 * Expects:
 *      None.
 * Notes:
 *      Uses std::sort to sort points in O(n log n) time.
 ************************/
static void sortPoints(Points &pts)
{
        sort(pts.begin(), pts.end(), [](const my_point &a, const my_point &b) {
                /* Sort by ascending x-coordinate */
                if (a.x != b.x) {
                return a.x < b.x;
                }
                /* Break ties by sorting by ascending y-coordinate */
                return a.y < b.y;
        });
}

/******** solveRecursive ********
 *
 * Recursively builds the convex hull of a set of sorted points by partitioning
 * the set, building sub-hulls, and merging them.
 *
 * Parameters:
 *      Points &pts:                            The set of points whose hull
 *                                              will be calculated.
 *      const Points &allPts:                   The original set of points.
 *      const vector<Points> &prevHulls:        A list of all other hulls.
 * Returns:
 *      A Points object containing the points of the sub-hull.
 * Expects:
 *      pts is sorted by x-coordinate (ties broken by y-coordinate).
 * Notes:
 *      Calls helper functions to merge and visualize.
 ************************/
static Points solveRecursive(Points &pts, const Points &allPts,
                             const vector<Points> &prevHulls)
{
        /* Base (trivial) case: Convex hull of a point is itself */
        if (pts.size() <= 1) {
                return pts;
        }
        
        /* Divide the points into two subsets */
        int mid = pts.size() / 2;
        Points lPts(pts.begin(), pts.begin() + mid);
        Points rPts(pts.begin() + mid, pts.end());
        
        /* Recurse to find convex hull of subsets */
        Points lHull = solveRecursive(lPts, allPts, prevHulls);
        
        /* Build hull list including left sub-hull for right recursion */
        vector<Points> withLeftHull = prevHulls;
        withLeftHull.push_back(lHull);
        
        Points rHull = solveRecursive(rPts, allPts, withLeftHull);

        /* Build new hull list including both current sub-hulls */
        vector<Points> newHulls = withLeftHull;
        newHulls.push_back(rHull);

        /* Display sub-hulls for incremental visualization */
        drawSubHulls(lHull, rHull, allPts, newHulls);

        /* Merge (conquer) */
        ptPair upper = getUpperBridge(lHull, rHull);
        ptPair lower = getLowerBridge(lHull, rHull);
        Points merged = merge(lHull, rHull, upper, lower);
        
        /* Draw the merged hull */
        en47_wait(WAIT);
        en47_clear();
        drawMergedHull(merged, allPts, newHulls);
        
        return merged;
}

/******** getUpperBridge ********
 *
 * Finds the upper bridge between two separated convex hulls.
 *
 * Parameters:
 *      const Points &left:     The left sub-hull (CW order).
 *      const Points &right:    The right sub-hull (CW order).
 * Returns:
 *      Indices {left, right} of the bridge.
 * Expects:
 *      left and right are disjoint hulls.
 *      left and right are sorted by x-coordinate relative to each other.
 * Notes:
 *      Draws searching process in ORANGE.
 ************************/
static ptPair getUpperBridge(const Points &left, const Points &right)
{
        assert(!left.empty());
        assert(!right.empty());

        int lSize = left.size();
        int rSize = right.size();
        
        /* Find starting points */
        int i = findRightmost(left, true);
        int j = findLeftmost(right, true);
        
        bool done = false;
        while (!done) {
                done = true;
                
                drawBridgeLine(left[i], right[j], SEARCH);
                
                /* Check if we need to move counterclockwise on left hull */
                while (crossProduct(right[j], left[i], 
                                    left[(i - 1 + lSize) % lSize]) > 0) {
                        drawBridgeLine(left[i], right[j], ERASED);
                        i = (i - 1 + lSize) % lSize;
                        done = false;
                        drawBridgeLine(left[i], right[j], SEARCH);
                }
                
                /* Check if we need to move counterclockwise on right hull */
                while (crossProduct(left[i], right[j], 
                                    right[(j + 1) % rSize]) < 0) {
                        drawBridgeLine(left[i], right[j], ERASED);
                        j = (j + 1) % rSize;
                        done = false;
                        drawBridgeLine(left[i], right[j], SEARCH);
                }
                
                drawBridgeLine(left[i], right[j], ERASED);
        }
        
        return {i, j};
}

/******** getLowerBridge ********
 *
 * Finds the lower bridge between two separated convex hulls.
 *
 * Parameters:
 *      const Points &left:     The left sub-hull (sorted CW).
 *      const Points &right:    The right sub-hull (sorted CW).
 * Returns:
 *      Indices {left, right} of the bridge.
 * Expects:
 *      left and right are disjoint hulls.
 *      left and right are sorted by x-coordinate relative to each other.
 * Notes:
 *      Draws searching process in ORANGE.
 ************************/
static ptPair getLowerBridge(const Points &left, const Points &right)
{
        assert(!left.empty());
        assert(!right.empty());

        int lSize = left.size();
        int rSize = right.size();
        
        /* Find starting points */
        int i = findRightmost(left, false);
        int j = findLeftmost(right, false);
        
        bool done = false;
        while (!done) {
                done = true;
                
                drawBridgeLine(left[i], right[j], SEARCH);
                
                /* Check if we need to move clockwise on left hull */
                while (crossProduct(right[j], left[i], 
                                    left[(i + 1) % lSize]) < 0) {
                        drawBridgeLine(left[i], right[j], ERASED);
                        i = (i + 1) % lSize;
                        done = false;
                        drawBridgeLine(left[i], right[j], SEARCH);
                }
                
                /* Check if we need to move clockwise on right hull */
                while (crossProduct(left[i], right[j], 
                                    right[(j - 1 + rSize) % rSize]) > 0) {
                        drawBridgeLine(left[i], right[j], ERASED);
                        j = (j - 1 + rSize) % rSize;
                        done = false;
                        drawBridgeLine(left[i], right[j], SEARCH);
                }
                
                drawBridgeLine(left[i], right[j], ERASED);
        }
        
        return {i, j};
}

/******** merge ********
 *
 * Constructs the single merged hull by traversing the sub-hulls clockwise
 * between the bridge points.
 *
 * Parameters:
 *      const Points &lHull:    The left sub-hull (sorted CW).
 *      const Points &rHull:    The right sub-hull (sorted CW).
 *      ptPair upper:           Indices of the upper bridge {l, r}.
 *      ptPair lower:           Indices of the lower bridge {l, r}.
 * Returns:
 *      A Points representing the merged convex hull.
 * Expects:
 *      None.
 * Notes:
 *      None.
 ************************/
static Points merge(const Points &lHull, const Points &rHull,
                    ptPair upper, ptPair lower)
{
        assert(upper.first < (int) lHull.size() &&
               lower.first < (int) lHull.size());
        assert(upper.second < (int) rHull.size() &&
               lower.second < (int) rHull.size());

        Points merged;

        /* Start at upper bridge on right hull */
        int ind = upper.second;
        
        /* Traverse right hull (upper -> lower bridge, CW) */
        while (true) {
                merged.push_back(rHull[ind]);
                if (ind == lower.second) break;
                ind = (ind + 1) % rHull.size();
        }
        
        /* Continue from lower bridge on left hull */
        ind = lower.first;
        
        /* Traverse left hull (lower -> upper bridge, CW) */
        while (true) {
                merged.push_back(lHull[ind]);
                if (ind == upper.first) break;
                ind = (ind + 1) % lHull.size();
        }

        return merged;
}

/******** drawPoints ********
 *
 * Draws all points in the point set.
 *
 * Parameters:
 *      const Points &pts:      The set of points to draw.
 * Returns:
 *      None.
 * Expects:
 *      None.
 * Notes:
 *      Draws points in BLACK.
 ************************/
static void drawPoints(const Points &pts)
{
        for (size_t i = 0; i < pts.size(); i++) {
                en47_draw_point(pts[i].x, pts[i].y, BLACK);
        }
}

/******** drawAllHulls ********
 *
 * Draws all previous sub-hulls.
 *
 * Parameters:
 *      const vector<Points> &hulls:    Vector of all previous hulls.
 * Returns:
 *      None.
 * Expects:
 *      None.
 * Notes:
 *      Draws all hulls in GREEN.
 ************************/
static void drawAllHulls(const vector<Points> &hulls)
{
        for (size_t i = 0; i < hulls.size(); i++) {
                drawHull(hulls[i], STORED);
        }
}

/******** drawSubHulls ********
 *
 * Draws the current left and right sub-hulls.
 *
 * Parameters:
 *      const Points &lHull:                    The left sub-hull.
 *      const Points &rHull:                    The right sub-hull.
 *      const Points &allPts:                   The original set of points.
 *      const vector<Points> &prevHulls:        A list of all other hulls.
 * Returns:
 *      None.
 * Expects:
 *      None.
 * Notes:
 *      Draws current sub-hulls in BLACK and previous ones in GREEN.
 ************************/
static void drawSubHulls(const Points &lHull, const Points &rHull,
                         const Points &allPts, const vector<Points> &prevHulls)
{
        drawPoints(allPts);
        drawAllHulls(prevHulls);
        drawHull(lHull, SUBHULL);
        drawHull(rHull, SUBHULL);
        en47_wait(WAIT);
}

/******** drawMergedHull ********
 *
 * Draws the complete merged hull in BLACK.
 *
 * Parameters:
 *      const Points &hull:                     A merged convex hull.
 *      const Points &allPts:                   The original set of points.
 *      const vector<Points> &prevHulls:        A list of all other hulls.
 * Returns:
 *      None.
 * Expects:
 *      None.
 * Notes:
 *      Draws hulls in BLACK.
 ************************/
static void drawMergedHull(const Points &hull, const Points &allPts,
                           const vector<Points> &prevHulls)
{
        drawPoints(allPts);
        drawAllHulls(prevHulls);
        drawHull(hull, SUBHULL);
}

/******** drawHull ********
 *
 * Draws a single hull with the specified color.
 *
 * Parameters:
 *      const Points &hull:     The hull to draw.
 *      int color:              The color to use.
 * Returns:
 *      None.
 * Expects:
 *      None.
 * Notes:
 *      None.
 ************************/
static void drawHull(const Points &hull, int color)
{
        for (size_t i = 0; i < hull.size(); i++) {
                const my_point &p = hull[i];
                const my_point &next = hull[(i + 1) % hull.size()];
                en47_draw_segment(p.x, p.y, next.x, next.y, color);
        }
}

/******** drawBridgeLine ********
 *
 * Helper function to draw line.
 *
 * Parameters:
 *      const my_point &p1:     The first vertex.
 *      const my_point &p2:     The second vertex.
 *      int color:              The color of the line.
 * Returns:
 *      None.
 * Expects:
 *      None.
 * Notes:
 *      None.
 ************************/
static void drawBridgeLine(const my_point &p1, const my_point &p2, int color)
{
        en47_draw_segment(p1.x, p1.y, p2.x, p2.y, color);
        
        /* Wait if searching, do not wait if erasing */
        if (color == SEARCH) {
                en47_wait(WAIT);
        }
}

/******** findRightmost ********
 *
 * Finds the rightmost point in a hull (for upper bridges, ties go to higher y).
 *
 * Parameters:
 *      const Points &hull:     The hull to search.
 *      bool higherY:           If true, tie-break with higher y (upper bridge).
 *                              If false, tie-break with lower y (lower bridge).
 * Returns:
 *      Index of the rightmost point.
 * Expects:
 *      hull is not empty.
 * Notes:
 *      None.
 ************************/
static int findRightmost(const Points &hull, bool higherY)
{
        int ind = 0;
        for (int k = 1; k < (int)hull.size(); k++) {
                if (hull[k].x > hull[ind].x) {
                        ind = k;
                } else if (hull[k].x == hull[ind].x) {
                        if (higherY && hull[k].y > hull[ind].y) {
                                ind = k;
                        } else if (!higherY && hull[k].y < hull[ind].y) {
                                ind = k;
                        }
                }
        }
        return ind;
}

/******** findLeftmost ********
 *
 * Finds the leftmost point in a hull (for upper bridges, ties go to higher y).
 *
 * Parameters:
 *      const Points &hull:     The hull to search.
 *      bool higherY:           If true, tie-break with higher y (upper bridge).
 *                              If false, tie-break with lower y (lower bridge).
 * Returns:
 *      Index of the leftmost point.
 * Expects:
 *      hull is not empty.
 * Notes:
 *      None.
 ************************/
static int findLeftmost(const Points &hull, bool higherY)
{
        int ind = 0;
        for (int k = 1; k < (int)hull.size(); k++) {
                if (hull[k].x < hull[ind].x) {
                        ind = k;
                } else if (hull[k].x == hull[ind].x) {
                        if (higherY && hull[k].y > hull[ind].y) {
                                ind = k;
                        } else if (!higherY && hull[k].y < hull[ind].y) {
                                ind = k;
                        }
                }
        }
        return ind;
}

/******** crossProduct ********
 *
 * Determines orientation of an ordered triplet (o, a, b).
 *
 * Parameters:
 *      const my_point &o:      Origin point.
 *      const my_point &a:      First point.
 *      const my_point &b:      Second point.
 * Returns:
 *      > 0 if counterclockwise (left turn), < 0 if clockwise (right turn), or
 *      0 if collinear.
 * Expects:
 *      None.
 * Notes:
 *      None.
 ************************/
static int crossProduct(const my_point &o, const my_point &a,
                        const my_point &b)
{
        return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}
