#ifndef __FWZIP_READZIPARCHIVE_HPP__
#define __FWZIP_READZIPARCHIVE_HPP__

#include <istream>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/categories.hpp>  // source_tag

#include <boost/filesystem/path.hpp>

#include <boost/make_shared.hpp>

#include <fwCore/macros.hpp>

#include "fwZip/config.hpp"
#include "fwZip/IReadArchive.hpp"


namespace fwZip
{

class ZipSource
{
public:
    typedef char char_type;
    typedef ::boost::iostreams::source_tag  category;

    ZipSource( void * zd ) : m_zipDescriptor(zd){}

    std::streamsize read(char* s, std::streamsize n);

protected:
    void *m_zipDescriptor;
};



class FWZIP_CLASS_API ReadZipArchive : public IReadArchive
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ReadZipArchive)(IReadArchive),
                                           ((( const ::boost::filesystem::path& ))),
                                           ::boost::make_shared< ReadZipArchive >);

    FWZIP_API ReadZipArchive( const ::boost::filesystem::path &archive );
    FWZIP_API ~ReadZipArchive();

    FWZIP_API std::istream& getFile(const ::boost::filesystem::path &path);

    FWZIP_API const ::boost::filesystem::path getArchivePath() const;

protected:

    FWZIP_API bool exists(const ::boost::filesystem::path &path);

    ::boost::filesystem::path m_archive;
    void *m_zipDescriptor;
    ::boost::iostreams::stream_buffer< ZipSource > m_streambuf;
    std::istream m_istream;
};

}


#endif /* __FWZIP_READZIPARCHIVE_HPP__ */

