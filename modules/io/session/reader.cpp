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

// cspell:ignore NOLINT

#include "modules/io/session/reader.hpp"

#include <core/com/signal.hxx>
#include <core/crypto/password_keeper.hpp>
#include <core/crypto/secure_string.hpp>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/system.hpp>

#include <io/session/SessionReader.hpp>
#include <io/zip/exception/Read.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

namespace sight::module::io::session
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::Archive;

/// Private reader implementation
class reader::ReaderImpl
{
public:

    /// Delete default constructors and assignment operators
    ReaderImpl(const ReaderImpl&)            = delete;
    ReaderImpl(ReaderImpl&&)                 = delete;
    ReaderImpl& operator=(const ReaderImpl&) = delete;
    ReaderImpl& operator=(ReaderImpl&&)      = delete;

    /// Constructor
    inline explicit ReaderImpl(reader* const _reader) noexcept :
        M_READER(_reader),
        m_job_created_signal(_reader->new_signal<signals::JobCreatedSignal>("jobCreated"))
    {
    }

    /// Default destructor
    inline ~ReaderImpl() noexcept = default;

    /// Pointer to the public interface
    reader* const M_READER;

    /// Extension name to use for session file
    std::string m_extension_name {".zip"};

    /// Extension description to use for file save dialog
    std::string m_extension_description {"Sight session"};

    /// Dialog policy to use for the file location
    DialogPolicy m_dialog_policy = {DialogPolicy::NEVER};

    /// Password policy to use
    password_keeper::password_policy m_password_policy {password_keeper::password_policy::NEVER};

    /// Encryption policy to use
    password_keeper::encryption_policy m_encryption_policy {password_keeper::encryption_policy::PASSWORD};

    /// Archive format to use
    Archive::ArchiveFormat m_archive_format {Archive::ArchiveFormat::DEFAULT};

    /// Signal emitted when job created.
    signals::JobCreatedSignal::sptr m_job_created_signal;

    /// Used in case of bad password
    int m_password_retry {0};
};

reader::reader() noexcept :
    m_pimpl(std::make_unique<ReaderImpl>(this))
{
    new_signal<signals::SessionPathSignal>(signals::SESSION_LOADED);
    new_signal<signals::SessionPathSignal>(signals::SESSION_LOADING_FAILED);
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
reader::~reader() noexcept = default;

//-----------------------------------------------------------------------------

void reader::starting()
{
}

//-----------------------------------------------------------------------------

void reader::stopping()
{
    m_pimpl->m_password_retry = 0;
    if(m_pimpl->m_dialog_policy != DialogPolicy::NEVER)
    {
        clearLocations();
    }
}

//-----------------------------------------------------------------------------

void reader::configuring()
{
    sight::io::service::reader::configuring();

    const auto& tree = this->get_config();

    // Extension configuration
    const auto& dialog = tree.get_child_optional("dialog.<xmlattr>");
    if(dialog.is_initialized())
    {
        m_pimpl->m_extension_name        = dialog->get<std::string>("extension");
        m_pimpl->m_extension_description = dialog->get<std::string>("description");
        m_pimpl->m_dialog_policy         = stringToDialogPolicy(dialog->get<std::string>("policy", "default"));

        SIGHT_THROW_IF(
            "Cannot read dialog policy.",
            m_pimpl->m_dialog_policy == DialogPolicy::INVALID
        );
    }

    // Password configuration
    const auto& password = tree.get_child_optional("password.<xmlattr>");
    if(password.is_initialized())
    {
        // Password policy
        m_pimpl->m_password_policy = password_keeper::string_to_password_policy(
            password->get<std::string>("policy", "default")
        );

        SIGHT_THROW_IF(
            "Cannot read password policy.",
            m_pimpl->m_password_policy == password_keeper::password_policy::INVALID
        );

        // Encryption policy
        m_pimpl->m_encryption_policy = password_keeper::string_to_encryption_policy(
            password->get<std::string>("encryption", "default")
        );

        SIGHT_THROW_IF(
            "Cannot read encryption policy.",
            m_pimpl->m_encryption_policy == password_keeper::encryption_policy::INVALID
        );
    }

    // Format configuration
    const auto& archive = tree.get_child_optional("archive.<xmlattr>");
    if(archive.is_initialized())
    {
        const auto& format                          = archive->get<std::string>("format", "default");
        const Archive::ArchiveFormat archive_format = Archive::stringToArchiveFormat(format);

        if(archive_format != Archive::ArchiveFormat::INVALID)
        {
            m_pimpl->m_archive_format = archive_format;
        }
        else if(format == "archive")
        {
            m_pimpl->m_archive_format = Archive::ArchiveFormat::DEFAULT;
        }
        else
        {
            SIGHT_THROW("Cannot read archive format '" + format + "'.");
        }
    }
}

//-----------------------------------------------------------------------------

void reader::updating()
{
    // Set to failed until successful
    m_readFailed = true;

    // Show the save dialog if the path is empty
    if((!hasLocationDefined() && m_pimpl->m_dialog_policy != DialogPolicy::NEVER)
       || (m_pimpl->m_dialog_policy == DialogPolicy::ALWAYS && m_pimpl->m_password_retry == 0))
    {
        openLocationDialog();
    }

    // If the user did not choose a file, we stop here
    if(!hasLocationDefined())
    {
        return;
    }

    const auto filepath = get_file();
    SIGHT_THROW_IF("The file '" << filepath << "' is an existing folder.", std::filesystem::is_directory(filepath));

    // Ask password if needed
    const secure_string& password =
        [&]
        {
            if(m_pimpl->m_password_policy == password_keeper::password_policy::NEVER)
            {
                // No password management
                return secure_string();
            }

            const secure_string& global_password = password_keeper::get_global_password();

            if(m_pimpl->m_password_retry > 0
               || (m_pimpl->m_password_policy == password_keeper::password_policy::ALWAYS)
               || (m_pimpl->m_password_policy == password_keeper::password_policy::GLOBAL
                   && global_password.empty()))
            {
                const auto& [newPassword, ok] =
                    sight::ui::dialog::input::showInputDialog(
                        "Enter Password",
                        "Password:",
                        global_password.c_str(), // NOLINT(readability-redundant-string-cstr)
                        sight::ui::dialog::input::EchoMode::PASSWORD
                    );

                return secure_string(newPassword);
            }

            return global_password;
        }();

    const auto read_job = std::make_shared<core::jobs::job>(
        "Reading " + filepath.string() + " file",
        [&](core::jobs::job& _running_job)
        {
            _running_job.done_work(10);

            // Create the session reader
            auto reader = std::make_shared<sight::io::session::SessionReader>();
            reader->set_file(filepath);
            reader->setPassword(password);
            reader->setEncryptionPolicy(m_pimpl->m_encryption_policy);
            reader->setArchiveFormat(m_pimpl->m_archive_format);

            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::BusyCursor busy_cursor;

            // Read the file
            reader->read();

            // Set output
            auto new_data = std::dynamic_pointer_cast<data::object>(reader->getObject());
            SIGHT_THROW_IF("Invalid session", !new_data);

            auto data = m_data.lock();
            data->shallow_copy(new_data);

            auto sig = data->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            {
                core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                sig->async_emit();
            }

            _running_job.done();
        },
        this->worker()
    );

    core::jobs::aggregator::sptr jobs = std::make_shared<core::jobs::aggregator>(filepath.string() + " reader");
    jobs->add(read_job);
    jobs->set_cancelable(false);

    m_pimpl->m_job_created_signal->emit(jobs);

    try
    {
        jobs->run().get();
        m_readFailed              = false;
        m_pimpl->m_password_retry = 0;

        // Signal that we successfully read this file
        this->signal<signals::SessionPathSignal>(signals::SESSION_LOADED)->async_emit(filepath);
    }
    catch(sight::io::zip::exception::BadPassword&)
    {
        // Ask if the user want to retry.
        sight::ui::dialog::message message_box;
        message_box.setTitle("Wrong password");
        message_box.setMessage(
            "The file is password protected and the provided password is wrong.\n\nRetry with a different password ?"
        );
        message_box.setIcon(ui::dialog::message::QUESTION);
        message_box.addButton(ui::dialog::message::RETRY);
        message_box.addButton(ui::dialog::message::CANCEL);

        if(message_box.show() == sight::ui::dialog::message::RETRY)
        {
            m_pimpl->m_password_retry++;
            updating();
        }
        else
        {
            m_pimpl->m_password_retry = 0;
        }

        // Signal that we failed to read this file
        this->signal<signals::SessionPathSignal>(signals::SESSION_LOADING_FAILED)->async_emit(filepath);
    }
    catch(const std::exception& e)
    {
        // Handle the error.
        SIGHT_ERROR(e.what());
        sight::ui::dialog::message::show(
            "Session reader failed",
            e.what(),
            sight::ui::dialog::message::CRITICAL
        );

        // Signal that we failed to read this file
        this->signal<signals::SessionPathSignal>(signals::SESSION_LOADING_FAILED)->async_emit(filepath);
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::dialog::message::show(
            "Session reader aborted",
            "Reading process aborted",
            sight::ui::dialog::message::WARNING
        );

        // Signal that we failed to read this file
        this->signal<signals::SessionPathSignal>(signals::SESSION_LOADING_FAILED)->async_emit(filepath);
    }
}

//-----------------------------------------------------------------------------

void reader::openLocationDialog()
{
    static auto default_location = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location location_dialog;

    // Set window title
    if(!m_windowTitle.empty())
    {
        location_dialog.setTitle(m_windowTitle);
    }
    else
    {
        location_dialog.setTitle("Enter file name");
    }

    location_dialog.setDefaultLocation(default_location);
    location_dialog.setOption(ui::dialog::location::READ);
    location_dialog.setOption(ui::dialog::location::FILE_MUST_EXIST);
    location_dialog.setType(ui::dialog::location::SINGLE_FILE);
    location_dialog.addFilter(m_pimpl->m_extension_description, "*" + m_pimpl->m_extension_name);

    // Show the dialog
    const auto result = std::dynamic_pointer_cast<core::location::single_file>(location_dialog.show());

    if(result)
    {
        const auto& filepath = result->get_file();
        set_file(filepath);
        m_pimpl->m_extension_name = location_dialog.getSelectedExtensions().front();

        // Save default location for later use
        default_location->set_folder(filepath.parent_path());
        location_dialog.saveDefaultLocation(default_location);
    }
    else
    {
        clearLocations();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::session
