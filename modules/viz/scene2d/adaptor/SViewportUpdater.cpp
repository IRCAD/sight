/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/viz/scene2d/adaptor/SViewportUpdater.hpp"

#include <viz/scene2d/Scene2DGraphicsView.hpp>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

//-----------------------------------------------------------------------------

void SViewportUpdater::configuring()
{
    this->configureParams();
}

//-----------------------------------------------------------------------------

void SViewportUpdater::starting()
{
    updating();
}

//-----------------------------------------------------------------------------

void SViewportUpdater::stopping()
{
}

//-----------------------------------------------------------------------------

void SViewportUpdater::updating()
{
    auto viewport = m_viewport.lock();

    double ySc = viewport->getY();
    double hSc = viewport->getHeight();

    // If the viewport Y and height are not set, scale the viewport to the height of the scene
    if(ySc == 0 && hSc == 0)
    {
        auto scene = this->getScene2DRender()->getScene();

        ySc = scene->sceneRect().y();
        hSc = scene->sceneRect().height();

        viewport->setY(ySc);
        viewport->setHeight(hSc);
    }

    sight::viz::scene2d::data::Viewport sceneViewport;
    sceneViewport.setX(viewport->getX());
    sceneViewport.setY(viewport->getY());
    sceneViewport.setWidth(viewport->getWidth());
    sceneViewport.setHeight(viewport->getHeight());

    this->getScene2DRender()->getView()->updateFromViewport(sceneViewport);
}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SViewportUpdater::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_VIEWPORT_INOUT, sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

} // namespace adaptor

} // namespace sight::module::viz::scene2d
