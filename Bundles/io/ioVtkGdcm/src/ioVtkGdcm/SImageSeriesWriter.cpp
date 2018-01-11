/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioVtkGdcm/SImageSeriesWriter.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>

#include <fwGui/backend.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwServices/macros.hpp>

#include <vtkGdcmIO/ImageSeriesWriter.hpp>

#include <io/IWriter.hpp>

namespace ioVtkGdcm
{

fwServicesRegisterMacro( ::io::IWriter, ::ioVtkGdcm::SImageSeriesWriter, ::fwMedData::ImageSeries );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SImageSeriesWriter::SImageSeriesWriter() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

SImageSeriesWriter::~SImageSeriesWriter() noexcept
{
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::configureWithIHM()
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

void SImageSeriesWriter::starting()
{
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::configuring()
{
    ::io::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::updating()
{
    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwMedData::ImageSeries::sptr series = this->getObject< ::fwMedData::ImageSeries >();
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
        this->saveImageSeries(folder, series);
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::saveImageSeries( const ::boost::filesystem::path folder,
                                          ::fwMedData::ImageSeries::sptr series )
{
    ::vtkGdcmIO::ImageSeriesWriter::sptr writer = ::vtkGdcmIO::ImageSeriesWriter::New();

    writer->setObject(series);
    ::fwData::location::Folder::sptr loc = ::fwData::location::Folder::New();
    loc->setFolder(folder);
    writer->setLocation(loc);

    m_sigJobCreated->emit(writer->getJob());

    try
    {
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

::io::IOPathType SImageSeriesWriter::getIOPathType() const
{
    return ::io::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace ioVtkGdcm
