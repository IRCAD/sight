/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGdcmIO/helper/DicomData.hpp"
#include "fwGdcmIO/reader/ie/Equipment.hpp"

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwRuntime/profile/Profile.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Equipment::Equipment(::fwMedData::DicomSeries::sptr dicomSeries,
                     SPTR(::gdcm::Reader)reader,
                     SPTR(::fwGdcmIO::container::DicomInstance)instance,
                     ::fwMedData::Equipment::sptr equipment,
                     ::fwLog::Logger::sptr logger,
                     const ProgressCallback& callback,
                     const bool& cancelled) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Equipment >(dicomSeries, reader, instance, equipment,
                                                                        logger, callback, cancelled)
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
    const ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // Institution Name - Type 3
    const std::string& institutName = ::fwGdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x0080 >(dataset);
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
