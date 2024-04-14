/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <data/image.hpp>

#include <viz/scene2d/adaptor.hpp>
#include <viz/scene2d/data/viewport.hpp>

namespace sight::module::viz::scene2d::adaptor
{

/**
 * @brief Adaptor implementation that manages the camera on the view (by updating the viewport object).
 *
 * The viewport_updater updates the viewport of the scene it is attached to, by picking values from the
 * managed sight::viz::scene2d::data::viewport object.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="viewportUpdater" type="sight::module::viz::scene2d::adaptor::viewport_updater" auto_connect="true">
       <in key="image" uid="..." />
       <inout key="viewport" uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" zValue="7" />
   </service>
   @endcode
 *
 * @subsection In In
 * - \b image [sight::data::image] (optional): scales the viewport range with the values from an image. If provided,
 *  this will update both the scene view and the viewport passed as argument.
 *
 * @subsection In-Out In-Out
 * - \b viewport [sight::viz::scene2d::data::viewport]: the viewport object this updater takes values from.
 * If the viewport is not initialized, it will be updated to fit the scene size.
 *
 * @subsection Configuration Configuration:
 * - \b config: contains the adaptor configuration
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *
 * This adaptor is intended to be used with a module::viz::scene2d::adaptor::viewport_range_selector adaptor.
 */
class viewport_updater : public sight::viz::scene2d::adaptor
{
public:

    SIGHT_DECLARE_SERVICE(viewport_updater, sight::viz::scene2d::adaptor);
    viewport_updater() noexcept = default;

protected:

    void configuring() override;
    void starting() override;
    void updating() override;
    void stopping() override;

    connections_t auto_connections() const override;

private:

    static constexpr std::string_view VIEWPORT_INOUT = "viewport";
    sight::data::ptr<sight::viz::scene2d::data::viewport, sight::data::access::inout> m_viewport {this, VIEWPORT_INOUT
    };
};

} // namespace sight::module::viz::scene2d::adaptor
