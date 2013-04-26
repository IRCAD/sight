/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>

#include <fwAtomsBoostIO/Reader.hpp>

#include <fwAtomsHdf5IO/Reader.hpp>

#include <fwAtomConversion/convert.hpp>

#include <fwTools/IBufferManager.hpp>

#include <fwData/Object.hpp>
#include <fwData/Composite.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/location/Folder.hpp>

#include <fwComEd/helper/Composite.hpp>

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

#include "ioAtoms/SReader.hpp"

namespace ioAtoms
{

fwServicesRegisterMacro( ::io::IReader , ::ioAtoms::SReader , ::fwData::Object );

//-----------------------------------------------------------------------------

void SReader::starting() throw(::fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void SReader::stopping() throw(::fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void SReader::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::io::IReader::configuring();

    typedef std::vector < SPTR(::fwRuntime::ConfigurationElement) >  ConfigurationElementContainer;

    ConfigurationElementContainer inject = m_configuration->find("inject");
    SLM_ASSERT("The <inject> element can be set at most once.", inject.size() <= 1 );
    if (inject.size() == 1)
    {
        m_inject = inject.at(0)->getValue();
    }

    ConfigurationElementContainer uuidPolicy = m_configuration->find("uuidPolicy");
    SLM_ASSERT("The <uuidPolicy> element can be set at most once.", uuidPolicy.size() <= 1 );
    if (uuidPolicy.size() == 1)
    {
        m_uuidPolicy = uuidPolicy.at(0)->getValue();
        SLM_ASSERT("Unknown policy : '"
                   << m_uuidPolicy
                   <<"', available policies : 'Strict','Change' or 'Reuse'.",
                   "Strict" == m_uuidPolicy || "Change" == m_uuidPolicy || "Reuse" == m_uuidPolicy );

        SLM_ASSERT("'Reuse' policy is available only with inject mode",
                   ("Reuse" == m_uuidPolicy && !m_inject.empty()) || "Reuse" != m_uuidPolicy
                  );
    }

}

//-----------------------------------------------------------------------------

void SReader::updating() throw(::fwTools::Failed)
{

    if(this->hasLocationDefined())
    {
        this->setBarrierDumpPolicy();

        ::fwData::Object::sptr data = this->getObject< ::fwData::Object >();

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        try
        {
            const ::boost::filesystem::path& filePath = this->getFile();
            const ::boost::filesystem::path folderPath = filePath.parent_path();
            const ::boost::filesystem::path filename = filePath.filename();
            const std::string extension = ::boost::filesystem::extension(filePath);

            FW_RAISE_IF( "Unable to guess file format (missing extension)", extension.empty() );

            ::fwAtoms::Object::sptr atom;
            if ( extension == ".hdf5" )
            {
                ::fwAtomsHdf5IO::Reader reader;
                atom = ::fwAtoms::Object::dynamicCast( reader.read( filePath ) );
            }
            else
            {
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
                atom = ::fwAtoms::Object::dynamicCast( reader.read( readArchive, archiveRootName ) );
            }

            FW_RAISE_IF( "Invalid atoms file :'" << filePath << "'", ! atom );

            ::fwData::Object::sptr newData ;


            if("Strict" == m_uuidPolicy)
            {
                newData = ::fwAtomConversion::convert(atom, ::fwAtomConversion::AtomVisitor::StrictPolicy());
            }
            else if("Reuse" == m_uuidPolicy)
            {
                newData = ::fwAtomConversion::convert(atom, ::fwAtomConversion::AtomVisitor::ReusePolicy());
            }
            else
            {
                newData = ::fwAtomConversion::convert(atom, ::fwAtomConversion::AtomVisitor::ChangePolicy());
            }

            FW_RAISE_IF( "Unable to load '" << filePath << "' : invalid data.", ! newData );

            if(m_inject.empty())
            {
                FW_RAISE_IF( "Unable to load '" << filePath
                        << "' : trying to load a '" << newData->getClassname()
                        << "' where a '" << data->getClassname() << "' was expected",
                        newData->getClassname() != data->getClassname() );

                data->shallowCopy(newData);
            }
            else
            {
                ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast(data);
                SLM_ASSERT("Inject mode works only on a Composite object", composite );

                ::fwComEd::helper::Composite helper(composite);
                helper.add(m_inject, newData);
                helper.notify(this->getSptr());
            }

            this->notificationOfUpdate();
        }
        catch( std::exception & e )
        {
            OSLM_ERROR( e.what() );
            ::fwGui::dialog::MessageDialog::showMessageDialog("Atoms reader failed", e.what(),
                    ::fwGui::dialog::MessageDialog::CRITICAL);
        }
        catch( ... )
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Atoms reader failed", "Aborting operation.",
                    ::fwGui::dialog::MessageDialog::CRITICAL);
        }

        cursor.setDefaultCursor();

        this->resetDumpPolicy();
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType SReader::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void SReader::setBarrierDumpPolicy()
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

void SReader::resetDumpPolicy()
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

void SReader::notificationOfUpdate()
{
    ::fwData::Object::sptr object = this->getObject();
    ::fwServices::ObjectMsg::sptr msg = ::fwServices::ObjectMsg::New();
    msg->addEvent( ::fwServices::ObjectMsg::UPDATED_OBJECT , object );
    ::fwServices::IEditionService::notify( this->getSptr(),  object, msg );
}

//-----------------------------------------------------------------------------

void SReader::configureWithIHM()
{
   static ::boost::filesystem::path _sDefaultPath;

   ::fwGui::dialog::LocationDialog dialogFile;
   dialogFile.setTitle("Enter file name");
   dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
   dialogFile.setType(::fwGui::dialog::ILocationDialog::SINGLE_FILE);
   dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
   dialogFile.setOption(::fwGui::dialog::LocationDialog::FILE_MUST_EXIST);

   dialogFile.addFilter( "Medical data", "*.json *.jsonz *.xml *.xmlz *.hdf5" );
   dialogFile.addFilter( "JSON", "*.json" );
   dialogFile.addFilter( "Zipped JSON", "*.jsonz" );
   dialogFile.addFilter( "XML", "*.xml" );
   dialogFile.addFilter( "Zipped XML", "*.xmlz" );
   dialogFile.addFilter( "HDF5", "*.hdf5" );

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

