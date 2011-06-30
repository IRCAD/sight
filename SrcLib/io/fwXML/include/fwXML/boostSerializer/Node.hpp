/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _FWXML_BOOSTSERIALIZER_NODE_HPP_
#define _FWXML_BOOSTSERIALIZER_NODE_HPP_

#include <fwData/Node.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

#include <fwTools/fromIsoExtendedString.hpp>

namespace boost {
namespace serialization {

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::Node & _node, const unsigned int version)
{
    SLM_FATAL("To implement");
    ::fwTools::Object::sptr obj;
    std::string ss="essai";
    ar &  ::boost::serialization::make_nvp( "object" , ss /*obj*/ );
    _node.setObject(obj);
}

//------------------------------------------------------------------------------

template<class Archive>
void save(Archive & ar, const ::fwData::Node & _node, const unsigned int version)
{
    ::fwTools::Object::csptr obj=  _node.getObject(); // make a non cst var
    SLM_ASSERT("obj not instanced", obj);
    ar & ::boost::serialization::make_nvp( "object" ,  obj );
}

//------------------------------------------------------------------------------

/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Node & _node, const unsigned int version)
{
    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
    ::boost::serialization::void_cast_register<  ::fwData::Node, fwTools::Object>(NULL,NULL);

    split_free(ar,_node,version); // call load or save depending of archive type
}

//------------------------------------------------------------------------------

} } // end namespace


INSTANTIATE_SERIALIZER(::fwData::Node);



#endif // _FWXML_BOOSTSERIALIZER_NODE_HPP_

