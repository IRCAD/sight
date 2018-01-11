/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlComputeCPR/SComputeCPR3D.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/PointList.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <cpr/functions.hpp>
#include <math.h>

fwServicesRegisterMacro(::fwServices::IController, ::ctrlComputeCPR::SComputeCPR3D, ::fwData::Mesh);


namespace ctrlComputeCPR
{
static const std::string s_SPLINE_KEY = "spline";
static const std::string s_IMAGE_KEY  = "image";
static const std::string s_MESH_KEY   = "mesh";

const ::fwCom::Slots::SlotKeyType SComputeCPR3D::s_CHANGE_HEIGHT_SLOT  = "changeHeight";
const ::fwCom::Slots::SlotKeyType SComputeCPR3D::s_CHANGE_SPACING_SLOT = "changeSpacing";
const ::fwCom::Slots::SlotKeyType SComputeCPR3D::s_CHANGE_ANGLE_SLOT   = "changeAngle";
const ::fwCom::Slots::SlotKeyType SComputeCPR3D::s_UPDATE_SPLINE_SLOT  = "updateSpline";

//----------------------------------------------------------------------------------------------------------

SComputeCPR3D::SComputeCPR3D() noexcept : m_nbSplinePoints(0),
                                          m_angle(0.),
                                          m_spacing(0.),
                                          m_height(50.0)
{
    newSlot(s_CHANGE_HEIGHT_SLOT, &SComputeCPR3D::setHeight, this);
    newSlot(s_CHANGE_SPACING_SLOT, &SComputeCPR3D::setSpacing, this);
    newSlot(s_CHANGE_ANGLE_SLOT, &SComputeCPR3D::setNormalRotation, this);
    newSlot(s_UPDATE_SPLINE_SLOT, &SComputeCPR3D::updateSpline, this);
}

//----------------------------------------------------------------------------------------------------------

SComputeCPR3D::~SComputeCPR3D() noexcept
{
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::starting()
{
    // Get the source image to set the spacing of the CPR.
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_KEY);
    SLM_ASSERT( s_IMAGE_KEY + " doesn't exist or is not an image", image);
    m_spacing = ::cpr::getImageMinSpacing(image);

    // Get the points of the spline point list
    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList >(s_SPLINE_KEY);
    SLM_ASSERT( s_SPLINE_KEY + " doesn't exist or is not a pointlist", pointList);
    m_nbSplinePoints = pointList->getCRefPoints().size();

    if(m_nbSplinePoints > 2)
    {
        this->computeMesh();
    }
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::stopping()
{
    SLM_TRACE_FUNC();
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::configuring()
{
    SLM_TRACE_FUNC();
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::updating()
{
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::updateSpline()
{
    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList >(s_SPLINE_KEY);
    SLM_ASSERT( s_SPLINE_KEY + " doesn't exist or is not a pointlist", pointList);
    m_nbSplinePoints = pointList->getCRefPoints().size();

    if(m_nbSplinePoints > 2)
    {
        this->computeMesh();
    }
    else if(m_nbSplinePoints <= 2)
    {
        ::fwData::Mesh::sptr mesh = this->getInOut< ::fwData::Mesh >(s_MESH_KEY);
        SLM_ASSERT( s_MESH_KEY + " doesn't exist or is not a mesh", mesh);

        mesh->clear();

        ::fwData::Object::ModifiedSignalType::sptr sig;
        sig = mesh->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
}

//----------------------------------------------------------------------------------------------------------

void SComputeCPR3D::computeMesh()
{
    ::fwData::Image::csptr imageSource = this->getInput< ::fwData::Image >(s_IMAGE_KEY);
    SLM_ASSERT( s_IMAGE_KEY + " doesn't exist or is not an image", imageSource);

    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList >(s_SPLINE_KEY);
    SLM_ASSERT( s_SPLINE_KEY + " doesn't exist or is not a pointlist", pointList);

    ::fwData::Mesh::sptr mesh = this->getInOut< ::fwData::Mesh >(s_MESH_KEY);
    SLM_ASSERT( s_MESH_KEY + " doesn't exist or is not a mesh", mesh);

    // Step I : init grids
    unsigned int nbRow = 0;
    unsigned int nbCol = 0;
    std::vector<double> pointGrid;
    std::vector<ImagePixelType> colorGrid;

    // Step II : fill point grid
    ::cpr::fillPointGrid(m_spacing, m_height, pointList, pointGrid, nbCol, nbRow, m_angle);

    // Step III : fill color grid
    ::cpr::fillColorGrid(pointGrid, nbCol, nbRow, imageSource, colorGrid);

    // Step IV : fill mesh
    ::cpr::fillMesh(pointGrid, colorGrid, nbCol, nbRow, imageSource, mesh);

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

