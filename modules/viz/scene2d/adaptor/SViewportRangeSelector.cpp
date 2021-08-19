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

#include "modules/viz/scene2d/adaptor/SViewportRangeSelector.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <service/macros.hpp>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/data/Viewport.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QGraphicsRectItem>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

static const std::string s_INITIAL_WIDTH_CONFIG = "initialWidth";
static const std::string s_INITIAL_POS_CONFIG   = "initialPos";
static const std::string s_COLOR_CONFIG         = "color";

//---------------------------------------------------------------------------------------------------------------

SViewportRangeSelector::SViewportRangeSelector() noexcept :
    m_shutter(nullptr),
    m_isLeftInteracting(false),
    m_isRightInteracting(false),
    m_isInteracting(false),
    m_clickCatchRange(15),
    m_layer(nullptr),
    m_initialX(0.f),
    m_initialWidth(0.f)
{
}

//---------------------------------------------------------------------------------------------------------------

SViewportRangeSelector::~SViewportRangeSelector() noexcept
{
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_initialWidth = config.get<float>(s_INITIAL_WIDTH_CONFIG, m_initialWidth);

    m_initialX = config.get<float>(s_INITIAL_POS_CONFIG, m_initialX);

    const std::string color = config.get(s_COLOR_CONFIG, "#FFFFFF");
    sight::viz::scene2d::data::InitQtPen::setPenColor(m_color, color, m_opacity);
}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SViewportRangeSelector::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_VIEWPORT_INOUT, sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::starting()
{
    {
        sight::viz::scene2d::data::Viewport::sptr sceneViewport = this->getScene2DRender()->getViewport();

        const double viewportWidth = sceneViewport->getWidth();
        const double defaultWidth  = 2. * viewportWidth / 4.;

        if(m_initialWidth > viewportWidth || m_initialWidth < m_clickCatchRange)
        {
            SIGHT_WARN("Set viewport width to a default value instead of the given one because it can't be accepted.");
            m_initialWidth = defaultWidth;
        }

        const double defaultPos = (viewportWidth - m_initialWidth) / 2.;
        if(m_initialX < sceneViewport->getX() || (m_initialX + m_initialWidth) > viewportWidth)
        {
            SIGHT_WARN("Set viewport position to a default value since the given one is not correct.");
            m_initialX = defaultPos;
        }
    }

    int height = 0;
    {
        auto viewport = m_viewport.lock();
        height = viewport->getHeight();
    }

    Point2DType pair = this->mapAdaptorToScene(
        Point2DType(m_initialX, height),
        m_xAxis,
        m_yAxis
    );
    m_shutter = new QGraphicsRectItem(
        pair.first,
        0,
        m_initialWidth * m_xAxis->getScale(),
        pair.second
    );
    m_shutter->setBrush(m_color.color());
    m_shutter->setPen(Qt::NoPen);

    m_layer = new QGraphicsItemGroup();
    m_layer->addToGroup(m_shutter);

    // Adjust the layer's position and zValue depending on the associated axis
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    this->getScene2DRender()->getScene()->addItem(m_layer);

    QRectF rect = m_shutter->rect();
    updateViewportFromShutter(rect.x(), rect.y(), rect.width(), rect.height());

    data::Object::ModifiedSignalType::sptr sig;
    {
        auto viewport = m_viewport.lock();
        sig = viewport->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::stopping()
{
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::updating()
{
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    // Event coordinates in scene
    sight::viz::scene2d::data::Coord coord;
    coord = this->getScene2DRender()->mapToScene(_event.getCoord());

    // Shutter coordinates in scene
    const Point2DType shutterCoordPair = this->mapAdaptorToScene(
        Point2DType(m_shutter->rect().x(), m_shutter->rect().y()),
        m_xAxis,
        m_yAxis
    );
    const double shutterWidth = m_shutter->rect().width() * m_xAxis->getScale();

    const QRectF sceneRect = this->getScene2DRender()->getScene()->sceneRect();

    const bool onShutterLeft   = mouseOnShutterLeft(coord);
    const bool onShutterRight  = mouseOnShutterRight(coord);
    const bool onShutterMiddle = mouseOnShutterMiddle(coord);

    QRectF rect = m_shutter->rect();

    if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress)
    {
        if(onShutterLeft)
        {
            m_isLeftInteracting = true;
        }
        else if(onShutterRight)
        {
            m_isRightInteracting = true;
        }
        else if(onShutterMiddle)
        {
            this->getScene2DRender()->getView()->setCursor(Qt::ClosedHandCursor);

            // Interaction when clicking on the center of the shutter
            m_isInteracting  = true;
            m_dragStartPoint = coord;
            m_dragStartShutterPos.setX(shutterCoordPair.first);
            m_dragStartShutterPos.setY(shutterCoordPair.second);
        }
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonRelease)
    {
        m_isInteracting      = false;
        m_isLeftInteracting  = false;
        m_isRightInteracting = false;

        // Reset cursor
        if(onShutterMiddle)
        {
            this->getScene2DRender()->getView()->setCursor(Qt::OpenHandCursor);
        }
        else
        {
            this->getScene2DRender()->getView()->setCursor(Qt::ArrowCursor);
        }
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseMove)
    {
        // If the mouse is moving onto the shutter, without interactions, the cursor is changed to an other cursor
        // that symbolize the available interactions
        if(!m_isLeftInteracting && !m_isRightInteracting && !m_isInteracting)
        {
            if(onShutterLeft || onShutterRight)
            {
                this->getScene2DRender()->getView()->setCursor(Qt::SizeHorCursor); // horizontal double arrow
            }
            else if(onShutterMiddle)
            {
                this->getScene2DRender()->getView()->setCursor(Qt::OpenHandCursor); // open hand, for moving the
                                                                                    // whole shutter
            }
            else
            {
                this->getScene2DRender()->getView()->setCursor(Qt::ArrowCursor); // reset the cursor to the
                                                                                 // default cursor
            }
        }

        bool update = false; // if a viewport update will be requested

        if(m_isLeftInteracting)
        {
            // Shutter right side position
            const double rightSide = rect.x() + rect.width();

            if(coord.getX() < rightSide - m_clickCatchRange)
            {
                if(coord.getX() >= sceneRect.x())
                {
                    rect.setX(coord.getX());
                }
                else
                {
                    rect.setX(sceneRect.x());
                }
            }
            else
            {
                rect.setX(rightSide - m_clickCatchRange);
            }

            update = true;
        }
        else if(m_isRightInteracting)
        {
            const double newWidth        = coord.getX() - shutterCoordPair.first;
            const double shutterRightPos = abs(sceneRect.x()) + shutterCoordPair.first + newWidth;

            if(newWidth > m_clickCatchRange) // Shutter's width must be greater than the allowed picking range
            {
                if(shutterRightPos < sceneRect.width())
                {
                    rect.setWidth(newWidth);
                }
                else
                {
                    rect.setWidth(sceneRect.width() - shutterCoordPair.first - abs(sceneRect.x()));
                }
            }
            else
            {
                rect.setWidth(m_clickCatchRange);
            }

            update = true;
        }
        else if(m_isInteracting)
        {
            const double offset          = coord.getX() - m_dragStartPoint.getX();
            const double newX            = m_dragStartShutterPos.getX() + offset;
            const double shutterRightPos = abs(sceneRect.x()) + newX + shutterWidth;

            if(newX >= sceneRect.x() && shutterRightPos < sceneRect.width())
            {
                rect.setX(newX);
            }
            else if(newX < sceneRect.x())
            {
                rect.setX(sceneRect.x());
            }
            else if(shutterRightPos >= sceneRect.width())
            {
                rect.setX(sceneRect.width() - shutterWidth - abs(sceneRect.x()));
            }

            rect.setWidth(shutterWidth);
            update = true;
        }

        if(update)
        {
            // Update graphical shutter
            m_shutter->setRect(rect);
            m_layer->removeFromGroup(m_shutter);
            m_layer->addToGroup(m_shutter);

            // Update object
            this->updateViewportFromShutter(rect.x(), rect.y(), rect.width(), rect.height());

            auto viewport = m_viewport.lock();

            data::Object::ModifiedSignalType::sptr sig =
                viewport->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            {
                core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::updateViewportFromShutter(double _x, double _y, double _width, double _height)
{
    auto viewport = m_viewport.lock();

    const Point2DType fromSceneCoord = this->mapSceneToAdaptor(Point2DType(_x, _y), m_xAxis, m_yAxis);
    viewport->setX(fromSceneCoord.first);
    viewport->setY(fromSceneCoord.second);

    const Point2DType pair = this->mapSceneToAdaptor(Point2DType(_width, _height), m_xAxis, m_yAxis);
    viewport->setWidth(pair.first);
    viewport->setHeight(this->getScene2DRender()->getViewport()->getHeight());
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterMiddle(sight::viz::scene2d::data::Coord _coord)
{
    Point2DType shutterCoordPair;
    shutterCoordPair = this->mapAdaptorToScene(
        Point2DType(m_shutter->rect().x(), m_shutter->rect().y()),
        m_xAxis,
        m_yAxis
    );

    return (_coord.getX() > m_shutter->rect().x() + m_clickCatchRange)
           && (_coord.getX() < m_shutter->rect().x() + m_shutter->rect().width() - m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterLeft(sight::viz::scene2d::data::Coord _coord)
{
    Point2DType shutterCoordPair = this->mapAdaptorToScene(
        Point2DType(m_shutter->rect().x(), m_shutter->rect().y()),
        m_xAxis,
        m_yAxis
    );

    return (_coord.getX() >= shutterCoordPair.first - m_clickCatchRange)
           && (_coord.getX() <= shutterCoordPair.first + m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterRight(sight::viz::scene2d::data::Coord _coord)
{
    const Point2DType shutterCoordPair = this->mapAdaptorToScene(
        Point2DType(m_shutter->rect().x(), m_shutter->rect().y()),
        m_xAxis,
        m_yAxis
    );

    const double shutterRightPos = shutterCoordPair.first + m_shutter->rect().width() * m_xAxis->getScale();

    return (_coord.getX() >= shutterRightPos - m_clickCatchRange)
           && (_coord.getX() <= shutterRightPos + m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

} // namespace adaptor

} // namespace sight::module::viz::scene2d
