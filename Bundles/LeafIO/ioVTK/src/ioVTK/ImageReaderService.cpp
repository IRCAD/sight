/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/ImageMsg.hpp>

#include <io/IReader.hpp>

#include <fwDataIO/reader/IObjectReader.hpp>
#include <vtkIO/ImageReader.hpp>
#include <vtkIO/MetaImageReader.hpp>
#include <vtkIO/VtiImageReader.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>

#include "ioVTK/ImageReaderService.hpp"

namespace ioVTK
{

//------------------------------------------------------------------------------

// Register a new reader of ::fwData::Image
REGISTER_SERVICE( ::io::IReader , ::ioVTK::ImageReaderService , ::fwData::Image );

//------------------------------------------------------------------------------

ImageReaderService::ImageReaderService() throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

ImageReaderService::~ImageReaderService() throw()
{
    SLM_TRACE_FUNC();
    // This method does nothing
}

//------------------------------------------------------------------------------

::io::IOPathType ImageReaderService::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void ImageReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a file to load an image");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk","*.vtk");
    dialogFile.addFilter("Vti","*.vtk");
    dialogFile.addFilter("MetaImage","*.mhd");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr  result;
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

void ImageReaderService::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    // This method does nothing
}

//------------------------------------------------------------------------------

void ImageReaderService::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    // This method does nothing
}

//------------------------------------------------------------------------------

void ImageReaderService::info( std::ostream &_sstream )
{
    _sstream << "ImageReaderService::info";
}

//------------------------------------------------------------------------------

void ImageReaderService::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >() ;
        SLM_ASSERT("pImage not instanced", pImage);

        // Read new image path and update image. If the reading process is a success, we notify all listeners that image has been modified.

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        try
        {
            if ( this->loadImage( this->getFile(), pImage ) )
            {
                notificationOfDBUpdate();
            }
        }
        catch(::fwTools::Failed& e)
        {
            OSLM_TRACE("Error : " << e.what());
            throw e;
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

bool ImageReaderService::loadImage( const ::boost::filesystem::path imgFile, ::fwData::Image::sptr _pImg )
{
    SLM_TRACE_FUNC();
    bool ok = true;

    // Use a reader of vtkIO library to read an image
    ::fwDataIO::reader::IObjectReader::sptr myReader;
    // Create a progress bar and attach it to reader
    ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Image ");
    std::string ext = ::boost::filesystem::extension(imgFile);
    ::boost::algorithm::to_lower(ext);

    if(ext == ".vtk")
    {
        ::vtkIO::ImageReader::NewSptr vtkReader;
        vtkReader->addHandler( progressMeterGUI );
        // Set the file system path
        vtkReader->setFile(imgFile);
        myReader = vtkReader;
    }
    else if(ext == ".vti")
    {
        ::vtkIO::VtiImageReader::NewSptr vtiReader;
        vtiReader->addHandler( progressMeterGUI );
        vtiReader->setFile(imgFile);
        myReader = vtiReader;
    }
    else if(ext == ".mhd")
    {
        ::vtkIO::MetaImageReader::NewSptr mhdReader;
        mhdReader->addHandler( progressMeterGUI );
        mhdReader->setFile(imgFile);
        myReader = mhdReader;
    }
    else
    {
        throw(::fwTools::Failed("Only .vtk, .vti and .mhd are supported."));
    }

    // Set the image (already created, but empty) that will be modified
    myReader->setObject(_pImg);

    try
    {
        // Launch reading process
        myReader->read();
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

void ImageReaderService::updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    // This method does nothing
}

//------------------------------------------------------------------------------

void ImageReaderService::notificationOfDBUpdate()
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >();
    SLM_ASSERT("pImage not instanced", pImage);

    // Creation of an image message to say that image is an new image ( or all fields are modified (old version of msg ) )
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::NEW_IMAGE ) ;
    msg->addEvent( ::fwComEd::ImageMsg::BUFFER ) ;
    msg->addEvent( ::fwComEd::ImageMsg::REGION ) ;
    msg->addEvent( ::fwComEd::ImageMsg::SPACING ) ;
    msg->addEvent( ::fwComEd::ImageMsg::PIXELTYPE ) ;

    // Notify message to all service listeners
    ::fwServices::IEditionService::notify(this->getSptr(), pImage, msg);
}

} // namespace ioVtk
