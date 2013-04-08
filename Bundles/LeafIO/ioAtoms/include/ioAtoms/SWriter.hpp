/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOATOMS_SWRITER_HPP__
#define __IOATOMS_SWRITER_HPP__

#include <io/IWriter.hpp>

#include "ioAtoms/config.hpp"

namespace fwServices
{
    class ObjectMsg;
}

namespace ioAtoms
{

/**
 * @brief Atoms writer.
 * @class SWriter
 * @date 2013
 */
class IOATOMS_CLASS_API SWriter : public ::io::IWriter
{

public:
    virtual ~SWriter() throw() {};

    fwCoreServiceClassDefinitionsMacro( (SWriter)(::io::IWriter) );

    /// Override
    IOATOMS_API void configureWithIHM();

protected:

    /**
     * @name Typedefs
     * @{ */
    typedef ::fwAtomsBoostIO::Writer::FormatType FormatType;
    typedef std::map< std::string, FormatType > Ext2FormatType;
    typedef std::map< std::string, std::string > FiltersType;
    /**  @} */

    /// Override
    IOATOMS_API void starting() throw(::fwTools::Failed);

    /// Override
    IOATOMS_API void stopping() throw(::fwTools::Failed);

    /// Override
    IOATOMS_API void updating() throw(::fwTools::Failed);

    /// Override
    IOATOMS_API ::io::IOPathType getIOPathType() const;

    /// Override
    IOATOMS_API void info(std::ostream & _stream);

    /// Specify if data must be saved using compression.
    bool m_archive;

    /// Maps an extension to a format type.
    Ext2FormatType m_formatsMap;

    /// Maps a filter name to its wildcard.
    FiltersType m_filters;
};

} // namespace ioAtoms

#endif // __IOATOMS_SWRITER_HPP__

