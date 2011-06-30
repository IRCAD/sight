/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_COLOR_HPP_
#define _FWXML_BOOSTSERIALIZER_COLOR_HPP_

#include <fwData/Color.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"



namespace boost {
namespace serialization {

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::Color & _color, const unsigned int version)
{
    std::vector< ::fwData::Color::ColorType > tmp; // compatibility mode std::vector -> ::boost::array
     ar &  ::boost::serialization::make_nvp( "RGBA" ,tmp );
    // _color.setCRefRGBA() = tmp;
     std::copy(tmp.begin(), tmp.end(), _color.getRefRGBA().begin());
}

template<class Archive>
void save(Archive & ar, const ::fwData::Color & _color, const unsigned int version)
{
    std::vector< ::fwData::Color::ColorType > tmp( _color.getCRefRGBA().size() ); // compatibility mode std::vector -> ::boost::array
    std::copy( _color.getCRefRGBA().begin(), _color.getCRefRGBA().end(), tmp.begin());
    ar &  ::boost::serialization::make_nvp( "RGBA" , tmp );
}






/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Color & _color, const unsigned int version)
{

    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register<  ::fwData::Color, fwTools::Object>(NULL,NULL);

    split_free(ar,_color,version); // call load or save depending of archive type
}




} } // end namespace

INSTANTIATE_SERIALIZER(::fwData::Color);

#endif // _FWXML_BOOSTSERIALIZER_COLOR_HPP_

