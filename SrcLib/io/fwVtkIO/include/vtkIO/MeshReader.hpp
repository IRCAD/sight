/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_MESHREADER_HPP_
#define _VTKIO_MESHREADER_HPP_

#include <boost/filesystem/path.hpp>

#include <vtkActor.h>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/TriangularMesh.hpp>

#include "vtkIO/config.hpp"

namespace vtkIO
{

/**
 * @brief   Read a mesh.
 * @class   MeshReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Read a VTK Mesh using the VTK lib
 */

class MeshReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::TriangularMesh >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((MeshReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::TriangularMesh >),
                                           (()),
                                           new MeshReader
                                          );
    fwCoreAllowSharedFromThis();

    //! @brief Reading operator.
    VTKIO_API void read();

    /// @return ".trian"
    VTKIO_API  std::string extension();

protected:

    //! @brief Constructor.
    VTKIO_API MeshReader();

    //! @brief Destructor.
    VTKIO_API ~MeshReader();
};

} // namespace vtkIO

#endif // _VTKIO_MESHREADER_HPP_
