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
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwServices/ObjectMsg.hpp>

#include <io/IReader.hpp>

#include <fwTools/System.hpp>

#include <ioXML/FwXMLGenericReaderService.hpp>
#include <fwXML/writer/fwxmlextension.hpp>

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
        : m_archiveExtenstion ("." FWXML_ARCHIVE_EXTENSION)
{}

//------------------------------------------------------------------------------

FwXMLGenericReaderService::~FwXMLGenericReaderService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::io::IReader::configuring();


    typedef std::vector < SPTR(::fwRuntime::ConfigurationElement) >  ConfigurationElementContainer;
    ConfigurationElementContainer extension = m_configuration->find("archiveExtension");

    SLM_ASSERT("The configuration accepts at most one <archiveExtension> and/or one <filename> element.", extension.size() <= 1 );

    if( extension.size() > 0 )
    {
        ConfigurationElementContainer::iterator iter = extension.begin() ;
        SLM_ASSERT("The <"<< (*iter)->getName() <<"> element can be set at most once.", extension.size() == 1 );
        SLM_ASSERT("The <"<< (*iter)->getName() <<"> element value can not be empty.", !(*iter)->getValue().empty() );
        m_archiveExtenstion =  (*iter)->getValue();
    }

}

//------------------------------------------------------------------------------

::io::IOPathType FwXMLGenericReaderService::getIOPathType() const
{
    return ::io::FILE;
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
    extensions.push_back("." FWXML_ARCHIVE_EXTENSION);
    return extensions ;
}

//------------------------------------------------------------------------------

::fwData::Object::sptr FwXMLGenericReaderService::loadData( const ::boost::filesystem::path xmlFile )
{
    SLM_TRACE_FUNC();
    ::fwXML::reader::FwXMLObjectReader::NewSptr myLoader;
    ::fwData::Object::sptr pObject;

    myLoader->setFile(xmlFile);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading data ");
        myLoader->addHandler( progressMeterGUI );
        myLoader->read();
        pObject = ::fwData::Object::dynamicCast( myLoader->getObject() );
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
                "Warning during loading",
                ::fwGui::dialog::IMessageDialog::WARNING);
    }
    return pObject;
}

//------------------------------------------------------------------------------

void FwXMLGenericReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();


    if( this->hasLocationDefined() )
    {

        m_reader.setFile( this->getFile() );

        ::fwData::Object::sptr obj; // object loaded

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        m_reader.setFile( this->correctFileFormat( m_reader.getFile() ));
        if ( this->isAnFwxmlArchive( m_reader.getFile() ) )
        {
            obj = this->manageZipAndLoadData( m_reader.getFile() );
        }
        else
        {
            obj = this->loadData(this->getFile() );
        }

        ::fwData::Object::sptr dataObj; // object loaded
        if (dataObj)
        {
            // Retrieve dataStruct associated with this service
            ::fwData::Object::sptr associatedObject = this->getObject< ::fwData::Object >();
            SLM_ASSERT("associatedObject not instanced", associatedObject);

            if(dataObj->getClassname() != associatedObject->getClassname())
            {
                std::stringstream stream;
                stream << "Sorry, the file "<<m_reader.getFile()<< " contains a "
                        << dataObj->getRootedClassname() << ", and you need a "
                        << associatedObject->getRootedClassname();
                ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                            stream.str(),
                            ::fwGui::dialog::IMessageDialog::WARNING);
            }
            else
            {
                associatedObject->shallowCopy( dataObj );
                notificationOfUpdate();
            }
        }
        cursor.setDefaultCursor();
    }
}


//------------------------------------------------------------------------------

void FwXMLGenericReaderService::notificationOfUpdate()
{
    SLM_TRACE_FUNC();
    ::fwData::Object::sptr object = this->getObject< ::fwData::Object >();
    SLM_ASSERT("object not instanced", object);
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

::fwData::Object::sptr FwXMLGenericReaderService::manageZipAndLoadData( const ::boost::filesystem::path _pArchivePath )
{
    ::fwData::Object::sptr obj;
    // Unzip folder
    ::boost::filesystem::path destFolder = ::fwTools::System::getTemporaryFolder() / "fwxmlArchiveFolder";


    FW_RAISE_IF( "<" << _pArchivePath << "> does not exist.", !::boost::filesystem::exists(_pArchivePath));
    OSLM_DEBUG("srcZipFileName = " << _pArchivePath );
    OSLM_DEBUG("destFolderName = " << destFolder );
    ::fwZip::ZipFolder::NewSptr zip;
    ::fwGui::dialog::ProgressDialog progress("Reading");
    zip->addHandler( progress );
    zip->unpackFolder( _pArchivePath, destFolder );

    // Load
    ::boost::filesystem::path xmlFile;
    if(::boost::filesystem::exists(destFolder/"root.xml"))
    {
        xmlFile = destFolder / "root.xml";
        obj = this->loadData( xmlFile );
    }
    else if(::boost::filesystem::exists(destFolder/"patient.xml"))
    {
        xmlFile = destFolder / "patient.xml";
        obj = this->loadData( xmlFile );
    }
    else
    {
        std::stringstream stream;
        stream << "Sorry, "<<_pArchivePath<< " is not valid a valid " FWXML_ARCHIVE_EXTENSION " file."
               << this->getObject< ::fwData::Object >()->getRootedClassname();
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                        stream.str(),
                        ::fwGui::dialog::IMessageDialog::WARNING);
    }
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
