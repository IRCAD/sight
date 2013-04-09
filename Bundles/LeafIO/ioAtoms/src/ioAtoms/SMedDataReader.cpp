/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>

#include <fwAtomsBoostIO/Reader.hpp>

#include <fwAtomConversion/convert.hpp>

#include <fwData/Composite.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/location/Folder.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwZip/ReadDirArchive.hpp>
#include <fwZip/ReadZipArchive.hpp>

#include "ioAtoms/SMedDataReader.hpp"

namespace ioAtoms
{

fwServicesRegisterMacro( ::io::IReader , ::ioAtoms::SMedDataReader , ::fwData::Composite );

//-----------------------------------------------------------------------------

void SMedDataReader::starting() throw(::fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void SMedDataReader::stopping() throw(::fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void SMedDataReader::updating() throw(::fwTools::Failed)
{

    if(this->hasLocationDefined())
    {
        ::fwData::Composite::sptr data = this->getObject< ::fwData::Composite >();

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        try
        {
            const ::boost::filesystem::path& filePath = this->getFile();
            const ::boost::filesystem::path folderPath = filePath.parent_path();
            const ::boost::filesystem::path filename = filePath.filename();
            const std::string extension = ::boost::filesystem::extension(filePath);

            FW_RAISE_IF( "Extension is empty", extension.empty() );

            // Read atom
            ::fwZip::IReadArchive::sptr readArchive;
            ::boost::filesystem::path archiveRootName;
            if ( extension == ".json" )
            {
                readArchive = ::fwZip::ReadDirArchive::New(folderPath.string());
                archiveRootName = filename;
            }
            else if ( extension == ".jsonz" )
            {
                readArchive = ::fwZip::ReadZipArchive::New(filePath.string());
                archiveRootName = "root.json";
            }
            else if ( extension == ".xml" )
            {
                readArchive = ::fwZip::ReadDirArchive::New(folderPath.string());
                archiveRootName = filename;
            }
            else if ( extension == ".xmlz" )
            {
                readArchive = ::fwZip::ReadZipArchive::New(filePath.string());
                archiveRootName = "root.xml";
            }
            else
            {
                FW_RAISE( "This file extension '" << extension << "' is not managed" );
            }

            readArchive->setRootFilename( archiveRootName );
            ::fwAtomsBoostIO::Reader reader;
            ::fwAtoms::Object::sptr atom = ::fwAtoms::Object::dynamicCast( reader.read( readArchive ) );
            readArchive.reset();
            FW_RAISE_IF( "Data not correspond to an fw4spl medical data", ! atom );

            ::fwData::Composite::sptr newData = ::fwData::Composite::dynamicCast( ::fwAtomConversion::convert(atom) );
            FW_RAISE_IF( "Data not correspond to an fw4spl medical data", ! newData );

            data->shallowCopy(newData);

            this->notificationOfUpdate();
        }
        catch( std::exception & e )
        {
            OSLM_ERROR( e.what() );
            ::fwGui::dialog::MessageDialog::showMessageDialog("Medical data reader failed",
                    e.what(),
                    ::fwGui::dialog::MessageDialog::CRITICAL);
        }
        catch( ... )
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Medical data reader failed",
                    "Writing process abort",
                    ::fwGui::dialog::MessageDialog::CRITICAL);
        }

        cursor.setDefaultCursor();
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType SMedDataReader::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void SMedDataReader::notificationOfUpdate()
{
    ::fwData::Object::sptr object = this->getObject();
    ::fwServices::ObjectMsg::NewSptr msg;
    msg->addEvent( ::fwServices::ObjectMsg::UPDATED_OBJECT , object );
    ::fwServices::IEditionService::notify( this->getSptr(),  object, msg );
}

//-----------------------------------------------------------------------------

void SMedDataReader::configureWithIHM()
{
   static ::boost::filesystem::path _sDefaultPath;

   ::fwGui::dialog::LocationDialog dialogFile;
   dialogFile.setTitle("Enter file name");
   dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
   dialogFile.setType(::fwGui::dialog::ILocationDialog::SINGLE_FILE);
   dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
   dialogFile.setOption(::fwGui::dialog::LocationDialog::FILE_MUST_EXIST);

   dialogFile.addFilter( "Medical data", "*.json *.jsonz *.xml *.xmlz" );
   dialogFile.addFilter( "JSON", "*.json" );
   dialogFile.addFilter( "Zipped JSON", "*.jsonz" );
   dialogFile.addFilter( "XML", "*.xml" );
   dialogFile.addFilter( "Zipped XML", "*.xmlz" );

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

