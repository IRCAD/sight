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

#include <ui/base/IAction.hpp>

#include <QObject>
#include <QSize>
#include <QUrl>

#include <filesystem>

namespace sight::module::ui::qt
{

/**
 * @brief   This action show the about frame.
 */
class MODULE_UI_QT_CLASS_API SShowAbout : public QObject,
                                          public ::sight::ui::base::IAction
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SShowAbout, sight::ui::base::IAction);

    /**
     * @name Constructor/Destructor
     * @{ */
    MODULE_UI_QT_API SShowAbout() noexcept;
    MODULE_UI_QT_API virtual ~SShowAbout() noexcept;
/**  @} */

protected:

    /**
     * @brief Configuring method.
     *
     * XML configuration sample:
       @code{.xml}
       <service impl="::sight::module::ui::qt::SShowAbout" type="ui::base::IAction">
         <filename id=".../aboutFile.html"/>
         <title>title</title>
         <size width="200" height="200" />
       </service>
       @endcode
     * This method is used to configure the service.
     */
    void configuring() override;

    /// Starts action
    void starting() override;

    /// Shows the frame
    void updating() override;

    /// Stops action
    void stopping() override;

    /// Prints service info
    void info(std::ostream& _sstream) override;

protected Q_SLOTS:

    /**
     * @brief Triggered when an URL is clicked in the about frame.
     *
     * @param url clicked URL
     */
    void onUrlClicked(const QUrl& url);

private:

    /// Set to 'true' if the about file path is known.
    bool m_bServiceIsConfigured;

    /**
     * @brief about file path.
     */
    std::filesystem::path m_fsAboutPath;

    /// Frame title (default value set to "About").
    std::string m_title;

    /// Frame size.
    QSize m_size;
};

} // namespace sight::module::ui::qt
