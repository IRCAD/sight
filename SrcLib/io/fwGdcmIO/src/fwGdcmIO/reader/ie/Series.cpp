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

#include "fwGdcmIO/reader/ie/Series.hpp"

#include "fwGdcmIO/helper/DicomDataReader.hxx"

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <sstream>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Series::Series(const ::fwMedData::DicomSeries::csptr& dicomSeries,
               const SPTR(::gdcm::Reader)& reader,
               const ::fwGdcmIO::container::DicomInstance::sptr& instance,
               const ::fwMedData::Series::sptr& series,
               const ::fwLog::Logger::sptr& logger,
               ProgressCallback progress,
               CancelRequestedCallback cancel) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Series >(dicomSeries, reader, instance, series,
                                                                     logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

Series::~Series()
{
}

//------------------------------------------------------------------------------

void Series::readGeneralSeriesModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_reader->GetFile().GetDataSet();

    // Serie's instance UID - Type 1
    const std::string& instanceUID = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0020, 0x000e >(dataset);
    m_object->setInstanceUID(instanceUID);

    // Series's modality - Type 1
    const std::string& modality = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x0060 >(dataset);
    m_object->setModality(modality);

    // Serie's date - Type 3
    const std::string& date = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x0021 >(dataset);
    m_object->setDate(date);

    // Serie's time - Type 3
    const std::string& time = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x0031 >(dataset);
    m_object->setTime(time);

    // Serie's description
    const std::string& description = ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x103e >(dataset);
    m_object->setDescription(description);

    // Serie's number - Type 2
    // NOTE: Not used in Sight

    // Performing physicians name - Type 3
    const std::string& performingPhysicianNamesStr =
        ::fwGdcmIO::helper::DicomDataReader::getTagValue< 0x0008, 0x1050 >(dataset);

    if(!performingPhysicianNamesStr.empty())
    {
        ::fwMedData::DicomValuesType performingPhysicianNames;
        ::boost::split( performingPhysicianNames, performingPhysicianNamesStr, ::boost::is_any_of("\\"));
        m_object->setPerformingPhysiciansName(performingPhysicianNames);
    }

    // Laterality - Type 2C
    // NOTE: Not used in Sight

    // Patient Position - Type 2C
    // NOTE: Not used in Sight
}

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
