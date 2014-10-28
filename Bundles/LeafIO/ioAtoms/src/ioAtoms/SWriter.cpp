/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/join.hpp>

#include <fwAtomsBoostIO/types.hpp>
#include <fwAtomsBoostIO/Writer.hpp>

#include <fwAtomsHdf5IO/Writer.hpp>

#include <fwAtomConversion/convert.hpp>

#include <fwData/Composite.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/location/Folder.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwServices/macros.hpp>

#include <fwZip/WriteDirArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <fwDataCamp/visitor/RecursiveLock.hpp>

#include <fwAtomsPatch/VersionsManager.hpp>
#include <fwAtomsPatch/VersionsGraph.hpp>
#include <fwAtomsPatch/PatchingManager.hpp>

#include "ioAtoms/SReader.hpp"
#include "ioAtoms/SWriter.hpp"

namespace ioAtoms
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::io::IWriter , ::ioAtoms::SWriter , ::fwData::Object );

//-----------------------------------------------------------------------------

SWriter::SWriter() :
        m_useAtomsPatcher(false),
        m_exportedVersion ("Undefined"),
        m_context ("Undefined"),
        m_version ("Undefined")
{
    BOOST_FOREACH(SReader::FileExtension2NameType::value_type ext, SReader::s_EXTENSIONS)
    {
        m_allowedExts.insert(m_allowedExts.end(), ext.first);
    }
}

//-----------------------------------------------------------------------------

void SWriter::starting() throw(::fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void SWriter::stopping() throw(::fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void SWriter::configuring() throw(::fwTools::Failed)
{
    ::io::IWriter::configuring();

    typedef SPTR(::fwRuntime::ConfigurationElement) ConfigurationElement;
    typedef std::vector < ConfigurationElement >    ConfigurationElementContainer;

    m_customExts.clear();
    m_allowedExtLabels.clear();

    ConfigurationElementContainer customExtsList = m_configuration->find("archive");
    BOOST_FOREACH(ConfigurationElement archive, customExtsList)
    {
        const std::string& backend = archive->getAttributeValue("backend");
        SLM_ASSERT("No backend attribute given in archive tag", backend != "");
        SLM_ASSERT("Unsupported backend '" + backend + "'",
                SReader::s_EXTENSIONS.find("." + backend) != SReader::s_EXTENSIONS.end());

        ConfigurationElementContainer exts = archive->find("extension");
        BOOST_FOREACH(ConfigurationElement ext, exts)
        {
            const std::string& extension = ext->getValue();
            SLM_ASSERT("No extension given for backend '" + backend + "'", !extension.empty());
            SLM_ASSERT("Extension must begin with '.'", extension[0] == '.');

            m_customExts[extension] = backend;
            m_allowedExtLabels[extension] = ext->getAttributeValue("label");
        }
    }

    ConfigurationElementContainer extensionsList = m_configuration->find("extensions");
    SLM_ASSERT("The <extensions> element can be set at most once.", extensionsList.size() <= 1);

    if(extensionsList.size() == 1)
    {
        m_allowedExts.clear();

        ConfigurationElementContainer extensions = extensionsList.at(0)->find("extension");
        BOOST_FOREACH(ConfigurationElement extension, extensions)
        {
            const std::string& ext = extension->getValue();

            // The extension must be found either in custom extensions list or in known extensions
            FileExtension2NameType::const_iterator itKnown = SReader::s_EXTENSIONS.find(ext);
            FileExtension2NameType::const_iterator itCustom = m_customExts.find(ext);

            const bool extIsKnown = (itKnown != SReader::s_EXTENSIONS.end() || itCustom != m_customExts.end());
            OSLM_ASSERT("Extension '" << ext << "' is not allowed in configuration", extIsKnown);

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

        BOOST_FOREACH(FileExtension2NameType::value_type ext, m_customExts)
        {
            m_allowedExts.insert(m_allowedExts.end(), ext.first);
        }

        BOOST_FOREACH(SReader::FileExtension2NameType::value_type ext, SReader::s_EXTENSIONS)
        {
            m_allowedExts.insert(m_allowedExts.end(), ext.first);
            m_allowedExtLabels[ext.first] = ext.second;
        }
    }

    ConfigurationElementContainer patcher = m_configuration->find("patcher");
    SLM_ASSERT("The <patcher> element can be set at most once.", patcher.size() <= 1 );
    if (patcher.size() == 1)
    {
        m_context = patcher.at(0)->getExistingAttributeValue("context");
        m_version = patcher.at(0)->getExistingAttributeValue("version");
        m_exportedVersion = m_version;
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
            ::fwGui::dialog::SelectorDialog dialogVersion;

            dialogVersion.setTitle("Archive version");
            dialogVersion.setMessage("Select an archive version");

            dialogVersion.setSelections(versions);
            std::string result = dialogVersion.show();
            if ( ! result.empty() )
            {
                m_exportedVersion = result;
            }
            return ! result.empty();
        }
    }
    else
    {
        m_exportedVersion = m_version;
        return true;
    }
}

//-----------------------------------------------------------------------------

void SWriter::updating() throw(::fwTools::Failed)
{
    if( this->hasLocationDefined() )
    {
        ::fwData::Object::sptr obj = this->getObject< ::fwData::Object >();

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        try
        {
            const ::boost::filesystem::path& requestedFilePath = this->getFile();
            ::boost::filesystem::path filePath = requestedFilePath;

            if( ::boost::filesystem::exists( requestedFilePath ) )
            {
                FW_RAISE_IF( "can't write to : " << requestedFilePath << ", it is a directory.",
                             ::boost::filesystem::is_directory(requestedFilePath)
                             );

                filePath.replace_extension("%%%%%%" + filePath.extension().string() );
                filePath = ::boost::filesystem::unique_path(filePath);
            }


            const ::boost::filesystem::path folderPath = filePath.parent_path();
            const ::boost::filesystem::path filename = filePath.filename();
            std::string extension = ::boost::filesystem::extension(filePath);

            FW_RAISE_IF( "Extension is empty", extension.empty() );

            // Mutex data lock
            ::fwDataCamp::visitor::RecursiveLock recursiveLock (obj);

            // Convert data to atom
            ::fwAtoms::Object::sptr atom = ::fwAtomConversion::convert(obj);

            // Path atom
            atom->setMetaInfo("context", m_context);
            atom->setMetaInfo("version_name", m_version);
            if( m_useAtomsPatcher )
            {
                ::fwAtomsPatch::PatchingManager globalPatcher( atom );
                atom = globalPatcher.transformTo( m_exportedVersion );
            }

            FW_RAISE_IF("The file extension '" << extension << "' is not managed",
                    m_allowedExts.find(extension) == m_allowedExts.end());

            if(m_customExts.find(extension) != m_customExts.end())
            {
                extension = "." + m_customExts[extension];
            }

            if (extension == ".hdf5")
            {
                ::fwAtomsHdf5IO::Writer(atom).write( filePath );
            }
            else
            {
                // Write atom
                ::fwZip::IWriteArchive::sptr writeArchive;
                ::fwAtomsBoostIO::FormatType format;
                ::boost::filesystem::path archiveRootName;
                if ( extension == ".json" )
                {
                    writeArchive = ::fwZip::WriteDirArchive::New(folderPath.string());
                    archiveRootName = filename;
                    format = ::fwAtomsBoostIO::JSON;
                }
                else if ( extension == ".jsonz" )
                {
                    if ( ::boost::filesystem::exists( filePath ) )
                    {
                        ::boost::filesystem::remove( filePath );
                    }
                    writeArchive = ::fwZip::WriteZipArchive::New(filePath.string());
                    archiveRootName = "root.json";
                    format = ::fwAtomsBoostIO::JSON;
                }
                else if ( extension == ".xml" )
                {
                    writeArchive = ::fwZip::WriteDirArchive::New(folderPath.string());
                    archiveRootName = filename;
                    format = ::fwAtomsBoostIO::XML;
                }
                else if ( extension == ".xmlz" )
                {
                    if ( ::boost::filesystem::exists( filePath ) )
                    {
                        ::boost::filesystem::remove( filePath );
                    }
                    writeArchive = ::fwZip::WriteZipArchive::New(filePath.string());
                    archiveRootName = "root.xml";
                    format = ::fwAtomsBoostIO::XML;
                }
                else
                {
                    FW_RAISE( "This file extension '" << extension << "' is not managed" );
                }

                ::fwAtomsBoostIO::Writer(atom).write( writeArchive, archiveRootName, format );
                writeArchive.reset();
            }

            if (filePath != requestedFilePath)
            {
                ::boost::filesystem::rename(filePath, requestedFilePath);
            }
        }
        catch( std::exception & e )
        {
            OSLM_ERROR( e.what() );
            ::fwGui::dialog::MessageDialog::showMessageDialog("Medical data writer failed",
                    e.what(),
                    ::fwGui::dialog::MessageDialog::CRITICAL);
        }
        catch( ... )
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Medical data writer failed",
                    "Writing process aborted",
                    ::fwGui::dialog::MessageDialog::CRITICAL);
        }
        cursor.setDefaultCursor();
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType SWriter::getIOPathType() const
{
    return ::io::FILE;
}

//-----------------------------------------------------------------------------

void SWriter::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    if( ! m_useAtomsPatcher || versionSelection() )
    {
        ::fwGui::dialog::LocationDialog dialogFile;
        dialogFile.setTitle("Enter file name");
        dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
        dialogFile.setType(::fwGui::dialog::LocationDialog::SINGLE_FILE);

        dialogFile.addFilter("Medical data", "*" + ::boost::algorithm::join(m_allowedExts, " *"));

        BOOST_FOREACH(const std::string& ext, m_allowedExts)
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

}

//-----------------------------------------------------------------------------

} // namespace ioAtoms

