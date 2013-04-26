/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_MESHREADER_HPP_
#define _VTKIO_MESHREADER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>

#include <fwData/Mesh.hpp>

#include "fwVtkIO/config.hpp"

namespace fwVtkIO
{

/**
 * @brief   Read a mesh.
 * @class   MeshReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 *
 * Read a VTK Mesh using the VTK lib
 */

class MeshReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::Mesh >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((MeshReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::Mesh >),
                                           (()),
                                           ::fwDataIO::reader::factory::New< MeshReader >
                                          );
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    VTKIO_API MeshReader(::fwDataIO::reader::IObjectReader::Key key);

    //! @brief Destructor.
    VTKIO_API ~MeshReader();

    //! @brief Reading operator.
    VTKIO_API void read();

    /// @return ".vtk"
    VTKIO_API  std::string extension();
};

} // namespace fwVtkIO

#endif // _VTKIO_MESHREADER_HPP_
