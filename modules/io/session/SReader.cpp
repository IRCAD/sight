/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/io/session/SReader.hpp"

#include <core/com/Signal.hxx>
#include <core/crypto/secure_string.hpp>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/System.hpp>

#include <io/session/PasswordKeeper.hpp>
#include <io/session/SessionReader.hpp>
#include <io/zip/exception/Read.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/InputDialog.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

namespace sight::module::io::session
{

using sight::io::session::PasswordKeeper;
using core::crypto::secure_string;

/// Private SReader implementation
class SReader::ReaderImpl
{
public:

    /// Delete default constructors and assignment operators
    ReaderImpl(const ReaderImpl&)            = delete;
    ReaderImpl(ReaderImpl&&)                 = delete;
    ReaderImpl& operator=(const ReaderImpl&) = delete;
    ReaderImpl& operator=(ReaderImpl&&)      = delete;

    /// Constructor
    inline ReaderImpl(SReader* const reader) noexcept :
        m_reader(reader),
        m_jobCreatedSignal(reader->newSignal<JobCreatedSignal>("jobCreated"))
    {
    }

    /// Default destructor
    inline ~ReaderImpl() noexcept = default;

    /// Pointer to the public interface
    SReader* const m_reader;

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

    /// Used in case of bad password
    int m_passwordRetry {0};
};

SReader::SReader() noexcept :
    m_pimpl(std::make_unique<ReaderImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
SReader::~SReader() noexcept = default;

//-----------------------------------------------------------------------------

void SReader::starting()
{
    clearLocations();
}

//-----------------------------------------------------------------------------

void SReader::stopping()
{
    clearLocations();
}

//-----------------------------------------------------------------------------

void SReader::configuring()
{
    sight::io::base::service::IReader::configuring();

    const auto& tree = this->getConfigTree();

    // Extension configuration
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
        m_pimpl->m_passwordPolicy = PasswordKeeper::stringToPasswordPolicy(
            password->get<std::string>("policy")
        );

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

void SReader::updating()
{
    // Set to failled until successful
    m_readFailed = true;

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

    auto filepath = getFile();
    SIGHT_THROW_IF("The file '" << filepath << "' is an existing folder.", std::filesystem::is_directory(filepath));

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

                if(m_pimpl->m_passwordRetry > 0
                   || (m_pimpl->m_passwordPolicy == PasswordKeeper::PasswordPolicy::ALWAYS)
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

    const auto readJob = core::jobs::Job::New(
        "Reading " + filepath.string() + " file",
        [&](core::jobs::Job& runningJob)
        {
            runningJob.doneWork(10);

            // Create the session reader
            auto reader = sight::io::session::SessionReader::New();
            reader->setFile(filepath);
            reader->setPassword(password);
            reader->setEncryptionPolicy(m_pimpl->m_encryptionPolicy);

            // Set cursor to busy state. It will be reset to default even if exception occurs
            const sight::ui::base::BusyCursor busyCursor;

            // Read the file
            reader->read();

            // Set output
            auto newData = std::dynamic_pointer_cast<data::Object>(reader->getObject());
            SIGHT_THROW_IF("Invalid session", !newData);

            auto data = m_data.lock();
            data->shallowCopy(newData);

            auto sig = data->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            {
                core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }

            runningJob.done();
        },
        m_associatedWorker
    );

    core::jobs::Aggregator::sptr jobs = core::jobs::Aggregator::New(filepath.string() + " reader");
    jobs->add(readJob);
    jobs->setCancelable(false);

    m_pimpl->m_jobCreatedSignal->emit(jobs);

    try
    {
        jobs->run().get();
        m_readFailed = false;
    }
    catch(sight::io::zip::exception::BadPassword& badPassword)
    {
        // Ask if the user want to retry.
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Wrong password");
        messageBox.setMessage(
            "The file is password protected and the provided password is wrong.\n\nRetry with a different password ?"
        );
        messageBox.setIcon(ui::base::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(ui::base::dialog::IMessageDialog::RETRY);
        messageBox.addButton(ui::base::dialog::IMessageDialog::CANCEL);

        if(messageBox.show() == sight::ui::base::dialog::IMessageDialog::RETRY)
        {
            m_pimpl->m_passwordRetry++;
            updating();
        }
        else
        {
            m_pimpl->m_passwordRetry = 0;
        }
    }
    catch(std::exception& _e)
    {
        // Handle the error.
        SIGHT_ERROR(_e.what());
        sight::ui::base::dialog::MessageDialog::show(
            "Session reader failed",
            _e.what(),
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
    }
    catch(...)
    {
        // Handle the error.
        sight::ui::base::dialog::MessageDialog::show(
            "Session reader aborted",
            "Reading process aborted",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
}

//-----------------------------------------------------------------------------

void SReader::openLocationDialog()
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
    locationDialog.setOption(ui::base::dialog::ILocationDialog::READ);
    locationDialog.setOption(ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);
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
