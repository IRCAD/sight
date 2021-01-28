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

#pragma once

#include "modules/io/atoms/config.hpp"

#include <core/com/Signal.hpp>
#include <core/jobs/IJob.hpp>

#include <io/atoms/Writer.hpp>
#include <io/base/services/IWriter.hpp>

#include <ui/base/dialog/LocationDialog.hpp>

#include <set>

namespace sight::modules::io::atoms
{

/**
 * @brief Atoms writer. Service to write an fwData medical data converted in fwAtoms.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(core::jobs::IJob)): emitted to display a progress bar while the image is written (it should be
 * connected to a SJobBar).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="::modules::io::atoms::SWriter">
        <in key="data" uid="..." />
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
            <extension label="Medical workspace">.mw</extension>
        </extensions>
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b data [data::Object]: object to write.
 *
 * @subsection Configuration Configuration
 * - \b patcher defines the atom patcher to use to convert the atoms (see io::atoms::patch::PatchingManager)
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
 *      The attribute label (mandatory) allows to display a label in front of extension when the file dialog is
 *      shown.
 *
 * @see io::base::services::IReader
 */
class MODULE_IO_ATOMS_CLASS_API SWriter : public sight::io::base::services::IWriter
{

public:

    fwCoreServiceMacro(SWriter, sight::io::base::services::IWriter)

    /// Signal type for job creation.
    typedef core::com::Signal< void ( core::jobs::IJob::sptr ) > JobCreatedSignalType;

    /// Does nothing
    MODULE_IO_ATOMS_API SWriter();

    /// Does nothing
    MODULE_IO_ATOMS_API virtual ~SWriter() noexcept
    {
    }

    /**
     * @brief Propose to create a medical data file
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    MODULE_IO_ATOMS_API void configureWithIHM() override;

    /// Propose to create a medical data file
    MODULE_IO_ATOMS_API void openLocationDialog() override;

protected:

    /// Maps file extension to format name.
    typedef std::map< std::string, std::string > FileExtension2NameType;

    /// Parse the configuration
    MODULE_IO_ATOMS_API void configuring() override;

    /// Does nothing
    MODULE_IO_ATOMS_API void starting() override;

    /// Does nothing
    MODULE_IO_ATOMS_API void stopping() override;

    /**
     * @brief Convert fwData to fwAtoms, and apply the writer chosen from file extension
     * @note Data is locked (mutex) recursively during writing
     */
    MODULE_IO_ATOMS_API void updating() override;

    /// Returns managed path type, here service manages only single file
    MODULE_IO_ATOMS_API sight::io::base::services::IOPathType getIOPathType() const override;

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

} // namespace sight::modules::io::atoms
