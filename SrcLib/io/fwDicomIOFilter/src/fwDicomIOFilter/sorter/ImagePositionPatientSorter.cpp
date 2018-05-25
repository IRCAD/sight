/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/sorter/ImagePositionPatientSorter.hpp"

#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"

#include <fwMath/VectorFunctions.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::sorter::ImagePositionPatientSorter );

namespace fwDicomIOFilter
{
namespace sorter
{

const std::string ImagePositionPatientSorter::s_FILTER_NAME        = "Image position patient sorter";
const std::string ImagePositionPatientSorter::s_FILTER_DESCRIPTION =
    "Sort instances by computing image position using <i>ImagePositionPatient</i> "
    "and <i>ImageOrientationPatient</i> tags.";

//-----------------------------------------------------------------------------

ImagePositionPatientSorter::ImagePositionPatientSorter(::fwDicomIOFilter::IFilter::Key key) :
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
    const ::fwMedData::DicomSeries::sptr& series,
    const ::fwLog::Logger::sptr& logger) const
{
    DicomSeriesContainerType result;

    typedef std::map< double, ::fwMemory::BufferObject::sptr > SortedDicomMapType;
    SortedDicomMapType sortedDicom;

    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    for(const auto& item :  series->getDicomContainer())
    {
        const ::fwMemory::BufferObject::sptr bufferObj = item.second;
        const size_t buffSize                          = bufferObj->getSize();
        ::fwMemory::BufferObject::Lock lock(bufferObj);
        char* buffer = static_cast< char* >( lock.getBuffer() );

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        fileFormat.transferInit();
        if (!fileFormat.read(is).good())
        {
            FW_RAISE("Unable to read Dicom file '"<< bufferObj->getStreamInfo().fsFile.string() <<"' "<<
                     "(slice: '" << item.first << "')");
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

        dataset = fileFormat.getDataset();

        if(!dataset->tagExists(DCM_ImagePositionPatient) || !dataset->tagExists(DCM_ImageOrientationPatient))
        {
            const std::string msg =
                "Unable to split the series using ImagePositionPatient and ImageOrientationPatient. "
                "Tag(s) missing.";
            throw ::fwDicomIOFilter::exceptions::FilterFailure(msg);
        }

        fwVec3d imagePosition;
        for(unsigned int i = 0; i < 3; ++i)
        {
            dataset->findAndGetFloat64(DCM_ImagePositionPatient, imagePosition[i], i);
        }

        fwVec3d imageOrientationU;
        fwVec3d imageOrientationV;
        for(unsigned int i = 0; i < 3; ++i)
        {
            dataset->findAndGetFloat64(DCM_ImageOrientationPatient, imageOrientationU[i], i);
            dataset->findAndGetFloat64(DCM_ImageOrientationPatient, imageOrientationV[i], i+3);
        }

        //Compute Z direction (cross product)
        const fwVec3d zVector = ::fwMath::cross(imageOrientationU, imageOrientationV);

        //Compute dot product to get the index
        const double index = ::fwMath::dot(imagePosition, zVector);

        sortedDicom[index] = bufferObj;
    }

    if(sortedDicom.size() != series->getDicomContainer().size())
    {
        const std::string msg =
            "Unable to sort the series using the ImagePositionPatient tag. Some images have the same "
            "position meaning this series contains multiple volumes. Try to split the volumes using a different "
            "filter.";
        throw ::fwDicomIOFilter::exceptions::FilterFailure(msg);
    }

    series->clearDicomContainer();
    size_t index = 0;
    for(const auto& item :  sortedDicom)
    {
        series->addBinary(index++, item.second);
    }

    result.push_back(series);

    logger->information("The instances have been sorted using the slices positions.");

    return result;
}

} // namespace sorter
} // namespace fwDicomIOFilter
