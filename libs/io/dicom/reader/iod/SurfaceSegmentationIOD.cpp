/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/dicom/reader/iod/SurfaceSegmentationIOD.hpp"

#include "io/dicom/reader/ie/Equipment.hpp"
#include "io/dicom/reader/ie/Patient.hpp"
#include "io/dicom/reader/ie/series.hpp"
#include "io/dicom/reader/ie/Study.hpp"
#include "io/dicom/reader/ie/Surface.hpp"

#include <core/runtime/path.hpp>

#include <data/boolean.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <gdcmSegmentReader.h>
#include <gdcmSurfaceHelper.h>
#include <gdcmSurfaceReader.h>

#include <memory>
#include <utility>

namespace sight::io::dicom::reader::iod
{

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::SurfaceSegmentationIOD(
    const data::dicom_series::csptr& _dicom_series,
    const SPTR(io::dicom::container::DicomInstance)& _instance,
    const core::log::logger::sptr& _logger,
    ProgressCallback _progress,
    CancelRequestedCallback _cancel
) :
    io::dicom::reader::iod::InformationObjectDefinition(_dicom_series, _instance, _logger, _progress, _cancel)
{
}

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::~SurfaceSegmentationIOD()
= default;

//------------------------------------------------------------------------------

void SurfaceSegmentationIOD::read(data::series::sptr _series)
{
    data::model_series::sptr model_series = std::dynamic_pointer_cast<data::model_series>(_series);
    SIGHT_ASSERT("ModelSeries should not be null.", model_series);

    // Create GDCM Reader
    SPTR(gdcm::SurfaceReader) reader = std::make_shared<gdcm::SurfaceReader>();

    // Dicom container
    data::dicom_series::dicom_container_t dicom_container = m_dicomSeries->getDicomContainer();
    if(dicom_container.size() > 1)
    {
        m_logger->warning(
            "More than one surface segmentation storage have been found in the series. "
            "Only the first one will be read."
        );
    }

    // Read first file
    const core::memory::buffer_object::sptr buffer_obj          = dicom_container.begin()->second;
    const core::memory::buffer_manager::stream_info stream_info = buffer_obj->get_stream_info();
    SPTR(std::istream) is = stream_info.stream;
    reader->SetStream(*is);

    const bool success = reader->Read();
    SIGHT_THROW_EXCEPTION_IF(
        io::dicom::exception::Failed(
            "Unable to read the DICOM instance \""
            + buffer_obj->get_stream_info().fs_file.string()
            + "\" using the GDCM Reader."
        ),
        !success
    );

    // Create Information Entity helpers
    io::dicom::reader::ie::Patient patient_ie(m_dicomSeries, reader, m_instance, _series, m_logger,
                                              m_progressCallback, m_cancelRequestedCallback);
    io::dicom::reader::ie::Study study_ie(m_dicomSeries, reader, m_instance, _series, m_logger,
                                          m_progressCallback, m_cancelRequestedCallback);
    io::dicom::reader::ie::series series_ie(m_dicomSeries, reader, m_instance, _series, m_logger,
                                            m_progressCallback, m_cancelRequestedCallback);
    // Use Image as frame of reference
    io::dicom::reader::ie::Equipment equipment_ie(m_dicomSeries, reader, m_instance, _series, m_logger,
                                                  m_progressCallback, m_cancelRequestedCallback);

    io::dicom::reader::ie::Surface surface_ie(m_dicomSeries, reader, m_instance, model_series, m_logger,
                                              m_progressCallback, m_cancelRequestedCallback);

    // Load Segmented Property Registry
    const std::filesystem::path filepath = core::runtime::get_library_resource_file_path(
        "io_dicom/SegmentedPropertyRegistry.csv"
    );
    if(!surface_ie.loadSegmentedPropertyRegistry(filepath))
    {
        throw io::dicom::exception::Failed(
                  "Unable to load segmented property registry: '"
                  + filepath.string() + "'. File does not exist."
        );
    }

    // Read Patient Module - PS 3.3 C.7.1.1
    patient_ie.readPatientModule();

    // Read General Study Module - PS 3.3 C.7.2.1
    study_ie.readGeneralStudyModule();

    // Read Patient Study Module - PS 3.3 C.7.2.2
    study_ie.readPatientStudyModule();

    // Read General Series Module - PS 3.3 C.7.3.1
    series_ie.readGeneralSeriesModule();

    // Read General Series Module - PS 3.3 C.8.20.1
    // NOTE: Not used in Sight

    // Read Frame of Reference Module - PS 3.3 C.7.4.1
    // NOTE: Not used in Sight

    // Read General Equipment Module - PS 3.3 C.7.5.1
    equipment_ie.readGeneralEquipmentModule();

    // Read SOP Common Module - PS 3.3 C.12.1
    // NOTE: Not used in Sight

    // Read Surface Segmentation Module - PS 3.3 C.8.23.1
    // And Surface Mesh Module - PS 3.3 C.27.1
    surface_ie.readSurfaceSegmentationAndSurfaceMeshModules();

    // Display reconstructions
    _series->set_field("ShowReconstructions", std::make_shared<data::boolean>(true));
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::reader::iod
