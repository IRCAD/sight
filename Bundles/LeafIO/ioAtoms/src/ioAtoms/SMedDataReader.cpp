/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>

#include <fwAtomsBoostIO/Reader.hpp>

#include <fwAtomConversion/convert.hpp>

#include <fwTools/IBufferManager.hpp>

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

#include <fwMemory/BufferManager.hpp>
#include <fwMemory/policy/BarrierDump.hpp>
#include <fwMemory/policy/NeverDump.hpp>
#include <fwMemory/tools/MemoryMonitorTools.hpp>

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
        this->setBarrierDumpPolicy();

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

            ::fwAtomsBoostIO::Reader reader;
            ::fwAtoms::Object::sptr atom = ::fwAtoms::Object::dynamicCast( reader.read( readArchive, archiveRootName ) );
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

        this->resetDumpPolicy();
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType SMedDataReader::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void SMedDataReader::setBarrierDumpPolicy()
{
    ::fwMemory::BufferManager::sptr manager;
    manager = ::boost::dynamic_pointer_cast< ::fwMemory::BufferManager >( ::fwTools::IBufferManager::getCurrent() );

    if( manager )
    {
        ::fwMemory::IPolicy::sptr policy = manager->getDumpPolicy();
        if( ::boost::dynamic_pointer_cast< ::fwMemory::policy::NeverDump >( policy ) )
        {
            ::fwMemory::policy::BarrierDump::sptr newDumpPolicy = ::fwMemory::policy::BarrierDump::New();
            size_t aliveMemory = manager->getManagedBufferSize() - manager->getDumpedBufferSize();
            size_t freeMemory = ::fwMemory::tools::MemoryMonitorTools::estimateFreeMem() / 2;
            size_t barrier = std::max( aliveMemory, std::max( freeMemory, static_cast<size_t>(500L * 1024 * 1024) ) );

            newDumpPolicy->setBarrier( barrier );
            manager->setDumpPolicy( newDumpPolicy );
            m_oldPolicy = policy;
        }
    }
}

//------------------------------------------------------------------------------

void SMedDataReader::resetDumpPolicy()
{
    ::fwMemory::BufferManager::sptr manager;
    manager = ::boost::dynamic_pointer_cast< ::fwMemory::BufferManager >( ::fwTools::IBufferManager::getCurrent() );

    if( manager && m_oldPolicy )
    {
        manager->setDumpPolicy( m_oldPolicy );
        m_oldPolicy.reset();
    }
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

