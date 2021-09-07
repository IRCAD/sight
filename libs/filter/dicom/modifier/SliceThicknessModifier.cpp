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

#include "filter/dicom/modifier/SliceThicknessModifier.hpp"

#include "filter/dicom/exceptions/FilterFailure.hpp"
#include "filter/dicom/registry/macros.hpp"

#include <geometry/data/VectorFunctions.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

fwDicomIOFilterRegisterMacro(::sight::filter::dicom::modifier::SliceThicknessModifier);

namespace sight::filter::dicom
{

namespace modifier
{

const std::string SliceThicknessModifier::s_FILTER_NAME        = "Slice thickness modifier";
const std::string SliceThicknessModifier::s_FILTER_DESCRIPTION =
    "Compute and modify slice thickness using <i>ImagePositionPatient</i> "
    "and <i>ImageOrientationPatient</i> tags of the two first instances.";

//-----------------------------------------------------------------------------

SliceThicknessModifier::SliceThicknessModifier(filter::dicom::IFilter::Key key) :
    IModifier()
{
}

//-----------------------------------------------------------------------------

SliceThicknessModifier::~SliceThicknessModifier()
{
}

//-----------------------------------------------------------------------------

std::string SliceThicknessModifier::getName() const
{
    return SliceThicknessModifier::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string SliceThicknessModifier::getDescription() const
{
    return SliceThicknessModifier::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

SliceThicknessModifier::DicomSeriesContainerType SliceThicknessModifier::apply(
    const data::DicomSeries::sptr& series,
    const core::log::Logger::sptr& logger
) const
{
    DicomSeriesContainerType result;

    if(series->getDicomContainer().size() < 2)
    {
        SIGHT_WARN("SliceThicknessModifier is being applied on a series containing only one slice.");
        result.push_back(series);
        return result;
    }

    // Retrieve the two first instances
    auto firstItem = series->getDicomContainer().begin();

    const core::memory::BufferObject::sptr& firstBufferObj  = firstItem->second;
    const core::memory::BufferObject::sptr& secondBufferObj = (++firstItem)->second;

    // Compute the slice thickness between the 2 first slices.
    const double firstIndex     = this->getInstanceZPosition(firstBufferObj);
    const double secondIndex    = this->getInstanceZPosition(secondBufferObj);
    const double sliceThickness = std::abs(secondIndex - firstIndex);

    // Check that the computed sliceThickness doesn't match the sliceThickness of the first instance
    const double currentSliceThickness = this->getSliceThickness(firstBufferObj);
    const double epsilon               = 1e-2;

    // If the computed sliceThickness doesn't match the sliceThickness value
    // we add the computed value to the DicomSeries.
    if(std::abs(sliceThickness - currentSliceThickness) > epsilon)
    {
        series->addComputedTagValue("SliceThickness", std::to_string(sliceThickness));
    }

    result.push_back(series);
    return result;
}

//-----------------------------------------------------------------------------

double SliceThicknessModifier::getInstanceZPosition(const core::memory::BufferObject::sptr& bufferObj) const
{
    DcmFileFormat fileFormat;
    DcmDataset* dataset;

    const size_t buffSize = bufferObj->getSize();
    core::memory::BufferObject::Lock lock(bufferObj);
    char* buffer = static_cast<char*>(lock.getBuffer());

    DcmInputBufferStream is;
    is.setBuffer(buffer, offile_off_t(buffSize));
    is.setEos();

    fileFormat.transferInit();
    if(!fileFormat.read(is).good())
    {
        SIGHT_THROW("Unable to read Dicom file '" << bufferObj->getStreamInfo().fsFile.string() << "'");
    }

    fileFormat.loadAllDataIntoMemory();
    fileFormat.transferEnd();

    dataset = fileFormat.getDataset();

    if(!dataset->tagExists(DCM_ImagePositionPatient) || !dataset->tagExists(DCM_ImageOrientationPatient))
    {
        const std::string msg = "Unable to compute the SliceThickness of the series.";
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

    return index;
}

//-----------------------------------------------------------------------------

double SliceThicknessModifier::getSliceThickness(const core::memory::BufferObject::sptr& bufferObj) const
{
    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    const size_t buffSize = bufferObj->getSize();
    core::memory::BufferObject::Lock lock(bufferObj);
    char* buffer = static_cast<char*>(lock.getBuffer());

    DcmInputBufferStream is;
    is.setBuffer(buffer, offile_off_t(buffSize));
    is.setEos();

    fileFormat.transferInit();
    if(!fileFormat.read(is).good())
    {
        SIGHT_THROW("Unable to read Dicom file '" << bufferObj->getStreamInfo().fsFile.string() << "'");
    }

    fileFormat.loadAllDataIntoMemory();
    fileFormat.transferEnd();

    dataset = fileFormat.getDataset();

    double sliceThickness = 0.;
    dataset->findAndGetFloat64(DCM_SliceThickness, sliceThickness);

    return sliceThickness;
}

} // namespace modifier

} // namespace sight::filter::dicom
