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

namespace fwAtomsBoostIO
{

class FWATOMSBOOSTIO_CLASS_API Writer
{

public:

    typedef enum
    {
        JSON,
        XML,
        JSON_Z,
        XML_Z
    } FormatType;

    Writer( SPTR(fwAtoms::Base) atom ) : m_atom(atom) {}

    FWATOMSBOOSTIO_API void write( const ::boost::filesystem::path &file, FormatType format );

    FWATOMSBOOSTIO_API void write( std::stringstream &sstr, FormatType format );

protected:

    SPTR(::fwAtoms::Base) m_atom;

};

}

#endif /* __FWATOMSBOOSTIO_WRITER_HPP__ */

