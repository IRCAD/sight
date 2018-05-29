/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioAtoms/config.hpp"

#include <fwCom/Signal.hpp>

#include <fwIO/IReader.hpp>

#include <fwJobs/IJob.hpp>

#include <map>
#include <set>

namespace fwMemory
{
class IPolicy;
}

namespace ioAtoms
{

/**
 * @brief Atoms reader. Service to load data from Atoms format
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(::fwJobs::IJob)): emitted when the image is loading to display a progress bar.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioAtoms::SReader">
       <inout key="data" uid="..." />
       <uuidPolicy>Strict|Change</uuidPolicy>
       <patcher context="..." version="..." />
       <filter>...</filter>
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
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [::fwData::Object]: object to read.
 * @subsection Configuration Configuration
 * - \b uuidPolicy(optional, default ChangePolicy): defines the policy for atoms conversion. 'ChangePolicy' changes the
 *      object uuid only if it already exists in the application. 'StrictPolicy' keeps the object uuid and throws an
 *      exception if the loaded uuid already exists. 'ReusePolicy' uses the existing object in the application with the
 *      uuid (it is not yet supported, the reader should be in output mode).
 * - \b patcher(optional): defines the atom patcher to use to convert the atoms (see ::fwAtomsPatch::PatchingManager)
 *    - \b context (optional, default=MedicalData): context of the atom patcher
 *    - \b version (optional, default=version of MedicalData): version of the atom patcher, by default it uses the
 *         current version of the MedicalData context.
 * - \b filter(optional): filter applied on the read atom before the conversion to the object. (see
 *      ::fwAtomsFilter::IFilter)
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
class IOATOMS_CLASS_API SReader : public ::fwIO::IReader
{

public:

    /// Signal type for job creation.
    typedef ::fwCom::Signal< void ( ::fwJobs::IJob::sptr ) > JobCreatedSignalType;

    fwCoreServiceClassDefinitionsMacro( (SReader)(::fwIO::IReader) );

    /// Does nothing
    SReader();

    /// Does nothing
    virtual ~SReader() noexcept
    {
    }

    /// Propose to choose a medical data file (*.json,*.jsonz,*.xml or *.xmlz)
    IOATOMS_API void configureWithIHM() override;

    /// Maps file extension to format name.
    typedef std::map< std::string, std::string > FileExtension2NameType;

    /// Managed file extensions
    static const FileExtension2NameType s_EXTENSIONS;

protected:

    /// Does nothing
    IOATOMS_API void starting() override;

    /// Does nothing
    IOATOMS_API void stopping() override;

    /// Parse the configuration
    IOATOMS_API void configuring() override;

    /**
     * @brief Tests file extension, applies the good atom reader, and converts atom in fwData::Composite
     */
    IOATOMS_API void updating() override;

    /// Returns managed path type, here service manages only single file
    IOATOMS_API ::fwIO::IOPathType getIOPathType() const override;

private:

    /// Notify modification on associated object if reading succeeded
    void notificationOfUpdate();

    /// fwAtomsConversion uuid policy
    std::string m_uuidPolicy;

    /// To enable atom patcher
    bool m_useAtomsPatcher;

    /// Defines context of data
    std::string m_context;

    /// Current version of format
    std::string m_version;

    /// Allowed file extensions
    std::set< std::string > m_allowedExts;

    /// IFilter name used to make an atom compliant with current context
    std::string m_filter;

    /// Maps custom extensions to knwon format.
    FileExtension2NameType m_customExts;

    /// Labels shown in file dialog for each allowed extension
    FileExtension2NameType m_allowedExtLabels;

    /// Signal emitted when job created.
    JobCreatedSignalType::sptr m_sigJobCreated;
};

} // namespace ioAtoms
