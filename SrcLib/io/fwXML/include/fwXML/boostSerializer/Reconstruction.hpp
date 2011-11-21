/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _FWXML_BOOSTSERIALIZER_RECONSTRUCTION_HPP_
#define _FWXML_BOOSTSERIALIZER_RECONSTRUCTION_HPP_

#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>
#include <fwData/Image.hpp>

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
void load(Archive & ar, ::fwData::Reconstruction & _reconstruction, const unsigned int version)
{
    ar &  ::boost::serialization::make_nvp( "IsVisible" , _reconstruction.getRefIsVisible() );
    ar &  ::boost::serialization::make_nvp( "Red" , _reconstruction.getMaterial()->ambient()->red() );
    ar &  ::boost::serialization::make_nvp( "Green" , _reconstruction.getMaterial()->ambient()->green() );
    ar &  ::boost::serialization::make_nvp( "Blue" , _reconstruction.getMaterial()->ambient()->blue() );
    ar &  ::boost::serialization::make_nvp( "Alpha" , _reconstruction.getMaterial()->ambient()->alpha() );
    ar &  ::boost::serialization::make_nvp( "ReconstructionFormat" , _reconstruction.getRefReconstructionFormat() );
    ar &  ::boost::serialization::make_nvp( "OrganName" , _reconstruction.getRefOrganName() );
    ar &  ::boost::serialization::make_nvp( "StructureType" , _reconstruction.getRefStructureType() );

    int isClosed = -1;
    ar &  ::boost::serialization::make_nvp( "IsClosed" , isClosed );
    switch(isClosed)
    {
    case 0:
        _reconstruction.getRefIsClosed() = false;
        break;
    case 1:
        _reconstruction.getRefIsClosed() = true;
        break;
    default:
        _reconstruction.getRefIsClosed() = ::boost::logic::indeterminate ;
    }

    ar &  ::boost::serialization::make_nvp( "IsAutomatic" , _reconstruction.getRefIsAutomatic() );
    ar &  ::boost::serialization::make_nvp( "AvgVolume" , _reconstruction.getRefAvgVolume() );
    ar &  ::boost::serialization::make_nvp( "VolStdDeviation" , _reconstruction.getRefVolStdDeviation() );
    ar &  ::boost::serialization::make_nvp( "VolPctConfidence" , _reconstruction.getRefVolPctConfidence() );
    ar &  ::boost::serialization::make_nvp( "ReconstructionTime" , _reconstruction.getRefReconstructionTime() );
    ar &  ::boost::serialization::make_nvp( "MaskGenerated" , _reconstruction.getRefMaskGenerated() );
    ar &  ::boost::serialization::make_nvp( "Level" , _reconstruction.getRefLevel() );
    ar &  ::boost::serialization::make_nvp( "Label" , _reconstruction.getRefLabel() );
    ar &  ::boost::serialization::make_nvp( "Generated3D" , _reconstruction.getRefGenerated3D() );
    ar &  ::boost::serialization::make_nvp( "Type3D" , _reconstruction.getRefType3D() );

    std::string path;
    ar &  ::boost::serialization::make_nvp( "Path" , path );
    _reconstruction.setCRefPath( ::boost::filesystem::path(path) );

    ar &  ::boost::serialization::make_nvp( "DbID" , _reconstruction.getRefDbID() );
}

template<class Archive>
void save(Archive & ar, const ::fwData::Reconstruction & _reconstruction, const unsigned int version)
{
    std::string newStr;
    ar &  ::boost::serialization::make_nvp( "IsVisible" , _reconstruction.getCRefIsVisible() );
    ar &  ::boost::serialization::make_nvp( "Red" , _reconstruction.getMaterial()->ambient()->red() );
    ar &  ::boost::serialization::make_nvp( "Green" , _reconstruction.getMaterial()->ambient()->green() );
    ar &  ::boost::serialization::make_nvp( "Blue" , _reconstruction.getMaterial()->ambient()->blue() );
    ar &  ::boost::serialization::make_nvp( "Alpha" , _reconstruction.getMaterial()->ambient()->alpha() );
    ar &  ::boost::serialization::make_nvp( "ReconstructionFormat" , _reconstruction.getCRefReconstructionFormat() );
    ar &  ::boost::serialization::make_nvp( "OrganName" , _reconstruction.getCRefOrganName() );
    ar &  ::boost::serialization::make_nvp( "StructureType" , _reconstruction.getCRefStructureType() );

    int isClosed = ( ::boost::logic::indeterminate(_reconstruction.getCRefIsClosed()) ) ? -1 : ( (_reconstruction.getCRefIsClosed()) ? 1 : 0 );
    ar &  ::boost::serialization::make_nvp( "IsClosed" , isClosed);

    ar &  ::boost::serialization::make_nvp( "IsAutomatic" , _reconstruction.getCRefIsAutomatic() );
    ar &  ::boost::serialization::make_nvp( "AvgVolume" , _reconstruction.getCRefAvgVolume() );
    ar &  ::boost::serialization::make_nvp( "VolStdDeviation" , _reconstruction.getCRefVolStdDeviation() );
    ar &  ::boost::serialization::make_nvp( "VolPctConfidence" , _reconstruction.getCRefVolPctConfidence() );
    ar &  ::boost::serialization::make_nvp( "ReconstructionTime" ,  _reconstruction.getCRefReconstructionTime() );
    ar &  ::boost::serialization::make_nvp( "MaskGenerated" , _reconstruction.getCRefMaskGenerated() );
    ar &  ::boost::serialization::make_nvp( "Level" , _reconstruction.getCRefLevel() );
    ar &  ::boost::serialization::make_nvp( "Label" , _reconstruction.getCRefLabel() );
    ar &  ::boost::serialization::make_nvp( "Generated3D" , _reconstruction.getCRefGenerated3D() );
    ar &  ::boost::serialization::make_nvp( "Type3D" , _reconstruction.getCRefType3D() );

    std::string path = _reconstruction.getCRefPath().string();
    ar &  ::boost::serialization::make_nvp( "Path" , path );

    ar &  ::boost::serialization::make_nvp( "DbID" , _reconstruction.getCRefDbID() );
}


/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Reconstruction & _study, const unsigned int version)
{
    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
    ::boost::serialization::void_cast_register< ::fwData::Reconstruction, ::fwTools::Object>(NULL,NULL);

    split_free(ar,_study,version); // call load or save depending of archive type
}


} // namespace serialization
} // namespace boost

INSTANTIATE_SERIALIZER(::fwData::Reconstruction);

#endif // _FWXML_BOOSTSERIALIZER_RECONSTRUCTION_HPP_

