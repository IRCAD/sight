/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDcmtkIO/reader/ImageTagReader.hpp"

#include <fwDataTools/helper/Array.hpp>

#include <fwDicomTools/Series.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

#include <algorithm>

namespace fwDcmtkIO
{
namespace reader
{

ImageTagReader::ImageTagReader()
{
}

//-----------------------------------------------------------------------------

ImageTagReader::~ImageTagReader()
{
}

//-----------------------------------------------------------------------------

::fwMedData::Series::sptr ImageTagReader::read(::fwMedData::DicomSeries::sptr series)
{
    DicomPathContainerType instances = series->getLocalDicomPaths();
    DcmFileFormat fileFormat;
    OFCondition status;
    DcmDataset* dataset;

    //Get informations from the first instance
    std::string firstInstance = instances.begin()->second.string();
    status = fileFormat.loadFile(firstInstance.c_str());

    dataset = fileFormat.getDataset();

    // Decompress data set if compressed
    dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);

    //param angle1: LAO/RAO     DICOM IOD (0018, 1510) Positioner Primary Angle
    OFString primaryAngle;
    dataset->findAndGetOFString(DCM_PositionerPrimaryAngle, primaryAngle);
    SLM_WARN_IF("Missing Tag (0018,1510) 'DCM_PositionerPrimaryAngle'", primaryAngle.empty());

    if(!primaryAngle.empty())
    {
        series->addComputedTagValue("PositionerPrimaryAngle", std::string(primaryAngle.c_str()));
    }

    //param angle2: CRA/CAU     DICOM IOD (0018, 1511) Positioner Secondary Angle
    OFString secondaryAngle;
    dataset->findAndGetOFString(DCM_PositionerSecondaryAngle, secondaryAngle);
    SLM_WARN_IF("Missing Tag (0018,1511) 'DCM_PositionerSecondaryAngle'", secondaryAngle.empty());

    if(!secondaryAngle.empty())
    {
        series->addComputedTagValue("PositionerSecondaryAngle", std::string(secondaryAngle.c_str()));
    }

    //param dap:                DICOM IOD (0018, 115e) Image and Fluoroscopy Area Dose Pro DS: '0.022'
    OFString doseProduct;
    dataset->findAndGetOFString(DCM_ImageAndFluoroscopyAreaDoseProduct, doseProduct);
    SLM_WARN_IF("Missing Tag (0018,115e) 'DCM_ImageAndFluoroscopyAreaDoseProduct'", doseProduct.empty());

    if(!doseProduct.empty())
    {
        series->addComputedTagValue("ImageAndFluoroscopyAreaDoseProduct", std::string(doseProduct.c_str()));
    }

    //param dist_src_iso: Distance source to iso-center DICOM IOD (0021, 1017) [Source to Isocenter] SL: 785
    OFString distSrcIso;
    dataset->findAndGetOFString(DcmTagKey(0x0021, 0x1017), distSrcIso);
    SLM_WARN_IF("Missing Tag (0021,10017) 'Distance source iso'", distSrcIso.empty());

    if(!distSrcIso.empty())
    {
        series->addComputedTagValue("DistanceSrcIso", std::string(distSrcIso.c_str()));
    }

    //param dist_src_det: Distance source to detector   DICOM IOD (0018, 1110) Distance Source to Detector   DS: '1200'
    OFString distSrcDetector;
    dataset->findAndGetOFString(DCM_DistanceSourceToDetector, distSrcDetector);
    SLM_WARN_IF("Missing Tag (0018, 1110) 'DCM_DistanceSourceToDetector'", distSrcDetector.empty());

    if(!distSrcDetector.empty())
    {
        series->addComputedTagValue("DistanceSourceToDetector", std::string(distSrcDetector.c_str()));
    }

    //param det_diam: diameter of the detector ( diagonal of the square) #TODO checkit DICOM IOD  0018, 1162)
    // Intensifier Size  DS: '420'
    OFString intensifierSize;
    dataset->findAndGetOFString(DCM_IntensifierSize, intensifierSize);
    SLM_WARN_IF("Missing Tag (0018, 1110) 'DCM_IntensifierSize'", intensifierSize.empty());

    if(!intensifierSize.empty())
    {
        series->addComputedTagValue("IntensifierSize", std::string(intensifierSize.c_str()));
    }

    OFString acqTime;
    dataset->findAndGetOFString(DCM_AcquisitionTime, acqTime);
    SLM_WARN_IF("Missing Tag (0008, 0032) 'DCM_AcquisitionTime'", acqTime.empty());

    if(!acqTime.empty())
    {
        series->addComputedTagValue("AcquisitionTime", std::string(acqTime.c_str()));
    }

    OFString acqDate;
    dataset->findAndGetOFString(DCM_AcquisitionDate, acqDate);
    SLM_WARN_IF("Missing Tag (0008, 0022) 'DCM_AcquisitionDate'", acqDate.empty());

    if(!acqTime.empty())
    {
        series->addComputedTagValue("AcquisitionDate", std::string(acqDate.c_str()));
    }

    return series;
}

//-----------------------------------------------------------------------------

} //reader
} //fwDcmtkIO
