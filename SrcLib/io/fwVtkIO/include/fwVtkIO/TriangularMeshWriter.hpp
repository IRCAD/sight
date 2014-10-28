/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_TRIANGULARMESHWRITER_HPP_
#define _VTKIO_TRIANGULARMESHWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/TriangularMesh.hpp>

#include "fwVtkIO/config.hpp"

namespace fwVtkIO
{

/**
 * @brief   Write a mesh.
 * @class   TriangularMeshWriter
 * 
 * @date    2009.
 *
 * Write a VTK Mesh using the VTK lib
 */

class TriangularMeshWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::TriangularMesh >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((TriangularMeshWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::TriangularMesh >),
                                           (()),
                                           ::fwDataIO::writer::factory::New< TriangularMeshWriter >
                                          );
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    VTKIO_API TriangularMeshWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! @brief Destructor.
    VTKIO_API ~TriangularMeshWriter();

    //! @brief Reading operator.
    VTKIO_API void write();

    /// @return ".trian"
    VTKIO_API  std::string extension();
};

} // namespace fwVtkIO

#endif // _VTKIO_TRIANGULARMESHWRITER_HPP_
