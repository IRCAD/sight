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

#include <viz/scene2d/IAdaptor.hpp>

namespace sight::module::viz::scene2d::adaptor
{

/**
 * @brief Adaptor implementation that manages the camera on the view.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="interactor" type="sight::module::viz::scene2d::adaptor::SViewportInteractor">
   </service>
   @endcode
 *
 */
class MODULE_VIZ_SCENE2D_CLASS_API SViewportInteractor : public sight::viz::scene2d::IAdaptor
{
public:

    SIGHT_DECLARE_SERVICE(SViewportInteractor, sight::viz::scene2d::IAdaptor);

    MODULE_VIZ_SCENE2D_API SViewportInteractor() noexcept;

    MODULE_VIZ_SCENE2D_API ~SViewportInteractor() noexcept override;

protected:

    MODULE_VIZ_SCENE2D_API void configuring() override;

    MODULE_VIZ_SCENE2D_API void starting() override;

    MODULE_VIZ_SCENE2D_API void updating() override;

    MODULE_VIZ_SCENE2D_API void stopping() override;

    MODULE_VIZ_SCENE2D_API void processInteraction(sight::viz::scene2d::data::Event& _event) override;

    MODULE_VIZ_SCENE2D_API void zoom(bool zoomIn);

private:

    bool m_viewportIsTranslated {};
    sight::viz::scene2d::vec2d_t m_lastCoordEvent {};

    sight::data::ptr<sight::viz::scene2d::data::Viewport, sight::data::Access::inout> m_viewport {this, "viewport"};
};

} // namespace sight::module::viz::scene2d::adaptor
