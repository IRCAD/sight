/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <utility>
#include <boost/foreach.hpp>

#include "fwData/registry/macros.hpp"
#include <fwData/Factory.hpp>

#include "fwData/Edge.hpp"
#include "fwData/Port.hpp"
#include "fwData/Node.hpp"
#include "fwData/Graph.hpp"

fwDataRegisterMacro( ::fwData::Graph );

namespace fwData
{

const bool Graph::UP_STREAM = true;
const bool Graph::DOWN_STREAM = false;

//------------------------------------------------------------------------------

Graph::Graph()
{}

//------------------------------------------------------------------------------

Graph::~Graph()
{}

//------------------------------------------------------------------------------

bool Graph::addNode( Node::sptr node)
{
    return m_nodes.insert(node).second;
}

//------------------------------------------------------------------------------

bool Graph::removeNode( Node::csptr node)
{
    // test if connected edge to it
    if ( haveConnectedEdges(node) )
    {
        return false;
    }
    return (m_nodes.erase( Node::constCast(node) ) > 0 );
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

bool Graph::haveConnectedEdges(Node::csptr node ) const
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
        Node::csptr nodeSource,
        std::string nodeSourceOutputPortID,
        Node::csptr nodeDestination,
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

bool Graph::addEdge(Edge::sptr edge, Node::csptr nodeSource, Node::csptr nodeDestination)
{
    // edge not already recorded
    if (m_connections.find( edge ) !=  m_connections.end() )
    {
        return false; // edge already stored
    }
    // node registred ?
    if (m_nodes.find( Node::constCast(nodeSource) ) ==  m_nodes.end() )
    {
        return false; // node already stored
    }
    // node registred ?
    if ( m_nodes.find( Node::constCast(nodeDestination) ) ==  m_nodes.end() )
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

    m_connections[ edge ] = std::make_pair(  Node::constCast(nodeSource), Node::constCast(nodeDestination) );

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

std::vector< Edge::sptr > Graph::getInputEdges( Node::csptr node )
{
    return getEdges( node, UP_STREAM );
}

//------------------------------------------------------------------------------

std::vector< Edge::sptr > Graph::getOutputEdges( Node::csptr node )
{
    return getEdges( node, DOWN_STREAM );
}

//------------------------------------------------------------------------------

std::vector< Edge::sptr > Graph::getEdges( Node::csptr node, bool upStream, std::string nature , std::string portID)
{
    OSLM_ASSERT("Node "<<node->getID() <<" not found in graph", m_nodes.find( Node::constCast(node) ) != m_nodes.end() );
    OSLM_ASSERT("Port "<< portID <<" not found in graph", portID.empty() || node->findPort(portID,upStream) ); // portID if specified must be on node

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
        ::fwData::Node::csptr node,
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

Graph::ConnectionContainer &Graph::getRefConnections()
{
    return m_connections;
}

//------------------------------------------------------------------------------

void Graph::shallowCopy( Graph::csptr _source )
{
    ::fwData::Object::shallowCopyOfChildren(_source );
    m_nodes = _source->m_nodes;
    m_connections = _source->m_connections;
}

//------------------------------------------------------------------------------

void Graph::deepCopy( Graph::csptr _source )
{
    ::fwData::Object::deepCopyOfChildren(_source );

    std::map< ::fwData::Node::sptr, ::fwData::Node::sptr > correspondenceBetweenNodes;
    typedef std::pair< Edge::sptr,  std::pair<  Node::sptr,  Node::sptr > > ConnectionContainerElt;

    m_nodes.clear();
    BOOST_FOREACH(::fwData::Node::sptr node, _source->m_nodes)
    {
        ::fwData::Node::NewSptr newNode;
        newNode->deepCopy( Node::constCast(node) );
        bool addOK =this->addNode(newNode);
        OSLM_ASSERT("Node "<<newNode->getID() <<" can't be deepCopy ", addOK );
        correspondenceBetweenNodes.insert(std::make_pair(node, newNode));
    }

    m_connections.clear();
    BOOST_FOREACH(ConnectionContainerElt connection, _source->m_connections)
    {
        // Edge deep copy .
        ::fwData::Edge::NewSptr newEdge;
        newEdge->deepCopy( connection.first );
        ::fwData::Node::sptr oldNode1 = (connection.second).first;
        ::fwData::Node::sptr oldNode2 = (connection.second).second;
        if ((correspondenceBetweenNodes.find(Node::constCast(oldNode1))!= correspondenceBetweenNodes.end())
             && (correspondenceBetweenNodes.find(Node::constCast(oldNode2)) != correspondenceBetweenNodes.end()))
        {
            // Add new Edge
            this->addEdge(newEdge, correspondenceBetweenNodes[oldNode1], correspondenceBetweenNodes[oldNode2]);
        }
    }
    correspondenceBetweenNodes.clear();
}

//------------------------------------------------------------------------------

} // namespace fwData
