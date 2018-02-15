/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuNavigation/SSplineAdaptor.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/PointList.hpp>

#include <fwServices/macros.hpp>

#include <navigation/functions.hpp>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuNavigation::SSplineAdaptor, ::fwData::PointList);

namespace visuNavigation
{

vtkSmartPointer<vtkParametricFunctionSource> m_functionSource;

static const ::fwCom::Slots::SlotKeyType s_ADD_POINT_SLOT     = "addPoint";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_POINT_SLOT  = "removePoint";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SPLINE_SLOT = "updateSpline";

static const ::fwServices::IService::KeyType s_POINTLIST_INPUT = "pointlist";

//-----------------------------------------------------------------------------------

SSplineAdaptor::SSplineAdaptor() noexcept :
    m_splineLength(0.0)
{
    m_parametricSpline = vtkSmartPointer<vtkParametricSpline>::New();
    m_vtkpoints        = vtkSmartPointer<vtkPoints>::New();
    m_functionSource   = vtkSmartPointer<vtkParametricFunctionSource>::New();

    newSlot(s_ADD_POINT_SLOT, &SSplineAdaptor::addPoint, this);
    newSlot(s_REMOVE_POINT_SLOT, &SSplineAdaptor::removePoint, this);
    newSlot(s_UPDATE_SPLINE_SLOT, &SSplineAdaptor::updateSpline, this);
}

//-----------------------------------------------------------------------------------

SSplineAdaptor::~SSplineAdaptor() noexcept
{
}

//-----------------------------------------------------------------------------------

void SSplineAdaptor::starting()
{
    this->initialize();

    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList >(s_POINTLIST_INPUT);
    SLM_ASSERT("Invalid pointList object", pointList);

    const size_t numberOfPoints = pointList->getPoints().size();

    for(size_t i = 0; i < numberOfPoints; ++i)
    {
        ::navigation::computeSpline(pointList, i, m_vtkpoints, m_parametricSpline, m_splineLength);
        this->updating();
    }
}

//-----------------------------------------------------------------------------------

void SSplineAdaptor::stopping()
{
    this->removeAllPropFromRenderer();
}

//-----------------------------------------------------------------------------------

void SSplineAdaptor::configuring()
{
    this->configureParams();
}

//----------------------------------------------------------------------------------------------------------------

void SSplineAdaptor::addPoint(::fwData::Point::sptr /*point*/)
{
    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList >(s_POINTLIST_INPUT);
    SLM_ASSERT("Invalid pointList object", pointList);

    const size_t numberOfPoints = pointList->getPoints().size();
    ::navigation::computeSpline(pointList, numberOfPoints - 1, m_vtkpoints, m_parametricSpline, m_splineLength);

    this->updating();
}

//----------------------------------------------------------------------------------------------------------------

void SSplineAdaptor::removePoint(::fwData::Point::sptr /*point*/)
{
    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList >(s_POINTLIST_INPUT);
    SLM_ASSERT("Invalid pointList object", pointList);

    ::navigation::updateSpline(pointList, m_vtkpoints, m_parametricSpline, m_splineLength);

    this->updating();
}

//----------------------------------------------------------------------------------------------------------------

void SSplineAdaptor::updateSpline()
{
    ::fwData::PointList::csptr pointList = this->getInput< ::fwData::PointList >(s_POINTLIST_INPUT);
    SLM_ASSERT("Invalid pointList object", pointList);

    ::navigation::updateSpline(pointList, m_vtkpoints, m_parametricSpline, m_splineLength);

    this->updating();
}

//-----------------------------------------------------------------------------------

void SSplineAdaptor::updating()
{
    SLM_ASSERT("No valid spline", m_parametricSpline);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> actor           = vtkSmartPointer<vtkActor>::New();

    m_functionSource->SetUResolution(static_cast<int>(std::floor(m_splineLength)));
    m_functionSource->SetParametricFunction(m_parametricSpline);
    m_functionSource->Update();

    OSLM_DEBUG(" U resolution " << m_functionSource->GetUResolution());

    mapper->SetInputConnection(m_functionSource->GetOutputPort());
    actor->SetMapper(mapper);

    this->addToRenderer(actor);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SSplineAdaptor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_POINTLIST_INPUT, ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_SPLINE_SLOT);
    connections.push(s_POINTLIST_INPUT, ::fwData::PointList::s_POINT_ADDED_SIG, s_ADD_POINT_SLOT);
    connections.push(s_POINTLIST_INPUT, ::fwData::PointList::s_POINT_REMOVED_SIG, s_REMOVE_POINT_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuNavigation
