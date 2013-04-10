#ifndef __FWZIP_READARCHIVE_HPP__
#define __FWZIP_READARCHIVE_HPP__

#include <istream>
#include <boost/filesystem/path.hpp>

#include <fwCore/macros.hpp>

namespace fwZip
{

class IReadArchive
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro((IReadArchive));

    virtual ~IReadArchive(){};

    virtual std::istream& getFile(const ::boost::filesystem::path &path) = 0;

    virtual const ::boost::filesystem::path getArchivePath() const = 0;
};

}


#endif /* __FWZIP_READARCHIVE_HPP__ */

