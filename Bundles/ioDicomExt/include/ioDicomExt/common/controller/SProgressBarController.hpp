/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_COMMON_CONTROLLER_SPROGRESSBARCONTROLLER_HPP__
#define __IODICOMEXT_COMMON_CONTROLLER_SPROGRESSBARCONTROLLER_HPP__

#include <boost/filesystem/path.hpp>

#include <fwServices/IController.hpp>
#include <fwGuiQt/dialog/ProgressDialog.hpp>
#include <fwCom/Slot.hpp>

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

public:

    fwCoreServiceClassDefinitionsMacro ( (SProgressBarController)( ::fwServices::IController ) );

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
    IODICOMEXT_API void info(std::ostream &_sstream );

    /**
     * @name Slots
     * @{
     */
    void startProgress(std::string id);

    void updateProgress(std::string id, float percentage, std::string message);

    void stopProgress(std::string id);
    /**
     * @}
     */

    /// Progress Dialog
    ProgressDialogContainerType m_progressDialogs;


};

} // namespace controller
} // namespace common
} // namespace ioDicomExt

#endif // __IODICOMEXT_COMMON_CONTROLLER_SPROGRESSBARCONTROLLER_HPP__
