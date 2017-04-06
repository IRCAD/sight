/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDcmtkIO/reader/MPPSReader.hpp"

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

//-----------------------------------------------------------------------------

OFString searchTagInSequence(DcmDataset* dataset, const DcmTagKey& subSeq, const DcmTagKey& tag)
{
    OFString tagValue = "";
    dataset->chooseRepresentation(EXS_LittleEndianExplicit, NULL);

    DcmSequenceOfItems* sequenceOfIt;
    dataset->findAndGetSequence(subSeq, sequenceOfIt);
    SLM_WARN_IF("No sequence found in 'DCM_ReferencedReferenceImageSequence'.", !sequenceOfIt);

    if(sequenceOfIt)
    {
        const unsigned long lastEl = sequenceOfIt->card() - 1;

        DcmItem* item = sequenceOfIt->getItem(lastEl);
        item->findAndGetOFString(tag, tagValue);
    }

    return tagValue;
}

//-----------------------------------------------------------------------------

MPPSReader::MPPSReader()
{
}

//-----------------------------------------------------------------------------

MPPSReader::~MPPSReader()
{
}

//-----------------------------------------------------------------------------

::fwMedData::Series::sptr MPPSReader::read(::fwMedData::DicomSeries::sptr series)
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

    OFString mmpsStatus;
    dataset->findAndGetOFString(DCM_PerformedProcedureStepStatus, mmpsStatus);
    SLM_ASSERT("Missing tag (0040,0252) 'PerformedProcedureStepStatus'.", !mmpsStatus.empty());
    series->addComputedTagValue("PerformedProcedureStepStatus", std::string(mmpsStatus.c_str()));

    OFString entranceDosemGy;
    dataset->findAndGetOFString(DCM_EntranceDoseInmGy, entranceDosemGy);
    if(!entranceDosemGy.empty())
    {
        series->addComputedTagValue("EntranceDoseInmGy", std::string(entranceDosemGy.c_str()));
    }

    DcmSequenceOfItems* sequenceOfIt;
    dataset->findAndGetSequence(DCM_ExposureDoseSequence, sequenceOfIt);
    SLM_WARN_IF("No sequence found in 'DCM_ExposureDoseSequence'.", !sequenceOfIt);

    if(sequenceOfIt)
    {
        unsigned long lastEl = sequenceOfIt->card() - 1;

        DcmItem* item = sequenceOfIt->getItem(lastEl);

        OFString commentsRadiationDose;
        item->findAndGetOFString(DCM_CommentsOnRadiationDose, commentsRadiationDose);

        if(!commentsRadiationDose.empty())
        {
            series->addComputedTagValue("CommentsOnRadiationDose", std::string(commentsRadiationDose.c_str()));
        }

        //(0018,0060) DS [77]#   2, 1 KVP
        OFString kvp;
        item->findAndGetOFString(DCM_KVP, kvp);
        if(!kvp.empty())
        {
            series->addComputedTagValue("KVP", std::string(kvp.c_str()));
        }

        //(0018,1150) IS [25]# 2, 1 ExposureTime
        OFString exposureTime;
        item->findAndGetOFString(DCM_ExposureTime, exposureTime);
        if(!exposureTime.empty())
        {
            series->addComputedTagValue("ExposureTime", std::string(exposureTime.c_str()));
        }

        //(0018,115a) CS [PULSED]#   6, 1 RadiationMode
        OFString radiationMode;
        item->findAndGetOFString(DCM_RadiationMode, radiationMode);
        if(!radiationMode.empty())
        {
            series->addComputedTagValue("RadiationMode", std::string(radiationMode.c_str()));
        }

        //(0018,8151) DS [23500] # 6, 1 XRayTubeCurrentInuA
        OFString xRayTubeCurrentInuA;
        item->findAndGetOFString(DCM_XRayTubeCurrentInuA, xRayTubeCurrentInuA);

        if(!xRayTubeCurrentInuA.empty())
        {
            series->addComputedTagValue("XRayTubeCurrentInuA", std::string(xRayTubeCurrentInuA.c_str()));
        }
    }

    // PerformedSeriesSequence, allows to find the associated image
    OFString performedSeriesSequence;
    performedSeriesSequence = searchTagInSequence(dataset, DCM_PerformedSeriesSequence, DCM_SeriesInstanceUID);

    if(!performedSeriesSequence.empty())
    {
        series->addComputedTagValue("PerformedSeriesSequence", std::string(performedSeriesSequence.c_str()));
    }

    return series;
}

//-----------------------------------------------------------------------------

} //reader
} //fwDcmtkIO
