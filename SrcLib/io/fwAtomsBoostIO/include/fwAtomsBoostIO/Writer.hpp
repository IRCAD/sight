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

    /// Defines writer version.
    FWATOMSBOOSTIO_API static const std::string s_VERSION;

    /// Defines key to retrieve fwAtoms version from file.
    FWATOMSBOOSTIO_API static const std::string s_ATOMS_VERSION_KEY;

    /// Defines key to retrieve writer version from file.
    FWATOMSBOOSTIO_API static const std::string s_WRITER_VERSION_KEY;

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

