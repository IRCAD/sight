/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/real.hpp>

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
 *
 * @subsection ConfigBindings Configuration data bindings
 * Configuration inputs accept literals or shared data objects.
 * Default values apply when no configuration value is provided.
 * - \b config.x [sight::data::real]: x.
 * - \b config.y [sight::data::real]: y.
 */
class square : public sight::viz::scene2d::adaptor
{
public:

    SIGHT_DECLARE_SERVICE(square, sight::viz::scene2d::adaptor);

    square() noexcept;
    ~square() noexcept override = default;

    void process_interaction(sight::viz::scene2d::data::event& _event) override;

protected:

    sight::service::connections_t auto_connections() const override;

    void configuring() override;
    void starting() override;
    void updating() override;
    void stopping() override;

    void set_color(const std::string& _color);
    bool coord_view_is_in_item(const sight::viz::scene2d::vec2d_t& _coord, QGraphicsItem* _item);
    sight::viz::scene2d::vec2d_t coord_view_to_coord_item(
        const sight::viz::scene2d::vec2d_t& _coord,
        QGraphicsItem* _item
    );

private:

    /// Apply settings from bound data at startup and after external modifications.
    void apply_config_x();
    void apply_config_y();

    std::uint32_t m_size {0};
    QColor m_color;
    QGraphicsItemGroup* m_layer {nullptr};
    QGraphicsRectItem* m_rec {nullptr};
    sight::viz::scene2d::vec2d_t m_old_coord {};

    bool m_point_is_captured {false};
    bool m_auto_refresh {true};
    bool m_interaction {true};

    /// Typed config.x configuration binding.
    ptr_in<sight::data::real> m_x {this, "config.x", 0.};

    /// Typed config.y configuration binding.
    ptr_in<sight::data::real> m_y {this, "config.y", 0.};
};

} // namespace sight::module::viz::scene2d::adaptor
