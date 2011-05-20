/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_ACTIONNOTIFYSERVICE_HPP_
#define _GUI_ACTION_ACTIONNOTIFYSERVICE_HPP_

#include <fwServices/IService.hpp>

#include <fwGui/IActionSrv.hpp>

#include "gui/export.hpp"


namespace gui
{

namespace action
{

class GUI_CLASS_API ActionNotifyService : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ActionNotifyService)(::fwGui::IActionSrv) ) ;

    /**
    * @brief Constructor. Do nothing.
    */
    GUI_API ActionNotifyService() throw();

    /**
    * @brief Destructor. Do nothing.
    */
    GUI_API virtual ~ActionNotifyService() throw();

protected:

    GUI_API virtual void info(std::ostream &_sstream ) ;

    GUI_API void starting() throw(::fwTools::Failed);

    GUI_API void stopping() throw(::fwTools::Failed);

    GUI_API void updating()throw (fwTools::Failed);

    GUI_API void updating(::fwServices::ObjectMsg::csptr _msg )throw (fwTools::Failed);

    /**
     * @brief Configure the message to send.
     *
     * Example of configuration :
     * @verbatim
        <service ... >
            <notify type="::fwServices::ObjectMsg" event="APPLY" />
        </service>
       @endverbatim
     * - <notify> :
     *   - \b type : the type of the message (i.e. ::fwServices::ObjectMsg)
     *   - \b event : the event of the message (i.e. "APPLY")
     */
    GUI_API void configuring() throw( ::fwTools::Failed );

    /// Message container [(MessageType, Event)]
    typedef ::boost::tuple< std::string, std::string > MsgEventType;

    /// Notify message vector
    typedef std::vector < MsgEventType > MsgVectType;

    MsgVectType  m_vectMsg;
};


} // namespace action
} // namespace gui


#endif // _GUI_ACTION_ACTIONNOTIFYSERVICE_HPP_
