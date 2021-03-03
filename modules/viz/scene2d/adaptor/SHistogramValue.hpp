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

#pragma once

#include "modules/viz/scene2d/config.hpp"

#include <viz/scene2d/data/Viewport.hpp>
#include <viz/scene2d/IAdaptor.hpp>

#include <QGraphicsTextItem>

namespace sight::module::viz::scene2d
{
namespace adaptor
{

/**
 * @brief Defines an adaptor to show the histogram values clicked by mouse.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="histogram" type="sight::module::viz::scene2d::adaptor::SHistogramValue" autoConnect="yes">
       <in key="histogram" uid="..." />
       <in key="point" uid="..." />
       <in key="viewport"  uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" color="gray" opacity="0.25" zValue="5" fontSize="8"/>
   </service>
   @endcode
 *
 * @subsection In In
 * - \b histogram [sight::data::Histogram]: histogram to display.
 * - \b point [sight::data::Point]: histogram point, used to show information at the current histogram index pointed by
 * the
 * mouse.
 * - \b viewport [sight::viz::scene2d::data::Viewport]: object listened to update adaptor.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration.
 *    - \b xAxis (optional): x axis associated to the adaptor.
 *    - \b yAxis (optional): y axis associated to the adaptor.
 *    - \b zValue (optional, default="0"): z value of the layer.
 *    - \b opacity (optional, default="1.0"): adaptor opacity.
 *    - \b color (optional, default="#FFFFFF"): color of the text
 *    - \b fontSize (optional, default="8"): size of the font used to display the current value.
 */
class SHistogramValue : public sight::viz::scene2d::IAdaptor
{

public:

    SIGHT_DECLARE_SERVICE(SHistogramValue, ::sight::viz::scene2d::IAdaptor)

    /// Creates the adaptor.
    MODULE_VIZ_SCENE2D_API SHistogramValue() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE2D_API virtual ~SHistogramValue() noexcept;

private:

    /// Configures the adaptor.
    virtual void configuring() override;

    /// Initializes the layer and the font.
    virtual void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG of s_VIEWPORT_INPUT to
     * module::viz::scene2d::adaptor::SHistogramValue::s_UPDATE_SLOT.
     * Connect data::Histogram::s_MODIFIED_SIG of s_HISTOGRAM_INPUT to
     * module::viz::scene2d::adaptor::SHistogramValue::s_UPDATE_SLOT.
     */
    virtual service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Draws the text at the clicked position.
    virtual void updating() override;

    /// Does nothing.
    virtual void stopping() override;

    /**
     * @brief Filters the event to call the right methods from mouse informations.
     * @param _event the 2D scene event.
     */
    virtual void processInteraction(sight::viz::scene2d::data::Event& _event ) override;

    /// Defines the color used for graphic item's inner color.
    QPen m_color;

    /// Stores the item which display the current values of the associated histogram pointed by this cursor.
    QGraphicsSimpleTextItem* m_text;

    /// Sets the display status.
    bool m_isInteracting;

    /// Defiens the font used for displaying the text.
    QFont m_font;

    /// Defines  the coordinates of the current event.
    sight::viz::scene2d::data::Coord m_coord;

    /// Defines the size of the font used for rendering the current value of this tracker.
    float m_fontSize;

    /// Stores the main layer.
    QGraphicsItemGroup* m_layer;

};

}   // namespace adaptor
}   // namespace sight::module::viz::scene2d
