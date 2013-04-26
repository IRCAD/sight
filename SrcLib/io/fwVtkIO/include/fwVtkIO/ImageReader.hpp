/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_IMAGEREADER_HPP_
#define _VTKIO_IMAGEREADER_HPP_

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
 * @class   ImageReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Read a VTK Image using the VTK lib
 */
class ImageReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((ImageReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >),
                                           (()),
                                           ::fwDataIO::reader::factory::New< ImageReader >
                                          );
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    VTKIO_API ImageReader(::fwDataIO::reader::IObjectReader::Key key);

    //! @brief Destructor.
    VTKIO_API ~ImageReader();

    //! @brief Reading operator.
    VTKIO_API void read();

    /// @return ".vtk"
    VTKIO_API  std::string extension();
};

} // namespace fwVtkIO

#endif // _VTKIO_IMAGEREADER_HPP_
