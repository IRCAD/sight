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

#include "SExtract.hpp"

#include <core/com/Signal.hxx>
#include <core/crypto/PasswordKeeper.hpp>
#include <core/crypto/secure_string.hpp>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/System.hpp>

#include <io/session/SessionReader.hpp>
#include <io/zip/exception/Read.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/InputDialog.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

namespace sight::module::io::zip
{

using core::crypto::PasswordKeeper;
using core::crypto::secure_string;
using sight::io::zip::Archive;

/// Private implementation
class SExtract::ExtractImpl
{
public:

    /// Delete default constructors and assignment operators
    ExtractImpl(const ExtractImpl&)            = delete;
    ExtractImpl(ExtractImpl&&)                 = delete;
    ExtractImpl& operator=(const ExtractImpl&) = delete;
    ExtractImpl& operator=(ExtractImpl&&)      = delete;

    /// Constructor
    inline explicit ExtractImpl(SExtract* const reader) noexcept :
        m_reader(reader),
        m_job_created_signal(reader->newSignal<JobCreatedSignal>("jobCreated"))
    {
    }

    /// Default destructor
    inline ~ExtractImpl() noexcept = default;

    /// Pointer to the public interface
    SExtract* const m_reader;

    /// Signal emitted when job created.
    JobCreatedSignal::sptr m_job_created_signal;

    /// Used in case of bad password
    int m_password_retry {0};

    /// The path where to extract the files
    std::filesystem::path m_outputPath;
};

SExtract::SExtract() noexcept :
    m_pimpl(std::make_unique<ExtractImpl>(this))
{
}

// Defining the destructor here, allows us to use PImpl with a unique_ptr
SExtract::~SExtract() noexcept = default;

//-----------------------------------------------------------------------------

void SExtract::starting()
{
}

//-----------------------------------------------------------------------------

void SExtract::stopping()
{
    m_pimpl->m_password_retry = 0;
    clearLocations();
}

//-----------------------------------------------------------------------------

void SExtract::configuring()
{
    sight::io::base::service::IReader::configuring();
}

//-----------------------------------------------------------------------------

void SExtract::updating()
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
        static auto defaultLocation = std::make_shared<core::location::SingleFolder>();
        defaultLocation->setFolder("/");

        sight::ui::base::dialog::LocationDialog locationDialog;
        locationDialog.setTitle("Enter the folder where the files must be extracted");

        if(!m_windowTitle.empty())
        {
            locationDialog.setTitle(m_windowTitle);
        }
        else
        {
            locationDialog.setTitle("Enter the output path");
        }

        locationDialog.setDefaultLocation(defaultLocation);
        locationDialog.setOption(ui::base::dialog::ILocationDialog::WRITE);
        locationDialog.setType(ui::base::dialog::ILocationDialog::FOLDER);

        const auto result = std::dynamic_pointer_cast<core::location::SingleFolder>(locationDialog.show());

        if(result)
        {
            if(!std::filesystem::is_empty(result->getFolder()))
            {
                sight::ui::base::dialog::MessageDialog messageDialog("Output path not empty",
                                                                     "The output path isn't empty. Continue anyway?",
                                                                     sight::ui::base::dialog::IMessageDialog::WARNING);
                messageDialog.addButton(sight::ui::base::dialog::IMessageDialog::YES);
                messageDialog.addButton(sight::ui::base::dialog::IMessageDialog::CANCEL);
                messageDialog.addButton(sight::ui::base::dialog::IMessageDialog::RETRY);
                auto action = messageDialog.show();
                switch(action)
                {
                    case sight::ui::base::dialog::IMessageDialog::YES:
                        // Continue as normal
                        break;

                    case sight::ui::base::dialog::IMessageDialog::CANCEL:
                        // Totally abort the operation
                        clearLocations();
                        return;

                    case sight::ui::base::dialog::IMessageDialog::RETRY:
                        // Let the user choose another output path
                        updating();
                        return;

                    default:
                        SIGHT_ASSERT("Invalid button", false);
                }
            }

            m_pimpl->m_outputPath = result->getFolder();
            defaultLocation->setFolder(result->getFolder().parent_path());
            locationDialog.saveDefaultLocation(defaultLocation);
        }
        else
        {
            return;
        }
    }

    const auto filepath = getFile();
    SIGHT_THROW_IF("The file '" << filepath << "' is an existing folder.", std::filesystem::is_directory(filepath));

    // Ask password if needed
    const secure_string& password =
        [&]
        {
            const secure_string& globalPassword = PasswordKeeper::get_global_password();

            if(m_pimpl->m_password_retry > 0)
            {
                const auto& [newPassword, ok] =
                    sight::ui::base::dialog::InputDialog::showInputDialog(
                        "Enter Password",
                        "Password:",
                        globalPassword.c_str(), // NOLINT(readability-redundant-string-cstr)
                        sight::ui::base::dialog::InputDialog::EchoMode::PASSWORD
                    );

                return secure_string(newPassword);
            }

            if(globalPassword.empty())
            {
                if constexpr(core::crypto::PasswordKeeper::has_default_password())
                {
                    return core::crypto::PasswordKeeper::get_default_password();
                }
            }

            return globalPassword;
        }();

    const auto readJob = core::jobs::Job::New(
        "Reading " + filepath.string() + " file",
        [&](core::jobs::Job& runningJob)
        {
            const sight::ui::base::BusyCursor busyCursor;
            runningJob.doneWork(10);
            sight::io::zip::ArchiveReader::get(
                filepath,
                Archive::ArchiveFormat::DEFAULT
            )->extractAllTo(m_pimpl->m_outputPath, password);
            runningJob.done();
        },
        this->worker()
    );

    core::jobs::Aggregator::sptr jobs = core::jobs::Aggregator::New(filepath.string() + " reader");
    jobs->add(readJob);
    jobs->setCancelable(false);

    m_pimpl->m_job_created_signal->emit(jobs);

    try
    {
        jobs->run().get();
        m_pimpl->m_outputPath.clear();
        m_readFailed              = false;
        m_pimpl->m_password_retry = 0;
        clearLocations();
        sight::ui::base::dialog::MessageDialog::show("Success", "The archive was successfully extracted.");
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
        sight::ui::base::dialog::MessageDialog::show(
            "Session reader failed",
            e.what(),
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
    }
    catch(...)
    {
        m_pimpl->m_outputPath.clear();
        clearLocations();
        sight::ui::base::dialog::MessageDialog::show(
            "Session reader aborted",
            "Reading process aborted",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
}

//-----------------------------------------------------------------------------

void SExtract::openLocationDialog()
{
    static auto defaultLocation = std::make_shared<core::location::SingleFolder>();
    defaultLocation->setFolder("/");

    sight::ui::base::dialog::LocationDialog locationDialog;

    if(!m_windowTitle.empty())
    {
        locationDialog.setTitle(m_windowTitle);
    }
    else
    {
        locationDialog.setTitle("Enter archive file");
    }

    locationDialog.setDefaultLocation(defaultLocation);
    locationDialog.setOption(ui::base::dialog::ILocationDialog::READ);
    locationDialog.setOption(ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);
    locationDialog.setType(ui::base::dialog::ILocationDialog::SINGLE_FILE);

    const auto result = std::dynamic_pointer_cast<core::location::SingleFile>(locationDialog.show());

    if(result)
    {
        const auto& filepath = result->getFile();
        setFile(filepath);

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

} // namespace sight::module::io::zip
