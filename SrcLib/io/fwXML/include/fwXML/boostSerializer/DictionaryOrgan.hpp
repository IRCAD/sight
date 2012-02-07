/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_DICTIONARYORGAN_HPP_
#define _FWXML_BOOSTSERIALIZER_DICTIONARYORGAN_HPP_

#include <fwCore/base.hpp>

#include <fwData/DictionaryOrgan.hpp>

#include "fwXML/boostSerializer/IFSBoostSerialize.hpp"

namespace boost {
namespace serialization {

/**
 * @brief serialize algorithm via boost.org serialization scheme
 * @author IRCAD (Research and Development Team).
 */
template<class Archive>
void load(Archive & ar, ::fwData::DictionaryOrgan & _dictionaryOrgan, const unsigned int version)
{
    ar &  ::boost::serialization::make_nvp( "StructureType" , _dictionaryOrgan.getRefStructureType() );
    ar &  ::boost::serialization::make_nvp( "SALabel" , _dictionaryOrgan.getRefSALabel() );
    ar &  ::boost::serialization::make_nvp( "IsMedicalStructure" , _dictionaryOrgan.getRefIsMedicalStructure() );
    ar &  ::boost::serialization::make_nvp( "AOColor" , _dictionaryOrgan.getRefAOColor() );
    ar &  ::boost::serialization::make_nvp( "AvgVolume" , _dictionaryOrgan.getRefAvgVolume() );
    ar &  ::boost::serialization::make_nvp( "VolStdDeviation" , _dictionaryOrgan.getRefVolStdDeviation() );
    ar &  ::boost::serialization::make_nvp( "NbExams" , _dictionaryOrgan.getRefNbExams() );
    ar &  ::boost::serialization::make_nvp( "Position" , _dictionaryOrgan.getRefPosition() );

    std::string iconPath;
    ar &  ::boost::serialization::make_nvp( "IconPath" , iconPath );
    _dictionaryOrgan.setCRefIconPath( ::boost::filesystem::path(iconPath) );

    ar &  ::boost::serialization::make_nvp( "Red" , _dictionaryOrgan.getRefRed() );
    ar &  ::boost::serialization::make_nvp( "Green" , _dictionaryOrgan.getRefGreen() );
    ar &  ::boost::serialization::make_nvp( "Blue" , _dictionaryOrgan.getRefBlue() );
    ar &  ::boost::serialization::make_nvp( "Alpha" , _dictionaryOrgan.getRefAlpha() );

    std::string texturePath;
    ar &  ::boost::serialization::make_nvp( "TexturePath" , texturePath );
    _dictionaryOrgan.setCRefTexturePath( ::boost::filesystem::path(texturePath) );

    ar &  ::boost::serialization::make_nvp( "AvgTriangleNb" , _dictionaryOrgan.getRefAvgTriangleNb() );
    ar &  ::boost::serialization::make_nvp( "SegMin" , _dictionaryOrgan.getRefSegMin() );
    ar &  ::boost::serialization::make_nvp( "SegMax" , _dictionaryOrgan.getRefSegMax() );
    ar &  ::boost::serialization::make_nvp( "Modality" , _dictionaryOrgan.getRefModality() );
    ar &  ::boost::serialization::make_nvp( "SurfaceElasticity" , _dictionaryOrgan.getRefSurfaceElasticity() );
    ar &  ::boost::serialization::make_nvp( "SurfaceRupture" , _dictionaryOrgan.getRefSurfaceRupture() );
    ar &  ::boost::serialization::make_nvp( "InteriorRupture" , _dictionaryOrgan.getRefInteriorRupture() );
    ar &  ::boost::serialization::make_nvp( "Comment" , _dictionaryOrgan.getRefComment() );
    ar &  ::boost::serialization::make_nvp( "BelongsTo" , _dictionaryOrgan.getRefBelongsTo() );
}

template<class Archive>
void save(Archive & ar, const ::fwData::DictionaryOrgan & _dictionaryOrgan, const unsigned int version)
{
    ar &  ::boost::serialization::make_nvp( "StructureType" , _dictionaryOrgan.getCRefStructureType() );
    ar &  ::boost::serialization::make_nvp( "SALabel" , _dictionaryOrgan.getCRefSALabel() );
    ar &  ::boost::serialization::make_nvp( "IsMedicalStructure" , _dictionaryOrgan.getCRefIsMedicalStructure() );
    ar &  ::boost::serialization::make_nvp( "AOColor" , _dictionaryOrgan.getCRefAOColor() );
    ar &  ::boost::serialization::make_nvp( "AvgVolume" , _dictionaryOrgan.getCRefAvgVolume() );
    ar &  ::boost::serialization::make_nvp( "VolStdDeviation" , _dictionaryOrgan.getCRefVolStdDeviation() );
    ar &  ::boost::serialization::make_nvp( "NbExams" , _dictionaryOrgan.getCRefNbExams() );
    ar &  ::boost::serialization::make_nvp( "Position" , _dictionaryOrgan.getCRefPosition() );

    std::string iconPath = _dictionaryOrgan.getCRefIconPath().string();
    ar &  ::boost::serialization::make_nvp( "IconPath" , iconPath );

    ar &  ::boost::serialization::make_nvp( "Red" , _dictionaryOrgan.getCRefRed() );
    ar &  ::boost::serialization::make_nvp( "Green" , _dictionaryOrgan.getCRefGreen() );
    ar &  ::boost::serialization::make_nvp( "Blue" , _dictionaryOrgan.getCRefBlue() );
    ar &  ::boost::serialization::make_nvp( "Alpha" , _dictionaryOrgan.getCRefAlpha() );

    std::string texturePath = _dictionaryOrgan.getCRefTexturePath().string();
    ar &  ::boost::serialization::make_nvp( "TexturePath" , texturePath );

    ar &  ::boost::serialization::make_nvp( "AvgTriangleNb" , _dictionaryOrgan.getCRefAvgTriangleNb() );
    ar &  ::boost::serialization::make_nvp( "SegMin" , _dictionaryOrgan.getCRefSegMin() );
    ar &  ::boost::serialization::make_nvp( "SegMax" , _dictionaryOrgan.getCRefSegMax() );
    ar &  ::boost::serialization::make_nvp( "Modality" , _dictionaryOrgan.getCRefModality() );
    ar &  ::boost::serialization::make_nvp( "SurfaceElasticity" , _dictionaryOrgan.getCRefSurfaceElasticity() );
    ar &  ::boost::serialization::make_nvp( "SurfaceRupture" , _dictionaryOrgan.getCRefSurfaceRupture() );
    ar &  ::boost::serialization::make_nvp( "InteriorRupture" , _dictionaryOrgan.getCRefInteriorRupture() );
    ar &  ::boost::serialization::make_nvp( "Comment" , _dictionaryOrgan.getCRefComment() );
    ar &  ::boost::serialization::make_nvp( "BelongsTo" , _dictionaryOrgan.getCRefBelongsTo() );
}






/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::DictionaryOrgan & _study, const unsigned int version)
{

    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
     ::boost::serialization::void_cast_register<  ::fwData::DictionaryOrgan, fwTools::Object>(NULL,NULL);

    split_free(ar,_study,version); // call load or save depending of archive type
}



} } // end namespace

#endif // _FWXML_BOOSTSERIALIZER_DICTIONARYORGAN_HPP_

