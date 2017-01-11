/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/ViewportInteractor.hpp"
#include "scene2D/Scene2DGraphicsView.hpp"

#include <fwServices/macros.hpp>

#include <fwData/Composite.hpp>


fwServicesRegisterMacro( ::scene2D::adaptor::IAdaptor, ::scene2D::adaptor::ViewportInteractor,  ::fwData::Composite );

namespace scene2D
{
namespace adaptor
{

//-----------------------------------------------------------------------------

ViewportInteractor::ViewportInteractor() throw() :
    m_viewportIsTranslated(false)
{
}

//-----------------------------------------------------------------------------

ViewportInteractor::~ViewportInteractor() throw()
{
}

//-----------------------------------------------------------------------------

void ViewportInteractor::configuring() throw( ::fwTools::Failed)
{
    this->IAdaptor::configuring();
}

//-----------------------------------------------------------------------------

void ViewportInteractor::doStart() throw( ::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void ViewportInteractor::doStop() throw( ::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void ViewportInteractor::doUpdate() throw( ::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void ViewportInteractor::doSwap() throw( ::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void ViewportInteractor::processInteraction( ::scene2D::data::Event::sptr _event )
{
    if ( _event->getType() == ::scene2D::data::Event::MouseWheelUp
         && _event->getModifier() == ::scene2D::data::Event::ShiftModifier )
    {
        this->zoom(true);
    }
    else if ( _event->getType() == ::scene2D::data::Event::MouseWheelDown
              && _event->getModifier() == ::scene2D::data::Event::ShiftModifier )
    {
        this->zoom(false);
    }
    else if ( _event->getType() == ::scene2D::data::Event::MouseButtonPress
              && _event->getButton() == ::scene2D::data::Event::LeftButton
              && _event->getModifier() == ::scene2D::data::Event::ShiftModifier )
    {
        m_viewportIsTranslated = true;
        m_lastCoordEvent       = _event->getCoord();
    }
    else if ( m_viewportIsTranslated )
    {
        if ( _event->getType() == ::scene2D::data::Event::MouseMove )
        {
            ::scene2D::data::Coord coord                  = _event->getCoord();
            ::scene2D::data::Viewport::sptr sceneViewport = this->getScene2DRender()->getViewport();

            float dx     = coord.getX() - m_lastCoordEvent.getX();
            float xTrans = dx * sceneViewport->getWidth() / (float) this->getScene2DRender()->getView()->width();

            float dy     = coord.getY() - m_lastCoordEvent.getY();
            float yTrans = dy * sceneViewport->getHeight() / (float) this->getScene2DRender()->getView()->height();

            sceneViewport->setX( sceneViewport->getX() - xTrans );
            sceneViewport->setY( sceneViewport->getY() - yTrans );
            this->getScene2DRender()->getView()->updateFromViewport();

            m_lastCoordEvent = coord;
        }
        else if ( _event->getType() == ::scene2D::data::Event::MouseButtonRelease )
        {
            m_viewportIsTranslated = false;
        }
    }
}

//-----------------------------------------------------------------------------

void ViewportInteractor::zoom( bool zoomIn )
{
    ::scene2D::data::Viewport::sptr sceneViewport = this->getScene2DRender()->getViewport();

    float zoomPercent = 10.f / 100.0f;
    float y           = sceneViewport->getY();
    float x           = sceneViewport->getX();
    float width       = sceneViewport->getWidth();
    float height      = sceneViewport->getHeight();
    float centerX     = x + width/2.0f;
    float centerY     = y + height/2.0f;

    float newWidth;
    float newHeight;
    if ( zoomIn )
    {
        newWidth  = width * zoomPercent;
        newHeight = height * zoomPercent;
    }
    else
    {
        newWidth  = -width * zoomPercent;
        newHeight = -height * zoomPercent;
    }

    newWidth  += width;
    newHeight += height;

    x = centerX - newWidth / 2;
    y = centerY - newHeight / 2;

    width  = newWidth;
    height = newHeight;

    // Set viewport
    sceneViewport->setX( x );
    sceneViewport->setY( y );
    sceneViewport->setWidth( width );
    sceneViewport->setHeight( height );
    this->getScene2DRender()->getView()->updateFromViewport();
}

//-----------------------------------------------------------------------------

}   // namespace adaptor
}   // namespace scene2D

