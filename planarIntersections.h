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

//types of solvers: Bentley-Ottman, Naive, Naive with sorting, Broad-First-Search, Disjoint-Sets
enum Solvers { OTTMAN, NAIVE, SORTED_NAIVE, BFS_GRAPH, DISJOINT_SET };

/*
 * class solving problem of intersecting segments on a plane and connected components of graph
 */
class PlanarIntersections
{
public:
	//defualt constructor
	PlanarIntersections();
	
	/* 
	 * add single segment from already created segment or coordinates
	 */
	void addSegment( const Segment& s );
	void addSegment( double, double, double, double );
	
	/* 
	 * generate given number of segments of maixmal length specified
	 * clears all previous segments
	 */
	void generateSegments( int n = 2000, double length = 50.0 );
	
	/*
	 * try to solve problem of intersecting segments on a plane and connected components of graph
	 */
	bool solve();
	/*
	 * solve problem and if suceeded print time needed for computing
	 */
	void solveWithTime();

	/*
	 * print info about every segments added to class
	 */
	void printResults() const;

	/* 
	 * create window showing every segment
	 * connected segments have the same colour
	 * may also add a red square for every intersection on plane
	 */
	void visualize();

	/* 
	 * get intersection solver
	 */
	Solvers getIntersectionSolver() const
	{ return intersection_solver; }
	/*  
	 *  set intersection solver
	 */
	void setIntersectionSolver( Solvers s) 
	{ intersection_solver = s; }
	/*  
	 *  get connected components solver
	 */
	Solvers getGraphSolver() const
	{ return graph_solver; }
	/*
	 * set connected components solver
	 */
	void setGraphSolver( Solvers s)
	{ graph_solver = s; }

	/*
	 * return whether squares are drawn
	 */
	bool drawSquares() const
	{ return draw_squares; }
	/*
	 * set drawing squares on visualization
	 */
	void setDrawSquares( bool d )
	{ draw_squares = d; }
	/*
	 * get minimal coordinate for generated segments
	 */
	double getMin() const
	{ return min; }
	/*
	 * set minimal coordinate for generated segments
	 */
	void setMin( double m )
	{ min = m; }
	/*
	 * get maximal coordinate for generated segments
	 */
	double getMax() const
	{ return max; }
	/*
	 * set maximal coordinate for generated segments
	 */
	void setMax( double m )
	{ max = m; }
	

private:
	/*
	 * use naive solver to find intersections of segments on plane
	 */
	bool naive();
	/*
	 * use naive solver with presorting to find intersections of segments on plane
	 */
	bool sorted_naive();
	/*
	 * use Bentley-Ottman algorithm to find intersections of segments on plane
	 */
	bool OttmanBentley();

	/*
	 * compute beginning, end or crossing point for Bentley-Ottman algorithm
	 */
	bool computeBeginningPoint(std::set<Point, Point::cmp_point >& event_queue, std::set<Segment*, Segment::cmp_ptr>& segments_tree, Point& p);
	bool computeEndPoint(std::set<Point, Point::cmp_point >& event_queue, std::set<Segment*, Segment::cmp_ptr>& segments_tree, Point& p);
	bool computeCrossingPoint(std::set<Point, Point::cmp_point >& event_queue, std::set<Segment*, Segment::cmp_ptr>& segments_tree, Point& p);

	/*
	 * use Broad-First-Search algorithm to find connected components of graph
	 */
	void BFS();
	/*
	 * use disjoints-sets to find connected components of graph
	 */
	void disjointSetFind();

	/*
	 * add new square representing new intersection of segments
	 */
	void addSquare(int, int);

	/*
	 * vector of squares representing intersections on plane
	 */
	std::vector<sf::RectangleShape> squares;
	/*
	 * vector of segments from which we solve a problem
	 */
	std::vector<Segment> segments;	
	/*
	 * minimal and maximal coordinate of generated segments
	 */
	double min, max;
	/*
	 * solvers for solving a problem
	 */
	Solvers graph_solver, intersection_solver;
	/*
	 * test whether squares should be drawn or not
	 */
	bool draw_squares;
};