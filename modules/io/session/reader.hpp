/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include <core/com/signal.hpp>
#include <core/jobs/base.hpp>

#include <io/__/service/reader.hpp>

namespace sight::module::io::session
{

/**
 * @brief Session reader.
 *
 * @details Service to read a session file and restore recursively, including all fields a data object.
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
 * - \b jobCreated(SPTR(core::jobs::base)): emitted to display a progress bar while the image is written (it should be
 * connected to a job_bar).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::io::session::reader">
        <inout key="data" uid="..." />
        <dialog extension=".sample" description="Sample Sight session file" policy="always"/>
        <password policy="once, encryption=salted"/>
        <archive format="default"/>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [sight::data::object]: object to read.
 *
 * @subsection Configuration Configuration
 * - \b dialog(optional):
 *      \b extension: defines the file extension that will be used for the session file.
 *      If the extension is not specified, default ".zip" will be used.
 *      \b description: allows to display a label in front of extension when the file dialog is shown.
 *      \b policy:
 *          - \b "never": never show the open dialog (DEFAULT)
 *          - \b "once": show only once, store the location as long as the service is started
 *          - \b "always": always show the location dialog
 *
 * - \b password(optional):
 *      \b policy: defines if we should protect the session file using a password and when to ask for it. It could be:
 *          - \b "never": a password will never be asked and the session file will never be encrypted. (DEFAULT)
 *          - \b "global": a password will be asked only if global password is not set.
 *                      The session file will be encrypted.
 *          - \b "always": a password will always be asked.
 *                      The session file will be encrypted.
 *
 *      \b encryption: defines if we should use password as is or salt it a bit to make . It could be:
 *          - \b "password": uses password as is. (DEFAULT)
 *          - \b "salted": uses password, but salt it. It means that encrypted file could not be open outside Sight.
 *          - \b "force": force encryption, even without password. Use a pseudo-random hidden password, if
 *                        no password are provided
 *
 * - \b archive(optional):
 *      \b format: defines the archive format.
 *          - \b "filesystem": Reads files from the filesystem.
 *          - \b "archive": Reads files from an session archive.
 *          - \b "default": uses the builtin default behavior which is "archive"
 *
 * @see sight::io::service::reader
 * @see sight::io::session::session_reader
 */

class MODULE_IO_SESSION_CLASS_API reader final : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(reader, sight::io::service::reader);

    struct signals
    {
        using job_created_signal_t = sight::core::com::signal<void (sight::core::jobs::base::sptr)>;
        using session_path_t       = core::com::signal<void (std::filesystem::path)>;

        using signal_t = sight::core::com::signals::key_t;
        inline static const signal_t SESSION_LOADED         = "sessionLoaded";
        inline static const signal_t SESSION_LOADING_FAILED = "sessionLoadingFailed";
    };

    MODULE_IO_SESSION_API reader() noexcept;

    MODULE_IO_SESSION_API ~reader() noexcept override;

    /// Propose to read a session data file
    MODULE_IO_SESSION_API void open_location_dialog() override;

protected:

    /// Does nothing
    MODULE_IO_SESSION_API void starting() override;

    /// Does nothing
    MODULE_IO_SESSION_API void stopping() override;

    /// Parses the configuration
    MODULE_IO_SESSION_API void configuring() override;

    /// Read session data from filesystem
    MODULE_IO_SESSION_API void updating() override;

    /// Returns managed path type, here service manages only single file
    MODULE_IO_SESSION_API sight::io::service::path_type_t get_path_type() const override
    {
        return sight::io::service::file;
    }

private:

    class ReaderImpl;
    std::unique_ptr<ReaderImpl> m_pimpl;
};

} // namespace sight::module::io::session
