/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ACTION_SSLOTCALLER_HPP__
#define __GUI_ACTION_SSLOTCALLER_HPP__

#include "gui/config.hpp"

#include <fwCom/Slots.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwServices/IService.hpp>

#include <vector>

namespace gui
{
namespace action
{

/**
 * @brief   Run a slot given by HasSlot id and slot key.
 *
 * This action works on a ::fwData::Object. It does the action specify by the specify config.
 * @code{.xml}
   <service uid="..." type="::gui::action::SSlotCaller" >
      <slots>
          <slot>hasSlotsId/slotKey</slot>
      </slots>
   </service>
   @endcode
 */

class GUI_CLASS_API SSlotCaller : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SSlotCaller)(::fwGui::IActionSrv) );
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    typedef std::string HasSlotIDType;
    typedef std::pair< HasSlotIDType, ::fwCom::Slots::SlotKeyType> SlotInfoType;
    typedef std::vector< SlotInfoType > SlotInfoContainerType;

    /// Constructor. Does nothing.
    GUI_API SSlotCaller() throw();

    /// Destructor. Does nothing.
    GUI_API virtual ~SSlotCaller() throw();

protected:

    /// This method gives information about the class.
    GUI_API virtual void info(std::ostream& _sstream );

    /// This method run the specified slots.
    GUI_API void updating() throw (fwTools::Failed);

    ///  This method is used to configure the service parameters: specifies which slots must be called.
    GUI_API void configuring() throw( ::fwTools::Failed );

    GUI_API virtual void starting() throw(::fwTools::Failed);

    GUI_API virtual void stopping() throw(::fwTools::Failed);

    // Vector representing slots
    SlotInfoContainerType m_slotInfos;
};


} // namespace action
} // namespace gui


#endif /*__GUI_ACTION_SSLOTCALLER_HPP__*/
