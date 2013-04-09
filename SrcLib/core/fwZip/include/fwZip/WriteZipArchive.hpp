#ifndef __FWZIP_WRITEZIPARCHIVE_HPP__
#define __FWZIP_WRITEZIPARCHIVE_HPP__

#include <ostream>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/categories.hpp>  // sink_tag

#include <boost/filesystem/path.hpp>

#include <boost/make_shared.hpp>

#include <fwCore/macros.hpp>

#include "fwZip/config.hpp"
#include "fwZip/IWriteArchive.hpp"


namespace fwZip
{

class ZipSink
{
public:
    typedef char char_type;
    typedef ::boost::iostreams::sink_tag  category;

    ZipSink( void * zd ) : m_zipDescriptor(zd){}

    std::streamsize write(const char* s, std::streamsize n);

protected:
    void *m_zipDescriptor;
};



class FWZIP_CLASS_API WriteZipArchive : public IWriteArchive
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((WriteZipArchive)(IWriteArchive),
                                           ((( const ::boost::filesystem::path& ))),
                                           ::boost::make_shared<WriteZipArchive> );

    FWZIP_API WriteZipArchive( const ::boost::filesystem::path &archive );
    FWZIP_API ~WriteZipArchive();

    FWZIP_API std::ostream& createFile(const ::boost::filesystem::path &path);

    FWZIP_API bool createDir(const ::boost::filesystem::path &path);

protected:

    FWZIP_API int openFile(const ::boost::filesystem::path &path);
    FWZIP_API void closeFile();

    ::boost::filesystem::path m_archive;
    void *m_zipDescriptor;
    ::boost::iostreams::stream_buffer< ZipSink > m_streambuf;
    std::ostream m_ostream;
};

}


#endif /* __FWZIP_WRITEZIPARCHIVE_HPP__ */

