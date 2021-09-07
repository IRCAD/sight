/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "GraphTest.hpp"

#include <data/Edge.hpp>
#include <data/Graph.hpp>
#include <data/Node.hpp>
#include <data/Port.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::GraphTest);

namespace sight::data
{

namespace ut
{

//------------------------------------------------------------------------------

void GraphTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void GraphTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void GraphTest::normalCase()
{
    // G :
    // n1 --e--> n2
    data::Graph::sptr g(data::Graph::New());

    data::Node::sptr n1(data::Node::New());
    data::Port::sptr p1(data::Port::New());
    p1->setIdentifier("sizex");
    p1->setType("float");
    n1->addOutputPort(p1);

    data::Node::sptr n2(data::Node::New());
    data::Port::sptr p2(data::Port::New());
    p2->setIdentifier("threshold");
    p2->setType("float");
    n2->addInputPort(p2);

    data::Edge::sptr e(data::Edge::New());
    e->setIdentifiers("sizex", "threshold");

    g->addNode(n1);
    g->addNode(n2);
    g->addEdge(e, n1, n2);

    CPPUNIT_ASSERT_EQUAL((size_t) 0, g->getInputEdges(n1).size());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getOutputEdges(n1).size());
    CPPUNIT_ASSERT_EQUAL(e, g->getOutputEdges(n1).at(0));
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getInputEdges(n2).size());
    CPPUNIT_ASSERT_EQUAL(e, g->getInputEdges(n2).at(0));
    CPPUNIT_ASSERT_EQUAL((size_t) 0, g->getOutputEdges(n2).size());

    CPPUNIT_ASSERT_EQUAL(n1, g->getSourceNode(e));
    CPPUNIT_ASSERT_EQUAL(n2, g->getDestinationNode(e));

    // Remove to obtain empty graph
    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());
    g->removeEdge(e);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 0, g->getNbEdges());
    g->removeNode(n1);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 0, g->getNbEdges());
    g->removeNode(n2);
    CPPUNIT_ASSERT_EQUAL((size_t) 0, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 0, g->getNbEdges());
}

//------------------------------------------------------------------------------

void GraphTest::limitCase1()
{
    // G :
    // n1 --e1--> n2
    data::Graph::sptr g(data::Graph::New());

    data::Node::sptr n1(data::Node::New());
    data::Port::sptr p1(data::Port::New());
    p1->setIdentifier("sizex");
    p1->setType("float");
    n1->addOutputPort(p1);

    data::Node::sptr n2(data::Node::New());
    data::Port::sptr p2(data::Port::New());
    p2->setIdentifier("threshold");
    p2->setType("float");
    n2->addInputPort(p2);

    data::Edge::sptr e1(data::Edge::New());
    e1->setIdentifiers("sizex", "threshold");

    bool success;

    CPPUNIT_ASSERT_EQUAL((size_t) 0, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 0, g->getNbEdges());

    success = g->addNode(n1);
    CPPUNIT_ASSERT_EQUAL(success, true);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 0, g->getNbEdges());

    success = g->addNode(n1);
    CPPUNIT_ASSERT_EQUAL(success, false);
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 0, g->getNbEdges());

    success = g->addNode(n2);
    CPPUNIT_ASSERT_EQUAL(success, true);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 0, g->getNbEdges());

    success = g->addEdge(e1, n1, n2);
    CPPUNIT_ASSERT_EQUAL(success, true);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());

    success = g->addEdge(e1, n1, n2);
    CPPUNIT_ASSERT_EQUAL(success, false);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());

    data::Node::sptr n3(data::Node::New());
    data::Port::sptr p3(data::Port::New());
    p3->setIdentifier("sizex");
    p3->setType("float");
    n3->addOutputPort(p3);

    success = g->addEdge(e1, n3, n2);
    CPPUNIT_ASSERT_EQUAL(success, false);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());

    success = g->addEdge(e1, n1, n3);
    CPPUNIT_ASSERT_EQUAL(success, false);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());

    data::Edge::sptr e2(data::Edge::New());
    e2->setIdentifiers("sizex", "BADID");

    success = g->addEdge(e2, n1, n2);
    CPPUNIT_ASSERT_EQUAL(success, false);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());

    data::Edge::sptr e3(data::Edge::New());
    e3->setIdentifiers("BADID", "threshold");

    success = g->addEdge(e3, n1, n2);
    CPPUNIT_ASSERT_EQUAL(success, false);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());

    data::Node::sptr n4(data::Node::New());
    data::Port::sptr p4(data::Port::New());
    p4->setIdentifier("threshold");
    p4->setType("BADTYPE");
    n4->addInputPort(p4);

    success = g->addNode(n4);
    CPPUNIT_ASSERT_EQUAL(success, true);
    CPPUNIT_ASSERT_EQUAL((size_t) 3, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());

    // G :
    // n1, n2, n4
    // e1 : n1 -> n2

    data::Edge::sptr e4(data::Edge::New());
    e4->setIdentifiers("sizex", "threshold");

    success = g->addEdge(e4, n1, n4);
    CPPUNIT_ASSERT_EQUAL(success, false);
    CPPUNIT_ASSERT_EQUAL((size_t) 3, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());
}

//------------------------------------------------------------------------------

void GraphTest::limitCase2()
{
    // G :
    // n1 --e1--> n2
    using namespace sight::data;
    data::Graph::sptr g(data::Graph::New());

    data::Node::sptr n1(data::Node::New());
    data::Port::sptr p1(data::Port::New());
    p1->setIdentifier("sizex");
    p1->setType("float");
    n1->addOutputPort(p1);
    g->addNode(n1);

    data::Node::sptr n2(data::Node::New());
    data::Port::sptr p2(data::Port::New());
    p2->setIdentifier("threshold");
    p2->setType("float");
    n2->addInputPort(p2);
    g->addNode(n2);

    data::Edge::sptr e1(data::Edge::New());
    e1->setIdentifiers("sizex", "threshold");
    g->addEdge(e1, n1, n2);

    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());

    bool success;
    success = g->removeNode(n1);
    CPPUNIT_ASSERT_EQUAL(success, false);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());

    data::Edge::sptr e2(data::Edge::New());
    e2->setIdentifiers("sizex", "threshold");

    success = g->removeEdge(e2);
    CPPUNIT_ASSERT_EQUAL(success, false);
    CPPUNIT_ASSERT_EQUAL((size_t) 2, g->getNbNodes());
    CPPUNIT_ASSERT_EQUAL((size_t) 1, g->getNbEdges());

    data::Node::sptr nullNode;
    CPPUNIT_ASSERT_EQUAL(g->getSourceNode(e2), nullNode);
    CPPUNIT_ASSERT_EQUAL(g->getDestinationNode(e2), nullNode);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::data
