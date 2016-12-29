#include "planarIntersections.h"

PlanarIntersections::PlanarIntersections() : min(0.0) , max(1000.0) , graph_solver(OTTMAN), intersection_solver(BFS_GRAPH), draw_squares(false) {}

void PlanarIntersections::generateSegments( int n, double length ) 
{
	Segment::resetIndex();
	segments.erase( segments.begin(), segments.end() );
	squares.erase( squares.begin(), squares.end() );
	for( unsigned int i = 0; i < n; ++i )
	{
		segments.push_back( Segment::generateLengthSegment( min, max, length ) );
	}
}

void PlanarIntersections::addSegment( const Segment& s )
{
	segments.push_back( s );
}

void PlanarIntersections::addSegment( double x1, double y1, double x2, double y2)
{
	segments.push_back( Segment(x1, y1, x2, y2) );
}

void PlanarIntersections::visualize()
{
	sf::RenderWindow window(sf::VideoMode((int)max, (int)max), "Test window");

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
		if( draw_squares ) {
			for(auto s : squares)
			{
				window.draw(s);
			}
		}
		window.display();
	}


}

bool PlanarIntersections::solve()
{
	if( intersection_solver == OTTMAN ) {
		if( !OttmanBentley() ) {
			std::cout<<" Too many points ! "<<std::endl;
			return false;
		}
	}
	else if( intersection_solver == SORTED_NAIVE ) {
		if( !sorted_naive() ) {
			return false;
		}
	}
	else {
		if( !naive() ) {
			return false;
		}
	}

	if( graph_solver == BFS_GRAPH ) {
		BFS();
	}
	else {
		disjointSetFind();
	}
	return true;
}

void PlanarIntersections::solveWithTime()
{
	clock_t tStart = clock();
	if( ! solve() ) {
		std::cout<<"Not enough precision needed for calculations!"<<std::endl;
		return;
	}

	std::cout<<"Time taken: "<<((double)(clock() - tStart))/CLOCKS_PER_SEC<<std::endl;
}

void PlanarIntersections::printResults() const
{
	for( auto& s : segments )
		s.printInfo();
}


void PlanarIntersections::addSquare(int x, int y)
{
	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(5, 5));
	rectangle.setFillColor(sf::Color::Red);
	rectangle.setPosition(x - 2 , y - 2);
	squares.push_back(rectangle);
}






bool PlanarIntersections::naive()
{
	double x,y;
	for( int i = 0; i < segments.size() ; ++i)
	{
		for( int j = i+1; j < segments.size(); ++j )
		{
			if( segments[i].intersects( segments[j], x, y ) ) {
				addSquare(x, y);
				segments[i].connect( segments[j] );
				segments[j].connect( segments[i] );
			}
		}
	}
	return true;
}


bool PlanarIntersections::sorted_naive()
{
	std::vector<Point> points;
	std::list<int> opened_segments;
	for( auto& s : segments )
	{
		points.push_back( s.getBeginning() );
		points.push_back( s.getEnd() );
	}

	std::sort( points.begin(), points.end(), Point::cmp_point() );
	
	for( auto& p : points )
	{
		double x,y;
		int i = p.getOwner();
		if( p.getType() == BEGINNING ) {
			for( int j : opened_segments ) {
				if( segments[i].intersects( segments[j], x, y ) ) {
					addSquare( x, y);
					segments[i].connect( segments[j] );
					segments[j].connect( segments[i] );
				}
			}
			opened_segments.push_back(i);
		}
		else {
			opened_segments.remove( i );
		}
	}
	return true;
}


bool PlanarIntersections::computeBeginningPoint(std::set<Point, Point::cmp_point >& event_queue, 
			std::set<Segment*, Segment::cmp_ptr>& segments_tree, Point& p)
{	
	typedef std::set<Segment*, Segment::cmp_ptr>::iterator seg_itr;	
	Segment *curr_segm = &(segments[p.getOwner()]);
	seg_itr predecessor = segments_tree.end();
	seg_itr sucessor = segments_tree.end();

	seg_itr curr_itr = segments_tree.insert( curr_segm ).first;
	if( curr_itr != segments_tree.begin() ){
		predecessor = std::prev(curr_itr);
	}
	sucessor = std::next(curr_itr);
	double x,y;
	if( predecessor != segments_tree.end() && sucessor != segments_tree.end() && (*predecessor)->intersects( *sucessor, x, y ) ){
		event_queue.erase( Point(x, y, CROSS, (*predecessor)->getData(), (*sucessor)->getData() ) );
	}

	if( predecessor != segments_tree.end() && (*predecessor)->intersects( *curr_itr, x, y ) ) {
		if( !event_queue.insert( Point(x, y, CROSS, (*predecessor)->getData(), (*curr_itr)->getData() ) ).second )
			return false;	
	}

	if( sucessor != segments_tree.end() && (*sucessor)->intersects( *curr_itr, x, y) ) {
		if( !event_queue.insert( Point(x, y, CROSS, (*curr_itr)->getData(), (*sucessor)->getData() ) ).second )
			return false;
	}
	return true;
}


bool PlanarIntersections::computeEndPoint(std::set<Point, Point::cmp_point >& event_queue, 
			std::set<Segment*, Segment::cmp_ptr>& segments_tree, Point& p)
{
	typedef std::set<Segment*, Segment::cmp_ptr>::iterator seg_itr;	
	Segment *curr_segm = &(segments[p.getOwner()]);
	seg_itr predecessor = segments_tree.end();
	seg_itr sucessor = segments_tree.end();

	seg_itr curr_itr = segments_tree.insert( curr_segm ).first;
	if( curr_itr != segments_tree.begin() ){
		predecessor = std::prev(curr_itr);
	}
	sucessor = std::next(curr_itr);
	double x,y;
	if( predecessor != segments_tree.end() && sucessor != segments_tree.end() ) {
		if( (*predecessor)->intersects( *sucessor, x, y ) ) {
			if( !smaller( x, p.x ) )
				if( !event_queue.insert( Point(x, y, CROSS, (*predecessor)->getData(), (*sucessor)->getData() ) ).second ) 
					return false;
		}
	}
	segments_tree.erase( curr_itr );
	return true;
}


bool PlanarIntersections::computeCrossingPoint(std::set<Point, Point::cmp_point >& event_queue, 
			std::set<Segment*, Segment::cmp_ptr>& segments_tree, Point& p)
{
	typedef std::set<Segment*, Segment::cmp_ptr>::iterator seg_itr;	
	Segment *curr_segm = &(segments[p.getOwner()]);
	seg_itr predecessor = segments_tree.end();
	seg_itr sucessor = segments_tree.end();

	int s1 = p.getOwner();
	int s2 = p.getIntersection();
	segments[s1].connect(segments[s2]);
	segments[s2].connect(segments[s1]);

	auto s1_itr = segments_tree.find( &segments[s1] );
	auto s2_itr = segments_tree.find( &segments[s2] );
	if( s1_itr == segments_tree.end() ) { 
		return false;
	}
	if( s2_itr == segments_tree.end() ) {
		return false;
	}

	if( s1_itr != segments_tree.begin() ) {
		predecessor = s1_itr;
		--predecessor;
	}
	sucessor = s2_itr;
	++sucessor;

	double x,y;
	segments[s1].intersects( segments[s2], x, y);
	addSquare(x, y);
	if( sucessor != segments_tree.end() && (*sucessor)->intersects( *s2_itr, x, y) ){
		event_queue.erase( Point(x, y, CROSS, (*s2_itr)->getData(), (*sucessor)->getData() ) );
	}

	if( predecessor != segments_tree.end() && (*predecessor)->intersects( *s1_itr, x, y) ){
		event_queue.erase( Point(x, y, CROSS, (*predecessor)->getData(), (*s1_itr)->getData() ) );
	}

	if( sucessor != segments_tree.end() && (*sucessor)->intersects( *s1_itr, x, y) ){
		if( !smaller( x, p.x) ) {
			if( !event_queue.insert( Point(x, y, CROSS, (*s1_itr)->getData(), (*sucessor)->getData() ) ).second ) {
				return false;
			}
		}
	}

	if( predecessor != segments_tree.end() && (*predecessor)->intersects( *s2_itr, x, y) ) {
		if( !smaller(x , p.x) ) {
			if( !event_queue.insert( Point(x, y, CROSS, (*predecessor)->getData(), (*s2_itr)->getData() ) ).second ) {
				return false;
			}
		}
	}

	if( segments[s1].isVertical() )
		segments[s1].setSpecialIntersection( segments[s2].sweepLineIntersection() );

	if( segments[s2].isVertical() )
		segments[s2].setSpecialIntersection( segments[s1].sweepLineIntersection() );

	segments[s1].swap( segments[s2] );
	return true;
}

bool PlanarIntersections::OttmanBentley()
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
		auto p = *(event_queue.begin());
		Segment *curr_segm = &(segments[p.getOwner()]);
		seg_itr predecessor = segments_tree.end();
		seg_itr sucessor = segments_tree.end();
		Segment::setSweepLine( p );
		event_queue.erase( p );
		if(p.getType() == BEGINNING ) {
			if( !computeBeginningPoint( event_queue, segments_tree, p) ) {
				return false;
			}
		}
		else if( p.getType() == END ){
			if( !computeEndPoint( event_queue, segments_tree, p) ) {
				return false;
			}
		}
		else {
			if( !computeCrossingPoint( event_queue, segments_tree, p) ) {
				return false;
			}
		}
	}
	return segments_tree.empty();
}

void PlanarIntersections::BFS()
{
	std::vector<bool> visited(segments.size(), false);
	int group_index = -1;
	std::queue<SegmentData*> group;
	SegmentData* s;
	int index;
	for(unsigned int i = 0; i < segments.size(); ++i)
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


void PlanarIntersections::disjointSetFind()
{
	DisjointSet dset( segments.size() );
	for( auto& s : segments )
	{
		auto& neighbours = s.getNeighbours();
		for( auto& n : neighbours )
		{
			dset.makeUnion( s.getIndex(), n->getIndex() );
		}
	}

	auto& groups = dset.getGroups();
	for( int i = 0; i < groups.size(); ++i )
	{
		segments[i].setGroup( groups[i] );
	}
}


