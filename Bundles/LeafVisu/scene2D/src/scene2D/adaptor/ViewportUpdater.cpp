/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/ViewportUpdater.hpp"

#include <fwRenderQt/data/Viewport.hpp>
#include <fwRenderQt/Scene2DGraphicsView.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::ViewportUpdater, ::fwRenderQt::data::Viewport);

namespace scene2D
{
namespace adaptor
{

ViewportUpdater::ViewportUpdater() noexcept
{
}

//-----------------------------------------------------------------------------

ViewportUpdater::~ViewportUpdater() noexcept
{
}

//-----------------------------------------------------------------------------

void ViewportUpdater::configuring()
{
    this->IAdaptor::configuring();

    doUpdate();
}

//-----------------------------------------------------------------------------

void ViewportUpdater::doStart()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ViewportUpdater::doStop()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ViewportUpdater::doUpdate()
{
    SLM_TRACE_FUNC();

    ::fwRenderQt::data::Viewport::sptr sceneViewport  = this->getScene2DRender()->getViewport();
    ::fwRenderQt::data::Viewport::sptr viewportObject = this->getObject< ::fwRenderQt::data::Viewport>();

    Point2DType pairCoord = this->mapAdaptorToScene(
        Point2DType( viewportObject->getX(), viewportObject->getY() ),
        *m_xAxis, *m_yAxis );

    Point2DType pairSize = this->mapAdaptorToScene(
        Point2DType( viewportObject->getWidth(), viewportObject->getHeight() ),
        *m_xAxis, *m_yAxis );

    sceneViewport->setX( pairCoord.first );
    sceneViewport->setY( pairCoord.second );
    sceneViewport->setWidth( pairSize.first );
    sceneViewport->setHeight( viewportObject->getHeight() );

    this->getScene2DRender()->getView()->updateFromViewport();
}

//-----------------------------------------------------------------------------

void ViewportUpdater::doSwap()
{
}

//-----------------------------------------------------------------------------

void ViewportUpdater::processInteraction( ::fwRenderQt::data::Event& _event )
{

}

}   // namespace adaptor
}   // namespace scene2D

