/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "uiGenericQt/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <boost/filesystem/path.hpp>

#include <QObject>
#include <QSize>
#include <QUrl>

namespace uiGenericQt
{

namespace action
{

/**
 * @brief   This action show the about frame.
 */
class UIGENERICQT_CLASS_API SShowAbout : public QObject,
                                         public ::fwGui::IActionSrv
{

Q_OBJECT
public:
    fwCoreServiceMacro(SShowAbout,  ::fwGui::IActionSrv);

    /**
     * @name Constructor/Destructor
     * @{ */
    UIGENERICQT_API SShowAbout() noexcept;
    UIGENERICQT_API virtual ~SShowAbout() noexcept;
    /**  @} */

protected:
    /**
     * @brief Configuring method.
     *
     * XML configuration sample:
       @code{.xml}
       <service impl="::uiGenericQt::action::SShowAbout" type="::fwGui::IActionSrv">
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
    ::boost::filesystem::path m_fsAboutPath;

    /// Frame title (default value set to "About").
    std::string m_title;

    /// Frame size.
    QSize m_size;
};

} // namespace action

} // namespace uiGenericQt
