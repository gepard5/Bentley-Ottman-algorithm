#include "disjoint.h"

DisjointSet::DisjointSet( int n ) : parents( std::vector<int>( n, 0 ) ), ranks( std::vector<int>( n, 0) )
{
	for( int i = 0; i < n; ++i )
	{
		parents[i] = i;
	}
}

void DisjointSet::makeSet( int x )
{
	parents[x] = x;
	ranks[x] = 0;
}

void DisjointSet::makeUnion( int x, int y )
{
	int xRoot = find(x);
	int yRoot = find(y);

	if( xRoot == yRoot ) 
		return;

	if( ranks[xRoot] < ranks[yRoot] ) {
		parents[xRoot] = yRoot;
		return;
	}
	else if( ranks[xRoot] > ranks[yRoot] ) {
		parents[yRoot] = xRoot;
		return;
	}
	else {
		parents[yRoot] = xRoot;
		++ranks[xRoot];
	}
}

int DisjointSet::find( int x )
{
	if( parents[x] != x ) {
		parents[x] = find(parents[x]);
	}
	return parents[x];
}

std::vector<int>& DisjointSet::getGroups()
{
	for( int i = 0; i < parents.size(); ++i )
	{
		find( i );
	}
	return parents;
}