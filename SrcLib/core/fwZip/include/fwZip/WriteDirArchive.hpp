#ifndef __FWZIP_WRITEDIRARCHIVE_HPP__
#define __FWZIP_WRITEDIRARCHIVE_HPP__

#include <fstream>

#include <boost/filesystem/path.hpp>

#include <boost/make_shared.hpp>

#include <fwCore/macros.hpp>

#include "fwZip/config.hpp"
#include "fwZip/IWriteArchive.hpp"


namespace fwZip
{

class FWZIP_CLASS_API WriteDirArchive : public IWriteArchive
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((WriteDirArchive)(IWriteArchive),
                                           ((( const ::boost::filesystem::path& ))),
                                           ::boost::make_shared<WriteDirArchive> );

    FWZIP_API WriteDirArchive( const ::boost::filesystem::path &archive );
    FWZIP_API ~WriteDirArchive();

    FWZIP_API std::ostream& createFile(const ::boost::filesystem::path &path);

    FWZIP_API void putFile(const ::boost::filesystem::path &sourceFile, const ::boost::filesystem::path &path);

    FWZIP_API bool createDir(const ::boost::filesystem::path &path);

    FWZIP_API const ::boost::filesystem::path getArchivePath() const;

protected:

    FWZIP_API void closeFile();

    std::ofstream m_outfile;
    ::boost::filesystem::path m_archive;
};

}


#endif /* __FWZIP_WRITEDIRARCHIVE_HPP__ */

