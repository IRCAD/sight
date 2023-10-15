/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "dicom_series_set_reader.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>
#include <core/tools/system.hpp>

#include <data/series_set.hpp>

#include <io/__/service/reader.hpp>
#include <io/dicom/reader/series_set.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/logger.hpp>
#include <ui/__/dialog/message.hpp>

namespace sight::module::io::dicom
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

dicom_series_set_reader::dicom_series_set_reader() noexcept :
    m_cancelled(false),
    m_showLogDialog(true),
    m_dicomDirSupport(USER_SELECTION)
{
    m_sigJobCreated = new_signal<JobCreatedSignal>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

void dicom_series_set_reader::configuring()
{
    sight::io::service::reader::configuring();

    const auto& config = this->get_config();

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

void dicom_series_set_reader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? this->getSelectorDialogTitle() : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::dialog::location::READ);
    dialogFile.setType(ui::dialog::location::FOLDER);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialogFile.show());
    if(result)
    {
        this->set_folder(result->get_folder());
        defaultDirectory->set_folder(result->get_folder());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
}

//------------------------------------------------------------------------------

void dicom_series_set_reader::starting()
{
}

//------------------------------------------------------------------------------

void dicom_series_set_reader::stopping()
{
}

//------------------------------------------------------------------------------

void dicom_series_set_reader::info(std::ostream& _sstream)
{
    _sstream << "dicom_series_set_reader::info";
}

//------------------------------------------------------------------------------

std::string dicom_series_set_reader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

data::series_set::sptr dicom_series_set_reader::createSeriesSet(const std::filesystem::path& dicomDir)
{
    auto reader     = std::make_shared<sight::io::dicom::reader::series_set>();
    auto series_set = std::make_shared<data::series_set>();
    reader->set_object(series_set);
    reader->set_folder(dicomDir);

    auto job = reader->getJob();
    m_sigJobCreated->emit(job);

    if(m_dicomDirSupport == USER_SELECTION && reader->isDicomDirAvailable())
    {
        sight::ui::dialog::message messageBox;
        messageBox.setTitle("Dicomdir file");
        messageBox.setMessage(
            "There is a dicomdir file in the root folder. "
            "Would you like to use it for the reading process ?"
        );
        messageBox.setIcon(ui::dialog::message::QUESTION);
        messageBox.addButton(ui::dialog::message::YES_NO);
        sight::ui::dialog::message::Buttons button = messageBox.show();

        reader->setDicomdirActivated(button == sight::ui::dialog::message::YES);
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
        core::log::logger::sptr logger = reader->getLogger();
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
            if(!reader->getJob()->cancel_requested())
            {
                result = sight::ui::dialog::logger::showLoggerDialog(
                    "Reading process over",
                    ss.str(),
                    logger
                );
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || reader->getJob()->cancel_requested())
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
        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
    }
    catch(...)
    {
        m_readFailed = true;
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during loading",
            sight::ui::dialog::message::WARNING
        );
    }

    m_cancelled = job->cancel_requested();

    return series_set;
}

//------------------------------------------------------------------------------

void dicom_series_set_reader::updating()
{
    if(this->hasLocationDefined())
    {
        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        auto series_set = this->createSeriesSet(this->get_folder());

        if(!series_set->empty() && !m_cancelled)
        {
            // Retrieve dataStruct associated with this service
            const auto data                  = m_data.lock();
            const auto associated_series_set = std::dynamic_pointer_cast<data::series_set>(data.get_shared());
            SIGHT_ASSERT("associated series_set not instanced", associated_series_set);

            // Clear series_set and add new series
            const auto scoped_emitter = associated_series_set->scoped_emit();

            associated_series_set->clear();
            associated_series_set->shallow_copy(series_set);
        }

        cursor.setDefaultCursor();
    }
    else
    {
        m_readFailed = true;
    }
}

//-----------------------------------------------------------------------------

sight::io::service::IOPathType dicom_series_set_reader::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
