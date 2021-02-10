/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "modules/viz/scene2d/adaptor/SViewportInteractor.hpp"

#include <services/macros.hpp>

#include <viz/scene2d/Scene2DGraphicsView.hpp>

fwServicesRegisterMacro( ::sight::viz::scene2d::IAdaptor, ::sight::modules::viz::scene2d::adaptor::SViewportInteractor)

namespace sight::modules::viz::scene2d
{
namespace adaptor
{

SViewportInteractor::SViewportInteractor() noexcept :
    m_viewportIsTranslated(false)
{
}

//-----------------------------------------------------------------------------

SViewportInteractor::~SViewportInteractor() noexcept
{
}

//-----------------------------------------------------------------------------

void SViewportInteractor::configuring()
{
}

//-----------------------------------------------------------------------------

void SViewportInteractor::starting()
{
}

//-----------------------------------------------------------------------------

void SViewportInteractor::stopping()
{
}

//-----------------------------------------------------------------------------

void SViewportInteractor::updating()
{
}

//-----------------------------------------------------------------------------

void SViewportInteractor::processInteraction( sight::viz::scene2d::data::Event& _event )
{
    if ( _event.getType() == sight::viz::scene2d::data::Event::MouseWheelUp
         && _event.getModifier() == sight::viz::scene2d::data::Event::ShiftModifier )
    {
        this->zoom(true);
    }
    else if ( _event.getType() == sight::viz::scene2d::data::Event::MouseWheelDown
              && _event.getModifier() == sight::viz::scene2d::data::Event::ShiftModifier )
    {
        this->zoom(false);
    }
    else if ( _event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress
              && _event.getButton() == sight::viz::scene2d::data::Event::LeftButton
              && _event.getModifier() == sight::viz::scene2d::data::Event::ShiftModifier )
    {
        m_viewportIsTranslated = true;
        m_lastCoordEvent       = _event.getCoord();
    }
    else if ( m_viewportIsTranslated )
    {
        if ( _event.getType() == sight::viz::scene2d::data::Event::MouseMove )
        {
            sight::viz::scene2d::data::Coord coord                  = _event.getCoord();
            sight::viz::scene2d::data::Viewport::sptr sceneViewport = this->getScene2DRender()->getViewport();

            const float dx     = coord.getX() - m_lastCoordEvent.getX();
            const float xTrans = dx * sceneViewport->getWidth() / (float) this->getScene2DRender()->getView()->width();

            const float dy     = coord.getY() - m_lastCoordEvent.getY();
            const float yTrans = dy * sceneViewport->getHeight() /
                                 (float) this->getScene2DRender()->getView()->height();

            sceneViewport->setX( sceneViewport->getX() - xTrans );
            sceneViewport->setY( sceneViewport->getY() - yTrans );
            this->getScene2DRender()->getView()->updateFromViewport();

            m_lastCoordEvent = coord;
        }
        else if ( _event.getType() == sight::viz::scene2d::data::Event::MouseButtonRelease )
        {
            m_viewportIsTranslated = false;
        }
    }
}

//-----------------------------------------------------------------------------

void SViewportInteractor::zoom( bool zoomIn )
{
    sight::viz::scene2d::data::Viewport::sptr sceneViewport = this->getScene2DRender()->getViewport();

    float y = sceneViewport->getY();
    float x = sceneViewport->getX();

    float width  = sceneViewport->getWidth();
    float height = sceneViewport->getHeight();

    const float zoomPercent = 10.f / 100.0f;
    const float centerX     = x + width/2.0f;
    const float centerY     = y + height/2.0f;

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
}   // namespace sight::modules::viz::scene2d
