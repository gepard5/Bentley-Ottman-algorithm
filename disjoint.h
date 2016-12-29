#include <vector>

class DisjointSet
{
public:
	DisjointSet( int );

	void makeSet( int );
	int find( int );
	void makeUnion( int, int);

	std::vector<int>& getGroups();

private:
	std::vector<int> parents;
	std::vector<int> ranks;
};
