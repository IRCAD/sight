/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_MATERIAL_HPP_
#define _FWXML_BOOSTSERIALIZER_MATERIAL_HPP_

#include <fwData/Material.hpp>

#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

namespace boost
{
namespace serialization
{

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void serialize(Archive & ar, ::fwData::Material & _material, const unsigned int version)
{
    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register< ::fwData::Material, fwTools::Object>(NULL,NULL);
     int shadingMode =  _material.getRefShadingMode();
     ar &  ::boost::serialization::make_nvp( "shadingMode", shadingMode);
     int representationMode =  _material.getRefRepresentationMode();
     ar &  ::boost::serialization::make_nvp( "representationMode", representationMode);
     int optionsMode =  _material.getRefOptionsMode();
     ar &  ::boost::serialization::make_nvp( "optionsMode", optionsMode);
}

} // namespace serialization
} // namespace boost

INSTANTIATE_SERIALIZER(::fwData::Material);

#endif // _FWXML_BOOSTSERIALIZER_MATERIAL_HPP_

