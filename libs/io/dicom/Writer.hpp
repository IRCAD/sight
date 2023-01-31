/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include <core/jobs/Job.hpp>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/ProgressAdviser.hpp>

#include <data/SeriesSet.hpp>

#include <io/base/writer/GenericObjectWriter.hpp>

namespace sight::io::dicom
{

class IO_DICOM_CLASS_API Writer final : public base::writer::GenericObjectWriter<data::SeriesSet>,
                                        public core::location::SingleFolder,
                                        public core::location::SingleFile,
                                        public core::tools::ProgressAdviser
{
public:

    SIGHT_DECLARE_CLASS(
        Writer,
        base::writer::GenericObjectWriter<data::SeriesSet>,
        base::writer::factory::New<Writer>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Delete default constructors and assignment operators
    Writer()                         = delete;
    Writer(const Writer&)            = delete;
    Writer(Writer&&)                 = delete;
    Writer& operator=(const Writer&) = delete;
    Writer& operator=(Writer&&)      = delete;

    /// Constructor/Destructor
    IO_DICOM_API Writer(io::base::writer::IObjectWriter::Key key);
    IO_DICOM_API ~Writer() noexcept override;

    /// Main writing method from GenericObjectWriter
    IO_DICOM_API void write() override;

    /// Return the extension to use, by default, or the one from file set by SingleFile::setFile(), if valid
    /// @return an extension as string
    [[nodiscard]] IO_DICOM_API std::string extension() const override;

    /// Set/get the current job
    IO_DICOM_API core::jobs::IJob::sptr getJob() const override;
    IO_DICOM_API void setJob(core::jobs::Job::sptr job);

private:

    /// PImpl
    class WriterImpl;
    std::unique_ptr<WriterImpl> m_pimpl;
};

//------------------------------------------------------------------------------

inline std::string Writer::extension() const
{
    return ".dcm";
}

} // namespace sight::io::dicom
