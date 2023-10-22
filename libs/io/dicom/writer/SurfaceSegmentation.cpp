/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "io/dicom/writer/SurfaceSegmentation.hpp"

#include "io/dicom/writer/iod/SurfaceSegmentationIOD.hpp"

#include <core/jobs/aggregator.hpp>
#include <core/jobs/job.hpp>
#include <core/jobs/observer.hpp>

#include <data/dicom_series.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/point_list.hpp>
#include <data/series.hpp>
#include <data/vector.hpp>

#include <io/__/writer/registry/macros.hpp>

SIGHT_REGISTER_IO_WRITER(sight::io::dicom::writer::SurfaceSegmentation);

namespace sight::io::dicom::writer
{

//------------------------------------------------------------------------------

SurfaceSegmentation::SurfaceSegmentation() :
    m_logger(std::make_shared<core::log::logger>()),
    m_writerJob(std::make_shared<core::jobs::observer>("Writing DICOM file"))
{
}

//------------------------------------------------------------------------------

SurfaceSegmentation::~SurfaceSegmentation()
= default;

//------------------------------------------------------------------------------

void SurfaceSegmentation::write()
{
    const data::model_series::csptr src_model_series        = this->getConcreteObject();
    const data::dicom_series::csptr associated_dicom_series = src_model_series->getDicomReference();

    SIGHT_ASSERT("sight::data::model_series not instanced", src_model_series);

    if(!associated_dicom_series)
    {
        m_logger->critical("Unable to retrieve information from the associated image series.");
        m_writerJob->done();
        m_writerJob->finish();
        return;
    }

    // Verify matching Patient's names
    const std::string& model_patient_name = src_model_series->getPatientName();
    const std::string& image_patient_name = associated_dicom_series->getPatientName();
    if(model_patient_name != image_patient_name)
    {
        m_logger->warning(
            "The patient's name of the model (\"" + model_patient_name
            + "\"  does not match the patient's name of the image (\"" + image_patient_name + "\" ."
        );
    }

    // Verify matching Patient ID
    const std::string& model_patient_id = src_model_series->getPatientID();
    const std::string& image_patient_id = associated_dicom_series->getPatientID();
    if(model_patient_id != image_patient_id)
    {
        m_logger->warning(
            "The patient ID of the model (\"" + model_patient_id
            + "\"  does not match the patient ID of the image (\"" + image_patient_id + "\" ."
        );
    }

    // Verify matching Study Instance UID
    const std::string& model_study_instance_uid = src_model_series->getStudyInstanceUID();
    const std::string& image_study_instance_uid = associated_dicom_series->getStudyInstanceUID();
    if(model_study_instance_uid != image_study_instance_uid)
    {
        m_logger->warning(
            "The study instance UID of the model (\"" + model_study_instance_uid
            + "\"  does not match the study instance UID of the image (\"" + image_study_instance_uid + "\" ."
        );
    }

    // Complete Model Series with information from associated image Series
    const data::model_series::sptr model_series = std::make_shared<data::model_series>();
    model_series->shallow_copy(src_model_series);

    // Copy Study and Patient
    /// @todo verify this is required since we already have the same patient ID and same study uid, which means we
    /// certainly already have the same data...
    model_series->copyPatientModule(associated_dicom_series);
    model_series->copyGeneralStudyModule(associated_dicom_series);
    model_series->copyPatientStudyModule(associated_dicom_series);

    SPTR(io::dicom::container::DicomInstance) associated_dicom_instance =
        std::make_shared<io::dicom::container::DicomInstance>(associated_dicom_series, m_logger);

    SPTR(io::dicom::container::DicomInstance) model_instance =
        std::make_shared<io::dicom::container::DicomInstance>(model_series, m_logger, false);

    m_writerJob->done_work(0);
    m_writerJob->set_total_work_units(model_series->getReconstructionDB().size());

    io::dicom::writer::iod::SurfaceSegmentationIOD iod(model_instance,
                                                       associated_dicom_instance,
                                                       this->get_file(),
                                                       m_logger,
                                                       m_writerJob->progress_callback(),
                                                       m_writerJob->cancel_requested_callback());
    try
    {
        iod.write(model_series);
    }
    catch(const io::dicom::exception::Failed& e)
    {
        m_logger->critical(e.what());
    }

    m_writerJob->done();
    m_writerJob->finish();
}

//------------------------------------------------------------------------------

std::string SurfaceSegmentation::extension() const
{
    return {""};
}

//------------------------------------------------------------------------------

SPTR(core::jobs::base) SurfaceSegmentation::getJob() const
{
    return m_writerJob;
}

//------------------------------------------------------------------------------

SPTR(core::log::logger) SurfaceSegmentation::getLogger() const
{
    return m_logger;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer
