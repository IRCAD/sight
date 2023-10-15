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

#include <io/session/SessionWriter.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/input.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

namespace sight::module::io::session
{

using core::crypto::password_keeper;
using core::crypto::secure_string;
using sight::io::zip::Archive;

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
    inline explicit WriterImpl(writer* const writer) noexcept :
        m_writer(writer),
        m_job_created_signal(writer->new_signal<JobCreatedSignal>("jobCreated"))
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
    DialogPolicy m_dialog_policy = {DialogPolicy::NEVER};

    /// Password policy to use
    password_keeper::password_policy m_password_policy {password_keeper::password_policy::NEVER};

    /// Encryption policy to use
    password_keeper::encryption_policy m_encryption_policy {password_keeper::encryption_policy::PASSWORD};

    /// Archive format to use
    Archive::ArchiveFormat m_archive_format {Archive::ArchiveFormat::DEFAULT};

    /// Signal emitted when job created.
    JobCreatedSignal::sptr m_job_created_signal;
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
    clearLocations();
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
        m_pimpl->m_archive_format = Archive::stringToArchiveFormat(archive->get<std::string>("format", "default"));

        SIGHT_THROW_IF(
            "Cannot read archive format.",
            m_pimpl->m_archive_format == Archive::ArchiveFormat::INVALID
        );
    }
}

//-----------------------------------------------------------------------------

void writer::updating()
{
    // Set to failed until successful
    m_writeFailed = true;

    // Show the save dialog if the path is empty
    if((!hasLocationDefined() && m_pimpl->m_dialog_policy != DialogPolicy::NEVER)
       || m_pimpl->m_dialog_policy == DialogPolicy::ALWAYS)
    {
        openLocationDialog();
    }

    // If the user did not choose a file, we stop here
    if(!hasLocationDefined())
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
    const core::os::temp_file tempFile;

    // Ask password if needed
    const secure_string& password =
        [&]
        {
            if(m_pimpl->m_password_policy == password_keeper::password_policy::NEVER)
            {
                // No password management
                return secure_string();
            }

            const secure_string& globalPassword = password_keeper::get_global_password();

            if((m_pimpl->m_password_policy == password_keeper::password_policy::ALWAYS)
               || (m_pimpl->m_password_policy == password_keeper::password_policy::GLOBAL
                   && globalPassword.empty()))
            {
                const auto& [newPassword, ok] =
                    sight::ui::dialog::input::showInputDialog(
                        "Enter Password",
                        "Password:",
                        globalPassword.c_str(), // NOLINT(readability-redundant-string-cstr)
                        sight::ui::dialog::input::EchoMode::PASSWORD

                    );

                return secure_string(newPassword);
            }

            return globalPassword;
        }();

    const auto writeJob = std::make_shared<core::jobs::job>(
        "Writing " + tempFile.string() + " file",
        [&](core::jobs::job& runningJob)
        {
            runningJob.done_work(10);

            // Create the session writer
            auto writer = std::make_shared<sight::io::session::SessionWriter>();
            {
                // The object must be unlocked since it will be locked again when writing
                auto data = m_data.lock();
                writer->set_object(data.get_shared());
                writer->set_file(tempFile);
                writer->setPassword(password);
                writer->setEncryptionPolicy(m_pimpl->m_encryption_policy);
                writer->setArchiveFormat(m_pimpl->m_archive_format);
            }

            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::BusyCursor busyCursor;

            // Write the file
            writer->write();

            runningJob.done();
        },
        this->worker()
    );

    const auto renameJob = std::make_shared<core::jobs::job>(
        "Rename file" + tempFile.string() + " to " + filepath.string() + ".",
        [&](core::jobs::job& runningJob)
        {
            runningJob.done_work(80);

            // Robust rename
            core::tools::system::robust_rename(tempFile, filepath, true);

            runningJob.done();
        },
        this->worker()
    );

    core::jobs::aggregator::sptr jobs = std::make_shared<core::jobs::aggregator>(filepath.string() + " writer");
    jobs->add(writeJob);
    jobs->add(renameJob);
    jobs->set_cancelable(false);

    m_pimpl->m_job_created_signal->emit(jobs);

    try
    {
        jobs->run().get();
        m_writeFailed = false;
    }
    catch(const std::exception& e)
    {
        // Handle the error.
        SIGHT_ERROR(e.what());
        sight::ui::dialog::message::show(
            "Session writer failed",
            e.what(),
            sight::ui::dialog::message::CRITICAL
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::dialog::message::show(
            "Session writer aborted",
            "Writing process aborted",
            sight::ui::dialog::message::WARNING
        );
    }
}

//-----------------------------------------------------------------------------

void writer::openLocationDialog()
{
    static auto defaultLocation = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location locationDialog;

    // Set window title
    if(!m_windowTitle.empty())
    {
        locationDialog.setTitle(m_windowTitle);
    }
    else
    {
        locationDialog.setTitle("Enter file name");
    }

    locationDialog.setDefaultLocation(defaultLocation);
    locationDialog.setOption(ui::dialog::location::WRITE);
    locationDialog.setType(ui::dialog::location::SINGLE_FILE);
    locationDialog.addFilter(m_pimpl->m_extension_description, "*" + m_pimpl->m_extension_name);

    // Show the dialog
    const auto result = std::dynamic_pointer_cast<core::location::single_file>(locationDialog.show());

    if(result)
    {
        const auto& filepath = result->get_file();
        set_file(filepath);
        m_pimpl->m_extension_name = locationDialog.getSelectedExtensions().front();

        // Save default location for later use
        defaultLocation->set_folder(filepath.parent_path());
        locationDialog.saveDefaultLocation(defaultLocation);
    }
    else
    {
        clearLocations();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::session
