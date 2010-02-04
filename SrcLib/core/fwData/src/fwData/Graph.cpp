/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>
#include "fwData/Edge.hpp"
#include "fwData/Port.hpp"
#include "fwData/Node.hpp"

#include "fwData/Graph.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Graph,  ::fwData::Graph);
namespace fwData {

const bool Graph::UP_STREAM = true;
const bool Graph::DOWN_STREAM = false;

//------------------------------------------------------------------------------

Graph::Graph()
{
	// TODO Auto-generated constructor stub
}

//------------------------------------------------------------------------------

Graph::~Graph()
{
	// TODO Auto-generated destructor stub
}

//------------------------------------------------------------------------------

bool Graph::addNode( Node::sptr node)
{
	return m_nodes.insert(node).second;
}

//------------------------------------------------------------------------------

bool Graph::removeNode( Node::sptr node)
{
	// test if connected edge to it
	if ( haveConnectedEdges(node) )
	{
		return false;
	}
	return (m_nodes.erase(node) > 0 );
}

//------------------------------------------------------------------------------

Graph::NodeContainer &Graph::getRefNodes()
{
	return m_nodes;
}

//------------------------------------------------------------------------------

const Graph::NodeContainer &Graph::getCRefNodes() const
{
	return m_nodes;
}

//------------------------------------------------------------------------------

bool Graph::haveConnectedEdges(Node::sptr node ) const
{
	for ( ConnectionContainer::const_iterator i=m_connections.begin() ; i !=  m_connections.end() ; ++i )
	{
		if ( i->second.first == node ||  i->second.second == node)
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------

Edge::sptr Graph::makeConnection(
		Node::sptr nodeSource,
		std::string nodeSourceOutputPortID,
		Node::sptr nodeDestination,
		std::string nodeDestinationInputPortID,
		std::string EdgeNature )
{
	::fwData::Edge::NewSptr nEdge;
	nEdge->setIdentifiers( nodeSourceOutputPortID, nodeDestinationInputPortID );
	nEdge->setNature( EdgeNature );
	if ( addEdge( nEdge, nodeSource, nodeDestination ) )
	{
		  return nEdge;// success return new Edge
	}
	else
	{
		return ::fwData::Edge::sptr(); // failure
	}
}

//------------------------------------------------------------------------------

bool Graph::addEdge(Edge::sptr edge, Node::sptr nodeSource, Node::sptr nodeDestination)
{
	// edge not already recorded
	if (m_connections.find( edge ) !=  m_connections.end() )
	{
		return false; // edge already stored
	}
	// node registred ?
	if (m_nodes.find( nodeSource ) ==  m_nodes.end() )
	{
		return false; // node already stored
	}
	// node registred ?
	if ( m_nodes.find( nodeDestination ) ==  m_nodes.end() )
	{
		return false; // node already stored
	}

	// test port existance
	Port::sptr sourcePort = nodeSource->findPort( edge->getIdentifiers().first, DOWN_STREAM );
	if ( !sourcePort )
	{
		return false; // port doesn't exist
	}

	// test port existance
	Port::sptr sourceDest= nodeDestination->findPort( edge->getIdentifiers().second , UP_STREAM );
	if ( !sourceDest )
	{
		return false; // port doesn't exist
	}


	// test port compatibility
	if ( sourcePort->getType() !=  sourceDest->getType() )
	{
		return false; // incompatible type
	}

	m_connections[ edge ] = std::make_pair(nodeSource,nodeDestination);

	return true;

}

//------------------------------------------------------------------------------

bool Graph::removeEdge(Edge::sptr edge)
{
	return ( m_connections.erase(edge) != 0 );
}

//------------------------------------------------------------------------------

Node::sptr Graph::getSourceNode(Edge::sptr edge)
{
	return getNode( edge, UP_STREAM );
}

//------------------------------------------------------------------------------

Node::sptr Graph::getDestinationNode( Edge::sptr edge )
{
	return getNode( edge, DOWN_STREAM );
}

//------------------------------------------------------------------------------

Node::sptr Graph::getNode( Edge::sptr edge, bool upStream )
{
	ConnectionContainer::iterator i;
	i = m_connections.find( edge );

	// check edge is valid ?
	if ( i ==  m_connections.end() )
	{
		return Node::sptr();
	}

	if( upStream )
	{
		return (*i).second.first;
	}
	else
	{
		return (*i).second.second;
	}
}

//------------------------------------------------------------------------------

std::vector< Edge::sptr > Graph::getInputEdges( Node::sptr node )
{
	return getEdges( node, UP_STREAM );
}

//------------------------------------------------------------------------------

std::vector< Edge::sptr > Graph::getOutputEdges( Node::sptr node )
{
	return getEdges( node, DOWN_STREAM );
}

//------------------------------------------------------------------------------

std::vector< Edge::sptr > Graph::getEdges( Node::sptr node, bool upStream, std::string nature , std::string portID)
{
	assert( m_nodes.find(node) != m_nodes.end() );
	assert( portID.empty() || node->findPort(portID,upStream) ); // portID if specified must be on node

	std::vector< Edge::sptr > result;
	result.reserve(4);

	for ( ConnectionContainer::const_iterator i=m_connections.begin() ; i !=  m_connections.end() ; ++i )
	{
		Edge::sptr edge = i->first;
		Node::sptr nodeFrom = i->second.first;
		Node::sptr nodeTo = i->second.second;

		bool isConnectedEdge = ( upStream ? ( nodeTo == node ) : ( nodeFrom == node ) );
		bool isCorrectNature =  nature.empty() || (  edge->getNature() == nature );
		bool isCorrectPort = portID.empty() ||
		          ( upStream ? ( edge->getToPortID()== portID ) : (  edge->getFromPortID()== portID ) );

		if ( isConnectedEdge && isCorrectNature && isCorrectPort)
		{
			result.push_back( edge );
		}
	}

	return result;
}

//------------------------------------------------------------------------------

std::vector< ::fwData::Node::sptr >
Graph::getNodes(
		::fwData::Node::sptr node,
		bool upStream,
		std::string nature,
		std::string portID )
{
	std::vector< Edge::sptr > edges;
	edges = getEdges( node, upStream, nature, portID);

	std::vector< Node::sptr > nodes;
	BOOST_FOREACH( Edge::sptr edge, edges )
	{
		Node::sptr distantNode;
		distantNode = ( upStream ? m_connections[edge].first : m_connections[edge].second );

		if( std::find( nodes.begin(), nodes.end(), distantNode ) == nodes.end() )
		{
			nodes.push_back(distantNode);
		}
	}

	return nodes;
}

//------------------------------------------------------------------------------

unsigned int Graph::getNbNodes() const
{
	return m_nodes.size();
}

//------------------------------------------------------------------------------

unsigned int Graph::getNbEdges() const
{
	return m_connections.size();
}

//------------------------------------------------------------------------------

const Graph::ConnectionContainer &Graph::getCRefConnections() const
{
	return m_connections;
}

//------------------------------------------------------------------------------

} // namespace fwData
