/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
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

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/mesh_picker_interactor.hpp>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor performs picking queries on mouse clicks and forwards the picked data through a signal.
 *
 * See @see sight::viz::scene3d::interactor::mesh_picker_interactor to learn more about the interactions
 * provided by this service.
 *
 * @section Signals Signals
 * - \b picked(data::tools::picking_info): emitted when a picking query is successful.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::viz::scene3d::adaptor::picker">
            <config priority="0" queryMask="0xFFFFFFFF" layerOrderDependant="true" />
       </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b priority (optional, int, default=0): picking priority, higher priority interactions are performed first.
 * - \b queryMask (optional, uint32, default=0xFFFFFFFF): filters out entities with mismatching flags when picking.
 * - \b layerOrderDependant (optional, bool, default=true): define if interaction must take into account above layers.
 */
class picker final : public sight::viz::scene3d::adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(picker, sight::viz::scene3d::adaptor);

    /// Initializes the adaptor.
    picker() noexcept;

    /// Destroys the adaptor.
    ~picker() noexcept final = default;

protected:

    /// Configures the picker's query mask and priority.
    void configuring() final;

    /// Creates the interactor and adds it to the layer.
    void starting() final;

    /// Updates the service. Doesn't do anything here.
    void updating() noexcept final;

    /// Destroys the interactor and removes it from the layer.
    void stopping() final;

private:

    /// Determines the execution order of the picking interactor.
    int m_priority {2};

    /// Defines the mask used to filter out entities when picking.
    std::uint32_t m_query_mask {0xFFFFFFFF};

    /// Defines if the interaction must take into account above layers.
    bool m_layer_order_dependant {true};

    /// Contains the interactor managed by the adaptor.
    std::shared_ptr<sight::viz::scene3d::interactor::mesh_picker_interactor> m_interactor;

    /// Defines the signal sent on picking events.
    sight::viz::scene3d::interactor::mesh_picker_interactor::point_clicked_sig_t::sptr m_picked_sig;
};

} // namespace sight::module::viz::scene3d::adaptor.
