/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/fieldHelper/BackupHelper.hpp>

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
#include <vtkIO/ImageWriter.hpp>
#include <vtkIO/MetaImageWriter.hpp>
#include <vtkIO/VtiImageWriter.hpp>

#include "ioVTK/ImageWriterService.hpp"

namespace ioVTK
{

REGISTER_SERVICE( ::io::IWriter , ::ioVTK::ImageWriterService , ::fwData::Image ) ;

//------------------------------------------------------------------------------

ImageWriterService::ImageWriterService() throw() :
    m_bServiceIsConfigured(false),
    m_fsImgPath("")
{}

//------------------------------------------------------------------------------

ImageWriterService::~ImageWriterService() throw()
{}

//------------------------------------------------------------------------------

void ImageWriterService::configuring() throw(::fwTools::Failed)
{
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id") ;
        m_fsImgPath = ::boost::filesystem::path( filename ) ;
        std::string ext = ::boost::filesystem::extension(m_fsImgPath);
        bool bIsAuthorizedExtension = (ext == ".vtk" || ext == ".vti" || ext ==".mhd");
        OSLM_TRACE_IF("Extension not supported. File ignored." << ext,  !bIsAuthorizedExtension);
        m_bServiceIsConfigured = ::boost::filesystem::is_regular_file(m_fsImgPath) && bIsAuthorizedExtension;
        OSLM_TRACE("Filename found" << filename ) ;
    }
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

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        m_fsImgPath = result->getPath();
        m_bServiceIsConfigured = true;
        _sDefaultPath = m_fsImgPath.parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
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

bool ImageWriterService::saveImage( const ::boost::filesystem::path imgFile, ::boost::shared_ptr< ::fwData::Image > _pImg )
{
    SLM_TRACE_FUNC();
    bool bValue = true;

    ::fwDataIO::writer::IObjectWriter::sptr myWriter;
    fwGui::dialog::ProgressDialog progressMeterGUI("Saving Images ");
    std::string ext = ::boost::filesystem::extension(imgFile);
    ::boost::algorithm::to_lower(ext);

    if(ext == ".vtk")
    {
        ::vtkIO::ImageWriter::NewSptr vtkWriter;
        vtkWriter->addHandler( progressMeterGUI );
        // Set the file system path
        vtkWriter->setFile(imgFile);
        myWriter = vtkWriter;
    }
    else if(ext == ".vti")
    {
        ::vtkIO::VtiImageWriter::NewSptr vtiWriter;
        vtiWriter->addHandler( progressMeterGUI );
        vtiWriter->setFile(imgFile);
        myWriter = vtiWriter;
    }
    else if(ext == ".mhd")
    {
        ::vtkIO::MetaImageWriter::NewSptr mhdWriter;
        mhdWriter->addHandler( progressMeterGUI );
        mhdWriter->setFile(imgFile);
        myWriter = mhdWriter;
    }
    else
    {
        OSLM_FATAL("Unknown extension for file "<< imgFile);
    }

    myWriter->setObject(_pImg);

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

    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >() ;
        SLM_ASSERT("pImage not instanced", pImage);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        this->saveImage(m_fsImgPath,pImage);

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
