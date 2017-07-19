/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ACTION_SSIGNAL_HPP__
#define __GUI_ACTION_SSIGNAL_HPP__

#include "gui/config.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Slots.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwServices/IService.hpp>

#include <vector>

namespace gui
{
namespace action
{

/**
 * @brief   Simple Signal service
 *
 * Triggers a signal when the action is triggered.
 * If the action uses a confirmation, different signals are emitted depending on the confirmation result.
 *
 * @code{.xml}
       <service uid="..." type="::gui::action::SSignal" />
   @endcode
 *
 * See also ::fwGui::IActionSrv::configuring for more configuration parameters.
 * @see IActionSrv::configuring
 *
 */

class GUI_CLASS_API SSignal : public ::fwGui::IActionSrv
{

public:

    typedef std::vector< ::fwData::Object::sptr > ObjectVectorType;

    fwCoreServiceClassDefinitionsMacro ( (SSignal)(::fwGui::IActionSrv) );
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Type of triggered signal
    typedef ::fwCom::Signal< void (ObjectVectorType) > TrigerredSignalType;

    /// Type of setObject slot
    typedef ::fwCom::Slot< void ( ObjectVectorType ) >  SetObjectsSlotType;

    /**
     * @brief Constructor. Do nothing.
     */
    GUI_API SSignal() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~SSignal() noexcept;

protected:

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream& _sstream );

    /**
     * @brief This method emit a signal.
     */
    GUI_API void updating();

    /**
     * @brief Configures the service
     */
    GUI_API void configuring();

    GUI_API virtual void starting();

    GUI_API virtual void stopping();

    /**
     * @brief setObjects slot's method
     */
    virtual void setObjects( ObjectVectorType objects )
    {
        m_objects = objects;
    }

    ///Signal trigerred when action has been trigerred
    SPTR(TrigerredSignalType) m_sigTriggered;
    ///Signal trigerred when action has been cancelled
    SPTR(TrigerredSignalType) m_sigCancelled;

    /// setObject slot : save a vector of objects
    SPTR(SetObjectsSlotType) m_slotSetObjects;

    /// vector of objects passed to triggered signals
    ObjectVectorType m_objects;

};

} // namespace action
} // namespace gui


#endif /* __GUI_ACTION_SSIGNAL_HPP__ */

