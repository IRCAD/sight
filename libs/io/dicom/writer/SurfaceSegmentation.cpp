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
    const data::model_series::csptr srcModelSeries        = this->getConcreteObject();
    const data::dicom_series::csptr associatedDicomSeries = srcModelSeries->getDicomReference();

    SIGHT_ASSERT("sight::data::model_series not instanced", srcModelSeries);

    if(!associatedDicomSeries)
    {
        m_logger->critical("Unable to retrieve information from the associated image series.");
        m_writerJob->done();
        m_writerJob->finish();
        return;
    }

    // Verify matching Patient's names
    const std::string& modelPatientName = srcModelSeries->getPatientName();
    const std::string& imagePatientName = associatedDicomSeries->getPatientName();
    if(modelPatientName != imagePatientName)
    {
        m_logger->warning(
            "The patient's name of the model (\"" + modelPatientName
            + "\"  does not match the patient's name of the image (\"" + imagePatientName + "\" ."
        );
    }

    // Verify matching Patient ID
    const std::string& modelPatientID = srcModelSeries->getPatientID();
    const std::string& imagePatientID = associatedDicomSeries->getPatientID();
    if(modelPatientID != imagePatientID)
    {
        m_logger->warning(
            "The patient ID of the model (\"" + modelPatientID
            + "\"  does not match the patient ID of the image (\"" + imagePatientID + "\" ."
        );
    }

    // Verify matching Study Instance UID
    const std::string& modelStudyInstanceUID = srcModelSeries->getStudyInstanceUID();
    const std::string& imageStudyInstanceUID = associatedDicomSeries->getStudyInstanceUID();
    if(modelStudyInstanceUID != imageStudyInstanceUID)
    {
        m_logger->warning(
            "The study instance UID of the model (\"" + modelStudyInstanceUID
            + "\"  does not match the study instance UID of the image (\"" + imageStudyInstanceUID + "\" ."
        );
    }

    // Complete Model Series with information from associated image Series
    const data::model_series::sptr modelSeries = std::make_shared<data::model_series>();
    modelSeries->shallow_copy(srcModelSeries);

    // Copy Study and Patient
    /// @todo verify this is required since we already have the same patient ID and same study uid, which means we
    /// certainly already have the same data...
    modelSeries->copyPatientModule(associatedDicomSeries);
    modelSeries->copyGeneralStudyModule(associatedDicomSeries);
    modelSeries->copyPatientStudyModule(associatedDicomSeries);

    SPTR(io::dicom::container::DicomInstance) associatedDicomInstance =
        std::make_shared<io::dicom::container::DicomInstance>(associatedDicomSeries, m_logger);

    SPTR(io::dicom::container::DicomInstance) modelInstance =
        std::make_shared<io::dicom::container::DicomInstance>(modelSeries, m_logger, false);

    m_writerJob->done_work(0);
    m_writerJob->set_total_work_units(modelSeries->getReconstructionDB().size());

    io::dicom::writer::iod::SurfaceSegmentationIOD iod(modelInstance,
                                                       associatedDicomInstance,
                                                       this->get_file(),
                                                       m_logger,
                                                       m_writerJob->progress_callback(),
                                                       m_writerJob->cancel_requested_callback());
    try
    {
        iod.write(modelSeries);
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
