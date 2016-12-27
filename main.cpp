/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  Implementing Ottman-Bentley algorithm for fiding intersections of segments on a plane and then finding connected graphs among them.
 *
 *        Version:  1.0
 *        Created:  06.11.2016 13:58:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michał Glinka 
 *   Organization:  Politechnika Warszawska
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <iostream>
#include <random>
#include <vector>
#include <queue>
#include <set>
#include <iterator>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

enum Type  { BEGINNING, END, CROSS };

bool equal(double a, double b, double epsilon)
{
    return fabs(a - b) <= epsilon;
}

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

bool Point::cmp_point::operator()(const Point& p1, const Point& p2) const
{ 
	if( p1.getNumber() == p2.getNumber() && p1.getType() == p2.getType() && p1.getOwner() == p2.getOwner() )
		return false;
	return equal( p1.x, p2.x, 0.0001 ) ? p1.y < p2.y : p1.x < p2.x; 
}

class Segment
{
public:
	Segment() : data(new SegmentData) {};
	Segment(double, double, double, double);	
	Segment(const Segment&);
	~Segment() { std::cout<<"Destructor: "<<getIndex()<<std::endl; if ( data != nullptr ) delete data; }

	bool intersects(const Segment&, double&, double&) const;
	bool intersects(const Segment *s, double& x, double& y) const
	{ return intersects(*s, x, y); }
	static Segment generateSegment(double min, double max);
	static Segment generateLengthSegment(double min, double max, double length);
	static Segment generateParallelSegment(const Segment& s, int range);
	static Segment generateSegmentFromStart(const Segment& s, int range);
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
	{ return data->s_x == 0; }

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


int Segment::global_index = 0;
double Segment::sweep_line = 0.0;


std::vector<sf::RectangleShape> squares;

Point::Point(double m_x, double m_y, Type m_t, SegmentData *m_owner, SegmentData *m_intersection): 
	x(m_x), y(m_y), t(m_t), owner(m_owner), intersection(m_intersection) {}
	
void Point::printInfo() const
{
	std::cout<<"Point. X: "<<x<<" Y: "<<y<<std::endl;
}

bool operator>(const Point& p1, const Point& p2)
{ 
	if( p1.getNumber() == p2.getNumber() && p1.getIntersection() == p2.getIntersection() && p1.getType() == p2.getType() ) 
		return false;
	return p1.x == p2.x ? p1.y < p2.y : p1.x < p2.x ; 
}

bool operator==(const Point& p1, const Point& p2)
{
	return p1.x == p2.x && p1.y == p2.y ;
}




bool Segment::cmp_ptr::operator()(const Segment *s1, const Segment *s2) const
{
	if( s1->data->index == s2->data->index ) return false;

	Point p1 = s1->sweepLineIntersection();
	Point p2 = s2->sweepLineIntersection();

	if( equal(p1.y, p2.y, 0.0001) ) {
		std::cout<<"Equal found!"<<std::endl;
		if( equal(s1->data->s_x, 0, 0.0001) ) return false;
		if( equal(s2->data->s_x, 0, 0.0001) ) return true;

		Point s1_beg = s1->getBeginning();
		Point s2_beg = s2->getBeginning();

		double old_sweep_line = sweep_line;
		sweep_line = s1_beg.x;

		p1 = s1->sweepLineIntersection();
		p2 = s2->sweepLineIntersection();
		std::cout<<"P1: "<<p1.y<<std::endl;
		std::cout<<"P2: "<<p2.y<<std::endl;

		sweep_line = old_sweep_line;
	}

	return p1.y < p2.y ;
}


Segment::Segment(double a, double b, double c, double d)
{
	data = new SegmentData();
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
	Point p = sweepLineIntersection();
	std::cout<<"Sweepline intersection: "<<p.x<<" "<<p.y<<std::endl;
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
	std::uniform_real_distribution<double> dist(min + length, max - length);
	double x1 = dist(e2);
	double y1 = dist(e2);

	std::uniform_real_distribution<double> dist_length(-length, length);
	double x_shift = dist_length(e2);
	double y_shift = dist_length(e2);
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

bool Segment::intersects(const Segment& s, double& x, double& y) const
{
	
	double parallel = -s.data->s_x * data->s_y + data->s_x * s.data->s_y;
	if(parallel == 0)
	{
		double a_x, a_y;
		a_x = s.data->x1 - data->x1;
		a_y = s.data->y1 - data->y1;
		if( a_x * data->s_y - a_y * data->s_x == 0) //colinear
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
	data->color = generateColor(data->group);
}

sf::Color Segment::generateColor(int group) const
{
	srand(data->group);
	return sf::Color(rand() % 255, rand() % 255, rand() % 255);
}

Point Segment::sweepLineIntersection() const
{
	if( equal(data->s_x, 0, 0.0001 ) ) {
			std::cout<<"Vertical sweep line"<<std::endl;
		return Point(sweep_line, data->special_intersection, CROSS, data );
	}
	double x = sweep_line;
	double y = data->y1 + ( (x - data->x1) * data->s_y / data->s_x ) ;
	return Point(x, y, CROSS, data );
}

void Segment::setSpecialIntersection( Point p )
{
	data->special_intersection = p.x;
}

void Segment::swap( Segment& s )
{
	auto tmp_index = data->index;
	auto tmp = data;
	data = s.data;
	s.data = tmp;
	s.data->index = data->index;
	data->index = tmp_index;
}

void addSquare(std::vector<sf::RectangleShape>& squares, int x, int y)
{
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(5, 5));
	rectangle.setFillColor(sf::Color::Red);
	rectangle.setPosition(x - 2 , y - 2);
	squares.push_back(rectangle);
}

void BFS(std::vector<Segment>& segments)
{
	std::vector<bool> visited(segments.size(), false);
	int group_index = -1;
	std::queue<SegmentData*> group;
	SegmentData* s;
	int index;
	for(int i = 0; i < segments.size(); ++i)
	{
		s = segments[i].getData();
		index = s->getIndex();
		if(visited[index]) continue;
		
		group.push(s);
		++group_index;
		visited[index] = true;
		s->setGroup(group_index);
		while(!group.empty())
		{
			s = group.front();
			index = s->getIndex();
			auto neighbours = s->getNeighbours();
			for( auto n : neighbours)
			{
				if(visited[n->getIndex()]) continue;

				visited[n->getIndex()] = true;
				group.push(n);
				n->setGroup(group_index);
			}
			group.pop();
		}
	}
}

void OttmanBentley(std::vector<Segment>& segments)
{
	typedef std::set<Segment*, Segment::cmp_ptr>::iterator seg_itr;
	//generate beginning priority queue
	std::set<Point, Point::cmp_point > event_queue;
	std::set<Segment*, Segment::cmp_ptr> segments_tree;

	for( auto& s : segments )
	{
		event_queue.insert(s.getBeginning());
		event_queue.insert(s.getEnd());
	}

	while( !event_queue.empty() )
	{
		for( auto s : segments_tree )
			s->printInfo();
		std::cout<<segments_tree.size()<<std::endl;
		auto& p = *event_queue.begin();
		std::cout<<"Point of segment: "<<p.getNumber()<<std::endl;
		Segment *curr_segm = &(segments[p.getOwner()]);
		seg_itr predecessor = segments_tree.end();
		seg_itr sucessor = segments_tree.end();
		Segment::setSweepLine( p );
		if(p.getType() == BEGINNING ) {
			std::cout<<"BEGINNING"<<std::endl;
			seg_itr curr_itr = segments_tree.insert( curr_segm ).first;
			if( curr_itr != segments_tree.begin() ){
				predecessor = std::prev(curr_itr);
			}
			sucessor = std::next(curr_itr);
			double x,y;
			if( predecessor != segments_tree.end() && sucessor != segments_tree.end() && (*predecessor)->intersects( *sucessor, x, y ) ){
				event_queue.erase( Point(x, y, CROSS, (*predecessor)->getData(), (*sucessor)->getData() ) );
			}

			if( predecessor != segments_tree.end() && (*predecessor)->intersects( *curr_itr, x, y ) && ( x > p.x || ( equal( x, p.x, 0.0001) && y > p.y ) ) ) {
				event_queue.insert( Point(x, y, CROSS, (*predecessor)->getData(), (*curr_itr)->getData() ) );
			}

			if( sucessor != segments_tree.end() && (*sucessor)->intersects( *curr_itr, x, y) && ( x > p.x || ( equal( x, p.x, 0.0001) && y > p.y ) ) ) {
				event_queue.insert( Point(x, y, CROSS, (*curr_itr)->getData(), (*sucessor)->getData() ) );
			}
		}
		else if( p.getType() == END ){
			std::cout<<"END"<<std::endl;
			seg_itr curr_itr = segments_tree.insert( curr_segm ).first;
			if( curr_itr != segments_tree.begin() ){
				predecessor = std::prev(curr_itr);
			}
			sucessor = std::next(curr_itr);
			double x,y;
			if( predecessor != segments_tree.end() && sucessor != segments_tree.end() ) {
				if( (*predecessor)->intersects( *sucessor, x, y ) && ( x > p.x || ( equal( x, p.x, 0.0001) && y > p.y ) ) ) {
					event_queue.insert( Point(x, y, CROSS, (*predecessor)->getData(), (*sucessor)->getData() ) );
				}
			}
			segments_tree.erase( curr_itr );
		}
		else {
			int s1 = p.getOwner();
			int s2 = p.getIntersection();
			std::cout<<"Intersection found between : "<<p.getNumber()<<" and "<<segments[s2].getEnd().getNumber()<<std::endl;
			segments[s1].connect(segments[s2]);
			segments[s2].connect(segments[s1]);

			std::cout<<"Finding s1!"<<std::endl;
			auto s1_itr = segments_tree.find( &segments[s1] );
			std::cout<<"Findind s2!"<<std::endl;
			auto s2_itr = segments_tree.find( &segments[s2] );
			if( s1_itr == segments_tree.end() ) std::cout<<"s1 fail!"<<std::endl;
			if( s2_itr == segments_tree.end() ) std::cout<<"s2 fail!"<<std::endl;

			if( s1_itr != segments_tree.begin() ) {
				predecessor = s1_itr;
				++predecessor;
			}
			sucessor = s2_itr;
			++sucessor;

			double x,y;
			segments[s1].intersects( segments[s2], x, y);
			addSquare(squares, x, y);
			if( sucessor != segments_tree.end() && (*sucessor)->intersects( *s2_itr, x, y) ){
				event_queue.erase( Point(x, y, CROSS, (*s2_itr)->getData(), (*sucessor)->getData() ) );
			}

			if( predecessor != segments_tree.end() && (*predecessor)->intersects( *s1_itr, x, y) ){
				event_queue.erase( Point(x, y, CROSS, (*predecessor)->getData(), (*s1_itr)->getData() ) );
			}

			if( sucessor != segments_tree.end() && (*sucessor)->intersects( *s1_itr, x, y) &&  ( x > p.x || ( equal(x, p.x, 0.0001) && y > p.y ) )  ){
				event_queue.insert( Point(x, y, CROSS, (*s1_itr)->getData(), (*sucessor)->getData() ) );
			}

			if( predecessor != segments_tree.end() && (*predecessor)->intersects( *s2_itr, x, y) && ( x > p.x || ( equal(x, p.x, 0.0001) && y > p.y ) ) ){
				event_queue.insert( Point(x, y, CROSS, (*predecessor)->getData(), (*s2_itr)->getData() ) );
			}

			if( segments[s1].isVertical() )
				segments[s1].setSpecialIntersection( segments[s2].sweepLineIntersection() );

			if( segments[s2].isVertical() )
				segments[s2].setSpecialIntersection( segments[s1].sweepLineIntersection() );

			segments[s1].swap( segments[s2] );
		}
		event_queue.erase( p );
	}

}

int main()
{
	int n = 10;
	double x,y;
	//cin>>
	std::vector<Segment> segments;

	for(int i = 0; i<n; ++i)
	{
		segments.push_back(Segment::generateLengthSegment(0,1000,400));
	}
/*  	for(int i = 0; i < n; ++i)
	{
		for(int j = i+1; j < n; ++j)
		{
			if(segments[i].intersect(segments[j], x, y))
			{
				std::cout<<"Intersection "<<i<<" "<<j<<" at "<<x<<" "<<y<<std::endl;
				addSquare(squares, x, y);
				segments[i].connect(segments[j]);
				segments[j].connect(segments[i]);
			}
		}
	}*/
	OttmanBentley(segments);
	BFS(segments);
	for( auto& s : segments )
		s.printInfo();

	sf::RenderWindow window(sf::VideoMode(1000, 1000), "Test window");

	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
				window.close();
		}


		window.clear();
		for( auto& s : segments)
		{
			window.draw(s.getVertexes(), 2, sf::Lines);
		}
		for(auto s : squares)
		{
			window.draw(s);
		}
		window.display();
	}

}

