/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwTools/System.hpp>

#include <fwData/Acquisition.hpp>
#include <fwData/location/Folder.hpp>

#include <fwXML/writer/FwXMLObjectWriter.hpp>
#include <fwXML/writer/fwxmlextension.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwZip/ZipFolder.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include "ioXML/FwXMLAcquisitionWriterService.hpp"

namespace ioXML
{

REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLAcquisitionWriterService , ::fwData::Acquisition ) ;

//------------------------------------------------------------------------------

FwXMLAcquisitionWriterService::FwXMLAcquisitionWriterService() throw()
{}

//------------------------------------------------------------------------------

FwXMLAcquisitionWriterService::~FwXMLAcquisitionWriterService() throw()
{}

//------------------------------------------------------------------------------

::io::IOPathType FwXMLAcquisitionWriterService::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a " FWXML_ARCHIVE_EXTENSION " or a xml file");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("fwXML archive","*." FWXML_ARCHIVE_EXTENSION);
    dialogFile.addFilter("fwXML archive","*.xml");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

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

void FwXMLAcquisitionWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLAcquisitionWriterService::info" ;
}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::saveAcquisition( const ::boost::filesystem::path inrFileDir, ::fwData::Acquisition::sptr _pAcquisition )
{
    SLM_TRACE_FUNC();
    ::fwXML::writer::FwXMLObjectWriter myWriter;

    myWriter.setObject(_pAcquisition);
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
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                "Warning during loading",
                ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Acquisition::sptr acquisition = this->getObject< ::fwData::Acquisition >();
        SLM_ASSERT("Acquisition is null", acquisition);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        ::boost::filesystem::path path = correctFileFormat( this->getFile() );
        if ( isAnFwxmlArchive( path ) )
        {
            manageZipAndSaveAcquisition(path, acquisition);
        }
        else
        {
            saveAcquisition(path, acquisition);
        }
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

::boost::filesystem::path FwXMLAcquisitionWriterService::correctFileFormat( const ::boost::filesystem::path _filePath ) const
{
    ::boost::filesystem::path newPath = _filePath;
    if ( ::boost::filesystem::extension(_filePath) != ("." FWXML_ARCHIVE_EXTENSION) && ::boost::filesystem::extension(_filePath) != ".xml" )
    {
        newPath = _filePath.string() + "." FWXML_ARCHIVE_EXTENSION;
    }

    return newPath;
}

//------------------------------------------------------------------------------

bool FwXMLAcquisitionWriterService::isAnFwxmlArchive( const ::boost::filesystem::path filePath )
{
    return ( ::boost::filesystem::extension(filePath) == "." FWXML_ARCHIVE_EXTENSION );
}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::manageZipAndSaveAcquisition( const ::boost::filesystem::path inrFileDir, ::fwData::Acquisition::sptr _pAcquisition )
{
    // Save in tmp folder
    ::boost::filesystem::path srcFolder = ::fwTools::System::getTemporaryFolder() / "fwxmlArchiveFolder";
    ::boost::filesystem::path xmlfile = srcFolder / "patient.xml";
    ::boost::filesystem::create_directories( srcFolder );
    saveAcquisition(xmlfile,_pAcquisition);

    // Zip
    ::fwZip::ZipFolder::NewSptr zip;
    ::fwGui::dialog::ProgressDialog progress("Saving acquisition");
    zip->addHandler( progress );
    zip->packFolder( srcFolder, inrFileDir );

    // Remove temp folder
    ::boost::filesystem::remove_all( srcFolder );
}

//------------------------------------------------------------------------------

} // namespace ioXML
