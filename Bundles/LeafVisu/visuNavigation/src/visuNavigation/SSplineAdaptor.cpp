/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

#include <fwComEd/PointListMsg.hpp>

#include <fwData/PointList.hpp>
#include <fwData/Point.hpp>

#include <fwServices/macros.hpp>

#include <navigation/functions.hpp>

#include "visuNavigation/SSplineAdaptor.hpp"

fwServicesRegisterMacro(::fwRenderVTK::IVtkAdaptorService, ::visuNavigation::SSplineAdaptor, ::fwData::PointList);

namespace visuNavigation
{

vtkSmartPointer<vtkParametricFunctionSource> m_functionSource;

//-----------------------------------------------------------------------------------

SSplineAdaptor::SSplineAdaptor() throw()
    : m_numberOfPoints(0), m_splineLength(0.0)
{
    m_parametricSpline = vtkSmartPointer<vtkParametricSpline>::New();
    m_vtkpoints        = vtkSmartPointer<vtkPoints>::New();
    m_functionSource   = vtkSmartPointer<vtkParametricFunctionSource>::New();
}

//-----------------------------------------------------------------------------------

SSplineAdaptor::~SSplineAdaptor() throw()
{
}

//-----------------------------------------------------------------------------------

void SSplineAdaptor::doStart() throw(fwTools::Failed)
{
    ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
    SLM_ASSERT("Invalid pointList object", pointList);

    for(::fwData::Point::sptr point :  pointList->getRefPoints())
    {
        ::navigation::computeSpline(pointList, m_numberOfPoints, m_vtkpoints, m_parametricSpline, m_splineLength);
        this->doUpdate();
        m_numberOfPoints++;
    }
}

//-----------------------------------------------------------------------------------

void SSplineAdaptor::doStop() throw(fwTools::Failed)
{
    this->removeAllPropFromRenderer();
}

//-----------------------------------------------------------------------------------

void SSplineAdaptor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    OSLM_ASSERT("Wrong configuration tag name", m_configuration->getName() == "config");
    this->setRenderId(m_configuration->getAttributeValue("renderer"));
}

//-----------------------------------------------------------------------------------

void SSplineAdaptor::doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    if( msg->hasEvent(::fwComEd::PointListMsg::ELEMENT_ADDED) )
    {
        ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
        m_numberOfPoints                    = pointList->getRefPoints().size();
        ::navigation::computeSpline(pointList, m_numberOfPoints - 1, m_vtkpoints, m_parametricSpline, m_splineLength);

        this->doUpdate();
    }

    if( msg->hasEvent(::fwComEd::PointListMsg::ELEMENT_REMOVED) )
    {
        ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
        m_numberOfPoints                    = pointList->getRefPoints().size();
        ::navigation::updateSpline(pointList, m_vtkpoints, m_parametricSpline, m_splineLength);

        this->doUpdate();
    }

    if( msg->hasEvent(::fwComEd::PointListMsg::ELEMENT_MODIFIED) )
    {
        ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
        ::navigation::updateSpline(pointList, m_vtkpoints, m_parametricSpline, m_splineLength);

        this->doUpdate();
    }
}

//-----------------------------------------------------------------------------------

void SSplineAdaptor::doUpdate() throw (fwTools::Failed)
{
    OSLM_ASSERT("No valid spline", m_parametricSpline);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> actor           = vtkSmartPointer<vtkActor>::New();

    m_functionSource->SetUResolution(floor(m_splineLength));
    m_functionSource->SetParametricFunction(m_parametricSpline);
    m_functionSource->Update();

    OSLM_DEBUG(" U resolution " << m_functionSource->GetUResolution());

    mapper->SetInputConnection(m_functionSource->GetOutputPort());
    actor->SetMapper(mapper);

    this->addToRenderer(actor);
    this->setVtkPipelineModified();
}

} //namespace visuNavigation

