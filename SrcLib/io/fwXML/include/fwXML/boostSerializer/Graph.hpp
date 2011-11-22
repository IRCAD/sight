/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_Graph_HPP_
#define _FWXML_BOOSTSERIALIZER_Graph_HPP_

#include <boost/serialization/set.hpp>
#include <fwTools/fromIsoExtendedString.hpp>
#include <fwData/Graph.hpp>

#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

namespace boost
{
namespace serialization
{

//------------------------------------------------------------------------------

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::Graph & _graph, const unsigned int version)
{
    //ar &  ::boost::serialization::make_nvp( "nodes" ,  _graph.getRefNodes() );
    SLM_FATAL("to implement");
}

//------------------------------------------------------------------------------

template<class Archive>
void save(Archive & ar, const ::fwData::Graph & _graph, const unsigned int version)
{
    //ar &  ::boost::serialization::make_nvp( "nodes" , _graph.getCRefNodes() );
    int nbElt = _graph.getCRefNodes().size();
    ar &  ::boost::serialization::make_nvp( "nbNodes", nbElt );
    for ( ::fwData::Graph::NodeContainer::const_iterator  i= _graph.getCRefNodes().begin(); i!=_graph.getCRefNodes().end(); ++i)
    {
        ar &  ::boost::serialization::make_nvp( "Node", **i );
    }
}

//------------------------------------------------------------------------------

/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Graph & _graph, const unsigned int version)
{
    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
    ::boost::serialization::void_cast_register<  ::fwData::Graph, fwTools::Object>(NULL,NULL);

    split_free(ar,_graph,version); // call load or save depending of archive type
}

//------------------------------------------------------------------------------

} //namespace serialization
} // namespace boost


#endif // _FWXML_BOOSTSERIALIZER_Graph_HPP_

