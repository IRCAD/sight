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

#include "fwGuiQml/config.hpp"

#include <fwCore/base.hpp>

#include <fwGui/dialog/ILocationDialog.hpp>

class QString;

/**
 * @brief       The namespace fwGuiQml contains classes which provide the implementation of the Gui using Qml library.
 */
namespace fwGuiQml
{
namespace dialog
{
/**
 * @brief   Defines the generic file/folder dialog for IHM.
 *
 */
class FWGUIQML_CLASS_API LocationDialog : public ::fwGui::dialog::ILocationDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LocationDialog)(::fwGui::dialog::ILocationDialog),
                                            (()),
                                            ::fwGui::factory::New< LocationDialog > );

    FWGUIQML_API LocationDialog(::fwGui::GuiBaseObject::Key key);

    FWGUIQML_API ::fwData::location::ILocation::sptr show() override;

    FWGUIQML_API void setType( ::fwGui::dialog::ILocationDialog::Types type ) override;

    FWGUIQML_API ::fwGui::dialog::ILocationDialog& setOption(
        ::fwGui::dialog::ILocationDialog::Options option) override;

    // Example ( addFilter("images","*.png *.jpg");
    FWGUIQML_API void addFilter(const std::string& filterName, const std::string& wildcardList ) override;

protected:

    ::fwGui::dialog::ILocationDialog::Options m_style;
    ::fwGui::dialog::ILocationDialog::Types m_type;
    std::vector< std::pair< std::string, std::string > > m_filters;

    /// helper to transform m_filters into qt encoding ("BMP and GIF files (*.bmp *.gif);;PNG files (*.png)"
    QString fileFilters();
    /// Gets the current extension file selection
    FWGUIQML_API std::string getCurrentSelection() const override;

    std::string m_wildcard;

};
} // namespace dialog
} // namespace fwGuiQml
