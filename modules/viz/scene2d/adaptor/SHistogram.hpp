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

#include <data/Point.hpp>

#include <viz/scene2d/IAdaptor.hpp>

namespace sight::modules::viz::scene2d
{

namespace adaptor
{

/**
 * @brief   IAdaptor implementation for histogram data.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="histogram" type="::sight::modules::viz::scene2d::adaptor::SHistogram" autoConnect="yes">
       <in key="histogram" uid="..." />
       <inout key="point" uid="..." optional="yes" />
       <config xAxis="xAxis" yAxis="axeHistogramY" color="gray" opacity="0.25" zValue="5"/>
   </service>
   @endcode
 *
 * @subsection In In
 * - \b histogram [sight::data::Histogram]: histogram to display.
 *
 * @subsection In-Out In-Out
 * - \b point [sight::data::Point](optional): histogram point, used to show information at the current histogram index
 * pointed by the mouse.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b color (optional, default black) : the background color of the histogram
 *    - \b opacity (optional, default=1.0): adaptor opacity (float)
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 */
class MODULE_VIZ_SCENE2D_CLASS_API SHistogram : public sight::viz::scene2d::IAdaptor
{
public:
    fwCoreServiceMacro(SHistogram,  ::sight::viz::scene2d::IAdaptor)

    MODULE_VIZ_SCENE2D_API SHistogram() noexcept;
    MODULE_VIZ_SCENE2D_API virtual ~SHistogram() noexcept;

protected:
    MODULE_VIZ_SCENE2D_API void configuring() override;
    MODULE_VIZ_SCENE2D_API void starting() override;
    MODULE_VIZ_SCENE2D_API void updating() override;
    MODULE_VIZ_SCENE2D_API void stopping() override;

    MODULE_VIZ_SCENE2D_API void processInteraction( sight::viz::scene2d::data::Event& _event ) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect SHistogram::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    MODULE_VIZ_SCENE2D_API services::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Ratio used for vertical scaling (default value: 1.1)
    static const float SCALE;

private:
    /// Update the value of m_ordinateValueUID according to the value pointed by mouse cursor.
    void updateCurrentPoint( sight::viz::scene2d::data::Event& _event, const data::Point::sptr& point );

    /// Color used for graphic item's inner and border color
    QPen m_color{Qt::green};

    /// Opacity
    float m_opacity{0.80f};

    /// Current vertical scaling ratio
    float m_scale{1.0f};

    // Graphics items contained into m_items are also added to this group item,
    // which is then added to the scene.
    // (This is the only graphic item which has to be added into the scene).
    QGraphicsItemGroup* m_layer{nullptr};
};

}   // namespace adaptor

}   // namespace sight::modules::viz::scene2d
