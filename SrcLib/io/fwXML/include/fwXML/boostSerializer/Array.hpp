/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALISER_ARRAY_HPP
#define _FWXML_BOOSTSERIALISER_ARRAY_HPP

#include <fwData/Array.hpp>

#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

namespace boost
{
namespace serialization
{

/**
 * @brief serialize image boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void save(Archive & ar, const ::fwData::Array &_array, const unsigned int version)
{
    ::boost::serialization::void_cast_register< ::fwData::Array, ::fwTools::Object>(NULL,NULL);
    size_t nbComponents = _array.getNumberOfComponents();
    ar &  ::boost::serialization::make_nvp( "Size", _array.getSize() );
    ar &  ::boost::serialization::make_nvp( "NumberOfComponents", nbComponents );
    ar &  ::boost::serialization::make_nvp( "Type", _array.getType().string() );

    // create a empty Buffer tag managed by other element
    std::string emptyString;
    ar &  ::boost::serialization::make_nvp( "Buffer", emptyString );

}

//------------------------------------------------------------------------------

template<class Archive>
void load(Archive & ar, ::fwData::Array &_array, const unsigned int version)
{
    ::boost::serialization::void_cast_register< ::fwData::Array, ::fwTools::Object>(NULL,NULL);
    ::fwData::Array::SizeType size;
    ::fwData::Array::OffsetType stride;
    size_t nbComponents;
    std::string typeStr;

    ar &  ::boost::serialization::make_nvp( "Size", size );
    ar &  ::boost::serialization::make_nvp( "NumberOfComponents", nbComponents );
    ar &  ::boost::serialization::make_nvp( "Type", typeStr );

    ::fwTools::Type type  = ::fwTools::Type::create(typeStr);
    _array.setIsBufferOwner(true);
    _array.resize(type, size, nbComponents, true);

     // read Buffer element to be correct with XML parsing but doesn't interpret field delegated to another mechanism
    std::string dummy;
    ar &  ::boost::serialization::make_nvp( "Buffer", dummy );
}

//------------------------------------------------------------------------------

/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Array &array, const unsigned int version)
{
    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialization of the base one
    // can be well casted to the derivated one during the deserialization
    ::boost::serialization::void_cast_register< ::fwData::Array, ::fwTools::Object>(NULL,NULL);

    split_free(ar, array, version); // call load or save depending of archive type
}

//------------------------------------------------------------------------------

} //namespace serialization
} //namespace boost

INSTANTIATE_SERIALIZER(::fwData::Array)

#endif // _FWXML_BOOSTSERIALISER_ARRAY_HPP



