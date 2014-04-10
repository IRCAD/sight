/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cmath>

// fwComEd
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>

// fwData
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/Boolean.hpp>

#include "gdcmIO/container/DicomLandmark.hpp"
#include "gdcmIO/helper/DicomData.hpp"

namespace gdcmIO
{
namespace container
{

//------------------------------------------------------------------------------

DicomLandmark::DicomLandmark() :
        m_labelContainer(), m_scoordContainer(), m_referencedFrameContainer()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomLandmark::~DicomLandmark()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomLandmark::addLandmark(const std::string &label, const ::gdcmIO::container::DicomSCoord &point,
            const unsigned int referencedFrame)
{
    m_labelContainer.push_back(label);
    m_scoordContainer.push_back(point);
    m_referencedFrameContainer.push_back(referencedFrame);
}

//------------------------------------------------------------------------------

void DicomLandmark::setFromData(::fwData::Image::csptr image) throw (::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();

    // Get landmarks
    ::fwData::PointList::csptr landmarks = image->getField< ::fwData::PointList >(
            ::fwComEd::Dictionary::m_imageLandmarksId);

    std::vector< ::fwData::Point::sptr > landmarkVector = landmarks->getPoints();
    ::gdcmIO::container::DicomSCoord scoord;
    float graphicData[2];

    std::vector< ::fwData::Point::sptr >::const_iterator iter;
    const std::string separator = ","; // Define separator between referenced frame numbers

    // Initialize variables to add a constrain : 0 <= coord z <= number of frames
    // Number of referenced frame number rejected
    unsigned int illegalRefFrame = 0;

    // Maximum required referenced frame number
    unsigned int refFrameMax = 1;

    float zSpacingInverse = 1;
    ::fwData::Image::OriginType::value_type zOrigin = 0;
    if (image->getNumberOfDimensions() > 2)
    {
        refFrameMax = image->getSize()[2];
        zSpacingInverse = 1. / image->getSpacing()[2];
        zOrigin = image->getOrigin()[2];
    }

    for (iter = landmarkVector.begin(); iter != landmarkVector.end(); ++iter)
    {
        // Referenced frame number - Compute the max legal coord z
        unsigned int referencedFrameNumber =
                floor(((*iter)->getCoord()[2] - zOrigin) * zSpacingInverse + 0.5) + 1; // +1 because frame number start at 1 and point at 0

        if (0 <= referencedFrameNumber && referencedFrameNumber <= refFrameMax)
        {
            // Set label
            std::string label = (*iter)->getField< ::fwData::String >(::fwComEd::Dictionary::m_labelId)->value();

            // Set SCOORD
            graphicData[0] = (*iter)->getCoord()[0]; // x
            graphicData[1] = (*iter)->getCoord()[1]; // y
            scoord.setGraphicData(graphicData, 2);
            scoord.setGraphicType("POINT");

            // Add landmark
            this->addLandmark(label, scoord, referencedFrameNumber);

            OSLM_TRACE("landmark : " << label << " " << graphicData[0] << " / " << graphicData[1]);
        }
        else
        {
            ++illegalRefFrame;
            SLM_ERROR("Landmark with coordinates out of bounds");
        }
    }

    if (illegalRefFrame > 0)
    {
        std::stringstream ss;
        ss << illegalRefFrame << " landmark(s) with coordinates out of bounds";
        throw ::gdcmIO::exception::Failed(ss.str());
    }
}

//------------------------------------------------------------------------------

void DicomLandmark::convertToData(::fwData::Image::sptr image)
{
    SLM_TRACE_FUNC();

    const unsigned int nbLandmarks = this->m_labelContainer.size();
    if (nbLandmarks > 0)
    {
        // Get landmarks
        ::fwComEd::fieldHelper::MedicalImageHelpers::checkLandmarks(image);
        ::fwData::PointList::sptr landmarks = image->getField< ::fwData::PointList >(
                ::fwComEd::Dictionary::m_imageLandmarksId);

        // Compute z spacing
        // NOTE : spacing between slice must be regular
        double spacingOnZ = 1;
        if (image->getNumberOfDimensions() > 2)
        {
            spacingOnZ = image->getSpacing()[2];
        }

        for (unsigned int i = 0; i < nbLandmarks; ++i)
        {
            // Create a new point
            ::fwData::Point::sptr newPoint = ::fwData::Point::New();

            const ::gdcmIO::container::DicomSCoord &scoord = m_scoordContainer[i];
            newPoint->getRefCoord()[0] = scoord[0];
            newPoint->getRefCoord()[1] = scoord[1];

            // Compute z coordinate from referenced frame number and z spacing
            newPoint->getRefCoord()[2] = (m_referencedFrameContainer[i] - 1) * spacingOnZ; // -1 because frame number start at 1 and point at 0

            // Append to landmark
            landmarks->getRefPoints().push_back(newPoint);

            // Append to point the label
            ::fwData::String::sptr label = ::fwData::String::New();
            label->value() = this->getLabelContainer()[i];
            newPoint->setField(::fwComEd::Dictionary::m_labelId, label);

            OSLM_TRACE(
                    "New landmark : " << label->value() << " ( " << newPoint->getRefCoord()[0] << "x"
                    << newPoint->getRefCoord()[1] << "x" << newPoint->getRefCoord()[2] << " )");
        }
        image->setField("ShowLandmarks", ::fwData::Boolean::New(true));
    }
    else
    {
        SLM_TRACE("Any landmark found");
    }
}

//------------------------------------------------------------------------------

} //namespace container
} //namespace gdcmIO
