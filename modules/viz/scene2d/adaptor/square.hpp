/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <service/macros.hpp>

#include <viz/scene2d/adaptor.hpp>

#include <QGraphicsItem>

namespace sight::module::viz::scene2d::adaptor
{

/**
 * @brief Create a square on the scene2D
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="square" type="sight::module::viz::scene2d::adaptor::square">
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
 * -\b set_double_parameter(double, std::string): set the double parameters 'x' and 'y'
 */
class square : public sight::viz::scene2d::adaptor
{
public:

    SIGHT_DECLARE_SERVICE(square, sight::viz::scene2d::adaptor);

    square() noexcept;
    ~square() noexcept override;

protected:

    void configuring() override;
    void starting() override;
    void updating() override;
    void stopping() override;

    void process_interaction(sight::viz::scene2d::data::event& _event) override;

    void set_color(const std::string& _color);
    bool coord_view_is_in_item(const sight::viz::scene2d::vec2d_t& _coord, QGraphicsItem* _item);
    sight::viz::scene2d::vec2d_t coord_view_to_coord_item(
        const sight::viz::scene2d::vec2d_t& _coord,
        QGraphicsItem* _item
    );

private:

    sight::viz::scene2d::vec2d_t m_coord {};
    std::uint32_t m_size {0};
    QColor m_color;
    QGraphicsItemGroup* m_layer {nullptr};
    QGraphicsRectItem* m_rec {nullptr};
    sight::viz::scene2d::vec2d_t m_old_coord {};

    bool m_point_is_captured {false};
    bool m_auto_refresh {true};
    bool m_interaction {true};
    static const core::com::slots::key_t SET_DOUBLE_PARAMETER_SLOT;
    void set_double_parameter(double _val, std::string _key);
};

} // namespace sight::module::viz::scene2d::adaptor
