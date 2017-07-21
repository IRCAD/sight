/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderQt/IAdaptor.hpp"

#include "fwRenderQt/Scene2DGraphicsView.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Composite.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <QGraphicsItemGroup>

namespace fwRenderQt
{

IAdaptor::IAdaptor() noexcept :
    m_xAxis(nullptr),
    m_yAxis(nullptr),
    m_zValue(0.f),
    m_opacity(1.f)
{
}

//-----------------------------------------------------------------------------

IAdaptor::~IAdaptor() noexcept
{
}

//-----------------------------------------------------------------------------

void IAdaptor::info(std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

void IAdaptor::setZValue(float _zValue)
{
    m_zValue = _zValue;
}

//-----------------------------------------------------------------------------

float IAdaptor::getZValue() const
{
    return m_zValue;
}

//-----------------------------------------------------------------------------

void IAdaptor::setScene2DRender( ::fwRenderQt::SRender::sptr render)
{
    SLM_ASSERT("Service not instanced", render);
    SLM_ASSERT("Adaptor is already started", this->isStopped() );
    m_scene2DRender = render;
}

//-----------------------------------------------------------------------------

::fwRenderQt::SRender::sptr IAdaptor::getScene2DRender() const
{
    return m_scene2DRender.lock();
}

//-----------------------------------------------------------------------------

IAdaptor::ViewSizeRatio IAdaptor::getViewSizeRatio() const
{
    return ViewSizeRatio(
        (float) ( m_viewInitialSize.first / this->getScene2DRender()->getView()->width() ),
        (float) ( m_viewInitialSize.second / this->getScene2DRender()->getView()->height() ) );
}

//-----------------------------------------------------------------------------

IAdaptor::ViewportSizeRatio IAdaptor::getViewportSizeRatio() const
{
    return ViewportSizeRatio(
        (float) ( m_viewportInitialSize.first / this->getScene2DRender()->getViewport()->getWidth() ),
        (float) ( m_viewportInitialSize.second / this->getScene2DRender()->getViewport()->getHeight() ) );
}

//-----------------------------------------------------------------------------

IAdaptor::Scene2DRatio IAdaptor::getRatio() const
{
    const ViewSizeRatio ratioView         = this->getViewSizeRatio();
    const ViewportSizeRatio ratioViewport = this->getViewportSizeRatio();

    return Scene2DRatio(    ratioView.first / ratioViewport.first,
                            ratioView.second / ratioViewport.second );
}

//-----------------------------------------------------------------------------

IAdaptor::Point2DType IAdaptor::mapAdaptorToScene(const Point2DType& _xy,
                                                  const ::fwRenderQt::data::Axis& _xAxis,
                                                  const ::fwRenderQt::data::Axis& _yAxis ) const
{
    double x, y;

    if (_xAxis.getScaleType() == ::fwRenderQt::data::Axis::LOG)
    {
        // Logarithme 10 cannot get negative values
        if (_xy.first <= 0.)
        {
            x = 0.;
        }
        else
        {
            // Apply the x scale and the log to the x value
            x = _xAxis.getScale() * log10( _xy.first );
        }
    }
    else
    {
        // Apply just the x scale to the x value
        x = _xAxis.getScale() * _xy.first;
    }

    if (_yAxis.getScaleType() == ::fwRenderQt::data::Axis::LOG)
    {
        // Logarithm 10 cannot get negative values
        if (_xy.second <= 0.)
        {
            y = 0.;
        }
        else
        {
            // Apply the y scale and the log to the y value
            y = _yAxis.getScale() * log10( _xy.second );
        }
    }
    else
    {
        // Apply just the y scale to the y value
        y = _yAxis.getScale() * _xy.second;
    }

    return Point2DType( x, y );
}

//-----------------------------------------------------------------------------

IAdaptor::Point2DType IAdaptor::mapSceneToAdaptor(const Point2DType& _xy,
                                                  const ::fwRenderQt::data::Axis& _xAxis,
                                                  const ::fwRenderQt::data::Axis& _yAxis ) const
{
    // Do the reverse operation of the mapAdaptorToScene function
    double x, y;
    if (_xAxis.getScaleType() == ::fwRenderQt::data::Axis::LOG)
    {
        x = 10. * exp( _xy.first ) / _xAxis.getScale();
    }
    else
    {
        x = ( _xy.first ) / _xAxis.getScale();
    }

    if (_yAxis.getScaleType() == ::fwRenderQt::data::Axis::LOG)
    {
        y = 10. * ( _xy.second ) / _yAxis.getScale();
    }
    else
    {
        y = _xy.second / _yAxis.getScale();
    }

    return Point2DType( x, y );
}

//-----------------------------------------------------------------------------

void IAdaptor::configuring()
{
    m_viewInitialSize.first  = -1.0f;
    m_viewInitialSize.second = -1.0f;

    m_viewportInitialSize.first  = -1.0f;
    m_viewportInitialSize.second = -1.0f;

    // If the corresponding attributes are present in the config, set the xAxis, yAxis and the adaptor zValue
    if ( m_configuration->hasAttribute("xAxis") )
    {
        m_xAxis = this->getScene2DRender()->getAxis(m_configuration->getAttributeValue("xAxis"));
        SLM_ASSERT("xAxis not found", m_xAxis);
    }

    if ( m_configuration->hasAttribute("yAxis") )
    {
        m_yAxis = this->getScene2DRender()->getAxis(m_configuration->getAttributeValue("yAxis"));
        SLM_ASSERT("yAxis not found", m_xAxis);
    }

    if ( m_configuration->hasAttribute("zValue") )
    {
        m_zValue = std::stof( m_configuration->getAttributeValue("zValue"));
    }

    if ( m_configuration->hasAttribute("opacity") )
    {
        m_opacity = std::stof( m_configuration->getAttributeValue("opacity"));
    }
}

//-----------------------------------------------------------------------------

void IAdaptor::initializeViewSize()
{
    // Initialize the initial width of the view
    if(m_viewInitialSize.first == -1.0f)
    {
        m_viewInitialSize.first = this->getScene2DRender()->getView()->width();
    }

    // Initialize the initial height of the view
    if(m_viewInitialSize.second == -1.0f)
    {
        m_viewInitialSize.second = this->getScene2DRender()->getView()->height();
    }
}

//-----------------------------------------------------------------------------

void IAdaptor::initializeViewportSize()
{
    // Initialize the initial width of the viewport
    if(m_viewportInitialSize.first == -1.0f)
    {
        m_viewportInitialSize.first = this->getScene2DRender()->getViewport()->getWidth();
    }

    // Initialize the initial height of the viewport
    if(m_viewportInitialSize.second == -1.0f)
    {
        m_viewportInitialSize.second = this->getScene2DRender()->getViewport()->getHeight();
    }
}

//-----------------------------------------------------------------------------

void IAdaptor::starting()
{
    m_connections.connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());

    doStart();
}

//-----------------------------------------------------------------------------

void IAdaptor::updating()
{
    doUpdate();
}

//-----------------------------------------------------------------------------

void IAdaptor::swapping()
{
    m_connections.disconnect();
    m_connections.connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());
    doSwap();
}

//-----------------------------------------------------------------------------

void IAdaptor::stopping()
{
    m_connections.disconnect();
    doStop();
}

//-----------------------------------------------------------------------------

void IAdaptor::processInteraction(::fwRenderQt::data::Event& _event )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void IAdaptor::registerService( ::fwRenderQt::IAdaptor::sptr srv )
{
    m_managedAdaptors.push_back( srv );
}

//-----------------------------------------------------------------------------

void IAdaptor::unregisterServices()
{
    for(const ManagedAdaptorVector::value_type& adaptor : m_managedAdaptors )
    {
        adaptor.lock()->stop();
        ::fwServices::OSR::unregisterService(adaptor.lock());
    }
    m_managedAdaptors.clear();
}

//-----------------------------------------------------------------------------

} // namespace fwRenderQt

