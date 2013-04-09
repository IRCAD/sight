/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOATOMS_SREADER_HPP__
#define __IOATOMS_SREADER_HPP__

#include <io/IReader.hpp>

#include "ioAtoms/config.hpp"
#include "ioAtoms/SWriter.hpp"

namespace ioAtoms
{

/**
 * @brief Atoms reader.
 * @class SReader
 * @date 2013
 */
class IOATOMS_CLASS_API SReader : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro( (SReader)(::io::IReader) );

    virtual ~SReader() throw() {};

    /// Override
    IOATOMS_API void configureWithIHM();

protected:

    /// Override
    IOATOMS_API void starting() throw(::fwTools::Failed);

    /// Override
    IOATOMS_API void stopping() throw(::fwTools::Failed);

    /// Override
    IOATOMS_API void updating() throw(::fwTools::Failed);

    /// Override
    IOATOMS_API ::io::IOPathType getIOPathType() const;

    /// notify modification on associated object if reading process is a success
    void notificationOfUpdate();

    /// Specify if data must be saved using compression.
    bool m_archive;

    /// Maps an extension to a format type.
    SWriter::Ext2FormatType m_formatsMap;

    /// Maps a filter name to its wildcard.
    SWriter::FiltersType m_filters;
};

} // namespace ioAtoms

#endif // __IOATOMS_SREADER_HPP__

