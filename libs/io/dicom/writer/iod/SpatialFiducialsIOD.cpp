/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "io/dicom/writer/iod/SpatialFiducialsIOD.hpp"

#include "io/dicom/helper/FileWriter.hpp"
#include "io/dicom/writer/ie/Equipment.hpp"
#include "io/dicom/writer/ie/Image.hpp"
#include "io/dicom/writer/ie/Patient.hpp"
#include "io/dicom/writer/ie/Series.hpp"
#include "io/dicom/writer/ie/SpatialFiducials.hpp"
#include "io/dicom/writer/ie/Study.hpp"

#include <core/spyLog.hpp>

#include <data/Equipment.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Patient.hpp>
#include <data/Study.hpp>
#include <data/tools/fieldHelper/Image.hpp>
#include <data/Vector.hpp>

#include <gdcmWriter.h>

namespace sight::io::dicom
{
namespace writer
{
namespace iod
{

//------------------------------------------------------------------------------

SpatialFiducialsIOD::SpatialFiducialsIOD(const SPTR(io::dicom::container::DicomInstance)& instance,
                                         const std::filesystem::path& destinationPath,
                                         const core::log::Logger::sptr& logger,
                                         ProgressCallback progress,
                                         CancelRequestedCallback cancel) :
    io::dicom::writer::iod::InformationObjectDefinition(instance, destinationPath, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

SpatialFiducialsIOD::~SpatialFiducialsIOD()
{
}

//------------------------------------------------------------------------------

void SpatialFiducialsIOD::write(const data::Series::csptr& series)
{
    // Retrieve image series
    data::ImageSeries::csptr imageSeries = data::ImageSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", imageSeries);

    // Retrieve image
    data::Image::csptr image = imageSeries->getImage();

    data::Vector::sptr distances = image->getField< data::Vector >(
        data::tools::fieldHelper::Image::m_imageDistancesId);
    SLM_WARN_IF("Writing Spatial Fiducials IOD : distances will be ignored.", distances && !distances->empty());

    // Create writer
    SPTR(::gdcm::Writer) writer = std::make_shared< ::gdcm::Writer >();

    // Create Information Entity helpers
    io::dicom::writer::ie::Patient patientIE(writer, m_instance, series->getPatient());
    io::dicom::writer::ie::Study studyIE(writer, m_instance, series->getStudy());
    io::dicom::writer::ie::Series seriesIE(writer, m_instance, series);
    io::dicom::writer::ie::Equipment equipmentIE(writer, m_instance, series->getEquipment());
    io::dicom::writer::ie::SpatialFiducials spatialFiducialsIE(writer, m_instance, image);

    // Write Patient Module - PS 3.3 C.7.1.1
    patientIE.writePatientModule();

    // Write General Study Module - PS 3.3 C.7.2.1
    studyIE.writeGeneralStudyModule();

    // Write Patient Study Module - PS 3.3 C.7.2.2
    studyIE.writePatientStudyModule();

    // Write General Series Module - PS 3.3 C.7.3.1
    seriesIE.writeGeneralSeriesModule();

    // Write General Series Module - PS 3.3 C.21.1
    seriesIE.writeSpatialFiducialsSeriesModule();

    // Write General Equipment Module - PS 3.3 C.7.5.1
    equipmentIE.writeGeneralEquipmentModule();

    // Write Spatial Fiducials Module - PS 3.3 C.21.2
    spatialFiducialsIE.writeSpatialFiducialsModule();

    // Write Common Instance Reference Module - PS 3.3 C.12.2
    spatialFiducialsIE.writeCommonInstanceReferenceModule();

    // Write SOP Common Module - PS 3.3 C.12.1
    spatialFiducialsIE.writeSOPCommonModule();

    // Write document
    io::dicom::helper::FileWriter::write(m_destinationPath, writer);

}

//------------------------------------------------------------------------------
} // namespace iod
} // namespace writer
} // namespace sight::io::dicom
