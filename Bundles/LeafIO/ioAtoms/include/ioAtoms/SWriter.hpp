/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOATOMS_SWRITER_HPP__
#define __IOATOMS_SWRITER_HPP__

#include <io/IWriter.hpp>

#include <fwAtomsBoostIO/Writer.hpp>

#include "ioAtoms/config.hpp"

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

    /// Does nothing
    IOATOMS_API  virtual ~SWriter() throw() {};

    /// Propose to create a medical data file (*.json,*.jsonz,*.xml, *.xmlz or *.hdf5)
    IOATOMS_API void configureWithIHM();

protected:

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
};

} // namespace ioAtoms

#endif // __IOATOMS_SWRITER_HPP__

