#ifndef __FWZIP_WRITEARCHIVE_HPP__
#define __FWZIP_WRITEARCHIVE_HPP__

#include <ostream>
#include <boost/filesystem/path.hpp>

#include <fwCore/macros.hpp>

namespace fwZip
{

class IWriteArchive
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro((IWriteArchive));

    virtual ~IWriteArchive(){};

    virtual std::ostream& createFile(const ::boost::filesystem::path &path) = 0;

    virtual void putFile(const ::boost::filesystem::path &sourceFile, const ::boost::filesystem::path &path) = 0;

    virtual bool createDir(const ::boost::filesystem::path &path) = 0;

    virtual const ::boost::filesystem::path getArchivePath() const = 0;
};

}


#endif /* __FWZIP_WRITEARCHIVE_HPP__ */

