/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOATOMS_SREADER_HPP__
#define __IOATOMS_SREADER_HPP__

#include <io/IReader.hpp>

#include "ioAtoms/config.hpp"

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
    virtual ~SReader() throw() {};

    /// Propose to choose a medical data file (*.json,*.jsonz,*.xml or *.xmlz)
    IOATOMS_API void configureWithIHM();

protected:

    /// Does nothing
    IOATOMS_API void starting() throw(::fwTools::Failed);

    /// Does nothing
    IOATOMS_API void stopping() throw(::fwTools::Failed);

    /**
    * @brief
    * @verbatim
        <config>
            <inject>ReadData</inject>
            <uuidPolicy>Strict|Change|Reuse</uuidPolicy>
            ...
        </config>
    * @endverbatim
    * @see ::io::IReader
    * @throw ::fwTools::Failed
    */
    void configuring() throw(::fwTools::Failed);

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

    /**
     * @brief Changes dump policy to barrier dump if needed.
     * Dump policy is changed only if the current policy is 'never dump'
     * Store old policy in m_oldPolicy to set it back after reading.
     */
    void setBarrierDumpPolicy();

    /// Resets dump policy to m_oldPolicy
    void resetDumpPolicy();

    /// Initial dump policy
    ::fwMemory::IPolicy::sptr m_oldPolicy;

    /// fwAtomsConversion uuid policy
    std::string m_inject;

    /// fwAtomsConversion uuid policy
    std::string m_uuidPolicy;
};

} // namespace ioAtoms

#endif // __IOATOMS_SREADER_HPP__

