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

#include "modules/viz/scene2d/adaptor/SScaleValues.hpp"

#include <service/macros.hpp>

#include <viz/scene2d/data/InitQtPen.hpp>
#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QGraphicsItemGroup>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

//---------------------------------------------------------------------------------------------------------------

void SScaleValues::configuring()
{
    this->configureParams(); // xAxis, yAxis, zValue, opacity

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    // Set the min/max values
    SIGHT_ASSERT("'min' atttribute is missing.", config.count("min"));
    SIGHT_ASSERT("'min' atttribute is missing.", config.count("max"));

    m_min = config.get<double>("min");
    m_max = config.get<double>("max");

    // Interval configuration
    if(config.count("interval"))
    {
        m_interval = config.get<float>("interval");
    }

    // Color configuration
    if(config.count("color"))
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_pen, config.get<std::string>("color"), m_opacity);
    }
    else
    {
        sight::viz::scene2d::data::InitQtPen::setPenColor(m_pen, "white", m_opacity);
    }

    // Font size configuration
    if(config.count("fontSize"))
    {
        m_fontSize = config.get<int>("fontSize");
    }

    // Show unit
    if(config.count("showUnit"))
    {
        const std::string showUnit = config.get<std::string>("showUnit");
        SIGHT_ASSERT(
            "Unsupported value " + showUnit + " for 'showUnit' attribute (true or false expected).",
            showUnit == "true" || showUnit == "false"
        );
        m_showUnit = (showUnit == "true");
    }

    // Unit text configuration
    m_displayedUnit = config.get<std::string>("unit", "");

    SIGHT_ASSERT(
        "'align' attribute is missing. "
        "Please add an 'align' attribute with value 'left', 'right', 'top' or 'bottom'",
        config.count("align")
    );

    // 'align' attribute configuration
    m_align = config.get<std::string>("align");

    SIGHT_ASSERT(
        "Unsupported value for 'align' attribute.",
        m_align == "left" || m_align == "right" || m_align == "top" || m_align == "bottom"
    );
}

//---------------------------------------------------------------------------------------------------------------

void SScaleValues::buildValues()
{
    SIGHT_ASSERT("m_interval can not be equal to 0", m_interval != 0);

    m_values.clear();

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
    for(int i = 0 ; i < nbValues ; ++i, val += m_interval)
    {
        QGraphicsSimpleTextItem* text = new QGraphicsSimpleTextItem();
        text->setText(QString::fromStdString(format).arg(val));
        text->setFont(m_font);
        text->setBrush(m_brush);

        m_values.push_back(text);
    }

    // Add the values to the item group
    for(QGraphicsItem* item : m_values)
    {
        m_layer->addToGroup(item);
    }

    // Build the unit graphic item
    m_unit = new QGraphicsSimpleTextItem();
    m_unit->setText(QString::fromStdString(m_displayedUnit));
    m_unit->setFont(m_font);
    m_unit->setBrush(m_brush);

    // Add it to the items to be displayed if required
    if(m_showUnit)
    {
        m_layer->addToGroup(m_unit);
    }

    // Set the layer position (according to the related axis) and zValue
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);

    // Add the layer containing grid's lines to the scene
    this->getScene2DRender()->getScene()->addItem(m_layer);
}

//---------------------------------------------------------------------------------------

void SScaleValues::starting()
{
    // Initialize the layer
    m_layer = new QGraphicsItemGroup();

    m_brush = QBrush(m_pen.color());
    m_pen.setCosmetic(true);

    // We set the point size to 1.f but we will scale the text later according to m_fontSize
    m_font.setPointSize(1);
    m_font.setKerning(true);
    m_font.setFixedPitch(true);

    this->buildValues();
    this->updating();
}

//---------------------------------------------------------------------------------------

double SScaleValues::getStartVal()
{
    return std::floor(m_min / m_interval) * m_interval;
}

//---------------------------------------------------------------------------------------
double SScaleValues::getEndVal()
{
    return std::floor(m_max / m_interval) * m_interval;
}

//---------------------------------------------------------------------------------------

void SScaleValues::updating()
{
    const auto viewport = m_viewport.lock();

    const auto& [scaleX, scaleY] = this->pixelsToViewport({m_fontSize, m_fontSize}, *viewport);

    QTransform transform;
    transform.scale(scaleX, scaleY);

    int step = 0;
    double valueSize;               // the size (width or height) of the current value
    Point2DType coord;              // coordinates of the current scale value item
    Point2DType size;               // size of the current scale value item
    bool suggestResampling = false; /* scale value items resampling is suggested because of
                                       a lack of sufficient width to display all of them */

    const std::size_t valuesSize = m_values.size();
    double val                   = getStartVal();

    const double viewportX      = viewport->getX();
    const double viewportWidth  = viewport->getWidth();
    const double viewportHeight = viewport->getHeight();

    if(m_align == "left" || m_align == "right")
    {
        const double valueSizeRatio = m_interval / scaleY;

        double coeff = 0.f;

        double textPosX;

        if(m_align == "left")
        {
            textPosX = viewportX;
        }
        else
        {
            coeff    = -1.f;
            textPosX = viewportX + viewportWidth;
        }

        for(std::size_t i = 0 ; i < valuesSize ; ++i, val += m_interval)
        {
            valueSize = m_values[i]->boundingRect().height();

            size = this->mapAdaptorToScene(Point2DType(m_values[i]->boundingRect().width(), valueSize));
            step = (int) (valueSize / valueSizeRatio) + 1;

            if(step > m_step)
            {
                m_step            = step;
                suggestResampling = true;
            }

            coord = this->mapAdaptorToScene((Point2DType(textPosX, val)));

            m_values[i]->setTransform(transform);

            m_values[i]->setPos(
                coord.first + coeff * size.first * scaleX,
                coord.second - (m_interval) * scaleY
            );
        }

        m_unit->setTransform(transform);

        val = viewportHeight * 0.8f;

        coord = this->mapAdaptorToScene((Point2DType(textPosX, val)));

        coeff = (m_align == "left") ? 1 : -1.5;

        m_unit->setPos(
            coord.first + coeff * 2 * size.first * scaleX,
            coord.second + size.second * scaleY
        );
    }
    else // axis centered on top or bottom
    {
        const double valueSizeRatio = m_interval / scaleX;

        float coeff = 0.5f;

        const double textPosY = (m_align == "bottom")
                                ? viewport->getY()
                                : viewportHeight * 0.9;

        for(std::size_t i = 0 ; i < valuesSize ; ++i, val += m_interval)
        {
            valueSize = m_values[i]->boundingRect().width();

            size = this->mapAdaptorToScene(Point2DType(valueSize, m_values[i]->boundingRect().height()));

            step = (int) (valueSize / valueSizeRatio) + 1;

            if(step > m_step)
            {
                m_step            = step;
                suggestResampling = true;
            }

            coord = this->mapAdaptorToScene(Point2DType(val, textPosY));

            m_values[i]->setTransform(transform);

            m_values[i]->setPos(
                coord.first - size.first / 2 * scaleX,
                coord.second - coeff * size.second / 2 * scaleY
            );
        }

        m_unit->setTransform(transform);

        val = viewportHeight * 0.8;

        size  = this->mapAdaptorToScene(Point2DType(m_unit->boundingRect().width(), m_unit->boundingRect().height()));
        coord = this->mapAdaptorToScene(Point2DType(viewportX + viewportWidth / 2, textPosY));
        coeff = (m_align == "left") ? 1 : -1.5;

        m_unit->setPos(
            coord.first - size.first * scaleX,
            coord.second - 1.5 * size.second * scaleY
        );
    }

    if(suggestResampling)
    {
        this->showHideScaleValues();
    }
    else if(!suggestResampling && step != m_step)
    {
        m_step = step;
        this->showHideScaleValues();
    }
}

//---------------------------------------------------------------------------------------

void SScaleValues::showHideScaleValues()
{
    double value;
    const int size        = (int) m_values.size();
    const double startVal = this->getStartVal();

    for(int i = 0 ; i < size ; ++i)
    {
        value = i * m_interval + startVal; // compute the value at index 'i'

        // Display this value or not according to the current step between two consecutive values
        // and in keeping the displaying of the value '0':
        m_values[i]->setVisible(fmod(value, (m_step * m_interval)) == 0.0);
    }
}

//---------------------------------------------------------------------------------------

void SScaleValues::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    if(_event.getType() == sight::viz::scene2d::data::Event::Resize)
    {
        this->updating();
    }
}

//---------------------------------------------------------------------------------------

void SScaleValues::stopping()
{
    // Remove the layer (and therefore all its related items) from the scene
    this->getScene2DRender()->getScene()->removeItem(m_layer);
}

//----------------------------------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SScaleValues::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_VIEWPORT_INPUT, sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

} // namespace adaptor

} // namespace sight::module::viz::scene2d
