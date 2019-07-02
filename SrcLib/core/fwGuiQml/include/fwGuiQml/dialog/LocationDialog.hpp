/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include <QObject>
#include <QString>
#include <QUrl>

namespace fwGuiQml
{
namespace dialog
{
/**
 * @brief   Defines the generic file/folder dialog for IHM.
 *
 */
class FWGUIQML_CLASS_API LocationDialog : public QObject,
                                          public ::fwGui::dialog::ILocationDialog
{
Q_OBJECT
Q_PROPERTY(QString filterSelected MEMBER m_filterSelected)

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LocationDialog)(::fwGui::dialog::ILocationDialog),
                                            (()),
                                            ::fwGui::factory::New< LocationDialog > );

    FWGUIQML_API LocationDialog(::fwGui::GuiBaseObject::Key key);

    /// show the locationDialog to the user and wait selection from the user
    FWGUIQML_API ::fwData::location::ILocation::sptr show() override;

    /// Set the type of locationDialog to open (multi selection, folder selection...)
    FWGUIQML_API void setType( ::fwGui::dialog::ILocationDialog::Types type ) override;

    /// Set the type of locationDialog to open (read, write...)
    FWGUIQML_API ::fwGui::dialog::ILocationDialog& setOption(
        ::fwGui::dialog::ILocationDialog::Options option) override;

    /// Set the extension of locationDialog to open example: addFilter("images","*.png *.jpg");
    FWGUIQML_API void addFilter(const std::string& filterName, const std::string& wildcardList ) override;
protected Q_SLOTS:
    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(const QVariant& msg);

private:

    ::fwGui::dialog::ILocationDialog::Options m_style {::fwGui::dialog::ILocationDialog::NONE};
    ::fwGui::dialog::ILocationDialog::Types m_type {::fwGui::dialog::ILocationDialog::SINGLE_FILE};
    std::vector< std::pair< std::string, std::string > > m_filters;

    /// helper to transform m_filters into qml encoding ("BMP and GIF files (*.bmp *.gif);;PNG files (*.png)"
    const QStringList fileFilters();

    std::string m_wildcard;
    ::fwData::location::ILocation::sptr m_location;

    /// the filter list and the current filter selected
    QString m_filterSelected;

    std::string getCurrentSelection() const override;

    /// event filter for Mac
    bool eventFilter(QObject* watched, QEvent* event) override;
};
} // namespace dialog
} // namespace fwGuiQml
