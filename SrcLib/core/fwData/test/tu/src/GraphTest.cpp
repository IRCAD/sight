/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>

#include "fwData/Graph.hpp"
#include "fwData/Node.hpp"
#include "fwData/Edge.hpp"
#include "fwData/Port.hpp"
#include "GraphTest.h"



// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( GraphTest );

//------------------------------------------------------------------------------

void GraphTest::setUp()
{
		// Set up context before running a test.
		//ptCubeDeTest =  new Cube();
}

//------------------------------------------------------------------------------

void GraphTest::tearDown()
{
		// Clean up after the test run.
		//delete ptCubeDeTest;
}

//------------------------------------------------------------------------------

void GraphTest::normalCase()
{
	// G :
	// n1 --e--> n2
	using namespace fwData;
	::boost::shared_ptr< Graph > g( new Graph() );

	::boost::shared_ptr< Node > n1( new Node() );
	::boost::shared_ptr< Port > p1( new Port() );
	p1->setIdentifier("sizex");
	p1->setType("float");
	n1->addOutputPort( p1 );

	::boost::shared_ptr< Node > n2( new Node() );
	::boost::shared_ptr< Port > p2( new Port() );
	p2->setIdentifier("threshold");
	p2->setType("float");
	n2->addInputPort( p2 );

	::boost::shared_ptr< Edge > e( new Edge() );
	e->setIdentifiers("sizex","threshold");

	g->addNode(n1);
	g->addNode(n2);
	g->addEdge(e,n1,n2);

	CPPUNIT_ASSERT_EQUAL( g->getInputEdges(n1).size() ,  (size_t)0 );
	CPPUNIT_ASSERT_EQUAL( g->getOutputEdges(n1).size() ,  (size_t)1 );
	CPPUNIT_ASSERT_EQUAL( g->getOutputEdges(n1).at(0),  e );
	CPPUNIT_ASSERT_EQUAL( g->getInputEdges(n2).size() ,  (size_t)1 );
	CPPUNIT_ASSERT_EQUAL( g->getInputEdges(n2).at(0),  e );
	CPPUNIT_ASSERT_EQUAL( g->getOutputEdges(n2).size() ,  (size_t)0 );

	CPPUNIT_ASSERT_EQUAL( g->getSourceNode(e), n1 );
	CPPUNIT_ASSERT_EQUAL( g->getDestinationNode(e), n2 );

	// Remove to obtain empty graph
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int)2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int)1 );
	g->removeEdge(e);
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int)2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int)0 );
	g->removeNode(n1);
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int)1 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int)0 );
	g->removeNode(n2);
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int)0 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int)0 );
}

//------------------------------------------------------------------------------

void GraphTest::limitCase1()
{
	// G :
	// n1 --e1--> n2
	using namespace fwData;
	::boost::shared_ptr< Graph > g( new Graph() );

	::boost::shared_ptr< Node > n1( new Node() );
	::boost::shared_ptr< Port > p1( new Port() );
	p1->setIdentifier("sizex");
	p1->setType("float");
	n1->addOutputPort( p1 );

	::boost::shared_ptr< Node > n2( new Node() );
	::boost::shared_ptr< Port > p2( new Port() );
	p2->setIdentifier("threshold");
	p2->setType("float");
	n2->addInputPort( p2 );

	::boost::shared_ptr< Edge > e1( new Edge() );
	e1->setIdentifiers("sizex","threshold");

	bool success;

	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 0 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 0 );

	success = g->addNode(n1);
	CPPUNIT_ASSERT_EQUAL( success ,  true );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 1 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 0 );

	success = g->addNode(n1);
	CPPUNIT_ASSERT_EQUAL( success ,  false );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 1 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 0 );

	success = g->addNode(n2);
	CPPUNIT_ASSERT_EQUAL( success ,  true );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 0 );


	success = g->addEdge(e1,n1,n2);
	CPPUNIT_ASSERT_EQUAL( success ,  true );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 1 );

	success = g->addEdge(e1,n1,n2);
	CPPUNIT_ASSERT_EQUAL( success ,  false );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 1 );

	::boost::shared_ptr< Node > n3( new Node() );
	::boost::shared_ptr< Port > p3( new Port() );
	p3->setIdentifier("sizex");
	p3->setType("float");
	n3->addOutputPort( p3 );

	success = g->addEdge(e1,n3,n2);
	CPPUNIT_ASSERT_EQUAL( success ,  false );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 1 );

	success = g->addEdge(e1,n1,n3);
	CPPUNIT_ASSERT_EQUAL( success ,  false );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 1 );

	::boost::shared_ptr< Edge > e2( new Edge() );
	e2->setIdentifiers("sizex","BADID");

	success = g->addEdge(e2,n1,n2);
	CPPUNIT_ASSERT_EQUAL( success ,  false );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 1 );

	::boost::shared_ptr< Edge > e3( new Edge() );
	e3->setIdentifiers("BADID","threshold");

	success = g->addEdge(e3,n1,n2);
	CPPUNIT_ASSERT_EQUAL( success ,  false );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 1 );

	::boost::shared_ptr< Node > n4( new Node() );
	::boost::shared_ptr< Port > p4( new Port() );
	p4->setIdentifier("threshold");
	p4->setType("BADTYPE");
	n4->addInputPort( p4 );

	success = g->addNode( n4 );
	CPPUNIT_ASSERT_EQUAL( success ,  true );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 3 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 1 );

	// G :
	// n1, n2, n4
	// e1 : n1 -> n2

	::boost::shared_ptr< Edge > e4( new Edge() );
	e4->setIdentifiers("sizex","threshold");

	success = g->addEdge(e4,n1,n4);
	CPPUNIT_ASSERT_EQUAL( success ,  false );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 3 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 1 );
}

void GraphTest::limitCase2()
{
	// G :
	// n1 --e1--> n2
	using namespace fwData;
	::boost::shared_ptr< Graph > g( new Graph() );

	::boost::shared_ptr< Node > n1( new Node() );
	::boost::shared_ptr< Port > p1( new Port() );
	p1->setIdentifier("sizex");
	p1->setType("float");
	n1->addOutputPort( p1 );
	g->addNode(n1);

	::boost::shared_ptr< Node > n2( new Node() );
	::boost::shared_ptr< Port > p2( new Port() );
	p2->setIdentifier("threshold");
	p2->setType("float");
	n2->addInputPort( p2 );
	g->addNode(n2);

	::boost::shared_ptr< Edge > e1( new Edge() );
	e1->setIdentifiers("sizex","threshold");
	g->addEdge(e1,n1,n2);

	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 1 );

	bool success;
	success = g->removeNode(n1);
	CPPUNIT_ASSERT_EQUAL( success ,  false );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 1 );

	::boost::shared_ptr< Edge > e2( new Edge() );
	e2->setIdentifiers("sizex","threshold");


	success = g->removeEdge(e2);
	CPPUNIT_ASSERT_EQUAL( success ,  false );
	CPPUNIT_ASSERT_EQUAL( g->getNbNodes(), (unsigned int) 2 );
	CPPUNIT_ASSERT_EQUAL( g->getNbEdges(), (unsigned int) 1 );

	::boost::shared_ptr<Node> nullNode;
	CPPUNIT_ASSERT_EQUAL( g->getSourceNode(e2), nullNode );
	CPPUNIT_ASSERT_EQUAL( g->getDestinationNode(e2), nullNode );



}

//------------------------------------------------------------------------------
