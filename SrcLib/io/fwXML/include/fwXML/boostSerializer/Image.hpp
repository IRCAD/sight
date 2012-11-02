/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef _IMAGE_BOOST_SERIALISER_HPP
#define _IMAGE_BOOST_SERIALISER_HPP


#include <fwData/Image.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"
#include "fwXML/boostSerializer/Type.hpp"


namespace boost {
namespace serialization {

/**
 * @brief serialize image boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void save(Archive & ar, const ::fwData::Image &_image, const unsigned int version)
{
    ::boost::serialization::void_cast_register<  ::fwData::Image, fwTools::Object>(NULL,NULL);

    ::fwTools::Type type = _image.getType();
    ar &  ::boost::serialization::make_nvp( "PixelType" , type );
    ar &  ::boost::serialization::make_nvp( "Spacing" , _image.getSpacing() );
    ar &  ::boost::serialization::make_nvp( "Origin" , _image.getOrigin() );
    ar &  ::boost::serialization::make_nvp( "Size" , _image.getSize() );

    double windowCenter = _image.getWindowCenter();
    double windowWidth = _image.getWindowWidth();
    ar &  ::boost::serialization::make_nvp( "WindowCenter" , windowCenter );
    ar &  ::boost::serialization::make_nvp( "WindowWidth" , windowWidth );

    size_t nbOfComponents = _image.getNumberOfComponents();
    ar &  ::boost::serialization::make_nvp( "NumberOfComponents"  , nbOfComponents );
}

template<class Archive>
void load(Archive & ar, ::fwData::Image &_image, const unsigned int version)
{
    ::boost::serialization::void_cast_register<  ::fwData::Image, fwTools::Object>(NULL,NULL);

    ::fwTools::Type type;
    ar &  ::boost::serialization::make_nvp( "PixelType" , type);
    _image.setType(type);

    ::fwData::Image::SizeType size;
    ::fwData::Image::SpacingType spacing;
    ::fwData::Image::OriginType origin;
    ar &  ::boost::serialization::make_nvp( "Spacing" , spacing );
    ar &  ::boost::serialization::make_nvp( "Origin" , origin );
    ar &  ::boost::serialization::make_nvp( "Size" , size );
    _image.setSpacing(spacing);
    _image.setOrigin(origin);
    _image.setSize(size);

    double windowCenter, windowWidth;
    ar &  ::boost::serialization::make_nvp( "WindowCenter" , windowCenter );
    ar &  ::boost::serialization::make_nvp( "WindowWidth" , windowWidth );
    _image.setWindowCenter(windowCenter);
    _image.setWindowWidth(windowWidth);

}


/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Image &image, const unsigned int version)
{
    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register<  ::fwData::Image, fwTools::Object>(NULL,NULL);

    split_free(ar,image,version); // call load or save depending of archive type
}


} }

#endif // _IMAGE_BOOST_SERIALISER_HPP



