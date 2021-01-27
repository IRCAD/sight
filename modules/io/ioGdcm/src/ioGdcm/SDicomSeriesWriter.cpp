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

#include "ioGdcm/SDicomSeriesWriter.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>
#include <core/tools/ProgressToLogger.hpp>

#include <data/DicomSeries.hpp>
#include <data/location/Folder.hpp>

#include <fwGdcmIO/helper/DicomSeriesWriter.hpp>

#include <fwIO/IWriter.hpp>

#include <gui/Cursor.hpp>
#include <gui/dialog/LocationDialog.hpp>
#include <gui/dialog/MessageDialog.hpp>
#include <gui/dialog/ProgressDialog.hpp>

#include <services/macros.hpp>

namespace ioGdcm
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioGdcm::SDicomSeriesWriter, ::sight::data::DicomSeries )

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

void SDicomSeriesWriter::configureWithIHM()
{
    this->openLocationDialog();
}
//------------------------------------------------------------------------------

void SDicomSeriesWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    gui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory for DICOM images" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(gui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(gui::dialog::LocationDialog::FOLDER);

    data::location::Folder::sptr result;
    result = data::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( result->getFolder() );
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
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
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::updating()
{
    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        data::DicomSeries::csptr series     = this->getInput< data::DicomSeries >(::fwIO::s_DATA_KEY);
        const std::filesystem::path& folder = this->getFolder();
        if(!std::filesystem::is_empty(folder))
        {
            gui::dialog::MessageDialog dialog;
            dialog.setMessage("Folder '"+folder.string()+"' isn't empty, files can be overwritten."
                              "\nDo you want to continue ?");
            dialog.setTitle("Folder not empty.");
            dialog.setIcon(gui::dialog::MessageDialog::QUESTION);
            dialog.addButton( gui::dialog::MessageDialog::YES_NO );
            gui::dialog::MessageDialog::Buttons button = dialog.show();

            if(button == gui::dialog::MessageDialog::NO)
            {
                m_writeFailed = true;
                return;
            }
        }
        else
        {
            m_writeFailed = true;
        }

        if (series->getModality() == "OT")
        {
            gui::dialog::MessageDialog dialog;
            dialog.setMessage("Series modality is '" + series->getModality() + "' some information can be lost."
                              "\nDo you want to continue ?");
            dialog.setTitle("Series modality.");
            dialog.setIcon(gui::dialog::MessageDialog::QUESTION);
            dialog.addButton( gui::dialog::MessageDialog::YES_NO );
            gui::dialog::MessageDialog::Buttons button = dialog.show();

            if(button == gui::dialog::MessageDialog::NO)
            {
                m_writeFailed = true;
                return;
            }
        }
        gui::Cursor cursor;
        cursor.setCursor(gui::ICursor::BUSY);
        this->saveDicomSeries(folder, series);
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::saveDicomSeries( const std::filesystem::path folder,
                                          const data::DicomSeries::csptr& series ) const
{
    ::fwGdcmIO::helper::DicomSeriesWriter::sptr writer = ::fwGdcmIO::helper::DicomSeriesWriter::New();

    writer->setObject(series);
    data::location::Folder::sptr loc = data::location::Folder::New();
    loc->setFolder(folder);
    writer->setLocation(loc);
    m_sigJobCreated->emit(writer->getJob());

    try
    {
        gui::dialog::ProgressDialog progressMeterGUI("Saving series ");
        writer->write();
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        gui::dialog::MessageDialog::show(
            "Warning", ss.str(), gui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        gui::dialog::MessageDialog::show(
            "Warning", "Warning during saving", gui::dialog::IMessageDialog::WARNING);
    }
}

//-----------------------------------------------------------------------------

::fwIO::IOPathType SDicomSeriesWriter::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace ioGdcm
