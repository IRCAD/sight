/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/location/SingleFolder.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

#include <string>

namespace sight::core::jobs
{

class Observer;
class IJob;

} // namespace sight::core::jobs

namespace sight::data
{

class DicomSeries;

} // namespace sight::data

namespace sight::io::zip
{

class IWriteArchive;

} // namespace sight::io::zip

namespace sight::io::dicom::helper
{

class DicomAnonymizer;

/**
 * @brief Write a DicomSeries in DICOM format.
 *
 * This writer optionally manages anonymization if an instance of DicomAnonymizer is set.
 * The output DICOM files can be exported into an archive (folder or zipped file) if output archive is set.
 *
 * If the DicomSeries contains the binaries of the DICOM files, the files are extracted (or zipped).
 * If the DicomSeries contains the paths of the DICOM files, a simple copy (or zip) is performed.
 */
class DicomSeriesWriter : public io::base::writer::GenericObjectWriter<data::DicomSeries>,
                          public core::location::SingleFolder
{
public:

    SIGHT_DECLARE_CLASS(
        DicomSeriesWriter,
        io::base::writer::GenericObjectWriter<data::DicomSeries>,
        io::base::writer::factory::New<DicomSeriesWriter>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Constructor. Does nothing
    IO_DICOM_API DicomSeriesWriter(io::base::writer::IObjectWriter::Key key);

    /// Write the image series in DICOM format.
    IO_DICOM_API void write() override;

    /// Return an empty string
    IO_DICOM_API std::string extension() const override;

    /// Get job observer
    IO_DICOM_API SPTR(core::jobs::IJob) getJob() const override;

    /// Defines optional anonymizer.
    IO_DICOM_API void setAnonymizer(const SPTR(helper::DicomAnonymizer)& anonymizer);

    /**
     * @brief Defines output archive for DICOM files.
     *
     * @param archive Archive instance
     * @param subPath Path within archive where DICOM files are saved (only if an anonymizer instance is defined)
     */
    IO_DICOM_API void setOutputArchive(
        const SPTR(io::zip::IWriteArchive)& archive,
        const std::string& subPath = ""
    );

protected:

    /// Compute DICOM filename according to anonymizer or return default filename.
    std::string getFilename(const std::size_t& instanceIndex);

    /// Process inputStream to outputStream with anonymization management.
    void processStream(std::istream& inputStream, std::ostream& outputStream);

    /// Process write on archive
    void processWriteArchive();

    /// Process write with Standard output stream
    void processWrite();

    /// Write count
    int m_writeCount {0};

    /// Job observer
    SPTR(core::jobs::Observer) m_job;

    /// Optional anonymiser
    SPTR(DicomAnonymizer) m_anonymizer;

    /// Optional output archive
    SPTR(io::zip::IWriteArchive) m_archive;

    /// Optional subPath (related to write archive
    std::string m_subPath;
};

} // namespace sight::io::dicom::helper
