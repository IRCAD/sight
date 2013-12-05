/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOATOMS_SREADER_HPP__
#define __IOATOMS_SREADER_HPP__

#include <map>
#include <set>

#include <io/IReader.hpp>

#include "ioAtoms/config.hpp"

namespace fwMemory
{
    class IPolicy;
}

namespace ioAtoms
{

/**
 * @brief Atoms reader. Service to load data from Atoms format
 * @class SReader
 * @date 2013
 */
class IOATOMS_CLASS_API SReader : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro( (SReader)(::io::IReader) );

    /// Does nothing
    SReader();

    /// Does nothing
    virtual ~SReader() throw() {};

    /// Propose to choose a medical data file (*.json,*.jsonz,*.xml or *.xmlz)
    IOATOMS_API void configureWithIHM();

    /// Maps file extension to format name.
    typedef std::map< std::string, std::string > FileExtension2NameType;

    /// Managed file extensions
    static const FileExtension2NameType s_EXTENSIONS;

protected:

    /// Does nothing
    IOATOMS_API void starting() throw(::fwTools::Failed);

    /// Does nothing
    IOATOMS_API void stopping() throw(::fwTools::Failed);

    /**
     * @brief Configures the reader.
     * @verbatim
     <config>
        <inject>ReadData</inject>
        <uuidPolicy>Strict|Change|Reuse</uuidPolicy>
        <patcher context="..." version="..." />
        <extensions>
            <extension>.xml</extension>
            <extension>.xmlz</extension>
            ...
        </extensions>
     </config>
     * @endverbatim
     *
     * extensions : defines allowed extensions 
     *
     * @see ::io::IReader
     * @throw ::fwTools::Failed
     */
    IOATOMS_API void configuring() throw(::fwTools::Failed);

    /**
     * @brief Tests file extension, applies the good atom reader, and converts atom in fwData::Composite
     * @note  Before reading, set dump policy to 'barrier dump' if policy is 'never dump', then reset old policy.
     */
    IOATOMS_API void updating() throw(::fwTools::Failed);

    /// Returns managed path type, here service manages only single file
    IOATOMS_API ::io::IOPathType getIOPathType() const;

private:

    /// Notify modification on associated object if reading succeeded
    void notificationOfUpdate();

    /// Initial dump policy
    SPTR(::fwMemory::IPolicy) m_oldPolicy;

    /// fwAtomsConversion uuid policy
    std::string m_inject;

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
};

} // namespace ioAtoms

#endif // __IOATOMS_SREADER_HPP__

