/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwZip/ZipFolder.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>

#include "ioXML/FwXMLGenericReaderService.hpp"


//------------------------------------------------------------------------------

namespace ioXML
{

REGISTER_SERVICE( ::io::IReader , ::ioXML::FwXMLGenericReaderService , ::fwTools::Object );

//------------------------------------------------------------------------------

FwXMLGenericReaderService::FwXMLGenericReaderService() throw()
        : m_archiveExtenstion (".fxz")
{}

//------------------------------------------------------------------------------

FwXMLGenericReaderService::~FwXMLGenericReaderService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( this->m_configuration->size() > 0 )
    {
        ::fwRuntime::ConfigurationElementContainer::Iterator iter = this->m_configuration->begin() ;
        SLM_ASSERT("Sorry, only one xml element \"archiveExtension\" is accepted.", this->m_configuration->size() == 1 && (*iter)->getName() == "archiveExtension" );
        SLM_ASSERT("Sorry, only xml element \"archiveExtension\" is empty.", ! (*iter)->getValue().empty() );
        m_archiveExtenstion =  (*iter)->getValue();
    }
}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    std::stringstream sstrTitle;
    sstrTitle << "Choose a " << m_archiveExtenstion.substr(1,m_archiveExtenstion.size()-1) << " or a xml file";
    dialogFile.setTitle( sstrTitle.str() );
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    std::stringstream archExt;
    archExt << "*" << m_archiveExtenstion;
    dialogFile.addFilter("fwXML compressed archive", archExt.str() );
    dialogFile.addFilter("fwXML archive","*.xml");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath();
        m_reader.setFile( result->getPath() );
    }
}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::setArchiveExtension( const std::string & _archiveExtenstion )
{
    m_archiveExtenstion = _archiveExtenstion;
}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
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
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading data ");
        myLoader.addHandler( progressMeterGUI );
        myLoader.read();
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
        return pObject;
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading : ";
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
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

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

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
        cursor.setDefaultCursor();
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
    return ( ::boost::filesystem::extension(filePath) == m_archiveExtenstion );
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr FwXMLGenericReaderService::manageZipAndLoadData( const ::boost::filesystem::path _pArchivePath )
{
    ::fwTools::Object::sptr obj;
    // Unzip folder
    ::boost::filesystem::path destFolder = ::fwTools::System::getTemporaryFolder() / "fwxmlArchiveFolder";

    OSLM_DEBUG("srcZipFileName = " << _pArchivePath );
    OSLM_DEBUG("destFolderName = " << destFolder );

    ::fwZip::ZipFolder::unpackFolder( _pArchivePath, destFolder );

    // Load
    ::boost::filesystem::path xmlfile = destFolder / "root.xml";
    obj = loadData( xmlfile );

    // Remove temp folder
    ::boost::filesystem::remove_all( destFolder );

    return obj;
}

//------------------------------------------------------------------------------

::boost::filesystem::path FwXMLGenericReaderService::correctFileFormat( const ::boost::filesystem::path _filePath ) const
{
    ::boost::filesystem::path newPath = _filePath;
    if ( ::boost::filesystem::extension(_filePath) != m_archiveExtenstion && ::boost::filesystem::extension(_filePath) != ".xml" )
    {
        newPath = _filePath.string() + m_archiveExtenstion;
    }

    return newPath;
}

} // namespace ioXML
