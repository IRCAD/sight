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

#include "modules/io/atoms/SWriter.hpp"

#include "modules/io/atoms/SReader.hpp"

#include <atoms/conversion/convert.hpp>

#include <core/com/Signal.hxx>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/System.hpp>

#include <data/Composite.hpp>
#include <data/reflection/visitor/RecursiveLock.hpp>

#include <io/atoms/patch/PatchingManager.hpp>
#include <io/atoms/patch/VersionsGraph.hpp>
#include <io/atoms/patch/VersionsManager.hpp>
#include <io/atoms/types.hpp>
#include <io/atoms/Writer.hpp>
#include <io/patch/semantic/PatchLoader.hpp>
#include <io/zip/WriteDirArchive.hpp>
#include <io/zip/WriteZipArchive.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>
#include <ui/base/dialog/SelectorDialog.hpp>
#include <ui/base/preferences/helper.hpp>

#include <boost/algorithm/string/join.hpp>

#include <filesystem>
#include <regex>

namespace sight::module::io::atoms
{

//-----------------------------------------------------------------------------

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//-----------------------------------------------------------------------------

SWriter::SWriter() :
    m_useAtomsPatcher(false),
    m_exportedVersion("Undefined"),
    m_context("Undefined"),
    m_version("Undefined")
{
    m_sigJobCreated = newSignal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);

    for(SReader::FileExtension2NameType::value_type ext : SReader::s_EXTENSIONS)
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
    sight::io::base::service::IWriter::configuring();

    m_customExts.clear();
    m_allowedExtLabels.clear();

    const ConfigType config = this->getConfigTree();

    const auto archiveCfgs = config.equal_range("archive");

    for(auto it = archiveCfgs.first ; it != archiveCfgs.second ; ++it)
    {
        const std::string backend = it->second.get<std::string>("<xmlattr>.backend");
        SIGHT_ASSERT("No backend attribute given in archive tag", backend != "");
        SIGHT_ASSERT(
            "Unsupported backend '" + backend + "'",
            SReader::s_EXTENSIONS.find("." + backend) != SReader::s_EXTENSIONS.end()
        );

        const auto extCfgs = it->second.equal_range("extension");

        for(auto itExt = extCfgs.first ; itExt != extCfgs.second ; ++itExt)
        {
            const std::string extension = itExt->second.get<std::string>("");
            SIGHT_ASSERT("No extension given for backend '" + backend + "'", !extension.empty());
            SIGHT_ASSERT("Extension must begin with '.'", extension[0] == '.');

            m_customExts[extension]       = backend;
            m_allowedExtLabels[extension] = itExt->second.get("<xmlattr>.label", "");
        }
    }

    const auto extensionsCfg = config.get_child_optional("extensions");

    if(extensionsCfg)
    {
        m_allowedExts.clear();

        const auto extCfgs = extensionsCfg->equal_range("extension");
        for(auto it = extCfgs.first ; it != extCfgs.second ; ++it)
        {
            const std::string ext = it->second.get<std::string>("");

            // The extension must be found either in custom extensions list or in known extensions
            FileExtension2NameType::const_iterator itKnown  = SReader::s_EXTENSIONS.find(ext);
            FileExtension2NameType::const_iterator itCustom = m_customExts.find(ext);

            const bool extIsKnown = (itKnown != SReader::s_EXTENSIONS.end() || itCustom != m_customExts.end());
            SIGHT_ASSERT("Extension '" + ext + "' is not allowed in configuration", extIsKnown);

            if(extIsKnown)
            {
                m_allowedExts.insert(m_allowedExts.end(), ext);
                m_allowedExtLabels[ext] = it->second.get("<xmlattr>.label", "");
                SIGHT_ASSERT("No label given for extension '" + ext + "'", !m_allowedExtLabels[ext].empty());
            }
        }
    }
    else
    {
        m_allowedExts.clear();

        for(FileExtension2NameType::value_type ext : m_customExts)
        {
            m_allowedExts.insert(m_allowedExts.end(), ext.first);
        }

        for(SReader::FileExtension2NameType::value_type ext : SReader::s_EXTENSIONS)
        {
            m_allowedExts.insert(m_allowedExts.end(), ext.first);
            m_allowedExtLabels[ext.first] = ext.second;
        }
    }

    const auto patcherCfg = config.get_child_optional("patcher");

    if(patcherCfg)
    {
        m_context = patcherCfg->get<std::string>("<xmlattr>.context", "MedicalData");
        m_version = patcherCfg->get<std::string>(
            "<xmlattr>.version",
            sight::io::patch::semantic::PatchLoader::getCurrentVersion()
        );
        m_useAtomsPatcher = true;
    }
}

//----------------------------------------------------------------------------

bool SWriter::versionSelection()
{
    auto vg = sight::io::atoms::patch::VersionsManager::getDefault()->getGraph(m_context);

    // have some information about this format
    if(vg)
    {
        std::vector<std::string> versions = vg->getConnectedVersions(m_version);
        if(versions.size() == 0)
        {
            m_exportedVersion = m_version;
            return true;
        }
        else
        {
            versions.push_back(m_version);

            std::transform(
                versions.begin(),
                versions.end(),
                versions.begin(),
                [](const std::string& _version)
                {
                    static const std::regex ala("ALA");
                    return std::regex_replace(_version, ala, "");
                });

            // Sort versions according to repository/number. This is not really honnest since we should not make any
            // assumption about the versioning convention, especially for child repositories... But since this is
            // how things are organized currently I don't think this is such a big issue
            std::sort(
                versions.begin(),
                versions.end(),
                [](const std::string& _a, const std::string& _b)
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
                    catch(std::out_of_range e)
                    {
                        SIGHT_ERROR("Bad version format: either " + _a + " or " + _b);
                        return false;
                    }
                });

            std::vector<std::string> prettyVersionsAll;
            std::map<std::string, std::string> prettyVersionsToVersions;

            for(const auto& v : versions)
            {
                try
                {
                    const std::string num  = v.substr(0, 3);
                    const std::string repo = v.substr(3, v.size());

                    const std::string prettyVersion = num + ((repo.empty()) ? "" : " (" + repo + ")");
                    prettyVersionsAll.push_back(prettyVersion);
                    prettyVersionsToVersions[prettyVersion] = v;
                }
                catch(std::out_of_range e)
                {
                    SIGHT_ERROR("Bad version format: " + v);

                    prettyVersionsAll.push_back(v);
                    prettyVersionsToVersions[v] = v;
                }
            }

            // Create a shortened list of versions, with only the latest ones of each repo
            std::vector<std::string> prettyVersions;
            prettyVersions.push_back(prettyVersionsAll[0]);
            for(auto itVersion = prettyVersionsAll.begin() + 1 ; itVersion != prettyVersionsAll.end() ; ++itVersion)
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

            sight::ui::base::dialog::SelectorDialog dialogVersion;

            dialogVersion.setTitle("Archive version");
            dialogVersion.setMessage("Select an archive version");

            bool selectAmongstAllVersions = false;
            dialogVersion.addCustomButton(
                "Advanced",
                [&selectAmongstAllVersions]()
                {
                    selectAmongstAllVersions = true;
                });

            dialogVersion.setSelections(prettyVersions);
            std::string result = dialogVersion.show();
            if(selectAmongstAllVersions)
            {
                sight::ui::base::dialog::SelectorDialog dialogVersionAll;

                dialogVersionAll.setTitle("Archive version");
                dialogVersionAll.setMessage("Select an archive version");
                dialogVersionAll.setSelections(prettyVersionsAll);
                result = dialogVersionAll.show();
            }

            if(!result.empty())
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

    sight::ui::base::Cursor cursor;
    cursor.setCursor(ui::base::ICursor::BUSY);

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
            sight::ui::base::dialog::MessageDialog::show(
                "Medical data writer failed",
                errorMessage,
                sight::ui::base::dialog::IMessageDialog::CRITICAL
            );
            m_writeFailed = true;
            return;
        }
    }

    std::filesystem::path filePath = requestedFilePath;
    if(std::filesystem::exists(requestedFilePath))
    {
        SIGHT_THROW_IF(
            "can't write to : " << requestedFilePath << ", it is a directory.",
            std::filesystem::is_directory(requestedFilePath)
        );
    }

    const std::filesystem::path folderPath = filePath.parent_path();
    std::string extension                  = filePath.extension().string();

    // Check if the extension of the filename is set. If not, assign it to the selected extension.
    if(extension.empty())
    {
        extension = m_selectedExtension;

        filePath += extension;
    }

    SIGHT_THROW_IF("Extension is empty", extension.empty());

    SIGHT_THROW_IF(
        "The file extension '" << extension << "' is not managed",
        m_allowedExts.find(extension) == m_allowedExts.end()
    );

    // Find in custom extensions if our extension exists.
    if(m_customExts.find(extension) != m_customExts.end())
    {
        extension = "." + m_customExts[extension];
    }

    // Must be unlocked since we use a RecursiveLock later;
    const auto obj =
        [&]
        {
            const auto data = m_data.lock();

            SIGHT_ASSERT(
                "The input key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.",
                data
            );

            return data.get_shared();
        }();

    // Mutex data lock
    data::reflection::visitor::RecursiveLock recursiveLock(obj);

    sight::atoms::Object::sptr atom;
    const unsigned int progressBarOffset = 10;

    // Convert data to atom : job 1
    core::jobs::Job::sptr convertJob = core::jobs::Job::New(
        "Writing " + extension + " file",
        [ =, &atom](core::jobs::Job& runningJob)
        {
            runningJob.doneWork(progressBarOffset);

            atom = sight::atoms::conversion::convert(data::Object::constCast(obj));
            runningJob.done();
        },
        m_associatedWorker
    );

    // Path atom : job 2
    core::jobs::Job::sptr patchingJob = core::jobs::Job::New(
        "Writing " + extension + " file",
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

            if(m_useAtomsPatcher)
            {
                sight::io::atoms::patch::PatchingManager globalPatcher(atom);
                atom = globalPatcher.transformTo(m_exportedVersion);
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
    core::jobs::Job::sptr writeJob = core::jobs::Job::New(
        "Writing " + extension + " file",
        [ =, &atom](core::jobs::Job& runningJob)
        {
            runningJob.doneWork(progressBarOffset);

            // Write atom
            sight::io::zip::IWriteArchive::sptr writeArchive;
            sight::io::atoms::FormatType format;
            std::filesystem::path archiveRootName;
            if(extension == ".json")
            {
                writeArchive    = sight::io::zip::WriteDirArchive::New(tmpFolderPath.string());
                archiveRootName = tmpFilename;
                format          = sight::io::atoms::JSON;
            }
            else if(extension == ".jsonz")
            {
                writeArchive = sight::io::zip::WriteZipArchive::New(
                    tmpFilePath.string(),
                    "",
                    ""
                );
                archiveRootName = "root.json";
                format          = sight::io::atoms::JSON;
            }
            else if(extension == ".cpz")
            {
                writeArchive = sight::io::zip::WriteZipArchive::New(
                    tmpFilePath.string(),
                    "",
                    sight::ui::base::preferences::getPassword()
                );
                archiveRootName = "root.json";
                format          = sight::io::atoms::JSON;
            }
            else if(extension == ".sight")
            {
                writeArchive = sight::io::zip::WriteZipArchive::New(
                    tmpFilePath.string(),
                    "",
                    sight::ui::base::preferences::getPassword()
                );
                archiveRootName = "root.json";
                format          = sight::io::atoms::JSON;
            }
            else if(extension == ".xml")
            {
                writeArchive    = sight::io::zip::WriteDirArchive::New(tmpFolderPath.string());
                archiveRootName = tmpFilename;
                format          = sight::io::atoms::XML;
            }
            else if(extension == ".xmlz")
            {
                writeArchive    = sight::io::zip::WriteZipArchive::New(tmpFilePath.string());
                archiveRootName = "root.xml";
                format          = sight::io::atoms::XML;
            }
            else
            {
                SIGHT_THROW("This file extension '" << extension << "' is not managed");
            }

            const std::filesystem::path folderDirName =
                sight::io::atoms::Writer(atom).write(writeArchive, archiveRootName, format);
            writeArchive.reset();

            // If the save is successful, remove the old file if it exists.
            if(std::filesystem::exists(filePath))
            {
                std::filesystem::remove(filePath);
            }

            // Rename the temporary file with the real name and move it to the right folder.

            core::tools::System::robustRename(tmpFilePath, filePath);

            if(std::filesystem::exists(tmpFolderPath / folderDirName))
            {
                core::tools::System::robustRename(tmpFolderPath / folderDirName, folderPath / folderDirName);
            }

            runningJob.done();
        },
        m_associatedWorker
    );

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
        SIGHT_ERROR(_e.what());
        sight::ui::base::dialog::MessageDialog::show(
            "Medical data writer failed",
            _e.what(),
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
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
        sight::ui::base::dialog::MessageDialog::show(
            "Medical data writer failed",
            "Writing process aborted",
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
        m_writeFailed = true;
    }

    cursor.setDefaultCursor();
}

//-----------------------------------------------------------------------------

sight::io::base::service::IOPathType SWriter::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//-----------------------------------------------------------------------------

void SWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    if(!m_useAtomsPatcher || versionSelection())
    {
        sight::ui::base::dialog::LocationDialog dialogFile;
        dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
        dialogFile.setDefaultLocation(defaultDirectory);
        dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);
        dialogFile.setType(ui::base::dialog::LocationDialog::SINGLE_FILE);

        for(const std::string& ext : m_allowedExts)
        {
            dialogFile.addFilter(m_allowedExtLabels[ext], "*" + ext);
        }

        auto result = core::location::SingleFile::dynamicCast(dialogFile.show());

        if(result)
        {
            this->setFile(result->getFile());
            m_selectedExtension = dialogFile.getCurrentSelection();
            defaultDirectory->setFolder(result->getFile().parent_path());
            dialogFile.saveDefaultLocation(defaultDirectory);
        }
        else
        {
            this->clearLocations();
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::atoms
