/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWGDCMIO_HELPER_DICOMSERIESANONYMIZER_HPP__
#define __FWGDCMIO_HELPER_DICOMSERIESANONYMIZER_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/helper/DicomAnonymizer.hpp"
#include "fwGdcmIO/helper/DicomSeriesWriter.hpp"
#include "fwGdcmIO/reader/SeriesDB.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwJobs
{
class IJob;
class Aggregator;
}

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief This class contains helpers to anonymize DicomSeries
 */
class FWGDCMIO_CLASS_API DicomSeriesAnonymizer
{
public:

    fwCoreClassFactoryMacro((DicomSeriesAnonymizer), (()), new DicomSeriesAnonymizer );

    /// Constructor
    FWGDCMIO_API DicomSeriesAnonymizer();

    /// Destructor
    FWGDCMIO_API virtual ~DicomSeriesAnonymizer();

    /// Anonymize the DicomSeries (modify the current object)
    FWGDCMIO_API void anonymize(const ::fwMedData::DicomSeries::sptr& source);

    /// Anonymize the DicomSeries (doesn't modify the current object)
    FWGDCMIO_API void anonymize(const ::fwMedData::DicomSeries::sptr& source,
                                const ::fwMedData::DicomSeries::sptr& destination);

    /// Get job observer
    FWGDCMIO_API SPTR(::fwJobs::Aggregator) getJob() const;

protected:

    /// Dicom writer
    ::fwGdcmIO::helper::DicomSeriesWriter::sptr m_writer;

    /// Dicom anonymizer
    ::fwGdcmIO::helper::DicomAnonymizer m_anonymizer;

    /// Dicom Reader
    ::fwGdcmIO::reader::SeriesDB::sptr m_reader;

    /// Job observer
    SPTR(::fwJobs::Aggregator) m_job;

};

} // namespace helper
} // namespace fwGdcmIO

#endif /*__FWGDCMIO_HELPER_DICOMSERIESANONYMIZER_HPP__*/
