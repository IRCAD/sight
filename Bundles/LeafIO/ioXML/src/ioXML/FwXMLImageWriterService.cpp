/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>

#include <fwXML/writer/FwXMLObjectWriter.hpp>
#include <fwXML/writer/fwxmlextension.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>

#include "ioXML/FwXMLImageWriterService.hpp"


namespace ioXML
{

REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLImageWriterService , ::fwData::Image ) ;

//------------------------------------------------------------------------------

FwXMLImageWriterService::FwXMLImageWriterService() throw()
{}

//------------------------------------------------------------------------------

FwXMLImageWriterService::~FwXMLImageWriterService() throw()
{}

//------------------------------------------------------------------------------

::io::IOPathType FwXMLImageWriterService::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle( "Choose a " FWXML_ARCHIVE_EXTENSION " or a xml file" );
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("fwXML archive","*.xml");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path() ;
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLImageWriterService::info" ;
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::saveImage( const ::boost::filesystem::path &file, ::fwData::Image::sptr _pImage )
{
    SLM_TRACE_FUNC();
    ::fwXML::writer::FwXMLObjectWriter myWriter;

    myWriter.setObject(_pImage);
    myWriter.setFile(file);

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
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                "Warning during loading.",
                ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void FwXMLImageWriterService::updating() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();
        SLM_ASSERT("associatedImage not instanced", associatedImage);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        saveImage(this->getFile(),associatedImage);
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioXML
