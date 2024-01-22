/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <ui/__/action.hpp>

#include <QObject>
#include <QSize>
#include <QUrl>

#include <filesystem>

namespace sight::module::ui::qt
{

/**
 * @brief   This action show the about frame.
 */
class MODULE_UI_QT_CLASS_API show_about : public QObject,
                                          public sight::ui::action
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(show_about, sight::ui::action);

    /**
     * @name Constructor/Destructor
     * @{ */
    MODULE_UI_QT_API show_about() noexcept;
    MODULE_UI_QT_API ~show_about() noexcept override;
/**  @} */

protected:

    /**
     * @brief Configuring method.
     *
     * XML configuration sample:
       @code{.xml}
       <service type="sight::module::ui::qt::show_about">
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
     * @param _url clicked URL
     */
    static void on_url_clicked(const QUrl& _url);

private:

    /// Set to 'true' if the about file path is known.
    bool m_b_service_is_configured {false};

    /**
     * @brief about file path.
     */
    std::filesystem::path m_fs_about_path;

    /// Frame title (default value set to "About").
    std::string m_title;

    /// Frame size.
    QSize m_size;
};

} // namespace sight::module::ui::qt
