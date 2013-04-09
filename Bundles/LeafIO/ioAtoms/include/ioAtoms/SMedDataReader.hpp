/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOATOMS_SMEDDATAREADER_HPP__
#define __IOATOMS_SMEDDATAREADER_HPP__

#include <io/IReader.hpp>

#include "ioAtoms/config.hpp"
#include "ioAtoms/SMedDataWriter.hpp"

namespace ioAtoms
{

/**
 * @brief Atoms reader. Service to load an fwAtoms medical data and convert it in fwData.
 * @class SMedDataReader
 * @date 2013
 */
class IOATOMS_CLASS_API SMedDataReader : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro( (SMedDataReader)(::io::IReader) );

    /// Does nothing
    virtual ~SMedDataReader() throw() {};

    /// Propose to choose a medical data file (*.json,*.jsonz,*.xml or *.xmlz)
    IOATOMS_API void configureWithIHM();

protected:

    /// Does nothing
    IOATOMS_API void starting() throw(::fwTools::Failed);

    /// Does nothing
    IOATOMS_API void stopping() throw(::fwTools::Failed);

    /// Tests file extension, applies the good atom reader, and converts atom in fwData::Composite
    IOATOMS_API void updating() throw(::fwTools::Failed);

    /// Returns managed path type, here service manages only single file
    IOATOMS_API ::io::IOPathType getIOPathType() const;

    /// Notify modification on associated object if reading process is a success
    void notificationOfUpdate();
};

} // namespace ioAtoms

#endif // __IOATOMS_SMEDDATAREADER_HPP__

