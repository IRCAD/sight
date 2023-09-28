/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include <ui/__/dialog/location_base.hpp>

#include <QObject>
#include <QString>
#include <QUrl>

namespace sight::ui::qml::dialog
{

/**
 * @brief   Defines the generic file/folder dialog for IHM.
 *
 */
class UI_QML_CLASS_API location : public QObject,
                                  public ui::dialog::location_base
{
Q_OBJECT
Q_PROPERTY(QString filterSelected MEMBER m_filterSelected)

public:

    SIGHT_DECLARE_CLASS(location, ui::dialog::location_base, ui::factory::make<location>);

    /// show the locationDialog to the user and wait selection from the user
    UI_QML_API core::location::base::sptr show() override;

    /// Set the type of locationDialog to open (multi selection, folder selection...)
    UI_QML_API void setType(location::Types type) override;

    /// Set the type of locationDialog to open (read, write...)
    UI_QML_API void setOption(location::Options option) override;

    /// Set the extension of locationDialog to open example: addFilter("images","*.png *.jpg");
    UI_QML_API void addFilter(const std::string& filterName, const std::string& wildcardList) override;

protected Q_SLOTS:

    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(const QVariant& msg);

private:

    location::Options m_style {location::NONE};
    location::Types m_type {location::SINGLE_FILE};
    std::vector<std::pair<std::string, std::string> > m_filters;

    /// helper to transform m_filters into qml encoding ("BMP and GIF files (*.bmp *.gif);;PNG files (*.png)"
    QStringList fileFilters();

    std::string m_wildcard;
    core::location::base::sptr m_location;

    /// the filter list and the current filter selected
    QString m_filterSelected;

    std::string getCurrentSelection() const override;

    /// event filter for Mac
    bool eventFilter(QObject* watched, QEvent* event) override;
};

} // namespace sight::ui::qml::dialog
