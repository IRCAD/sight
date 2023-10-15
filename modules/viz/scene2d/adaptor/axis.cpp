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

#include "modules/viz/scene2d/adaptor/axis.hpp"

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <glm/common.hpp>

#include <QGraphicsItemGroup>

#include <cmath>

using sight::viz::scene2d::vec2d_t;

namespace sight::module::viz::scene2d::adaptor
{

axis::axis() noexcept =
    default;

//--------------------------------------------------------------------------------------------------

axis::~axis() noexcept =
    default;

//--------------------------------------------------------------------------------------------------

void axis::starting()
{
    m_layer = new QGraphicsItemGroup();

    this->buildAxis();
    this->buildLabels();

    // Adjust the layer's position and zValue depending on the associated axis
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Add to the scene the unique item which gather the whole set of rectangle graphic items:
    this->getScene2DRender()->getScene()->addItem(m_layer);

    this->updating();
}

//--------------------------------------------------------------------------------------------------

void axis::stopping()
{
    delete m_layer;
}

//--------------------------------------------------------------------------------------------------

void axis::configuring()
{
    this->configureParams(); // Looks for 'xAxis', 'yAxis' and 'zValue'

    const config_t srvConfig = this->get_config();
    const config_t config    = srvConfig.get_child("config.<xmlattr>");

    // 'color'
    if(config.count("color") != 0U)
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_line.color, config.get<std::string>("color"));
    }

    // 'align' attribute configuration
    m_align = config.get<std::string>("align");
    SIGHT_ASSERT(
        "'align' attribute is missing. Please add an 'align' attribute "
        "with value 'left', 'right', 'top' or 'bottom'",
        !m_align.empty()
    );
    SIGHT_ASSERT(
        "Unsupported value for 'align' attribute.",
        m_align == "left" || m_align == "right" || m_align == "top" || m_align == "bottom"
    );

    // axis bounds
    SIGHT_ASSERT("'min' attribute is missing.", config.count("min"));
    SIGHT_ASSERT("'max' attribute is missing.", config.count("max"));
    m_min = config.get<double>("min");
    m_max = config.get<double>("max");
    SIGHT_FATAL_IF("'max' attribute should be greater than 'min'.", m_max <= m_min);

    // Ticks size
    m_line.tickSize = config.get<double>("tickSize", m_line.tickSize);

    // Step
    m_interval = config.get<double>("interval", m_interval);

    const auto labelConfig = srvConfig.get_child_optional("config.labels.<xmlattr>");
    if(labelConfig.has_value())
    {
        // Font size configuration
        m_labels.fontSize = labelConfig->get<int>("fontSize", m_labels.fontSize);

        // Show unit
        m_labels.showUnit = labelConfig->get<bool>("showUnit", m_labels.showUnit);

        // Unit text configuration
        m_labels.displayedUnit = labelConfig->get<std::string>("unit", "");

        // Color configuration
        if(labelConfig->count("color") != 0U)
        {
            sight::viz::scene2d::data::InitQtPen::setPenColor(
                m_labels.pen,
                labelConfig->get<std::string>("color"),
                m_opacity
            );
        }
        else
        {
            sight::viz::scene2d::data::InitQtPen::setPenColor(m_labels.pen, "white", m_opacity);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------

void axis::buildAxis()
{
    m_line.color.setCosmetic(true);
    const int nbValues = static_cast<int>(std::ceil((m_max - m_min) / m_interval)) + 1;

    for(int i = 0 ; i < nbValues ; ++i)
    {
        auto* tick = new QGraphicsLineItem(0, 0, 0, 0);
        tick->setPen(m_line.color);

        m_line.ticks.push_back(tick);
        m_layer->addToGroup(tick);
    }

    m_line.line = new QGraphicsLineItem();
    m_line.line->setPen(m_line.color);
    m_layer->addToGroup(m_line.line);
}

//---------------------------------------------------------------------------------------------------------------

void axis::buildLabels()
{
    SIGHT_ASSERT("m_interval can not be equal to 0", m_interval != 0);

    m_labels.brush = QBrush(m_labels.pen.color());
    m_labels.pen.setCosmetic(true);

    // We may not have a viewport with the same range magnitude on both axis, thus specifying the font size with only
    // 1 dimension does not work. The font would be deformed.
    // Since we can't specify a width and height for the font size, we are forced to use a QTransform to scale the font
    // and keep a correct display ratio.
    // We would like to set the point size to 1.f and scale the text later with a QTransform according to m_"fontSize",
    // but under Windows, rendering a font with a tiny size like 1,2, etc... gives incorrect results in terms of
    // letter spacing and weight. Thus we "pretend" to use a font of a more common size like 10 and take that extra
    // scale into account in our QTransform (in updateLabels())
    m_labels.font.setPixelSize(static_cast<int>(m_labels.extraScale));
    m_labels.font.setKerning(true);
    m_labels.font.setFixedPitch(true);

    m_labels.values.clear();

    double val         = this->getStartVal();
    const int range    = static_cast<int>(std::ceil(this->getEndVal() - val));
    const int nbValues = static_cast<int>((std::ceil(range / m_interval))) + 1;

    std::string format;

    if(m_align == "left")
    {
        format = "  %1";
    }
    else if(m_align == "right")
    {
        format = "%1  ";
    }
    else
    {
        format = "%1";
    }

    // Build the values as graphic items
    for(int i = 0 ; i < nbValues ; ++i)
    {
        auto* text = new QGraphicsSimpleTextItem();
        text->setText(QString::fromStdString(format).arg(val));
        text->setFont(m_labels.font);
        text->setBrush(m_labels.brush);

        m_labels.values.push_back(text);

        val += m_interval;
    }

    // Add the values to the item group
    for(QGraphicsItem* item : m_labels.values)
    {
        m_layer->addToGroup(item);
    }

    // Build the unit graphic item
    m_labels.unit = new QGraphicsSimpleTextItem();
    m_labels.unit->setText(QString::fromStdString(m_labels.displayedUnit));
    m_labels.unit->setFont(m_labels.font);
    m_labels.unit->setBrush(m_labels.brush);

    // Add it to the items to be displayed if required
    if(m_labels.showUnit)
    {
        m_layer->addToGroup(m_labels.unit);
    }
}

//---------------------------------------------------------------------------------------

double axis::getStartVal() const
{
    return std::floor(m_min / m_interval) * m_interval;
}

//---------------------------------------------------------------------------------------

double axis::getEndVal() const
{
    return std::ceil(m_max / m_interval) * m_interval;
}

//---------------------------------------------------------------------------------------------------------------

void axis::updating()
{
    updateAxis();
    updateLabels();
}

//---------------------------------------------------------------------------------------

void axis::updateAxis()
{
    const auto viewport       = m_viewport.lock();
    const auto viewToViewport = this->viewToViewport(*viewport);

    const std::size_t nbValues = m_line.ticks.size();

    const double min = this->getStartVal();
    const double max = this->getEndVal();

    double pos = NAN;
    vec2d_t tickSize;
    vec2d_t tickPos {0., 0.};

    if(m_align == "bottom")
    {
        tickSize = this->mapAdaptorToScene((vec2d_t(0, m_line.tickSize)));

        const double tickPosY = 0.;

        for(std::size_t i = 0 ; i < nbValues ; ++i)
        {
            pos     = min + static_cast<double>(i) * m_interval;
            tickPos = this->mapAdaptorToScene((vec2d_t(pos, tickPosY)));
            m_line.ticks.at(i)->setLine(tickPos.x, tickPos.y, tickPos.x, tickPos.y - tickSize.y * viewToViewport.y);
        }

        m_line.line->setLine(min, tickPos.y, max, tickPos.y);
    }
    else if(m_align == "top")
    {
        tickSize = this->mapAdaptorToScene((vec2d_t(0, m_line.tickSize)));

        const double tickPosY = 1.0;

        for(std::size_t i = 0 ; i < nbValues ; ++i)
        {
            pos     = min + static_cast<double>(i) * m_interval;
            tickPos = this->mapAdaptorToScene((vec2d_t(pos, tickPosY)));
            m_line.ticks.at(i)->setLine(tickPos.x, tickPos.y, tickPos.x, tickPos.y - tickSize.y * viewToViewport.y);
        }

        m_line.line->setLine(min, tickPos.y, max, tickPos.y);
    }
    else if(m_align == "left")
    {
        tickSize = this->mapAdaptorToScene((vec2d_t(m_line.tickSize, 0)));

        const double tickPosX = viewport->x();

        for(std::size_t i = 0 ; i < nbValues ; ++i)
        {
            pos     = min + static_cast<double>(i) * m_interval;
            tickPos = this->mapAdaptorToScene((vec2d_t(tickPosX, pos)));
            m_line.ticks.at(i)->setLine(tickPos.x, tickPos.y, tickPos.x + tickSize.x * viewToViewport.x, tickPos.y);
        }

        m_line.line->setLine(tickPos.x, min, tickPos.x, tickPos.y);
    }
    else if(m_align == "right")
    {
        tickSize = this->mapAdaptorToScene((vec2d_t(m_line.tickSize, 0)));

        const double tickPosX = viewport->x() + viewport->width();

        for(std::size_t i = 0 ; i < nbValues ; ++i)
        {
            pos = min + static_cast<double>(i) * m_interval;

            tickPos = this->mapAdaptorToScene((vec2d_t(tickPosX, pos)));
            m_line.ticks.at(i)->setLine(tickPos.x - tickSize.x * viewToViewport.x, tickPos.y, tickPos.x, tickPos.y);
        }

        m_line.line->setLine(tickPos.x, min, tickPos.x, tickPos.y);
    }
}

//---------------------------------------------------------------------------------------

void axis::updateLabels()
{
    const auto viewport = m_viewport.lock();

    const auto viewToViewport = this->viewToViewport(*viewport);
    const auto sizeVP         = vec2d_t(m_labels.fontSize, m_labels.fontSize) * viewToViewport / m_labels.extraScale;
    const vec2d_t intervalSC  = glm::abs(this->mapAdaptorToScene(vec2d_t(m_interval, m_interval)));

    QTransform transform;
    transform.scale(sizeVP.x, sizeVP.y);

    const std::size_t valuesSize = m_labels.values.size();
    double val                   = getStartVal();

    const double viewportX      = viewport->x();
    const double viewportWidth  = viewport->width();
    const double viewportHeight = viewport->height();

    if(m_align == "left" || m_align == "right")
    {
        double coeff = 0.F;

        double textPosX = NAN;

        if(m_align == "left")
        {
            textPosX = viewportX;
        }
        else
        {
            coeff    = -1.F;
            textPosX = viewportX + viewportWidth;
        }

        for(std::size_t i = 0 ; i < valuesSize ; ++i)
        {
            const vec2d_t coord = this->mapAdaptorToScene((vec2d_t(textPosX, val)));

            const vec2d_t size =
                this->mapAdaptorToScene(
                    vec2d_t(
                        m_labels.values[i]->boundingRect().width(),
                        m_labels.values[i]->boundingRect().height()
                    )
                );

            m_labels.values[i]->setTransform(transform);
            m_labels.values[i]->setPos(coord.x + coeff * size.x * sizeVP.x, coord.y);

            val += m_interval;
        }

        // Always displays the maximum value but we may hide other values depending on available size
        double prevAvailableSize = 0.;
        for(std::size_t i = 1 ; i < valuesSize ; ++i)
        {
            const std::size_t idx = valuesSize - i - 1;
            const vec2d_t size    =
                vec2d_t(
                    m_labels.values[idx]->boundingRect().width(),
                    m_labels.values[idx]->boundingRect().height()
                );

            const double margin = 0.8;
            const auto sizeY    = std::abs(size.y * margin * sizeVP.y);

            if(sizeY < intervalSC.y + prevAvailableSize)
            {
                m_labels.values[idx]->setVisible(true);
                prevAvailableSize = 0.;
            }
            else
            {
                m_labels.values[idx]->setVisible(false);
                prevAvailableSize += intervalSC.y;
            }
        }

        m_labels.unit->setTransform(transform);

        val = viewportHeight * 0.8F;

        const vec2d_t size =
            this->mapAdaptorToScene(
                vec2d_t(
                    m_labels.unit->boundingRect().width(),
                    m_labels.unit->boundingRect().height()
                )
            );

        const vec2d_t coord = this->mapAdaptorToScene((vec2d_t(textPosX, val)));
        coeff = (m_align == "left") ? 1 : -1.5;
        m_labels.unit->setPos(coord.x + coeff * 2 * size.x * sizeVP.x, coord.y);
    }
    else // axis centered on top or bottom
    {
        float coeff = 0.5F;

        const double textPosY = (m_align == "bottom") ? 0 : 1.1;

        for(std::size_t i = 0 ; i < valuesSize ; ++i)
        {
            const vec2d_t coord = this->mapAdaptorToScene(vec2d_t(val, textPosY));

            const vec2d_t size =
                this->mapAdaptorToScene(
                    vec2d_t(
                        m_labels.values[i]->boundingRect().width(),
                        m_labels.values[i]->boundingRect().height()
                    )
                );

            m_labels.values[i]->setTransform(transform);
            m_labels.values[i]->setPos(coord.x - size.x / 2 * sizeVP.x, coord.y - coeff * size.y / 2 * sizeVP.y);

            val += m_interval;
        }

        // Always displays the maximum value but we may hide other values depending on available size
        double prevAvailableSize = 0.;
        for(std::size_t i = 1 ; i < valuesSize ; ++i)
        {
            const vec2d_t size =
                vec2d_t(
                    m_labels.values[i]->boundingRect().width(),
                    m_labels.values[i]->boundingRect().height()
                );

            const double margin = 1.25;
            const auto sizeX    = size.x * margin * sizeVP.x;
            if(sizeX < intervalSC.x + prevAvailableSize)
            {
                m_labels.values[i]->setVisible(true);
                prevAvailableSize = 0.;
            }
            else
            {
                m_labels.values[i]->setVisible(false);
                prevAvailableSize += intervalSC.x;
            }
        }

        m_labels.unit->setTransform(transform);

        const vec2d_t size =
            this->mapAdaptorToScene(
                vec2d_t(
                    m_labels.unit->boundingRect().width(),
                    m_labels.unit->boundingRect().height()
                )
            );
        const vec2d_t coord = this->mapAdaptorToScene(vec2d_t(viewportX + viewportWidth * .5, textPosY));
        coeff = (m_align == "top") ? 1 : -1.5;

        m_labels.unit->setPos(coord.x - size.x * sizeVP.x, coord.y + coeff * size.y * sizeVP.y);
    }
}

//---------------------------------------------------------------------------------------

void axis::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    if(_event.getType() == sight::viz::scene2d::data::Event::Resize)
    {
        this->updating();
    }
}

//----------------------------------------------------------------------------------------------------------

service::connections_t axis::auto_connections() const
{
    connections_t connections;
    connections.push(s_VIEWPORT_INPUT, sight::viz::scene2d::data::Viewport::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//--------------------------------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
