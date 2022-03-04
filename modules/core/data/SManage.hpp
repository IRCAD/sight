/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/data/config.hpp"

#include <data/Object.hpp>

#include <service/IController.hpp>

namespace sight::module::data
{

/**
 * @brief This service manages a contained object (add/swap/remove) into a container object (composite, vector,
 * seriesDB).
 *
 * It works on different objects:
 * - data::Composite: the object is added/swapped/removed from composite at the given key
 * - data::Vector: the object is added or removed from the container
 * - data::SeriesDB: the object is added or removed from the container
 * - data::Object: the object is added or removed from the field map at the given key
 *
 * @section Slots Slots
 * - \b add() : Adds the object into the target (vector, seriesDB, composite), if target is a Composite or a Field, it
 *  is added at the key given by config.
 * - \b addCopy() : Adds a copy of the object into the target (vector, seriesDB, composite), if target is a Composite
 * or a Field, it is added at the key given by config.
 * - \b addOrSwap() : Adds the object if it is not present in the target, else if target is a composite or a Field, the
 * object is swapped.
 * - \b swapObj() : Only if target is a Composite or a Field : swaps the object into the composite with the key given by
 * config.
 * - \b remove() : Removes the object.
 * - \b removeIfPresent() : Removes the object if it is present.
 * - \b clear() : Removes all objects.
 *
 * @section XML XML Configuration
 *
 * For data::Composite:
 * @code{.xml}
   <service type="sight::module::data::SManage">
      <inout key="object" uid="..." />
      <inout key="container" uid="..." />
      <compositeKey>...</compositeKey>
   </service>
   @endcode
 *
 * For data::Vector:
 * @code{.xml}
   <service type="sight::module::data::SManage">
      <inout key="object" uid="..." />
      <inout key="container" uid="..." />
   </service>
   @endcode
 *
 * For data::SeriesDB:
 * @code{.xml}
   <service type="sight::module::data::SManage">
      <inout key="object" uid="..." />
      <inout key="container" uid="..." />
   </service>
   @endcode
 *
 * For data::Object:
 * @code{.xml}
   <service type="sight::module::data::SManage">
      <inout key="object" uid="..." />
      <inout key="container" uid="..." />
      <field>...</field>
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b object [sight::data::Object] (optional): object to add/swap/remove. Not needed when invoking clean slot and
 * remove
 * slots with composites and fields, since the removal is based on the name.
 * - \b container [sight::data::Object]: Composite/Vector/Series where to add/swap/remove object, or where
 * to add/swap/remove object as a field.
 *
 * @subsection Configuration Configuration
 * - \b compositeKey (optional, only used if the target object in a Composite) : key of the object in the composite
 * - \b field (optional) : name of the field
 */
class MODULE_DATA_CLASS_API SManage : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SManage, sight::service::IController);

    /// Constructor.  Do nothing.
    MODULE_DATA_API SManage() noexcept;

    /// Destructor. Do nothing.
    MODULE_DATA_API virtual ~SManage() noexcept;

    /**
     * @name Slots
     * @{
     */
    static const core::com::Slots::SlotKeyType s_ADD_SLOT;
    static const core::com::Slots::SlotKeyType s_ADD_COPY_SLOT;
    static const core::com::Slots::SlotKeyType s_ADD_OR_SWAP_SLOT;
    static const core::com::Slots::SlotKeyType s_SWAP_OBJ_SLOT;
    static const core::com::Slots::SlotKeyType s_REMOVE_SLOT;
    static const core::com::Slots::SlotKeyType s_REMOVE_IF_PRESENT_SLOT;
    static const core::com::Slots::SlotKeyType s_CLEAR_SLOT;
/**
 * @}
 */

protected:

    /// Configures the service.
    MODULE_DATA_API void configuring() override;

    /// Implements starting method derived from IService. Do nothing.
    MODULE_DATA_API void starting() override;

    /// Implements stopping method derived from IService. Do nothing.
    MODULE_DATA_API void stopping() override;

    /// Implements updating method derived from IService. Do nothing.
    MODULE_DATA_API void updating() override;

    /**
     * @name Slots
     * @{
     */
    /// Adds the object into the composite with the key given by config.
    void add();

    /// Adds a copy of object into the composite with the key given by config.
    void addCopy();

    /**
     * @brief Adds or swap the object into the composite with the key given by config.
     *
     * Adds the object if it is not present in the composite, else swaps it.
     */
    void addOrSwap();

    /// Swaps the object into the composite with the key given by config.
    void swap();

    /// Removes the object from the composite at the key given by config.
    void remove();

    /// Removes the object from the composite at the key given by config if it is present.
    void removeIfPresent();

    /// Removes all objects.
    void clear();
/**
 * @}
 */

private:

    void internalAdd(bool _copy);

    std::string m_objectUid;    ///< uid of the object
    std::string m_compositeKey; ///< key of the object to manage in the composite
    std::string m_fieldName;    ///< name of the field to manage in the object

    sight::data::ptr<sight::data::Object, sight::data::Access::inout> m_object {this, "object", false, true};
    sight::data::ptr<sight::data::Object, sight::data::Access::inout> m_container {this, "container"};
};

} // sight::module::data
