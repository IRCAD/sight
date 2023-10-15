/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/viz/scene2d/adaptor/viewport_updater.hpp"

#include <viz/scene2d/Scene2DGraphicsView.hpp>

namespace sight::module::viz::scene2d::adaptor
{

//-----------------------------------------------------------------------------

void viewport_updater::configuring()
{
    this->configureParams();
}

//-----------------------------------------------------------------------------

void viewport_updater::starting()
{
    {
        // If the viewport Y and height are not set, scale the viewport to the height of the scene
        auto viewport = m_viewport.lock();
        auto* scene   = this->getScene2DRender()->getScene();

        viewport->setY(viewport->y_or(scene->sceneRect().y()));
        viewport->setHeight(viewport->height_or(scene->sceneRect().height()));
    }

    this->updating();
}

//-----------------------------------------------------------------------------

void viewport_updater::stopping()
{
}

//-----------------------------------------------------------------------------

void viewport_updater::updating()
{
    auto viewport = m_viewport.lock();
    this->getScene2DRender()->getView()->updateFromViewport(*viewport);
}

//----------------------------------------------------------------------------------------------------------

service::connections_t viewport_updater::auto_connections() const
{
    connections_t connections;
    connections.push(s_VIEWPORT_INOUT, sight::viz::scene2d::data::Viewport::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

} // namespace sight::module::viz::scene2d::adaptor
