/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "modules/io/dicom/config.hpp"

#include <core/com/Signal.hpp>
#include <core/jobs/IJob.hpp>

#include <io/base/service/IReader.hpp>

namespace sight::module::io::dicom
{

class MODULE_IO_DICOM_CLASS_API SReader final : public sight::io::base::service::IReader
{
public:

    SIGHT_DECLARE_SERVICE(SReader, sight::io::base::service::IReader);

    using JobCreatedSignal = core::com::Signal<void (core::jobs::IJob::sptr)>;

    MODULE_IO_DICOM_API SReader() noexcept;

    MODULE_IO_DICOM_API ~SReader() noexcept;

    /// Propose to read a session data file
    MODULE_IO_DICOM_API void openLocationDialog() override;

protected:

    /// Does nothing
    MODULE_IO_DICOM_API void starting() override;

    /// Does nothing
    MODULE_IO_DICOM_API void stopping() override;

    /// Parses the configuration
    MODULE_IO_DICOM_API void configuring() override;

    /// Read DICOM data from filesystem
    MODULE_IO_DICOM_API void updating() override;

    /// Returns managed path type, here service manages only single file
    MODULE_IO_DICOM_API sight::io::base::service::IOPathType getIOPathType() const override
    {
        return sight::io::base::service::FOLDER;
    }

private:

    class SReaderImpl;
    std::unique_ptr<SReaderImpl> m_pimpl;
};

} // namespace sight::module::io::session
