/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioAtoms/SReader.hpp"

#include <fwAtomConversion/convert.hpp>

#include <fwAtomsBoostIO/Reader.hpp>
#include <fwAtomsBoostIO/types.hpp>

#include <fwAtomsFilter/factory/new.hpp>
#include <fwAtomsFilter/IFilter.hpp>

#include <fwAtomsPatch/PatchingManager.hpp>

#include <fwCom/Signal.hxx>

#include <fwData/Array.hpp>
#include <fwData/Composite.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/Job.hpp>

#include <fwServices/macros.hpp>

#include <fwZip/ReadDirArchive.hpp>
#include <fwZip/ReadZipArchive.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem/path.hpp>

namespace ioAtoms
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioAtoms::SReader, ::fwData::Object );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

const SReader::FileExtension2NameType SReader::s_EXTENSIONS
    = ::boost::assign::map_list_of(".xml", "XML")
          (".xmlz", "Zipped XML")
          (".json", "JSON")
          (".jsonz", "Zipped JSON");

//-----------------------------------------------------------------------------

SReader::SReader() :
    m_useAtomsPatcher(false),
    m_context("Undefined"),
    m_version("Undefined"),
    m_filter("")
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );

    for(SReader::FileExtension2NameType::value_type ext :  s_EXTENSIONS)
    {
        m_allowedExts.insert(m_allowedExts.end(), ext.first);
    }
}

//-----------------------------------------------------------------------------

void SReader::starting()
{
}

//-----------------------------------------------------------------------------

void SReader::stopping()
{
}

//-----------------------------------------------------------------------------

void SReader::configuring()
{
    SLM_TRACE_FUNC();

    ::fwIO::IReader::configuring();

    typedef SPTR (::fwRuntime::ConfigurationElement) ConfigurationElement;
    typedef std::vector < ConfigurationElement >    ConfigurationElementContainer;

    m_customExts.clear();
    m_allowedExtLabels.clear();

    ConfigurationElementContainer customExtsList = m_configuration->find("archive");
    for(ConfigurationElement archive :  customExtsList)
    {
        const std::string& backend = archive->getAttributeValue("backend");
        SLM_ASSERT("No backend attribute given in archive tag", backend != "");
        SLM_ASSERT("Unsupported backend '" + backend + "'", s_EXTENSIONS.find("." + backend) != s_EXTENSIONS.end());

        ConfigurationElementContainer exts = archive->find("extension");
        for(ConfigurationElement ext :  exts)
        {
            const std::string& extension = ext->getValue();
            SLM_ASSERT("No extension given for backend '" + backend + "'", !extension.empty());
            SLM_ASSERT("Extension must begin with '.'", extension[0] == '.');

            m_customExts[extension]       = backend;
            m_allowedExtLabels[extension] = ext->getAttributeValue("label");
        }
    }

    ConfigurationElementContainer extensionsList = m_configuration->find("extensions");
    SLM_ASSERT("The <extensions> element can be set at most once.", extensionsList.size() <= 1);

    if(extensionsList.size() == 1)
    {
        m_allowedExts.clear();

        ConfigurationElementContainer extensions = extensionsList.at(0)->find("extension");
        for(ConfigurationElement extension :  extensions)
        {
            const std::string& ext = extension->getValue();

            // The extension must be found either in custom extensions list or in known extensions
            FileExtension2NameType::const_iterator itKnown  = s_EXTENSIONS.find(ext);
            FileExtension2NameType::const_iterator itCustom = m_customExts.find(ext);

            const bool extIsKnown = (itKnown != SReader::s_EXTENSIONS.end() || itCustom != m_customExts.end());
            SLM_ASSERT("Extension '" + ext + "' is not allowed in configuration", extIsKnown);

            if(extIsKnown)
            {
                m_allowedExts.insert(m_allowedExts.end(), ext);
                m_allowedExtLabels[ext] = extension->getAttributeValue("label");
            }
        }
    }
    else
    {
        m_allowedExts.clear();

        for(FileExtension2NameType::value_type ext :  m_customExts)
        {
            m_allowedExts.insert(m_allowedExts.end(), ext.first);
        }

        for(SReader::FileExtension2NameType::value_type ext :  SReader::s_EXTENSIONS)
        {
            m_allowedExts.insert(m_allowedExts.end(), ext.first);
            m_allowedExtLabels[ext.first] = ext.second;
        }
    }

    ConfigurationElementContainer inject = m_configuration->find("inject");
    SLM_ASSERT("The <inject> element can be set at most once.", inject.size() <= 1);
    if (inject.size() == 1)
    {
        m_inject = inject.at(0)->getValue();
    }

    ConfigurationElementContainer filter = m_configuration->find("filter");
    SLM_ASSERT("The <filter> element can be set at most once.", filter.size() <= 1);
    if (filter.size() == 1)
    {
        m_filter = filter.at(0)->getValue();
    }

    ConfigurationElementContainer uuidPolicy = m_configuration->find("uuidPolicy");
    SLM_ASSERT("The <uuidPolicy> element can be set at most once.", uuidPolicy.size() <= 1);
    if (uuidPolicy.size() == 1)
    {
        m_uuidPolicy = uuidPolicy.at(0)->getValue();
        SLM_ASSERT("Unknown policy : '"
                   + m_uuidPolicy +
                   "', available policies : 'Strict','Change' or 'Reuse'.",
                   "Strict" == m_uuidPolicy || "Change" == m_uuidPolicy || "Reuse" == m_uuidPolicy );

        SLM_ASSERT("'Reuse' policy is available only with inject mode",
                   ("Reuse" == m_uuidPolicy && !m_inject.empty()) || "Reuse" != m_uuidPolicy
                   );
    }

    ConfigurationElementContainer patcher = m_configuration->find("patcher");
    SLM_ASSERT("The <patcher> element can be set at most once.", patcher.size() <= 1 );
    if (patcher.size() == 1)
    {
        m_context         = patcher.at(0)->getExistingAttributeValue("context");
        m_version         = patcher.at(0)->getExistingAttributeValue("version");
        m_useAtomsPatcher = true;
    }

}

//-----------------------------------------------------------------------------

void SReader::updating()
{
    if(this->hasLocationDefined())
    {
        ::fwData::Object::sptr data = this->getInOut< ::fwData::Object >(::fwIO::s_DATA_KEY);
        if (!data)
        {
            FW_DEPRECATED_KEY(::fwIO::s_DATA_KEY, "inout", "fw4spl_18.0");
            data = this->getObject< ::fwData::Object >();
        }

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        try
        {
            const ::boost::filesystem::path& filePath  = this->getFile();
            const ::boost::filesystem::path folderPath = filePath.parent_path();
            const ::boost::filesystem::path filename   = filePath.filename();
            std::string extension                      = ::boost::filesystem::extension(filePath);

            FW_RAISE_IF( "Unable to guess file format (missing extension)", extension.empty() );

            if(m_customExts.find(extension) != m_customExts.end())
            {
                extension = "." + m_customExts[extension];
            }

            ::fwAtoms::Object::sptr atom;

            const unsigned int progressBarOffset = 10;

            // Reading file : job 1
            ::fwJobs::Job::sptr fileReadingJob = ::fwJobs::Job::New("Reading " + extension + " file",
                                                                    [ =, &atom](::fwJobs::Job& runningJob)
                {
                    runningJob.doneWork(progressBarOffset);

                    // Read atom
                    ::fwZip::IReadArchive::sptr readArchive;
                    ::boost::filesystem::path archiveRootName;
                    ::fwAtomsBoostIO::FormatType format = ::fwAtomsBoostIO::UNSPECIFIED;

                    if ( extension == ".json" )
                    {
                        readArchive     = ::fwZip::ReadDirArchive::New(folderPath.string());
                        archiveRootName = filename;
                        format          = ::fwAtomsBoostIO::JSON;
                    }
                    else if ( extension == ".jsonz" )
                    {
                        readArchive     = ::fwZip::ReadZipArchive::New(filePath.string());
                        archiveRootName = "root.json";
                        format          = ::fwAtomsBoostIO::JSON;
                    }
                    else if ( extension == ".xml" )
                    {
                        readArchive     = ::fwZip::ReadDirArchive::New(folderPath.string());
                        archiveRootName = filename;
                        format          = ::fwAtomsBoostIO::XML;
                    }
                    else if ( extension == ".xmlz" )
                    {
                        readArchive     = ::fwZip::ReadZipArchive::New(filePath.string());
                        archiveRootName = "root.xml";
                        format          = ::fwAtomsBoostIO::XML;
                    }
                    else
                    {
                        FW_RAISE( "This file extension '" << extension << "' is not managed" );
                    }

                    ::fwAtomsBoostIO::Reader reader;
                    atom = ::fwAtoms::Object::dynamicCast( reader.read( readArchive, archiveRootName, format ) );

                    FW_RAISE_IF( "Invalid atoms file :'" << filePath << "'", !atom );

                    runningJob.doneWork(progressBarOffset);

                    runningJob.done();

                }, m_associatedWorker);

            // patching atom : job 2
            ::fwJobs::Job::sptr patchingJob = ::fwJobs::Job::New("Reading " + extension + " file",
                                                                 [ =, &atom](::fwJobs::Job& runningJob)
                {
                    if(runningJob.cancelRequested())
                    {
                        return;
                    }

                    runningJob.doneWork(progressBarOffset);

                    /// patch atom
                    if ( m_useAtomsPatcher )
                    {
                        FW_RAISE_IF( "Unable to load data, found '" << atom->getMetaInfo("context")
                                                                    << "' context, but '" << m_context <<
                                     "' was excepted.",
                                     atom->getMetaInfo("context") != m_context);

                        ::fwAtomsPatch::PatchingManager globalPatcher(atom);
                        atom = globalPatcher.transformTo( m_version );
                    }

                    if(!m_filter.empty())
                    {
                        ::fwAtomsFilter::IFilter::sptr filter = ::fwAtomsFilter::factory::New(m_filter);
                        OSLM_ASSERT("Failed to create IFilter implementation '" << m_filter << "'", filter);
                        filter->apply(atom);
                    }
                    runningJob.done();
                }, m_associatedWorker);

            ::fwData::Object::sptr newData;

            // convert to fwData : job 3
            ::fwJobs::Job::sptr atomToDataJob = ::fwJobs::Job::New("Reading " + extension + " file",
                                                                   [ =, &newData, &atom](::fwJobs::Job& runningJob)
                {
                    runningJob.doneWork(progressBarOffset);
                    if(runningJob.cancelRequested())
                    {
                        return;
                    }
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

                    runningJob.done();
                }, m_associatedWorker);

            ::fwJobs::Aggregator::sptr jobs = ::fwJobs::Aggregator::New(extension + " reader");
            jobs->add(fileReadingJob);
            jobs->add(patchingJob);
            jobs->add(atomToDataJob);

            m_sigJobCreated->emit(jobs);

            jobs->run().get();

            if(jobs->getState() == ::fwJobs::IJob::CANCELED)
            {
                return;
            }

            FW_RAISE_IF( "Unable to load '" << filePath << "' : invalid data.", !newData );

            if(m_inject.empty())
            {
                FW_RAISE_IF( "Unable to load '" << filePath
                                                << "' : trying to load a '" << newData->getClassname()
                                                << "' where a '" << data->getClassname() << "' was expected",
                             newData->getClassname() != data->getClassname() );

                // Workaround to read a fwData::Array.
                // The shallowCopy of a fwData::Array is not allowed due to unknown buffer owner.
                // So in the case of reading an Array we swap buffers.
                if(newData->getClassname() == ::fwData::Array::classname())
                {
                    ::fwData::Array::dynamicCast(data)->swap( ::fwData::Array::dynamicCast(newData) );
                }
                else
                {
                    data->shallowCopy(newData);
                }

            }
            else
            {
                ::fwData::Composite::sptr composite = ::fwData::Composite::dynamicCast(data);
                SLM_ASSERT("Inject mode works only on a Composite object", composite );

                ::fwDataTools::helper::Composite helper(composite);
                helper.add(m_inject, newData);
                helper.notify();
            }

            this->notificationOfUpdate();
        }
        catch( std::exception& e )
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

    }
}

//-----------------------------------------------------------------------------

::fwIO::IOPathType SReader::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SReader::notificationOfUpdate()
{
    ::fwData::Object::sptr object = this->getInOut< ::fwData::Object >(::fwIO::s_DATA_KEY);
    if (!object)
    {
        FW_DEPRECATED_KEY(::fwIO::s_DATA_KEY, "inout", "fw4spl_18.0");
        object = this->getObject< ::fwData::Object >();
    }
    auto sig = object->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SReader::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setType(::fwGui::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::LocationDialog::FILE_MUST_EXIST);

    dialogFile.addFilter("Medical data", "*" + ::boost::algorithm::join(m_allowedExts, " *"));

    for(const std::string& ext :  m_allowedExts)
    {
        dialogFile.addFilter(m_allowedExtLabels[ext], "*" + ext);
    }

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
