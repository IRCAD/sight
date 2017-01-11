/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef WIN32
// To fix problem in class boost::detail::stored_edge_property
// Default constructor is a move constructor and no copy constructor is implemented
// The instance of the class can not be copied.
#define BOOST_NO_CXX11_RVALUE_REFERENCES
#define BOOST_NO_CXX11_REF_QUALIFIERS
#endif
#include <algorithm>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <fwCore/spyLog.hpp>

#include "fwAtomsPatch/exceptions/UnknownVersion.hpp"

#include "fwAtomsPatch/VersionsGraph.hpp"

namespace fwAtomsPatch
{

class VertexVisitor : public boost::default_bfs_visitor
{
public:

    VertexVisitor(std::vector< std::string >* vector) : m_vector(vector)
    {
    }

    void discover_vertex(VersionsGraph::NodeIDType n, VersionsGraph::GraphType g)
    {
        m_vector->push_back( g[n].getVersionName());
    }

protected:
    std::vector< std::string >* m_vector;
};

VersionsGraph::VersionsGraph()
{
}

// ----------------------------------------------------------------------------

VersionsGraph::~VersionsGraph()
{
}

// ----------------------------------------------------------------------------

void VersionsGraph::addNode(NodeType node)
{
    ::fwCore::mt::WriteLock lock(m_nodesMutex);
    NodeIDType newNode = this->createOrUpdateNode(node);
    m_nodes[node] = newNode;
}

// ----------------------------------------------------------------------------

void VersionsGraph::addEdge(EdgeType edge)
{
    ::fwCore::mt::WriteLock lock(m_edgesMutex);
    EdgeIDType newEdge = this->createEdge(edge);
    m_edges[edge] = newEdge;
}

// ----------------------------------------------------------------------------

VersionsGraph::VersionSeriesType VersionsGraph::shortestPath(const std::string& origin,
                                                             const std::string& target)
{
    NodeIDType originID = this->getNode(origin);
    NodeIDType targetID = this->getNode(target);
    return this->shortestPath(this->getNode(originID), this->getNode(targetID));
}

// ----------------------------------------------------------------------------

VersionsGraph::VersionSeriesType VersionsGraph::shortestPath(const NodeType& origin, const NodeType& target)
{
    VersionSeriesType serie;

    ::fwCore::mt::ReadLock nodesLock(m_nodesMutex);
    ::fwCore::mt::ReadLock graphLock(m_graphMutex);

    std::vector< NodeIDType > predecessor( ::boost::num_vertices(m_graph) );
    std::vector< int > distances( ::boost::num_vertices(m_graph) );

    const NodeIDType& vOrig = m_nodes[origin];
    ::boost::dijkstra_shortest_paths(m_graph, vOrig,
                                     ::boost::weight_map(get(&EdgeType::m_weight, m_graph))
                                     .predecessor_map(&predecessor[0])
                                     .distance_map(::boost::make_iterator_property_map(distances.begin(),
                                                                                       ::boost::get(::boost::
                                                                                                    vertex_index,
                                                                                                    m_graph )))
                                     );

    NodeIDType current = m_nodes[target];

    while(current != predecessor[current])
    {
        serie.insert(serie.begin(), current);
        current = predecessor[current];
    }

    return serie;
}

// ----------------------------------------------------------------------------

VersionsGraph::NodeType VersionsGraph::getNode(const NodeIDType& nodeID)
{
    ::fwCore::mt::ReadLock lock(m_graphMutex);
    return m_graph[nodeID];
}

// ----------------------------------------------------------------------------

VersionsGraph::NodeIDType VersionsGraph::getNode(const std::string& name) const
{
    ::fwCore::mt::ReadLock lock(m_nodesMutex);
    ExistingNodesType::const_iterator it;
    for(it = m_nodes.begin(); it != m_nodes.end(); ++it)
    {
        if(it->first.getVersionName() == name)
        {
            break;
        }
    }

    if(it == m_nodes.end())
    {
        throw ::fwAtomsPatch::exceptions::UnknownVersion("There is no version named \"" + name + "\".");
    }

    return it->second;
}

// ----------------------------------------------------------------------------

VersionsGraph::EdgeType VersionsGraph::getEdge(const NodeIDType& origin, const NodeIDType& target)
{
    EdgeIDType edgeID;
    bool success = false;
    ::fwCore::mt::ReadLock lock(m_graphMutex);
    ::boost::tie(edgeID,success) = ::boost::edge(origin, target, m_graph);
    OSLM_ASSERT("There is no edge between '" << m_graph[origin].getVersionName() <<"' and '"
                                             << m_graph[target].getVersionName() << "'.", success);
    return m_graph[edgeID];
}

// ----------------------------------------------------------------------------

VersionsGraph::LinkedVersionType VersionsGraph::getLinkedVersion(
    const NodeIDType& originID, const NodeIDType& targetID, LinkDescriptor::VersionIDType current)
{
    NodeType origin = this->getNode(originID);
    NodeType target = this->getNode(targetID);
    EdgeType edge   = this->getEdge(originID, targetID);

    bool success = false;
    LinkDescriptor::VersionIDType result;
    LinkDescriptor::LinksType::const_iterator linkIt;

    //Explicit
    const LinkDescriptor::LinksType& links = edge.getLinks();
    linkIt = links.find(current);
    if(linkIt != links.end())
    {
        result  = linkIt->second;
        success = true;
    }
    //Implicit
    else
    {
        VersionDescriptor::VersionsType::const_iterator versIt;
        VersionDescriptor::VersionsType::const_iterator versItEnd = target.getVersions().end();
        for(versIt = target.getVersions().begin(); versIt != versItEnd; ++versIt)
        {
            //Same type
            if(current.first == versIt->first)
            {
                result  = *versIt;
                success = true;
                break;
            }
        }
    }

    return std::make_pair(result, success);
}

// ----------------------------------------------------------------------------

VersionsGraph::NodeIDType VersionsGraph::createOrUpdateNode(const NodeType& node)
{
    ExistingNodesType::const_iterator cIt = m_nodes.find(node);
    if(cIt != m_nodes.end())
    {
        return cIt->second;
    }
    else
    {
        ::fwCore::mt::WriteLock lock(m_graphMutex);
        NodeIDType v = ::boost::add_vertex(m_graph);
        m_graph[v] = node;
        return v;
    }
}

// ----------------------------------------------------------------------------

VersionsGraph::EdgeIDType VersionsGraph::createEdge(const EdgeType& edge)
{
    NodeIDType origin = this->getNode(edge.getOriginVersion());
    NodeIDType target = this->getNode(edge.getTargetVersion());

    EdgeIDType newEdge;
    bool success = false;
    ::fwCore::mt::ReadLock lock(m_graphMutex);
    ::boost::tie(newEdge, success) = ::boost::add_edge(origin, target, edge, m_graph);

    OSLM_ASSERT("Unable to create the edge between '" << edge.getOriginVersion() << "' "
                "and '" << edge.getTargetVersion() << "'", success);

    return newEdge;
}

// ----------------------------------------------------------------------------

std::vector< std::string > VersionsGraph::getConnectedVersions(const std::string &currentVersion)
{
    std::vector< std::string > vector;
    VertexVisitor vis(&vector);
    try
    {
        ::fwCore::mt::ReadLock lock(m_graphMutex);
        VersionsGraph::NodeIDType nodeId = this->getNode(currentVersion);
        ::boost::breadth_first_search( m_graph, nodeId, ::boost::visitor(vis) );
        vector.erase(vector.begin());
    }
    catch ( ::fwAtomsPatch::exceptions::UnknownVersion & )
    {
    }

    return vector;
}


} // fwAtomsPatch
