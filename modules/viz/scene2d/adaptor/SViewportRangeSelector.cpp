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

#include "modules/viz/scene2d/adaptor/SViewportRangeSelector.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/helper/MedicalImage.hpp>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/data/Viewport.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QGraphicsRectItem>

using sight::viz::scene2d::vec2d_t;

namespace sight::module::viz::scene2d::adaptor
{

static const std::string s_INITIAL_WIDTH_CONFIG = "initialWidth";
static const std::string s_INITIAL_POS_CONFIG   = "initialPos";
static const std::string s_COLOR_CONFIG         = "color";

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration().get_child("config.<xmlattr>");
    m_initialWidth = config.get<double>(s_INITIAL_WIDTH_CONFIG, m_initialWidth);

    m_initialX = config.get<double>(s_INITIAL_POS_CONFIG, m_initialX);

    const std::string color = config.get(s_COLOR_CONFIG, "#FFFFFF");
    sight::viz::scene2d::data::InitQtPen::setPenColor(m_color, color, m_opacity);
}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SViewportRangeSelector::getAutoConnections() const
{
    return {
        {s_SELECTED_VIEWPORT_INOUT, sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_IMAGE_INPUT, sight::data::Image::s_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_IMAGE_INPUT, sight::data::Image::s_BUFFER_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_TF_INPUT, sight::data::TransferFunction::s_POINTS_MODIFIED_SIG, IService::slots::s_UPDATE}
    };
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::starting()
{
    {
        auto* const scene        = this->getScene2DRender()->getScene();
        const auto sceneViewport = m_viewport.lock();

        const double viewportWidth = sceneViewport->width_or(scene->sceneRect().width());
        const double defaultWidth  = 2. * viewportWidth / 4.;

        if(m_initialWidth > viewportWidth || m_initialWidth < m_clickCatchRange)
        {
            SIGHT_WARN("Set viewport width to a default value instead of the given one because it can't be accepted.");
            m_initialWidth = defaultWidth;
        }

        const double defaultPos = (viewportWidth - m_initialWidth) / 2.;
        if(m_initialX < sceneViewport->x_or(scene->sceneRect().x()) || (m_initialX + m_initialWidth) > viewportWidth)
        {
            SIGHT_WARN("Set viewport position to a default value since the given one is not correct.");
            m_initialX = defaultPos;
        }
    }

    {
        auto* const scene = this->getScene2DRender()->getScene();
        auto viewport     = m_viewport.lock();

        // If the viewport Y and height are not set, scale the viewport to the height of the scene
        {
            viewport->setY(viewport->y_or(scene->sceneRect().y()));
            viewport->setHeight(viewport->height_or(scene->sceneRect().height()));
        }
    }

    const vec2d_t pair = this->mapSceneToAdaptor(vec2d_t(m_initialX, 1.0));

    QRectF rect(pair.x, 0, m_initialWidth * m_xAxis->getScale(), pair.y);
    {
        auto image = m_image.lock();
        if(image)
        {
            sight::data::helper::MedicalImage::getMinMax(image.get_shared(), m_imageMin, m_imageMax);
            rect.setRect(m_imageMin, rect.y(), m_imageMax - m_imageMin, rect.height());
        }
    }

    m_shutter = new QGraphicsRectItem(rect);
    m_shutter->setBrush(m_color.color());
    m_shutter->setPen(Qt::NoPen);

    m_layer = new QGraphicsItemGroup();
    m_layer->addToGroup(m_shutter);

    // Adjust the layer's position and zValue depending on the associated axis
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    this->getScene2DRender()->getScene()->addItem(m_layer);

    updateViewportFromShutter(rect.x(), rect.y(), rect.width(), rect.height());

    this->updating();
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::stopping()
{
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::updating()
{
    const auto tf    = m_tf.lock();
    const auto image = m_image.lock();

    m_min = (tf || image) ? std::numeric_limits<double>::max() : m_initialX;
    m_max = (tf || image) ? std::numeric_limits<double>::lowest() : m_initialX + m_initialWidth;

    if(tf)
    {
        std::tie(m_min, m_max) = tf->windowMinMax();
    }

    if(image)
    {
        sight::data::helper::MedicalImage::getMinMax(image.get_shared(), m_imageMin, m_imageMax);

        m_min = std::min(m_min, m_imageMin);
        m_max = std::max(m_max, m_imageMax);
    }

    auto viewport = m_viewport.lock();

    viewport->setX(m_min);
    viewport->setWidth(m_max - m_min);
    this->getScene2DRender()->getView()->updateFromViewport(*viewport);

    auto sig = viewport->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(slot(IService::slots::s_UPDATE)));
        sig->asyncEmit();
    }

    m_clickCatchRange = static_cast<int>(m_max - m_min) / 100;

    // Fit the shutter from the current range
    QRectF rect = m_shutter->rect();

    rect.setX(std::max(rect.x(), m_min));
    rect.setWidth(std::min(rect.width(), m_max - rect.x()));
    m_shutter->setRect(rect);
    m_layer->removeFromGroup(m_shutter);
    m_layer->addToGroup(m_shutter);
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    // Event coordinates in scene
    sight::viz::scene2d::vec2d_t coord;
    coord = this->getScene2DRender()->mapToScene(_event.getCoord());

    // Shutter coordinates in scene
    const vec2d_t shutterCoordPair =
        this->mapAdaptorToScene(vec2d_t(m_shutter->rect().x(), m_shutter->rect().y()));
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
            m_isInteracting         = true;
            m_dragStartPoint        = coord;
            m_dragStartShutterPos.x = shutterCoordPair.x;
            m_dragStartShutterPos.y = shutterCoordPair.y;
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

        const auto image               = m_image.lock();
        const double leftSideBoundary  = image ? m_min : sceneRect.x();
        const double rightSideBoundary = image ? m_max : sceneRect.width() - sceneRect.x();
        if(m_isLeftInteracting)
        {
            // Shutter right side position
            const double rightSide = rect.x() + rect.width();

            if(coord.x < rightSide - m_clickCatchRange)
            {
                rect.setX(std::max(leftSideBoundary, coord.x));
            }
            else
            {
                rect.setX(rightSide - m_clickCatchRange);
            }

            update = true;
        }
        else if(m_isRightInteracting)
        {
            const double newWidth        = coord.x - shutterCoordPair.x;
            const double shutterRightPos = shutterCoordPair.x + newWidth;

            if(newWidth > m_clickCatchRange) // Shutter's width must be greater than the allowed picking range
            {
                if(shutterRightPos <= rightSideBoundary)
                {
                    rect.setWidth(newWidth);
                }
                else
                {
                    rect.setWidth(rightSideBoundary - shutterCoordPair.x);
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
            const double offset          = coord.x - m_dragStartPoint.x;
            const double newX            = m_dragStartShutterPos.x + offset;
            const double shutterRightPos = newX + shutterWidth;

            if(newX >= leftSideBoundary && shutterRightPos < rightSideBoundary)
            {
                rect.setX(newX);
            }
            else if(newX < leftSideBoundary)
            {
                rect.setX(leftSideBoundary);
            }
            else if(shutterRightPos >= rightSideBoundary)
            {
                rect.setX(rightSideBoundary - shutterWidth);
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
            {
                auto selectedViewport = m_selectedViewport.lock();
                auto sig              = selectedViewport->signal<data::Object::ModifiedSignalType>(
                    data::Object::s_MODIFIED_SIG
                );
                {
                    core::com::Connection::Blocker block(sig->getConnection(slot(IService::slots::s_UPDATE)));
                    sig->asyncEmit();
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::updateViewportFromShutter(double _x, double _y, double _width, double _height)
{
    auto selectedViewport = m_selectedViewport.lock();

    const vec2d_t fromSceneCoord = this->mapSceneToAdaptor(vec2d_t(_x, _y));
    const vec2d_t pair           = this->mapSceneToAdaptor(vec2d_t(_width, _height));

    selectedViewport->setX(fromSceneCoord.x);
    selectedViewport->setWidth(pair.x);
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterMiddle(sight::viz::scene2d::vec2d_t _coord)
{
    vec2d_t shutterCoordPair;
    shutterCoordPair = this->mapAdaptorToScene({m_shutter->rect().x(), m_shutter->rect().y()});

    return (_coord.x > m_shutter->rect().x() + m_clickCatchRange)
           && (_coord.x < m_shutter->rect().x() + m_shutter->rect().width() - m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterLeft(sight::viz::scene2d::vec2d_t _coord)
{
    vec2d_t shutterCoordPair = this->mapAdaptorToScene({m_shutter->rect().x(), m_shutter->rect().y()});

    return (_coord.x >= shutterCoordPair.x - m_clickCatchRange)
           && (_coord.x <= shutterCoordPair.x + m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterRight(sight::viz::scene2d::vec2d_t _coord)
{
    const vec2d_t shutterCoordPair = this->mapAdaptorToScene({m_shutter->rect().x(), m_shutter->rect().y()});

    const double shutterRightPos = shutterCoordPair.x + m_shutter->rect().width() * m_xAxis->getScale();

    return (_coord.x >= shutterRightPos - m_clickCatchRange)
           && (_coord.x <= shutterRightPos + m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
