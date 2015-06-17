/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIDROID_SBUTTON_HPP__
#define __UIDROID_SBUTTON_HPP__

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
 * @brief   SButton service allows to set a simple button in an android app
 * @class   SButton
 *
 * Service registered details : \n
 * fwServicesRegisterMacro(::fwServices::IService, ::SButton::SButton, ::fwData::Object)
 */
class UIDROID_CLASS_API SButton : public ::fwServices::IService
{

public:
    fwCoreServiceClassDefinitionsMacro((SButton)(::fwServices::IService));

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void () > ClickedSignalType;

    /// Key in m_signals map of signal m_sigClicked
    UIDROID_API static const ::fwCom::Signals::SignalKeyType s_CLICKED_SIG;

    ///@}

    /**
     * @brief Constructor.
     */
    UIDROID_API SButton() throw ();

    /**
     * @brief Destructor.
     */
    UIDROID_API ~SButton() throw ();

    ///nothing
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

    ///
    void onClicked();

private:

    ClickedSignalType::sptr m_sigClicked; ///< signal button clicked.
    std::string m_label; ///< button label.

    ::arGuiDroid::ViewManager* m_manager; ///< Android view manager.

};
} // namespace uiDroid

#endif // __UIDROID_SBUTTON_HPP__
