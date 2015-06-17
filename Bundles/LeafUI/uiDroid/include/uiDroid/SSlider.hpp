/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIDROID_SSLIDER_HPP__
#define __UIDROID_SSLIDER_HPP__

#include <arGuiDroid/ViewManager.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwData/Object.hpp>

#include <fwTools/Failed.hpp>
#include <fwServices/IService.hpp>
#include "uiDroid/config.hpp"

namespace uiDroid
{
/**
 * @brief   SSlider service allows to set a simple slider in an android app
 * @class   SSlider
 *
 * Service registered details : \n
 * fwServicesRegisterMacro(::fwServices::IService, ::SSlider::SSlider, ::fwData::Object)
 */
class UIDROID_CLASS_API SSlider : public ::fwServices::IService
{

public:
    fwCoreServiceClassDefinitionsMacro((SSlider)(::fwServices::IService));

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void (unsigned int) > ValueChangedSignalType;

    /// Key in m_signals map of signal m_sigValueChanged
    UIDROID_API static const ::fwCom::Signals::SignalKeyType s_VALUE_CHANGED_SIG;

    ///@}

    /**
     * @brief Constructor.
     */
    UIDROID_API SSlider() throw ();

    /**
     * @brief Destructor.
     */
    UIDROID_API ~SSlider() throw ();

    /**
     * @brief Configuring method: This method get slider parameters from the xml configuration.
     */
    UIDROID_API void configuring() throw (fwTools::Failed);

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    UIDROID_API void starting() throw (fwTools::Failed);

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    UIDROID_API void stopping() throw (fwTools::Failed);

    /**
     * @brief Updating method : This method is used to update the service.
     */
    UIDROID_API void updating() throw (fwTools::Failed);


protected:

    /// Slot called when Method comboBox value changed.
    void onValueChanged(int value);

private:

    ValueChangedSignalType::sptr m_sigValueChanged; ///< signal value changed.
    int m_max; ///< max value of the slider.
    int m_value; ///< default value.

    ::arGuiDroid::ViewManager* m_manager; ///< Android view manager.

};
} // namespace uiDroid

#endif // __UIDROID_SSLIDER_HPP__
