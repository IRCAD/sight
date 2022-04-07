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

#include "modules/viz/scene2d/adaptor/SHistogram.hpp"

#include <core/com/Slots.hxx>

#include <data/helper/MedicalImage.hpp>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QGraphicsRectItem>
#include <QGraphicsView>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

const core::com::Slots::SlotKeyType s_IMAGE_CHANGE_SLOT = "imageChange";

//---------------------------------------------------------------------------------------------------------

SHistogram::SHistogram() noexcept
{
    newSlot(s_IMAGE_CHANGE_SLOT, &SHistogram::onImageChange, this);
}

//---------------------------------------------------------------------------------------------------------

SHistogram::~SHistogram() noexcept
{
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::configuring()
{
    this->configureParams(); // Looks for 'xAxis', 'yAxis' and 'zValue'

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_scale = m_yAxis->getScale();

    if(config.count("color"))
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_color, config.get<std::string>("color"));
    }

    m_cursorEnabled = config.get<bool>("cursor", m_cursorEnabled);

    if(config.count("cursorColor"))
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_cursorColor, config.get<std::string>("cursorColor"));
    }

    if(config.count("cursorBorderColor"))
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(
            m_cursorBorderColor,
            config.get<std::string>("cursorBorderColor")
        );
    }

    if(config.count("cursorSize"))
    {
        m_cursorSize = config.get<double>("cursorSize");
    }

    const std::string color = config.get<std::string>("cursorTextColor", "#FFFFFF");
    sight::viz::scene2d::data::InitQtPen::setPenColor(m_cursorTextColor, color);

    m_fontSize = config.get<int>("cursorFontSize", m_fontSize);
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::starting()
{
    if(m_cursorEnabled)
    {
        m_cursorLayer = new QGraphicsItemGroup();
        // Adjust the layer's position and zValue depending on the associated axis
        m_cursorLayer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
        m_cursorLayer->setZValue(m_zValue);
        this->getScene2DRender()->getScene()->addItem(m_cursorLayer);

        m_cursorItem = new QGraphicsEllipseItem();
        m_cursorItem->setBrush(m_cursorColor.color());
        m_cursorItem->setPen(m_cursorBorderColor);
        m_cursorItem->setZValue(m_zValue);
        m_color.setCosmetic(true);
        m_cursorLayer->addToGroup(m_cursorItem);

        // We set the point size to 1.f but we will scale the text later according to m_fontSize
        QFont font;
        font.setPointSize(1);
        font.setKerning(true);
        font.setFixedPitch(true);

        m_cursorText = new QGraphicsSimpleTextItem();
        m_cursorText->setBrush(QBrush(m_cursorTextColor.color()));
        m_cursorText->setFont(font);
        m_cursorText->setVisible(false);
        m_cursorLayer->addToGroup(m_cursorText);
    }

    const auto image = m_image.lock();
    m_histogram = std::make_unique<data::helper::Histogram>(image.get_shared());

    onImageChange();
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::updating()
{
    if(m_layer)
    {
        this->getScene2DRender()->getScene()->removeItem(m_layer);
        delete m_layer;
    }

    m_layer = new QGraphicsItemGroup();

    const auto image = m_image.lock();

    if(data::helper::MedicalImage::checkImageValidity(image.get_shared()))
    {
        const auto values = m_histogram->sample(m_histogramBinsWidth);

        if(!values.empty())
        {
            // Update color with opacity
            QColor color = m_color.color();
            color.setAlphaF(m_opacity);
            m_color.setColor(color);

            const double min       = m_histogram->min();
            const double binsWidth = static_cast<double>(m_histogramBinsWidth);

            QBrush brush = QBrush(m_color.color());

            // Build the graphic items:
            for(size_t i = 0 ; i < values.size() ; ++i)
            {
                Point2DType pt1 = this->mapAdaptorToScene({min + static_cast<double>(i) * binsWidth, values[i]});
                Point2DType pt2 = this->mapAdaptorToScene({min + static_cast<double>(i + 1) * binsWidth, values[i]});

                QPainterPath painter(QPointF(pt1.first, 0));
                painter.lineTo(pt1.first, pt1.second);
                painter.lineTo(pt2.first, pt1.second);
                painter.lineTo(pt2.first, 0);

                QGraphicsPathItem* item = new QGraphicsPathItem(painter);
                item->setPath(painter);
                item->setBrush(brush);
                item->setPen(Qt::NoPen);

                m_layer->addToGroup(item);
            }

            // Adjust the layer's position and zValue depending on the associated axis
            m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
            m_layer->setZValue(m_zValue);

            // Add to the scene the unique item which gather the whole set of rectangle graphic items:
            this->getScene2DRender()->getScene()->addItem(m_layer);
        }
    }
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::stopping()
{
    m_histogram.reset();

    if(m_layer)
    {
        this->getScene2DRender()->getScene()->removeItem(m_layer);
        delete m_layer;
        m_layer = nullptr;
    }

    if(m_cursorLayer)
    {
        this->getScene2DRender()->getScene()->removeItem(m_cursorLayer);
        delete m_cursorLayer;
        m_cursorLayer = nullptr;
    }
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    if(this->getStatus() != sight::service::IService::STARTED)
    {
        return;
    }

    bool updatePointedPos = false;

    // Vertical scaling
    if(_event.getType() == sight::viz::scene2d::data::Event::MouseWheelUp)
    {
        if(_event.getModifier() == sight::viz::scene2d::data::Event::ControlModifier)
        {
            m_histogramBinsWidth = std::min(
                static_cast<std::size_t>(m_histogram->max() - m_histogram->min()),
                m_histogramBinsWidth * 2
            );
            this->updating();
        }
        else
        {
            double scale = SCALE;
            if(_event.getModifier() == sight::viz::scene2d::data::Event::ShiftModifier)
            {
                scale = FAST_SCALE;
            }

            m_scale *= scale;

            m_layer->setTransform(QTransform::fromScale(1., scale), true);

            _event.setAccepted(true);
            m_yAxis->setScale(m_scale);

            auto viewport = m_viewport.lock();
            auto sig      = viewport->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
        }

        updatePointedPos = true;
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseWheelDown)
    {
        if(_event.getModifier() == sight::viz::scene2d::data::Event::ControlModifier)
        {
            m_histogramBinsWidth = std::max(std::size_t(1), m_histogramBinsWidth / 2);
            this->updating();
        }
        else
        {
            double scale = SCALE;
            if(_event.getModifier() == sight::viz::scene2d::data::Event::ShiftModifier)
            {
                scale = FAST_SCALE;
            }

            m_scale /= scale;

            m_layer->setTransform(QTransform::fromScale(1., 1. / scale), true);

            _event.setAccepted(true);
            m_yAxis->setScale(m_scale);

            auto viewport = m_viewport.lock();
            auto sig      = viewport->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
        }

        updatePointedPos = true;
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseMove)
    {
        updatePointedPos = true;
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress)
    {
        m_isInteracting  = true;
        updatePointedPos = true;
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonRelease)
    {
        m_isInteracting  = false;
        updatePointedPos = true;
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::EnterEvent)
    {
        m_entered        = true;
        updatePointedPos = true;
    }
    else if(_event.getType() == sight::viz::scene2d::data::Event::LeaveEvent)
    {
        m_entered        = false;
        updatePointedPos = true;
    }

    if(updatePointedPos)
    {
        this->updateCurrentPoint(_event);
    }
}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SHistogram::getAutoConnections() const
{
    return {
        {s_IMAGE_INPUT, data::Image::s_MODIFIED_SIG, s_IMAGE_CHANGE_SLOT},
        {s_IMAGE_INPUT, data::Image::s_BUFFER_MODIFIED_SIG, s_IMAGE_CHANGE_SLOT}
    };
}

//---------------------------------------------------------------------------------------------------------

void SHistogram::updateCurrentPoint(sight::viz::scene2d::data::Event& _event)
{
    if(m_cursorEnabled)
    {
        const auto values               = m_histogram->sample(m_histogramBinsWidth);
        const double histogramMinValue  = m_histogram->min();
        const double histogramBinsWidth = static_cast<double>(m_histogramBinsWidth);

        // Event coordinates in scene
        sight::viz::scene2d::data::Coord sceneCoord = this->getScene2DRender()->mapToScene(_event.getCoord());
        const double histIndex                      = sceneCoord.getX();
        const double index                          = histIndex - histogramMinValue;
        const size_t nbValues                       = values.size() * m_histogramBinsWidth;

        if(index >= 0.f && index < static_cast<double>(nbValues) && m_entered)
        {
            sight::viz::scene2d::data::Coord sceneCoord = this->getScene2DRender()->mapToScene(_event.getCoord());
            const double histIndex                      = sceneCoord.getX();
            const double index                          = histIndex - histogramMinValue;

            sight::viz::scene2d::data::Coord coord;
            coord.setX(sceneCoord.getX());
            coord.setY(static_cast<double>(values.at(static_cast<size_t>(index / histogramBinsWidth))) * m_scale);

            const auto viewport = m_viewport.lock();

            // Draw the cursor
            const auto& [diameterH, diameterV] = this->pixelsToViewport({m_cursorSize, m_cursorSize}, *viewport);

            const double x = coord.getX() - diameterH / 2;
            const double y = coord.getY() - diameterV / 2;

            m_cursorItem->setRect(x, y, diameterH, diameterV);
            m_cursorItem->setVisible(true);

            if(m_isInteracting)
            {
                // Draw the cursor text
                const auto& [scaleX, scaleY] = this->pixelsToViewport({m_fontSize, m_fontSize}, *viewport);

                // Event coordinates in scene
                m_cursorText->setText(QString::number(static_cast<int>((coord.getX()))));

                QTransform transform;
                transform.scale(scaleX, scaleY);

                m_cursorText->setTransform(transform);
                m_cursorText->setPos(coord.getX() + scaleX * 0.5, coord.getY() - scaleY * 0.5);
                m_cursorText->setVisible(true);
            }
            else
            {
                m_cursorText->setVisible(false);
            }
        }
        else
        {
            m_cursorItem->setVisible(false);
            m_cursorText->setVisible(false);
        }
    }
}

//------------------------------------------------------------------------------

void SHistogram::onImageChange()
{
    m_histogram->compute();
    m_histogramBinsWidth = static_cast<size_t>(m_histogram->max() - m_histogram->min()) / 50;
    this->updating();
}

} // namespace adaptor

} // namespace sight::module::viz::scene2d
