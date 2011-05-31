/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef _IMAGE_BOOST_SERIALISER_HPP
#define _IMAGE_BOOST_SERIALISER_HPP


#include <fwData/Image.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"
#include "fwXML/boostSerializer/DynamicType.hpp"




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

    ar &  ::boost::serialization::make_nvp( "Dimension" , _image.getCRefDimension() );
    ar &  ::boost::serialization::make_nvp( "PixelType" , _image.getCRefPixelType() );
    ar &  ::boost::serialization::make_nvp( "ManagesBuff" , _image.getCRefManagesBuff() );
    ar &  ::boost::serialization::make_nvp( "Spacing" , _image.getCRefSpacing() );
    ar &  ::boost::serialization::make_nvp( "Origin" , _image.getCRefOrigin() );
    ar &  ::boost::serialization::make_nvp( "Size" , _image.getCRefSize() );

    std::string filename = _image.getCRefFilename().string();
    ar &  ::boost::serialization::make_nvp( "Filename" , filename );

    ar &  ::boost::serialization::make_nvp( "WindowCenter" , _image.getCRefWindowCenter() );
    ar &  ::boost::serialization::make_nvp( "WindowWidth" , _image.getCRefWindowWidth() );
    ar &  ::boost::serialization::make_nvp( "RescaleIntercept" , _image.getCRefRescaleIntercept() );

    // create a empty Buffer tag managed by other element
    std::string emptyString;
    ar &  ::boost::serialization::make_nvp( "Buffer"     , emptyString );

}

template<class Archive>
void load(Archive & ar, ::fwData::Image &_image, const unsigned int version)
{
    ::boost::serialization::void_cast_register<  ::fwData::Image, fwTools::Object>(NULL,NULL);

    ar &  ::boost::serialization::make_nvp( "Dimension" , _image.getRefDimension() );
    ar &  ::boost::serialization::make_nvp( "PixelType" , _image.getRefPixelType() );
    ar &  ::boost::serialization::make_nvp( "ManagesBuff" , _image.getRefManagesBuff() );
    ar &  ::boost::serialization::make_nvp( "Spacing" , _image.getRefSpacing() );
    ar &  ::boost::serialization::make_nvp( "Origin" , _image.getRefOrigin() );
    ar &  ::boost::serialization::make_nvp( "Size" , _image.getRefSize() );

    std::string filename;
    ar &  ::boost::serialization::make_nvp( "Filename" , filename );
    _image.setCRefFilename( ::boost::filesystem::path(filename) );

    ar &  ::boost::serialization::make_nvp( "WindowCenter" , _image.getRefWindowCenter() );
    ar &  ::boost::serialization::make_nvp( "WindowWidth" , _image.getRefWindowWidth() );
    ar &  ::boost::serialization::make_nvp( "RescaleIntercept" , _image.getRefRescaleIntercept() );


     // read Buffer element to be correct with XML parsing but doesn't interpret field delegated to abother mechanisme
    std::string dummy;
    ar &  ::boost::serialization::make_nvp( "Buffer"   , dummy );



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

INSTANTIATE_SERIALIZER(::fwData::Image)

#endif // _IMAGE_BOOST_SERIALISER_HPP



