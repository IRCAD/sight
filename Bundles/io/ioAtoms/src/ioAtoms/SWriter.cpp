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

#include "ioAtoms/SWriter.hpp"

#include "ioAtoms/SReader.hpp"

#include <fwAtomConversion/convert.hpp>

#include <fwAtomsBoostIO/types.hpp>
#include <fwAtomsBoostIO/Writer.hpp>

#include <fwAtomsPatch/PatchingManager.hpp>
#include <fwAtomsPatch/VersionsGraph.hpp>
#include <fwAtomsPatch/VersionsManager.hpp>

#include <fwCom/Signal.hxx>

#include <fwData/Composite.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataCamp/visitor/RecursiveLock.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/Job.hpp>

#include <fwMDSemanticPatch/PatchLoader.hpp>

#include <fwServices/macros.hpp>

#include <fwZip/WriteDirArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/assign.hpp>

#include <filesystem>

#include <regex>

namespace ioAtoms
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioAtoms::SWriter, ::fwData::Object );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

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
    using namespace boost::assign;

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
                        OSLM_ERROR("Bad version format: either " + _a + " or " + _b);
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
                    OSLM_ERROR("Bad version format: " + v);

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

            ::fwGui::dialog::SelectorDialog dialogVersion;

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
                ::fwGui::dialog::SelectorDialog dialogVersionAll;

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

    ::fwData::Object::csptr obj = this->getInput< ::fwData::Object >(::fwIO::s_DATA_KEY);
    SLM_ASSERT("The input key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", obj);

    ::fwGui::Cursor cursor;
    cursor.setCursor(::fwGui::ICursor::BUSY);

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
            ::fwGui::dialog::MessageDialog::showMessageDialog("Medical data writer failed",
                                                              errorMessage,
                                                              ::fwGui::dialog::IMessageDialog::CRITICAL);
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
        filePath = filePath / std::tmpnam(nullptr);
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

    const std::filesystem::path filename = filePath.filename();

    FW_RAISE_IF("The file extension '" << extension << "' is not managed",
                m_allowedExts.find(extension) == m_allowedExts.end());

    // Find in custom extensions if our extension exists.
    if (m_customExts.find(extension) != m_customExts.end())
    {
        extension = "." + m_customExts[extension];
    }

    // Mutex data lock
    ::fwDataCamp::visitor::RecursiveLock recursiveLock(obj);

    ::fwAtoms::Object::sptr atom;
    const unsigned int progressBarOffset = 10;

    // Convert data to atom : job 1
    ::fwJobs::Job::sptr convertJob = ::fwJobs::Job::New("Writing " + extension + " file",
                                                        [ =, &atom](::fwJobs::Job& runningJob)
        {
            runningJob.doneWork(progressBarOffset);

            atom = ::fwAtomConversion::convert(::fwData::Object::constCast(obj));
            runningJob.done();
        }, m_associatedWorker );

    // Path atom : job 2
    ::fwJobs::Job::sptr patchingJob = ::fwJobs::Job::New("Writing " + extension + " file",
                                                         [ =, &atom](::fwJobs::Job& runningJob)
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

    // Writing file : job 3
    ::fwJobs::Job::sptr writeJob = ::fwJobs::Job::New("Writing " + extension + " file",
                                                      [ =, &atom](::fwJobs::Job& runningJob)
        {
            runningJob.doneWork(progressBarOffset);
            // Write atom
            ::fwZip::IWriteArchive::sptr writeArchive;
            ::fwAtomsBoostIO::FormatType format;
            std::filesystem::path archiveRootName;
            if ( extension == ".json" )
            {
                writeArchive    = ::fwZip::WriteDirArchive::New(folderPath.string());
                archiveRootName = filename;
                format          = ::fwAtomsBoostIO::JSON;
            }
            else if ( extension == ".jsonz")
            {
                if ( std::filesystem::exists( filePath ) )
                {
                    std::filesystem::remove( filePath );
                }
                writeArchive    = ::fwZip::WriteZipArchive::New(filePath.string());
                archiveRootName = "root.json";
                format          = ::fwAtomsBoostIO::JSON;
            }
            else if ( extension == ".xml" )
            {
                writeArchive    = ::fwZip::WriteDirArchive::New(folderPath.string());
                archiveRootName = filename;
                format          = ::fwAtomsBoostIO::XML;
            }
            else if ( extension == ".xmlz" )
            {
                if ( std::filesystem::exists( filePath ) )
                {
                    std::filesystem::remove( filePath );
                }
                writeArchive    = ::fwZip::WriteZipArchive::New(filePath.string());
                archiveRootName = "root.xml";
                format          = ::fwAtomsBoostIO::XML;
            }
            else
            {
                FW_RAISE( "This file extension '" << extension << "' is not managed" );
            }

            ::fwAtomsBoostIO::Writer(atom).write( writeArchive, archiveRootName, format );
            writeArchive.reset();

            runningJob.done();
        }, m_associatedWorker );

    ::fwJobs::Aggregator::sptr jobs = ::fwJobs::Aggregator::New(extension + " writer");
    jobs->add(convertJob);
    jobs->add(patchingJob);
    jobs->add(writeJob);
    jobs->setCancelable(false);

    m_sigJobCreated->emit(jobs);

    try
    {
        jobs->run().get();
    }
    catch( std::exception& e )
    {
        OSLM_ERROR( e.what() );
        ::fwGui::dialog::MessageDialog::showMessageDialog("Medical data writer failed",
                                                          e.what(),
                                                          ::fwGui::dialog::IMessageDialog::CRITICAL);
        m_writeFailed = true;
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Medical data writer failed",
                                                          "Writing process aborted",
                                                          ::fwGui::dialog::IMessageDialog::CRITICAL);
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
    static std::filesystem::path _sDefaultPath;

    if( !m_useAtomsPatcher || versionSelection() )
    {
        ::fwGui::dialog::LocationDialog dialogFile;
        dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
        dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
        dialogFile.setType(::fwGui::dialog::LocationDialog::SINGLE_FILE);

        for(const std::string& ext :  m_allowedExts)
        {
            dialogFile.addFilter(m_allowedExtLabels[ext], "*" + ext);
        }

        ::fwData::location::SingleFile::sptr result
            = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );

        if (result)
        {
            m_selectedExtension = dialogFile.getCurrentSelection();
            _sDefaultPath       = result->getPath();
            this->setFile( _sDefaultPath );
            dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath.parent_path()) );
        }
        else
        {
            this->clearLocations();
        }

    }

}

//-----------------------------------------------------------------------------

} // namespace ioAtoms
