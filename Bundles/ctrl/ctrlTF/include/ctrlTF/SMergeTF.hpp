/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
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

#include "ctrlTF/config.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Point.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwServices/IController.hpp>

namespace ctrlTF
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
   <service type="::ctrlTF::SMergeTF">
       <in key="tfPool" uid="..." />
       <inout key="tf" uid="..." />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b tfPool [::fwData::Composite]: composite where looking for TF.
 *
 * @subsection In-Out In-Out
 * - \b tf [::fwData::TransferFunction]: the merged TF.
 */
class CTRLTF_CLASS_API SMergeTF final : public ::fwServices::IController
{

public:

    fwCoreServiceMacro(::ctrlTF::SMergeTF, ::fwServices::IController)

    /// Creates the service.
    CTRLTF_API SMergeTF() noexcept;

    /// Destroys the service.
    CTRLTF_API virtual ~SMergeTF() noexcept final;

private:

    /// Does nothing.
    virtual void configuring() override;

    /// Calls @ref updating().
    virtual void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Object::s_MODIFIED_SIG of s_TF_POOL_INOUT to ::ctrlTF::adaptor::SMultipleTF::s_UPDATE_SLOT.
     * Connect ::fwData::Composite::s_ADDED_OBJECTS_SIGof s_TF_POOL_INOUT to
     * ::ctrlTF::SMergeTF::s_UPDATE_SLOT.
     * Connect ::fwData::Composite::s_REMOVED_OBJECTS_SIG of s_TF_POOL_INOUT to
     * ::ctrlTF::SMergeTF::s_UPDATE_SLOT.
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

    /// Creates connection between all TF from the composite, and this service.
    virtual void updating() override;

    /// Does nothing.
    virtual void stopping() override;

    /// Merges all TF from the input composite to the output TF.
    void merge() const;

    /**
     * @brief Retrieves all TF colors from the value and merge them into one.
     * @param _tfPool pool where find all TF.
     * @param _value the value used to get interpolated colors in the window/level space.
     * @return The final blended color.
     */
    ::fwData::TransferFunction::TFColor mergeColors(const ::fwData::Composite::csptr _tfPool,
                                                    ::fwData::TransferFunction::TFValueType _value) const;

    /// Handles all connections between this service and all TF.
    ::fwCom::helper::SigSlotConnection m_connections;

};

} // namespace ctrlTF.
