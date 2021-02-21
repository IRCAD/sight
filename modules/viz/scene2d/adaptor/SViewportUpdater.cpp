/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <service/macros.hpp>

#include <viz/scene2d/data/Viewport.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

fwServicesRegisterMacro( ::sight::viz::scene2d::IAdaptor, ::sight::module::viz::scene2d::adaptor::SViewportUpdater)

namespace sight::module::viz::scene2d
{
namespace adaptor
{

static const service::IService::KeyType s_VIEWPORT_INPUT = "viewport";

SViewportUpdater::SViewportUpdater() noexcept
{
}

//-----------------------------------------------------------------------------

SViewportUpdater::~SViewportUpdater() noexcept
{
}

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
    sight::viz::scene2d::data::Viewport::sptr sceneViewport   = this->getScene2DRender()->getViewport();
    sight::viz::scene2d::data::Viewport::csptr viewportObject =
        this->getInput< sight::viz::scene2d::data::Viewport>(s_VIEWPORT_INPUT);

    Point2DType pairCoord = this->mapAdaptorToScene(
        Point2DType(viewportObject->getX(), viewportObject->getY() ), m_xAxis, m_yAxis );

    Point2DType pairSize = this->mapAdaptorToScene(
        Point2DType(viewportObject->getWidth(), viewportObject->getHeight() ), m_xAxis, m_yAxis );

    sceneViewport->setX( pairCoord.first );
    sceneViewport->setY( pairCoord.second );
    sceneViewport->setWidth( pairSize.first );
    sceneViewport->setHeight( viewportObject->getHeight() );

    this->getScene2DRender()->getView()->updateFromViewport();
}

//-----------------------------------------------------------------------------

void SViewportUpdater::processInteraction( sight::viz::scene2d::data::Event& )
{

}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SViewportUpdater::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_VIEWPORT_INPUT, sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

}   // namespace adaptor
}   // namespace sight::module::viz::scene2d
