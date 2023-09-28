/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/config.hpp"

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/MeshPickerInteractor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor performs picking queries on mouse clicks and forwards the picked data through a signal.
 *
 * See @see sight::viz::scene3d::interactor::MeshPickerInteractor to learn more about the interactions
 * provided by this service.
 *
 * @section Signals Signals
 * - \b picked(data::tools::PickingInfo): emitted when a picking query is successful.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::viz::scene3d::adaptor::SPicker">
            <config priority="0" queryMask="0xFFFFFFFF" layerOrderDependant="true" />
       </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b priority (optional, int, default=0): picking priority, higher priority interactions are performed first.
 * - \b queryMask (optional, uint32, default=0xFFFFFFFF): filters out entities with mismatching flags when picking.
 * - \b layerOrderDependant (optional, bool, default=true): define if interaction must take into account above layers.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SPicker final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SPicker, sight::viz::scene3d::adaptor);

    /// Initializes the adaptor.
    MODULE_VIZ_SCENE3D_API SPicker() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SPicker() noexcept final = default;

protected:

    /// Configures the picker's query mask and priority.
    MODULE_VIZ_SCENE3D_API void configuring() final;

    /// Creates the interactor and adds it to the layer.
    MODULE_VIZ_SCENE3D_API void starting() final;

    /// Updates the service. Doesn't do anything here.
    MODULE_VIZ_SCENE3D_API void updating() noexcept final;

    /// Destroys the interactor and removes it from the layer.
    MODULE_VIZ_SCENE3D_API void stopping() final;

private:

    /// Determines the execution order of the picking interactor.
    int m_priority {2};

    /// Defines the mask used to filter out entities when picking.
    std::uint32_t m_queryMask {0xFFFFFFFF};

    /// Defines if the interaction must take into account above layers.
    bool m_layerOrderDependant {true};

    /// Contains the interactor managed by the adaptor.
    std::shared_ptr<sight::viz::scene3d::interactor::MeshPickerInteractor> m_interactor;

    /// Defines the signal sent on picking events.
    sight::viz::scene3d::interactor::MeshPickerInteractor::PointClickedSigType::sptr m_pickedSig;
};

} // namespace sight::module::viz::scene3d::adaptor.
