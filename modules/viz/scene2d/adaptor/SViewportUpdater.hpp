/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/viz/scene2d/config.hpp"

#include <data/Image.hpp>

#include <viz/scene2d/data/Viewport.hpp>
#include <viz/scene2d/IAdaptor.hpp>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

/**
 * @brief Adaptor implementation that manages the camera on the view (by updating the viewport object).
 *
 * The SViewportUpdater updates the viewport of the scene it is attached to, by picking values from the
 * managed sight::viz::scene2d::data::Viewport object.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="viewportUpdater" type="sight::module::viz::scene2d::adaptor::SViewportUpdater" autoConnect="true">
       <in key="image" uid="..." />
       <inout key="viewport" uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" zValue="7" />
   </service>
   @endcode
 *
 * @subsection In In
 * - \b image [sight::data::Image] (optional): scales the viewport range with the values from an image. If provided,
 *  this will update both the scene view and the viewport passed as argument.
 *
 * @subsection In-Out In-Out
 * - \b viewport [sight::viz::scene2d::data::Viewport]: the viewport object this updater takes values from.
 * If the viewport is not initialized, it will be updated to fit the scene size.
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *
 * This adaptor is intended to be used with a module::viz::scene2d::adaptor::SViewportRangeSelector adaptor.
 */
class MODULE_VIZ_SCENE2D_CLASS_API SViewportUpdater : public sight::viz::scene2d::IAdaptor
{
public:

    SIGHT_DECLARE_SERVICE(SViewportUpdater, sight::viz::scene2d::IAdaptor);
    SViewportUpdater() noexcept = default;

protected:

    MODULE_VIZ_SCENE2D_API void configuring() override;
    MODULE_VIZ_SCENE2D_API void starting() override;
    MODULE_VIZ_SCENE2D_API void updating() override;
    MODULE_VIZ_SCENE2D_API void stopping() override;

    MODULE_VIZ_SCENE2D_API KeyConnectionsMap getAutoConnections() const override;

private:

    static constexpr std::string_view s_VIEWPORT_INOUT = "viewport";
    sight::data::ptr<sight::viz::scene2d::data::Viewport, sight::data::Access::inout> m_viewport {this, s_VIEWPORT_INOUT
    };
};

} // namespace adaptor

} // namespace sight::module::viz::scene2d
