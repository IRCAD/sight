/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGdcmIO/writer/iod/SpatialFiducialsIOD.hpp"

#include "fwGdcmIO/helper/FileWriter.hpp"
#include "fwGdcmIO/writer/ie/Equipment.hpp"
#include "fwGdcmIO/writer/ie/Image.hpp"
#include "fwGdcmIO/writer/ie/Patient.hpp"
#include "fwGdcmIO/writer/ie/Series.hpp"
#include "fwGdcmIO/writer/ie/SpatialFiducials.hpp"
#include "fwGdcmIO/writer/ie/Study.hpp"

#include <fwCore/spyLog.hpp>

#include <fwData/Image.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <boost/make_shared.hpp>

#include <gdcmWriter.h>

namespace fwGdcmIO
{
namespace writer
{
namespace iod
{

//------------------------------------------------------------------------------

SpatialFiducialsIOD::SpatialFiducialsIOD(const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                         const std::filesystem::path& destinationPath,
                                         const ::fwLog::Logger::sptr& logger,
                                         ProgressCallback progress,
                                         CancelRequestedCallback cancel) :
    ::fwGdcmIO::writer::iod::InformationObjectDefinition(instance, destinationPath, logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

SpatialFiducialsIOD::~SpatialFiducialsIOD()
{
}

//------------------------------------------------------------------------------

void SpatialFiducialsIOD::write(const ::fwMedData::Series::csptr& series)
{
    // Retrieve image series
    ::fwMedData::ImageSeries::csptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    SLM_ASSERT("Image series should not be null.", imageSeries);

    // Retrieve image
    ::fwData::Image::csptr image = imageSeries->getImage();

    ::fwData::Vector::sptr distances = image->getField< ::fwData::Vector >(
        ::fwDataTools::fieldHelper::Image::m_imageDistancesId);
    SLM_WARN_IF("Writing Spatial Fiducials IOD : distances will be ignored.", distances && !distances->empty());

    // Create writer
    SPTR(::gdcm::Writer) writer = std::make_shared< ::gdcm::Writer >();

    // Create Information Entity helpers
    ::fwGdcmIO::writer::ie::Patient patientIE(writer, m_instance, series->getPatient());
    ::fwGdcmIO::writer::ie::Study studyIE(writer, m_instance, series->getStudy());
    ::fwGdcmIO::writer::ie::Series seriesIE(writer, m_instance, series);
    ::fwGdcmIO::writer::ie::Equipment equipmentIE(writer, m_instance, series->getEquipment());
    ::fwGdcmIO::writer::ie::SpatialFiducials spatialFiducialsIE(writer, m_instance, image);

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
    ::fwGdcmIO::helper::FileWriter::write(m_destinationPath, writer);

}

//------------------------------------------------------------------------------
} // namespace iod
} // namespace writer
} // namespace fwGdcmIO
