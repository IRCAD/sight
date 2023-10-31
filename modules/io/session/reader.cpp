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

#include <io/session/session_reader.hpp>
#include <io/zip/exception/read.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

namespace sight::module::io::session
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::archive;

/// Private reader implementation
class reader::reader_impl
{
public:

    /// Delete default constructors and assignment operators
    reader_impl(const reader_impl&)            = delete;
    reader_impl(reader_impl&&)                 = delete;
    reader_impl& operator=(const reader_impl&) = delete;
    reader_impl& operator=(reader_impl&&)      = delete;

    /// Constructor
    inline explicit reader_impl(reader* const _reader) noexcept :
        m_reader(_reader),
        m_job_created_signal(_reader->new_signal<signals::job_created_signal_t>("jobCreated"))
    {
    }

    /// Default destructor
    inline ~reader_impl() noexcept = default;

    /// Pointer to the public interface
    reader* const m_reader;

    /// Extension name to use for session file
    std::string m_extension_name {".zip"};

    /// Extension description to use for file save dialog
    std::string m_extension_description {"Sight session"};

    /// Dialog policy to use for the file location
    dialog_policy m_dialog_policy = {dialog_policy::never};

    /// Password policy to use
    password_keeper::password_policy m_password_policy {password_keeper::password_policy::never};

    /// Encryption policy to use
    password_keeper::encryption_policy m_encryption_policy {password_keeper::encryption_policy::password};

    /// Archive format to use
    archive::archive_format m_archive_format {archive::archive_format::DEFAULT};

    /// Signal emitted when job created.
    signals::job_created_signal_t::sptr m_job_created_signal;

    /// Used in case of bad password
    int m_password_retry {0};
};

reader::reader() noexcept :
    m_pimpl(std::make_unique<reader_impl>(this))
{
    new_signal<signals::session_path_t>(signals::SESSION_LOADED);
    new_signal<signals::session_path_t>(signals::SESSION_LOADING_FAILED);
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
    if(m_pimpl->m_dialog_policy != dialog_policy::never)
    {
        clear_locations();
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
        m_pimpl->m_dialog_policy         = string_to_dialog_policy(dialog->get<std::string>("policy", "default"));

        SIGHT_THROW_IF(
            "Cannot read dialog policy.",
            m_pimpl->m_dialog_policy == dialog_policy::invalid
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
            m_pimpl->m_password_policy == password_keeper::password_policy::invalid
        );

        // Encryption policy
        m_pimpl->m_encryption_policy = password_keeper::string_to_encryption_policy(
            password->get<std::string>("encryption", "default")
        );

        SIGHT_THROW_IF(
            "Cannot read encryption policy.",
            m_pimpl->m_encryption_policy == password_keeper::encryption_policy::invalid
        );
    }

    // Format configuration
    const auto& archive = tree.get_child_optional("archive.<xmlattr>");
    if(archive.is_initialized())
    {
        const auto& format                           = archive->get<std::string>("format", "default");
        const archive::archive_format archive_format = archive::string_to_archive_format(format);

        if(archive_format != archive::archive_format::invalid)
        {
            m_pimpl->m_archive_format = archive_format;
        }
        else if(format == "archive")
        {
            m_pimpl->m_archive_format = archive::archive_format::DEFAULT;
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
    m_read_failed = true;

    // Show the save dialog if the path is empty
    if((!has_location_defined() && m_pimpl->m_dialog_policy != dialog_policy::never)
       || (m_pimpl->m_dialog_policy == dialog_policy::always && m_pimpl->m_password_retry == 0))
    {
        open_location_dialog();
    }

    // If the user did not choose a file, we stop here
    if(!has_location_defined())
    {
        return;
    }

    const auto filepath = get_file();
    SIGHT_THROW_IF("The file '" << filepath << "' is an existing folder.", std::filesystem::is_directory(filepath));

    // Ask password if needed
    const secure_string& password =
        [&]
        {
            if(m_pimpl->m_password_policy == password_keeper::password_policy::never)
            {
                // No password management
                return secure_string();
            }

            const secure_string& global_password = password_keeper::get_global_password();

            if(m_pimpl->m_password_retry > 0
               || (m_pimpl->m_password_policy == password_keeper::password_policy::always)
               || (m_pimpl->m_password_policy == password_keeper::password_policy::global
                   && global_password.empty()))
            {
                const auto& [newPassword, ok] =
                    sight::ui::dialog::input::show_input_dialog(
                        "Enter Password",
                        "Password:",
                        global_password.c_str(), // NOLINT(readability-redundant-string-cstr)
                        sight::ui::dialog::input::echo_mode::password
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
            auto reader = std::make_shared<sight::io::session::session_reader>();
            reader->set_file(filepath);
            reader->set_password(password);
            reader->set_encryption_policy(m_pimpl->m_encryption_policy);
            reader->set_archive_format(m_pimpl->m_archive_format);

            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::busy_cursor busy_cursor;

            // Read the file
            reader->read();

            // Set output
            auto new_data = std::dynamic_pointer_cast<data::object>(reader->get_object());
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
        m_read_failed             = false;
        m_pimpl->m_password_retry = 0;

        // Signal that we successfully read this file
        this->signal<signals::session_path_t>(signals::SESSION_LOADED)->async_emit(filepath);
    }
    catch(sight::io::zip::exception::bad_password&)
    {
        // Ask if the user want to retry.
        sight::ui::dialog::message message_box;
        message_box.set_title("Wrong password");
        message_box.set_message(
            "The file is password protected and the provided password is wrong.\n\nRetry with a different password ?"
        );
        message_box.set_icon(ui::dialog::message::question);
        message_box.add_button(ui::dialog::message::retry);
        message_box.add_button(ui::dialog::message::cancel);

        if(message_box.show() == sight::ui::dialog::message::retry)
        {
            m_pimpl->m_password_retry++;
            updating();
        }
        else
        {
            m_pimpl->m_password_retry = 0;
        }

        // Signal that we failed to read this file
        this->signal<signals::session_path_t>(signals::SESSION_LOADING_FAILED)->async_emit(filepath);
    }
    catch(const std::exception& e)
    {
        // Handle the error.
        SIGHT_ERROR(e.what());
        sight::ui::dialog::message::show(
            "Session reader failed",
            e.what(),
            sight::ui::dialog::message::critical
        );

        // Signal that we failed to read this file
        this->signal<signals::session_path_t>(signals::SESSION_LOADING_FAILED)->async_emit(filepath);
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::dialog::message::show(
            "Session reader aborted",
            "Reading process aborted",
            sight::ui::dialog::message::warning
        );

        // Signal that we failed to read this file
        this->signal<signals::session_path_t>(signals::SESSION_LOADING_FAILED)->async_emit(filepath);
    }
}

//-----------------------------------------------------------------------------

void reader::open_location_dialog()
{
    static auto default_location = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location location_dialog;

    // Set window title
    if(!m_window_title.empty())
    {
        location_dialog.set_title(m_window_title);
    }
    else
    {
        location_dialog.set_title("Enter file name");
    }

    location_dialog.set_default_location(default_location);
    location_dialog.set_option(ui::dialog::location::read);
    location_dialog.set_option(ui::dialog::location::file_must_exist);
    location_dialog.set_type(ui::dialog::location::single_file);
    location_dialog.add_filter(m_pimpl->m_extension_description, "*" + m_pimpl->m_extension_name);

    // Show the dialog
    const auto result = std::dynamic_pointer_cast<core::location::single_file>(location_dialog.show());

    if(result)
    {
        const auto& filepath = result->get_file();
        set_file(filepath);
        m_pimpl->m_extension_name = location_dialog.get_selected_extensions().front();

        // Save default location for later use
        default_location->set_folder(filepath.parent_path());
        location_dialog.save_default_location(default_location);
    }
    else
    {
        clear_locations();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::session
