/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gdcmIO/writer/DicomEquipmentWriter.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomEquipmentWriter::DicomEquipmentWriter():
        m_isEnhanced(false),
        m_instanceHasImage(false),
        m_type()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomEquipmentWriter::~DicomEquipmentWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomEquipmentWriter::write(::gdcm::DataSet & a_gDs)
{
    SLM_TRACE_FUNC();

    ::boost::shared_ptr< DicomInstance > dicomInstance = this->getDicomInstance();

    // Institution Name
    const std::string & institutName = dicomInstance->getCRefInstitutionName();
    // Not necessary because this tag is optional (type 3)
//    if (institutName.empty() /* && object set*/)
//    {
//        ::fwData::Study::csptr study = this->getConcreteObject();
//        if (study)
//            institutName = study->getHospital();
//    }
    helper::GdcmData::setTagValue<0x0008,0x0080>(institutName, a_gDs);   // Type 3
    OSLM_TRACE("Institution Name : "<<institutName);

    if (this->getInstanceHasImage())
    {
        // Pixel Padding Value  // TODO : Compute and define "Pixel Padding Value"
//        helper::GdcmData::setTagValue<short,0x0028,0x0120>(this->getPixelPaddingValue(), gDsRoot ); // Type 1C
//
//        ::gdcm::Attribute<0x0028,0x0120,::gdcm::VR::US> gAt; //FIXME: can be VR::SS
//        gAt.SetValue(0/*this->getPixelPaddingValue()*/);
//        gDsRoot.Insert(gAt.GetAsDataElement());
//
//        OSLM_TRACE("Pixel Padding Value : "<<this->getPixelPaddingValue());
    }

    if (this->getIsEnhanced())
    {// Write Enhanced General Equipment Module (see PS 3.3 C.7.5.2)
        this->writeEnhancedGeneralEquipement(a_gDs);
    }
    else
    {
        // Manufacturer
        helper::GdcmData::setTagValue<0x0008,0x0070>("IRCAD", a_gDs); // Type 2
        OSLM_TRACE("Manufacturer : " << "IRCAD");
    }
}

//------------------------------------------------------------------------------

void DicomEquipmentWriter::writeEnhancedGeneralEquipement(::gdcm::DataSet & a_gDs)
{
    // Manufacturer
    helper::GdcmData::setTagValue<0x0008,0x0070>("IRCAD", a_gDs);   // Type 1   // TODO : avoid hard coded
    OSLM_TRACE("Manufacturer : "<<"IRCAD");

    // Manufacturer’s Model Name
    helper::GdcmData::setTagValue<0x0008,0x1090>("FW4SPL", a_gDs);  // Type 1   // TODO : avoid hard coded
    OSLM_TRACE("Manufacturer’s Model Name : "<<"FW4SPL");

    // Device Serial Number
    helper::GdcmData::setTagValue<0x0018,0x1000>("", a_gDs);        // Type 1   // TODO : define "Device Serial Number"
    OSLM_TRACE("Device Serial Number : "<<"");

    // Software Versions (See Section C.7.5.1.1.3.)
    ::gdcm::Attribute<0x0018,0x1020> gAt;                           // Type 1
    gAt.SetNumberOfValues(1);
    gAt.SetValue("0.7");                                            // TODO : avoid hard coded
    a_gDs.Insert(gAt.GetAsDataElement());
    OSLM_TRACE("Software Versions : "<<"0.7");
}

//------------------------------------------------------------------------------

void DicomEquipmentWriter::setImagePyxelType(const ::fwTools::DynamicType & a_type)
{
    this->m_type = a_type;
}

}   // namespace writer

}   // namespace gdcmIO
