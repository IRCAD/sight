/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_TRANSFERFUNCTION_HPP_
#define _FWXML_BOOSTSERIALIZER_TRANSFERFUNCTION_HPP_

#include <fwData/TransfertFunction.hpp>
#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

#include <fwTools/fromIsoExtendedString.hpp>

namespace boost {
namespace serialization {


//-----------------------------------------------------------------------------

template<class Archive>
void load(Archive & ar, ::fwData::TransferFunction::TFColor & _tfColor, const unsigned int version)
{
    std::vector< float > vecColor;
    ar &  ::boost::serialization::make_nvp( "TFColor" ,vecColor );
    _tfColor.r = vecColor[0];
    _tfColor.g = vecColor[1];
    _tfColor.b = vecColor[2];
    _tfColor.a = vecColor[3];
}

//-----------------------------------------------------------------------------

template<class Archive>
void save(Archive & ar, const ::fwData::TransferFunction::TFColor & _tfColor, const unsigned int version)
{
    std::vector< float > vecColor (4);
    vecColor[0] = _tfColor.r;
    vecColor[1] = _tfColor.g;
    vecColor[2] = _tfColor.b;
    vecColor[3] = _tfColor.a;
    ar &  ::boost::serialization::make_nvp( "TFColor" , vecColor );
}

//-----------------------------------------------------------------------------

/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::TransferFunction::TFColor & _tfColor, const unsigned int version)
{
    split_free( ar, _tfColor, version ); // call load or save depending of archive type
}

//-----------------------------------------------------------------------------

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::TransferFunction & _transfertFunction, const unsigned int version)
{
    double attrLevel;
    double attrWindow;
    std::string attrName;
    bool attrIsClamped;
    int attrInterpolationMode;
    ::fwData::TransferFunction::TFColor attrBackgroundColor;
    ::fwData::TransferFunction::TFDataType tfData;

    ar &  ::boost::serialization::make_nvp( "level"             , attrLevel );
    ar &  ::boost::serialization::make_nvp( "window"            , attrWindow );
    ar &  ::boost::serialization::make_nvp( "name"              , attrName );
    ar &  ::boost::serialization::make_nvp( "isClamped"         , attrIsClamped );
    ar &  ::boost::serialization::make_nvp( "interpolationMode" , attrInterpolationMode );
    ar &  ::boost::serialization::make_nvp( "backgroundColor"   , attrBackgroundColor );
    ar &  ::boost::serialization::make_nvp( "tfData"            , tfData );

    _transfertFunction.setLevel(attrLevel);
    _transfertFunction.setWindow(attrWindow);
    _transfertFunction.setName(attrName);
    _transfertFunction.setIsClamped(attrIsClamped);
    _transfertFunction.setInterpolationMode( static_cast< ::fwData::TransferFunction::InterpolationMode >(attrInterpolationMode) );
    _transfertFunction.setBackgroundColor( attrBackgroundColor );
    _transfertFunction.setTFData( tfData );
}

//-----------------------------------------------------------------------------

template<class Archive>
void save(Archive & ar, const ::fwData::TransferFunction & _transfertFunction, const unsigned int version)
{
    std::string newStr;

    double attrLevel = _transfertFunction.getLevel();
    double attrWindow = _transfertFunction.getWindow();
    std::string attrName = _transfertFunction.getName();
    bool attrIsClamped = _transfertFunction.getIsClamped();
    int attrInterpolationMode = _transfertFunction.getInterpolationMode();
    ::fwData::TransferFunction::TFColor attrBackgroundColor = _transfertFunction.getBackgroundColor();
    ::fwData::TransferFunction::TFDataType tfData = _transfertFunction.getTFData();

    ar &  ::boost::serialization::make_nvp( "level"             , attrLevel );
    ar &  ::boost::serialization::make_nvp( "window"            , attrWindow );
    ar &  ::boost::serialization::make_nvp( "name"              , attrName );
    ar &  ::boost::serialization::make_nvp( "isClamped"         , attrIsClamped );
    ar &  ::boost::serialization::make_nvp( "interpolationMode" , attrInterpolationMode );
    ar &  ::boost::serialization::make_nvp( "backgroundColor"   , attrBackgroundColor );
    ar &  ::boost::serialization::make_nvp( "tfData"            , tfData );

}

//-----------------------------------------------------------------------------

/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::TransferFunction & _transfertFunction, const unsigned int version)
{

    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register<  ::fwData::TransferFunction, fwTools::Object>(NULL,NULL);

    split_free(ar,_transfertFunction,version); // call load or save depending of archive type
}



} } // end namespace


#endif // _FWXML_BOOSTSERIALIZER_TRANSFERFUNCTION_HPP_

