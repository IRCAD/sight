/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

// fwComEd
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

#include "gdcmIO/writer/DicomDistanceWriter.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/DicomDictionarySR.hpp"

namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomDistanceWriter::DicomDistanceWriter()
{
    SLM_TRACE_FUNC();

    this->m_distances = ::boost::shared_ptr<DicomDistance>( new DicomDistance );
}

//------------------------------------------------------------------------------

DicomDistanceWriter::~DicomDistanceWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomDistanceWriter::write(::gdcm::DataSet & a_gDs) throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Image::csptr image = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not set", image);

    //*****       Set Distances     *****//
    try
    {
        m_distances->setFromData(image);    // Change distances of image in a DICOM form
    }
    catch(::fwTools::Failed & e)
    {
        throw;
    }

    //*****       Write Distances     *****//

    // Get or create a content sequence in order to insert distances
    const ::gdcm::Tag contentSQTag(0x0040,0xa730);  // Content Sequence
    if ( !a_gDs.FindDataElement(contentSQTag) )
    {// Create a content sequence
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSQ = new ::gdcm::SequenceOfItems();
        helper::GdcmData::setSQ<0x0040,0xa730>(gSQ, a_gDs);
    }
    // Get the content sequence of the root of document SR
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gContentSQ = a_gDs.GetDataElement(contentSQTag).GetValueAsSQ();

    const unsigned int nbDistance = image->getFieldSize( fwComEd::Dictionary::m_imageDistancesId );
    for (unsigned int i = 0; i < nbDistance; ++i)
    {// Write one distance as a child of root of document SR
        this->writeDistance(i, gContentSQ);
    }
}

//------------------------------------------------------------------------------

void DicomDistanceWriter::writeDistance(const unsigned int                              idx,
                                        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > a_gSq)
{
    // Create one item as child
    // See PS 3.16 TID 1400
    ::gdcm::Item        gItMeasure;
    gItMeasure.SetVLToUndefined();
    ::gdcm::DataSet &   gDsMeasure = gItMeasure.GetNestedDataSet();

    // Relationship Value
    helper::GdcmData::setTagValue<0x0040,0xa010>(DicomDictionarySR::getRelationshipString(DicomDictionarySR::CONTAINS), gDsMeasure);    // Type 1

    // Type Value
    helper::GdcmData::setTagValue<0x0040,0xa040>(DicomDictionarySR::getTypeString(DicomDictionarySR::NUM),              gDsMeasure);    //Type 1

    // Concept Name Code Sequence
    helper::DicomTools::createCodeSequence<0x0040,0xa043>(DicomDictionarySR::getCodeValue(DicomDictionarySR::DISTANCE),
                                                          DicomDictionarySR::getCodeScheme(DicomDictionarySR::DISTANCE),
                                                          DicomDictionarySR::getCodeMeaning(DicomDictionarySR::DISTANCE),
                                                          gDsMeasure);          //Type 1C

    // Create a SQ which contains NUM (measured value sequence)
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqNum = new ::gdcm::SequenceOfItems();
    gSqNum->SetLengthToUndefined();
    {
        //*** Create a measured value ***//
        ::gdcm::Item        gItNum;
        gItNum.SetVLToUndefined();
        ::gdcm::DataSet &   gDsNum = gItNum.GetNestedDataSet();

        // Add numerical value
        const double        numValue = atof(m_distances->getDists()[ idx ].c_str());
        // Numeric Value
        helper::GdcmData::setTagValues<double,0x0040,0xa30a>(&numValue, 1, gDsNum); // Type 1

        // Add measured units code sequence
        helper::DicomTools::createCodeSequence<0x0040,0x08ea>("mm", "UCUM", "millimeter", "1.4", gDsNum);

        // Add Measured value to the sequence
        gSqNum->AddItem(gItNum);
    }
    // Add Measured value sequence to the data set
    helper::GdcmData::setSQ<0x0040,0xa300>(gSqNum, gDsMeasure); // Measured Value Sequence

    // Add Spatial COORDinates (SCOORD)
    ::boost::shared_ptr< DicomInstance >    dicomInstance           = this->getDicomInstance();
    SCoord &                                currentSCOORD           = m_distances->getRefSCoords()[ idx ];
    const std::string &                     referencedFrameNumbers  = m_distances->getCRefRefFrames()[ idx ];

    // Split referenced frame numbers into 2 string
    std::vector<std::string> splitedRefFrame;
    ::boost::split( splitedRefFrame, referencedFrameNumbers, ::boost::is_any_of( "," ) );  // Get each referenced frame number

    SLM_ASSERT("Referenced frame numbers are corrupted", splitedRefFrame.size() == 2);

    // Get the graphic data of 2 points
    const std::vector< float >      graphicData    = currentSCOORD.getGraphicData();

    // If the image storage is multi-frame
    if (dicomInstance->getIsMultiFrame())
    {
        currentSCOORD.setContext( DicomCodedAttribute("121055", "DCM", "Path") );

        // Split graphic data into 2 points
        float                           graphicDataTmp[2];

        // Get first point
        graphicDataTmp[0] = graphicData[0]; graphicDataTmp[1] = graphicData[1];
        currentSCOORD.setGraphicData(graphicDataTmp, 2);
        // Write first SCOORD
        helper::DicomSR::createSCOORD(currentSCOORD,
                                      splitedRefFrame[0],
                                      dicomInstance->getCRefSOPClassUID(),
                                      dicomInstance->getCRefSOPInstanceUID(),
                                      gDsMeasure);

        // Get second point
        graphicDataTmp[0] = graphicData[2]; graphicDataTmp[1] = graphicData[3];
        currentSCOORD.setGraphicData(graphicDataTmp, 2);
        // Write second SCOORD
        helper::DicomSR::createSCOORD(currentSCOORD,
                                      splitedRefFrame[1],
                                      dicomInstance->getCRefSOPClassUID(),
                                      dicomInstance->getCRefSOPInstanceUID(),
                                      gDsMeasure);
    }
    else
    {// else (image save in several files)
        currentSCOORD.setGraphicType(DicomDictionarySR::POINT);
        currentSCOORD.setContext( DicomCodedAttribute("121230", "DCM", "Path Vertex") );

        // Split graphic data into 2 points
        float                           graphicDataTmp[2];

        // Get first point
        graphicDataTmp[0] = graphicData[0]; graphicDataTmp[1] = graphicData[1];
        currentSCOORD.setGraphicData(graphicDataTmp, 2);
        // Write first SCOORD
        helper::DicomSR::createSCOORD(currentSCOORD,
                                      dicomInstance->getCRefSOPClassUID(),
                                      dicomInstance->getCRefReferencedSOPInstanceUIDs()[ atoi(splitedRefFrame[0].c_str()) -1 ], // -1 because frame number start at 1
                                      gDsMeasure);

        // Get second point
        graphicDataTmp[0] = graphicData[2]; graphicDataTmp[1] = graphicData[3];
        currentSCOORD.setGraphicData(graphicDataTmp, 2);
        // Write second SCOORD
        helper::DicomSR::createSCOORD(currentSCOORD,
                                      dicomInstance->getCRefSOPClassUID(),
                                      dicomInstance->getCRefReferencedSOPInstanceUIDs()[ atoi(splitedRefFrame[1].c_str()) -1 ], // -1 because frame number start at 1
                                      gDsMeasure);
    }

    // Add the distance to the content sequence of root
    a_gSq->AddItem(gItMeasure);
}

}   // namespace writer

}   // namespace gdcmIO
