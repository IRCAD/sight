/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "filter/dicom/splitter/ImagePositionPatientSplitter.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <geometry/data/VectorFunctions.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

SIGHT_REGISTER_DICOM_FILTER(sight::filter::dicom::splitter::ImagePositionPatientSplitter);

namespace sight::filter::dicom::splitter
{

const std::string ImagePositionPatientSplitter::s_FILTER_NAME        = "Image position patient splitter";
const std::string ImagePositionPatientSplitter::s_FILTER_DESCRIPTION =
    "Split instances by finding gaps in image position continuity. This filter assume that "
    "the instances are <b>already sorted</b> and only gaps between volumes remain.";

//-----------------------------------------------------------------------------

ImagePositionPatientSplitter::ImagePositionPatientSplitter(filter::dicom::IFilter::Key /*unused*/)
{
}

//-----------------------------------------------------------------------------

ImagePositionPatientSplitter::~ImagePositionPatientSplitter()
= default;

//-----------------------------------------------------------------------------

std::string ImagePositionPatientSplitter::getName() const
{
    return ImagePositionPatientSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string ImagePositionPatientSplitter::getDescription() const
{
    return ImagePositionPatientSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

ImagePositionPatientSplitter::DicomSeriesContainerType ImagePositionPatientSplitter::apply(
    const data::DicomSeries::sptr& series,
    const core::log::Logger::sptr& logger
)
const
{
    DicomSeriesContainerType result;

    OFCondition status;
    DcmDataset* dataset = nullptr;

    double previousIndex        = 0.;
    unsigned int instanceNumber = 0;
    double spacingBetweenSlices = 0.;
    const double epsilon        = 1e-2; // Value used to find a gap
    data::DicomSeries::sptr currentSeries;
    for(const auto& item : series->getDicomContainer())
    {
        const core::memory::BufferObject::sptr bufferObj = item.second;
        const std::size_t buffSize                       = bufferObj->getSize();
        core::memory::BufferObject::Lock lock(bufferObj);
        char* buffer = static_cast<char*>(lock.getBuffer());

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        DcmFileFormat fileFormat;
        fileFormat.transferInit();
        if(!fileFormat.read(is).good())
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << bufferObj->getStreamInfo().fsFile.string() << "' "
                << "(slice: '" << item.first << "')"
            );
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

        dataset = fileFormat.getDataset();

        if(!dataset->tagExists(DCM_ImagePositionPatient) || !dataset->tagExists(DCM_ImageOrientationPatient))
        {
            const std::string msg =
                "Unable to split the series using ImagePositionPatient and ImageOrientationPatient. "
                "Tag(s) missing.";
            throw filter::dicom::exceptions::FilterFailure(msg);
        }

        fwVec3d imagePosition;
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[i], i);
        }

        fwVec3d imageOrientationU;
        fwVec3d imageOrientationV;
        for(unsigned int i = 0 ; i < 3 ; ++i)
        {
            dataset->findAndGetFloat64(DCM_ImageOrientationPatient, imageOrientationU[i], i);
            dataset->findAndGetFloat64(DCM_ImageOrientationPatient, imageOrientationV[i], i + std::size_t(3));
        }

        //Compute Z direction (cross product)
        const fwVec3d zVector = geometry::data::cross(imageOrientationU, imageOrientationV);

        //Compute dot product to get the index
        const double index = geometry::data::dot(imagePosition, zVector);

        //Compute spacing
        const double spacing = index - previousIndex;
        if(currentSeries && fabs(spacingBetweenSlices) < epsilon)
        {
            spacingBetweenSlices = spacing;
        }

        // First frame or volume detected: We create a new Series
        if(!currentSeries || (fabs(spacing - spacingBetweenSlices) > epsilon))
        {
            if(currentSeries)
            {
                result.push_back(currentSeries);
                currentSeries->setNumberOfInstances(currentSeries->getDicomContainer().size());
            }

            instanceNumber = 0;
            currentSeries  = data::DicomSeries::New();
            currentSeries->shallowCopy(series);
            currentSeries->clearDicomContainer();
        }

        currentSeries->addBinary(instanceNumber++, bufferObj);
        previousIndex = index;
    }

    // Push last series created
    result.push_back(currentSeries);
    currentSeries->setNumberOfInstances(currentSeries->getDicomContainer().size());

    if(result.size() > 1)
    {
        logger->warning("Series has been split according to slice positions.");
    }

    return result;
}

} // namespace sight::filter::dicom::splitter
