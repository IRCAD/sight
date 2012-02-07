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

    typedef std::vector < SPTR(::fwRuntime::ConfigurationElement) >  ConfigurationElementContainer;
    ConfigurationElementContainer filename  = m_configuration->find("filename");
    ConfigurationElementContainer extension = m_configuration->find("archiveExtension");

    int elements = 0;

    if( filename.size() > 0 )
    {
        ++elements;
        ConfigurationElementContainer::iterator iter = filename.begin() ;
        SLM_ASSERT("The <"<< (*iter)->getName() <<"> element can be set at most once.", filename.size() == 1 );
        SLM_ASSERT("The <"<< (*iter)->getName() <<"> element value can not be empty.", !(*iter)->getValue().empty() );
        ::boost::filesystem::path filePath((*iter)->getValue());
        m_reader.setFile( filePath );
    }

    if( extension.size() > 0 )
    {
        ++elements;
        ConfigurationElementContainer::iterator iter = extension.begin() ;
        SLM_ASSERT("The <"<< (*iter)->getName() <<"> element can be set at most once.", extension.size() == 1 );
        SLM_ASSERT("The <"<< (*iter)->getName() <<"> element value can not be empty.", !(*iter)->getValue().empty() );
        m_archiveExtenstion =  (*iter)->getValue();
    }

    SLM_ASSERT("The configuration accepts at most one <archiveExtension> and/or one <filename> element.", elements <= 2 );
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
    extensions.push_back(".fxz");
    return extensions ;
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr FwXMLGenericReaderService::loadData( const ::boost::filesystem::path xmlFile )
{
    SLM_TRACE_FUNC();
    ::fwXML::reader::FwXMLObjectReader::NewSptr myLoader;
    ::fwTools::Object::sptr pObject;

    myLoader->setFile(xmlFile);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading data ");
        myLoader->addHandler( progressMeterGUI );
        myLoader->read();
        pObject = ::fwTools::Object::dynamicCast( myLoader->getObject() );
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

    if( !m_reader.getFile().empty() )
    {
        ::fwTools::Object::sptr obj; // object loaded

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        m_reader.setFile( this->correctFileFormat( m_reader.getFile() ));
        if ( this->isAnFwxmlArchive( m_reader.getFile() ) )
        {
            obj = this->manageZipAndLoadData( m_reader.getFile() );
        }
        else
        {
            obj = this->loadData(m_reader.getFile() );
        }

        if (obj)
        {
            // Retrieve dataStruct associated with this service
            ::fwTools::Object::sptr associatedObject = this->getObject< ::fwTools::Object >();
            SLM_ASSERT("associatedObject not instanced", associatedObject);

            if(obj->getClassname() != associatedObject->getClassname())
            {
                std::stringstream stream;
                stream << "Sorry, the file "<<m_reader.getFile()<< " contains a "
                        << obj->getRootedClassname() << ", and you need a "
                        << associatedObject->getRootedClassname();
                ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                            stream.str(),
                            ::fwGui::dialog::IMessageDialog::WARNING);
            }
            else
            {
                associatedObject->shallowCopy( obj );
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

::fwTools::Object::sptr FwXMLGenericReaderService::manageZipAndLoadData( const ::boost::filesystem::path _pArchivePath )
{
    ::fwTools::Object::sptr obj;
    // Unzip folder
    ::boost::filesystem::path destFolder = ::fwTools::System::getTemporaryFolder() / "fwxmlArchiveFolder";

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
        stream << "Sorry, "<<_pArchivePath<< " is not valid a valid fxz file."
               << this->getObject< ::fwTools::Object >()->getRootedClassname();
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
