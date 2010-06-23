/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/version.h>
#include <wx/event.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>


#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Object.hpp>
#include <fwData/location/Folder.hpp>

#include <fwServices/Factory.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwServices/ObjectMsg.hpp>

#include <io/IReader.hpp>

#include <fwTools/System.hpp>

#include <ioXML/FwXMLGenericReaderService.hpp>

#include <fwGui/ProgressDialog.hpp>
#include <fwGui/LocationDialog.hpp>
#include <fwWX/wxZipFolder.hpp>
#include <fwWX/convert.hpp>

#include <fwGui/MessageDialog.hpp>

#include "ioXML/FwXMLGenericReaderService.hpp"


//------------------------------------------------------------------------------
//
namespace ioXML
{

REGISTER_SERVICE( ::io::IReader , ::ioXML::FwXMLGenericReaderService , ::fwTools::Object );

//------------------------------------------------------------------------------

FwXMLGenericReaderService::FwXMLGenericReaderService() throw()
{}

//------------------------------------------------------------------------------

FwXMLGenericReaderService::~FwXMLGenericReaderService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::LocationDialog dialogFile;
    dialogFile.setTitle( this->getSelectorDialogTitle() );
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("fwXML archive","*.fxz");
    dialogFile.addFilter("fwXML archive","*.xml");
    dialogFile.setOption(::fwGui::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath();
        m_reader.setFile( result->getPath() );
    }
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLGenericReaderService::starting()");
}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLGenericReaderService::stopping()");
}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLGenericReaderService::info" ;
}

//------------------------------------------------------------------------------

std::vector< std::string > FwXMLGenericReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".xml");
    return extensions ;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

::fwTools::Object::sptr FwXMLGenericReaderService::loadData( const ::boost::filesystem::path inrFileDir )
{
    SLM_TRACE_FUNC();
    ::fwXML::reader::FwXMLObjectReader myLoader;
    ::fwTools::Object::sptr pObject;

    myLoader.setFile(inrFileDir);

    try
    {
        ::fwGui::ProgressDialog progressMeterGUI("Loading data ");
        myLoader.addHandler( progressMeterGUI );
        myLoader.read();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        wxString msg = _("Warning during loading : ");
        ss << wxConvertWX2MB(msg.c_str()) << e.what();
        ::fwGui::IMessageDialog::Icons icon = ::fwGui::IMessageDialog::WARNING;
        ::fwGui::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::IMessageDialog::OK);
        messageBox.show();
        return pObject;
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading : ";
        ::fwGui::IMessageDialog::Icons icon = ::fwGui::IMessageDialog::WARNING;
        ::fwGui::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::IMessageDialog::OK);
        messageBox.show();
        return pObject;
    }

    pObject = ::fwTools::Object::dynamicCast( myLoader.getObject() );

    return pObject;
}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::updating() throw(::fwTools::Failed)
{
    OSLM_TRACE("FwXMLGenericReaderService::updating()  m_fsObjectPath:"<<  m_fsObjectPath);

    SLM_TRACE_FUNC();

    if( !m_reader.getFile().empty() )
    {
        ::fwTools::Object::sptr obj; // object loaded

        wxBeginBusyCursor();
        m_reader.setFile( correctFileFormat( m_reader.getFile() ));
        if ( isAnFwxmlArchive( m_reader.getFile() ) )
        {
            obj = manageZipAndLoadData( m_reader.getFile() );
        }
        else
        {
            obj = loadData(m_reader.getFile() );
        }
        if (obj)
        {
            // Retrieve dataStruct associated with this service
            ::fwTools::Object::sptr associatedObject = this->getObject< ::fwTools::Object >();
            assert( associatedObject ) ;

            associatedObject->shallowCopy( obj );

            notificationOfUpdate();
        }
        wxEndBusyCursor();
    }
}


//------------------------------------------------------------------------------

void FwXMLGenericReaderService::notificationOfUpdate()
{
    SLM_TRACE_FUNC();
    ::fwData::Object::sptr object = this->getObject< ::fwData::Object >();
    assert( object );
    ::fwServices::ObjectMsg::NewSptr msg;
    msg->addEvent( ::fwServices::ObjectMsg::UPDATED_OBJECT , object );
    ::fwServices::IEditionService::notify( this->getSptr(),  object, msg );
}

//------------------------------------------------------------------------------

bool FwXMLGenericReaderService::isAnFwxmlArchive( const ::boost::filesystem::path filePath )
{
    return ( ::boost::filesystem::extension(filePath) == ".fxz" );
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr FwXMLGenericReaderService::manageZipAndLoadData( const ::boost::filesystem::path _pArchivePath )
{

    ::fwTools::Object::sptr obj;

    // Unzip folder
    ::boost::filesystem::path destFolder = ::fwTools::System::getTemporaryFolder() / "fwxmlArchiveFolder";
    ::boost::filesystem::path xmlfile = destFolder / "root.xml";

    OSLM_DEBUG("srcZipFileName = " << _pArchivePath );
    OSLM_DEBUG("destFolderName = " << destFolder );

    wxString srcZipFileName ( wxConvertMB2WX( _pArchivePath.string().c_str() ) );
    wxString destFolderName ( wxConvertMB2WX( destFolder.string().c_str() ) );
    ::fwWX::wxZipFolder::unpackFolder( srcZipFileName, destFolderName );

    // Load
    obj = loadData( xmlfile );

    // Remove temp folder
    ::boost::filesystem::remove_all( destFolder );

    return obj;
}

//------------------------------------------------------------------------------

::boost::filesystem::path FwXMLGenericReaderService::correctFileFormat( const ::boost::filesystem::path _filePath ) const
{
    ::boost::filesystem::path newPath = _filePath;
    if ( ::boost::filesystem::extension(_filePath) != ".fxz" && ::boost::filesystem::extension(_filePath) != ".xml" )
    {
        newPath = _filePath.string() + ".fxz";
    }

    return newPath;
}

} // namespace ioXML
