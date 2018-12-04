/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __IOPACS_SPROGRESSBARCONTROLLER_HPP__
#define __IOPACS_SPROGRESSBARCONTROLLER_HPP__

#include "ioPacs/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwCore/mt/types.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwServices/IController.hpp>

#include <boost/filesystem/path.hpp>

namespace ioPacs
{

/**
 * @brief   This editor service is used to display a progress bar
 */
class IOPACS_CLASS_API SProgressBarController : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (SProgressBarController)( ::fwServices::IController ) );

    /// Type of progress bar container
    typedef std::map< std::string, ::fwGui::dialog::ProgressDialog::sptr > ProgressDialogContainerType;

    /**
     * @brief constructor
     */
    IOPACS_API SProgressBarController() noexcept;

    /**
     * @brief destructor
     */
    IOPACS_API virtual ~SProgressBarController() noexcept;

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @code{.xml}
       <service uid="progressBarController" impl="::ioPacs::SProgressBarController"
         autoConnect="no">
       </service>
       @endcode
     */
    IOPACS_API virtual void configuring() override;

    /// Override
    IOPACS_API virtual void starting() override;

    /// Override
    IOPACS_API virtual void stopping() override;

    /// Override
    IOPACS_API void updating() override;

    /// Override
    IOPACS_API void info(std::ostream& _sstream ) override;

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

    ::fwCore::mt::Mutex m_mutex; ///< mutex for progress dialog access

};

} // namespace ioPacs

#endif // __IOPACS_SPROGRESSBARCONTROLLER_HPP__
