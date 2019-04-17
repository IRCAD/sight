/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

#include <fwMDSemanticPatch/PatchLoader.hpp>

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
    m_outputMode(false),
    m_uuidPolicy("Change"),
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
    if (m_outputMode)
    {
        this->setOutput(::fwIO::s_DATA_KEY, nullptr);
    }
}

//-----------------------------------------------------------------------------

void SReader::configuring()
{
    ::fwIO::IReader::configuring();

    m_customExts.clear();
    m_allowedExtLabels.clear();

    const ConfigType config = this->getConfigTree();

    const auto archiveCfgs = config.equal_range("archive");

    for (auto it = archiveCfgs.first; it != archiveCfgs.second; ++it)
    {
        const std::string backend = it->second.get<std::string>("<xmlattr>.backend");
        SLM_ASSERT("No backend attribute given in archive tag", backend != "");
        SLM_ASSERT("Unsupported backend '" + backend + "'", s_EXTENSIONS.find("." + backend) != s_EXTENSIONS.end());

        const auto extCfgs = it->second.equal_range("extension");

        for (auto itExt = extCfgs.first; itExt != extCfgs.second; ++itExt)
        {
            const std::string extension = itExt->second.get<std::string>("");
            SLM_ASSERT("No extension given for backend '" + backend + "'", !extension.empty());
            SLM_ASSERT("Extension must begin with '.'", extension[0] == '.');

            m_customExts[extension]       = backend;
            m_allowedExtLabels[extension] = itExt->second.get("<xmlattr>.label", "");
        }
    }

    const auto extensionsCfg = config.get_child_optional("extensions");

    if (extensionsCfg)
    {
        m_allowedExts.clear();

        const auto extCfgs = extensionsCfg->equal_range("extension");
        for (auto it = extCfgs.first; it != extCfgs.second; ++it)
        {
            const std::string ext = it->second.get<std::string>("");

            // The extension must be found either in custom extensions list or in known extensions
            FileExtension2NameType::const_iterator itKnown  = s_EXTENSIONS.find(ext);
            FileExtension2NameType::const_iterator itCustom = m_customExts.find(ext);

            const bool extIsKnown = (itKnown != SReader::s_EXTENSIONS.end() || itCustom != m_customExts.end());
            SLM_ASSERT("Extension '" + ext + "' is not allowed in configuration", extIsKnown);

            if(extIsKnown)
            {
                m_allowedExts.insert(m_allowedExts.end(), ext);
                m_allowedExtLabels[ext] = it->second.get("<xmlattr>.label", "");
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

    m_filter     = config.get("filter", "");
    m_uuidPolicy = config.get("uuidPolicy", m_uuidPolicy);

    SLM_ASSERT("Unknown policy : '"
               + m_uuidPolicy +
               "', available policies : 'Strict','Change' or 'Reuse'.",
               "Strict" == m_uuidPolicy || "Change" == m_uuidPolicy || "Reuse" == m_uuidPolicy );

    const auto patcherCfg = config.get_child_optional("patcher");

    if (patcherCfg)
    {
        m_context = patcherCfg->get<std::string>("<xmlattr>.context", "MedicalData");
        m_version = patcherCfg->get<std::string>("<xmlattr>.version",
                                                 ::fwMDSemanticPatch::PatchLoader::getCurrentVersion());
        m_useAtomsPatcher = true;
    }

    const std::string output = config.get<std::string>("out.<xmlattr>.key", "");
    if (output == ::fwIO::s_DATA_KEY )
    {
        m_outputMode = true;
    }

    SLM_ASSERT("'Reuse' policy is only available when data is set as 'out'", m_outputMode || "Reuse" != m_uuidPolicy);
}

//-----------------------------------------------------------------------------

void SReader::updating()
{
    if(this->hasLocationDefined())
    {
        ::fwData::Object::sptr data = this->getInOut< ::fwData::Object >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", m_outputMode || data);

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
                        m_readFailed = true;
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
                m_readFailed = true;
                return;
            }

            FW_RAISE_IF( "Unable to load '" << filePath << "' : invalid data.", !newData );

            if (m_outputMode)
            {
                this->setOutput(::fwIO::s_DATA_KEY, newData);
            }
            else
            {
                if(!data)
                {
                    m_readFailed = true;
                }
                SLM_ASSERT("'" + ::fwIO::s_DATA_KEY + "' key is not defined", data);

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

                this->notificationOfUpdate();
            }
        }
        catch( std::exception& e )
        {
            m_readFailed = true;
            OSLM_ERROR( e.what() );
            ::fwGui::dialog::MessageDialog::showMessageDialog("Atoms reader failed", e.what(),
                                                              ::fwGui::dialog::MessageDialog::CRITICAL);
        }
        catch( ... )
        {
            m_readFailed = true;
            ::fwGui::dialog::MessageDialog::showMessageDialog("Atoms reader failed", "Aborting operation.",
                                                              ::fwGui::dialog::MessageDialog::CRITICAL);
        }

        cursor.setDefaultCursor();

    }
    else
    {
        m_readFailed = true;
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
    SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", object);

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
