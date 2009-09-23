/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
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


boost::shared_ptr< ::fwData::Graph > buildGraph2Node()
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
	::boost::shared_ptr< String > o1( new ::fwData::String() );
	o1->value()="J aime le rouge";
	n1->setObject(o1);


	::boost::shared_ptr< Node > n2( new Node() );
	::boost::shared_ptr< Port > p2( new Port() );
	p2->setIdentifier("threshold");
	p2->setType("float");
	n2->addInputPort( p2 );
	::boost::shared_ptr< ::fwData::Color > o2( new ::fwData::Color() );
	o2->getRefRGBA()[0]=1.0;
	n2->setObject(o2);

	::boost::shared_ptr< Edge > e( new Edge() );
	e->setIdentifiers("sizex","threshold");

	g->addNode(n1);
	g->addNode(n2);
	g->addEdge(e,n1,n2);

	return g;

}


boost::shared_ptr< ::fwData::Node > buildNode()
{
	::boost::shared_ptr< ::fwData::Node > n1( new ::fwData::Node() );
	::boost::shared_ptr< ::fwData::Port > p1( new ::fwData::Port() );
	p1->setIdentifier("sizexXX");
	p1->setType("float");
	n1->addOutputPort( p1 );
	::boost::shared_ptr< ::fwData::Float > o1( new ::fwData::Float() );
	o1->value()=3.1415;
	n1->setObject(o1);

	return n1;
}



int main()
{
	::boost::shared_ptr< ::fwData::Graph > g =  buildGraph2Node();
	::boost::shared_ptr< ::fwData::Node > n1 =  buildNode();
	::fwDataIO::Serializer serializer;
	serializer.rootFolder() = boost::filesystem::path("./"); // current path
	serializer.serialize(g,false);
	serializer.serialize(n1,false);
}

