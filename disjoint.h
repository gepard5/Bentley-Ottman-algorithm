#include <vector>

/*
 * class realizing disjoint-set structure, used for finding connected components of graph
 */
class DisjointSet
{
public:
	/*
	 * defualt constructor
	 */
	DisjointSet( int );

	/*
	 * create new set for this element
	 */
	void makeSet( int );
	/*
	 * return root of set given element belongs to
	 * set parent of that element to root
	 */
	int find( int );
	/*
	 * connect two sets containing given elements into single one
	 * if elements are from the same set, don't do anything
	 */
	void makeUnion( int, int);

	/*
	 * return root of every points set
	 */
	std::vector<int>& getGroups();

private:
	/*
	 * vector holding root of every points set
	 */
	std::vector<int> parents;
	/*
	 * vector holding rank of every set, used for quicker computing
	 */ 
	std::vector<int> ranks;
};
