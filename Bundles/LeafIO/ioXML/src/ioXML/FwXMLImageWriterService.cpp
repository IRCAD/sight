/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>

#include <fwXML/writer/FwXMLObjectWriter.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>

#include "ioXML/FwXMLImageWriterService.hpp"


namespace ioXML
{

REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLImageWriterService , ::fwData::Image ) ;

//------------------------------------------------------------------------------

FwXMLImageWriterService::FwXMLImageWriterService() throw() :
    m_bServiceIsConfigured(false),
    m_fsImagePath("")
{}

//------------------------------------------------------------------------------

FwXMLImageWriterService::~FwXMLImageWriterService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::configuring() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle( "Choose a fxz or a xml file" );
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("fwXML archive","*.xml");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath() ;
        m_fsImagePath = result->getPath() ;
        m_bServiceIsConfigured = true;
    }
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLImageWriterService::starting()");
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLImageWriterService::stopping()");
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLImageWriterService::info" ;
}

//------------------------------------------------------------------------------

std::string FwXMLImageWriterService::getCfgExtensionPoint()
{
    return "" ;
}

//------------------------------------------------------------------------------

std::string FwXMLImageWriterService::getPersistanceId()
{
    return "ioITK::FwXMLImageWriterService" ;
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::saveImage( const ::boost::filesystem::path inrFileDir, ::fwData::Image::sptr _pPatient )
{
    SLM_TRACE("FwXMLImageWriterService::createImage");
    ::fwXML::writer::FwXMLObjectWriter myWriter;

    myWriter.setObject(_pPatient);
    myWriter.setFile(inrFileDir);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving Image ");
        myWriter.addHandler( progressMeterGUI );
        myWriter.write();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
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
        ss << "Warning during loading. ";
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::updating() throw(fwTools::Failed)
{
    SLM_TRACE("FwXMLImageWriterService::updating()");

    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();
        assert( associatedImage ) ;

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        saveImage(m_fsImagePath,associatedImage);
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioXML
