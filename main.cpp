/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
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
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <random>
#include <vector>
#include <queue>
#include <set>
#include <list>
#include <iterator>
#include <time.h>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


#include "planarIntersections.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

void solveRandomizedTests(PlanarIntersections& test, double size, int number, int leng, int iterations)
{
	std::vector<double> solving_times(iterations);
	std::vector<long> complexities(iterations);
	double step_size = size;
	int step_number = number;
	for( int i = 0; i < iterations; ++i )
	{
		test.setMax( step_size );
		test.generateSegments( step_number, leng );
		solving_times[i] = test.solveWithTime();
		complexities[i] = test.getComplexity(step_number);
		step_size += size * 0.3;
		step_number += number;
	}

	double solve_median = solving_times[iterations/2];
	double complexity_median = complexities[iterations/2];
	
	std::cout<<std::setw(12);
	std::cout<<"t(n)"<<std::setw(12)<<"T(n)"<<std::setw(12)<<"q(n)"<<std::endl;
	for( int i = 0; i < iterations; ++i )
		std::cout<<std::setw(12)<<solving_times[i]<<std::setw(12)<<complexities[i]<<std::setw(12)<<solving_times[i] * complexity_median / ( solve_median * complexities[i] ) << std::endl;
}
/* 
 * main function, parses command line arguments and solves a task
 * */
int main(int ac, char *av[])
{
	int number = 1000;
	double size = 1000;
	double leng = 50;
	PlanarIntersections test;

	//initialize command line parser
	po::variables_map vm;
	po::options_description desc("Allowed options");
	try
	{
		desc.add_options()
			("help,h", 								"produce help message")
			("number,n", 	po::value<int>(), 		"number of segments" )
			("size,s", 		po::value<double>(), 	"size of a plane")
			("read,r",								"read data from standard input")
			("length,l", 	po::value<double>(),	"maximal length of segment")
			("random", 		po::value<int>(),		"make specified number of tests with random data, each one with more segments and bigger plane")
			("visualize,v",							"show visualization of segments")
			("draw_squares",						"draw square on each intersection")
			("ottman_bentley", 						"use Ottman-Bentley algorithm")
			("naive",								"use naive algorithm")
			("naive_sorted",						"use naive pre-sorted algorithm")
			("BFS",									"use BFS algorithm")
			("disjoint_set",						"use disjoint-set algorithm")
			("dense",								"indicate data will be hard, complexity of algorithms might change")
		;

		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);
	}
	catch( po::error& e)
	{
		std::cout<<e.what()<<std::endl;
		std::cout<<desc<<std::endl;
		return 0;
	}
	
	//print help
	if ( vm.count("help") ) {
		std::cout<<desc<<std::endl;
		return 0;
	}

	//set segments number
	if( vm.count("number") ) {
		number = vm["number"].as<int>();
	}

	//set plane size
	if( vm.count("size") ) {
		size = vm["size"].as<double>();
	}
	test.setMax( size );

	//set maximal segment length
	if( vm.count("length") ) {
		leng = vm["length"].as<double>();
	}

	if( vm.count("dense") ) {
		test.setDenseData(true);
	}

	//read segments from input
	if( vm.count("read") ) {
		std::cout<<"Specify number of segments"<<std::endl;
		std::cin>>number;
		std::cout<<"Write segments data: x1 y1 x2 y2"<<std::endl;
		for( int i = 0; i < number; ++i )
		{
			double x1, y1, x2, y2;
			std::cin>>x1>>y1>>x2>>y2;
			test.addSegment( x1, y1, x2, y2 );
		}
	}
	else {
		test.generateSegments( number, leng );
	}

	//set solver for intersections
	if( vm.count("ottman_bentley") ) {
		test.setIntersectionSolver( OTTMAN );
		std::cout<<"Chosen Ottman-Bentley algorithm for solving intersections."<<std::endl;
	}
	else if( vm.count("naive_sorted") ) {
		test.setIntersectionSolver( SORTED_NAIVE );
		std::cout<<"Chosen naive pre-sorted algorithm for solving intersections."<<std::endl;
	}
	else {
		test.setIntersectionSolver( NAIVE );
		std::cout<<"Chosen naive algorithm for solving intersections."<<std::endl;
	}

	//set solver for graphs
	if( vm.count("disjoint_set") ) {
		test.setGraphSolver( DISJOINT_SET );
		std::cout<<"Chosen disjoint-sets for solving graph algorithm."<<std::endl;
	}
	else {
		test.setGraphSolver( BFS_GRAPH );
		std::cout<<"Chosen BFS for solving graph algorithm."<<std::endl;
	}

	//create random data and test program
	//creating more segments in each step
	if( vm.count("random") ) {
		int iterations = vm["random"].as<int>();
		solveRandomizedTests(test, size, number, leng, iterations );
	}
	//else solve problem and prints its time
	else {
		double time = test.solveWithTime();
		if( time == -1 ) {
			std::cout<<"Too much precision needed to finish calculations"<<std::endl;
		}
		else {
			std::cout<<"Time taken: "<<time<<std::endl;
		}
	}


	//set drawing squares on visualization
	if( vm.count("draw_squares") ) {
		test.setDrawSquares( true );
	}

	//visualize results
	if( vm.count("visualize") ) {
		test.visualize();
	}

	return 0;
}

