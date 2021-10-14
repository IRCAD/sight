/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "modules/io/session/config.hpp"

#include <core/com/Signal.hpp>
#include <core/jobs/IJob.hpp>

#include <io/base/service/IWriter.hpp>

namespace sight::module::io::session
{

/**
 * @brief Session writer.
 *
 * @details Service to recursively write a data object tree, including all fields, to a session file.
 * The session file is indeed a standard "ZIP" archive, while the compression algorithm for files inside
 * the session archive is ZSTD. A standard archive reader could open a session file, if it is able to handle
 * ZIP archive with ZSTD compression.
 *
 * The archive can be password protected using AES256 algorithm. Depending of the chosen encryption policy, the files
 * can be protected, and thus not be readable from external archive reader, even if no password (encryption=forced) are
 * provided.
 *
 * The compression level is set individually, depending of the type of data to serialize.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(core::jobs::IJob)): emitted to display a progress bar while the image is written (it should be
 * connected to a SJobBar).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::io::session::SWriter">
        <in key="data" uid="..." />
        <dialog extension=".sample" description="Sample Sight session file" policy="once"/>
        <password policy="once", encryption=salted/>
        <archive format="optimized"/>
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b data [sight::data::Object]: object to write.
 *
 * @subsection Configuration Configuration
 * - \b dialog(optional):
 *      \b extension: defines the file extension that will be used for the session file.
 *      If the extension is not specified, default ".zip" will be used.
 *      \b description: allows to display a label in front of extension when the file dialog is shown.
 *      \b policy:
 *          - \b "never": never show the open dialog
 *          - \b "once": show only once, store the location as long as the service is started
 *          - \b "always": always show the location dialog
 *          - \b "default": default behavior, which is "never"
 *
 * - \b password(optional):
 *      \b policy: defines if we should protect the session file using a password and when to ask for it. It could be:
 *          - \b "never": a password will never be asked and the session file will never be encrypted.
 *          - \b "once": a password will be asked only once and stored in the memory for subsequent uses.
 *                      The session file will be encrypted.
 *          - \b "always": a password will always be asked.
 *                      The session file will be encrypted.
 *          - \b "default": uses the builtin default behavior which is "never".
 *
 *       \b encryption: defines if we should use password as is or salt it a bit to make . It could be:
 *          - \b "password": uses password as is.
 *          - \b "salted": uses password, but salt it. It means that encrypted file could not be open outside Sight.
 *          - \b "force": force encryption, even without password. Use a pseudo-random hidden password, if
 *                        no password are provided
 *          - \b "default": uses the builtin default behavior which is "password".
 *
 * - \b archive(optional):
 *      \b format: defines the archive format.
 *          - \b "filesystem": Use the filesystem to store files.
 *          - \b "compatible": Store files in a ZIP archive, with old deflate algorithm
 *          - \b "optimized": Store files in a ZIP archive, with zstd algorithm
 *          - \b "default": uses the builtin default behavior which is "optimized"
 *
 * @see sight::io::base::service::IWriter
 * @see sight::io::session::SessionWriter
 */

class MODULE_IO_SESSION_CLASS_API SWriter final : public sight::io::base::service::IWriter
{
public:

    SIGHT_DECLARE_SERVICE(SWriter, sight::io::base::service::IWriter);

    using JobCreatedSignal = core::com::Signal<void (core::jobs::IJob::sptr)>;

    MODULE_IO_SESSION_API SWriter() noexcept;

    MODULE_IO_SESSION_API ~SWriter() noexcept;

    /// Propose to create a medical data file
    MODULE_IO_SESSION_API void openLocationDialog() override;

protected:

    /// Parses the configuration
    MODULE_IO_SESSION_API void configuring() override;

    /// Does nothing
    MODULE_IO_SESSION_API void starting() override;

    /// Does nothing
    MODULE_IO_SESSION_API void stopping() override;

    /// Writes session data to filesystem
    MODULE_IO_SESSION_API void updating() override;

    /// Returns managed path type, here service manages only single file
    MODULE_IO_SESSION_API sight::io::base::service::IOPathType getIOPathType() const override
    {
        return sight::io::base::service::FILE;
    }

private:

    class WriterImpl;
    std::unique_ptr<WriterImpl> m_pimpl;
};

} // namespace sight::module::io::session
