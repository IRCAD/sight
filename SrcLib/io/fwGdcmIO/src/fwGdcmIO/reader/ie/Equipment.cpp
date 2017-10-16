/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomDataReader.hxx"
#include "fwGdcmIO/reader/ie/Equipment.hpp"

#include <fwMedData/DicomSeries.hpp>
#include <fwRuntime/profile/Profile.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Equipment::Equipment(const ::fwMedData::DicomSeries::sptr& dicomSeries,
                     const SPTR(::gdcm::Reader)& reader,
                     const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                     const ::fwMedData::Equipment::sptr& equipment,
                     const ::fwLog::Logger::sptr& logger,
                     ProgressCallback progress,
                     CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Equipment >(dicomSeries, reader, instance, equipment, logger, progress, cancel)
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
    const ::gdcm::DataSet &dataset = m_reader->GetFile().GetDataSet();

    // Institution Name - Type 3
    const std::string& institutName =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x0080 >(dataset);
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
} // namespace fwGdcmIO
