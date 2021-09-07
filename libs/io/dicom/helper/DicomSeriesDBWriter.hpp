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

#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

#include <string>

namespace sight::data
{

class SeriesDB;

}

namespace sight::core::jobs
{

class Aggregator;

}

namespace sight::io::dicom
{

namespace helper
{

class DicomAnonymizer;

class IO_DICOM_CLASS_API DicomSeriesDBWriter :
    public io::base::writer::GenericObjectWriter<data::SeriesDB>,
    public core::location::SingleFolder,
    public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(
        DicomSeriesDBWriter,
        io::base::writer::GenericObjectWriter<data::SeriesDB>,
        io::base::writer::factory::New<DicomSeriesDBWriter>
    );
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @brief Construtor/Destructor
     * @{ */
    IO_DICOM_API DicomSeriesDBWriter(io::base::writer::IObjectWriter::Key key);
    IO_DICOM_API virtual ~DicomSeriesDBWriter();
    /**  @} */

    /// Return an empty string
    IO_DICOM_API std::string extension() override;

    /// Get job Aggregator
    IO_DICOM_API SPTR(core::jobs::Aggregator) getAggregator();

    /// Set anonymizer (if anonymization required)
    IO_DICOM_API void setAnonymizer(const SPTR(helper::DicomAnonymizer)& anonymizer);

    /// Write the image series in DICOM format.
    IO_DICOM_API void write() override;

    /// Enable Zip output format
    IO_DICOM_API void enableZippedArchive(bool enable);

    /// Set Producer
    IO_DICOM_API void setProducer(std::string producer);

private:

    /// Job observer
    SPTR(core::jobs::Aggregator) m_aggregator;

    /// Optionnal anonymiser
    SPTR(DicomAnonymizer) m_anonymizer;

    /// Enable zip archive
    bool m_enableZippedArchive;

    /// Producer
    std::string m_producer;
};

} // namespace helper

} // namespace sight::io::dicom
