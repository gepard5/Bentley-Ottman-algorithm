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
#include <list>
#include <iterator>
#include <time.h>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>


#include "planarIntersections.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;



int main(int ac, char *av[])
{
	int number = 1000;
	double size = 1000;
	double leng = 50;
	PlanarIntersections test;
	po::variables_map vm;
	po::options_description desc("Allowed options");
	try
	{
		desc.add_options()
			("help,h", 			"produce help message")
			("number,n", po::value<int>(), 		"number of segments" )
			("size,s", po::value<double>(), 			"size of a plane")
			("read,r",		"read data from standard input")
			("length,l", po::value<double>(),		"average length of segment")
			("random", po::value<int>(),		"make specified number of tests with random data, each one with two times more segments and two times bigger plane")
			("visualize,v",			"show visualization of segments")
			("draw_squares",		"draw square on each intersection")
			("ottman_bentley", 		"use Ottman-Bentley algorithm")
			("naive",				"use naive algorithm")
			("naive_sorted",		"use naive pre-sorted algorithm")
			("BFS",					"use BFS algorithm")
			("disjoint_set",		"use disjoint-set algorithm")
		;

		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);
	}
	catch( po::error& e)
	{
		std::cout<<e.what()<<std::endl;
		return 0;
	}
	if ( vm.count("help") ) {
		std::cout<<desc<<std::endl;
		return 0;
	}

	if( vm.count("number") ) {
		number = vm["number"].as<int>();
	}

	if( vm.count("size") ) {
		size = vm["size"].as<double>();
	}
	test.setMax( size );

	if( vm.count("length") ) {
		leng = vm["length"].as<double>();
	}

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

	if( vm.count("disjoint_set") ) {
		test.setGraphSolver( DISJOINT_SET );
		std::cout<<"Chosen disjoint-sets for solving graph algorithm."<<std::endl;
	}
	else {
		test.setGraphSolver( BFS_GRAPH );
		std::cout<<"Chosen BFS for solving graph algorithm."<<std::endl;
	}

	if( vm.count("random") ) {
		int iterations = vm["random"].as<int>();
		double step_size = size;
		int step_number = number;
		for( int i = 0; i < iterations; ++i )
		{
			test.setMax( step_size );
			test.generateSegments( step_number, leng );
			test.solveWithTime();
			step_size += size * 0.3;
			step_number += number;
		}
	}
	else {
		test.solveWithTime();
	}

	if( vm.count("draw_squares") ) {
		test.setDrawSquares( true );
	}

	if( vm.count("visualize") ) {
		test.visualize();
	}

}

