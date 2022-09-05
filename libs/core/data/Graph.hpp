/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#pragma once

#include "data/config.hpp"
#include "data/Edge.hpp"
#include "data/factory/new.hpp"
#include "data/Node.hpp"
#include "data/Object.hpp"

#include <map>
#include <set>

namespace sight::data
{

/**
 * @brief   This class defines a graph object.
 *
 * A graph is represented by connections between Edge and Node.

 * @see     data::Edge, data::Node
 */
class DATA_CLASS_API Graph : public Object
{
public:

    SIGHT_DECLARE_CLASS(Graph, Object, data::factory::New<Graph>);

    DATA_API static const bool UP_STREAM;
    DATA_API static const bool DOWN_STREAM;

    typedef std::map<Edge::sptr, std::pair<Node::sptr, Node::sptr> > ConnectionContainer;
    typedef std::set<Node::sptr> NodeContainer; //  Be careful, if you change
                                                // we use erase(it++)

    /// Type of signal m_sigUpdated
    typedef std::vector<Object::sptr> UpdatedDataType;

    /**
     * @brief Update signal type
     * Signal updated is composed of two parameters which represent added and removed elements in the graph.
     */
    typedef core::com::Signal<void (UpdatedDataType, UpdatedDataType)> UpdatedSignalType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Graph(Object::Key key);

    /// Destructor
    DATA_API ~Graph() override;

    /**
     * @brief add a node
     *
     * @return true on success( node not already in graph)
     */
    DATA_API bool addNode(Node::sptr _node);

    /**
     * @brief remove a node
     *
     * @return true on success
     */
    DATA_API bool removeNode(Node::csptr _node);

    /**
     * @{
     * @brief Get the node container
     */
    DATA_API NodeContainer& getNodes();
    DATA_API const NodeContainer& getNodes() const;
    ///@}

    /**
     * @brief Get the connection container
     *
     * A connection is defined by an edge, a source node and a destination node.
     *
     * @return map<Edge, pair<source node, destination node> >
     */
    DATA_API const ConnectionContainer& getConnections() const;
    DATA_API ConnectionContainer& getConnections();

    /**
     * @brief Add and edge
     *
     * @return true if success
     * @li nodes exist
     * @li edge is unique
     * @li port is compatible identifier & type
     */
    DATA_API bool addEdge(Edge::sptr _edge, Node::csptr _nodeSource, Node::csptr _nodeDestination);

    /**
     * @brief create an edge from given info and add edge in the graph
     *
     * @return new edge created if success else return null one
     */
    DATA_API
    Edge::sptr makeConnection(
        Node::csptr _nodeSource,
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
    DATA_API bool removeEdge(Edge::sptr _edge);

    /**
     * @brief Get source node
     *
     * @param[in] _edge edge associated with the node
     */
    DATA_API Node::sptr getSourceNode(Edge::sptr _edge);

    /**
     * @brief Get destination node
     *
     * @param[in] _edge edge associated with the node
     */
    DATA_API Node::sptr getDestinationNode(Edge::sptr _edge);

    /**
     * @brief Get node
     *
     * @param[in] _edge edge associated with the node
     * @param[in] _upStream if true return source node, else return destination node
     *
     * @return source or destination node if edge exists, else return a new node
     */
    DATA_API Node::sptr getNode(Edge::sptr _edge, bool _upStream);

    /**
     * @brief Get input edges
     *
     * @param[in] _node source node
     * @return a vector of edges where _node is source node
     */
    DATA_API std::vector<Edge::sptr> getInputEdges(Node::csptr _node);

    /**
     * @brief Get output edges
     *
     * @param[in] _node destination node
     * @return a vector of  edges where _node is destination node
     */
    DATA_API std::vector<Edge::sptr> getOutputEdges(Node::csptr _node);

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
    DATA_API std::vector<Edge::sptr> getEdges(
        const Node::csptr& _node,
        bool _upStream,
        const std::string& _nature = "",
        const std::string& _portID = ""
    );

    /**
     * @brief Get a vector of nodes
     *
     * @param[in] node node associated with the nodes
     * @param[in] upStream if true _node is a source node, else _node is a destination node
     * @param[in] nature if nature is an empty string : no check on nature of edge
     * @param[in] portID if portID not specified (no check) : porID muts be an BoolupStream port of given node
     *
     * @return the vector of all nodes associated with _node
     */
    DATA_API std::vector<data::Node::sptr> getNodes(
        const data::Node::csptr& node,
        bool upStream,
        const std::string& nature = "",
        const std::string& portID = ""
    );

    /**
     * @return Number of nodes
     */
    DATA_API std::size_t getNbNodes() const;

    /**
     * @return Number of edges
     */
    DATA_API std::size_t getNbEdges() const;

    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /**
     * @brief Check if an edge is connected to the node
     *
     * @return true if at least one edge is connected to given node
     */
    DATA_API bool haveConnectedEdges(Node::csptr _node) const;

    /// Updated signal key
    DATA_API static const core::com::Signals::SignalKeyType s_UPDATED_SIG;

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Graph& other) const noexcept;
    DATA_API bool operator!=(const Graph& other) const noexcept;
    /// @}

protected:

    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    NodeContainer m_nodes;
    ConnectionContainer m_connections;

    /// Updated signal
    UpdatedSignalType::sptr m_sigUpdated;
};

} // namespace sight::data
