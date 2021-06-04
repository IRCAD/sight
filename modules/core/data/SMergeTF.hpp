/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "modules/data/config.hpp"

#include <data/Composite.hpp>
#include <data/Point.hpp>
#include <data/TransferFunction.hpp>

#include <service/IController.hpp>

namespace sight::module::data
{

/**
 * @brief Controller that merge all TF from a composite into one TF.
 *
 * @section Slots Slots
 * - \b merge(): merge all TF from the composite into one TF.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::data::SMergeTF">
       <in key="tfPool" uid="..." />
       <inout key="tf" uid="..." />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b tfPool [sight::data::Composite]: composite where looking for TF.
 *
 * @subsection In-Out In-Out
 * - \b tf [sight::data::TransferFunction]: the merged TF.
 */
class MODULE_DATA_CLASS_API SMergeTF final : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SMergeTF, ::sight::service::IController);

    /// Creates the service.
    MODULE_DATA_API SMergeTF() noexcept;

    /// Destroys the service.
    MODULE_DATA_API virtual ~SMergeTF() noexcept final;

private:

    /// Does nothing.
    void configuring() override;

    /// Calls @ref updating().
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Object::s_MODIFIED_SIG of s_TF_POOL_INPUT to module::data::SMergeTF::s_UPDATE_SLOT.
     * Connect data::Composite::s_ADDED_OBJECTS_SIG of s_TF_POOL_INPUT to module::data::SMergeTF::s_UPDATE_SLOT.
     * Connect data::Composite::s_REMOVED_OBJECTS_SIG of s_TF_POOL_INPUT to module::data::SMergeTF::s_UPDATE_SLOT.
     */
    KeyConnectionsMap getAutoConnections() const override;

    /// Creates connection between all TF from the composite, and this service.
    void updating() override;

    /// Does nothing.
    void stopping() override;

    /// Merges all TF from the input composite to the output TF.
    void merge() const;

    /**
     * @brief Retrieves all TF colors from the value and merge them into one.
     * @param _tfPool pool where find all TF.
     * @param _value the value used to get interpolated colors in the window/level space.
     * @return The final blended color.
     */
    sight::data::TransferFunction::TFColor mergeColors(
        const sight::data::Composite::csptr _tfPool,
        sight::data::TransferFunction::TFValueType _value,
        const sight::data::TransferFunction::csptr& _already_locked_tf
    )
    const;

    /// Handles all connections between this service and all TF.
    core::com::helper::SigSlotConnection m_connections;
};

} // namespace sight::module::data.
