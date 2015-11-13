/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ACTION_PUSHOBJECTSRV_HPP__
#define __GUI_ACTION_PUSHOBJECTSRV_HPP__

#include "gui/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwTools/Failed.hpp>

#include <set>
#include <map>

namespace gui
{
namespace action
{

/**
 * @class   PushObjectSrv
 * @brief   To add or remove object in composite with specific key.
 *
 * @date    2011.
 */
class GUI_CLASS_API PushObjectSrv : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (PushObjectSrv)(::fwGui::IActionSrv) );

    /// Constructor. Do nothing.
    GUI_API PushObjectSrv() throw();

    /// Destructor. Do nothing.
    GUI_API virtual ~PushObjectSrv() throw();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_UPDATE_OBJECTS_SLOT
     */
    GUI_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:


    virtual void starting() throw(::fwTools::Failed);

    virtual void stopping() throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);


    /**
     * Call the IAction::configuring()
     *
     * Example of this service configuration
     * @verbatim
       <service impl="::gui::action::PushObjectSrv" type="::fwGui::IActionSrv">
           <push src="COMPOSITE_UID[KEY]" key="KEY_IN_COMPOSITE" />
       </service>
        @endverbatim
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );

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
};

} //action
} // GUI


#endif // __GUI_ACTION_PUSHOBJECTSRV_HPP__


