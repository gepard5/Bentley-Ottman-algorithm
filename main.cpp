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

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

enum Type  { BEGINNING, END, CROSS };

class Point
{
public:
	Point();
	Point(double, double, Type);
	~Point() {}

	void printInfo();

	double x;
	double y;
	Type t;
};

Point::Point(double m_x, double m_y, Type m_t): x(m_x), y(m_y), t(m_t) {}
	
void Point::printInfo()
{
	std::cout<<"Point. X: "<<x<<" Y: "<<y<<std::endl;
}

bool operator>(const Point& p1, const Point& p2)
{ 
	return p1.x == p2.x ? p1.y > p2.y : p1.x > p2.x ; 
}

class Segment
{
public:
	Segment();
	Segment(double, double, double, double);	
	~Segment() {}

	bool intersect(const Segment&, double&, double&) const;
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
	{ return index; }

	std::vector<Segment*>& getNeighbours()
	{ return neighbours; }

	Point getBeginning()
	{ return Point(x1, y1, BEGINNING); }

	Point getEnd()
	{ return Point(x1, y2, END); }

	int getGroup() const
	{ return group; }

	void setGroup(int g);
private:
	double x1, y1, x2, y2;
	double s_x, s_y;
	std::vector<Segment*> neighbours;
	sf::Vertex line[2];
	sf::Color color;
	int group;
	int index;
	static int global_index;
};

int Segment::global_index = 0;

Segment::Segment(double a, double b, double c, double d)
{
	if(a <= c)
	{
		x1 = a; x2 = c;
		y1 = b; y2 = d;
	}
	else
	{
		x1 = c; x2 = a;
		y1 = d; y2 = b;
	}
	s_x = x2 - x1;
	s_y = y2 - y1;
	color = sf::Color::White;
	index = ++global_index;
	group = -1;
}

void Segment::printInfo() const
{
	std::cout<<"Index: "<<index<<" Beginning: ("<<x1<<", "<<y1<<")  End: ("<<x2<<", "<<y2<<")"<<" Group: "<<group<<std::endl;
}

void Segment::printNeighbours() const
{
	std::cout<<"Segment "<<index<<std::endl;
	for(auto n : neighbours)
		std::cout<<"Neighbour: "<<n->index<<std::endl;
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
	return Segment(s.x1 + x_shift, s.y1 + y_shift, s.x2 + x_shift, s.y2 + y_shift);
}

Segment Segment::generateSegmentFromStart(const Segment& s, int range)
{
	std::random_device rd;
	std::default_random_engine e2(rd());
	std::uniform_real_distribution<double> dist(0, 1);
	double x = s.x1 + s.s_x * dist(e2);
	double y = s.y1 + s.s_y * dist(e2);

	std::uniform_real_distribution<double> dist_shift(-range, range);
	return Segment(x, y, s.x2 + dist_shift(e2), s.y2 + dist_shift(e2));
}

bool Segment::intersect(const Segment& s, double& x, double& y) const
{
	
	double parallel = -s.s_x * s_y + s_x * s.s_y;
	if(parallel == 0)
	{
		double a_x, a_y;
		a_x = s.x1 - x1;
		a_y = s.y1 - y1;
		if( a_x * s_y - a_y * s_x == 0) //colinear
		{
			if( x1 >= s.x1 && x1 <= s.x2 )
			{
				x = x1; y = y1;
				return true;
			}
			if( s.x1 >= x1 && s.x1 <= s.x2 )
			{
				x = s.x1; y = s.y1;
				return true;
			}
			return false;
		}
		

	}

	double u,t;
	u = (-s_y * (x1 - s.x1) + s_x * (y1 - s.y1) ) / ( -s.s_x * s_y + s_x * s.s_y) ;
	t = ( s.s_x * (y1 - s.y1) - s.s_y * (x1 - s.x1)) / ( -s.s_x * s_y + s_x * s.s_y) ;

	if(u >= 0 && u <= 1 && t >= 0 && t <= 1)
	{
		x = x1 + (t * s_x);
		y = y1 + (t * s_y);
		return true;
	}
	return false;
}	

sf::Vertex *Segment::getVertexes()
{	
	line[0] = sf::Vertex(sf::Vector2f(x1, y1), color);
	line[1] = sf::Vertex(sf::Vector2f(x2, y2), color);
	return line;
}

void Segment::connect(Segment& s)
{
	neighbours.push_back(&s);
}

void Segment::setGroup(int g)
{
	group = g;
	color = generateColor(group);
}

sf::Color Segment::generateColor(int group) const
{
	srand(group);
	return sf::Color(rand() % 255, rand() % 255, rand() % 255);
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
	std::queue<Segment*> group;
	Segment* s;
	int index;
	for(int i = 0; i < segments.size(); ++i)
	{
		s = &segments[i];
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
	//generate beginning priority queue
	std::priority_queue<Point, std::vector<Point>, std::greater<Point> > event_queue;
	for( auto s : segments )
	{
		event_queue.push(s.getBeginning());
		event_queue.push(s.getEnd());
	}	

	while( !event_queue.empty() )
	{
		event_queue.top().printInfo();
		event_queue.pop();
	}
}

int main()
{
	int n = 50;
	double x,y;
	//cin>>n;
	std::vector<Segment> segments;
	std::vector<sf::RectangleShape> squares;

	Segment s = Segment::generateSegment(0, 100);
	s.printInfo();
	for(int i = 0; i<n; ++i)
	{
		segments.push_back(Segment::generateLengthSegment(0,1000,30));
	}
	for(int i = 0; i < n; ++i)
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
	}
	OttmanBentley(segments);
	BFS(segments);
		
	for( auto s : segments )
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
		for( auto s : segments)
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

