/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_WRITER_MESHWRITER_HPP_
#define _FWDATAIO_WRITER_MESHWRITER_HPP_

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <fwData/Mesh.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/GenericObjectWriter.hpp"

namespace fwDataIO
{

namespace writer
{

/**
 * @brief   Mesh writer. Write file format .trian
 * @class   MeshWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2011
 *
 * Ircad writer to write a ::fwData::Mesh (with only triangular cells) on filesystem which the file format
 * is .trian. This format is basic. This is an ascii file which contains :
 * 1) nb of points
 * 2) list of points (x,y,z). Line numbers represent index of point in vector
 * 3) nb of triangles
 * 4) list of triangles with index of points and normals ( often set a -1 )
 */
class FWDATAIO_CLASS_API MeshWriter :
    public GenericObjectWriter< ::fwData::Mesh>,
    public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((MeshWriter)(GenericObjectWriter< ::fwData::Mesh >),
                                           (()),
                                           new  MeshWriter
                                           );
    /// Constructor. Do nothing.
    FWDATAIO_API MeshWriter( );

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~MeshWriter();

    /**
     * Write the file with standard iostream API.
     * @pre Mesh must contains only triangular cells.
     */
    FWDATAIO_API virtual void write();

    /// Defines extension supported by this reader ".trian"
    FWDATAIO_API virtual std::string  extension();

};

} // namespace writer

} // namespace fwDataIO


#endif // _FWDATAIO_WRITER_MESHWRITER_HPP_
