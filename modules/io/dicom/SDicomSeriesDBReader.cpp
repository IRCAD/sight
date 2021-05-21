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

#include "SDicomSeriesDBReader.hpp"

#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>
#include <core/tools/System.hpp>

#include <data/helper/SeriesDB.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/SeriesDB.hpp>

#include <io/base/service/IReader.hpp>
#include <io/dicom/reader/SeriesDB.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/LoggerDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <string>
#include <vector>

namespace sight::module::io::dicom
{

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesDBReader::SDicomSeriesDBReader() noexcept :
    m_cancelled(false),
    m_showLogDialog(true),
    m_dicomDirSupport(USER_SELECTION)
{
    m_sigJobCreated = newSignal<JobCreatedSignal>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

SDicomSeriesDBReader::~SDicomSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::configuring()
{
    sight::io::base::service::IReader::configuring();

    // Show log dialog
    core::runtime::ConfigurationElement::sptr logDialog = m_configuration->findConfigurationElement("showLogDialog");
    if(logDialog)
    {
        std::string logDialogStr = logDialog->getValue();
        SIGHT_ASSERT("<showLogDialog> value must be 'yes' or 'no'", logDialogStr == "yes" || logDialogStr == "no");
        m_showLogDialog = (logDialogStr == "yes");
    }

    // Enable dicomdir
    core::runtime::ConfigurationElement::sptr dicomDir = m_configuration->findConfigurationElement("dicomdirSupport");
    if(dicomDir)
    {
        std::string dicomDirStr = dicomDir->getValue();
        SIGHT_ASSERT(
            "<dicomdirSupport> value must be 'always' or 'never' or 'user_selection'",
            dicomDirStr == "always" || dicomDirStr == "never" || dicomDirStr == "user_selection"
        );

        if(dicomDirStr == "always")
        {
            m_dicomDirSupport = ALWAYS;
        }
        else if(dicomDirStr == "never")
        {
            m_dicomDirSupport = NEVER;
        }
        else if(dicomDirStr == "user_selection")
        {
            m_dicomDirSupport = USER_SELECTION;
        }
    }
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? this->getSelectorDialogTitle() : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setType(ui::base::dialog::LocationDialog::FOLDER);

    auto result = core::location::SingleFolder::dynamicCast(dialogFile.show());
    if(result)
    {
        this->setFolder(result->getFolder());
        defaultDirectory->setFolder(result->getFolder());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::starting()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::stopping()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::info(std::ostream& _sstream)
{
    _sstream << "SDicomSeriesDBReader::info";
}

//------------------------------------------------------------------------------

std::string SDicomSeriesDBReader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

data::SeriesDB::sptr SDicomSeriesDBReader::createSeriesDB(const std::filesystem::path& dicomDir)
{
    auto reader                   = sight::io::dicom::reader::SeriesDB::New();
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(dicomDir);

    auto job = reader->getJob();
    m_sigJobCreated->emit(job);

    if(m_dicomDirSupport == USER_SELECTION && reader->isDicomDirAvailable())
    {
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Dicomdir file");
        messageBox.setMessage(
            "There is a dicomdir file in the root folder. "
            "Would you like to use it for the reading process ?"
        );
        messageBox.setIcon(ui::base::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(ui::base::dialog::IMessageDialog::YES_NO);
        sight::ui::base::dialog::IMessageDialog::Buttons button = messageBox.show();

        reader->setDicomdirActivated(button == sight::ui::base::dialog::IMessageDialog::YES);
    }
    else if(m_dicomDirSupport == ALWAYS)
    {
        reader->setDicomdirActivated(true);
    }
    else //m_dicomDirSupport == NEVER
    {
        reader->setDicomdirActivated(false);
    }

    try
    {
        reader->readDicomSeries();

        // Retrieve logger
        core::log::Logger::sptr logger = reader->getLogger();
        logger->sort();

        // Display logger dialog if enabled
        if(m_showLogDialog && !logger->empty())
        {
            std::stringstream ss;
            if(seriesDB->size() > 1)
            {
                ss << "The reading process is over : <b>" << seriesDB->size() << " series</b> have been found. "
                                                                                 "<br>Please verify the log report to be informed of the potential errors.";
            }
            else
            {
                ss << "The reading process is over : <b>" << seriesDB->size() << " series</b> has been found. "
                                                                                 "<br>Please verify the log report to be informed of the potential errors.";
            }

            bool result = false;
            if(!reader->getJob()->cancelRequested())
            {
                result = sight::ui::base::dialog::LoggerDialog::showLoggerDialog(
                    "Reading process over",
                    ss.str(),
                    logger
                );
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || reader->getJob()->cancelRequested())
            {
                data::helper::SeriesDB sDBhelper(seriesDB);
                sDBhelper.clear();
            }
        }
    }
    catch(const std::exception& e)
    {
        m_readFailed = true;
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
    catch(...)
    {
        m_readFailed = true;
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during loading",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }

    m_cancelled = job->cancelRequested();

    return seriesDB;
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::updating()
{
    if(this->hasLocationDefined())
    {
        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        data::SeriesDB::sptr seriesDB = this->createSeriesDB(this->getFolder());

        if(seriesDB->size() > 0 && !m_cancelled)
        {
            // Retrieve dataStruct associated with this service
            data::SeriesDB::sptr associatedSeriesDB =
                this->getInOut<data::SeriesDB>(sight::io::base::service::s_DATA_KEY);
            SIGHT_ASSERT("associated SeriesDB not instanced", associatedSeriesDB);

            // Clear SeriesDB and add new series
            data::helper::SeriesDB sDBhelper(associatedSeriesDB);
            data::mt::ObjectWriteLock lock(associatedSeriesDB);
            sDBhelper.clear();

            // Notify removal.
            sDBhelper.notify();
            {
                data::mt::ObjectWriteLock lock(seriesDB);
                associatedSeriesDB->shallowCopy(seriesDB);
            }

            data::SeriesDB::ContainerType addedSeries = associatedSeriesDB->getContainer();

            auto sig = associatedSeriesDB->signal<data::SeriesDB::AddedSeriesSignalType>(
                data::SeriesDB::s_ADDED_SERIES_SIG
            );
            sig->asyncEmit(addedSeries);
        }

        cursor.setDefaultCursor();
    }
    else
    {
        m_readFailed = true;
    }
}

//-----------------------------------------------------------------------------

sight::io::base::service::IOPathType SDicomSeriesDBReader::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
