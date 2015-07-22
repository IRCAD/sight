/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include <fwMath/VectorFunctions.hpp>

#include "fwDicomIOFilter/registry/macros.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/sorter/ImagePositionPatientSorter.hpp"

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
    ::fwDicomData::DicomSeries::sptr series) const throw(::fwDicomIOFilter::exceptions::FilterFailure)
{
    DicomSeriesContainerType result;

    typedef std::map< double, std::string > SortedFileMapType;
    SortedFileMapType sortedFiles;

    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    BOOST_FOREACH(const ::fwDicomData::DicomSeries::DicomPathContainerType::value_type& file,
                  series->getLocalDicomPaths())
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

    ::fwDicomData::DicomSeries::DicomPathContainerType dicomPathContainer;
    series->setLocalDicomPaths(dicomPathContainer);

    unsigned int index = 0;
    BOOST_FOREACH(SortedFileMapType::value_type file, sortedFiles)
    {
        series->addDicomPath(index++, file.second);
    }

    result.push_back(series);
    return result;

}

} // namespace sorter
} // namespace fwDicomIOFilter
