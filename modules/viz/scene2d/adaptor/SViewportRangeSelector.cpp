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

#include "modules/viz/scene2d/adaptor/SViewportRangeSelector.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/helper/MedicalImage.hpp>

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

void SViewportRangeSelector::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    const std::string color = config.get(s_COLOR_CONFIG, "#FFFFFF");
    sight::viz::scene2d::data::InitQtPen::setPenColor(m_color, color, m_opacity);
}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SViewportRangeSelector::getAutoConnections() const
{
    return {
        {s_SELECTED_VIEWPORT_INOUT, sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT},
        {s_IMAGE_INPUT, sight::data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT},
        {s_IMAGE_INPUT, sight::data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT}
    };
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::starting()
{
    double height = 0.;
    {
        auto viewport = m_viewport.lock();
        height = static_cast<int>(viewport->getHeight());

        // If the viewport Y and height are not set, scale the viewport to the height of the scene
        if(viewport->getY() == 0 && viewport->getHeight() == 0)
        {
            auto scene       = this->getScene2DRender()->getScene();
            const double ySc = scene->sceneRect().y();
            height = scene->sceneRect().height();

            viewport->setY(ySc);
            viewport->setHeight(height);
        }
    }

    const Point2DType pair = this->mapSceneToAdaptor(Point2DType(m_initialX, 1.0));

    QRectF rect(pair.first, 0, m_initialWidth * m_xAxis->getScale(), pair.second);
    {
        auto image = m_image.lock();
        if(image)
        {
            sight::data::helper::MedicalImage::getMinMax(image.get_shared(), m_imageMin, m_imageMax);
            rect.setX(m_imageMin);
            rect.setWidth(m_imageMax - m_imageMin);
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
    auto image = m_image.lock();
    if(image)
    {
        sight::data::helper::MedicalImage::getMinMax(image.get_shared(), m_imageMin, m_imageMax);
        auto viewport = m_viewport.lock();

        viewport->setX(m_imageMin);
        viewport->setWidth(m_imageMax - m_imageMin);
        this->getScene2DRender()->getView()->updateFromViewport(*viewport);

        auto sig = viewport->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }

        m_clickCatchRange = static_cast<int>(m_imageMax - m_imageMin) / 100;
    }
}

//---------------------------------------------------------------------------------------------------------------

void SViewportRangeSelector::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    // Event coordinates in scene
    sight::viz::scene2d::data::Coord coord;
    coord = this->getScene2DRender()->mapToScene(_event.getCoord());

    // Shutter coordinates in scene
    const Point2DType shutterCoordPair =
        this->mapAdaptorToScene(Point2DType(m_shutter->rect().x(), m_shutter->rect().y()));
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

        const auto image               = m_image.lock();
        const double leftSideBoundary  = image ? m_imageMin : sceneRect.x();
        const double rightSideBoundary = image ? m_imageMax : sceneRect.width() - sceneRect.x();
        if(m_isLeftInteracting)
        {
            // Shutter right side position
            const double rightSide = rect.x() + rect.width();

            if(coord.getX() < rightSide - m_clickCatchRange)
            {
                rect.setX(std::max(leftSideBoundary, coord.getX()));
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
            const double shutterRightPos = shutterCoordPair.first + newWidth;

            if(newWidth > m_clickCatchRange) // Shutter's width must be greater than the allowed picking range
            {
                if(shutterRightPos <= rightSideBoundary)
                {
                    rect.setWidth(newWidth);
                }
                else
                {
                    rect.setWidth(rightSideBoundary - shutterCoordPair.first);
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
                    core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
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

    const Point2DType fromSceneCoord = this->mapSceneToAdaptor(Point2DType(_x, _y));
    const Point2DType pair           = this->mapSceneToAdaptor(Point2DType(_width, _height));

    selectedViewport->setY(fromSceneCoord.second);
    selectedViewport->setHeight(selectedViewport->getHeight());

    auto image = m_image.lock();
    if(image)
    {
        selectedViewport->setX(std::max(fromSceneCoord.first, m_imageMin));
        selectedViewport->setWidth(std::min(pair.first, m_imageMax - m_imageMin));
    }
    else
    {
        selectedViewport->setX(fromSceneCoord.first);
        selectedViewport->setWidth(pair.first);
    }
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterMiddle(sight::viz::scene2d::data::Coord _coord)
{
    Point2DType shutterCoordPair;
    shutterCoordPair = this->mapAdaptorToScene({m_shutter->rect().x(), m_shutter->rect().y()});

    return (_coord.getX() > m_shutter->rect().x() + m_clickCatchRange)
           && (_coord.getX() < m_shutter->rect().x() + m_shutter->rect().width() - m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterLeft(sight::viz::scene2d::data::Coord _coord)
{
    Point2DType shutterCoordPair = this->mapAdaptorToScene({m_shutter->rect().x(), m_shutter->rect().y()});

    return (_coord.getX() >= shutterCoordPair.first - m_clickCatchRange)
           && (_coord.getX() <= shutterCoordPair.first + m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

bool SViewportRangeSelector::mouseOnShutterRight(sight::viz::scene2d::data::Coord _coord)
{
    const Point2DType shutterCoordPair = this->mapAdaptorToScene({m_shutter->rect().x(), m_shutter->rect().y()});

    const double shutterRightPos = shutterCoordPair.first + m_shutter->rect().width() * m_xAxis->getScale();

    return (_coord.getX() >= shutterRightPos - m_clickCatchRange)
           && (_coord.getX() <= shutterRightPos + m_clickCatchRange);
}

//---------------------------------------------------------------------------------------------------------------

} // namespace adaptor

} // namespace sight::module::viz::scene2d
