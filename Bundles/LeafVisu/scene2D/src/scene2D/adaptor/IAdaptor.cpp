/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QGraphicsItemGroup>

#include <fwData/Composite.hpp>
#include <fwServices/Base.hpp>

#include "scene2D/adaptor/IAdaptor.hpp"
#include "scene2D/Scene2DGraphicsView.hpp"

namespace scene2D
{
namespace adaptor
{


IAdaptor::IAdaptor() throw() : m_zValue(0), m_opacity(1)
{
}

//-----------------------------------------------------------------------------

IAdaptor::~IAdaptor() throw()
{
}

//-----------------------------------------------------------------------------

void IAdaptor::info(std::ostream &_sstream )
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

float IAdaptor::getZValue()
{
    return m_zValue;
}

//-----------------------------------------------------------------------------

void IAdaptor::setScene2DRender( SPTR(::scene2D::Render) _scene2DRender)
{
    SLM_ASSERT("Service not instanced", _scene2DRender);
    SLM_ASSERT("Adaptor is already started", this->isStopped() ) ;
    m_scene2DRender = _scene2DRender;
}

//-----------------------------------------------------------------------------

SPTR(::scene2D::Render) IAdaptor::getScene2DRender()
{
    return m_scene2DRender.lock();
}

//-----------------------------------------------------------------------------

IAdaptor::ViewSizeRatio IAdaptor::getViewSizeRatio()
{
    return ViewSizeRatio(
            (float) ( m_viewInitialSize.first / this->getScene2DRender()->getView()->width() ),
            (float) ( m_viewInitialSize.second / this->getScene2DRender()->getView()->height() ) );
}

//-----------------------------------------------------------------------------

IAdaptor::ViewportSizeRatio IAdaptor::getViewportSizeRatio()
{
    return ViewportSizeRatio(
            (float) ( m_viewportInitialSize.first / this->getScene2DRender()->getViewport()->getWidth() ),
            (float) ( m_viewportInitialSize.second / this->getScene2DRender()->getViewport()->getHeight() ) );
}

//-----------------------------------------------------------------------------

IAdaptor::Scene2DRatio IAdaptor::getRatio()
{
    ViewSizeRatio ratioView = this->getViewSizeRatio();
    ViewportSizeRatio ratioViewport = this->getViewportSizeRatio();

    return Scene2DRatio(    ratioView.first / ratioViewport.first,
                            ratioView.second / ratioViewport.second );
}

//-----------------------------------------------------------------------------

std::pair< double , double > IAdaptor::mapAdaptorToScene(
        std::pair< double , double > _xy , SPTR(::scene2D::data::Axis) _xAxis, SPTR(::scene2D::data::Axis) _yAxis )
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
        // Logarithme 10 cannot get negative values
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

    return std::pair < double , double >( x , y );
}

//-----------------------------------------------------------------------------

std::pair< double , double > IAdaptor::mapSceneToAdaptor(
        std::pair< double , double > _xy , SPTR(::scene2D::data::Axis) _xAxis, SPTR(::scene2D::data::Axis) _yAxis )
{
    // Do the reverse opration of the mapAdaptorToScene function
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

    return std::pair < double , double >( x , y );
}

//-----------------------------------------------------------------------------

void IAdaptor::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    m_viewInitialSize.first = -1.0f;
    m_viewInitialSize.second = -1.0f;

    m_viewportInitialSize.first = -1.0f;
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
        m_zValue = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("zValue"));
    }

    if ( m_configuration->hasAttribute("opacity") )
    {
        m_opacity = ::boost::lexical_cast< float >( m_configuration->getAttributeValue("opacity"));
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

void IAdaptor::starting() throw ( ::fwTools::Failed )
{
    doStart();
}

//-----------------------------------------------------------------------------

void IAdaptor::updating() throw ( ::fwTools::Failed )
{
    doUpdate();
}

//-----------------------------------------------------------------------------

void IAdaptor::updating( fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    doUpdate(_msg);
}

//-----------------------------------------------------------------------------

void IAdaptor::swapping() throw(fwTools::Failed)
{
    doSwap();
}

//-----------------------------------------------------------------------------

void IAdaptor::stopping() throw ( ::fwTools::Failed )
{
    doStop();

    m_xAxis.reset();
    m_yAxis.reset();

}

//-----------------------------------------------------------------------------

SPTR(::fwData::Object) IAdaptor::getRegisteredObject(::scene2D::Render::ObjectIDType _objectId)
{
    SPTR(::fwData::Object) obj;
    if (!_objectId.empty())
    {
        // Get an object from an id
        obj = m_scene2DRender.lock()->getRegisteredObject(_objectId);
    }

    return obj;
}

//-----------------------------------------------------------------------------

void IAdaptor::processInteraction( SPTR(::scene2D::data::Event) _event )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

}
} // namespace scene2D



