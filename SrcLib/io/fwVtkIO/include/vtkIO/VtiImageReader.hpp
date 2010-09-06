/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_VTIIMAGEREADER_HPP_
#define _VTKIO_VTIIMAGEREADER_HPP_

#include <boost/filesystem/path.hpp>

#include <vtkActor.h>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Image.hpp>

#include "vtkIO/config.hpp"

namespace vtkIO
{

/**
 * @brief   Read an image.
 * @class   VtiImageReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Read a VTK Image using the VTK lib
 */
class VtiImageReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro(     (VtiImageReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >),
                                                     (()),
                                                     new  VtiImageReader
                                                );



    //! @brief Constructor.
    VTKIO_API VtiImageReader();

    //! @brief Destructor.
    VTKIO_API ~VtiImageReader();

    //! @brief Reading operator.
    VTKIO_API void read();

    /// @return ".vtk"
    VTKIO_API  std::string extension();
};

} // namespace vtkIO

#endif // _VTKIO_VTIIMAGEREADER_HPP_
