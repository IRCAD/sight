/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "ui/qml/config.hpp"

#include <core/base.hpp>

#include <ui/base/dialog/ILocationDialog.hpp>

#include <QObject>
#include <QString>
#include <QUrl>

namespace sight::ui::qml
{

namespace dialog
{

/**
 * @brief   Defines the generic file/folder dialog for IHM.
 *
 */
class UI_QML_CLASS_API LocationDialog : public QObject,
                                        public ui::base::dialog::ILocationDialog
{
Q_OBJECT
Q_PROPERTY(QString filterSelected MEMBER m_filterSelected)

public:

    SIGHT_DECLARE_CLASS(LocationDialog, ui::base::dialog::ILocationDialog, ui::base::factory::New<LocationDialog>)

    UI_QML_API LocationDialog(ui::base::GuiBaseObject::Key key);

    /// show the locationDialog to the user and wait selection from the user
    UI_QML_API core::location::ILocation::sptr show() override;

    /// Set the type of locationDialog to open (multi selection, folder selection...)
    UI_QML_API void setType(ui::base::dialog::ILocationDialog::Types type) override;

    /// Set the type of locationDialog to open (read, write...)
    UI_QML_API ui::base::dialog::ILocationDialog& setOption(
        ui::base::dialog::ILocationDialog::Options option
    ) override;

    /// Set the extension of locationDialog to open example: addFilter("images","*.png *.jpg");
    UI_QML_API void addFilter(const std::string& filterName, const std::string& wildcardList) override;

protected Q_SLOTS:

    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(const QVariant& msg);

private:

    ui::base::dialog::ILocationDialog::Options m_style {ui::base::dialog::ILocationDialog::NONE};
    ui::base::dialog::ILocationDialog::Types m_type {ui::base::dialog::ILocationDialog::SINGLE_FILE};
    std::vector<std::pair<std::string, std::string> > m_filters;

    /// helper to transform m_filters into qml encoding ("BMP and GIF files (*.bmp *.gif);;PNG files (*.png)"
    const QStringList fileFilters();

    std::string m_wildcard;
    core::location::ILocation::sptr m_location;

    /// the filter list and the current filter selected
    QString m_filterSelected;

    std::string getCurrentSelection() const override;

    /// event filter for Mac
    bool eventFilter(QObject* watched, QEvent* event) override;
};

} // namespace dialog

} // namespace sight::ui::qml
