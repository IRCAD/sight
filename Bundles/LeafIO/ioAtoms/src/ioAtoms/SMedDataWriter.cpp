/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>

#include <fwAtomsBoostIO/Writer.hpp>

#include <fwAtomConversion/convert.hpp>

#include <fwData/Composite.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/location/Folder.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <fwZip/WriteDirArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <fwDataCamp/visitor/RecursiveLock.hpp>

#include "ioAtoms/SMedDataWriter.hpp"

namespace ioAtoms
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::io::IWriter , ::ioAtoms::SMedDataWriter , ::fwData::Composite );

//-----------------------------------------------------------------------------

void SMedDataWriter::starting() throw(::fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void SMedDataWriter::stopping() throw(::fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void SMedDataWriter::updating() throw(::fwTools::Failed)
{
    if( this->hasLocationDefined() )
    {
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        try
        {
            const ::boost::filesystem::path& filePath = this->getFile();
            const ::boost::filesystem::path folderPath = filePath.parent_path();
            const ::boost::filesystem::path filename = filePath.filename();
            const std::string extension = ::boost::filesystem::extension(filePath);

            FW_RAISE_IF( "Extension is empty", extension.empty() );

            // Hack manage medical workspace
            ::fwData::Composite::sptr md = ::fwData::Composite::New();
            ::fwData::Composite::sptr processingdb = ::fwData::Composite::New();
            ::fwData::Composite::sptr planningdb = ::fwData::Composite::New();
            ::fwData::PatientDB::sptr pdb = ::fwData::PatientDB::New();
            md->getContainer()["processingDB"] = processingdb;
            md->getContainer()["planningDB"] = planningdb;
            md->getContainer()["patientDB"] = pdb;
            processingdb->shallowCopy( composite->getContainer()["processingDB"] );
            planningdb->shallowCopy( composite->getContainer()["planningDB"] );
            pdb->shallowCopy( composite->getContainer()["patientDB"] );

            // Mutex data lock
            ::fwDataCamp::visitor::RecursiveLock recursiveLock (md);

            // Convert data to atom
            ::fwAtoms::Base::sptr atom = ::fwAtomConversion::convert(md);

            // Write atom
            ::fwZip::IWriteArchive::sptr writeArchive;
            ::fwAtomsBoostIO::Writer::FormatType format;
            ::boost::filesystem::path archiveRootName;
            if ( extension == ".json" )
            {
                writeArchive = ::fwZip::WriteDirArchive::New(folderPath.string());
                archiveRootName = filename;
                format = ::fwAtomsBoostIO::Writer::JSON;
            }
            else if ( extension == ".jsonz" )
            {
                if ( ::boost::filesystem::exists( filePath ) )
                {
                    ::boost::filesystem::remove( filePath );
                }
                writeArchive = ::fwZip::WriteZipArchive::New(filePath.string());
                archiveRootName = "root.json";
                format = ::fwAtomsBoostIO::Writer::JSON;
            }
            else if ( extension == ".xml" )
            {
                writeArchive = ::fwZip::WriteDirArchive::New(folderPath.string());
                archiveRootName = filename;
                format = ::fwAtomsBoostIO::Writer::XML;
            }
            else if ( extension == ".xmlz" )
            {
                if ( ::boost::filesystem::exists( filePath ) )
                {
                    ::boost::filesystem::remove( filePath );
                }
                writeArchive = ::fwZip::WriteZipArchive::New(filePath.string());
                archiveRootName = "root.xml";
                format = ::fwAtomsBoostIO::Writer::XML;
            }
            else
            {
                FW_RAISE( "This file extension '" << extension << "' is not managed" );
            }

            writeArchive->setRootFilename( archiveRootName );
            ::fwAtomsBoostIO::Writer(atom).write( writeArchive, format );
            writeArchive.reset();

        }
        catch( std::exception & e )
        {
            OSLM_ERROR( e.what() );
            ::fwGui::dialog::MessageDialog::showMessageDialog("Medical data writer failed",
                    e.what(),
                    ::fwGui::dialog::MessageDialog::CRITICAL);
        }
        catch( ... )
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Medical data writer failed",
                    "Writing process abort",
                    ::fwGui::dialog::MessageDialog::CRITICAL);
        }
        cursor.setDefaultCursor();
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType SMedDataWriter::getIOPathType() const
{
    return ::io::FILE;
}

//-----------------------------------------------------------------------------

void SMedDataWriter::configureWithIHM()
{
   static ::boost::filesystem::path _sDefaultPath;

   ::fwGui::dialog::LocationDialog dialogFile;
   dialogFile.setTitle("Enter file name");
   dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
   dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
   dialogFile.setType(::fwGui::dialog::LocationDialog::SINGLE_FILE);

   dialogFile.addFilter( "JSON", "*.json");
   dialogFile.addFilter( "Zipped JSON", "*.jsonz");
   dialogFile.addFilter( "XML", "*.xml");
   dialogFile.addFilter( "Zipped XML", "*.xmlz");

   ::fwData::location::SingleFile::sptr result
       = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );

   if (result)
   {
       _sDefaultPath = result->getPath();
       this->setFile( _sDefaultPath );
       dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath.parent_path()) );
   }
   else
   {
       this->clearLocations();
   }
}

//-----------------------------------------------------------------------------

} // namespace ioAtoms

