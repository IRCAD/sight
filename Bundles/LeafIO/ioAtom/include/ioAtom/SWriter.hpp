/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOATOM_SWRITER_HPP__
#define __IOATOM_SWRITER_HPP__

#include <io/IWriter.hpp>

#include "ioAtom/config.hpp"

namespace fwServices
{
    class ObjectMsg;
}

namespace ioAtom
{

/**
 * @brief Atom writer.
 * @class SWriter
 * @date 2013
 */
class IOATOM_CLASS_API SWriter : public ::io::IWriter
{

public:
    virtual ~SWriter() throw() {};

    fwCoreServiceClassDefinitionsMacro( (SWriter)(::io::IWriter) );

    /// Override
    IOATOM_API void configureWithIHM();

protected:

    /**
     * @name Typedefs
     * @{ */
    typedef ::fwAtomsBoostIO::Writer::FormatType FormatType;
    typedef std::map< std::string, FormatType > Ext2FormatType;
    typedef std::map< std::string, std::string > FiltersType;
    /**  @} */

    /// Override
    IOATOM_API void starting() throw(::fwTools::Failed);

    /// Override
    IOATOM_API void stopping() throw(::fwTools::Failed);

    /// Override
    IOATOM_API void updating() throw(::fwTools::Failed);

    /// Override
    IOATOM_API ::io::IOPathType getIOPathType() const;

    /// Override
    IOATOM_API void info(std::ostream & _stream);

    /// Specify if data must be saved using compression.
    bool m_archive;

    /// Maps an extension to a format type.
    Ext2FormatType m_formatsMap;

    /// Maps a filter name to its wildcard.
    FiltersType m_filters;
};

} // namespace ioAtom

#endif // __IOATOM_SWRITER_HPP__

