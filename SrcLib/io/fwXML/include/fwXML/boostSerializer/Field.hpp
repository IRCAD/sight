/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _LABELEDOBJECT_SERIALIZER_HPP
#define _LABELEDOBJECT_SERIALIZER_HPP


#include <fwTools/Object.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"
#include "fwXML/boostSerializer/Object.hpp"


// macro necessary if this class is serialized via a (smart)pointer : define a portable type_info in the archive



namespace boost {
namespace serialization {

/**
 * @brief serialize object boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void serialize(Archive & ar, ::fwTools::Field &_object, const unsigned int version)
{
    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register< ::fwTools::Field, fwTools::Object>(NULL,NULL);

     ar &  ::boost::serialization::make_nvp( "label"  , _object.label() );

}



} }
INSTANTIATE_SERIALIZER(fwTools::Field);

#endif // _LABELEDOBJECT_SERIALIZER_HPP
