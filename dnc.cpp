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

typedef pair<int, int> ptPair;

/* Helper function declarations */
static void sortPoints(Points &pts);
static Points solveRecursive(Points &pts);
static void drawSubHulls(const Points &lHull, const Points &rHull);
static ptPair getUpperBridge(const Points &left, const Points &right);
static ptPair getLowerBridge(const Points &left, const Points &right);
static void drawBridgeLine(const my_point &p1, const my_point &p2, int color);
static Points merge(const Points &lHull, const Points &rHull, 
                         ptPair upper, ptPair lower);
static void drawMergedHull(const Points &hull);
static int cross_product(my_point o, my_point a, my_point b);

/* Define constants */
const int WAIT = 10;
#define SUBHULL BLACK
#define UPPER GREEN
#define LOWER ORANGE
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
                assert(!pts.empty());
        }
        sortPoints(pts);
        return solveRecursive(pts);
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
 *      Uses std::sort to sort points in O(nlogn) time.
 ************************/
static void sortPoints(Points &pts)
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
 *      Points &pts:    The set of points whose hull will be calculated.
 * Returns:
 *      A Points object containing the points of the sub-hull.
 * Expects:
 *      pts is sorted by x-coordinate (ties broken by y-coordinate).
 * Notes:
 *      Calls helper functions to merge and viisualize.
 ************************/
static Points solveRecursive(Points &pts)
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
        Points lHull = solveRecursive(lPts);
        Points rHull = solveRecursive(rPts);

        /* Display sub-hulls for incremental visualization */
        drawSubHulls(lHull, rHull);

        /* Merge (conquer) */
        ptPair upper = getUpperBridge(lHull, rHull);
        ptPair lower = getLowerBridge(lHull, rHull);
        Points merged = merge(lHull, rHull, upper, lower);
        
        /* Draw the merged hull */
        en47_wait(WAIT);
        en47_clear();
        drawMergedHull(merged);
        
        return merged;
}

/******** drawSubHulls ********
 *
 * Draws the current left and right sub-hulls.
 *
 * Parameters:
 *      const Points &lHull:    The left sub-hull.
 *      const Points &rHull:    The right sub-hull.
 * Returns:
 *      None.
 * Expects:
 *      None.
 * Notes:
 *      Draws sub-hulls in BLACK.
 ************************/
static void drawSubHulls(const Points &lHull, const Points &rHull)
{
        my_point p;
        my_point next;

        /* Draw Left Hull */
        for (size_t i = 0; i < lHull.size(); i++) {
                p = lHull[i];
                next = lHull[(i + 1) % lHull.size()];
                en47_draw_segment(p.x, p.y, next.x, next.y, SUBHULL);
        }

        /* Draw Right Hull */
        for (size_t i = 0; i < rHull.size(); i++) {
                p = rHull[i];
                next = rHull[(i + 1) % rHull.size()];
                en47_draw_segment(p.x, p.y, next.x, next.y, SUBHULL);
        }

        en47_wait(WAIT);
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
 *      Draws searching process in GREEN.
 ************************/
static ptPair getUpperBridge(const Points &left, const Points &right)
{
        int lSize = left.size();
        int rSize = right.size();
        int i = lSize - 1;
        int j = 0;
        
        bool done = false;
        while (!done) {
                done = true;
                
                /* Initial search line */
                drawBridgeLine(left[i], right[j], UPPER);

                /* Advance left index if not tangent */
                while (lSize > 1 && cross_product(right[j], left[i],
                                     left[(i + 1) % lSize]) >= 0) {
                        drawBridgeLine(left[i], right[j], ERASED);
                        i = (i + 1) % lSize;
                        done = false;
                        drawBridgeLine(left[i], right[j], UPPER);
                }
                /* Advance right index if not tangent */
                while (rSize > 1 && cross_product(left[i], right[j],
                                     right[(j - 1 + rSize) % rSize]) <= 0) {
                        drawBridgeLine(left[i], right[j], ERASED);
                        j = (j - 1 + rSize) % rSize;
                        done = false;
                        drawBridgeLine(left[i], right[j], UPPER);
                }
                
                /* Clean up */
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
        int lSize = left.size();
        int rSize = right.size();        
        int i = lSize - 1;
        int j = 0;
        
        bool done = false;
        while (!done) {
                done = true;
                
                /* Initial search line */
                drawBridgeLine(left[i], right[j], LOWER);
                
                /* Advance left index if not tangent */
                while (lSize > 1 && cross_product(right[j], left[i],
                                     left[(i - 1 + lSize) % lSize]) <= 0) {
                        drawBridgeLine(left[i], right[j], ERASED);
                        i = (i - 1 + lSize) % lSize;
                        done = false;
                        drawBridgeLine(left[i], right[j], LOWER);
                }
                /* Advance right index if not tangent */
                while (rSize > 1 && cross_product(left[i], right[j],
                                     right[(j + 1) % rSize]) >= 0) {
                        drawBridgeLine(left[i], right[j], ERASED);
                        j = (j + 1) % rSize;
                        done = false;
                        drawBridgeLine(left[i], right[j], LOWER);
                }
                
                /* Clean up */
                drawBridgeLine(left[i], right[j], ERASED);
        }
        return {i, j};
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
        
        /* Wait if searching, don't wait if erasing */
        if (color == UPPER || color == LOWER) {
                en47_wait(WAIT);
        }
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
        Points merged;

        /* Traverse right hull (upper -> lower bridge, CW along outer edge) */
        int ind = upper.second;
        merged.push_back(rHull[ind]);
        while (ind != lower.second) {
                ind = (ind + 1) % rHull.size();
                merged.push_back(rHull[ind]);
        }
        
        /* Traverse left hull (lower -> upper bridge, CW along outer edge) */
        ind = lower.first;
        merged.push_back(lHull[ind]);
        while (ind != upper.first) {
                ind = (ind + 1) % lHull.size();
                merged.push_back(lHull[ind]);
        }

        return merged;
}

/******** drawMergedHull ********
 *
 * Draws the complete merged hull in BLACK.
 *
 * Parameters:
 *      const Points &hull:     A merged convex hull.
 * Returns:
 *      None.
 * Expects:
 *      None.
 * Notes:
 *      Draws hulls in BLACK.
 ************************/
static void drawMergedHull(const Points &hull)
{
        my_point p;
        my_point next;
        for (size_t i = 0; i < hull.size(); i++) {
                p = hull[i];
                next = hull[(i + 1) % hull.size()];
                en47_draw_segment(p.x, p.y, next.x, next.y, SUBHULL);
        }
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
