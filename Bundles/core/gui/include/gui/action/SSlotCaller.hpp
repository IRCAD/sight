/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_ACTION_SSLOTCALLER_HPP_
#define _GUI_ACTION_SSLOTCALLER_HPP_

#include <vector>

#include <fwCom/Slots.hpp>

#include <fwServices/IService.hpp>

#include <fwGui/IActionSrv.hpp>

#include "gui/export.hpp"

namespace gui
{
namespace action
{

/**
 * @brief   Run a slot given by HasSlot id and slot key.
 * @class   SSlotCaller.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 *
 * This action works on a ::fwData::Object. It does the action specify by the specify config.
 */

class GUI_CLASS_API SSlotCaller : public ::fwGui::IActionSrv
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SSlotCaller)(::fwGui::IActionSrv) ) ;
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    typedef std::string HasSlotIDType;
    typedef std::pair< HasSlotIDType, ::fwCom::Slots::SlotKeyType> SlotInfoType;
    typedef std::vector< SlotInfoType > SlotInfoContainerType;

    /**
    * @brief Constructor. Do nothing.
    */
    GUI_API SSlotCaller() throw();

    /**
    * @brief Destructor. Do nothing.
    */
    GUI_API virtual ~SSlotCaller() throw();

protected:

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream &_sstream ) ;

    /**
     * @brief This method run the specified slots.
     */
    GUI_API void updating()throw (fwTools::Failed);

    /**
     * @brief Do nothing.
     */
    GUI_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    /**
     * @brief This method is used to configure the service parameters: specifies which slots must be called.
     * @verbatim
       <service uid="..." type="::fwGui::IActionSrv" implementation="::gui::action::SSlotCaller" autoComChannel="no">
          <slots>
              <slot>hasSlotsId/slotKey</slot>
          </slots>
       </service>
       @endverbatim
     */
    GUI_API void configuring() throw( ::fwTools::Failed );

    GUI_API virtual void starting() throw(::fwTools::Failed);

    GUI_API virtual void stopping() throw(::fwTools::Failed);

private:
    // vector representing slots
    SlotInfoContainerType m_slotInfos;
};


} // namespace action
} // namespace gui


#endif /*_GUI_ACTION_SSLOTCALLER_HPP_*/
