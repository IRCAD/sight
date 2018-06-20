/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioAtoms/config.hpp"

#include <fwAtomsBoostIO/Writer.hpp>

#include <fwCom/Signal.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwIO/IWriter.hpp>

#include <fwJobs/IJob.hpp>

#include <set>

namespace ioAtoms
{

/**
 * @brief Atoms writer. Service to write an fwData medical data converted in fwAtoms.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(::fwJobs::IJob)): emitted to display a progress bar while the image is written (it should be
 * connected to a SJobBar).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioAtoms::SWriter">
       <inout key="data" uid="..." />
       <config>
        <patcher context="..." version="..." />

        <archive backend="json">
            <extension>.j</extension>
        </archive>

        <archive backend="jsonz">
            <extension>.vpz</extension>
        </archive>

        <extensions>
            <extension label="XML">.xml</extension>
            <extension label="Zipped XML>.xmlz</extension>
            <extension>.f4s</extension>
            <extension>.j</extension>
            <extension label="Medical workspace">.mw</extension>
            <extension>.vpz</extension>
        </extensions>

       </config>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [::fwData::Object]: object to write.
 * @subsection Configuration Configuration
 * - \b patcher defines the atom patcher to use to convert the atoms (see ::fwAtomsPatch::PatchingManager)
 *    - \b context (optional, default=MedicalData): context of the atom patcher
 *    - \b version (optional, default=version of MedicalData): version of the atom patcher, by default it uses the
 *         current version of the MedicalData context.
 * - \b archive(optional): defines custom file extensions. The file to be read with an extension given in 'archive' tag
 *      will be processed with the given backend in archive tag (the 'backend' attribute is mandatory). Extensions must
 *      begin with '.'. Available 'backend' values are json, xml, jsonz and xmlz.
 *
 * - \b extensions(optional): defines available extensions displayed in dialog to read file. If the 'extensions' is
 *      empty or not specified, all the extensions (.json, .xml, .jsonz, .xmlz extensions and custom extensions) are
 *      available.
 *      The attribute label (not mandatory) allows to display a label in front of extension when the file dialog is
 *      shown.
 *
 * @see ::fwIO::IReader
 */
class IOATOMS_CLASS_API SWriter : public ::fwIO::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro( (SWriter)(::fwIO::IWriter) );

    /// Signal type for job creation.
    typedef ::fwCom::Signal< void ( ::fwJobs::IJob::sptr ) > JobCreatedSignalType;

    /// Does nothing
    IOATOMS_API SWriter();

    /// Does nothing
    IOATOMS_API virtual ~SWriter() noexcept
    {
    }

    /// Propose to create a medical data file
    IOATOMS_API void configureWithIHM() override;

protected:

    /// Maps file extension to format name.
    typedef std::map< std::string, std::string > FileExtension2NameType;

    /// Parse the configuration
    IOATOMS_API void configuring() override;

    /// Does nothing
    IOATOMS_API void starting() override;

    /// Does nothing
    IOATOMS_API void stopping() override;

    /**
     * @brief Convert fwData to fwAtoms, and apply the writer chosen from file extension
     * @note Data is locked (mutex) recursively during writing
     */
    IOATOMS_API void updating() override;

    /// Returns managed path type, here service manages only single file
    IOATOMS_API ::fwIO::IOPathType getIOPathType() const override;

    /// To activate atom patcher
    bool m_useAtomsPatcher;

    /// Selected exported version of atom
    std::string m_exportedVersion;

    /// Defines context of data
    std::string m_context;

    /// Current version of format
    std::string m_version;

    /// Proposes a gui to select available exports (set m_exportedVersion).
    /// Returns false if user cancel the selection.
    bool versionSelection();

    /// Allowed file extensions
    std::set< std::string > m_allowedExts;

    /// Maps custom extensions to known format.
    FileExtension2NameType m_customExts;

    /// Labels shown in file dialog for each allowed extension
    FileExtension2NameType m_allowedExtLabels;

    /// Signal emitted when job created.
    JobCreatedSignalType::sptr m_sigJobCreated;

    /// Extension selected in file dialog
    std::string m_selectedExtension;
};

} // namespace ioAtoms
