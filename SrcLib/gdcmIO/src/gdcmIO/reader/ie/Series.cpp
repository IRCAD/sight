/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>
#include <boost/algorithm/string/split.hpp>

#include <gdcmUIDGenerator.h>

#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>

#include "gdcmIO/helper/DicomData.hpp"
#include "gdcmIO/reader/ie/Series.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Series::Series(::fwDicomData::DicomSeries::sptr dicomSeries,
               SPTR(::gdcm::Reader)reader,
               SPTR(::gdcmIO::container::DicomInstance)instance,
               ::fwMedData::Series::sptr series) :
    ::gdcmIO::reader::ie::InformationEntity< ::fwMedData::Series >(dicomSeries, reader, instance, series)
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
    ::gdcm::DataSet &dataset = m_reader->GetFile().GetDataSet();

    // Serie's instance UID - Type 1
    const std::string& instanceUID = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0020, 0x000e >(dataset);
    m_object->setInstanceUID(instanceUID);

    // Series's modality - Type 1
    const std::string& modality = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x0060 >(dataset);
    m_object->setModality(modality);

    // Serie's date - Type 3
    const std::string& date = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x0021 >(dataset);
    m_object->setDate(date);

    // Serie's time - Type 3
    const std::string& time = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x0031 >(dataset);
    m_object->setTime(time);

    // Serie's description
    const std::string& description = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x103e >(dataset);
    m_object->setDescription(description);

    // Serie's number - Type 2
    // NOTE: Not used in FW4SPL

    // Performing physicians name - Type 3
    const std::string& performingPhysicianNamesStr = ::gdcmIO::helper::DicomData::getTrimmedTagValue< 0x0008, 0x1050 >(
        dataset);
    ::fwMedData::DicomValuesType performingPhysicianNames;
    ::boost::split( performingPhysicianNames, performingPhysicianNamesStr, ::boost::is_any_of("\\"));
    m_object->setPerformingPhysiciansName(performingPhysicianNames);

    // Laterality - Type 2C
    // NOTE: Not used in FW4SPL

    // Patient Position - Type 2C
    // NOTE: Not used in FW4SPL
}

} // namespace ie
} // namespace reader
} // namespace gdcmIO
