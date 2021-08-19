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

#include "SDicomSeriesWriter.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/ProgressToLogger.hpp>

#include <data/DicomSeries.hpp>

#include <io/base/service/IWriter.hpp>
#include <io/dicom/helper/DicomSeriesWriter.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

namespace sight::module::io::dicom
{

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesWriter::SDicomSeriesWriter() noexcept :
    m_cancelled(false)
{
    m_sigJobCreated = newSignal<JobCreatedSignal>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

SDicomSeriesWriter::~SDicomSeriesWriter() noexcept
{
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory for DICOM images" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);
    dialogFile.setType(ui::base::dialog::LocationDialog::FOLDER);

    auto result = core::location::SingleFolder::dynamicCast(dialogFile.show());
    if(result)
    {
        defaultDirectory->setFolder(result->getFolder());
        this->setFolder(result->getFolder());
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
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        data::DicomSeries::csptr series =
            this->getInput<data::DicomSeries>(sight::io::base::service::s_DATA_KEY);
        const std::filesystem::path& folder = this->getFolder();
        if(!std::filesystem::is_empty(folder))
        {
            sight::ui::base::dialog::MessageDialog dialog;
            dialog.setMessage(
                "Folder '" + folder.string() + "' isn't empty, files can be overwritten."
                                               "\nDo you want to continue ?"
            );
            dialog.setTitle("Folder not empty.");
            dialog.setIcon(ui::base::dialog::MessageDialog::QUESTION);
            dialog.addButton(sight::ui::base::dialog::MessageDialog::YES_NO);
            sight::ui::base::dialog::MessageDialog::Buttons button = dialog.show();

            if(button == sight::ui::base::dialog::MessageDialog::NO)
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
            sight::ui::base::dialog::MessageDialog dialog;
            dialog.setMessage(
                "Series modality is '" + series->getModality() + "' some information can be lost."
                                                                 "\nDo you want to continue ?"
            );
            dialog.setTitle("Series modality.");
            dialog.setIcon(ui::base::dialog::MessageDialog::QUESTION);
            dialog.addButton(sight::ui::base::dialog::MessageDialog::YES_NO);
            sight::ui::base::dialog::MessageDialog::Buttons button = dialog.show();

            if(button == sight::ui::base::dialog::MessageDialog::NO)
            {
                m_writeFailed = true;
                return;
            }
        }

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);
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
    auto writer = sight::io::dicom::helper::DicomSeriesWriter::New();

    writer->setObject(series);
    writer->setFolder(folder);
    m_sigJobCreated->emit(writer->getJob());

    try
    {
        sight::ui::base::dialog::ProgressDialog progressMeterGUI("Saving series ");
        writer->write();
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
    catch(...)
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during saving",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
}

//-----------------------------------------------------------------------------

sight::io::base::service::IOPathType SDicomSeriesWriter::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
