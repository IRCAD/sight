/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gdcmIO/writer/module/DicomEquipment.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{
namespace writer
{
namespace module
{

//------------------------------------------------------------------------------

DicomEquipment::DicomEquipment():
        m_isEnhanced(false)
{
}

//------------------------------------------------------------------------------

DicomEquipment::~DicomEquipment()
{
}

//------------------------------------------------------------------------------

void DicomEquipment::write(::gdcm::DataSet & a_gDs)
{
    ::fwMedData::Equipment::csptr equipment  = this->getConcreteObject();

    // Institution Name
    const std::string& institutName = equipment->getInstitutionName();
    helper::GdcmData::setTagValue<0x0008,0x0080>(institutName, a_gDs);   // Type 3

    if (this->getIsEnhanced())
    {
        // Write Enhanced General Equipment Module (see PS 3.3 C.7.5.2)
        this->writeEnhancedGeneralEquipement(a_gDs);
    }
    else
    {
        // Manufacturer
        helper::GdcmData::setTagValue<0x0008,0x0070>("FW4SPL_GDCMIO", a_gDs); // Type 2
    }
}

//------------------------------------------------------------------------------

void DicomEquipment::writeEnhancedGeneralEquipement(::gdcm::DataSet & a_gDs)
{
    // Manufacturer
    helper::GdcmData::setTagValue<0x0008,0x0070>("FW4SPL_GDCMIO", a_gDs);   // Type 1   // TODO : avoid hard coded

    // Manufacturer’s Model Name
    helper::GdcmData::setTagValue<0x0008,0x1090>("FW4SPL", a_gDs);  // Type 1   // TODO : avoid hard coded

    // Device Serial Number
    helper::GdcmData::setTagValue<0x0018,0x1000>("fw4spl_0.9.2", a_gDs);// Type 1 // TODO: define "Device Serial Number"

    // Software Versions (See Section C.7.5.1.1.3.)
    ::gdcm::Attribute<0x0018,0x1020> gAt;                           // Type 1
    gAt.SetNumberOfValues(1);
    gAt.SetValue("0.7");                                            // TODO : avoid hard coded
    a_gDs.Insert(gAt.GetAsDataElement());
}


} // namespace module
} // namespace writer
} // namespace gdcmIO
