/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioVTK/ImageReaderService.hpp"

#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>
#include <fwJobs/Aggregator.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <io/IReader.hpp>

#include <fwDataIO/reader/IObjectReader.hpp>
#include <fwVtkIO/ImageReader.hpp>
#include <fwVtkIO/MetaImageReader.hpp>
#include <fwVtkIO/VtiImageReader.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/registry/worker.hpp>

#include <fwCore/base.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

#include <cstdint>
#include <thread>
#include <chrono>

namespace ioVTK
{

//------------------------------------------------------------------------------

// Register a new reader of ::fwData::Image
fwServicesRegisterMacro( ::io::IReader, ::ioVTK::ImageReaderService, ::fwData::Image );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

::io::IOPathType ImageReaderService::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void ImageReaderService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to load an image");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk","*.vtk");
    dialogFile.addFilter("Vti","*.vti");
    dialogFile.addFilter("MetaImage","*.mhd");
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

ImageReaderService::ImageReaderService() throw()
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

void ImageReaderService::starting() throw ( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------

void ImageReaderService::stopping() throw ( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------

void ImageReaderService::info( std::ostream &_sstream )
{
    _sstream << "ImageReaderService::info";
}

//------------------------------------------------------------------------------

void ImageReaderService::updating() throw ( ::fwTools::Failed )
{
    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >();
        SLM_ASSERT("pImage not instanced", pImage);

        // Read new image path and update image. If the reading process is a success, we notify all listeners that image has been modified.

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        try
        {
            if ( this->loadImage( this->getFile(), pImage ) )
            {
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

template< typename READER > typename READER::sptr configureReader(const ::boost::filesystem::path &imgFile )
{
    typename READER::sptr reader = READER::New();
    reader->setFile(imgFile);
    return reader;
}

//------------------------------------------------------------------------------

bool ImageReaderService::loadImage( const ::boost::filesystem::path imgFile, ::fwData::Image::sptr _pImg )
{
    bool ok = true;

    std::string ext = ::boost::filesystem::extension(imgFile);
    ::boost::algorithm::to_lower(ext);

    ::fwDataIO::reader::IObjectReader::sptr imageReader;
    if(ext == ".vtk")
    {
        imageReader = configureReader< ::fwVtkIO::ImageReader >( imgFile );
    }
    else if(ext == ".vti")
    {
        imageReader = configureReader< ::fwVtkIO::VtiImageReader >( imgFile );
    }
    else if(ext == ".mhd")
    {
        imageReader = configureReader< ::fwVtkIO::MetaImageReader >( imgFile );
    }
    else
    {
        FW_RAISE_EXCEPTION(::fwTools::Failed("Only .vtk, .vti and .mhd are supported."));
    }

    // Set the image (already created, but empty) that will be modified
    ::fwData::mt::ObjectWriteLock lock(_pImg);
    imageReader->setObject(_pImg);

    m_sigJobCreated->emit(imageReader->getJob());

    try
    {
        imageReader->read();
    }
    catch(::fwTools::Failed& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning",
            ss.str(),
            ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
        // Raise exception  for superior level
        FW_RAISE_EXCEPTION(e);
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning",
            ss.str(),
            ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning",
            "Warning during loading.",
            ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
    }

    return ok;
}

//------------------------------------------------------------------------------

void ImageReaderService::notificationOfDBUpdate()
{
    ::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >();
    SLM_ASSERT("pImage not instanced", pImage);

    auto sig = pImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

} // namespace ioVtk
