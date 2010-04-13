/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_IMAGEWRITER_HPP_
#define _VTKIO_IMAGEWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <vtkActor.h>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Image.hpp>

#include "vtkIO/config.hpp"

namespace vtkIO
{

/**
 * @brief   Write an image.
 * @class   ImageWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Write a VTK Image using the VTK lib
 */
class ImageWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro(     (ImageWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >),
                                                     (()),
                                                     new  ImageWriter
                                                );

    //! @brief Constructor.
    VTKIO_API ImageWriter();

    //! @brief Destructor.
    VTKIO_API ~ImageWriter();

    //! @brief Writing operator.
    VTKIO_API void write();

    /// @return ".vtk"
    VTKIO_API  std::string extension();
    };

} // namespace vtkIO

#endif // _VTKIO_IMAGEWRITER_HPP_
