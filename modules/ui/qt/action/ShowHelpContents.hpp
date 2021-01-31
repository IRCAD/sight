/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "modules/ui/qt/config.hpp"

#include <ui/base/IActionSrv.hpp>

#include <filesystem>

namespace sight::modules::ui::qt
{
namespace action
{

/**
 * @brief   This action show the help contents.
 */
class MODULE_UI_QT_CLASS_API ShowHelpContents : public ::sight::ui::base::IActionSrv
{
public:
    fwCoreServiceMacro(ShowHelpContents, sight::ui::base::IActionSrv)
    MODULE_UI_QT_API ShowHelpContents() noexcept;

    MODULE_UI_QT_API virtual ~ShowHelpContents() noexcept;

protected:

    /**
     * @brief Configuring method.
     *
     * XML configuration sample:
     * @code{.xml}
       <service impl="::modules::ui::qt::ui::action::ShowHelpContents" type="::ui::base::IActionSrv">
        <filename id=".../doc.qhc"/>
       </service>
       @endcode
     * This method is used to configure the service.
     */
    void configuring() override;

    /// Starts action
    void starting() override;

    /// Show the frame
    void updating() override;

    /// Stops action
    void stopping() override;

    MODULE_UI_QT_API void info(std::ostream& _sstream ) override;

private:
    /**
     * @brief the m_bServiceIsConfigured value is \b true
     * if the help files path is known.
     */
    bool m_bServiceIsConfigured;

    /**
     * @brief help files path.
     */
    std::filesystem::path m_fsHelpPath;
};

} // namespace action

} // namespace sight::modules::ui::qt
