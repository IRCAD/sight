/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/dicom/writer/surface_segmentation.hpp"

#include "io/dicom/writer/iod/surface_segmentation_io_d.hpp"

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

namespace sight::io::dicom::writer
{

//------------------------------------------------------------------------------

surface_segmentation::surface_segmentation() :
    m_logger(std::make_shared<core::log::logger>()),
    m_writer_job(std::make_shared<core::jobs::observer>("Writing DICOM file"))
{
}

//------------------------------------------------------------------------------

surface_segmentation::~surface_segmentation()
= default;

//------------------------------------------------------------------------------

void surface_segmentation::write()
{
    const data::model_series::csptr src_model_series        = this->get_concrete_object();
    const data::dicom_series::csptr associated_dicom_series = src_model_series->get_dicom_reference();

    SIGHT_ASSERT("sight::data::model_series not instanced", src_model_series);

    if(!associated_dicom_series)
    {
        m_logger->critical("Unable to retrieve information from the associated image series.");
        m_writer_job->done();
        m_writer_job->finish();
        return;
    }

    // Verify matching Patient's names
    const std::string& model_patient_name = src_model_series->get_patient_name();
    const std::string& image_patient_name = associated_dicom_series->get_patient_name();
    if(model_patient_name != image_patient_name)
    {
        m_logger->warning(
            "The patient's name of the model (\"" + model_patient_name
            + "\"  does not match the patient's name of the image (\"" + image_patient_name + "\" ."
        );
    }

    // Verify matching Patient ID
    const std::string& model_patient_id = src_model_series->get_patient_id();
    const std::string& image_patient_id = associated_dicom_series->get_patient_id();
    if(model_patient_id != image_patient_id)
    {
        m_logger->warning(
            "The patient ID of the model (\"" + model_patient_id
            + "\"  does not match the patient ID of the image (\"" + image_patient_id + "\" ."
        );
    }

    // Verify matching Study Instance UID
    const std::string& model_study_instance_uid = src_model_series->get_study_instance_uid();
    const std::string& image_study_instance_uid = associated_dicom_series->get_study_instance_uid();
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
    model_series->copy_patient_module(associated_dicom_series);
    model_series->copy_general_study_module(associated_dicom_series);
    model_series->copy_patient_study_module(associated_dicom_series);

    SPTR(io::dicom::container::dicom_instance) associated_dicom_instance =
        std::make_shared<io::dicom::container::dicom_instance>(associated_dicom_series, m_logger);

    SPTR(io::dicom::container::dicom_instance) model_instance =
        std::make_shared<io::dicom::container::dicom_instance>(model_series, m_logger, false);

    m_writer_job->done_work(0);
    m_writer_job->set_total_work_units(model_series->get_reconstruction_db().size());

    io::dicom::writer::iod::surface_segmentation_iod iod(model_instance,
                                                         associated_dicom_instance,
                                                         this->get_file(),
                                                         m_logger,
                                                         m_writer_job->progress_callback(),
                                                         m_writer_job->cancel_requested_callback());
    try
    {
        iod.write(model_series);
    }
    catch(const io::dicom::exception::failed& e)
    {
        m_logger->critical(e.what());
    }

    m_writer_job->done();
    m_writer_job->finish();
}

//------------------------------------------------------------------------------

std::string surface_segmentation::extension() const
{
    return {""};
}

//------------------------------------------------------------------------------

SPTR(core::jobs::base) surface_segmentation::get_job() const
{
    return m_writer_job;
}

//------------------------------------------------------------------------------

SPTR(core::log::logger) surface_segmentation::get_logger() const
{
    return m_logger;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer
