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

// cspell:ignore NOLINT

#include "extract.hpp"

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

namespace sight::module::io::zip
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::archive;

/// Private implementation
class extract::ExtractImpl
{
public:

    /// Delete default constructors and assignment operators
    ExtractImpl(const ExtractImpl&)            = delete;
    ExtractImpl(ExtractImpl&&)                 = delete;
    ExtractImpl& operator=(const ExtractImpl&) = delete;
    ExtractImpl& operator=(ExtractImpl&&)      = delete;

    /// Constructor
    inline explicit ExtractImpl(extract* const _reader) noexcept :
        m_reader(_reader),
        m_job_created_signal(_reader->new_signal<job_created_signal_t>("job_created"))
    {
    }

    /// Default destructor
    inline ~ExtractImpl() noexcept = default;

    /// Pointer to the public interface
    extract* const m_reader;

    /// Signal emitted when job created.
    job_created_signal_t::sptr m_job_created_signal;

    /// Used in case of bad password
    int m_password_retry {0};

    /// The path where to extract the files
    std::filesystem::path m_output_path;
};

extract::extract() noexcept :
    m_pimpl(std::make_unique<ExtractImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
extract::~extract() noexcept = default;

//-----------------------------------------------------------------------------

void extract::starting()
{
}

//-----------------------------------------------------------------------------

void extract::stopping()
{
    m_pimpl->m_password_retry = 0;
    clear_locations();
}

//-----------------------------------------------------------------------------

void extract::configuring()
{
    sight::io::service::reader::configuring();
}

//-----------------------------------------------------------------------------

void extract::updating()
{
    // Set to failed until successful
    m_read_failed = true;

    // Show the save dialog if the path is empty
    if(!has_location_defined())
    {
        open_location_dialog();
    }

    // If the user did not choose a file, we stop here
    if(!has_location_defined())
    {
        return;
    }

    if(m_pimpl->m_output_path.empty())
    {
        static auto default_location = std::make_shared<core::location::single_folder>();
        default_location->set_folder("/");

        sight::ui::dialog::location location_dialog;
        location_dialog.set_title("Enter the folder where the files must be extracted");

        if(!m_window_title.empty())
        {
            location_dialog.set_title(m_window_title);
        }
        else
        {
            location_dialog.set_title("Enter the output path");
        }

        location_dialog.set_default_location(default_location);
        location_dialog.set_option(ui::dialog::location::write);
        location_dialog.set_type(ui::dialog::location::folder);

        const auto result = std::dynamic_pointer_cast<core::location::single_folder>(location_dialog.show());

        if(result)
        {
            if(!std::filesystem::is_empty(result->get_folder()))
            {
                sight::ui::dialog::message message("Output path not empty",
                                                   "The output path isn't empty. Continue anyway?",
                                                   sight::ui::dialog::message::warning);
                message.add_button(sight::ui::dialog::message::yes);
                message.add_button(sight::ui::dialog::message::cancel);
                message.add_button(sight::ui::dialog::message::retry);
                auto action = message.show();
                switch(action)
                {
                    case sight::ui::dialog::message::yes:
                        // Continue as normal
                        break;

                    case sight::ui::dialog::message::cancel:
                        // Totally abort the operation
                        clear_locations();
                        return;

                    case sight::ui::dialog::message::retry:
                        // Let the user choose another output path
                        updating();
                        return;

                    default:
                        SIGHT_ASSERT("Invalid button", false);
                }
            }

            m_pimpl->m_output_path = result->get_folder();
            default_location->set_folder(result->get_folder().parent_path());
            location_dialog.save_default_location(default_location);
        }
        else
        {
            return;
        }
    }

    const auto filepath = get_file();
    SIGHT_THROW_IF("The file '" << filepath << "' is an existing folder.", std::filesystem::is_directory(filepath));

    // Ask password if needed
    const secure_string& password =
        [&]
        {
            const secure_string& global_password = password_keeper::get_global_password();

            if(m_pimpl->m_password_retry > 0)
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

            if(global_password.empty())
            {
                if constexpr(core::crypto::password_keeper::has_default_password())
                {
                    return core::crypto::password_keeper::get_default_password();
                }
            }

            return global_password;
        }();

    const auto read_job = std::make_shared<core::jobs::job>(
        "Reading " + filepath.string() + " file",
        [&](core::jobs::job& _running_job)
        {
            const sight::ui::busy_cursor busy_cursor;
            _running_job.done_work(10);
            sight::io::zip::archive_reader::get(
                filepath,
                archive::archive_format::DEFAULT
            )->extract_all_to(m_pimpl->m_output_path, password);
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
        m_pimpl->m_output_path.clear();
        m_read_failed             = false;
        m_pimpl->m_password_retry = 0;
        clear_locations();
        sight::ui::dialog::message::show("Success", "The archive was successfully extracted.");
    }
    catch(sight::io::zip::exception::bad_password&)
    {
        if(m_pimpl->m_password_retry == 0)
        {
            // First attempt to extract the archive: The user just discovers that it is encrypted
            m_pimpl->m_password_retry++;
            updating();
        }
        else
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
        }
    }
    catch(const std::exception& e)
    {
        m_pimpl->m_output_path.clear();
        clear_locations();
        SIGHT_ERROR(e.what());
        sight::ui::dialog::message::show(
            "Session reader failed",
            e.what(),
            sight::ui::dialog::message::critical
        );
    }
    catch(...)
    {
        m_pimpl->m_output_path.clear();
        clear_locations();
        sight::ui::dialog::message::show(
            "Session reader aborted",
            "Reading process aborted",
            sight::ui::dialog::message::warning
        );
    }
}

//-----------------------------------------------------------------------------

void extract::open_location_dialog()
{
    static auto default_location = std::make_shared<core::location::single_folder>();
    default_location->set_folder("/");

    sight::ui::dialog::location location_dialog;

    if(!m_window_title.empty())
    {
        location_dialog.set_title(m_window_title);
    }
    else
    {
        location_dialog.set_title("Enter archive file");
    }

    location_dialog.set_default_location(default_location);
    location_dialog.set_option(ui::dialog::location::read);
    location_dialog.set_option(ui::dialog::location::file_must_exist);
    location_dialog.set_type(ui::dialog::location::single_file);

    const auto result = std::dynamic_pointer_cast<core::location::single_file>(location_dialog.show());

    if(result)
    {
        const auto& filepath = result->get_file();
        set_file(filepath);

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

} // namespace sight::module::io::zip
