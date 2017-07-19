/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/registry/macros.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/modifier/SliceThicknessModifier.hpp"

#include <fwMath/VectorFunctions.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmimgle/dcmimage.h>

#include <boost/lexical_cast.hpp>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::modifier::SliceThicknessModifier );

namespace fwDicomIOFilter
{
namespace modifier
{

const std::string SliceThicknessModifier::s_FILTER_NAME        = "Slice thickness modifier";
const std::string SliceThicknessModifier::s_FILTER_DESCRIPTION =
    "Compute and modify slice thickness using <i>ImagePositionPatient</i> "
    "and <i>ImageOrientationPatient</i> tags of the two first instances.";

//-----------------------------------------------------------------------------

SliceThicknessModifier::SliceThicknessModifier(::fwDicomIOFilter::IFilter::Key key) : IModifier()
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
    const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger)
const
{
    DicomSeriesContainerType result;

    if(series->getLocalDicomPaths().size() < 2)
    {
        SLM_WARN("SliceThicknessModifier is being applied on a series containing only one slice.");
        result.push_back(series);
        return result;
    }

    // Retrieve the two first instances
    ::fwMedData::DicomSeries::DicomPathContainerType::const_iterator it = series->getLocalDicomPaths().begin();
    const std::string& firstFile  = it->second.string();
    const std::string& secondFile = (++it)->second.string();

    // Compute the slice thickness between the 2 first slices.
    double firstIndex     = this->getInstanceZPosition(firstFile);
    double secondIndex    = this->getInstanceZPosition(secondFile);
    double sliceThickness = std::abs(secondIndex - firstIndex);

    // Check that the computed sliceThickness doesn't match the sliceThickness of the first instance
    double currentSliceThickness = this->getSliceThickness(firstFile);
    const double epsilon         = 1e-2;

    // If the computed sliceThickness doesn't match the sliceThickness value
    // we add the computed value to the DicomSeries.
    if(std::abs(sliceThickness-currentSliceThickness) > epsilon)
    {
        series->addComputedTagValue("SliceThickness", boost::lexical_cast<std::string>(sliceThickness));
    }

    result.push_back(series);
    return result;

}

//-----------------------------------------------------------------------------

double SliceThicknessModifier::getInstanceZPosition(const std::string& file) const
{
    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    status = fileFormat.loadFile(file.c_str());
    FW_RAISE_IF("Unable to read the file: \""+file+"\"", status.bad());
    dataset = fileFormat.getDataset();

    if(!dataset->tagExists(DCM_ImagePositionPatient) || !dataset->tagExists(DCM_ImageOrientationPatient))
    {
        const std::string msg = "Unable to compute the SliceThickness of the series.";
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

    return index;
}

//-----------------------------------------------------------------------------

double SliceThicknessModifier::getSliceThickness(const std::string& file) const
{
    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    status = fileFormat.loadFile(file.c_str());
    FW_RAISE_IF("Unable to read the file: \""+file+"\"", status.bad());
    dataset = fileFormat.getDataset();

    double sliceThickness = 0;
    dataset->findAndGetFloat64(DCM_SliceThickness,sliceThickness);

    return sliceThickness;
}

} // namespace modifier
} // namespace fwDicomIOFilter
