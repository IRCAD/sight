/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_SMANAGE_HPP__
#define __CTRLSELECTION_SMANAGE_HPP__

#include "ctrlSelection/config.hpp"

#include <fwData/Object.hpp>

#include <fwServices/IController.hpp>

namespace ctrlSelection
{

/**
 * @brief This service manages an object (add/swap/remove) into a container object (composite, vector, seriesDB).
 *
 * It works on different objects:
 * - ::fwData::Composite: the object is added/swapped/removed from composite at the given key when the respective slot
 *   is called.
 * - ::fwData::Vector: the object is added or removed from the container
 * - ::fwMedData::SeriesDB: the object is added or removed from the container
 *
 * @section Slots Slots
 * - \b add() : Adds the object into the target (vector, seriesDB, composite), if target is a Composite, it is add at
 *   the key given by config.
 * - \b addOrSwap() : Adds the object if it is not present in the target, else if target is a composite, the object is
 *   swapped.
 * - \b swapObj() : Only if target is a Composite : swaps the object into the composite with the key given by config.
 * - \b remove() : Removes the object.
 * - \b removeIfPresent() : Removes the object if it is present.
 * - \b clear() : Removes all objects.
 *
 * @section XML XML Configuration
 *
 * For ::fwData::Composite:
 * @code{.xml}
   <service type="::ctrlSelection::SManage">
      <inout key="object" uid="..." />
      <inout key="composite" uid="..." />
      <compositeKey>...</compositeKey>
   </service>
   @endcode
 *
 * For ::fwData::Vector:
 * @code{.xml}
   <service type="::ctrlSelection::SManage">
      <inout key="object" uid="..." />
      <inout key="vector" uid="..." />
   </service>
   @endcode
 *
 * For ::fwDMedata::SeriesDB:
 * @code{.xml}
   <service type="::ctrlSelection::SManage">
      <inout key="object" uid="..." />
      <inout key="seriesDB" uid="..." />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b object [::fwData::Object]: object to add/swap/remove. Not needed when invoking clean slot.
 * - \b composite [::fwData::Composite] (optional): Composite where to add/swap/remove object.
 * - \b vector [::fwData::Vector] (optional): Vector where to add/remove object.
 * - \b seriesDB [::fwMedData::SeriesDB] (optional): SeriesDB where to add/remove object.
 *
 * <b>Only one of the target (composite, vector or seriesDB) is allowed.</b>
 * For SeriesDB, the object must inherit of Series
 * @subsection Configuration Configuration
 * - \b compositeKey (optional, only if target object in a Composite) : key of the object in the composite
 */
class CTRLSELECTION_CLASS_API SManage : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (SManage)(::fwServices::IController) );

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SManage() noexcept;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SManage() noexcept;

    /**
     * @name Slots
     * @{
     */
    static const ::fwCom::Slots::SlotKeyType s_ADD_OR_SWAP_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_ADD_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_SWAP_OBJ_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_REMOVE_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_REMOVE_IF_PRESENT_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_CLEAR_SLOT;
    /**
     * @}
     */

protected:

    /// Configures the service.
    CTRLSELECTION_API virtual void configuring();

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting();

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping();

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating();

    /**
     * @name Slots
     * @{
     */
    /// Adds the object into the composite with the key given by config.
    void add();

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

    std::string m_objectUid; ///< uid of the object
    std::string m_compositeKey; ///< key of the object to manage in the composite

};

} // ctrlSelection

#endif // __CTRLSELECTION_SMANAGE_HPP__
