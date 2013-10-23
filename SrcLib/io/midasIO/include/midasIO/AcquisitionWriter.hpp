/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _MIDASIO_ACQUISITIONWRITER_HPP_
#define _MIDASIO_ACQUISITIONWRITER_HPP_

#include <vtkActor.h>

#include <fwTools/ProgressAdviser.hpp>

#include <fwData/Acquisition.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/StructureTraits.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include "midasIO/config.hpp"

namespace midasIO
{

/**
 * @brief   Write a reconstruction.
 * @class   ReconstructionWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * Write a ::fwData::Reconstruction using the VTK lib and the obj format
 */
class MIDASIO_CLASS_API AcquisitionWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Acquisition >,
                             public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                             public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((AcquisitionWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Acquisition >),
                                           (()),
                                           ::fwDataIO::writer::factory::New< AcquisitionWriter >
                                          );
    fwCoreAllowSharedFromThis();

    /// Constructor. Does nothing.
    MIDASIO_API AcquisitionWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor. Does nothing.
    MIDASIO_API ~AcquisitionWriter();

    /// Writes obj files and sceneDescription.xml in the folder.
    MIDASIO_API void write();

    /// Returns "", does nothing.
    MIDASIO_API std::string  extension();

private:

    /**
     * @name Maps an organ class to a layer value.
     * @{ */
    typedef std::map< ::fwData::StructureTraits::StructureClass, std::string > OrganClass2LayerType;
    static const OrganClass2LayerType s_ORGAN_LAYERS;
    /**  @} */

    /*!
     * @brief Create a vtkActor* from a ::fwData::Reconstruction::sptr.
     *
     * @param[in] _pReconstruction ::fwData::Reconstruction::sptr.
     * @return vtkActor*.
     */
    MIDASIO_API vtkActor * createActor( ::fwData::Reconstruction::sptr pReconstruction );

};

} // namespace midasIO

#endif // _MIDASIO_ACQUISITIONWRITER_HPP_
