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

#include <io/session/SessionReader.hpp>
#include <io/zip/exception/Read.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

namespace sight::module::io::zip
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::Archive;

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
        M_READER(_reader),
        m_job_created_signal(_reader->new_signal<JobCreatedSignal>("jobCreated"))
    {
    }

    /// Default destructor
    inline ~ExtractImpl() noexcept = default;

    /// Pointer to the public interface
    extract* const M_READER;

    /// Signal emitted when job created.
    JobCreatedSignal::sptr m_job_created_signal;

    /// Used in case of bad password
    int m_password_retry {0};

    /// The path where to extract the files
    std::filesystem::path m_outputPath;
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
    clearLocations();
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
    m_readFailed = true;

    // Show the save dialog if the path is empty
    if(!hasLocationDefined())
    {
        openLocationDialog();
    }

    // If the user did not choose a file, we stop here
    if(!hasLocationDefined())
    {
        return;
    }

    if(m_pimpl->m_outputPath.empty())
    {
        static auto default_location = std::make_shared<core::location::single_folder>();
        default_location->set_folder("/");

        sight::ui::dialog::location location_dialog;
        location_dialog.setTitle("Enter the folder where the files must be extracted");

        if(!m_windowTitle.empty())
        {
            location_dialog.setTitle(m_windowTitle);
        }
        else
        {
            location_dialog.setTitle("Enter the output path");
        }

        location_dialog.setDefaultLocation(default_location);
        location_dialog.setOption(ui::dialog::location::WRITE);
        location_dialog.setType(ui::dialog::location::FOLDER);

        const auto result = std::dynamic_pointer_cast<core::location::single_folder>(location_dialog.show());

        if(result)
        {
            if(!std::filesystem::is_empty(result->get_folder()))
            {
                sight::ui::dialog::message message("Output path not empty",
                                                   "The output path isn't empty. Continue anyway?",
                                                   sight::ui::dialog::message::WARNING);
                message.addButton(sight::ui::dialog::message::YES);
                message.addButton(sight::ui::dialog::message::CANCEL);
                message.addButton(sight::ui::dialog::message::RETRY);
                auto action = message.show();
                switch(action)
                {
                    case sight::ui::dialog::message::YES:
                        // Continue as normal
                        break;

                    case sight::ui::dialog::message::CANCEL:
                        // Totally abort the operation
                        clearLocations();
                        return;

                    case sight::ui::dialog::message::RETRY:
                        // Let the user choose another output path
                        updating();
                        return;

                    default:
                        SIGHT_ASSERT("Invalid button", false);
                }
            }

            m_pimpl->m_outputPath = result->get_folder();
            default_location->set_folder(result->get_folder().parent_path());
            location_dialog.saveDefaultLocation(default_location);
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
                    sight::ui::dialog::input::showInputDialog(
                        "Enter Password",
                        "Password:",
                        global_password.c_str(), // NOLINT(readability-redundant-string-cstr)
                        sight::ui::dialog::input::EchoMode::PASSWORD
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
            const sight::ui::BusyCursor busy_cursor;
            _running_job.done_work(10);
            sight::io::zip::ArchiveReader::get(
                filepath,
                Archive::ArchiveFormat::DEFAULT
            )->extractAllTo(m_pimpl->m_outputPath, password);
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
        m_pimpl->m_outputPath.clear();
        m_readFailed              = false;
        m_pimpl->m_password_retry = 0;
        clearLocations();
        sight::ui::dialog::message::show("Success", "The archive was successfully extracted.");
    }
    catch(sight::io::zip::exception::BadPassword&)
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
        }
    }
    catch(const std::exception& e)
    {
        m_pimpl->m_outputPath.clear();
        clearLocations();
        SIGHT_ERROR(e.what());
        sight::ui::dialog::message::show(
            "Session reader failed",
            e.what(),
            sight::ui::dialog::message::CRITICAL
        );
    }
    catch(...)
    {
        m_pimpl->m_outputPath.clear();
        clearLocations();
        sight::ui::dialog::message::show(
            "Session reader aborted",
            "Reading process aborted",
            sight::ui::dialog::message::WARNING
        );
    }
}

//-----------------------------------------------------------------------------

void extract::openLocationDialog()
{
    static auto default_location = std::make_shared<core::location::single_folder>();
    default_location->set_folder("/");

    sight::ui::dialog::location location_dialog;

    if(!m_windowTitle.empty())
    {
        location_dialog.setTitle(m_windowTitle);
    }
    else
    {
        location_dialog.setTitle("Enter archive file");
    }

    location_dialog.setDefaultLocation(default_location);
    location_dialog.setOption(ui::dialog::location::READ);
    location_dialog.setOption(ui::dialog::location::FILE_MUST_EXIST);
    location_dialog.setType(ui::dialog::location::SINGLE_FILE);

    const auto result = std::dynamic_pointer_cast<core::location::single_file>(location_dialog.show());

    if(result)
    {
        const auto& filepath = result->get_file();
        set_file(filepath);

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

} // namespace sight::module::io::zip
