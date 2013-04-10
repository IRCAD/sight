/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWZIP_READDIRARCHIVE_HPP__
#define __FWZIP_READDIRARCHIVE_HPP__

#include <fstream>

#include <boost/filesystem/path.hpp>

#include <boost/make_shared.hpp>

#include <fwCore/macros.hpp>

#include "fwZip/config.hpp"
#include "fwZip/IReadArchive.hpp"


namespace fwZip
{

class FWZIP_CLASS_API ReadDirArchive : public IReadArchive
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ReadDirArchive)(IReadArchive),
                                           ((( const ::boost::filesystem::path& ))),
                                           ::boost::make_shared< ReadDirArchive >);

    FWZIP_API ReadDirArchive( const ::boost::filesystem::path &archive );

    FWZIP_API ~ReadDirArchive();

    FWZIP_API std::istream& getFile(const ::boost::filesystem::path &path);

    FWZIP_API const ::boost::filesystem::path getArchivePath() const;

protected:

    FWZIP_API bool exists(const ::boost::filesystem::path &path);

    FWZIP_API void closeFile();

    std::ifstream m_infile;
    ::boost::filesystem::path m_archive;
};

}


#endif /* __FWZIP_READDIRARCHIVE_HPP__ */

