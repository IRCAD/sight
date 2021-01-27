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

#include "ioAtoms/SWriter.hpp"

#include "ioAtoms/SReader.hpp"

#include <atoms/conversion/convert.hpp>

#include <core/com/Signal.hxx>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>
#include <core/tools/System.hpp>

#include <data/Composite.hpp>
#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/reflection/visitor/RecursiveLock.hpp>

#include <fwAtomsBoostIO/types.hpp>
#include <fwAtomsBoostIO/Writer.hpp>

#include <fwAtomsPatch/PatchingManager.hpp>
#include <fwAtomsPatch/VersionsGraph.hpp>
#include <fwAtomsPatch/VersionsManager.hpp>

#include <fwMDSemanticPatch/PatchLoader.hpp>

#include <fwZip/WriteDirArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <gui/Cursor.hpp>
#include <gui/dialog/MessageDialog.hpp>
#include <gui/dialog/ProgressDialog.hpp>
#include <gui/dialog/SelectorDialog.hpp>
#include <gui/preferences/helper.hpp>

#include <services/macros.hpp>

#include <boost/algorithm/string/join.hpp>

#include <filesystem>
#include <regex>

namespace ioAtoms
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioAtoms::SWriter, ::sight::data::Object )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//-----------------------------------------------------------------------------

SWriter::SWriter() :
    m_useAtomsPatcher(false),
    m_exportedVersion("Undefined"),
    m_context("Undefined"),
    m_version("Undefined")
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );

    for(SReader::FileExtension2NameType::value_type ext :  SReader::s_EXTENSIONS)
    {
        m_allowedExts.insert(m_allowedExts.end(), ext.first);
    }
}

//-----------------------------------------------------------------------------

void SWriter::starting()
{
}

//-----------------------------------------------------------------------------

void SWriter::stopping()
{
}

//-----------------------------------------------------------------------------

void SWriter::configuring()
{
    ::fwIO::IWriter::configuring();

    m_customExts.clear();
    m_allowedExtLabels.clear();

    const ConfigType config = this->getConfigTree();

    const auto archiveCfgs = config.equal_range("archive");

    for (auto it = archiveCfgs.first; it != archiveCfgs.second; ++it)
    {
        const std::string backend = it->second.get<std::string>("<xmlattr>.backend");
        SLM_ASSERT("No backend attribute given in archive tag", backend != "");
        SLM_ASSERT("Unsupported backend '" + backend + "'",
                   SReader::s_EXTENSIONS.find("." + backend) != SReader::s_EXTENSIONS.end());

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
            FileExtension2NameType::const_iterator itKnown  = SReader::s_EXTENSIONS.find(ext);
            FileExtension2NameType::const_iterator itCustom = m_customExts.find(ext);

            const bool extIsKnown = (itKnown != SReader::s_EXTENSIONS.end() || itCustom != m_customExts.end());
            SLM_ASSERT("Extension '" + ext + "' is not allowed in configuration", extIsKnown);

            if(extIsKnown)
            {
                m_allowedExts.insert(m_allowedExts.end(), ext);
                m_allowedExtLabels[ext] = it->second.get("<xmlattr>.label", "");
                SLM_ASSERT("No label given for extension '" + ext + "'", !m_allowedExtLabels[ext].empty());
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

    const auto patcherCfg = config.get_child_optional("patcher");

    if (patcherCfg)
    {
        m_context = patcherCfg->get<std::string>("<xmlattr>.context", "MedicalData");
        m_version = patcherCfg->get<std::string>("<xmlattr>.version",
                                                 ::fwMDSemanticPatch::PatchLoader::getCurrentVersion());
        m_useAtomsPatcher = true;
    }
}

//----------------------------------------------------------------------------

bool SWriter::versionSelection()
{
    ::fwAtomsPatch::VersionsGraph::sptr vg = ::fwAtomsPatch::VersionsManager::getDefault()->getGraph(m_context);

    // have some information about this format
    if ( vg )
    {
        std::vector< std::string > versions = vg->getConnectedVersions(m_version);
        if ( versions.size() == 0 )
        {
            m_exportedVersion = m_version;
            return true;
        }
        else
        {
            versions.push_back(m_version);

            std::transform(versions.begin(), versions.end(), versions.begin(), [](const std::string& _version)
                {
                    static const std::regex ala("ALA");
                    return std::regex_replace(_version, ala, "");
                } );

            // Sort versions according to repository/number. This is not really honnest since we should not make any
            // assumption about the versioning convention, especially for child repositories... But since this is
            // how things are organized currently I don't think this is such a big issue
            std::sort(versions.begin(), versions.end(), [](const std::string& _a, const std::string& _b)
                {
                    try
                    {
                        // Assume a pattern like V[0-9][0-9][A-Z][A-Z][A-Z]
                        // Sort with the version number
                        const std::string repoA = _a.substr(3, _a.size());
                        const std::string repoB = _b.substr(3, _b.size());
                        if(repoA == repoB)
                        {
                            const std::string numA = _a.substr(1, 2);
                            const std::string numB = _b.substr(1, 2);
                            return numA > numB;
                        }
                        return repoA < repoB;
                    }
                    catch ( std::out_of_range e)
                    {
                        SLM_ERROR("Bad version format: either " + _a + " or " + _b);
                        return false;
                    }
                });

            std::vector< std::string > prettyVersionsAll;
            std::map< std::string, std::string > prettyVersionsToVersions;

            for(const auto& v : versions)
            {
                try
                {
                    const std::string num  = v.substr(0, 3);
                    const std::string repo = v.substr(3, v.size());

                    const std::string prettyVersion = num + ((repo.empty()) ? "" : " (" + repo + ")");
                    prettyVersionsAll.push_back(prettyVersion);
                    prettyVersionsToVersions[ prettyVersion ] = v;

                }
                catch ( std::out_of_range e)
                {
                    SLM_ERROR("Bad version format: " + v);

                    prettyVersionsAll.push_back(v);
                    prettyVersionsToVersions[ v ] = v;
                }
            }

            // Create a shortened list of versions, with only the latest ones of each repo
            std::vector< std::string > prettyVersions;
            prettyVersions.push_back(prettyVersionsAll[0]);
            for(auto itVersion = prettyVersionsAll.begin() + 1; itVersion != prettyVersionsAll.end(); ++itVersion)
            {
                const auto& versionA    = *(itVersion - 1);
                const auto& versionB    = *(itVersion);
                const std::string repoA = versionA.substr(3, versionA.size());
                const std::string repoB = versionB.substr(3, versionB.size());

                if(repoA != repoB)
                {
                    prettyVersions.push_back(versionB);
                }
            }

            gui::dialog::SelectorDialog dialogVersion;

            dialogVersion.setTitle("Archive version");
            dialogVersion.setMessage("Select an archive version");

            bool selectAmongstAllVersions = false;
            dialogVersion.addCustomButton("Advanced", [&selectAmongstAllVersions]()
                {
                    selectAmongstAllVersions = true;
                });

            dialogVersion.setSelections(prettyVersions);
            std::string result = dialogVersion.show();
            if(selectAmongstAllVersions)
            {
                gui::dialog::SelectorDialog dialogVersionAll;

                dialogVersionAll.setTitle("Archive version");
                dialogVersionAll.setMessage("Select an archive version");
                dialogVersionAll.setSelections(prettyVersionsAll);
                result = dialogVersionAll.show();
            }
            if( !result.empty() )
            {
                m_exportedVersion = prettyVersionsToVersions[result];
            }
            return !result.empty();
        }
    }
    else
    {
        m_exportedVersion = m_version;
        return true;
    }
}

//-----------------------------------------------------------------------------

void SWriter::updating()
{
    // Reset m_writeFailed to the default value (default).
    m_writeFailed = false;

    if(!this->hasLocationDefined())
    {
        m_writeFailed = true;
        return;
    }

    data::Object::csptr obj = this->getInput< data::Object >(::fwIO::s_DATA_KEY);
    SLM_ASSERT("The input key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", obj);

    gui::Cursor cursor;
    cursor.setCursor(gui::ICursor::BUSY);

    // Get the selected extension
    const std::filesystem::path& requestedFilePath = this->getFile();
    std::string requestedExtension                 = requestedFilePath.extension().string();

    if(!requestedExtension.empty())
    {
        // Check first if the requestedExtension exists in allowed extensions.
        if(m_allowedExts.find(requestedExtension) != m_allowedExts.end())
        {
            // Overwrite selectedExtension.
            m_selectedExtension = requestedExtension;
        }
        else
        {
            const std::string errorMessage("File extension '" + requestedExtension + "' is not handled.");
            gui::dialog::MessageDialog::show("Medical data writer failed",
                                             errorMessage,
                                             gui::dialog::IMessageDialog::CRITICAL);
            m_writeFailed = true;
            return;
        }
    }

    std::filesystem::path filePath = requestedFilePath;
    if( std::filesystem::exists( requestedFilePath ) )
    {
        FW_RAISE_IF( "can't write to : " << requestedFilePath << ", it is a directory.",
                     std::filesystem::is_directory(requestedFilePath)
                     );
    }

    const std::filesystem::path folderPath = filePath.parent_path();
    std::string extension                  = filePath.extension().string();

    // Check if the extension of the filename is set. If not, assign it to the selected extension.
    if (extension.empty())
    {
        extension = m_selectedExtension;

        filePath += extension;
    }

    FW_RAISE_IF( "Extension is empty", extension.empty() );

    FW_RAISE_IF("The file extension '" << extension << "' is not managed",
                m_allowedExts.find(extension) == m_allowedExts.end());

    // Find in custom extensions if our extension exists.
    if (m_customExts.find(extension) != m_customExts.end())
    {
        extension = "." + m_customExts[extension];
    }

    // Mutex data lock
    data::reflection::visitor::RecursiveLock recursiveLock(obj);

    atoms::Object::sptr atom;
    const unsigned int progressBarOffset = 10;

    // Convert data to atom : job 1
    core::jobs::Job::sptr convertJob = core::jobs::Job::New("Writing " + extension + " file",
                                                            [ =, &atom](core::jobs::Job& runningJob)
        {
            runningJob.doneWork(progressBarOffset);

            atom = atoms::conversion::convert(data::Object::constCast(obj));
            runningJob.done();
        }, m_associatedWorker );

    // Path atom : job 2
    core::jobs::Job::sptr patchingJob = core::jobs::Job::New("Writing " + extension + " file",
                                                             [ =, &atom](core::jobs::Job& runningJob)
        {

            atom->setMetaInfo("context", m_context);
            atom->setMetaInfo("version_name", m_version);

            if(runningJob.cancelRequested())
            {
                m_writeFailed = true;
                return;
            }

            runningJob.doneWork(progressBarOffset);

            if( m_useAtomsPatcher )
            {
                ::fwAtomsPatch::PatchingManager globalPatcher( atom );
                atom = globalPatcher.transformTo( m_exportedVersion );
            }

            runningJob.done();
        },
                                                             m_associatedWorker
                                                             );

    // Generate a temporary file name for saving the file.
    const std::filesystem::path tmpFolderPath = core::tools::System::getTemporaryFolder();
    const std::filesystem::path tmpFilePath   = tmpFolderPath / (core::tools::System::genTempFileName() + ".sight");
    const std::filesystem::path tmpFilename   = tmpFilePath.filename();

    // Writing file : job 3
    core::jobs::Job::sptr writeJob = core::jobs::Job::New("Writing " + extension + " file",
                                                          [ =, &atom](core::jobs::Job& runningJob)
        {
            runningJob.doneWork(progressBarOffset);
            // Write atom
            ::fwZip::IWriteArchive::sptr writeArchive;
            ::fwAtomsBoostIO::FormatType format;
            std::filesystem::path archiveRootName;
            if ( extension == ".json" )
            {
                writeArchive    = ::fwZip::WriteDirArchive::New(tmpFolderPath.string());
                archiveRootName = tmpFilename;
                format          = ::fwAtomsBoostIO::JSON;
            }
            else if ( extension == ".jsonz")
            {
                writeArchive    = ::fwZip::WriteZipArchive::New(tmpFilePath.string());
                archiveRootName = "root.json";
                format          = ::fwAtomsBoostIO::JSON;
            }
            else if ( extension == ".cpz" )
            {
                writeArchive = ::fwZip::WriteZipArchive::New(tmpFilePath.string(), "",
                                                             gui::preferences::getPassword());
                archiveRootName = "root.json";
                format          = ::fwAtomsBoostIO::JSON;
            }
            else if ( extension == ".xml" )
            {
                writeArchive    = ::fwZip::WriteDirArchive::New(tmpFolderPath.string());
                archiveRootName = tmpFilename;
                format          = ::fwAtomsBoostIO::XML;
            }
            else if ( extension == ".xmlz" )
            {
                writeArchive    = ::fwZip::WriteZipArchive::New(tmpFilePath.string());
                archiveRootName = "root.xml";
                format          = ::fwAtomsBoostIO::XML;
            }
            else
            {
                FW_RAISE( "This file extension '" << extension << "' is not managed" );
            }

            const std::filesystem::path folderDirName =
                ::fwAtomsBoostIO::Writer(atom).write( writeArchive, archiveRootName, format );
            writeArchive.reset();

            // If the save is successful, remove the old file if it exists.
            if(std::filesystem::exists(filePath))
            {
                std::filesystem::remove(filePath);
            }

            // Rename the temporary file with the real name and move it to the right folder.

            core::tools::System::robustRename(tmpFilePath, filePath);

            if(std::filesystem::exists(tmpFolderPath/folderDirName))
            {
                core::tools::System::robustRename(tmpFolderPath/folderDirName, folderPath/folderDirName);
            }

            runningJob.done();
        }, m_associatedWorker );

    core::jobs::Aggregator::sptr jobs = core::jobs::Aggregator::New(extension + " writer");
    jobs->add(convertJob);
    jobs->add(patchingJob);
    jobs->add(writeJob);
    jobs->setCancelable(false);

    m_sigJobCreated->emit(jobs);

    try
    {
        jobs->run().get();
    }
    catch(std::exception& _e)
    {
        // Delete the temporary file if the process failed.
        if(std::filesystem::exists(tmpFilePath))
        {
            std::filesystem::remove(tmpFilePath);
        }

        // Handle the error.
        SLM_ERROR(_e.what());
        gui::dialog::MessageDialog::show("Medical data writer failed",
                                         _e.what(),
                                         gui::dialog::IMessageDialog::CRITICAL);
        m_writeFailed = true;
    }
    catch(...)
    {
        // Delete the temporary file if the process failed.
        if(std::filesystem::exists(tmpFilePath))
        {
            std::filesystem::remove(tmpFilePath);
        }

        // Handle the error.
        gui::dialog::MessageDialog::show("Medical data writer failed",
                                         "Writing process aborted",
                                         gui::dialog::IMessageDialog::CRITICAL);
        m_writeFailed = true;
    }

    cursor.setDefaultCursor();
}

//-----------------------------------------------------------------------------

::fwIO::IOPathType SWriter::getIOPathType() const
{
    return ::fwIO::FILE;
}

//-----------------------------------------------------------------------------

void SWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//-----------------------------------------------------------------------------

void SWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    if( !m_useAtomsPatcher || versionSelection() )
    {
        gui::dialog::LocationDialog dialogFile;
        dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
        dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        dialogFile.setOption(gui::dialog::ILocationDialog::WRITE);
        dialogFile.setType(gui::dialog::LocationDialog::SINGLE_FILE);

        for(const std::string& ext :  m_allowedExts)
        {
            dialogFile.addFilter(m_allowedExtLabels[ext], "*" + ext);
        }

        data::location::SingleFile::sptr result
            = data::location::SingleFile::dynamicCast( dialogFile.show() );

        if (result)
        {
            m_selectedExtension = dialogFile.getCurrentSelection();
            _sDefaultPath       = result->getPath();
            this->setFile( _sDefaultPath );
            dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath.parent_path()) );
        }
        else
        {
            this->clearLocations();
        }

    }
}

//-----------------------------------------------------------------------------

} // namespace ioAtoms
