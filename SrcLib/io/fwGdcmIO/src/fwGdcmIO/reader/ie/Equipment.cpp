/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwGdcmIO/reader/ie/Equipment.hpp"

#include "fwGdcmIO/helper/DicomDataReader.hxx"

#include <fwMedData/DicomSeries.hpp>

#include <fwRuntime/profile/Profile.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Equipment::Equipment(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                     const SPTR(::gdcm::Reader)& reader,
                     const ::fwGdcmIO::container::DicomInstance::sptr& instance,
                     const ::fwMedData::Equipment::sptr& equipment,
                     const ::fwLog::Logger::sptr& logger,
                     ProgressCallback progress,
                     CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Equipment >(dicomSeries, reader, instance, equipment,
                                                                        logger, progress, cancel)
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
    const std::string& institutName =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x0080 >(dataset);
    m_object->setInstitutionName(institutName);

    // Manufacturer - Type 2
    // NOTE: Not used in FW4SPL

    // Manufacturer's Model Name - Type 3 (Type 1 for EnhancedGeneralEquipmentModule)
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
