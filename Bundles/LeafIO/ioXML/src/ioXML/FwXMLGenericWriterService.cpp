/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwTools/System.hpp>

#include <fwData/Acquisition.hpp>
#include <fwData/location/Folder.hpp>

#include <fwXML/writer/FwXMLObjectWriter.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwZip/ZipFolder.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>

#include "ioXML/FwXMLGenericWriterService.hpp"

namespace ioXML
{

REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLGenericWriterService , ::fwTools::Object ) ;

//------------------------------------------------------------------------------

FwXMLGenericWriterService::FwXMLGenericWriterService() throw()
{}

//------------------------------------------------------------------------------

FwXMLGenericWriterService::~FwXMLGenericWriterService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::configuring() throw(::fwTools::Failed)
{
    m_writer.setFile( ::boost::filesystem::path("SAVEDGRAPH.fxz") );
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::configureWithIHM()
{

    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle( "Choose a fxz or a xml file" );
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("fwXML archive","*.fxz");
    dialogFile.addFilter("fwXML archive","*.xml");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath();
        m_writer.setFile( result->getPath() );
    }
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLGenericWriterService::info" ;
}

//------------------------------------------------------------------------------

std::string FwXMLGenericWriterService::getCfgExtensionPoint()
{
    return "" ;
}

//------------------------------------------------------------------------------

std::string FwXMLGenericWriterService::getPersistanceId()
{
    return "ioITK::FwXMLGenericWriterService" ;
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::saveData( const ::boost::filesystem::path path, ::fwTools::Object::sptr _obj )
{
    SLM_TRACE_FUNC();
    ::fwXML::writer::FwXMLObjectWriter myWriter;

    myWriter.setObject(_obj);
    myWriter.setFile(path);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving data ");
        myWriter.addHandler( progressMeterGUI );
        myWriter.write();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::dialog::IMessageDialog::Icons icon = ::fwGui::dialog::IMessageDialog::WARNING;
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
        ss << "Warning during loading : ";
        ::fwGui::dialog::IMessageDialog::Icons icon = ::fwGui::dialog::IMessageDialog::WARNING;
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLGenericWriterService::updating()");

    if( !m_writer.getFile().empty() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Object::sptr obj = this->getObject< ::fwData::Object >();
        SLM_ASSERT("data is null", obj);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        m_writer.setFile( correctFileFormat( m_writer.getFile() ));
        if ( isAnFwxmlArchive( m_writer.getFile() ) )
        {
            manageZipAndSaveData( m_writer.getFile(), obj);
        }
        else
        {
            saveData(m_writer.getFile(), obj);
        }
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

::boost::filesystem::path FwXMLGenericWriterService::correctFileFormat( const ::boost::filesystem::path _filePath ) const
{
    ::boost::filesystem::path newPath = _filePath;
    if ( ::boost::filesystem::extension(_filePath) != ".fxz" && ::boost::filesystem::extension(_filePath) != ".xml" )
    {
        newPath = _filePath.string() + ".fxz";
    }

    return newPath;
}

//------------------------------------------------------------------------------

bool FwXMLGenericWriterService::isAnFwxmlArchive( const ::boost::filesystem::path filePath )
{
    return ( ::boost::filesystem::extension(filePath) == ".fxz" );
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::manageZipAndSaveData( const ::boost::filesystem::path path, ::fwTools::Object::sptr _obj )
{
    // Save in tmp folder
    ::boost::filesystem::path srcFolder = ::fwTools::System::getTemporaryFolder() / "fwxmlArchiveFolder";
    ::boost::filesystem::path xmlfile = srcFolder / "root.xml";
    ::boost::filesystem::create_directories( srcFolder );
    saveData(xmlfile,_obj);

    // Zip
    ::fwZip::ZipFolder::packFolder( srcFolder, path );

    // Remove temp folder
    ::boost::filesystem::remove_all( srcFolder );
}

//------------------------------------------------------------------------------

} // namespace ioXML
