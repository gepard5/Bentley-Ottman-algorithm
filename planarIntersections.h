#include <stdlib.h>
#include <iostream>
#include <random>
#include <vector>
#include <queue>
#include <set>
#include <list>
#include <iterator>
#include <time.h>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "segment.h"
#include "disjoint.h"

enum Solvers { OTTMAN, NAIVE, SORTED_NAIVE, BFS_GRAPH, DISJOINT_SET };

class PlanarIntersections
{
public:

	PlanarIntersections();
	
	void addSegment( const Segment& s );
	void addSegment( double, double, double, double );
	void generateSegments( int n = 2000, double length = 50.0 );
	
	bool solve();
	void solveWithTime();

	void printResults() const;
	void visualize();

	Solvers getIntersectionSolver() const
	{ return intersection_solver; }
	void setIntersectionSolver( Solvers s) 
	{ intersection_solver = s; }
	Solvers getGraphSolver() const
	{ return graph_solver; }
	void setGraphSolver( Solvers s)
	{ graph_solver = s; }

	bool drawSquares() const
	{ return draw_squares; }
	void setDrawSquares( bool d )
	{ draw_squares = d; }

	double getMin() const
	{ return min; }
	void setMin( double m )
	{ min = m; }
	double getMax() const
	{ return max; }
	void setMax( double m )
	{ max = m; }
	

private:
	bool naive();
	bool sorted_naive();
	bool OttmanBentley();

	bool computeBeginningPoint(std::set<Point, Point::cmp_point >& event_queue, std::set<Segment*, Segment::cmp_ptr>& segments_tree, Point& p);
	bool computeEndPoint(std::set<Point, Point::cmp_point >& event_queue, std::set<Segment*, Segment::cmp_ptr>& segments_tree, Point& p);
	bool computeCrossingPoint(std::set<Point, Point::cmp_point >& event_queue, std::set<Segment*, Segment::cmp_ptr>& segments_tree, Point& p);

	void BFS();
	void disjointSetFind();

	void addSquare(int, int);

	std::vector<sf::RectangleShape> squares;
	std::vector<Segment> segments;	
	double min, max;
	Solvers graph_solver, intersection_solver;
	bool draw_squares;
};