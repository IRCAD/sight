/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/version.h>

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/fieldHelper/BackupHelper.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/MessageDialog.hpp>
#include <fwGui/LocationDialog.hpp>

#include <fwData/location/Folder.hpp>

#include <fwWX/ProgressTowx.hpp>
#include <vtkIO/ImageWriter.hpp>

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
        m_bServiceIsConfigured = ::boost::filesystem::exists(m_fsImgPath);
        OSLM_TRACE("Filename found" << filename ) ;
    }
}

//------------------------------------------------------------------------------

void ImageWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::LocationDialog dialogFile;
    dialogFile.setTitle("Choose an vtk file to save an image");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk","*.vtk");
    dialogFile.setOption(::fwGui::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        m_fsImgPath = result->getPath();
        m_bServiceIsConfigured = true;
        _sDefaultPath = m_fsImgPath.branch_path();
    }
}

//------------------------------------------------------------------------------

void ImageWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE("ImageWriterService::starting()");
}

//------------------------------------------------------------------------------

void ImageWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE("ImageWriterService::stopping()");
}

//------------------------------------------------------------------------------

void ImageWriterService::info(std::ostream &_sstream )
{
    _sstream << "ImageWriterService::info";
}

//------------------------------------------------------------------------------

bool ImageWriterService::saveImage( const ::boost::filesystem::path vtkFile, ::boost::shared_ptr< ::fwData::Image > _pImg )
{
    SLM_TRACE("ImageWriterService::saveImage");
    ::vtkIO::ImageWriter myWriter;

    myWriter.setObject(_pImg);
    myWriter.setFile(vtkFile);

    bool bValue = true;

    try
    {
        ::fwWX::ProgressTowx progressMeterGUI("Saving Images ");
        myWriter.addHandler( progressMeterGUI );
        myWriter.write();

    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ::fwGui::IMessageDialog::Icons icon = ::fwGui::IMessageDialog::WARNING;
        ::fwGui::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::IMessageDialog::OK);
        messageBox.show();

        bValue = false;
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading. ";

        ::fwGui::IMessageDialog::Icons icon = ::fwGui::IMessageDialog::WARNING;
        ::fwGui::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::IMessageDialog::OK);
        messageBox.show();

        bValue = false;
    }
    return bValue;
}

//------------------------------------------------------------------------------

void ImageWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("ImageWriterService::updating()");

    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >() ;
        assert(pImage);

        wxBeginBusyCursor();
        saveImage(m_fsImgPath,pImage);
        wxEndBusyCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
