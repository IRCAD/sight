/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwTools/ProgressToLogger.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>

#include <io/IWriter.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>

#include <vtkGdcmIO/DicomImageWriter.hpp>

#include "ioVtkGdcm/DicomImageWriterService.hpp"




namespace ioVtkGdcm
{

REGISTER_SERVICE( ::io::IWriter , ::ioVtkGdcm::DicomImageWriterService , ::fwData::Image ) ;

//------------------------------------------------------------------------------

DicomImageWriterService::DicomImageWriterService() throw() :
    m_bServiceIsConfigured(false),
    m_fsImagePath("")
{}

//------------------------------------------------------------------------------

DicomImageWriterService::~DicomImageWriterService() throw()
{}

//------------------------------------------------------------------------------

void DicomImageWriterService::configuring() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void DicomImageWriterService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(this->getSelectorDialogTitle());
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE );
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    ::fwData::location::Folder::sptr  result;
    result= ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        m_fsImagePath = result->getFolder();
        m_bServiceIsConfigured = true;
    }
}

//------------------------------------------------------------------------------

void DicomImageWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE("DicomImageWriterService::starting()");
}

//------------------------------------------------------------------------------

void DicomImageWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE("DicomImageWriterService::stopping()");
}

//------------------------------------------------------------------------------

void DicomImageWriterService::info(std::ostream &_sstream )
{
    _sstream << "DicomImageWriterService::info";
}

//------------------------------------------------------------------------------

std::vector< std::string > DicomImageWriterService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    return extensions ;
}

//------------------------------------------------------------------------------

std::string DicomImageWriterService::getSelectorDialogTitle()
{
    return "Choose a directory for DICOM images";
}


//------------------------------------------------------------------------------

void DicomImageWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("DicomImageWriterService::updating()");
    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::sptr associatedImage =  this->getObject< ::fwData::Image >() ;
        SLM_ASSERT("associatedImage not instanced", associatedImage);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        saveImage(m_fsImagePath, associatedImage);
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void DicomImageWriterService::saveImage( const ::boost::filesystem::path _patientDBPath,
        ::fwData::Image::sptr _pImage )
{
    SLM_TRACE("DicomImageWriterService::saveImage");
    ::vtkGdcmIO::DicomImageWriter myWriter;

    myWriter.setObject(_pImage);
    ::boost::shared_ptr< ::fwData::location::Folder > loc(new ::fwData::location::Folder());
    loc->setFolder(_patientDBPath);
    myWriter.setLocation(loc);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving Image ");
        myWriter.addHandler( progressMeterGUI );
        myWriter.write();

    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during saving : ";
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtkGdcm
