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

#include "fwVtkIO/config.hpp"

namespace fwData
{
    class Reconstruction;
}

namespace fwMedData
{
    class ModelSeries;
}

namespace fwVtkIO
{

/**
 * @brief   Write a ::fwData::Reconstruction.
 * @class   ModelSeriesObjWriter
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
    FWVTKIO_API ModelSeriesObjWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! Destructor.
    FWVTKIO_API ~ModelSeriesObjWriter();

    //! Writing operator.
    FWVTKIO_API void write();

    /**
     * @return ".obj"
     */
    FWVTKIO_API std::string extension();

};

} // namespace fwVtkIO

#endif // __VTKIO_MODELSERIESOBJWRITER_HPP__

