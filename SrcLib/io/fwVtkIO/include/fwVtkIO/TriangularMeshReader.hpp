/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_TRIANGULARMESHREADER_HPP_
#define _VTKIO_TRIANGULARMESHREADER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/TriangularMesh.hpp>

#include "fwVtkIO/config.hpp"

namespace fwVtkIO
{

/**
 * @brief   Read a mesh.
 * @class   TriangularMeshReader
 * 
 * @date    2009.
 *
 * Read a VTK Mesh using the VTK lib
 */

class TriangularMeshReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::TriangularMesh >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((TriangularMeshReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::TriangularMesh >),
                                           (()),
                                           ::fwDataIO::reader::factory::New< TriangularMeshReader >
                                          );
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    VTKIO_API TriangularMeshReader(::fwDataIO::reader::IObjectReader::Key key);

    //! @brief Destructor.
    VTKIO_API ~TriangularMeshReader();

    //! @brief Reading operator.
    VTKIO_API void read();

    /// @return ".trian"
    VTKIO_API  std::string extension();
};

} // namespace fwVtkIO

#endif // _VTKIO_TRIANGULARMESHREADER_HPP_
