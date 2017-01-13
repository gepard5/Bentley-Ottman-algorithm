/*
 * =====================================================================================
 *
 *       Filename:  segment.cpp
 *
 *    Description:  Implementing Ottman-Bentley algorithm for fiding intersections of segments on a plane and then finding connected graphs among them.
 *    				Also allowsuser to compare different methods of finding intersections and connected graph components.
 *
 *        Version:  1.0
 *        Created:  06.11.2016 13:58:40
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Michał Glinka 
 *   Organization:  Politechnika Warszawska
 *
 * =====================================================================================
 */
#include "segment.h"

int Segment::global_index = 0;
double Segment::sweep_line = 0.0;
const double precision = 0.0001;

//function for better doubles comparing
bool equal(double a, double b, double epsilon)
{
    return fabs(a - b) <= epsilon;
}

bool smaller( double a, double b, double epsilon)
{
	return  (b - a) > epsilon;
}


Point::Point(double m_x, double m_y, Type m_t, SegmentData *m_owner, SegmentData *m_intersection): 
	x(m_x), y(m_y), t(m_t), owner(m_owner), intersection(m_intersection) {}

//function used by priority queue and tree to compare points
bool Point::cmp_point::operator()(const Point& p1, const Point& p2) const
{ 
	//if points are the same they can't be smaller
	if( p1.getNumber() == p2.getNumber() && p1.getType() == p2.getType() && p1.getOwner() == p2.getOwner() )
		return false;

	//if points are from the same owner then
	//beginning < cross < end
	if( p1.getNumber() == p2.getNumber() ) {
		if( p1.t != p2.t ) return p1.t < p2.t;
	}
	
	//if comparing crossing and beginning or ending of its owner or intersection
	//then beginning < cross < end
	if( p1.getType() == CROSS && p1.getIntersection() == p2.getOwner() ) {
		if( p1.t != p2.t ) return p1.t < p2.t;
	}

	if( p2.getType() == CROSS && p2.getIntersection() == p1.getOwner() ) {
		if( p1.t != p2.t ) return p1.t < p2.t;
	}
	
	return equal( p1.x, p2.x, precision ) ? smaller(p1.y, p2.y) : smaller(p1.x, p2.x); 
}


void Point::printInfo() const
{
	std::cout<<"Point. X: "<<x<<" Y: "<<y;
	std::cout<<"type: "<<t<<" segmentsi: "<<getNumber();
	if( intersection ) std::cout<<" and "<<intersection->number;
	std::cout<<std::endl;
}

bool operator>(const Point& p1, const Point& p2)
{ 
	//same points can't be smaller
	if( p1.getNumber() == p2.getNumber() && p1.getIntersection() == p2.getIntersection() && p1.getType() == p2.getType() ) 
		return false;
	return equal(p1.x, p2.x, precision) ? p1.y < p2.y : p1.x < p2.x ; 
}

bool operator==(const Point& p1, const Point& p2)
{
	return p1.x == p2.x && p1.y == p2.y ;
}



//function used by segment tree to compare segments
bool Segment::cmp_ptr::operator()(const Segment *s1, const Segment *s2) const
{
	if( s1->data->index == s2->data->index ) return false;

	Point p1 = s1->sweepLineIntersection();
	Point p2 = s2->sweepLineIntersection();

	if( equal(p1.y, p2.y, precision) ) {
		//vertical segment is always bigger
		if( equal(s1->data->s_x, 0, precision) ) return false;
		if( equal(s2->data->s_x, 0, precision) ) return true;
	
		//comparing is happening before swap
		//so if we compare crossing of two points
		//we can compare their beginnings
		Point s1_beg = s1->getBeginning();

		double old_sweep_line = sweep_line;
		sweep_line = s1_beg.x;

		p1 = s1->sweepLineIntersection();
		p2 = s2->sweepLineIntersection();
		sweep_line = old_sweep_line;
	}

	return p1.y < p2.y ;
}


Segment::Segment(double a, double b, double c, double d)
{
	data = new SegmentData();
	//x1,y1 - beginning
	//x2,y2 - end
	if(a <= c)
	{
		data->x1 = a; data->x2 = c;
		data->y1 = b; data->y2 = d;
	}
	else
	{
		data->x1 = c; data->x2 = a;
		data->y1 = d; data->y2 = b;
	}
	data->s_x = data->x2 - data->x1;
	data->s_y = data->y2 - data->y1;
	data->color = sf::Color::White;
	data->index = global_index++;
	data->group = -1;
	data->special_intersection = data->y1;
	data->number = data->index;
}

Segment::Segment( const Segment& s)
{
	data = new SegmentData();
	*data = *(s.data);
}

void Segment::printInfo() const
{
	if( data == nullptr ) {
		std::cout<<"Tried reading empty segment!"<<std::endl;
		return;
	}
	std::cout<<"Index: "<<data->number<<" Beginning: ("<<data->x1<<", "<<data->y1<<")  End: ("<<data->x2<<", "<<data->y2<<")"<<" Group: "<<data->group;
}

void Segment::printNeighbours() const
{
	std::cout<<"Segment "<<getIndex()<<std::endl;
	for(auto n : data->neighbours)
		std::cout<<"Neighbour: "<<n->getIndex()<<std::endl;
}

Segment Segment::generateSegment(double min, double max)
{
	std::random_device rd;
	std::default_random_engine e2(rd());
	std::uniform_real_distribution<double> dist(min, max);
	return Segment(dist(e2), dist(e2), dist(e2), dist(e2));
}
		
Segment Segment::generateLengthSegment(double min, double max, double length)
{
	std::random_device rd;
	std::default_random_engine e2(rd());
	std::uniform_real_distribution<double> dist(min, max);
	double x1 = dist(e2);
	double y1 = dist(e2);

	//try until segment won't fit into square
	double x_shift, y_shift;
	std::uniform_real_distribution<double> dist_length(-length, length);
	do {
		x_shift = dist_length(e2);
		y_shift = dist_length(e2);
	} while ( ( x1 + x_shift ) < min || ( x1 + x_shift ) > max || ( y1 + y_shift) < min || ( y1 + y_shift ) > max  );
	return Segment(x1, y1, x1 + x_shift, y1 + y_shift );
}

Segment Segment::generateParallelSegment(const Segment& s, int range)
{
	std::random_device rd;
	std::default_random_engine e2(rd());
	std::uniform_real_distribution<double> dist(-range, range);
	int x_shift = dist(e2);
	int y_shift = dist(e2);
	return Segment(s.data->x1 + x_shift, s.data->y1 + y_shift, s.data->x2 + x_shift, s.data->y2 + y_shift);
}

Segment Segment::generateSegmentFromStart(const Segment& s, int range)
{
	std::random_device rd;
	std::default_random_engine e2(rd());
	std::uniform_real_distribution<double> dist(0, 1);
	double x = s.data->x1 + s.data->s_x * dist(e2);
	double y = s.data->y1 + s.data->s_y * dist(e2);

	std::uniform_real_distribution<double> dist_shift(-range, range);
	return Segment(x, y, s.data->x2 + dist_shift(e2), s.data->y2 + dist_shift(e2));
}

bool Segment::intersects(const Segment& s) const
{
	double x, y;
	return intersects( s, x, y);
}

bool Segment::intersects(const Segment& s, double& x, double& y) const
{
	
	double parallel = -s.data->s_x * data->s_y + data->s_x * s.data->s_y;
	//two segments are parallel
	if(parallel == 0)
	{
		double a_x, a_y;
		a_x = s.data->x1 - data->x1;
		a_y = s.data->y1 - data->y1;
		//segments might be colinear
		if( a_x * data->s_y - a_y * data->s_x == 0)
		{
			if( data->x1 >= s.data->x1 && data->x1 <= s.data->x2 )
			{
				x = data->x1; y = data->y1;
				return true;
			}
			if( s.data->x1 >= data->x1 && s.data->x1 <= s.data->x2 )
			{
				x = s.data->x1; y = s.data->y1;
				return true;
			}
			return false;
		}
		

	}

	double u,t;
	u = (-data->s_y * (data->x1 - s.data->x1) + data->s_x * (data->y1 - s.data->y1) ) / ( -s.data->s_x * data->s_y + data->s_x * s.data->s_y) ;
	t = ( s.data->s_x * (data->y1 - s.data->y1) - s.data->s_y * (data->x1 - s.data->x1)) / ( -s.data->s_x * data->s_y + data->s_x * s.data->s_y) ;

	//segments are not colinear and intersect
	if(u >= 0 && u <= 1 && t >= 0 && t <= 1)
	{
		x = data->x1 + (t * data->s_x);
		y = data->y1 + (t * data->s_y);
		return true;
	}
	return false;
}	

sf::Vertex *Segment::getVertexes()
{	
	data->color = generateColor(data->group);
	data->line[0] = sf::Vertex(sf::Vector2f(data->x1, data->y1), data->color);
	data->line[1] = sf::Vertex(sf::Vector2f(data->x2, data->y2), data->color);
	return data->line;
}

void Segment::connect(Segment& s)
{
	data->neighbours.push_back(s.getData());
}

void Segment::setGroup(int g)
{
	data->group = g;
}

sf::Color Segment::generateColor(int group) const
{
	srand(data->group);
	return sf::Color(rand() % 255, rand() % 255, rand() % 255);
}

Point Segment::sweepLineIntersection() const
{
	//segment is vertical - return special point
	if( equal(data->s_x, 0, precision ) ) {
		return Point(sweep_line, data->special_intersection, CROSS, data );
	}
	
	double x = sweep_line;
	double y = data->y1 + ( (x - data->x1) * data->s_y / data->s_x ) ;
	return Point(x, y, CROSS, data );
}

void Segment::setSpecialIntersection( Point p )
{
	data->special_intersection = p.y;
}

//swap segments data
//don;t change their index cause their position in vector didn't change
void Segment::swap( Segment& s )
{
	auto tmp_index = data->index;
	auto tmp = data;
	data = s.data;
	s.data = tmp;
	s.data->index = data->index;
	data->index = tmp_index;
}