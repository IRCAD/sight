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

#include "modules/io/atoms/SReader.hpp"

#include <atoms/conversion/convert.hpp>

#include <core/com/Signal.hxx>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>

#include <data/Array.hpp>
#include <data/Composite.hpp>
#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/tools/helper/Composite.hpp>

#include <service/macros.hpp>

#include <boost/algorithm/string/join.hpp>

#include <io/atoms/filter/factory/new.hpp>
#include <io/atoms/filter/IFilter.hpp>
#include <io/atoms/patch/PatchingManager.hpp>
#include <io/atoms/Reader.hpp>
#include <io/atoms/types.hpp>
#include <io/patch/semantic/PatchLoader.hpp>
#include <io/zip/ReadDirArchive.hpp>
#include <io/zip/ReadZipArchive.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/preferences/helper.hpp>

#include <filesystem>

namespace sight::module::io::atoms
{

fwServicesRegisterMacro( ::sight::io::base::service::IReader, ::sight::module::io::atoms::SReader,
                         ::sight::data::Object )

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
        this->setOutput(sight::io::base::service::s_DATA_KEY, nullptr);
    }
}

//-----------------------------------------------------------------------------

void SReader::configuring()
{
    sight::io::base::service::IReader::configuring();

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
                                                 sight::io::patch::semantic::PatchLoader::getCurrentVersion());
        m_useAtomsPatcher = true;
    }

    const std::string output = config.get<std::string>("out.<xmlattr>.key", "");
    if (output == sight::io::base::service::s_DATA_KEY )
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
        data::Object::sptr data = this->getInOut< data::Object >(sight::io::base::service::s_DATA_KEY);
        SLM_ASSERT("The inout key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.",
                   m_outputMode || data);

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

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

            sight::atoms::Object::sptr atom;

            const unsigned int progressBarOffset = 10;

            // Reading file : job 1
            core::jobs::Job::sptr fileReadingJob = core::jobs::Job::New("Reading " + extension + " file",
                                                                        [ =, &atom](core::jobs::Job& runningJob)
                {
                    runningJob.doneWork(progressBarOffset);

                    // Read atom
                    sight::io::zip::IReadArchive::sptr readArchive;
                    std::filesystem::path archiveRootName;
                    sight::io::atoms::FormatType format = sight::io::atoms::UNSPECIFIED;

                    if ( extension == ".json" )
                    {
                        readArchive     = sight::io::zip::ReadDirArchive::New(folderPath.string());
                        archiveRootName = filename;
                        format          = sight::io::atoms::JSON;
                    }
                    else if ( extension == ".jsonz" )
                    {
                        readArchive     = sight::io::zip::ReadZipArchive::New(filePath.string());
                        archiveRootName = "root.json";
                        format          = sight::io::atoms::JSON;
                    }
                    else if ( extension == ".cpz" )
                    {
                        readArchive = sight::io::zip::ReadZipArchive::New(filePath.string(),
                                                                          sight::ui::base::preferences::getPassword());
                        archiveRootName = "root.json";
                        format          = sight::io::atoms::JSON;
                    }
                    else if ( extension == ".xml" )
                    {
                        readArchive     = sight::io::zip::ReadDirArchive::New(folderPath.string());
                        archiveRootName = filename;
                        format          = sight::io::atoms::XML;
                    }
                    else if ( extension == ".xmlz" )
                    {
                        readArchive     = sight::io::zip::ReadZipArchive::New(filePath.string());
                        archiveRootName = "root.xml";
                        format          = sight::io::atoms::XML;
                    }
                    else
                    {
                        FW_RAISE( "This file extension '" << extension << "' is not managed" );
                    }

                    sight::io::atoms::Reader reader;
                    atom = sight::atoms::Object::dynamicCast( reader.read( readArchive, archiveRootName, format ) );

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

                        sight::io::atoms::patch::PatchingManager globalPatcher(atom);
                        atom = globalPatcher.transformTo( m_version );
                    }

                    if(!m_filter.empty())
                    {
                        auto filter = sight:: io::atoms::filter::factory::New(m_filter);
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
                        newData =
                            sight::atoms::conversion::convert(atom,
                                                              sight::atoms::conversion::AtomVisitor::StrictPolicy());
                    }
                    else if("Reuse" == m_uuidPolicy)
                    {
                        newData =
                            sight::atoms::conversion::convert(atom,
                                                              sight::atoms::conversion::AtomVisitor::ReusePolicy());
                    }
                    else
                    {
                        newData =
                            sight::atoms::conversion::convert(atom,
                                                              sight::atoms::conversion::AtomVisitor::ChangePolicy());
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
                this->setOutput(sight::io::base::service::s_DATA_KEY, newData);
            }
            else
            {
                if(!data)
                {
                    m_readFailed = true;
                }
                SLM_ASSERT("'" + sight::io::base::service::s_DATA_KEY + "' key is not defined", data);

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
            sight::ui::base::dialog::MessageDialog::show("Atoms reader failed", e.what(),
                                                         sight::ui::base::dialog::MessageDialog::CRITICAL);
        }
        catch( ... )
        {
            m_readFailed = true;
            sight::ui::base::dialog::MessageDialog::show("Atoms reader failed", "Aborting operation.",
                                                         sight::ui::base::dialog::MessageDialog::CRITICAL);
        }

        cursor.setDefaultCursor();

    }
    else
    {
        m_readFailed = true;
    }
}

//-----------------------------------------------------------------------------

sight::io::base::service::IOPathType SReader::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void SReader::notificationOfUpdate()
{
    data::Object::sptr object = this->getInOut< data::Object >(sight::io::base::service::s_DATA_KEY);
    SLM_ASSERT("The inout key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", object);

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

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setType(ui::base::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setOption(ui::base::dialog::LocationDialog::FILE_MUST_EXIST);

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

} // namespace sight::module::io::atoms
