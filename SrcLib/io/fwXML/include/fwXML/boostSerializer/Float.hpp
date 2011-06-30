/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _Float_SERIALIZER_HPP
#define _Float_SERIALIZER_HPP


#include <fwData/Float.hpp>

namespace boost
{
namespace serialization
{

/**
 * @brief serialize object boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void serialize(Archive & ar, ::fwData::Float &_object, const unsigned int version)
{
    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register< ::fwData::Float, ::fwTools::Object>(NULL,NULL);

     ar &  ::boost::serialization::make_nvp( "value"  , _object.value() );

}

}
}

INSTANTIATE_SERIALIZER(::fwData::Float);

#endif // _Float_SERIALIZER_HPP
