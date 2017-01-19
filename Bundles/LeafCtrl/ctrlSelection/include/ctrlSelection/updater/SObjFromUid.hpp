/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_UPDATER_SOBJFROMUID_HPP__
#define __CTRLSELECTION_UPDATER_SOBJFROMUID_HPP__

#include "ctrlSelection/IUpdaterSrv.hpp"
#include "ctrlSelection/config.hpp"

#include <fwData/Object.hpp>

namespace ctrlSelection
{

namespace updater
{

/**
 * @brief Updates the composite from object given by uid. The objects is added/swapped/removed from composite when the
 * respective slot is called.
 *
 * @section Slots Slots
 * - \b add() : Adds the object into the composite with the key given by config.
 * - \b addOrSwap() : Adds the object if it is not present in the composite, else swaps it.
 * - \b swapObj() : Swaps the object into the composite with the key given by config.
 * - \b remove() : Removes the object from the composite at the key given by config.
 * - \b removeIfPresent() : Removes the object from the composite at the key given by config if it is present.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ctrlSelection::updater::SObjFromUid">
      <inout key="composite" uid="..." />
      <inout key="object" uid="..." />
      <compositeKey>...</compositeKey>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b composite [::fwData::Composite]: Composite where to add/swap/remove objects.
 * - \b object [::fwData::Object]: object to add/swap/remove into the composite.
 * @subsection Configuration Configuration
 * - \b compositeKey : key of the object in the composite
 */
class CTRLSELECTION_CLASS_API SObjFromUid : public ::ctrlSelection::IUpdaterSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SObjFromUid)(::ctrlSelection::IUpdaterSrv) );

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SObjFromUid() throw();

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SObjFromUid() throw();

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

    /// Configures the service.
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream& _sstream );

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
    /**
     * @}
     */

private:

    std::string m_objectUid; ///< uid of the object
    std::string m_compositeKey; ///< key of the object to manage in the composite

};

} // updater
} // ctrlSelection

#endif // __CTRLSELECTION_UPDATER_SOBJFROMUID_HPP__
