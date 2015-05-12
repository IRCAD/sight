/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSHDF5IO_READER_HPP__
#define __FWATOMSHDF5IO_READER_HPP__

#include <boost/filesystem/path.hpp>

#include <cpp/H5Cpp.h>

#include <fwCore/macros.hpp>

#include "fwAtomsHdf5IO/config.hpp"

namespace fwAtoms
{
class Base;
}
namespace fwZip
{
class IReadArchive;
}

namespace fwAtomsHdf5IO
{

/**
 * @brief This reader parses a HDF5 file and create the associated atom.
 * @class Reader
 */
class FWATOMSHDF5IO_CLASS_API Reader
{

public:

    /**
     * @brief Visits the HDF5 file to create the associated atoms.
     * @param path path of the HDF5 file.
     */
    FWATOMSHDF5IO_API SPTR(::fwAtoms::Base) read(const ::boost::filesystem::path &path);

};

}

#endif /* __FWATOMSHDF5IO_READER_HPP__ */

