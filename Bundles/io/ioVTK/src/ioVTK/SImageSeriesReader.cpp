/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioVTK/SImageSeriesReader.hpp"

#include "ioVTK/SImageReader.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwIO/ioTypes.hpp>
#include <fwIO/IReader.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Failed.hpp>

#include <fwVtkIO/BitmapImageReader.hpp>

namespace ioVTK
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioVTK::SImageSeriesReader, ::fwMedData::ImageSeries );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SImageSeriesReader::SImageSeriesReader() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SImageSeriesReader::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SImageSeriesReader::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    // Initialize the available extensions for BitmapImageReader
    std::vector<std::string> ext;
    ::fwVtkIO::BitmapImageReader::getAvailableExtensions(ext);
    std::string availableExtensions = "";

    if(ext.size() > 0)
    {
        availableExtensions = "*" + ext.at(0);
        for(size_t i = 1; i < ext.size(); i++)
        {
            availableExtensions = availableExtensions + " *" + ext.at(i);
        }
    }

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load an ImageSeries" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.addFilter("Bitmap image", availableExtensions);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SImageSeriesReader::starting()
{
}

//------------------------------------------------------------------------------

void SImageSeriesReader::stopping()
{
}

//------------------------------------------------------------------------------

void SImageSeriesReader::configuring()
{
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void SImageSeriesReader::info(std::ostream& _sstream )
{
    _sstream << "SImageSeriesReader::info";
}

//------------------------------------------------------------------------------

void SImageSeriesReader::updating()
{
    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwMedData::ImageSeries::sptr imageSeries = this->getInOut< ::fwMedData::ImageSeries >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("ImageSeries is not instanced", imageSeries);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        try
        {
            ::fwData::Image::sptr image = ::fwData::Image::New();

            if ( SImageReader::loadImage( this->getFile(), image, m_sigJobCreated ) )
            {
                imageSeries->setImage(image);
                this->notificationOfDBUpdate();
            }
        }
        catch(::fwTools::Failed& e)
        {
            OSLM_TRACE("Error : " << e.what());
            FW_RAISE_EXCEPTION(e);
        }
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void SImageSeriesReader::notificationOfDBUpdate()
{
    ::fwMedData::ImageSeries::sptr imageSeries = this->getInOut< ::fwMedData::ImageSeries >(::fwIO::s_DATA_KEY);
    SLM_ASSERT("imageSeries not instanced", imageSeries);

    auto sig = imageSeries->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
