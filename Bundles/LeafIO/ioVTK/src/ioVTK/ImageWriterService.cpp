/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioVTK/ImageWriterService.hpp"

#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>

#include <fwTools/Failed.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwDataIO/reader/IObjectReader.hpp>
#include <fwVtkIO/ImageWriter.hpp>
#include <fwVtkIO/MetaImageWriter.hpp>
#include <fwVtkIO/VtiImageWriter.hpp>

#include <boost/algorithm/string.hpp>

namespace ioVTK
{

fwServicesRegisterMacro( ::io::IWriter, ::ioVTK::ImageWriterService, ::fwData::Image );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

ImageWriterService::ImageWriterService() throw()
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::io::IOPathType ImageWriterService::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void ImageWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose an file to save an image");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk","*.vtk");
    dialogFile.addFilter("Vti","*.vti");
    dialogFile.addFilter("MetaImage","*.mhd");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

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

void ImageWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ImageWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ImageWriterService::info(std::ostream &_sstream )
{
    _sstream << "ImageWriterService::info";
}

//------------------------------------------------------------------------------

bool ImageWriterService::saveImage( const ::boost::filesystem::path& imgFile,
                                    const SPTR(::fwData::Image)& image,
                                    const SPTR(JobCreatedSignalType)& sigJobCreated )
{
    SLM_TRACE_FUNC();
    bool bValue = true;

    ::fwDataIO::writer::IObjectWriter::sptr myWriter;
    fwGui::dialog::ProgressDialog progressMeterGUI("Saving images... ");
    std::string ext = ::boost::filesystem::extension(imgFile);
    ::boost::algorithm::to_lower(ext);

    if(ext == ".vtk")
    {
        ::fwVtkIO::ImageWriter::sptr vtkWriter = ::fwVtkIO::ImageWriter::New();
        // Set the file system path
        vtkWriter->setFile(imgFile);
        myWriter = vtkWriter;
    }
    else if(ext == ".vti")
    {
        ::fwVtkIO::VtiImageWriter::sptr vtiWriter = ::fwVtkIO::VtiImageWriter::New();
        vtiWriter->setFile(imgFile);
        myWriter = vtiWriter;
    }
    else if(ext == ".mhd")
    {
        ::fwVtkIO::MetaImageWriter::sptr mhdWriter = ::fwVtkIO::MetaImageWriter::New();
        mhdWriter->setFile(imgFile);
        myWriter = mhdWriter;
    }
    else
    {
        FW_RAISE_EXCEPTION(::fwTools::Failed("Only .vtk, .vti and .mhd are supported."));
    }

    myWriter->setObject(image);

    sigJobCreated->emit(myWriter->getJob());

    try
    {
        // Launch writing process
        myWriter->write();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();

        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning",
            ss.str(),
            ::fwGui::dialog::IMessageDialog::WARNING);
        bValue = false;
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning",
            "Warning during saving.",
            ::fwGui::dialog::IMessageDialog::WARNING);
        bValue = false;
    }
    return bValue;
}

//------------------------------------------------------------------------------

void ImageWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >();
        SLM_ASSERT("Image not instanced", pImage);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        try
        {
            this->saveImage(this->getFile(), pImage, m_sigJobCreated);
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

} // namespace ioVtk
