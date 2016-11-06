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

using namespace std;



class Segment
{
public:
	Segment();
	Segment(double, double, double, double);	
	~Segment() {}

	bool intersect(const Segment&, double&, double&);
	static Segment generateSegment(double min, double max);
	static Segment generateParallelSegment(const Segment& s, int range);
	static Segment generateSegmentFromStart(const Segment& s, int range);
	void printInfo();
private:
	double x1, y1, x2, y2;
	double s_x, s_y;
};

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
}

void Segment::printInfo()
{
	std::cout<<"Beginning: ("<<x1<<", "<<y1<<")  End: ("<<x2<<", "<<y2<<")"<<std::endl;
}

Segment Segment::generateSegment(double min, double max)
{
	random_device rd;
	default_random_engine e2(rd());
	uniform_real_distribution<double> dist(0, 100);
	return Segment(dist(e2), dist(e2), dist(e2), dist(e2));
}

Segment Segment::generateParallelSegment(const Segment& s, int range)
{
	random_device rd;
	default_random_engine e2(rd());
	uniform_real_distribution<double> dist(-range, range);
	int x_shift = dist(e2);
	int y_shift = dist(e2);
	return Segment(s.x1 + x_shift, s.y1 + y_shift, s.x2 + x_shift, s.y2 + y_shift);
}

Segment Segment::generateSegmentFromStart(const Segment& s, int range)
{
	random_device rd;
	default_random_engine e2(rd());
	uniform_real_distribution<double> dist(0, 1);
	double x = s.x1 + s.s_x * dist(e2);
	double y = s.y1 + s.s_y * dist(e2);

	uniform_real_distribution<double> dist_shift(-range, range);
	return Segment(x, y, s.x2 + dist_shift(e2), s.y2 + dist_shift(e2));
}

bool Segment::intersect(const Segment& s, double& x, double& y)
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



		

int main()
{
	int n = 100;
	double x,y;
	//cin>>n;
	//vector<Segment> segments;

	Segment s = Segment::generateSegment(0, 100);
	s.printInfo();
	for(int i = 0; i<n; ++i)
	{
		Segment::generateParallelSegment(s, 100).printInfo();
		Segment::generateSegmentFromStart(s, 100).printInfo();
	}

}

