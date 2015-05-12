/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWVTKIO_VTIIMAGEREADER_HPP__
#define __FWVTKIO_VTIIMAGEREADER_HPP__

#include <boost/filesystem/path.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Image.hpp>

#include "fwVtkIO/config.hpp"

namespace fwVtkIO
{

/**
 * @brief   Read an image.
 * @class   VtiImageReader
 *
 * @date    2009.
 *
 * Read a VTK Image using the VTK lib
 */
class VtiImageReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >,
                       public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >,
                       public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (VtiImageReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >),
        (()),
        ::fwDataIO::reader::factory::New< VtiImageReader >
        );

    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API VtiImageReader(::fwDataIO::reader::IObjectReader::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~VtiImageReader();

    //! @brief Reading operator.
    FWVTKIO_API void read();

    /// @return ".vtk"
    FWVTKIO_API std::string extension();
};

} // namespace fwVtkIO

#endif // __FWVTKIO_VTIIMAGEREADER_HPP__
