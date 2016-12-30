#include <stdlib.h>
#include <iostream>
#include <random>
#include <vector>
#include <queue>
#include <set>
#include <iterator>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

/*
 * types of points: beginning, crossing or ending
 */
enum Type  { BEGINNING = 1, CROSS = 2, END = 3 };

/*
 * functions used for better comparing double values
 */
bool smaller( double a, double b, double epsilon = 0.0001 );
bool equal(double a, double b, double epsilon = 0.0001 );

/*
 * struct holding segments data
 */
struct SegmentData {
	/*
	 * get index of segment
	 */
	int getIndex() const
	{ return index; }

	/*
	 * get vector of data of segments intersecting this segment
	 */
	std::vector<SegmentData*>& getNeighbours()
	{ return neighbours; }

	/*
	 * set group of this segment
	 */
	void setGroup( int g )
	{ group = g; }

	/*
	 * coordinates of this segment
	 */
	double x1, y1, x2, y2;
	/*
	 * coordinates of vector creating this segment
	 */
	double s_x, s_y;
	/*
	 * special value used for comparing crossing points of vertical segments in Bentley-Ottman algorithm
	 */
	double special_intersection;
	/*
	 * vector of data of segments intersecting with this segment
	 */
	std::vector<SegmentData*> neighbours;
	/* 
	 * struct holding data for drawing thsi segment
	 */
	sf::Vertex line[2];
	/*
	 * color of this segment
	 */
	sf::Color color;
	/*
	 * group of this segment. Connected segments have the same group.
	 */
	int group;
	/*
	 * index in vector of segments where this segment is contained. Doesn't change during swaps of segments.
	 */
	int index;
	/*
	 * unique number representing segment
	 */
	int number;
};

/*
 * class used for representation of points on a plane
 * There are three types of points:
 * beginning, crossing and ending
 * beginning point can never be above or to the right of ending point 
 */
class Point
{
public:
	/*
	 * constructors creating points
	 */
	Point();
	Point(double m_x, double m_y, Type m_t, SegmentData *m_owner, SegmentData *m_intersection=nullptr);
	~Point() {}

	/*
	 * print info about this point
	 */
	void printInfo() const;
	/*
	 * get type of this point
	 */
	Type getType() const
	{ return t; }

	/*
	 * get index of segment this point belongs to
	 */
	int getOwner() const
	{ return owner->getIndex(); }

	/*
	 * get index of segment that intersects owner of this point in this point
	 */
	int getIntersection() const
	{ return intersection->getIndex(); }

	/*
	 * set index of segment that intersects owner of this point in this point
	 */
	void setIntersection( SegmentData *i ) 
	{ intersection = i; }

	/*
	 * get number of segment that this point belongs to
	 */
	int getNumber() const 
	{ return owner->number; }

	/*
	 * struct used for comparing points during sorting
	 */
	struct cmp_point {
		bool operator()(const Point& p1, const Point& p2) const;
	};
	/*
	 * coordinates of point
	 */
	double x;
	double y;
	/*
	 * type of point
	 */
	Type t;
	/*
	 * pointer to data of segment that this point belongs to
	 */
	SegmentData *owner;
	/*
	 * pointer to data of segment that intersects with owner of this point in this point
	 */
	SegmentData *intersection;
};

/*
 * class representing single segment on a plane
 * provides static methods for generating new segments
 */
class Segment
{
public:
	/*
	 * constructors 
	 */
	Segment() : data(new SegmentData) {};
	Segment(double, double, double, double);	
	Segment(const Segment&);
	~Segment() { if ( data != nullptr ) delete data; }

	/*
	 * test whether two segments intersect
	 * write coordinates of intersection into given variables
	 */
	bool intersects(const Segment&) const;
	bool intersects(const Segment&, double&, double&) const;
	bool intersects(const Segment *s, double& x, double& y) const
	{ return intersects(*s, x, y); }

	/*
	 * generate segment with coordinates from range min - max
	 */
	static Segment generateSegment(double min, double max);
	/*
	 * generate segment with coordinates from range min - max
	 * and with length no bigger than specified ( Manhattan metrics )
	 */
	static Segment generateLengthSegment(double min, double max, double length);
	/*
	 * generate segment parallel to the given segment
	 * that lays no further than range
	 */
	static Segment generateParallelSegment(const Segment& s, int range);
	/*
	 * generate segment with one of his ending points lying on given segment
	 * and other ending lying no further than range (Manhattan metrics)
	 */
	static Segment generateSegmentFromStart(const Segment& s, int range);
	/*
	 * reset global index used for calculating indices of new segments
	 */
	static void resetIndex() 
	{ global_index = 0; }

	/*
	 * generate color of segment. Segments from the same group have the same color
	 */
	sf::Color generateColor(int) const;
	/*
	 * print info about this segment
	 */
	void printInfo() const;
	/*
	 * print all segments that intersect this segment
	 */
	void printNeighbours() const;
	/*
	 * add given segment to the list of neighbours
	 */
	void connect(Segment& s);

	/*
	 * return vertexes of segment needed for drawing it
	 */
	sf::Vertex *getVertexes();

	/*
	 * get index of segment
	 */
	int getIndex() const
	{ return data->index; }

	/*
	 * get vector of data of segments intersecting this segment
	 */
	std::vector<SegmentData*>& getNeighbours()
	{ return data->neighbours; }

	/*
	 * get Point representing beginning of this segment
	 */
	Point getBeginning() const
	{ return Point(data->x1, data->y1, BEGINNING, data); }

	/*
	 * get Point representing end of this segment
	 */
	Point getEnd() const
	{ return Point(data->x2, data->y2, END, data); }

	/*
	 * get group of this segment
	 */
	int getGroup() const
	{ return data->group; }

	/*
	 * set group of this segment
	 */
	void setGroup(int g);

	/*
	 * get Point representing intersection of this segment with current sweep line
	 */
	Point sweepLineIntersection() const;
	/*
	 * set sweep line coordinate
	 */
	static void setSweepLine( const Point& p)
	{ sweep_line = p.x; }

	/*
	 * swap data of two segments
	 * indices of segments are not being swapped
	 */
	void swap( Segment& );

	/*
	 * set special intersection value used for comparing points
	 * of crossing with horizontal segments
	 */
	void setSpecialIntersection( Point );

	/*
	 * return whether segment is vertical
	 */
	bool isVertical() const
	{ return equal(data->s_x, 0, 0.0001); }

	/*
	 * get pointer to segment data
	 */
	SegmentData *getData() const
	{ return data; }


	/*
	 * struct used for comparing two segments
	 * based on their intersecions with current sweep line
	 */
	struct cmp_ptr
	{
		bool operator()(const Segment *s1, const Segment *s2) const;
	};
private:

	/* 
	 *	pointer to data of this segment
	 */
	SegmentData *data;
	/*
	 * global variable used for calculating new indices of segments
	 */
	static int global_index;
	/*
	 * current coordinate od sweep line
	 */
	static double sweep_line;
};


