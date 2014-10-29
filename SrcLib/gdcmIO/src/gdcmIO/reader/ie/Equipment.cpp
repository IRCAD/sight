/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/Equipment.hpp>

#include <fwRuntime/profile/Profile.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/reader/ie/Equipment.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Equipment::Equipment(::fwDicomData::DicomSeries::sptr dicomSeries,
        SPTR(::gdcm::Reader) reader,
        SPTR(::gdcmIO::container::DicomInstance) instance,
        ::fwMedData::Equipment::sptr equipment):
        ::gdcmIO::reader::ie::InformationEntity< ::fwMedData::Equipment >(dicomSeries, reader, instance, equipment)
{
}

//------------------------------------------------------------------------------

Equipment::~Equipment()
{
}

//------------------------------------------------------------------------------

void Equipment::readGeneralEquipmentModule()
{
    // Retrieve dataset
    ::gdcm::DataSet &dataset = m_reader->GetFile().GetDataSet();

    // Institution Name - Type 3
    const std::string& institutName = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x0080 >(dataset);
    m_object->setInstitutionName(institutName);

    // Manufacturer - Type 2
    // NOTE: Not used in FW4SPL

    // Manufacturer’s Model Name - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
    // NOTE: Not used in FW4SPL

    // Device Serial Number - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
    // NOTE: Not used in FW4SPL

    // Software Versions - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
    // NOTE: Not used in FW4SPL
}

//------------------------------------------------------------------------------


} // namespace ie
} // namespace reader
} // namespace gdcmIO
