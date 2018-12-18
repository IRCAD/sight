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

#ifndef __FWGUIQT_DIALOG_LOCATIONDIALOG_HPP__
#define __FWGUIQT_DIALOG_LOCATIONDIALOG_HPP__

#include "fwGuiQt/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/dialog/ILocationDialog.hpp>

class QString;

/**
 * @brief       The namespace fwGuiQt contains classes which provide the implementation of the Gui using Qt library.
 */
namespace fwGuiQt
{
namespace dialog
{
/**
 * @brief   Defines the generic file/folder dialog for IHM.
 *
 */
class FWGUIQT_CLASS_API LocationDialog : public ::fwGui::dialog::ILocationDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LocationDialog)(::fwGui::dialog::ILocationDialog),
                                            (()),
                                            ::fwGui::factory::New< LocationDialog > );

    FWGUIQT_API LocationDialog(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API ::fwData::location::ILocation::sptr show() override;

    FWGUIQT_API void setType( ::fwGui::dialog::ILocationDialog::Types type ) override;

    FWGUIQT_API ::fwGui::dialog::ILocationDialog& setOption( ::fwGui::dialog::ILocationDialog::Options option) override;

    // Example ( addFilter("images","*.png *.jpg");
    FWGUIQT_API void addFilter(const std::string& filterName, const std::string& wildcardList ) override;

protected:

    ::fwGui::dialog::ILocationDialog::Options m_style;
    ::fwGui::dialog::ILocationDialog::Types m_type;
    std::vector< std::pair< std::string, std::string > > m_filters;

    /// helper to transform m_filters into qt encoding ("BMP and GIF files (*.bmp *.gif);;PNG files (*.png)"
    QString fileFilters();
    /// Gets the current extension file selection
    FWGUIQT_API std::string getCurrentSelection() const override;

    std::string m_wildcard;

};
} // namespace dialog
} // namespace fwGuiQt

#endif /*__FWGUIQT_DIALOG_LOCATIONDIALOG_HPP__*/

