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

#include "modules/io/session/SWriter.hpp"

#include <core/com/Signal.hxx>
#include <core/crypto/secure_string.hpp>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/System.hpp>

#include <io/session/PasswordKeeper.hpp>
#include <io/session/SessionWriter.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/InputDialog.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

namespace sight::module::io::session
{

using sight::io::session::PasswordKeeper;
using core::crypto::secure_string;

/// Private SWriter implementation
class SWriter::WriterImpl
{
public:

    /// Delete default constructors and assignment operators
    WriterImpl(const WriterImpl&)            = delete;
    WriterImpl(WriterImpl&&)                 = delete;
    WriterImpl& operator=(const WriterImpl&) = delete;
    WriterImpl& operator=(WriterImpl&&)      = delete;

    /// Constructor
    inline WriterImpl(SWriter* const writer) noexcept :
        m_writer(writer),
        m_jobCreatedSignal(writer->newSignal<JobCreatedSignal>("jobCreated"))
    {
    }

    /// Default destructor
    inline ~WriterImpl() noexcept = default;

    /// Pointer to the public interface
    SWriter* const m_writer;

    /// Extension name to use for session file
    std::string m_extensionName {".zip"};

    /// Extension description to use for file save dialog
    std::string m_extensionDescription {"Sight session"};

    /// Dialog policy to use for the file location
    DialogPolicy m_dialogPolicy = {DialogPolicy::DEFAULT};

    /// Password policy to use
    PasswordKeeper::PasswordPolicy m_passwordPolicy {PasswordKeeper::PasswordPolicy::DEFAULT};

    /// Encryption policy to use
    PasswordKeeper::EncryptionPolicy m_encryptionPolicy {PasswordKeeper::EncryptionPolicy::DEFAULT};

    /// Signal emitted when job created.
    JobCreatedSignal::sptr m_jobCreatedSignal;
};

SWriter::SWriter() noexcept :
    m_pimpl(std::make_unique<WriterImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
SWriter::~SWriter() noexcept = default;

//-----------------------------------------------------------------------------

void SWriter::starting()
{
    clearLocations();
}

//-----------------------------------------------------------------------------

void SWriter::stopping()
{
    clearLocations();
}

//-----------------------------------------------------------------------------

void SWriter::configuring()
{
    sight::io::base::service::IWriter::configuring();

    const auto& tree = this->getConfigTree();

    // Dialog configuration
    const auto& dialog = tree.get_child_optional("dialog.<xmlattr>");
    if(dialog.is_initialized())
    {
        m_pimpl->m_extensionName        = dialog->get<std::string>("extension");
        m_pimpl->m_extensionDescription = dialog->get<std::string>("description");
        m_pimpl->m_dialogPolicy         = stringToDialogPolicy(dialog->get<std::string>("policy"));

        SIGHT_THROW_IF(
            "Cannot read dialog policy.",
            m_pimpl->m_dialogPolicy == DialogPolicy::INVALID
        );
    }

    // Password configuration
    const auto& password = tree.get_child_optional("password.<xmlattr>");
    if(password.is_initialized())
    {
        // Password policy
        m_pimpl->m_passwordPolicy = PasswordKeeper::stringToPasswordPolicy(password->get<std::string>("policy"));
        SIGHT_THROW_IF(
            "Cannot read password policy.",
            m_pimpl->m_passwordPolicy == PasswordKeeper::PasswordPolicy::INVALID
        );

        // Encryption policy
        m_pimpl->m_encryptionPolicy = PasswordKeeper::stringToEncryptionPolicy(
            password->get<std::string>("encryption")
        );

        SIGHT_THROW_IF(
            "Cannot read encryption policy.",
            m_pimpl->m_encryptionPolicy == PasswordKeeper::EncryptionPolicy::INVALID
        );
    }
}

//-----------------------------------------------------------------------------

void SWriter::updating()
{
    // Set to failed until successful
    m_writeFailed = true;

    // Show the save dialog if the path is empty
    if((!hasLocationDefined() && m_pimpl->m_dialogPolicy != DialogPolicy::NEVER)
       || m_pimpl->m_dialogPolicy == DialogPolicy::ALWAYS)
    {
        openLocationDialog();
    }

    // If the user did not choose a file, we stop here
    if(!hasLocationDefined())
    {
        return;
    }

    // Check if we must add an extension or not
    auto filepath = getFile();
    if(!filepath.has_extension())
    {
        filepath += m_pimpl->m_extensionName;
    }

    SIGHT_THROW_IF("The file '" << filepath << "' is an existing folder.", std::filesystem::is_directory(filepath));

    // Generate temporary file
    const core::tools::System::TemporaryFile temporaryFile;

    // Ask password if needed
    const secure_string& password =
        [&]
        {
            if(m_pimpl->m_passwordPolicy == PasswordKeeper::PasswordPolicy::NEVER)
            {
                // No password management
                return secure_string();
            }
            else
            {
                const secure_string& globalPassword = PasswordKeeper::getGlobalPassword();

                if((m_pimpl->m_passwordPolicy == PasswordKeeper::PasswordPolicy::ALWAYS)
                   || (m_pimpl->m_passwordPolicy == PasswordKeeper::PasswordPolicy::ONCE
                       && globalPassword.empty()))
                {
                    sight::ui::base::dialog::InputDialog inputDialog;
                    const auto& newPassword = secure_string(
                        inputDialog.showInputDialog(
                            "Enter Password",
                            "Password:",
                            globalPassword.c_str(),
                            sight::ui::base::dialog::InputDialog::EchoMode::ECHO_ON_EDIT
                        )
                    );

                    if(m_pimpl->m_passwordPolicy == PasswordKeeper::PasswordPolicy::ONCE)
                    {
                        PasswordKeeper::setGlobalPassword(newPassword);
                    }

                    return newPassword;
                }

                return globalPassword;
            }
        }();

    const auto writeJob = core::jobs::Job::New(
        "Writing " + temporaryFile.getTemporaryFilePath().string() + " file",
        [&](core::jobs::Job& runningJob)
        {
            runningJob.doneWork(10);

            // Create the session writer
            auto writer = sight::io::session::SessionWriter::New();
            {
                // The object must be unlocked since it will be locked again when writing
                auto data = m_data.lock();
                writer->setObject(data.get_shared());
                writer->setFile(temporaryFile.getTemporaryFilePath());
                writer->setPassword(password);
                writer->setEncryptionPolicy(m_pimpl->m_encryptionPolicy);
            }

            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::base::BusyCursor busyCursor;

            // Write the file
            writer->write();

            runningJob.done();
        },
        m_associatedWorker
    );

    const auto renameJob = core::jobs::Job::New(
        "Rename file" + temporaryFile.getTemporaryFilePath().string() + " to " + filepath.string() + ".",
        [&](core::jobs::Job& runningJob)
        {
            runningJob.doneWork(80);

            // Robust rename
            core::tools::System::robustRename(temporaryFile.getTemporaryFilePath(), filepath, true);

            runningJob.done();
        },
        m_associatedWorker
    );

    core::jobs::Aggregator::sptr jobs = core::jobs::Aggregator::New(filepath.string() + " writer");
    jobs->add(writeJob);
    jobs->add(renameJob);
    jobs->setCancelable(false);

    m_pimpl->m_jobCreatedSignal->emit(jobs);

    try
    {
        jobs->run().get();
        m_writeFailed = false;
    }
    catch(std::exception& _e)
    {
        // Handle the error.
        SIGHT_ERROR(_e.what());
        sight::ui::base::dialog::MessageDialog::show(
            "Session writer failed",
            _e.what(),
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::base::dialog::MessageDialog::show(
            "Session writer aborted",
            "Writing process aborted",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
}

//-----------------------------------------------------------------------------

void SWriter::openLocationDialog()
{
    static auto defaultLocation = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog locationDialog;

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
    locationDialog.setOption(ui::base::dialog::ILocationDialog::WRITE);
    locationDialog.setType(ui::base::dialog::ILocationDialog::SINGLE_FILE);
    locationDialog.addFilter(m_pimpl->m_extensionDescription, "*" + m_pimpl->m_extensionName);

    // Show the dialog
    const auto result = std::dynamic_pointer_cast<core::location::SingleFile>(locationDialog.show());

    if(result)
    {
        const auto& filepath = result->getFile();
        setFile(filepath);
        m_pimpl->m_extensionName = locationDialog.getCurrentSelection();

        // Save default location for later use
        defaultLocation->setFolder(filepath.parent_path());
        locationDialog.saveDefaultLocation(defaultLocation);
    }
    else
    {
        clearLocations();
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::session
