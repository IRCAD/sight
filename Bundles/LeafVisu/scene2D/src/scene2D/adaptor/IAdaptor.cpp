/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/IAdaptor.hpp"

#include "scene2D/Scene2DGraphicsView.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Composite.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <QGraphicsItemGroup>

namespace scene2D
{
namespace adaptor
{

IAdaptor::IAdaptor() noexcept :
    m_zValue(0),
    m_opacity(1)
{
}

//-----------------------------------------------------------------------------

IAdaptor::~IAdaptor() noexcept
{
    m_xAxis.reset();
    m_yAxis.reset();
}

//-----------------------------------------------------------------------------

void IAdaptor::info(std::ostream& _sstream )
{
    /*_sstream << "IAdaptor : " ;
       this->SuperClass::info( _sstream ) ;*/
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

void IAdaptor::setScene2DRender( ::scene2D::SRender::sptr _scene2DRender)
{
    SLM_ASSERT("Service not instanced", _scene2DRender);
    SLM_ASSERT("Adaptor is already started", this->isStopped() );
    m_scene2DRender = _scene2DRender;
}

//-----------------------------------------------------------------------------

::scene2D::SRender::sptr IAdaptor::getScene2DRender() const
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
    ViewSizeRatio ratioView         = this->getViewSizeRatio();
    ViewportSizeRatio ratioViewport = this->getViewportSizeRatio();

    return Scene2DRatio(    ratioView.first / ratioViewport.first,
                            ratioView.second / ratioViewport.second );
}

//-----------------------------------------------------------------------------

IAdaptor::Point2DType IAdaptor::mapAdaptorToScene(Point2DType _xy, ::scene2D::data::Axis::sptr _xAxis,
                                                  ::scene2D::data::Axis::sptr _yAxis )
{
    double x, y;

    if (_xAxis->getScaleType() == "LOG")
    {
        // Logarithme 10 cannot get negative values
        if (_xy.first <= 0)
        {
            x = 0;
        }
        else
        {
            // Apply the x scale and the log to the x value
            x = _xAxis->getScale() * log10( _xy.first );
        }
    }
    else
    {
        // Apply just the x scale to the x value
        x = _xAxis->getScale() * _xy.first;
    }

    if (_yAxis->getScaleType() == "LOG")
    {
        // Logarithm 10 cannot get negative values
        if (_xy.second <= 0)
        {
            y = 0;
        }
        else
        {
            // Apply the y scale and the log to the y value
            y = _yAxis->getScale() * log10( _xy.second );
        }
    }
    else
    {
        // Apply just the y scale to the y value
        y = _yAxis->getScale() * _xy.second;
    }

    return Point2DType( x, y );
}

//-----------------------------------------------------------------------------

IAdaptor::Point2DType IAdaptor::mapSceneToAdaptor(Point2DType _xy, ::scene2D::data::Axis::sptr _xAxis,
                                                  ::scene2D::data::Axis::sptr _yAxis )
{
    // Do the reverse operation of the mapAdaptorToScene function
    double x, y;

    if (_xAxis->getScaleType() == "LOG")
    {
        x = 10 * exp( _xy.first ) / _xAxis->getScale();
    }
    else
    {
        x = ( _xy.first ) / _xAxis->getScale();
    }

    if (_yAxis->getScaleType() == "LOG")
    {
        y = 10 * ( _xy.second ) / _yAxis->getScale();
    }
    else
    {
        y = _xy.second / _yAxis->getScale();
    }

    return Point2DType( x, y );
}

//-----------------------------------------------------------------------------

void IAdaptor::configuring()
{
    SLM_TRACE_FUNC();

    m_viewInitialSize.first  = -1.0f;
    m_viewInitialSize.second = -1.0f;

    m_viewportInitialSize.first  = -1.0f;
    m_viewportInitialSize.second = -1.0f;

    // If the corresponding attributes are present in the config, set the xAxis, yAxis and the adaptor zValue
    if ( m_configuration->hasAttribute("xAxis") )
    {
        m_xAxis = ::scene2D::data::Axis::dynamicCast(
            this->getScene2DRender()->getRegisteredObject(m_configuration->getAttributeValue("xAxis")));
    }
    else
    {
        m_xAxis = ::scene2D::data::Axis::New();
    }

    if ( m_configuration->hasAttribute("yAxis") )
    {
        m_yAxis = ::scene2D::data::Axis::dynamicCast(
            this->getScene2DRender()->getRegisteredObject(m_configuration->getAttributeValue("yAxis")));
    }
    else
    {
        m_yAxis = ::scene2D::data::Axis::New();
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

::fwData::Object::sptr IAdaptor::getRegisteredObject(::scene2D::SRender::ObjectIDType _objectId) const
{
    ::fwData::Object::sptr obj;
    if (!_objectId.empty())
    {
        // Get an object from an id
        obj = m_scene2DRender.lock()->getRegisteredObject(_objectId);
    }

    return obj;
}

//-----------------------------------------------------------------------------

void IAdaptor::processInteraction( ::scene2D::data::Event::sptr _event )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void IAdaptor::registerService( ::scene2D::adaptor::IAdaptor::sptr srv )
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

}
} // namespace scene2D

