/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSHDF5IO_WRITER_HPP__
#define __FWATOMSHDF5IO_WRITER_HPP__

#include <sstream>

#include <boost/filesystem/path.hpp>

#include <cpp/H5Cpp.h>

#include <fwCore/macros.hpp>

#include "fwAtomsHdf5IO/config.hpp"

namespace fwAtoms
{
class Base;
}

namespace fwAtomsHdf5IO
{

/**
 * @brief This writer parses a fwAtoms to save it in a HDF5 file.
 * @class Writer
 */
class FWATOMSHDF5IO_CLASS_API Writer
{

public:

    /// Defines writer version.
    FWATOMSHDF5IO_API static const std::string s_VERSION;

    /// Defines key to retrieve fwAtoms version from file.
    FWATOMSHDF5IO_API static const std::string s_ATOMS_VERSION_KEY;

    /// Defines key to retrieve writer version from file.
    FWATOMSHDF5IO_API static const std::string s_WRITER_VERSION_KEY;

    /// Constructor : initializes the writer.
    FWATOMSHDF5IO_API Writer( SPTR(::fwAtoms::Base)atom );

    /**
     * @brief Visits the current atom to save it.
     * @param path path of the HDF5 file to create.
     */
    FWATOMSHDF5IO_API void write(const ::boost::filesystem::path &path);

protected:

    /// current atom to save.
    SPTR(::fwAtoms::Base) m_atom;

};

}

#endif /* __FWATOMSHDF5IO_WRITER_HPP__ */

