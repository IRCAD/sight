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

#ifndef __UIGENERICQT_ACTION_SHOWHELPCONTENTS_HPP__
#define __UIGENERICQT_ACTION_SHOWHELPCONTENTS_HPP__

#include "uiGenericQt/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <boost/filesystem/path.hpp>

namespace uiGenericQt
{
namespace action
{

/**
 * @brief   This action show the help contents.
 */
class UIGENERICQT_CLASS_API ShowHelpContents : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (ShowHelpContents)( ::fwGui::IActionSrv) );
    UIGENERICQT_API ShowHelpContents() noexcept;

    UIGENERICQT_API virtual ~ShowHelpContents() noexcept;

protected:

    /**
     * @brief Configuring method.
     *
     * XML configuration sample:
     * @code{.xml}
       <service impl="::uiGenericQt::ui::action::ShowHelpContents" type="::fwGui::IActionSrv">
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

    UIGENERICQT_API void info(std::ostream& _sstream ) override;

private:
    /**
     * @brief the m_bServiceIsConfigured value is \b true
     * if the help files path is known.
     */
    bool m_bServiceIsConfigured;

    /**
     * @brief help files path.
     */
    ::boost::filesystem::path m_fsHelpPath;
};

} // namespace action

} // namespace uiGenericQt

#endif /*__UIGENERICQT_ACTION_SHOWHELPCONTENTS_HPP__*/
