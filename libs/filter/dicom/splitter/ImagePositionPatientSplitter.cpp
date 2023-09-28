/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
    const core::log::logger::sptr& logger
)
const
{
    DicomSeriesContainerType result;

    OFCondition status;
    double previousIndex        = 0.;
    unsigned int instanceNumber = 0;
    double spacingBetweenSlices = 0.;
    const double epsilon        = 1e-2; // Value used to find a gap
    data::DicomSeries::sptr currentSeries;
    for(const auto& item : series->getDicomContainer())
    {
        const core::memory::buffer_object::sptr bufferObj = item.second;
        const std::size_t buffSize                        = bufferObj->size();
        core::memory::buffer_object::lock_t lock(bufferObj);
        char* buffer = static_cast<char*>(lock.buffer());

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        DcmFileFormat fileFormat;
        fileFormat.transferInit();
        if(!fileFormat.read(is).good())
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << bufferObj->get_stream_info().fs_file.string() << "' "
                << "(slice: '" << item.first << "')"
            );
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

        DcmDataset* dataset = fileFormat.getDataset();

        if(!dataset->tagExists(DCM_ImagePositionPatient) || !dataset->tagExists(DCM_ImageOrientationPatient))
        {
            const std::string msg =
                "Unable to split the series using ImagePositionPatient and ImageOrientationPatient. "
                "Tag(s) missing.";
            throw sight::filter::dicom::exceptions::FilterFailure(msg);
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
            currentSeries  = std::make_shared<data::DicomSeries>();
            currentSeries->shallow_copy(series);
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
