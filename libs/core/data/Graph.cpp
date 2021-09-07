/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "data/Graph.hpp"

#include "data/Edge.hpp"
#include "data/Exception.hpp"
#include "data/Node.hpp"
#include "data/Port.hpp"
#include "data/registry/macros.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <utility>

SIGHT_REGISTER_DATA(sight::data::Graph);

namespace sight::data
{

const bool Graph::UP_STREAM   = true;
const bool Graph::DOWN_STREAM = false;

const core::com::Signals::SignalKeyType Graph::s_UPDATED_SIG = "updated";

//------------------------------------------------------------------------------

Graph::Graph(data::Object::Key) :
    m_sigUpdated(UpdatedSignalType::New())
{
    m_signals(s_UPDATED_SIG, m_sigUpdated);
}

//------------------------------------------------------------------------------

Graph::~Graph()
{
}

//------------------------------------------------------------------------------

bool Graph::addNode(Node::sptr node)
{
    return m_nodes.insert(node).second;
}

//------------------------------------------------------------------------------

bool Graph::removeNode(Node::csptr node)
{
    // test if connected edge to it
    if(haveConnectedEdges(node))
    {
        return false;
    }

    return m_nodes.erase(Node::constCast(node)) > 0;
}

//------------------------------------------------------------------------------

Graph::NodeContainer& Graph::getNodes()
{
    return m_nodes;
}

//------------------------------------------------------------------------------

const Graph::NodeContainer& Graph::getNodes() const
{
    return m_nodes;
}

//------------------------------------------------------------------------------

bool Graph::haveConnectedEdges(Node::csptr node) const
{
    for(const auto& connection : m_connections)
    {
        if(connection.second.first == node || connection.second.second == node)
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
    std::string EdgeNature
)
{
    data::Edge::sptr nEdge = data::Edge::New();
    nEdge->setIdentifiers(nodeSourceOutputPortID, nodeDestinationInputPortID);
    nEdge->setNature(EdgeNature);
    if(addEdge(nEdge, nodeSource, nodeDestination))
    {
        return nEdge; // success return new Edge
    }
    else
    {
        return data::Edge::sptr(); // failure
    }
}

//------------------------------------------------------------------------------

bool Graph::addEdge(Edge::sptr edge, Node::csptr nodeSource, Node::csptr nodeDestination)
{
    // edge not already recorded
    if(m_connections.find(edge) != m_connections.end())
    {
        return false; // edge already stored
    }

    // node registered ?
    if(m_nodes.find(Node::constCast(nodeSource)) == m_nodes.end())
    {
        return false; // node already stored
    }

    // node registered ?
    if(m_nodes.find(Node::constCast(nodeDestination)) == m_nodes.end())
    {
        return false; // node already stored
    }

    // test port existence
    Port::sptr sourcePort = nodeSource->findPort(edge->getIdentifiers().first, DOWN_STREAM);
    if(!sourcePort)
    {
        return false; // port doesn't exist
    }

    // test port existence
    Port::sptr sourceDest = nodeDestination->findPort(edge->getIdentifiers().second, UP_STREAM);
    if(!sourceDest)
    {
        return false; // port doesn't exist
    }

    // test port compatibility
    if(sourcePort->getType() != sourceDest->getType())
    {
        return false; // incompatible type
    }

    m_connections[edge] = std::make_pair(Node::constCast(nodeSource), Node::constCast(nodeDestination));

    return true;
}

//------------------------------------------------------------------------------

bool Graph::removeEdge(Edge::sptr edge)
{
    return m_connections.erase(edge) != 0;
}

//------------------------------------------------------------------------------

Node::sptr Graph::getSourceNode(Edge::sptr edge)
{
    return getNode(edge, UP_STREAM);
}

//------------------------------------------------------------------------------

Node::sptr Graph::getDestinationNode(Edge::sptr edge)
{
    return getNode(edge, DOWN_STREAM);
}

//------------------------------------------------------------------------------

Node::sptr Graph::getNode(Edge::sptr edge, bool upStream)
{
    ConnectionContainer::iterator i;
    i = m_connections.find(edge);

    // check edge is valid ?
    if(i == m_connections.end())
    {
        return Node::sptr();
    }

    if(upStream)
    {
        return (*i).second.first;
    }
    else
    {
        return (*i).second.second;
    }
}

//------------------------------------------------------------------------------

std::vector<Edge::sptr> Graph::getInputEdges(Node::csptr node)
{
    return getEdges(node, UP_STREAM);
}

//------------------------------------------------------------------------------

std::vector<Edge::sptr> Graph::getOutputEdges(Node::csptr node)
{
    return getEdges(node, DOWN_STREAM);
}

//------------------------------------------------------------------------------

std::vector<Edge::sptr> Graph::getEdges(
    const Node::csptr& node,
    bool upStream,
    const std::string& nature,
    const std::string& portID
)
{
    SIGHT_ASSERT(
        "Node " + node->getID() + " not found in graph",
        m_nodes.find(Node::constCast(node))
        != m_nodes.end()
    );
    SIGHT_ASSERT(
        "Port " + portID + " not found on node" + node->getID(),
        portID.empty() || node->findPort(portID, upStream)
    );

    std::vector<Edge::sptr> result;
    result.reserve(4);

    for(const auto& connection : m_connections)
    {
        const Edge::sptr& edge     = connection.first;
        const Node::sptr& nodeFrom = connection.second.first;
        const Node::sptr& nodeTo   = connection.second.second;

        bool isConnectedEdge = (upStream ? nodeTo : nodeFrom) == node;
        if(!isConnectedEdge)
        {
            continue;
        }

        bool isCorrectPort = portID.empty() || portID == (upStream ? edge->getToPortID() : edge->getFromPortID());
        if(!isCorrectPort)
        {
            continue;
        }

        bool isCorrectNature = nature.empty() || edge->getNature() == nature;
        if(!isCorrectNature)
        {
            continue;
        }

        result.push_back(edge);
    }

    return result;
}

//------------------------------------------------------------------------------

std::vector<data::Node::sptr> Graph::getNodes(
    const data::Node::csptr& node,
    bool upStream,
    const std::string& nature,
    const std::string& portID
)
{
    std::vector<Edge::sptr> edges;
    edges = getEdges(node, upStream, nature, portID);

    std::vector<Node::sptr> nodes;
    for(const Edge::sptr& edge : edges)
    {
        Node::sptr distantNode;
        distantNode = (upStream ? m_connections[edge].first : m_connections[edge].second);

        if(std::find(nodes.begin(), nodes.end(), distantNode) == nodes.end())
        {
            nodes.push_back(distantNode);
        }
    }

    return nodes;
}

//------------------------------------------------------------------------------

size_t Graph::getNbNodes() const
{
    return m_nodes.size();
}

//------------------------------------------------------------------------------

size_t Graph::getNbEdges() const
{
    return m_connections.size();
}

//------------------------------------------------------------------------------

const Graph::ConnectionContainer& Graph::getConnections() const
{
    return m_connections;
}

//------------------------------------------------------------------------------

Graph::ConnectionContainer& Graph::getConnections()
{
    return m_connections;
}

//------------------------------------------------------------------------------

void Graph::shallowCopy(const Object::csptr& _source)
{
    Graph::csptr other = Graph::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(_source);
    m_nodes       = other->m_nodes;
    m_connections = other->m_connections;
}

//------------------------------------------------------------------------------

void Graph::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Graph::csptr other = Graph::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source, cache);

    std::map<data::Node::sptr, data::Node::sptr> correspondenceBetweenNodes;

    m_nodes.clear();
    for(const data::Node::sptr& node : other->m_nodes)
    {
        data::Node::sptr newNode = data::Object::copy(node, cache);
        bool addOK               = this->addNode(newNode);
        SIGHT_ASSERT("Node " << newNode->getID() << " can't be added ", addOK);
        SIGHT_NOT_USED(addOK);
        correspondenceBetweenNodes.insert(std::make_pair(node, newNode));
    }

    m_connections.clear();
    for(const auto& connection : other->m_connections)
    {
        // Edge deep copy .
        data::Edge::sptr newEdge  = data::Object::copy(connection.first, cache);
        data::Node::sptr oldNode1 = (connection.second).first;
        data::Node::sptr oldNode2 = (connection.second).second;
        if((correspondenceBetweenNodes.find(Node::constCast(oldNode1)) != correspondenceBetweenNodes.end())
           && (correspondenceBetweenNodes.find(Node::constCast(oldNode2)) != correspondenceBetweenNodes.end()))
        {
            // Add new Edge
            this->addEdge(newEdge, correspondenceBetweenNodes[oldNode1], correspondenceBetweenNodes[oldNode2]);
        }
    }

    correspondenceBetweenNodes.clear();
}

//------------------------------------------------------------------------------

} // namespace sight::data
