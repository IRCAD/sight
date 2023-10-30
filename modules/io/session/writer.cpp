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

#include "modules/io/session/writer.hpp"

#include <core/com/signal.hxx>
#include <core/crypto/password_keeper.hpp>
#include <core/crypto/secure_string.hpp>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_folder.hpp>
#include <core/os/temp_path.hpp>
#include <core/tools/system.hpp>

#include <io/session/session_writer.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

namespace sight::module::io::session
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::archive;

/// Private writer implementation
class writer::WriterImpl
{
public:

    /// Delete default constructors and assignment operators
    WriterImpl(const WriterImpl&)            = delete;
    WriterImpl(WriterImpl&&)                 = delete;
    WriterImpl& operator=(const WriterImpl&) = delete;
    WriterImpl& operator=(WriterImpl&&)      = delete;

    /// Constructor
    inline explicit WriterImpl(writer* const _writer) noexcept :
        m_writer(_writer),
        m_job_created_signal(_writer->new_signal<job_created_signal_t>("jobCreated"))
    {
    }

    /// Default destructor
    inline ~WriterImpl() noexcept = default;

    /// Pointer to the public interface
    writer* const m_writer;

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
    job_created_signal_t::sptr m_job_created_signal;
};

writer::writer() noexcept :
    m_pimpl(std::make_unique<WriterImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
writer::~writer() noexcept = default;

//-----------------------------------------------------------------------------

void writer::starting()
{
}

//-----------------------------------------------------------------------------

void writer::stopping()
{
    clear_locations();
}

//-----------------------------------------------------------------------------

void writer::configuring()
{
    sight::io::service::writer::configuring();

    const auto& tree = this->get_config();

    // Dialog configuration
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
        m_pimpl->m_archive_format = archive::string_to_archive_format(archive->get<std::string>("format", "default"));

        SIGHT_THROW_IF(
            "Cannot read archive format.",
            m_pimpl->m_archive_format == archive::archive_format::invalid
        );
    }
}

//-----------------------------------------------------------------------------

void writer::updating()
{
    // Set to failed until successful
    m_write_failed = true;

    // Show the save dialog if the path is empty
    if((!has_location_defined() && m_pimpl->m_dialog_policy != dialog_policy::never)
       || m_pimpl->m_dialog_policy == dialog_policy::always)
    {
        open_location_dialog();
    }

    // If the user did not choose a file, we stop here
    if(!has_location_defined())
    {
        return;
    }

    // Check if we must add an extension or not
    auto filepath = get_file();
    if(!filepath.has_extension())
    {
        filepath += m_pimpl->m_extension_name;
    }

    // In case the path is computed instead of user-selected, make sure it exists
    if(filepath.has_parent_path() && !std::filesystem::exists(filepath.parent_path()))
    {
        std::filesystem::create_directories(filepath.parent_path());
    }

    SIGHT_THROW_IF("The file '" << filepath << "' is an existing folder.", std::filesystem::is_directory(filepath));

    // Generate temporary file
    const core::os::temp_file temp_file;

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

            if((m_pimpl->m_password_policy == password_keeper::password_policy::always)
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

    const auto write_job = std::make_shared<core::jobs::job>(
        "Writing " + temp_file.string() + " file",
        [&](core::jobs::job& _running_job)
        {
            _running_job.done_work(10);

            // Create the session writer
            auto writer = std::make_shared<sight::io::session::session_writer>();
            {
                // The object must be unlocked since it will be locked again when writing
                auto data = m_data.lock();
                writer->set_object(data.get_shared());
                writer->set_file(temp_file);
                writer->set_password(password);
                writer->set_encryption_policy(m_pimpl->m_encryption_policy);
                writer->set_archive_format(m_pimpl->m_archive_format);
            }

            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::busy_cursor busy_cursor;

            // Write the file
            writer->write();

            _running_job.done();
        },
        this->worker()
    );

    const auto rename_job = std::make_shared<core::jobs::job>(
        "Rename file" + temp_file.string() + " to " + filepath.string() + ".",
        [&](core::jobs::job& _running_job)
        {
            _running_job.done_work(80);

            // Robust rename
            core::tools::system::robust_rename(temp_file, filepath, true);

            _running_job.done();
        },
        this->worker()
    );

    core::jobs::aggregator::sptr jobs = std::make_shared<core::jobs::aggregator>(filepath.string() + " writer");
    jobs->add(write_job);
    jobs->add(rename_job);
    jobs->set_cancelable(false);

    m_pimpl->m_job_created_signal->emit(jobs);

    try
    {
        jobs->run().get();
        m_write_failed = false;
    }
    catch(const std::exception& e)
    {
        // Handle the error.
        SIGHT_ERROR(e.what());
        sight::ui::dialog::message::show(
            "Session writer failed",
            e.what(),
            sight::ui::dialog::message::critical
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::dialog::message::show(
            "Session writer aborted",
            "Writing process aborted",
            sight::ui::dialog::message::warning
        );
    }
}

//-----------------------------------------------------------------------------

void writer::open_location_dialog()
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
    location_dialog.set_option(ui::dialog::location::write);
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
