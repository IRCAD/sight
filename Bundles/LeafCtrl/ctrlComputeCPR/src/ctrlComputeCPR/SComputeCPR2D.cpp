/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlComputeCPR/SComputeCPR2D.hpp"


#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCom/Slots.hxx>
#include <fwComEd/Dictionary.hpp>

#include <fwData/String.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <cpr/functions.hpp>

#include <math.h>
#include <vector>

fwServicesRegisterMacro(::fwServices::IController,::ctrlComputeCPR::SComputeCPR2D, ::fwData::Image);

namespace ctrlComputeCPR
{

const ::fwCom::Slots::SlotKeyType SComputeCPR2D::s_CHANGE_HEIGHT_SLOT  = "changeHeight";
const ::fwCom::Slots::SlotKeyType SComputeCPR2D::s_CHANGE_SPACING_SLOT = "changeSpacing";
const ::fwCom::Slots::SlotKeyType SComputeCPR2D::s_CHANGE_ANGLE_SLOT   = "changeAngle";
const ::fwCom::Slots::SlotKeyType SComputeCPR2D::s_SELECT_POINT_SLOT   = "selectPoint";

static const ::fwCom::Slots::SlotKeyType s_ADD_POINT_SLOT     = "addPoint";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_POINT_SLOT  = "removePoint";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SPLINE_SLOT = "updateSpline";

//----------------------------------------------------------------------------------------------------------------

SComputeCPR2D::SComputeCPR2D() throw ()
    : m_nbSplinePoints(0),
      m_angle(0),
      m_height(50.0),
      m_selectedPointIndex(0)
{
    m_slotChangeHeight  = newSlot(s_CHANGE_HEIGHT_SLOT, &SComputeCPR2D::setHeight, this);
    m_slotChangeSpacing = newSlot(s_CHANGE_SPACING_SLOT, &SComputeCPR2D::setSpacing, this);
    m_slotChangeAngle   = newSlot(s_CHANGE_ANGLE_SLOT, &SComputeCPR2D::setNormalRotation, this);
    m_slotSelectPoint   = newSlot(s_SELECT_POINT_SLOT, &SComputeCPR2D::fillVisualizePointList, this);
    newSlot(s_ADD_POINT_SLOT, &SComputeCPR2D::addPoint, this);
    newSlot(s_REMOVE_POINT_SLOT, &SComputeCPR2D::removePoint, this);
    newSlot(s_UPDATE_SPLINE_SLOT, &SComputeCPR2D::updateSpline, this);
}

//----------------------------------------------------------------------------------------------------------------

SComputeCPR2D::~SComputeCPR2D() throw ()
{
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::starting() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Get the source image to set the height and spacing of the CPR.
    ::fwTools::Object::sptr imgObj = ::fwTools::fwID::getObject(m_sourceImageUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_sourceImageUID << "'", imgObj);
    ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(imgObj);
    OSLM_ASSERT("Failed to retrieve image", image);

    m_spacing = ::cpr::getImageMinSpacing(image);
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::stopping() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::configuring() throw (fwTools::Failed)
{
    SLM_TRACE_FUNC();

    const std::vector<ConfigurationType>& splinePointsConfig = m_configuration->find("splinePoints");
    if (!splinePointsConfig.empty())
    {
        SLM_ASSERT ("UID attribute is missing", splinePointsConfig.at(0)->hasAttribute("uid"));
        m_splinePointsUID = (splinePointsConfig.at(0)->getAttributeValue("uid"));
    }

    // Get the source image
    const std::vector<ConfigurationType>& sourceImageConfig = m_configuration->find("sourceImage");
    if (!sourceImageConfig.empty())
    {
        SLM_ASSERT("UID attribute is missing", sourceImageConfig.at(0)->hasAttribute("uid"));
        m_sourceImageUID = (sourceImageConfig.at(0)->getAttributeValue("uid"));
    }

    // Get the point list to visualize points on the cpr image
    const std::vector<ConfigurationType>& visuPointsConfig = m_configuration->find("visuPoints");
    if (!visuPointsConfig.empty())
    {
        SLM_ASSERT("UID attribute is missing", visuPointsConfig.at(0)->hasAttribute("uid"));
        m_visuPointsUID = (visuPointsConfig.at(0)->getAttributeValue("uid"));
    }
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::updating() throw (::fwTools::Failed)
{
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::addPoint(::fwData::Point::sptr /*point*/)
{
    ::fwTools::Object::sptr splineObj = ::fwTools::fwID::getObject(m_splinePointsUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_splinePointsUID << "'", splineObj);
    ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(splineObj);
    OSLM_ASSERT("Failed to retrieve point list", pointList);

    m_nbSplinePoints = pointList->getRefPoints().size();
    this->updateSpline();
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::removePoint(::fwData::Point::sptr /*point*/)
{
    ::fwTools::Object::sptr splineObj = ::fwTools::fwID::getObject(m_splinePointsUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_splinePointsUID << "'", splineObj);
    ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(splineObj);
    OSLM_ASSERT("Failed to retrieve point list", pointList);

    m_nbSplinePoints = pointList->getRefPoints().size();
    this->clearVisualizePointList();

    // Visualize the last points
    if (m_nbSplinePoints > 0)
    {
        this->fillVisualizePointList(m_nbSplinePoints - 1);
    }
    this->updateSpline();
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::updateSpline()
{
    if(m_nbSplinePoints == 2)
    {
        this->computeCPRImage();
        this->fillVisualizePointList(m_selectedPointIndex);
    }
    else if (m_nbSplinePoints > 2)
    {
        this->computeCPRImage();
    }
    if(m_nbSplinePoints < 3) // Set an empty image
    {
        ::fwData::Image::sptr image = this->getObject< ::fwData::Image>();
        SLM_ASSERT("Image not valid", image);

        ::fwData::Image::SizeType imageSize(3);
        imageSize[0] = 0;
        imageSize[1] = 0;
        imageSize[2] = 0;

        image->setSize(imageSize);

        auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::computeCPRImage()
{
    ::fwTools::Object::sptr splineObj = ::fwTools::fwID::getObject(m_splinePointsUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_splinePointsUID << "'", splineObj);
    ::fwTools::Object::sptr imageObj = ::fwTools::fwID::getObject(m_sourceImageUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_sourceImageUID << "'", imageObj);

    ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(splineObj);
    OSLM_ASSERT("Failed to retrieve point list", pointList);
    ::fwData::Image::sptr imageSource = ::fwData::Image::dynamicCast(imageObj);
    OSLM_ASSERT("Failed to retrieve image", imageSource);

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    OSLM_ASSERT("Failed to retrieve associated image", image);

    // Init grids
    unsigned int nbRow;
    unsigned int nbCol;
    std::vector<double> pointGrid;
    std::vector<ImagePixelType> colorGrid;

    // Fill point grid
    ::cpr::fillPointGrid(m_spacing, m_height, pointList, pointGrid, nbCol, nbRow, m_angle);

    // Fill color grid
    ::cpr::fillColorGrid(pointGrid, nbCol, nbRow, imageSource, colorGrid);

    // Fill image
    ::cpr::fillImage(colorGrid, nbCol, nbRow, m_spacing, image);
    OSLM_DEBUG("Number of pixels in length " << nbCol);
    OSLM_DEBUG("Number of pixels in height " << nbRow);

    // Notify
    auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::setHeight(double height)
{
    m_height = height;

    if(m_nbSplinePoints > 2 && m_selectedPointIndex > 0)
    {
        this->computeCPRImage();
        this->fillVisualizePointList(m_selectedPointIndex);
    }
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::setSpacing(double spacing)
{
    m_spacing = spacing;

    if(m_nbSplinePoints > 2)
    {
        this->computeCPRImage();
    }
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::setNormalRotation(double angle)
{
    m_angle = angle;

    if(m_nbSplinePoints > 2)
    {
        this->computeCPRImage();
    }
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::fillVisualizePointList(int selectedPointIndex)
{
    m_selectedPointIndex = selectedPointIndex;

    ::fwTools::Object::sptr splineObj = ::fwTools::fwID::getObject(m_splinePointsUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_splinePointsUID << "'", splineObj);
    ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(splineObj);
    OSLM_ASSERT("Failed to retrieve point list", pointList);

    // Get the PointList to visualize points on the cpr image.
    ::fwTools::Object::sptr cprObj = ::fwTools::fwID::getObject(m_visuPointsUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_visuPointsUID << "'", cprObj);
    ::fwData::PointList::sptr visualizePointList = ::fwData::PointList::dynamicCast(cprObj);
    OSLM_ASSERT("Failed to retrieve point list", visualizePointList);

    // Clear the pointList if there are already points in
    this->clearVisualizePointList();

    if(selectedPointIndex < pointList->getRefPoints().size() && selectedPointIndex >= 0)
    {
        this->addPointToVisualizePointList(pointList, selectedPointIndex, visualizePointList);
    }

    // if the selected point is the last point visualize the 2 points before
    if(selectedPointIndex == m_nbSplinePoints - 1)
    {
        if(m_nbSplinePoints == 2)
        {
            this->addPointToVisualizePointList(pointList, selectedPointIndex - 1, visualizePointList);
        }
        else if (m_nbSplinePoints >= 3)
        {
            this->addPointToVisualizePointList(pointList, selectedPointIndex - 1, visualizePointList);
            this->addPointToVisualizePointList(pointList, selectedPointIndex - 2, visualizePointList);
        }
    }
    else // visualize the 2 neighbors of selected points
    {
        if(selectedPointIndex > 0 && selectedPointIndex + 1 < m_nbSplinePoints)
        {
            this->addPointToVisualizePointList(pointList, selectedPointIndex - 1, visualizePointList);
            this->addPointToVisualizePointList(pointList, selectedPointIndex + 1, visualizePointList);
        }
        else if(selectedPointIndex == 0 && m_nbSplinePoints > 1)
        {
            this->addPointToVisualizePointList(pointList, selectedPointIndex + 1, visualizePointList);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::addPointToVisualizePointList(
    const ::fwData::PointList::sptr& pointList,
    const int indexSelectedPoint,
    const ::fwData::PointList::sptr& visualizePointList)
{
    OSLM_ASSERT("Point of index '" << indexSelectedPoint << "' not found in point list of size '"
                                   << pointList->getRefPoints().size() << "'",
                indexSelectedPoint >= 0 && indexSelectedPoint < pointList->getRefPoints().size());

    // Initialize the points
    ::fwData::Point::sptr srcPoint  = pointList->getRefPoints()[indexSelectedPoint];
    ::fwData::Point::sptr destPoint = ::fwData::Point::New();

    // Point label
    ::fwData::String::sptr text = ::fwData::String::dynamicCast(srcPoint->getField(::fwComEd::Dictionary::m_labelId));
    destPoint->setField(::fwComEd::Dictionary::m_labelId, text);

    // Compute destPoint coordinates
    double yCoord   = floor(m_height/m_spacing) + 1.;
    double position = 0.;
    ::fwData::Point::PointCoordArrayType vec0 = destPoint->getCoord();

    ::cpr::computePositionOfPointOnSpline(pointList, indexSelectedPoint, position);

    vec0 [0] = position;
    vec0 [1] = yCoord * m_spacing;
    vec0 [2] = 0.;

    destPoint->setCoord(vec0);

    OSLM_DEBUG("size in length " << position);
    OSLM_DEBUG("size in height " << yCoord * m_spacing * 2);

    // Add destPoint to visualizePointList
    visualizePointList->getRefPoints().push_back(destPoint);

    // Notify
    auto sig = visualizePointList->signal< ::fwData::PointList::PointAddedSignalType >(
        ::fwData::PointList::s_POINT_ADDED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_ADD_POINT_SLOT)));
        sig->asyncEmit(destPoint);
    }
}

//----------------------------------------------------------------------------------------------------------------

void SComputeCPR2D::clearVisualizePointList()
{
    // Get the PointList to visualize points on the cpr image.
    ::fwTools::Object::sptr cprObj = ::fwTools::fwID::getObject(m_visuPointsUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_visuPointsUID << "'", cprObj);
    ::fwData::PointList::sptr visualizePointList = ::fwData::PointList::dynamicCast(cprObj);
    OSLM_ASSERT("Failed to retrieve point list", visualizePointList);

    if(visualizePointList && visualizePointList->getRefPoints().size() > 0)
    {
        // Messages to notify that points of visualizePointList are removed
        for(int i = 0; i < visualizePointList->getRefPoints().size(); ++i)
        {
            ::fwData::Point::sptr point = visualizePointList->getRefPoints()[i];

            auto sig = visualizePointList->signal< ::fwData::PointList::PointRemovedSignalType >(
                ::fwData::PointList::s_POINT_REMOVED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_REMOVE_POINT_SLOT)));
                sig->asyncEmit(point);
            }
        }

        // Clear the pointList
        visualizePointList->getRefPoints().clear();
    }
}

//----------------------------------------------------------------------------------------------------------------

}   //namespace ctrlComputeCPR

