/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_PUSHOBJECTSRV_HPP_
#define _GUI_ACTION_PUSHOBJECTSRV_HPP_

#include <set>
#include <map>

#include <fwTools/Failed.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwGui/IActionSrv.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace action
{

/**
 * @class   PushObjectSrv
 * @brief   To add or remove object in composite with specific key.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GUI_CLASS_API PushObjectSrv : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (PushObjectSrv)(::fwGui::IActionSrv) ) ;

protected:

    /// Constructor. Do nothing.
    GUI_API PushObjectSrv() throw() ;

    /// Destructor. Do nothing.
    GUI_API virtual ~PushObjectSrv() throw() ;

    virtual void starting() throw(::fwTools::Failed);

    virtual void stopping() throw(::fwTools::Failed);

    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);


    /**
     * Call the IAction::configuring()
     *
     * Example of this service configuration
     * @verbatim
       <service implementation="::gui::action::PushObjectSrv" type="::fwGui::IActionSrv">
           <push src="COMPOSITE_UID[KEY]" key="KEY_IN_COMPOSITE" />
       </service>
        @endverbatim
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

private:

    // [dest_map]->[(dest_key1,dest_key2,...)]
    typedef std::map< std::string, std::set< std::string > > SrcKeyMapType;
    // [src_key]->[<dest_map, dest_key>]
    typedef std::map< std::string, std::pair< std::string, std::string > > DestKeyMapType;
    /**
     * @brief keep the association between associated key and source object
     */
    DestKeyMapType m_key2src;
    SrcKeyMapType  m_srcMap;
};

} //action
} // GUI


#endif // _GUI_ACTION_PUSHOBJECTSRV_HPP_


