/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/io/dicom/config.hpp>

#include "io/dicom/helper/dicom_anonymizer.hpp"
#include "io/dicom/helper/dicom_series_writer.hpp"
#include "io/dicom/reader/series_set.hpp"

#include <data/dicom_series.hpp>

namespace sight::core::jobs
{

class base;
class aggregator;

} // namespace sight::core::jobs

namespace sight::io::dicom::helper
{

/**
 * @brief This class contains helpers to anonymize DicomSeries
 */
class SIGHT_IO_DICOM_CLASS_API dicom_series_anonymizer
{
public:

    SIGHT_DECLARE_CLASS(dicom_series_anonymizer);

    /// Constructor
    SIGHT_IO_DICOM_API dicom_series_anonymizer();

    /// Destructor
    SIGHT_IO_DICOM_API virtual ~dicom_series_anonymizer();

    /// Anonymize the DicomSeries (modify the current object)
    SIGHT_IO_DICOM_API void anonymize(const data::dicom_series::sptr& _source);

    /// Anonymize the DicomSeries (doesn't modify the current object)
    SIGHT_IO_DICOM_API void anonymize(
        const data::dicom_series::sptr& _source,
        const data::dicom_series::sptr& _destination
    );

    /// Get job observer
    [[nodiscard]] SIGHT_IO_DICOM_API SPTR(core::jobs::aggregator) get_job() const;

protected:

    /// Dicom writer
    io::dicom::helper::dicom_series_writer::sptr m_writer;

    /// Dicom anonymizer
    io::dicom::helper::dicom_anonymizer m_anonymizer;

    /// Dicom Reader
    io::dicom::reader::series_set::sptr m_reader;

    /// Job observer
    SPTR(core::jobs::aggregator) m_job;
};

} // namespace sight::io::dicom::helper
