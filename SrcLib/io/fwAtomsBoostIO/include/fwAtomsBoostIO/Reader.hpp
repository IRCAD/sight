#ifndef __FWATOMSBOOSTIO_READER_HPP__
#define __FWATOMSBOOSTIO_READER_HPP__

#include <fwCore/macros.hpp>
#include <boost/filesystem/path.hpp>

#include "fwAtomsBoostIO/config.hpp"

namespace fwAtoms
{
    class Base;
}

namespace fwAtomsBoostIO
{

class FWATOMSBOOSTIO_CLASS_API Reader
{

public:

    FWATOMSBOOSTIO_API SPTR(::fwAtoms::Base) read( const ::boost::filesystem::path &file );
    FWATOMSBOOSTIO_API SPTR(::fwAtoms::Base) read( std::stringstream &sstr );

protected:

    SPTR(::fwAtoms::Base) m_atom;

};

}

#endif /* __FWATOMSBOOSTIO_READER_HPP__ */

