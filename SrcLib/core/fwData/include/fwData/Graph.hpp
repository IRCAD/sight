/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_GRAPH_HPP_
#define _FWDATA_GRAPH_HPP_

#include <map>
#include <set>

#include "fwData/config.hpp"

#include "fwData/Object.hpp"
#include "fwData/Edge.hpp"
#include "fwData/Node.hpp"

namespace fwData {
/**
 * @class   Graph
 * @brief   This class defines a graph object.
 *
 * A graph is represented by connections between Edge and Node.
 *
 * @see     ::fwData::Edge, ::fwData::Node
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */

class FWDATA_CLASS_API Graph : public ::fwData::Object {

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Graph)(::fwData::Object), (()), ::fwTools::Factory::New< Graph >) ;

    fwDataObjectMacro();

    FWDATA_API static const bool UP_STREAM;
    FWDATA_API static const bool DOWN_STREAM;

    typedef std::map< Edge::sptr,  std::pair<  Node::sptr,  Node::sptr > > ConnectionContainer;
    typedef std::set< Node::sptr >                                         NodeContainer;

    /// Constructor
    FWDATA_API Graph();

    /// Destructor
    FWDATA_API virtual ~Graph();

    /**
     * @brief add a node
     *
     * @return true on sucess( node not already in graph)
     */
    FWDATA_API bool addNode( Node::sptr _node);

    /**
     * @brief remove a node
     *
     * @return true on sucess
     */
    FWDATA_API bool removeNode( Node::csptr _node);

    /**
     * @{
     * @brief Get the node container
     */
    FWDATA_API NodeContainer &getRefNodes();
    FWDATA_API const NodeContainer &getCRefNodes() const;
    ///@}

    /**
     * @brief Get the connection container
     *
     * A connection is defined by an edge, a source node and a destination node.
     *
     * @return map<Edge, pair<source node, destination node> >
     */
    FWDATA_API const ConnectionContainer &getCRefConnections() const;
    FWDATA_API ConnectionContainer &getRefConnections();

    /**
     * @brief Add and edge
     *
     * @return true if success
     * @li nodes exist
     * @li edge is unique
     * @li port is compatible identifier & type
     */
    FWDATA_API bool addEdge(Edge::sptr _edge, Node::csptr _nodeSource, Node::csptr _nodeDestination);

    /**
     * @brief create an edge from given info and add edge in the graph
     *
     * @return new edge created if success else return null one
     */
    FWDATA_API
    Edge::sptr makeConnection(  Node::csptr _nodeSource,
                                std::string _nodeSourceOutputPortID,
                                Node::csptr _nodeDestination,
                                std::string _nodeDestinationInputPortID,
                                std::string _EdgeNature
                             );

    /**
     * @brief remove an edge
     *
     * @param[in] _edge edge to be remove
     * @return true iif success
     */
    FWDATA_API bool removeEdge(Edge::sptr _edge);

    /**
     * @brief Get source node
     *
     * @param[in] _edge edge associated with the node
     */
    FWDATA_API Node::sptr getSourceNode(Edge::sptr _edge);

    /**
     * @brief Get destination node
     *
     * @param[in] _edge edge associated with the node
     */
    FWDATA_API Node::sptr getDestinationNode(Edge::sptr _edge);

    /**
     * @brief Get node
     *
     * @param[in] _edge edge associated with the node
     * @_upStream if true return souce node, else return destination node
     *
     * @return source or destination node if edge exists, else return a new node
     */
    FWDATA_API Node::sptr getNode(Edge::sptr _edge, bool _upStream);

    /**
     * @brief Get input edges
     *
     * @param[in] _node source node
     * @return a vector of edges where _node is source node
     */
    FWDATA_API std::vector< Edge::sptr > getInputEdges(Node::csptr _node);

    /**
     * @brief Get output edges
     *
     * @param[in] _node destination node
     * @return a vector of  edges where _node is destination node
     */
    FWDATA_API std::vector< Edge::sptr > getOutputEdges(Node::csptr _node);

    /**
     * @brief Get a vector of edges
     *
     * @param[in] _node node associated with the edges
     * @param[in] _upStream if true _node is a source node, else _node is a destination node
     * @param[in] _nature if nature is an empty string : no check on nature of edge
     * @param[in] _portID if portID not specified (no check) : porID must be an BoolupStream port of given node
     *
     * @return the vector of all edges with correct nature and portID where _node is a source/destination node
     */
    FWDATA_API std::vector< Edge::sptr > getEdges(Node::csptr _node, bool _upStream, std::string _nature="",  std::string _portID="");

    /**
     * @brief Get a vector of nodes
     *
     * @param[in] _node node associated with the nodes
     * @param[in] _upStream if true _node is a source node, else _node is a destination node
     * @param[in] _nature if nature is an empty string : no check on nature of edge
     * @param[in] _portID if portID not specified (no check) : porID muts be an BoolupStream port of given node
     *
     * @return the vector of all nodes associated with _node
     */
    FWDATA_API  std::vector< ::fwData::Node::sptr > getNodes( ::fwData::Node::csptr node, bool upStream, std::string nature="",  std::string portID="" );

    /**
     * @return Number of nodes
     */
    FWDATA_API unsigned int getNbNodes() const;

    /**
     * @return Number of edges
     */
    FWDATA_API unsigned int getNbEdges() const;

    FWDATA_API void shallowCopy( Graph::csptr _source );

    FWDATA_API void deepCopy( Graph::csptr _source );

protected :

    /**
     * @brief Check if an edge is connected to the node
     *
     * @return true if at least one edge is connected to given node
     */
    FWDATA_API bool haveConnectedEdges(Node::csptr _node ) const;

    NodeContainer m_nodes;
    ConnectionContainer m_connections;
};

}

#endif // _FWDATA_GRAPH_HPP_
