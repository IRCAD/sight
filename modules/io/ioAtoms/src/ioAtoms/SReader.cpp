/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <atoms/conversion/convert.hpp>

#include <core/com/Signal.hxx>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>

#include <data/Array.hpp>
#include <data/Composite.hpp>
#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/tools/helper/Composite.hpp>

#include <fwAtomsBoostIO/Reader.hpp>
#include <fwAtomsBoostIO/types.hpp>

#include <fwAtomsFilter/factory/new.hpp>
#include <fwAtomsFilter/IFilter.hpp>

#include <fwAtomsPatch/PatchingManager.hpp>

#include <fwMDSemanticPatch/PatchLoader.hpp>

#include <fwZip/ReadDirArchive.hpp>
#include <fwZip/ReadZipArchive.hpp>

#include <gui/Cursor.hpp>
#include <gui/dialog/LocationDialog.hpp>
#include <gui/dialog/MessageDialog.hpp>
#include <gui/preferences/helper.hpp>

#include <services/macros.hpp>

#include <boost/algorithm/string/join.hpp>

#include <filesystem>

namespace ioAtoms
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioAtoms::SReader, ::sight::data::Object )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

const SReader::FileExtension2NameType SReader::s_EXTENSIONS = { {".xml", "XML"},
                                                                { ".xmlz", "Zipped XML"},
                                                                { ".json", "JSON"},
                                                                { ".jsonz", "Zipped JSON"},
                                                                { ".cpz", "Crypted JSON"} };

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
        data::Object::sptr data = this->getInOut< data::Object >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", m_outputMode || data);

        gui::Cursor cursor;
        cursor.setCursor(gui::ICursor::BUSY);

        try
        {
            const std::filesystem::path& filePath  = this->getFile();
            const std::filesystem::path folderPath = filePath.parent_path();
            const std::filesystem::path filename   = filePath.filename();
            std::string extension                  = filePath.extension().string();

            FW_RAISE_IF( "Unable to guess file format (missing extension)", extension.empty() );

            if(m_customExts.find(extension) != m_customExts.end())
            {
                extension = "." + m_customExts[extension];
            }

            atoms::Object::sptr atom;

            const unsigned int progressBarOffset = 10;

            // Reading file : job 1
            core::jobs::Job::sptr fileReadingJob = core::jobs::Job::New("Reading " + extension + " file",
                                                                        [ =, &atom](core::jobs::Job& runningJob)
                {
                    runningJob.doneWork(progressBarOffset);

                    // Read atom
                    ::fwZip::IReadArchive::sptr readArchive;
                    std::filesystem::path archiveRootName;
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
                    else if ( extension == ".cpz" )
                    {
                        readArchive = ::fwZip::ReadZipArchive::New(filePath.string(),
                                                                   gui::preferences::getPassword());
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
                    atom = atoms::Object::dynamicCast( reader.read( readArchive, archiveRootName, format ) );

                    FW_RAISE_IF( "Invalid atoms file :'" << filePath << "'", !atom );

                    runningJob.doneWork(progressBarOffset);

                    runningJob.done();

                }, m_associatedWorker);

            // patching atom : job 2
            core::jobs::Job::sptr patchingJob = core::jobs::Job::New("Reading " + extension + " file",
                                                                     [ =, &atom](core::jobs::Job& runningJob)
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
                        SLM_ASSERT("Failed to create IFilter implementation '" << m_filter << "'", filter);
                        filter->apply(atom);
                    }
                    runningJob.done();
                }, m_associatedWorker);

            data::Object::sptr newData;

            // convert to fwData : job 3
            core::jobs::Job::sptr atomToDataJob = core::jobs::Job::New("Reading " + extension + " file",
                                                                       [ =, &newData, &atom](
                                                                           core::jobs::Job& runningJob)
                {
                    runningJob.doneWork(progressBarOffset);
                    if(runningJob.cancelRequested())
                    {
                        return;
                    }
                    if("Strict" == m_uuidPolicy)
                    {
                        newData = atoms::conversion::convert(atom, atoms::conversion::AtomVisitor::StrictPolicy());
                    }
                    else if("Reuse" == m_uuidPolicy)
                    {
                        newData = atoms::conversion::convert(atom, atoms::conversion::AtomVisitor::ReusePolicy());
                    }
                    else
                    {
                        newData = atoms::conversion::convert(atom, atoms::conversion::AtomVisitor::ChangePolicy());
                    }

                    runningJob.done();
                }, m_associatedWorker);

            core::jobs::Aggregator::sptr jobs = core::jobs::Aggregator::New(extension + " reader");
            jobs->add(fileReadingJob);
            jobs->add(patchingJob);
            jobs->add(atomToDataJob);

            m_sigJobCreated->emit(jobs);

            jobs->run().get();

            if(jobs->getState() == core::jobs::IJob::CANCELED)
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

                // Workaround to read a data::Array.
                // The shallowCopy of a data::Array is not allowed due to unknown buffer owner.
                // So in the case of reading an Array we swap buffers.
                if(newData->getClassname() == data::Array::classname())
                {
                    data::Array::dynamicCast(data)->swap( data::Array::dynamicCast(newData) );
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
            SLM_ERROR( e.what() );
            gui::dialog::MessageDialog::show("Atoms reader failed", e.what(),
                                             gui::dialog::MessageDialog::CRITICAL);
        }
        catch( ... )
        {
            m_readFailed = true;
            gui::dialog::MessageDialog::show("Atoms reader failed", "Aborting operation.",
                                             gui::dialog::MessageDialog::CRITICAL);
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
    data::Object::sptr object = this->getInOut< data::Object >(::fwIO::s_DATA_KEY);
    SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", object);

    auto sig = object->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SReader::configureWithIHM()
{
    this->openLocationDialog();
}

//-----------------------------------------------------------------------------

void SReader::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    gui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setType(gui::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.setOption(gui::dialog::ILocationDialog::READ);
    dialogFile.setOption(gui::dialog::LocationDialog::FILE_MUST_EXIST);

    dialogFile.addFilter("Medical data", "*" + ::boost::algorithm::join(m_allowedExts, " *"));

    for(const std::string& ext :  m_allowedExts)
    {
        dialogFile.addFilter(m_allowedExtLabels[ext], "*" + ext);
    }

    data::location::SingleFile::sptr result
        = data::location::SingleFile::dynamicCast( dialogFile.show() );

    if (result)
    {
        _sDefaultPath = result->getPath();
        this->setFile( _sDefaultPath );
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath.parent_path()) );
    }
    else
    {
        this->clearLocations();
    }
}

//-----------------------------------------------------------------------------

} // namespace ioAtoms
