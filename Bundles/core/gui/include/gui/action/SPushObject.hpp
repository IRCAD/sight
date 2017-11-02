/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ACTION_SPUSHOBJECT_HPP__
#define __GUI_ACTION_SPUSHOBJECT_HPP__

#include "gui/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwTools/Failed.hpp>

#include <map>
#include <set>

namespace gui
{
namespace action
{

/**
 * @brief   To add or remove an object in composite with specific key.
 *
 * @section XML Configuration
 *
 * @code{.xml}
       <service impl="::gui::action::SPushObject">
           <inout key="source" uid="compositeId" />
           <out key="destination" uid="objectId" />
           <push srcKey="item" />
       </service>
   @endcode
 */
class GUI_CLASS_API SPushObject : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SPushObject)(::fwGui::IActionSrv) );

    /// Constructor. Do nothing.
    GUI_API SPushObject() noexcept;

    /// Destructor. Do nothing.
    GUI_API virtual ~SPushObject() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect source Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     * Connect source Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     */
    GUI_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Configure the service.
    virtual void configuring() override;

    /// Register the action and check if the action is executable.
    virtual void starting() override;

    /// Unregister the action.
    virtual void stopping() override;

    /// Extract the object.
    virtual void updating() override;

private:

    /**
     * @brief Slot: called when source composite objects are updated, enabled/disabled the action if the defined objects
     * are present/not present.
     */
    void updateObjects();

    // [src_map]->[(src_key1, src_key2, ...)]
    typedef std::map< std::string, std::set< std::string > > SrcKeyMapType;
    // [dest_key]->[<src_map, src_key>]
    typedef std::map< std::string, std::pair< std::string, std::string > > DestKeyMapType;
    /**
     * @brief keep the association between associated key and source object
     */
    DestKeyMapType m_key2src;
    SrcKeyMapType m_srcMap;

    /// Key in the source composite to extract
    std::string m_srcKey;
};

} // namespace action

} // namespace gui

#endif // __GUI_ACTION_SPUSHOBJECT_HPP__

