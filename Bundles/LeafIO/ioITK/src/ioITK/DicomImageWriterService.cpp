/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
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

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <io/IWriter.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>

#include <itkIO/DicomImageWriter.hpp>

#include "ioITK/DicomImageWriterService.hpp"


namespace ioITK
{

REGISTER_SERVICE( ::io::IWriter , ::ioITK::DicomImageWriterService , ::fwData::Image ) ;

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
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(this->getSelectorDialogTitle());
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    ::fwData::location::Folder::sptr  result;
    result= ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        m_fsImagePath = result->getFolder();
        m_bServiceIsConfigured = true;
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
}

//------------------------------------------------------------------------------

void DicomImageWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomImageWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
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
    SLM_TRACE_FUNC();
    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();
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
    SLM_TRACE_FUNC();
    ::itkIO::DicomImageWriter::NewSptr myWriter;

    myWriter->setObject(_pImage);
    ::fwData::location::Folder::NewSptr loc;
    loc->setFolder(_patientDBPath);
    myWriter->setLocation(loc);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving Image ");
        myWriter->addHandler( progressMeterGUI );
        myWriter->write();

    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                ss.str(),
                ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                "Warning during saving",
                ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK

