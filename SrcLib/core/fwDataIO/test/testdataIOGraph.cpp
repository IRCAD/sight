/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Graph.hpp>
#include <fwData/Node.hpp>
#include <fwData/Edge.hpp>
#include <fwData/Port.hpp>
#include <fwData/String.hpp>
#include <fwData/Float.hpp>
#include <fwData/Color.hpp>
#include "fwDataIO/Serializer.hpp"


::fwData::Graph::sptr buildGraph2Node()
{
    // G :
    // n1 --e--> n2
    ::fwData::Graph::NewSptr g;

    ::fwData::Node::NewSptr n1;
    ::fwData::Port::NewSptr p1;
    p1->setIdentifier("sizex");
    p1->setType("float");
    n1->addOutputPort( p1 );
    ::fwData::String::NewSptr o1;
    o1->value()="J aime le rouge";
    n1->setObject(o1);


    ::fwData::Node::NewSptr n2;
    ::fwData::Port::NewSptr p2;
    p2->setIdentifier("threshold");
    p2->setType("float");
    n2->addInputPort( p2 );
    ::fwData::Color::NewSptr o2;
    o2->getRefRGBA()[0]=1.0;
    n2->setObject(o2);

    ::fwData::Edge::NewSptr e;
    e->setIdentifiers("sizex","threshold");

    g->addNode(n1);
    g->addNode(n2);
    g->addEdge(e,n1,n2);

    return g;

}


::fwData::Node::sptr buildNode()
{
    ::fwData::Node::NewSptr n1;
    ::fwData::Port::NewSptr p1;
    p1->setIdentifier("sizexXX");
    p1->setType("float");
    n1->addOutputPort( p1 );
    ::fwData::Float::NewSptr o1;
    o1->value()=3.1415;
    n1->setObject(o1);

    return n1;
}



int main()
{
    ::fwData::Graph::NewSptr g;
    ::fwData::Node::NewSptr n1;
    ::fwDataIO::Serializer serializer;
    serializer.rootFolder() = ::boost::filesystem::path("./"); // current path
    serializer.serialize(g,false);
    serializer.serialize(n1,false);
}

