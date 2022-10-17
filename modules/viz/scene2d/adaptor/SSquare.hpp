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

#pragma once

#include "modules/viz/scene2d/config.hpp"

#include <service/macros.hpp>

#include <viz/scene2d/IAdaptor.hpp>

#include <QGraphicsItem>

namespace sight::module::viz::scene2d::adaptor
{

/**
 * @brief Create a square on the scene2D
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="square" type="sight::module::viz::scene2d::adaptor::SSquare">
      <config x="20" y="20" size="30" color="blue" zValue="3" autoRefresh="false" interaction="false" />
   </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b x (mandatory): specify x square coordinate
 *    - \b y (mandatory): specify y square coordinate
 *    - \b size (mandatory): specify size of the square
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b color (optional, default=black): color of the square
 *    - \b autoRefresh (optional, default=true): specify if the position is updated as soon as a new position is
 * received
 *    - \b interaction (optional, default=true): enable or disable mouse interaction with the square
 *
 * @section Slots Slots
 * -\b setDoubleParameter(double, std::string): set the double parameters 'x' and 'y'
 */
class MODULE_VIZ_SCENE2D_CLASS_API SSquare : public sight::viz::scene2d::IAdaptor
{
public:

    SIGHT_DECLARE_SERVICE(SSquare, sight::viz::scene2d::IAdaptor);

    MODULE_VIZ_SCENE2D_API SSquare() noexcept;
    MODULE_VIZ_SCENE2D_API ~SSquare() noexcept override;

protected:

    MODULE_VIZ_SCENE2D_API void configuring() override;
    MODULE_VIZ_SCENE2D_API void starting() override;
    MODULE_VIZ_SCENE2D_API void updating() override;
    MODULE_VIZ_SCENE2D_API void stopping() override;

    MODULE_VIZ_SCENE2D_API void processInteraction(sight::viz::scene2d::data::Event& _event) override;

    MODULE_VIZ_SCENE2D_API void setColor(const std::string& _color);
    MODULE_VIZ_SCENE2D_API bool coordViewIsInItem(const sight::viz::scene2d::vec2d_t& coord, QGraphicsItem* item);
    MODULE_VIZ_SCENE2D_API sight::viz::scene2d::vec2d_t coordViewToCoordItem(
        const sight::viz::scene2d::vec2d_t& coord,
        QGraphicsItem* item
    );

private:

    sight::viz::scene2d::vec2d_t m_coord {};
    std::uint32_t m_size {0};
    QColor m_color;
    QGraphicsItemGroup* m_layer {nullptr};
    QGraphicsRectItem* m_rec {nullptr};
    sight::viz::scene2d::vec2d_t m_oldCoord {};

    bool m_pointIsCaptured {false};
    bool m_autoRefresh {true};
    bool m_interaction {true};
    static const core::com::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    void setDoubleParameter(double val, std::string key);
};

} // namespace sight::module::viz::scene2d::adaptor
