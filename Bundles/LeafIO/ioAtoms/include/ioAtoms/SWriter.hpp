/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOATOMS_SWRITER_HPP__
#define __IOATOMS_SWRITER_HPP__

#include "ioAtoms/config.hpp"

#include <io/IWriter.hpp>

#include <fwCom/Signal.hpp>

#include <fwJobs/IJob.hpp>

#include <fwAtomsBoostIO/Writer.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <set>

namespace ioAtoms
{

/**
 * @brief Atoms writer. Service to write an fwData medical data converted in fwAtoms.
 * @class SWriter
 * @date 2013
 */
class IOATOMS_CLASS_API SWriter : public ::io::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro( (SWriter)(::io::IWriter) );

    /// Signal type for job creation.
    typedef ::fwCom::Signal< void ( ::fwJobs::IJob::sptr ) > JobCreatedSignalType;

    /// Does nothing
    IOATOMS_API SWriter();

    /// Does nothing
    IOATOMS_API virtual ~SWriter() throw()
    {
    }

    /// Propose to create a medical data file
    IOATOMS_API void configureWithIHM();

protected:

    /// Maps file extension to format name.
    typedef std::map< std::string, std::string > FileExtension2NameType;

    /**
     * @brief Configures the writer
       @code{.xml}
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
       @endcode
     *
     * archive : defines custom file extensions. The file to be saved with an extension given in 'archive' tag will be
     * written with the given backend in archive tag (the 'backend' attribute is mandatory). Extensions must begin with
     * '.'.
     * Available 'backend' values are json, xml, jsonz and xmlz.
     *
     * extensions : defines available extensions displayed in dialog to save file. If the 'extensions' is empty or not
     * specified, all the extensions (.json, .xml, .jsonz, .xmlz extensions and custom extensions) are available.
     * The attribute label (not mandatory) allows to display a label in front of extension when the file dialog is
     * shown.
     *
     * @see ::io::IWriter
     * @throw ::fwTools::Failed
     */
    IOATOMS_API void configuring() throw(::fwTools::Failed);

    /// Does nothing
    IOATOMS_API void starting() throw(::fwTools::Failed);

    /// Does nothing
    IOATOMS_API void stopping() throw(::fwTools::Failed);

    /**
     * @brief Convert fwData to fwAtoms, and apply the writer chosen from file extension
     * @note Data is locked (mutex) recursively during writing
     */
    IOATOMS_API void updating() throw(::fwTools::Failed);

    /// Returns managed path type, here service manages only single file
    IOATOMS_API ::io::IOPathType getIOPathType() const;

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

#endif // __IOATOMS_SWRITER_HPP__

