/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_COMMON_CONTROLLER_SPROGRESSBARCONTROLLER_HPP__
#define __IODICOMEXT_COMMON_CONTROLLER_SPROGRESSBARCONTROLLER_HPP__

#include <boost/filesystem/path.hpp>

#include <fwServices/IController.hpp>
#include <fwGuiQt/dialog/ProgressDialog.hpp>
#include <fwCom/Slot.hpp>

#include "ioDicomExt/common/data/ProgressMsg.hpp"
#include "ioDicomExt/config.hpp"

namespace ioDicomExt
{

namespace common
{

namespace controller
{

/**
 * @brief   This editor service is used to display a progress bar
 * @class   SProgressBarController
 * @date    2013.
 */
class IODICOMEXT_CLASS_API SProgressBarController : public ::fwServices::IController
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SProgressBarController)( ::fwServices::IController ) ) ;

    IODICOMEXT_API static const ::fwCom::Slots::SlotKeyType s_PROGRESS_SLOT;
    typedef ::fwCom::Slot<void(::ioDicomExt::common::data::ProgressMsg::sptr)> ProgressBarSlotType;

    /// Type of progress bar container
    typedef std::map< std::string, ::fwGuiQt::dialog::ProgressDialog::sptr > ProgressDialogContainerType;

    /**
     * @brief constructor
     */
    IODICOMEXT_API SProgressBarController() throw();

    /**
     * @brief destructor
     */
    IODICOMEXT_API virtual ~SProgressBarController() throw();

    /**
     * @brief Update progress bar slot
     * @param[in] progressEvent Progress Event
     */
    IODICOMEXT_API void progressBar(::ioDicomExt::common::data::ProgressMsg::sptr progressEvent);

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
     @verbatim
     <service uid="progressBarController" impl="::ioDicomExt::common::controller::SProgressBarController"
         autoConnect="no">
     </service>
     @endverbatim
    */
    IODICOMEXT_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void updating() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void info(std::ostream &_sstream ) ;

    /// Slot to call progressBar method
    ProgressBarSlotType::sptr m_slotProgressBar;

    /// Progress Dialog
    ProgressDialogContainerType m_progressDialogs;


};

} // namespace controller
} // namespace common
} // namespace ioDicomExt

#endif // __IODICOMEXT_COMMON_CONTROLLER_SPROGRESSBARCONTROLLER_HPP__
