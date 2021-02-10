/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "io/dicom/config.hpp"
#include "io/dicom/helper/DicomAnonymizer.hpp"
#include "io/dicom/helper/DicomSeriesWriter.hpp"
#include "io/dicom/reader/SeriesDB.hpp"

#include <data/DicomSeries.hpp>

namespace sight::core::jobs
{
class IJob;
class Aggregator;
}

namespace sight::io::dicom
{
namespace helper
{

/**
 * @brief This class contains helpers to anonymize DicomSeries
 */
class IO_DICOM_CLASS_API DicomSeriesAnonymizer
{
public:

    fwCoreClassMacro(DicomSeriesAnonymizer)

    //------------------------------------------------------------------------------

    static sptr New()
    {
        return std::make_shared<DicomSeriesAnonymizer>();
    }

    /// Constructor
    IO_DICOM_API DicomSeriesAnonymizer();

    /// Destructor
    IO_DICOM_API virtual ~DicomSeriesAnonymizer();

    /// Anonymize the DicomSeries (modify the current object)
    IO_DICOM_API void anonymize(const data::DicomSeries::sptr& source);

    /// Anonymize the DicomSeries (doesn't modify the current object)
    IO_DICOM_API void anonymize(const data::DicomSeries::sptr& source,
                                const data::DicomSeries::sptr& destination);

    /// Get job observer
    IO_DICOM_API SPTR(core::jobs::Aggregator) getJob() const;

protected:

    /// Dicom writer
    io::dicom::helper::DicomSeriesWriter::sptr m_writer;

    /// Dicom anonymizer
    io::dicom::helper::DicomAnonymizer m_anonymizer;

    /// Dicom Reader
    io::dicom::reader::SeriesDB::sptr m_reader;

    /// Job observer
    SPTR(core::jobs::Aggregator) m_job;

};

} // namespace helper
} // namespace sight::io::dicom
