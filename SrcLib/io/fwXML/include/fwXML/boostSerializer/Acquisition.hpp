/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_BOOSTSERIALIZER_ACQUISITION_HPP_
#define _FWXML_BOOSTSERIALIZER_ACQUISITION_HPP_

#include <fwCore/base.hpp>
#include <fwTools/fromIsoExtendedString.hpp>

#include <fwData/Acquisition.hpp>
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
void load(Archive & ar, ::fwData::Acquisition & _acquisition, const unsigned int version)
{
    ar &  ::boost::serialization::make_nvp( "BitsPerPixel" , _acquisition.getRefBitsPerPixel() );

    ar &  ::boost::serialization::make_nvp( "SliceThickness" , _acquisition.getRefSliceThickness() );

    ar &  ::boost::serialization::make_nvp( "Axe" , _acquisition.getRefAxe() );

    ar &  ::boost::serialization::make_nvp( "UnsignedFlag" , _acquisition.getRefUnsignedFlag() );

    ar &  ::boost::serialization::make_nvp( "AcquisitionIndex" , _acquisition.getRefAcquisitionIndex() );

    ar &  ::boost::serialization::make_nvp( "ImageType" , _acquisition.getRefImageType() );

    ar &  ::boost::serialization::make_nvp( "ImageFormat" , _acquisition.getRefImageFormat() );

    std::string creationDate;
    ar &  ::boost::serialization::make_nvp( "CreationDate" , creationDate );
    _acquisition.setCRefCreationDate( fwTools::fromIsoExtendedString( creationDate ) );

    ar &  ::boost::serialization::make_nvp( "IsMain" , _acquisition.getRefIsMain() );

    ar &  ::boost::serialization::make_nvp( "IsNormalDir" , _acquisition.getRefIsNormalDir() );

    ar &  ::boost::serialization::make_nvp( "UID" , _acquisition.getRefUID() );

    ar &  ::boost::serialization::make_nvp( "DbID" , _acquisition.getRefDbID() );

    ar &  ::boost::serialization::make_nvp( "LaboID" , _acquisition.getRefLaboID() );

    ar &  ::boost::serialization::make_nvp( "NetID" , _acquisition.getRefNetID() );

    std::string dateSendToLaboAt;
    ar &  ::boost::serialization::make_nvp( "DateSendToLaboAt" , dateSendToLaboAt );
    _acquisition.setCRefDateSendToLaboAt( fwTools::fromIsoExtendedString( dateSendToLaboAt ) );

    std::string dateReceiveFromLaboAt;
    ar &  ::boost::serialization::make_nvp( "DateReceiveFromLaboAt" , dateReceiveFromLaboAt );
    _acquisition.setCRefDateReceiveFromLaboAt( fwTools::fromIsoExtendedString( dateReceiveFromLaboAt ) );

    std::string dateSendToBDDAt;
    ar &  ::boost::serialization::make_nvp( "DateSendToBDDAt" , dateSendToBDDAt );
    _acquisition.setCRefDateSendToBDDAt( fwTools::fromIsoExtendedString( dateSendToBDDAt ) );

    std::string dateDisponibilityAt;
    ar &  ::boost::serialization::make_nvp( "DateDisponibilityAt" , dateDisponibilityAt );
    _acquisition.setCRefDateDisponibilityAt( fwTools::fromIsoExtendedString( dateDisponibilityAt ) );

    ar &  ::boost::serialization::make_nvp( "PatientSize" , _acquisition.getRefPatientSize() );

    ar &  ::boost::serialization::make_nvp( "PatientWeight" , _acquisition.getRefPatientWeight() );

    ar &  ::boost::serialization::make_nvp( "Radiations" , _acquisition.getRefRadiations() );

    ar &  ::boost::serialization::make_nvp( "MedicalPrinter" , _acquisition.getRefMedicalPrinter() );

    ar &  ::boost::serialization::make_nvp( "MedicalPrinterCorp" , _acquisition.getRefMedicalPrinterCorp() );

    ar &  ::boost::serialization::make_nvp( "PatientPosition" , _acquisition.getRefPatientPosition() );
}

template<class Archive>
void save(Archive & ar, const ::fwData::Acquisition & _acquisition, const unsigned int version)
{
    std::string newStr;

    ar &  ::boost::serialization::make_nvp( "BitsPerPixel" , _acquisition.getCRefBitsPerPixel() );

    ar &  ::boost::serialization::make_nvp( "SliceThickness" , _acquisition.getCRefSliceThickness() );

    ar &  ::boost::serialization::make_nvp( "Axe" , _acquisition.getCRefAxe() );

    ar &  ::boost::serialization::make_nvp( "UnsignedFlag" , _acquisition.getCRefUnsignedFlag() );

    ar &  ::boost::serialization::make_nvp( "AcquisitionIndex" , _acquisition.getCRefAcquisitionIndex() );

    ar &  ::boost::serialization::make_nvp( "ImageType" , _acquisition.getCRefImageType() );

    ar &  ::boost::serialization::make_nvp( "ImageFormat" , _acquisition.getCRefImageFormat() );

    std::string creationDate = fwTools::toIsoExtendedString(_acquisition.getCRefCreationDate());
    ar &  ::boost::serialization::make_nvp( "CreationDate" , creationDate );

    ar &  ::boost::serialization::make_nvp( "IsMain" , _acquisition.getCRefIsMain() );

    ar &  ::boost::serialization::make_nvp( "IsNormalDir" , _acquisition.getCRefIsNormalDir() );

    ar &  ::boost::serialization::make_nvp( "UID" , _acquisition.getCRefUID() );

    ar &  ::boost::serialization::make_nvp( "DbID" , _acquisition.getCRefDbID() );

    ar &  ::boost::serialization::make_nvp( "LaboID" , _acquisition.getCRefLaboID() );

    ar &  ::boost::serialization::make_nvp( "NetID" , _acquisition.getCRefNetID() );

    std::string dateSendToLaboAt = fwTools::toIsoExtendedString(_acquisition.getCRefDateSendToLaboAt());
    ar &  ::boost::serialization::make_nvp( "DateSendToLaboAt" , dateSendToLaboAt );

    std::string dateReceiveFromLaboAt = fwTools::toIsoExtendedString(_acquisition.getCRefDateReceiveFromLaboAt());
    ar &  ::boost::serialization::make_nvp( "DateReceiveFromLaboAt" , dateReceiveFromLaboAt );

    std::string dateSendToBDDAt = fwTools::toIsoExtendedString(_acquisition.getCRefDateSendToBDDAt());
    ar &  ::boost::serialization::make_nvp( "DateSendToBDDAt" , dateSendToBDDAt );

    std::string dateDisponibilityAt = fwTools::toIsoExtendedString(_acquisition.getCRefDateDisponibilityAt());
    ar &  ::boost::serialization::make_nvp( "DateDisponibilityAt" , dateDisponibilityAt );

    ar &  ::boost::serialization::make_nvp( "PatientSize" , _acquisition.getCRefPatientSize() );

    ar &  ::boost::serialization::make_nvp( "PatientWeight" , _acquisition.getCRefPatientWeight() );

    ar &  ::boost::serialization::make_nvp( "Radiations" , _acquisition.getCRefRadiations() );

    ar &  ::boost::serialization::make_nvp( "MedicalPrinter" , _acquisition.getCRefMedicalPrinter() );

    ar &  ::boost::serialization::make_nvp( "MedicalPrinterCorp" , _acquisition.getCRefMedicalPrinterCorp() );

    ar &  ::boost::serialization::make_nvp( "PatientPosition" , _acquisition.getCRefPatientPosition() );
}


/// serialize image via boost.org serialization framework (split load/save)
template<class Archive>
void serialize(Archive & ar, ::fwData::Acquisition & _study, const unsigned int version)
{
    // inform for serializer that this class is a subclass of a polymorphic one. Hence ptr serialisation of the base one
    // can be well casted to the derivated one during the deserialisation
    ::boost::serialization::void_cast_register<  ::fwData::Acquisition, fwTools::Object>(NULL,NULL);

    split_free(ar,_study,version); // call load or save depending of archive type
}


} } // end namespace


#endif // _FWXML_BOOSTSERIALIZER_ACQUISITION_HPP_

