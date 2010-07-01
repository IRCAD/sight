/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKIO_RECONSTRUCTIONWRITER_HPP_
#define _VTKIO_RECONSTRUCTIONWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <vtkActor.h>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Reconstruction.hpp>

#include "vtkIO/config.hpp"


namespace vtkIO
{

/**
 * @brief   Write a reconstruction.
 * @class   ReconstructionWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Write a ::fwData::Reconstruction using the VTK lib and the obj format
 */
class ReconstructionWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Acquisition >,
                             public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro(     (ReconstructionWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Acquisition >),
                                                     (()),
                                                     new  ReconstructionWriter
                                                );

    //! @brief Constructor.
    VTKIO_API ReconstructionWriter();

    //! @brief Destructor.
    VTKIO_API ~ReconstructionWriter();

    //! @brief Writing operator.
    VTKIO_API void write();

    /// @return ".obj"
    VTKIO_API  std::string extension();
protected:
    /*!
     * @brief Create a vtkActor* from a ::fwData::Reconstruction::sptr.
     *
     * @param[in] _pReconstruction ::fwData::Reconstruction::sptr.
     * @return vtkActor*.
     */
    vtkActor * createActor( ::fwData::Reconstruction::sptr _pReconstruction );
};

} // namespace vtkIO

#endif // _VTKIO_RECONSTRUCTIONWRITER_HPP_
