/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
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

#include "gdcmIO/DicomLandmark.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"
#include "gdcmIO/DicomDictionarySR.hpp"

namespace gdcmIO
{

//------------------------------------------------------------------------------

DicomLandmark::DicomLandmark():
        m_labels(),
        m_SCoords(),
        m_refFrames()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomLandmark::~DicomLandmark()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomLandmark::convertToData(::fwData::Image::sptr a_image)
{
    SLM_TRACE_FUNC();

    const unsigned int nb_landmarks = this->m_labels.size();
    if (nb_landmarks > 0)
    {
        // Get landmarks
        ::fwComEd::fieldHelper::MedicalImageHelpers::checkLandmarks( a_image );
        ::fwData::PointList::sptr landmarks = a_image->getFieldSingleElement< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId);

        // Compute z spacing
        // NOTE : spacing between slice must be regular
        double spacingOnZ = 1;
        if (a_image->getDimension() > 2)
            spacingOnZ = a_image->getSpacing()[2];

        for (unsigned int i = 0; i < nb_landmarks; ++i)
        {
            // create a new point
            ::fwData::Point::NewSptr   newPoint;

            const SCoord & scoord = m_SCoords[i];
            newPoint->getRefCoord()[0] = scoord[0];
            newPoint->getRefCoord()[1] = scoord[1];

            // Compute z coordinate from referenced frame number and z spacing
            newPoint->getRefCoord()[2] = (m_refFrames[i] - 1)* spacingOnZ; // -1 because frame number start at 1 and point at 0

            // append to landmark
            landmarks->getRefPoints().push_back( newPoint );

            // append to point the label
            ::fwData::String::NewSptr label;
            label->value() = this->getLabels()[i];
            newPoint->setFieldSingleElement( ::fwComEd::Dictionary::m_labelId , label );

            OSLM_TRACE("new landmark : "<<label->value()<<" ( "<<newPoint->getRefCoord()[0]<<"x"<<newPoint->getRefCoord()[1]<<"x"<<newPoint->getRefCoord()[2]<<" )");
        }
        a_image->setFieldSingleElement("ShowLandmarks", ::fwData::Boolean::NewSptr(true));
    }
    else
    {
        OSLM_TRACE("Any landmark found");
    }
}

//------------------------------------------------------------------------------

void DicomLandmark::setFromData(::fwData::Image::csptr a_image) throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Get landmarks
    ::fwData::PointList::csptr landmarks =  a_image->getFieldSingleElement< ::fwData::PointList >( ::fwComEd::Dictionary::m_imageLandmarksId);

    std::vector< ::fwData::Point::sptr >    vLandmarks = landmarks->getPoints();
    SCoord              scoord;
    float               graphicData[2];

    std::vector< ::fwData::Point::sptr >::const_iterator iter;
    std::stringstream   strm;
    const std::string   separator = ",";    // Define separator between referenced frame numbers

    // Initialize variables to add a constrain : 0 <= coord z <= number of frames
    unsigned int illegalRefFrame    = 0;    // Number of referenced frame number rejected
    unsigned int refFrameMax        = 1;    // Maximum required referenced frame number
    unsigned int refFrame;                  // Referenced frame number
    float        zSpacingInverse    = 1;
    if (a_image->getDimension() > 2)
    {
        refFrameMax     = a_image->getSize()[2];
        zSpacingInverse = 1. / a_image->getSpacing()[2];
    }

    for (iter = vLandmarks.begin() ; iter != vLandmarks.end() ; ++iter)
    {
        // Compute the max legal coord z
        refFrame = floor((*iter)->getCoord()[2] * zSpacingInverse + 0.5) + 1;    // +1 because frame number start at 1 and point at 0

        if ( 0 < refFrame && refFrame <= refFrameMax )
        {
            // Set label
            m_labels.push_back( (*iter)->getFieldSingleElement< ::fwData::String >(::fwComEd::Dictionary::m_labelId)->value() );

            // Set SCOORD
            graphicData[0] = (*iter)->getCoord()[0];    // x
            graphicData[1] = (*iter)->getCoord()[1];    // y
            scoord.setGraphicData( graphicData, 2);
            scoord.setGraphicType( DicomDictionarySR::getGraphicTypeString( DicomDictionarySR::POINT ) );

            m_SCoords.push_back( scoord );

            // Set referenced frame
            m_refFrames.push_back( refFrame );

            OSLM_TRACE("landmark : "<<(*iter)->getFieldSingleElement< ::fwData::String >(::fwComEd::Dictionary::m_labelId)->value()<<" "<<strm.str());
        }
        else
        {
            ++illegalRefFrame;
            OSLM_ERROR("Landmark with coordinates out of bounds")
        }
    }

    if (illegalRefFrame > 0)
    {
        strm.str("");   strm << illegalRefFrame << " landmark(s) with coordinates out of bounds";
        throw ::fwTools::Failed(strm.str());
    }
}

//------------------------------------------------------------------------------

void DicomLandmark::addLabel(const std::string & a_label)
{
    this->m_labels.push_back(a_label);
}

//------------------------------------------------------------------------------

void DicomLandmark::addSCoord(const SCoord & a_point)
{
    this->m_SCoords.push_back(a_point);
}

//------------------------------------------------------------------------------

void DicomLandmark::addRefFrame(const unsigned int a_refFrame)
{
    this->m_refFrames.push_back(a_refFrame);
}

}
