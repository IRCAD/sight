/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/jobs/Aggregator.hpp>
#include <core/jobs/Job.hpp>
#include <core/jobs/Observer.hpp>

#include <data/DicomSeries.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/PointList.hpp>
#include <data/Series.hpp>
#include <data/Vector.hpp>

#include <io/base/writer/registry/macros.hpp>

SIGHT_REGISTER_IO_WRITER(sight::io::dicom::writer::SurfaceSegmentation);

namespace sight::io::dicom::writer
{

//------------------------------------------------------------------------------

SurfaceSegmentation::SurfaceSegmentation(io::base::writer::IObjectWriter::Key /*key*/) :
    m_logger(core::log::Logger::New()),
    m_writerJob(core::jobs::Observer::New("Writing DICOM file"))
{
}

//------------------------------------------------------------------------------

SurfaceSegmentation::~SurfaceSegmentation()
= default;

//------------------------------------------------------------------------------

void SurfaceSegmentation::write()
{
    const data::ModelSeries::csptr srcModelSeries        = this->getConcreteObject();
    const data::DicomSeries::csptr associatedDicomSeries = srcModelSeries->getDicomReference();

    SIGHT_ASSERT("sight::data::ModelSeries not instanced", srcModelSeries);

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
            std::string("The patient's name of the model (\"").append(modelPatientName)
            .append("\") does not match the patient's name of the image (\"").append(imagePatientName).append("\").")
        );
    }

    // Verify matching Patient ID
    const std::string& modelPatientID = srcModelSeries->getPatientID();
    const std::string& imagePatientID = associatedDicomSeries->getPatientID();
    if(modelPatientID != imagePatientID)
    {
        m_logger->warning(
            std::string("The patient ID of the model (\"").append(modelPatientID)
            .append("\") does not match the patient ID of the image (\"").append(imagePatientID).append("\").")
        );
    }

    // Verify matching Study Instance UID
    const std::string& modelStudyInstanceUID = srcModelSeries->getStudyInstanceUID();
    const std::string& imageStudyInstanceUID = associatedDicomSeries->getStudyInstanceUID();
    if(modelStudyInstanceUID != imageStudyInstanceUID)
    {
        m_logger->warning(
            std::string("The study instance UID of the model (\"").append(modelStudyInstanceUID)
            .append("\") does not match the study instance UID of the image (\"").append(imageStudyInstanceUID)
            .append("\").")
        );
    }

    // Complete Model Series with information from associated Image Series
    const data::ModelSeries::sptr modelSeries = data::ModelSeries::New();
    modelSeries->shallowCopy(srcModelSeries);

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

    m_writerJob->doneWork(0);
    m_writerJob->setTotalWorkUnits(modelSeries->getReconstructionDB().size());

    io::dicom::writer::iod::SurfaceSegmentationIOD iod(modelInstance,
                                                       associatedDicomInstance,
                                                       this->getFile(),
                                                       m_logger,
                                                       m_writerJob->progressCallback(),
                                                       m_writerJob->cancelRequestedCallback());
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

SPTR(core::jobs::IJob) SurfaceSegmentation::getJob() const
{
    return m_writerJob;
}

//------------------------------------------------------------------------------

SPTR(core::log::Logger) SurfaceSegmentation::getLogger() const
{
    return m_logger;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::writer
