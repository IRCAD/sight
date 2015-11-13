/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlComputeCPR/SComputeCPR3D.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slots.hxx>

#include <fwComEd/PointListMsg.hpp>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <cpr/functions.hpp>
#include <math.h>

fwServicesRegisterMacro(::fwServices::IController,::ctrlComputeCPR::SComputeCPR3D, ::fwData::Mesh);


namespace ctrlComputeCPR
{

const ::fwCom::Slots::SlotKeyType SComputeCPR3D::s_CHANGE_HEIGHT_SLOT  = "changeHeight";
const ::fwCom::Slots::SlotKeyType SComputeCPR3D::s_CHANGE_SPACING_SLOT = "changeSpacing";
const ::fwCom::Slots::SlotKeyType SComputeCPR3D::s_CHANGE_ANGLE_SLOT   = "changeAngle";

//----------------------------------------------------------------------------------------------------------

SComputeCPR3D::SComputeCPR3D() throw ()
    : m_nbSplinePoints(0), m_angle (0), m_height(50.0)
{
    m_slotChangeHeight = ::fwCom::newSlot(&SComputeCPR3D::setHeight, this);
    ::fwCom::HasSlots::m_slots(s_CHANGE_HEIGHT_SLOT, m_slotChangeHeight);

    m_slotChangeSpacing = ::fwCom::newSlot(&SComputeCPR3D::setSpacing, this);
    ::fwCom::HasSlots::m_slots(s_CHANGE_SPACING_SLOT, m_slotChangeSpacing);

    m_slotChangeAngle = ::fwCom::newSlot(&SComputeCPR3D::setNormalRotation, this);
    ::fwCom::HasSlots::m_slots(s_CHANGE_ANGLE_SLOT, m_slotChangeAngle);

    // Set default worker to new slots
    this->setWorker(::fwServices::registry::ActiveWorkers::getDefault()->getWorker(
                        ::fwServices::registry::ActiveWorkers::s_DEFAULT_WORKER));
}

//----------------------------------------------------------------------------------------------------------

SComputeCPR3D::~SComputeCPR3D() throw ()
{
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::starting() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Get the source image to set the spacing of the CPR.
    ::fwTools::Object::sptr imgObj = ::fwTools::fwID::getObject(m_sourceImageUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_sourceImageUID << "'", imgObj);
    ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(imgObj);
    OSLM_ASSERT("Failed to retrieve image", image);
    m_spacing = ::cpr::getImageMinSpacing(image);

    // Get the points of the spline point list
    ::fwTools::Object::sptr splineObj = ::fwTools::fwID::getObject(m_splinePointsUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_splinePointsUID << "'", splineObj);
    ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(splineObj);
    OSLM_ASSERT("Failed to retrieve point list", pointList);

    m_nbSplinePoints = pointList->getRefPoints().size();

    if(m_nbSplinePoints > 2)
    {
        this->computeMesh();
    }
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::stopping() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::configuring() throw (fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Get the spline points
    const std::vector<ConfigurationType>& splinePointsConfig = m_configuration->find("splinePoints");
    if (!splinePointsConfig.empty())
    {
        SLM_ASSERT("UID attribute is missing", splinePointsConfig.at(0)->hasAttribute("uid"));
        m_splinePointsUID = (splinePointsConfig.at(0)->getAttributeValue("uid"));
    }

    // Get the source image
    const std::vector<ConfigurationType>& sourceImageConfig = m_configuration->find("sourceImage");
    if (!sourceImageConfig.empty())
    {
        SLM_ASSERT("UID attribute is missing", sourceImageConfig.at(0)->hasAttribute("uid"));
        m_sourceImageUID = (sourceImageConfig.at(0)->getAttributeValue("uid"));
    }
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::updating() throw (::fwTools::Failed)
{
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::receiving(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    if(msg->hasEvent(::fwComEd::PointListMsg::ELEMENT_ADDED) || msg->hasEvent(::fwComEd::PointListMsg::ELEMENT_REMOVED))
    {
        ::fwTools::Object::sptr splineObj = ::fwTools::fwID::getObject(m_splinePointsUID);
        OSLM_ASSERT("Failed to retrieve object with UID '" << m_splinePointsUID << "'", splineObj);
        ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(splineObj);
        OSLM_ASSERT("Failed to retrieve point list", pointList);

        m_nbSplinePoints = pointList->getRefPoints().size();
    }

    if(msg->hasEvent(::fwComEd::PointListMsg::ELEMENT_MODIFIED) && m_nbSplinePoints > 2 )
    {
        this->computeMesh();
    }

    if(m_nbSplinePoints > 2)
    {
        this->computeMesh();
    }
    else if(m_nbSplinePoints <= 2)
    {
        ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh>();
        SLM_ASSERT("Mesh not valid", mesh);

        mesh->clear();

        ::fwData::Object::ModifiedSignalType::sptr sig;
        sig = mesh->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::computeMesh()
{
    ::fwTools::Object::sptr splineObj = ::fwTools::fwID::getObject(m_splinePointsUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_splinePointsUID << "'", splineObj);
    ::fwTools::Object::sptr imageObj = ::fwTools::fwID::getObject(m_sourceImageUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_sourceImageUID << "'", imageObj);

    ::fwData::PointList::sptr pointList = ::fwData::PointList::dynamicCast(splineObj);
    OSLM_ASSERT("Failed to retrieve point list", pointList);
    ::fwData::Image::sptr imageSource = ::fwData::Image::dynamicCast(imageObj);
    OSLM_ASSERT("Failed to retrieve image", imageSource);

    ::fwData::Mesh::sptr mesh = this->getObject< ::fwData::Mesh>();
    OSLM_ASSERT("Failed to retrieve associated mesh", mesh);

    // Step I : init grids
    unsigned int nbRow;
    unsigned int nbCol;
    std::vector<double> pointGrid;
    std::vector<ImagePixelType> colorGrid;

    // Step II : fill point grid
    ::cpr::fillPointGrid(m_spacing, m_height, pointList, pointGrid, nbCol, nbRow, m_angle);

    // Step III : fill color grid
    ::cpr::fillColorGrid(pointGrid, nbCol, nbRow, imageSource, colorGrid);

    // Step IV : fill mesh
    ::cpr::fillMesh(pointGrid, colorGrid, nbCol, nbRow,imageSource, mesh);

    // Step V : notify
    OSLM_DEBUG("Mesh nb points => " << mesh->getNumberOfPoints());
    OSLM_DEBUG("Mesh nb cells => " << mesh->getNumberOfCells());

    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = mesh->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::setHeight(double height)
{
    m_height = height;

    if(m_nbSplinePoints > 2)
    {
        this->computeMesh();
    }
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::setSpacing(double spacing)
{
    m_spacing = spacing;

    if(m_nbSplinePoints > 2)
    {
        this->computeMesh();
    }
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::setNormalRotation(double angle)
{
    m_angle = angle;

    if(m_nbSplinePoints > 2)
    {
        this->computeMesh();
    }
}

}   //namespace ctrlComputeCPR

