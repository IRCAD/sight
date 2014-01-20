/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_MESHWRITER_HPP_
#define _VTKIO_MESHWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwTools/ProgressAdviser.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwData/Mesh.hpp>

#include "fwVtkIO/config.hpp"

namespace fwVtkIO
{

/**
 * @brief   Write a mesh.
 * @class   MeshWriter
 * 
 * @date    2011.
 *
 * Write a VTK Mesh using the VTK lib
 */

class MeshWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Mesh >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((MeshWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Mesh >),
                                           (()),
                                           ::fwDataIO::writer::factory::New< MeshWriter >
                                          );
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    VTKIO_API MeshWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! @brief Destructor.
    VTKIO_API ~MeshWriter();

    //! @brief Reading operator.
    VTKIO_API void write();

    /// @return ".vtk"
    VTKIO_API  std::string extension();
};

} // namespace fwVtkIO

#endif // _VTKIO_MESHWRITER_HPP_
