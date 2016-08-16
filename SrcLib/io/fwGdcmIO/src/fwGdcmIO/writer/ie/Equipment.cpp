/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomData.hpp"
#include "fwGdcmIO/writer/ie/Equipment.hpp"

#include <fwMedData/Equipment.hpp>
#include <fwRuntime/profile/Profile.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

Equipment::Equipment(SPTR(::gdcm::Writer)writer,
                     SPTR(::fwGdcmIO::container::DicomInstance)instance,
                     ::fwMedData::Equipment::sptr equipment) :
    ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Equipment >(writer, instance, equipment)
{
}

//------------------------------------------------------------------------------

Equipment::~Equipment()
{
}

//------------------------------------------------------------------------------

void Equipment::writeGeneralEquipmentModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Retrieve runtime profile
    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();

    // Institution Name - Type 3
    const std::string& institutName = m_object->getInstitutionName();
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0080 >(institutName, dataset);

    // Manufacturer - Type 2
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x0070 >("FW4SPL", dataset);

    // Manufacturer’s Model Name - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0008, 0x1090 >("FW4SPL_FWGDCMIO", dataset);

    // Device Serial Number - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
    std::string deviceSerialNumber = std::string(FWGDCMIO_VER);
    ::fwGdcmIO::helper::DicomData::setTagValue< 0x0018, 0x1000 >(deviceSerialNumber, dataset);

    // Software Versions - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
    ::gdcm::Attribute< 0x0018, 0x1020 > attribute;
    attribute.SetNumberOfValues(1);
    attribute.SetValue((profile) ? (profile->getName()+" "+profile->getVersion()) : "Unknown");
    dataset.Insert(attribute.GetAsDataElement());
}

//------------------------------------------------------------------------------

void Equipment::writeEnhancedGeneralEquipmentModule()
{
    this->writeGeneralEquipmentModule();
}


} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
