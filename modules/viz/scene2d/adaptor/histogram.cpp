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

#include "modules/viz/scene2d/adaptor/histogram.hpp"

#include <core/com/slots.hxx>

#include <data/helper/MedicalImage.hpp>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QGraphicsRectItem>
#include <QGraphicsView>

using sight::viz::scene2d::vec2d_t;

namespace sight::module::viz::scene2d::adaptor
{

const core::com::slots::key_t IMAGE_CHANGE_SLOT = "imageChange";

//---------------------------------------------------------------------------------------------------------

histogram::histogram() noexcept
{
    new_slot(IMAGE_CHANGE_SLOT, &histogram::onImageChange, this);
}

//---------------------------------------------------------------------------------------------------------

histogram::~histogram() noexcept =
    default;

//---------------------------------------------------------------------------------------------------------

void histogram::configuring()
{
    this->configureParams(); // Looks for 'xAxis', 'yAxis' and 'zValue'

    const config_t srvConfig = this->get_config();
    const config_t config    = srvConfig.get_child("config.<xmlattr>");

    m_scale = m_yAxis->getScale();

    if(config.count("color") != 0U)
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_color, config.get<std::string>("color"));
    }

    const auto cursorConfig = srvConfig.get_child_optional("config.cursor");
    if(cursorConfig.has_value())
    {
        std::string color;
        m_cursorEnabled = true;

        color = cursorConfig->get<std::string>("<xmlattr>.color", "");
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_cursorColor, color);

        color = cursorConfig->get<std::string>("<xmlattr>.borderColor", "");
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_cursorBorderColor, color);

        m_cursorSize = cursorConfig->get<double>("<xmlattr>.size", m_cursorSize);

        color = cursorConfig->get<std::string>("<xmlattr>.textColor", "#FFFFFF");
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_cursorLabelColor, color);

        m_fontSize = cursorConfig->get<int>("<xmlattr>.fontSize", m_fontSize);
    }
}

//---------------------------------------------------------------------------------------------------------

void histogram::starting()
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

        m_cursorLabel = new QGraphicsSimpleTextItem();
        m_cursorLabel->setBrush(QBrush(m_cursorLabelColor.color()));
        m_cursorLabel->setFont(font);
        m_cursorLabel->setVisible(false);
        m_cursorLayer->addToGroup(m_cursorLabel);
    }

    const auto image = m_image.lock();
    m_histogram = std::make_unique<data::helper::Histogram>(image.get_shared());

    onImageChange();
}

//---------------------------------------------------------------------------------------------------------

void histogram::updating()
{
    if(m_layer != nullptr)
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

            const double min     = m_histogram->min();
            const auto binsWidth = static_cast<double>(m_histogramBinsWidth);

            QBrush brush = QBrush(m_color.color());

            // Build the graphic items:
            for(std::size_t i = 0 ; i < values.size() ; ++i)
            {
                vec2d_t pt1 = this->mapAdaptorToScene({min + static_cast<double>(i) * binsWidth, values[i]});
                vec2d_t pt2 = this->mapAdaptorToScene({min + static_cast<double>(i + 1) * binsWidth, values[i]});

                QPainterPath painter(QPointF(pt1.x, 0));
                painter.lineTo(pt1.x, pt1.y);
                painter.lineTo(pt2.x, pt1.y);
                painter.lineTo(pt2.x, 0);

                auto* item = new QGraphicsPathItem(painter);
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

void histogram::stopping()
{
    m_histogram.reset();

    if(m_layer != nullptr)
    {
        this->getScene2DRender()->getScene()->removeItem(m_layer);
        delete m_layer;
        m_layer = nullptr;
    }

    if(m_cursorLayer != nullptr)
    {
        this->getScene2DRender()->getScene()->removeItem(m_cursorLayer);
        delete m_cursorLayer;
        m_cursorLayer = nullptr;
    }
}

//---------------------------------------------------------------------------------------------------------

void histogram::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    if(this->status() != sight::service::base::STARTED)
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
            auto sig      = viewport->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
            sig->async_emit();
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
            auto sig      = viewport->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
            sig->async_emit();
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

service::connections_t histogram::auto_connections() const
{
    return {
        {s_IMAGE_INPUT, data::image::MODIFIED_SIG, IMAGE_CHANGE_SLOT},
        {s_IMAGE_INPUT, data::image::BUFFER_MODIFIED_SIG, IMAGE_CHANGE_SLOT}
    };
}

//---------------------------------------------------------------------------------------------------------

void histogram::updateCurrentPoint(sight::viz::scene2d::data::Event& _event)
{
    if(m_cursorEnabled)
    {
        const auto values              = m_histogram->sample(m_histogramBinsWidth);
        const double histogramMinValue = m_histogram->min();
        const auto histogramBinsWidth  = static_cast<double>(m_histogramBinsWidth);

        // Event coordinates in scene
        sight::viz::scene2d::vec2d_t sceneCoord = this->getScene2DRender()->mapToScene(_event.getCoord());
        const double histIndex                  = sceneCoord.x;
        const double index                      = histIndex - histogramMinValue;
        const std::size_t nbValues              = values.size() * m_histogramBinsWidth;

        const auto viewport       = m_viewport.lock();
        const auto viewToViewport = this->viewToViewport(*viewport);

        if(index >= 0.F && index < static_cast<double>(nbValues) && m_entered)
        {
            sight::viz::scene2d::vec2d_t coord;
            coord.x = sceneCoord.x;
            coord.y = static_cast<double>(values.at(static_cast<std::size_t>(index / histogramBinsWidth))) * m_scale;

            // Draw the cursor
            const vec2d_t diameter = vec2d_t(m_cursorSize, m_cursorSize) * viewToViewport;

            const double x = coord.x - diameter.x / 2;
            const double y = coord.y - diameter.y / 2;

            m_cursorItem->setRect(x, y, diameter.x, diameter.y);
            m_cursorItem->setVisible(true);

            if(m_isInteracting)
            {
                // Draw the cursor text
                const vec2d_t scale = vec2d_t(m_fontSize, m_fontSize) * viewToViewport;

                // Event coordinates in scene
                m_cursorLabel->setText(QString::number(static_cast<int>((coord.x))));

                QTransform transform;
                transform.scale(scale.x, scale.y);

                m_cursorLabel->setTransform(transform);
                m_cursorLabel->setPos(coord.x + scale.x * 0.5, coord.y - scale.y * 0.5);
                m_cursorLabel->setVisible(true);
            }
            else
            {
                m_cursorLabel->setVisible(false);
            }
        }
        else
        {
            m_cursorItem->setVisible(false);
            m_cursorLabel->setVisible(false);
        }
    }
}

//------------------------------------------------------------------------------

void histogram::onImageChange()
{
    m_histogram->compute();
    m_histogramBinsWidth = static_cast<std::size_t>(m_histogram->max() - m_histogram->min()) / 50;
    this->updating();
}

} // namespace sight::module::viz::scene2d::adaptor
