/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_UPDATER_STRANSLATE_HPP__
#define __CTRLSELECTION_UPDATER_STRANSLATE_HPP__

#include "ctrlSelection/IUpdaterSrv.hpp"
#include "ctrlSelection/config.hpp"

#include <fwData/Composite.hpp>

namespace ctrlSelection
{

namespace updater
{

/**
 * @brief   Update the composite related to this service according to the receiving composite signals and translate the
 * composite keys.
 */
class CTRLSELECTION_CLASS_API STranslate : public ::ctrlSelection::IUpdaterSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (STranslate)(::ctrlSelection::IUpdaterSrv) );

    /// Constructor.  Do nothing.
    CTRLSELECTION_API STranslate() noexcept;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~STranslate() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_ADD_OBJECTS_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_CHANGE_OBJECTS_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_REMOVE_OBJECTS_SLOT
     */
    CTRLSELECTION_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting();

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping();

    /**
     * @brief Configure the services : declare the events to react.
     * @code{.xml}
         <service uid="myUpdater" impl="::ctrlSelection::updater::STranslate" type="::ctrlSelection::IUpdaterSrv" autoConnect="no"  >
            <source>compositeSrcUid</source>
            <translate fromKey="myObject1Key1" toKey="myObject1Key2" />
            <translate fromKey="myObject2Key1" toKey="myObject2Key2" />
        </service>
       @endcode
     * - \b source (optional): composite used to translate objects. If it is defined, the service's slot saddObjects,
     *      changeObjects, removeObjects are connected to composite signal,  and the objects are translated on start if
     *      they are present.
     * - \b translate: objects to translate from source composite to current composite
     *   - \b fromKey: key of the object in the source composite
     *   - \b toKey: key of the object in the current composite
     */
    CTRLSELECTION_API virtual void configuring();

    /// Implements reconfiguring method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void reconfiguring();

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating();

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream& _sstream );

private:

    /// Slot: add objects
    void addObjects(::fwData::Composite::ContainerType objects);

    /// Slot: change objects
    void changeObjects(::fwData::Composite::ContainerType newObjects, ::fwData::Composite::ContainerType oldObjects);

    /// Slot: remove objects
    void removeObjects(::fwData::Composite::ContainerType objects);

    /// Managed translation : fromUID, fromKey, toKey
    typedef std::map <std::string, std::string > ManagedTranslations;
    /// List of the managed translations
    ManagedTranslations m_managedTranslations;

    /// FwID of the source composite.
    std::string m_sourceCompositeID;

    /// Source composite (can be current composite)
    ::fwData::Composite::wptr m_source;

    /// Connection to source
    ::fwCom::helper::SigSlotConnection m_connections;
};

} // updater
} // ctrlSelection

#endif // __CTRLSELECTION_UPDATER_STRANSLATE_HPP__
