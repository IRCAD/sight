/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cmath>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

// fwComEd
#include <fwComEd/Dictionary.hpp>

// fwData
#include <fwData/Vector.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Boolean.hpp>

#include "gdcmIO/container/DicomDistance.hpp"
#include "gdcmIO/helper/DicomData.hpp"

namespace gdcmIO
{
namespace container
{

//------------------------------------------------------------------------------

DicomDistance::DicomDistance() :
        m_scoordContainer(), m_distanceContainer(), m_referencedFrameContainer()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomDistance::~DicomDistance()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomDistance::addDistance(const std::string &distance, const ::gdcmIO::container::DicomSCoord &scoord,
        const std::string &referencedFrame)
{
    m_distanceContainer.push_back(distance);
    m_scoordContainer.push_back(scoord);
    m_referencedFrameContainer.push_back(referencedFrame);
}

//------------------------------------------------------------------------------

void DicomDistance::setFromData(::fwData::Image::csptr image) throw (::gdcmIO::exception::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::Vector::sptr vectDist;
    vectDist = image->getField< ::fwData::Vector >(::fwComEd::Dictionary::m_imageDistancesId);
    if (vectDist)
    {
        // Initialize
        ::fwData::Point::csptr point1, point2; // Point of the distance
        ::fwData::PointList::PointListContainer distance;
        float z1, z2; // z coordinates of points

        // Define separator between referenced frame numbers
        const std::string separator = ",";

        // Spatial coordinates which contains x and y coordinates
        ::gdcmIO::container::DicomSCoord scoord;

        // x, y coordinates
        float graphicData[4];

        // Add a constraint on "z" coordinate
        // Number of referenced frame number rejected
        unsigned int illegalRefFrame = 0;

        // mMximum required referenced frame number
        unsigned int refFrameMax = 1;
        float zSpacingInverse = 1;
        if (image->getNumberOfDimensions() > 2)
        {
            refFrameMax = image->getSize()[2];
            zSpacingInverse = 1. / image->getSpacing()[2];
        }

        BOOST_FOREACH(::fwData::Object::sptr obj, *vectDist)
        {
            ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(obj);
            SLM_ASSERT("PointList dynamicCast failed", pointList);
            distance = pointList->getPoints();

            // Set each point
            point1 = distance[0];
            point2 = distance[1];

            graphicData[0] = point1->getCoord()[0]; graphicData[1] =
                    point1->getCoord()[1]; z1 = point1->getCoord()[2] * zSpacingInverse;
            graphicData[2] = point2->getCoord()[0]; graphicData[3] =
                    point2->getCoord()[1]; z2 = point2->getCoord()[2] * zSpacingInverse;

            // If "z" could be a referenced frame
            if (z1 >= 0 && z1 < refFrameMax && z2 >= 0 && z2 < refFrameMax)
            {
                // Set SCOORD
                scoord.setGraphicData( graphicData, 4);
                scoord.setGraphicType( "POLYLINE" );
                m_scoordContainer.push_back( scoord );


                // Convert "z" coordinate to a frame number.
                std::stringstream ssReferencedFrame;
                ssReferencedFrame << floor( z1 + 1 + 0.5) << separator << floor( z2 + 1 + 0.5);// +1 because frame number start at 1
                std::string referencedFrame = ssReferencedFrame.str();


                // Compute the distance value (Euclidienne)
                std::stringstream ssDistance;
                ssDistance << sqrt( (scoord[0] - scoord[2]) * (scoord[0] - scoord[2]) +
                        (scoord[1] - scoord[3]) * (scoord[1] - scoord[3]) +
                        (z1 - z2) * (z1 - z2) );
                std::string distance = ssDistance.str();

                OSLM_TRACE("SCOORD : " << scoord[0] << "x" << scoord[1] << "x" << scoord[2] << "x" << scoord[3]);
                OSLM_TRACE("Referenced frame : " << referencedFrame);
                OSLM_TRACE("Distance : " << distance);

                // Add distance
                this->addDistance(distance, scoord, referencedFrame);
            }
            else
            {
                ++illegalRefFrame;
            }
        }

        if (illegalRefFrame > 0)
        {
            std::stringstream ssIllegalReferencedFrame;
            ssIllegalReferencedFrame << illegalRefFrame << " distance(s) with coordinates out of bounds";
            throw ::gdcmIO::exception::Failed(ssIllegalReferencedFrame.str());
        }
    }
}

//------------------------------------------------------------------------------

void DicomDistance::convertToData(::fwData::Image::sptr image)
{
    SLM_TRACE_FUNC();

    // get number of distances
    const unsigned int nbDistances = this->m_referencedFrameContainer.size();

    if (nbDistances > 0)
    {
        ::fwData::Vector::sptr vectDist;
        vectDist = image->setDefaultField(::fwComEd::Dictionary::m_imageDistancesId, ::fwData::Vector::New());

        // Define separator between referenced frame numbers
        const std::string separator = ",";

        // Compute z spacing
        // NOTE : spacing between slice must be regular
        double spacingOnZ = 1;
        if (image->getNumberOfDimensions() > 2)
        {
            spacingOnZ = image->getSpacing()[2];
        }

        // For each distance
        for (unsigned int i = 0; i < nbDistances; ++i)
        {
            // Create a new distance
            ::fwData::PointList::sptr pointList = ::fwData::PointList::New();

            // Create 2 points
            ::fwData::Point::sptr point1 = ::fwData::Point::New();
            ::fwData::Point::sptr point2 = ::fwData::Point::New();

            // Set x, y coordinates
            const ::gdcmIO::container::DicomSCoord &scoord = m_scoordContainer[i];

            point1->getRefCoord()[0] = scoord[0];
            point1->getRefCoord()[1] = scoord[1];

            point2->getRefCoord()[0] = scoord[2];
            point2->getRefCoord()[1] = scoord[3];

            // Set z coordinates
            const std::string &refFrames = m_referencedFrameContainer[i];
            const size_t pos = refFrames.find(separator); // Get each slice number

            point1->getRefCoord()[2] = (atof(refFrames.substr(0, pos).c_str()) - 1) * spacingOnZ; // -1 because frame number start at 1 and point at 0
            point2->getRefCoord()[2] = (atof(refFrames.substr(pos + 1).c_str()) - 1) * spacingOnZ;

            // Add points to the distance
            pointList->getRefPoints().push_back(point1);
            pointList->getRefPoints().push_back(point2);

            // Add the distance in the image
            vectDist->getContainer().push_back(pointList);

            OSLM_TRACE("new distance : ( " << point1->getRefCoord()[0] <<
                    "x" << point1->getRefCoord()[1] << "x" << point1->getRefCoord()[2] << " - "  <<
                    point2->getRefCoord()[0] <<
                    "x" << point2->getRefCoord()[1] << "x" << point2->getRefCoord()[2] << " )");
        }

        // Force distance to be shown
        image->setField("ShowDistances", ::fwData::Boolean::New(true));
    }
    else
    {
        OSLM_TRACE("Any distance found");
    }
}

//------------------------------------------------------------------------------


} //namespace container
} //namespace gdcmIO
