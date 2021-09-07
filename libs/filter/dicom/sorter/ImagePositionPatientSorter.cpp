/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "filter/dicom/sorter/ImagePositionPatientSorter.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <geometry/data/VectorFunctions.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

fwDicomIOFilterRegisterMacro(::sight::filter::dicom::sorter::ImagePositionPatientSorter);

namespace sight::filter::dicom
{

namespace sorter
{

const std::string ImagePositionPatientSorter::s_FILTER_NAME        = "Image position patient sorter";
const std::string ImagePositionPatientSorter::s_FILTER_DESCRIPTION =
    "Sort instances by computing image position using <i>ImagePositionPatient</i> "
    "and <i>ImageOrientationPatient</i> tags.";

//-----------------------------------------------------------------------------

ImagePositionPatientSorter::ImagePositionPatientSorter(filter::dicom::IFilter::Key key) :
    ISorter()
{
}

//-----------------------------------------------------------------------------

ImagePositionPatientSorter::~ImagePositionPatientSorter()
{
}

//-----------------------------------------------------------------------------

std::string ImagePositionPatientSorter::getName() const
{
    return ImagePositionPatientSorter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string ImagePositionPatientSorter::getDescription() const
{
    return ImagePositionPatientSorter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

ImagePositionPatientSorter::DicomSeriesContainerType ImagePositionPatientSorter::apply(
    const data::DicomSeries::sptr& series,
    const core::log::Logger::sptr& logger
) const
{
    DicomSeriesContainerType result;

    typedef std::map<double, core::memory::BufferObject::sptr> SortedDicomMapType;
    SortedDicomMapType sortedDicom;

    OFCondition status;
    DcmDataset* dataset;

    for(const auto& item : series->getDicomContainer())
    {
        const core::memory::BufferObject::sptr bufferObj = item.second;
        const size_t buffSize                            = bufferObj->getSize();
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
            dataset->findAndGetFloat64(DCM_ImageOrientationPatient, imageOrientationV[i], i + 3);
        }

        //Compute Z direction (cross product)
        const fwVec3d zVector = geometry::data::cross(imageOrientationU, imageOrientationV);

        //Compute dot product to get the index
        const double index = geometry::data::dot(imagePosition, zVector);

        sortedDicom[index] = bufferObj;
    }

    if(sortedDicom.size() != series->getDicomContainer().size())
    {
        const std::string msg =
            "Unable to sort the series using the ImagePositionPatient tag. Some images have the same "
            "position meaning this series contains multiple volumes. Try to split the volumes using a different "
            "filter.";
        throw filter::dicom::exceptions::FilterFailure(msg);
    }

    series->clearDicomContainer();
    size_t index = 0;
    for(const auto& item : sortedDicom)
    {
        series->addBinary(index++, item.second);
    }

    result.push_back(series);

    logger->information("The instances have been sorted using the slices positions.");

    return result;
}

} // namespace sorter

} // namespace sight::filter::dicom
