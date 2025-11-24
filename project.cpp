/*
 *      project.cpp
 *      Justin Paik (jpaik03)
 *      11 December, 2025
 *      Divide & Conquer Convex Hull Algorithm
 *
 *      The main driver for dnc_ch. Uses CGAL for geometry logic. Visual output
 *      will be handled via SVG generation.
 *      TODO: Further description
 */

/* Standard c++ Includes */
#include <iostream>
#include <vector>
#include <algorithm>

/* CGAL Includes */
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>

using namespace std;

/* CGAL Typedefs */
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_2 Point;


int main()
{
        /* Test CGAL */
        cout << "Initializing CGAL...\n";

        vector<Point> points;
        points.push_back(Point(1, 1));
        points.push_back(Point(5, 5));
        
        cout << points.size() << " points created.\n";

        // TODO: Algorithm Implementation

        // TODO: Visualization

        return 0;
}