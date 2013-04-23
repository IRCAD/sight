/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VTKIO_MODELSERIESOBJWRITER_HPP__
#define __VTKIO_MODELSERIESOBJWRITER_HPP__

#include <boost/filesystem/path.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/Folder.hpp>
#include <fwTools/ProgressAdviser.hpp>

#include "vtkIO/config.hpp"

namespace fwData
{
    class Reconstruction;
}

namespace fwMedData
{
    class ModelSeries;
}

namespace vtkIO
{

/**
 * @brief   Write a ::fwData::Reconstruction.
 * @class   ModelSeriesObjWriter.
 *
 * Write a ::fwData::Reconstruction as .obj file using the VTK lib and the obj format.
 */
class ModelSeriesObjWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ModelSeries >,
                             public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro(
            (ModelSeriesObjWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ModelSeries >),
            (()),
            ::fwDataIO::writer::factory::New< ModelSeriesObjWriter >
            );

    fwCoreAllowSharedFromThis();

    //! Constructor.
    VTKIO_API ModelSeriesObjWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! Destructor.
    VTKIO_API ~ModelSeriesObjWriter();

    //! Writing operator.
    VTKIO_API void write();

    /**
     * @return ".obj"
     */
    VTKIO_API std::string extension();

};

} // namespace vtkIO

#endif // __VTKIO_MODELSERIESOBJWRITER_HPP__

