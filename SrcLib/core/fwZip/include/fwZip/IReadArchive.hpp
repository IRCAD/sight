/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_READARCHIVE_HPP__
#define __FWZIP_READARCHIVE_HPP__

#include <istream>
#include <boost/filesystem/path.hpp>

#include <fwCore/macros.hpp>

namespace fwZip
{

/**
 * @brief   This interface defines functions to read a file in an archive.
 * @class   IReadArchive.
 */
class IReadArchive
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro((IReadArchive));

    virtual ~IReadArchive(){};

    /**
     * @brief Returns input stream for the file in current archive.
     * @param path file in archive.
     * @return input stream from request file.
     */
    virtual SPTR(std::istream) getFile(const ::boost::filesystem::path &path) = 0;

    /**
     * @brief Returns archive path.
     */
    virtual const ::boost::filesystem::path getArchivePath() const = 0;

    virtual IReadArchive::sptr clone() const = 0 ;
};

}


#endif /* __FWZIP_READARCHIVE_HPP__ */

