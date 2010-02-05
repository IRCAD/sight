/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_MESHWRITER_HPP_
#define _VTKIO_MESHWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <vtkActor.h>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/TriangularMesh.hpp>

#include "vtkIO/config.hpp"

namespace vtkIO
{

/**
 * @brief   Write a mesh.
 * @class   MeshWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Write a VTK Mesh using the VTK lib
 */

class MeshWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::TriangularMesh >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro(     (MeshWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::TriangularMesh >::Baseclass),
                                                     (()),
                                                     new  MeshWriter
                                                );

    //! @brief Constructor.
    VTKIO_API MeshWriter();

    //! @brief Destructor.
    VTKIO_API ~MeshWriter();

    //! @brief Reading operator.
    VTKIO_API void write();

};

} // namespace vtkIO

#endif // _VTKIO_MESHWRITER_HPP_
