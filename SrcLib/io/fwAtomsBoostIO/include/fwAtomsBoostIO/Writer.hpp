#ifndef __FWATOMSBOOSTIO_WRITER_HPP__
#define __FWATOMSBOOSTIO_WRITER_HPP__

#include <sstream>

#include <fwCore/macros.hpp>
#include <boost/filesystem/path.hpp>

#include "fwAtomsBoostIO/config.hpp"

namespace fwAtoms
{
    class Base;
}
namespace fwZip
{
    class IWriteArchive;
}

namespace fwAtomsBoostIO
{

class FWATOMSBOOSTIO_CLASS_API Writer
{

public:

    typedef enum
    {
        JSON,
        XML,
    } FormatType;

    Writer( SPTR(::fwAtoms::Base) atom ) : m_atom(atom) {}

    FWATOMSBOOSTIO_API void write( SPTR(::fwZip::IWriteArchive) archive,
                                   const ::boost::filesystem::path& rootFilename = "root.json",
                                   FormatType format = JSON ) const;

protected:

    SPTR(::fwAtoms::Base) m_atom;

};

}

#endif /* __FWATOMSBOOSTIO_WRITER_HPP__ */

