/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/viz/scene2d/adaptor/viewport_interactor.hpp"

#include <service/macros.hpp>

#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <cmath>

namespace sight::module::viz::scene2d::adaptor
{

viewport_interactor::viewport_interactor() noexcept =
    default;

//-----------------------------------------------------------------------------

viewport_interactor::~viewport_interactor() noexcept =
    default;

//-----------------------------------------------------------------------------

void viewport_interactor::configuring()
{
}

//-----------------------------------------------------------------------------

void viewport_interactor::starting()
{
}

//-----------------------------------------------------------------------------

void viewport_interactor::stopping()
{
}

//-----------------------------------------------------------------------------

void viewport_interactor::updating()
{
}

//-----------------------------------------------------------------------------

void viewport_interactor::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    if(_event.getType() == sight::viz::scene2d::data::Event::MouseWheelUp
       && _event.getModifier() == sight::viz::scene2d::data::Event::ShiftModifier)
    {
        this->zoom(true);
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseWheelDown
            && _event.getModifier() == sight::viz::scene2d::data::Event::ShiftModifier)
    {
        this->zoom(false);
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress
            && _event.getButton() == sight::viz::scene2d::data::Event::LeftButton
            && _event.getModifier() == sight::viz::scene2d::data::Event::ShiftModifier)
    {
        m_viewportIsTranslated = true;
        m_lastCoordEvent       = _event.getCoord();
    }
    else if(m_viewportIsTranslated)
    {
        if(_event.getType() == sight::viz::scene2d::data::Event::MouseMove)
        {
            sight::viz::scene2d::vec2d_t coord = _event.getCoord();
            const auto viewport                = m_viewport.lock();

            const double dx     = coord.x - m_lastCoordEvent.x;
            const double xTrans = dx * viewport->width()
                                  / static_cast<double>(this->getScene2DRender()->getView()->width());

            const double dy     = coord.y - m_lastCoordEvent.y;
            const double yTrans = dy * viewport->height()
                                  / static_cast<double>(this->getScene2DRender()->getView()->height());

            viewport->setX(viewport->x() - xTrans);
            viewport->setY(viewport->y() - yTrans);

            this->getScene2DRender()->getView()->updateFromViewport(*viewport);

            m_lastCoordEvent = coord;
        }
        else if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonRelease)
        {
            m_viewportIsTranslated = false;
        }
    }
}

//-----------------------------------------------------------------------------

void viewport_interactor::zoom(bool zoomIn)
{
    const auto sceneViewport = m_viewport.lock();

    double y = sceneViewport->y();
    double x = sceneViewport->x();

    double width  = sceneViewport->width();
    double height = sceneViewport->height();

    const double zoomPercent = 10.F / 100.0F;
    const double centerX     = x + width / 2.0F;
    const double centerY     = y + height / 2.0F;

    double newWidth  = NAN;
    double newHeight = NAN;
    if(zoomIn)
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
    sceneViewport->setX(x);
    sceneViewport->setY(y);
    sceneViewport->setWidth(width);
    sceneViewport->setHeight(height);
    auto viewportObject = m_viewport.lock();
    this->getScene2DRender()->getView()->updateFromViewport(*viewportObject);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
