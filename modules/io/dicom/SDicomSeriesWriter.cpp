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

#include "SDicomSeriesWriter.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/progress_to_logger.hpp>

#include <data/DicomSeries.hpp>

#include <io/__/service/writer.hpp>
#include <io/dicom/helper/DicomSeriesWriter.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

namespace sight::module::io::dicom
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesWriter::SDicomSeriesWriter() noexcept :
    m_sigJobCreated(new_signal<JobCreatedSignal>(JOB_CREATED_SIGNAL))
{
}

//------------------------------------------------------------------------------

SDicomSeriesWriter::~SDicomSeriesWriter() noexcept =
    default;

//------------------------------------------------------------------------------

void SDicomSeriesWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory for DICOM images" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::dialog::location::WRITE);
    dialogFile.setType(ui::dialog::location::FOLDER);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialogFile.show());
    if(result)
    {
        defaultDirectory->set_folder(result->get_folder());
        this->set_folder(result->get_folder());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::starting()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto data   = m_data.lock();
        const auto series = std::dynamic_pointer_cast<const data::DicomSeries>(data.get_shared());

        const std::filesystem::path& folder = this->get_folder();
        if(!std::filesystem::is_empty(folder))
        {
            sight::ui::dialog::message dialog;
            dialog.setMessage(
                "Folder '" + folder.string() + "' isn't empty, files can be overwritten."
                                               "\nDo you want to continue ?"
            );
            dialog.setTitle("Folder not empty.");
            dialog.setIcon(ui::dialog::message::QUESTION);
            dialog.addButton(sight::ui::dialog::message::YES_NO);
            sight::ui::dialog::message::Buttons button = dialog.show();

            if(button == sight::ui::dialog::message::NO)
            {
                m_writeFailed = true;
                return;
            }
        }
        else
        {
            m_writeFailed = true;
        }

        if(series->getModality() == "OT")
        {
            sight::ui::dialog::message dialog;
            dialog.setMessage(
                "Series modality is '" + series->getModality() + "' some information can be lost."
                                                                 "\nDo you want to continue ?"
            );
            dialog.setTitle("Series modality.");
            dialog.setIcon(ui::dialog::message::QUESTION);
            dialog.addButton(sight::ui::dialog::message::YES_NO);
            sight::ui::dialog::message::Buttons button = dialog.show();

            if(button == sight::ui::dialog::message::NO)
            {
                m_writeFailed = true;
                return;
            }
        }

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);
        this->saveDicomSeries(folder, series);
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::saveDicomSeries(
    const std::filesystem::path folder,
    const data::DicomSeries::csptr& series
) const
{
    auto writer = std::make_shared<sight::io::dicom::helper::DicomSeriesWriter>();

    writer->setObject(series);
    writer->set_folder(folder);
    m_sigJobCreated->emit(writer->getJob());

    try
    {
        sight::ui::dialog::progress progressMeterGUI("Saving series ");
        writer->write();
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
    }
    catch(...)
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during saving",
            sight::ui::dialog::message::WARNING
        );
    }
}

//-----------------------------------------------------------------------------

sight::io::service::IOPathType SDicomSeriesWriter::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
