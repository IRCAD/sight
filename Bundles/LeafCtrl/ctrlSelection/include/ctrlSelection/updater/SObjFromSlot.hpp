/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_UPDATER_SOBJFROMSLOT_HPP__
#define __CTRLSELECTION_UPDATER_SOBJFROMSLOT_HPP__

#include "ctrlSelection/IUpdaterSrv.hpp"
#include "ctrlSelection/config.hpp"

namespace ctrlSelection
{

namespace updater
{

/**
 * @brief  Registers/unregisters the object given by the slots.
 *
 * @section Slots Slots
 * - \b add(::fwData::Object::sptr) : Registers the given object in the OSR
 * - \b remove(::fwData::Object::sptr) : Unregisters the object in the OSR
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="::ctrlSelection::updater::SObjFromSlot">
         <out key="object" uid="..." />
     </service>
     @endcode
 * @subsection Output Output
 * - \b object [::fwData::Object]: Object used to register/unregister the object given by the slots.
 *
 * @deprecated The slots 'addOrSwap', 'swapObj' and 'removeIfPresent' should be replaced by 'add' or 'remove'.
 */
class CTRLSELECTION_CLASS_API SObjFromSlot : public ::ctrlSelection::IUpdaterSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SObjFromSlot)(::ctrlSelection::IUpdaterSrv) );

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SObjFromSlot() noexcept;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SObjFromSlot() noexcept;

    /**
     * @name Slots
     * @{
     */
    static const ::fwCom::Slots::SlotKeyType s_ADD_OR_SWAP_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_ADD_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_SWAP_OBJ_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_REMOVE_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_REMOVE_IF_PRESENT_SLOT;
    /**
     * @}
     */

protected:

    /**
     * @brief Configures the service.
     *
     * @code{.xml}
       <service impl="::ctrlSelection::updater::SObjFromSlot">
           <compositeKey>key</compositeKey>
       </service>
       @endcode
     * - \b compositeKey key of the object to manage into the composite (add/swap/remove)
     */
    CTRLSELECTION_API virtual void configuring();

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting();

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping();

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating();

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream& _sstream );

    /**
     * @name Slots
     * @{
     */
    /// Adds the object into the composite with the key given by config.
    void add(::fwData::Object::sptr obj);

    /**
     * @brief Adds or swap the object into the composite with the key given by config.
     *
     * Adds the object if it is not present in the composite, else swaps it.
     */
    void addOrSwap(::fwData::Object::sptr obj);

    /// Swaps the object into the composite with the key given by config.
    void swap(::fwData::Object::sptr obj);

    /// Adds the object from the composite with the key given by config.
    void remove();

    /// Adds the object into the composite with the key given by config if it is present.
    void removeIfPresent();
    /**
     * @}
     */

private:

    std::string m_compositeKey; ///< key of the object to manage in the composite

    std::string m_objectUid; ///< Uid of the object to output
};

} // updater
} // ctrlSelection

#endif // __CTRLSELECTION_UPDATER_SOBJFROMSLOT_HPP__
