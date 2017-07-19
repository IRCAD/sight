/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/sorter/ImagePositionPatientSorter.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"

#include <fwMath/VectorFunctions.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

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

ImagePositionPatientSorter::ImagePositionPatientSorter(::fwDicomIOFilter::IFilter::Key key) : ISorter()
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
    const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger)
const
{
    DicomSeriesContainerType result;

    typedef std::map< double, std::string > SortedFileMapType;
    SortedFileMapType sortedFiles;

    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    for(const ::fwMedData::DicomSeries::DicomPathContainerType::value_type& file :  series->getLocalDicomPaths())
    {
        const std::string& filename = file.second.string();
        status = fileFormat.loadFile(filename.c_str());
        FW_RAISE_IF("Unable to read the file: \""+filename+"\"", status.bad());
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
        fwVec3d zVector = ::fwMath::cross(imageOrientationU, imageOrientationV);

        //Compute dot product to get the index
        double index = ::fwMath::dot(imagePosition, zVector);

        sortedFiles[index] = filename;
    }

    if(sortedFiles.size() != series->getLocalDicomPaths().size())
    {
        const std::string msg =
            "Unable to sort the series using the ImagePositionPatient tag. Some images have the same "
            "position meaning this series contains multiple volumes. Try to split the volumes using a different "
            "filter.";
        throw ::fwDicomIOFilter::exceptions::FilterFailure(msg);
    }

    ::fwMedData::DicomSeries::DicomPathContainerType dicomPathContainer;
    series->setLocalDicomPaths(dicomPathContainer);

    unsigned int index = 0;
    for(SortedFileMapType::value_type file :  sortedFiles)
    {
        series->addDicomPath(index++, file.second);
    }

    result.push_back(series);

    logger->information("The instances have been sorted using the slices positions.");

    return result;

}

} // namespace sorter
} // namespace fwDicomIOFilter
