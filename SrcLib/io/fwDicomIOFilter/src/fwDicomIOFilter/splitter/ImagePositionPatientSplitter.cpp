/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/registry/macros.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/splitter/ImagePositionPatientSplitter.hpp"

#include <fwMath/VectorFunctions.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::ImagePositionPatientSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string ImagePositionPatientSplitter::s_FILTER_NAME        = "Image position patient splitter";
const std::string ImagePositionPatientSplitter::s_FILTER_DESCRIPTION =
    "Split instances by finding gaps in image position continuity. This filter assume that "
    "the instances are <b>already sorted</b> and only gaps between volumes remain.";

//-----------------------------------------------------------------------------

ImagePositionPatientSplitter::ImagePositionPatientSplitter(::fwDicomIOFilter::IFilter::Key key) : ISplitter()
{
}

//-----------------------------------------------------------------------------

ImagePositionPatientSplitter::~ImagePositionPatientSplitter()
{
}

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
    const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger)
const throw(::fwDicomIOFilter::exceptions::FilterFailure)
{
    DicomSeriesContainerType result;

    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    double previousIndex        = 0;
    unsigned int instanceNumber = 0;
    double spacingBetweenSlices = 0;
    const double epsilon        = 1e-2; // Value used to find a gap
    ::fwMedData::DicomSeries::sptr currentSeries;
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

        //Compute spacing
        double spacing = index - previousIndex;
        if(currentSeries && spacingBetweenSlices == 0)
        {
            spacingBetweenSlices = spacing;
        }

        // First frame or volume detected: We create a new Series
        if(!currentSeries || (fabs(spacing-spacingBetweenSlices) > epsilon) )
        {
            if(currentSeries)
            {
                result.push_back(currentSeries);
                currentSeries->setNumberOfInstances((unsigned int)(currentSeries->getLocalDicomPaths().size()));
            }
            instanceNumber = 0;
            currentSeries  = ::fwMedData::DicomSeries::New();
            currentSeries->deepCopy(series);
            ::fwMedData::DicomSeries::DicomPathContainerType dicomPathContainer;
            currentSeries->setLocalDicomPaths(dicomPathContainer);
        }

        currentSeries->addDicomPath(instanceNumber++, filename);
        previousIndex = index;

    }

    // Push last series created
    result.push_back(currentSeries);
    currentSeries->setNumberOfInstances((unsigned int)(currentSeries->getLocalDicomPaths().size()));

    if(result.size() > 1)
    {
        logger->warning("Series has been split according to slice positions.");
    }

    return result;

}

} // namespace splitter
} // namespace fwDicomIOFilter
