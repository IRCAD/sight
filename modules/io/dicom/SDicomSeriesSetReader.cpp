/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "SDicomSeriesSetReader.hpp"

#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>
#include <core/tools/System.hpp>

#include <data/SeriesSet.hpp>

#include <io/base/service/IReader.hpp>
#include <io/dicom/reader/SeriesSet.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/LoggerDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

namespace sight::module::io::dicom
{

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesSetReader::SDicomSeriesSetReader() noexcept :
    m_cancelled(false),
    m_showLogDialog(true),
    m_dicomDirSupport(USER_SELECTION)
{
    m_sigJobCreated = newSignal<JobCreatedSignal>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

void SDicomSeriesSetReader::configuring()
{
    sight::io::base::service::IReader::configuring();

    const auto& config = this->getConfigTree();

    m_showLogDialog = config.get<bool>("showLogDialog", m_showLogDialog);

    if(const auto dicomDir = config.get_optional<std::string>("dicomdirSupport"); dicomDir.has_value())
    {
        const std::string& dicomDirStr = dicomDir.value();
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

void SDicomSeriesSetReader::openLocationDialog()
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

void SDicomSeriesSetReader::starting()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesSetReader::stopping()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesSetReader::info(std::ostream& _sstream)
{
    _sstream << "SDicomSeriesSetReader::info";
}

//------------------------------------------------------------------------------

std::string SDicomSeriesSetReader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

data::SeriesSet::sptr SDicomSeriesSetReader::createSeriesSet(const std::filesystem::path& dicomDir)
{
    auto reader     = sight::io::dicom::reader::SeriesSet::New();
    auto series_set = data::SeriesSet::New();
    reader->setObject(series_set);
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
            if(series_set->size() > 1)
            {
                ss << "The reading process is over : <b>" << series_set->size() << " series</b> have been found. "
                                                                                   "<br>Please verify the log report to be informed of the potential errors.";
            }
            else
            {
                ss << "The reading process is over : <b>" << series_set->size() << " series</b> has been found. "
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
                const auto scoped_emitter = series_set->scoped_emit();
                series_set->clear();
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

    return series_set;
}

//------------------------------------------------------------------------------

void SDicomSeriesSetReader::updating()
{
    if(this->hasLocationDefined())
    {
        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        auto series_set = this->createSeriesSet(this->getFolder());

        if(!series_set->empty() && !m_cancelled)
        {
            // Retrieve dataStruct associated with this service
            const auto data                  = m_data.lock();
            const auto associated_series_set = std::dynamic_pointer_cast<data::SeriesSet>(data.get_shared());
            SIGHT_ASSERT("associated SeriesSet not instanced", associated_series_set);

            // Clear SeriesSet and add new series
            const auto scoped_emitter = associated_series_set->scoped_emit();

            associated_series_set->clear();
            associated_series_set->shallowCopy(series_set);
        }

        cursor.setDefaultCursor();
    }
    else
    {
        m_readFailed = true;
    }
}

//-----------------------------------------------------------------------------

sight::io::base::service::IOPathType SDicomSeriesSetReader::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
