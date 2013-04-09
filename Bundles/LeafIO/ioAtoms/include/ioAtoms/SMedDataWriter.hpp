/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOATOMS_SMEDDATAWRITER_HPP__
#define __IOATOMS_SMEDDATAWRITER_HPP__

#include <io/IWriter.hpp>

#include <fwAtomsBoostIO/Writer.hpp>

#include "ioAtoms/config.hpp"

namespace ioAtoms
{

/**
 * @brief Atoms writer. Service to write an fwData medical data converted in fwAtoms.
 * @class SMedDataWriter
 * @date 2013
 */
class IOATOMS_CLASS_API SMedDataWriter : public ::io::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro( (SMedDataWriter)(::io::IWriter) );

    /// Does nothing
    IOATOMS_API  virtual ~SMedDataWriter() throw() {};

    /// Propose to create a medical data file (*.json,*.jsonz,*.xml or *.xmlz)
    IOATOMS_API void configureWithIHM();

protected:

    /// Does nothing
    IOATOMS_API void starting() throw(::fwTools::Failed);

    /// Does nothing
    IOATOMS_API void stopping() throw(::fwTools::Failed);

    /**
     * @brief Convert fwData medical data in fwAtoms, and apply the good writer ( chosen from file extension )
     *
     * Actually, this method creates a new composite representing a medical workspace : add new patient db, new
     * planning db and new processing db. Shallow copy these structures to avoid UUID conflict if reader is used
     * in the same application instance.
     *
     * @note Data is lock (mutex) recursively during writing
     */
    IOATOMS_API void updating() throw(::fwTools::Failed);

    /// Returns managed path type, here service manages only single file
    IOATOMS_API ::io::IOPathType getIOPathType() const;
};

} // namespace ioAtoms

#endif // __IOATOMS_SMEDDATAWRITER_HPP__

