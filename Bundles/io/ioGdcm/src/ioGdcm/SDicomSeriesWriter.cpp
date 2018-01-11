/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioGdcm/SDicomSeriesWriter.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>

#include <fwGdcmIO/helper/DicomSeriesWriter.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/ProgressToLogger.hpp>

#include <io/IWriter.hpp>

namespace ioGdcm
{

fwServicesRegisterMacro( ::io::IWriter, ::ioGdcm::SDicomSeriesWriter, ::fwMedData::DicomSeries );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesWriter::SDicomSeriesWriter() noexcept :
    m_sigJobCreated(JobCreatedSignal::New()),
    m_cancelled(false)
{
    ::fwCom::HasSignals::m_signals
        ( JOB_CREATED_SIGNAL, m_sigJobCreated );
}

//------------------------------------------------------------------------------

SDicomSeriesWriter::~SDicomSeriesWriter() noexcept
{
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory for DICOM images" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( result->getFolder() );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
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
    ::io::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::updating()
{
    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwMedData::DicomSeries::sptr series = this->getObject< ::fwMedData::DicomSeries >();
        const ::boost::filesystem::path& folder = this->getFolder();
        if(!::boost::filesystem::is_empty(folder))
        {
            ::fwGui::dialog::MessageDialog dialog;
            dialog.setMessage("Folder '"+folder.string()+"' isn't empty, files can be overwritten."
                              "\nDo you want to continue ?");
            dialog.setTitle("Folder not empty.");
            dialog.setIcon(::fwGui::dialog::MessageDialog::QUESTION);
            dialog.addButton( ::fwGui::dialog::MessageDialog::YES_NO );
            ::fwGui::dialog::MessageDialog::Buttons button = dialog.show();

            if(button == ::fwGui::dialog::MessageDialog::NO)
            {
                return;
            }
        }

        if (series->getModality() == "OT")
        {
            ::fwGui::dialog::MessageDialog dialog;
            dialog.setMessage("Series modality is '" + series->getModality() + "' some information can be lost."
                              "\nDo you want to continue ?");
            dialog.setTitle("Series modality.");
            dialog.setIcon(::fwGui::dialog::MessageDialog::QUESTION);
            dialog.addButton( ::fwGui::dialog::MessageDialog::YES_NO );
            ::fwGui::dialog::MessageDialog::Buttons button = dialog.show();

            if(button == ::fwGui::dialog::MessageDialog::NO)
            {
                return;
            }
        }
        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        this->saveDicomSeries(folder, series);
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void SDicomSeriesWriter::saveDicomSeries( const ::boost::filesystem::path folder,
                                          ::fwMedData::DicomSeries::sptr series ) const
{
    ::fwGdcmIO::helper::DicomSeriesWriter::sptr writer = ::fwGdcmIO::helper::DicomSeriesWriter::New();

    writer->setObject(series);
    ::fwData::location::Folder::sptr loc = ::fwData::location::Folder::New();
    loc->setFolder(folder);
    writer->setLocation(loc);
    m_sigJobCreated->emit(writer->getJob());

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving series ");
        writer->write();
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning", "Warning during saving", ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType SDicomSeriesWriter::getIOPathType() const
{
    return ::io::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace ioGdcm
