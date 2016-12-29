#include <stdlib.h>
#include <iostream>
#include <random>
#include <vector>
#include <queue>
#include <set>
#include <iterator>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


enum Type  { BEGINNING = 1, CROSS = 2, END = 3 };

bool smaller( double a, double b, double epsilon = 0.0001 );
bool equal(double a, double b, double epsilon);

struct SegmentData {
	int getIndex() const
	{ return index; }

	std::vector<SegmentData*>& getNeighbours()
	{ return neighbours; }

	void setGroup( int g )
	{ group = g; }

	double x1, y1, x2, y2;
	double s_x, s_y;
	double special_intersection;
	std::vector<SegmentData*> neighbours;
	sf::Vertex line[2];
	sf::Color color;
	int group;
	int index;
	int number;
};

class Point
{
public:
	Point();
	Point(double m_x, double m_y, Type m_t, SegmentData *m_owner, SegmentData *m_intersection=nullptr);
	~Point() {}

	void printInfo() const;
	Type getType() const
	{ return t; }

	int getOwner() const
	{ return owner->getIndex(); }

	int getIntersection() const
	{ return intersection->getIndex(); }

	void setIntersection( SegmentData *i ) 
	{ intersection = i; }

	int getNumber() const 
	{ return owner->number; }

	struct cmp_point {
		bool operator()(const Point& p1, const Point& p2) const;
	};
	double x;
	double y;
	Type t;
	SegmentData *owner;
	SegmentData *intersection;
};

class Segment
{
public:
	Segment() : data(new SegmentData) {};
	Segment(double, double, double, double);	
	Segment(const Segment&);
	~Segment() { if ( data != nullptr ) delete data; }

	bool intersects(const Segment&) const;
	bool intersects(const Segment&, double&, double&) const;
	bool intersects(const Segment *s, double& x, double& y) const
	{ return intersects(*s, x, y); }
	static Segment generateSegment(double min, double max);
	static Segment generateLengthSegment(double min, double max, double length);
	static Segment generateParallelSegment(const Segment& s, int range);
	static Segment generateSegmentFromStart(const Segment& s, int range);
	static void resetIndex() 
	{ global_index = 0; }

	sf::Color generateColor(int) const;
	void printInfo() const;
	void printNeighbours() const;
	void connect(Segment& s);

	sf::Vertex *getVertexes();

	int getIndex() const
	{ return data->index; }

	std::vector<SegmentData*>& getNeighbours()
	{ return data->neighbours; }

	Point getBeginning() const
	{ return Point(data->x1, data->y1, BEGINNING, data); }

	Point getEnd() const
	{ return Point(data->x2, data->y2, END, data); }

	int getGroup() const
	{ return data->group; }

	void setGroup(int g);
	Point sweepLineIntersection() const;
	static void setSweepLine( const Point& p)
	{ sweep_line = p.x; }

	void swap( Segment& );

	void setSpecialIntersection( Point );

	bool isVertical() const
	{ return equal(data->s_x, 0, 0.0001); }

	SegmentData *getData() const
	{ return data; }


	struct cmp_ptr
	{
		bool operator()(const Segment *s1, const Segment *s2) const;
	};
private:


	SegmentData *data;
	static int global_index;
	static double sweep_line;
};


