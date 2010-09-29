/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _STRING_SERIALIZER_HPP
#define _STRING_SERIALIZER_HPP


#include <fwData/String.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"
#include "fwXML/boostSerializer/Object.hpp"


// macro necessary if this class is serialized via a (smart)pointer : define a portable type_info in the archive
// (GUID stands for Globally Unique IDentfier.)
// http://boost.org/libs/serialization/doc/serialization.html#registration
//BOOST_CLASS_EXPORT(::fwData::Float);


namespace boost {
namespace serialization {

/**
 * @brief serialize object boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void serialize(Archive & ar, ::fwData::String &_object, const unsigned int version)
{
    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     boost::serialization::void_cast_register< ::fwData::String fwTools::Object>(NULL,NULL);

     ar &  boost::serialization::make_nvp( "value"  , _object.value() );
}



} }

INSTANTIATE_SERIALIZER(::fwData::String);

#endif // _STRING_SERIALIZER_HPP
