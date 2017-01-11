/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __MIDASIO_MODELSERIESWRITER_HPP__
#define __MIDASIO_MODELSERIESWRITER_HPP__

#include <vtkActor.h>

#include <fwTools/ProgressAdviser.hpp>

#include <fwMedData/ModelSeries.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/StructureTraits.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include "midasIO/config.hpp"

namespace midasIO
{

/**
 * @brief   Write a reconstruction.
 * @class   ModelSeriesWriter.
 *
 * Write a ::fwData::Reconstruction using the VTK lib and the obj format
 */
class MIDASIO_CLASS_API ModelSeriesWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ModelSeries >,
                                            public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                                            public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ModelSeriesWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::
                                                                                                         ModelSeries >),
                                           (()),
                                           ::fwDataIO::writer::factory::New< ModelSeriesWriter >
                                           );
    fwCoreAllowSharedFromThis();

    /// Constructor. Does nothing.
    MIDASIO_API ModelSeriesWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor. Does nothing.
    MIDASIO_API ~ModelSeriesWriter();

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

#endif // __MIDASIO_MODELSERIESWRITER_HPP__
