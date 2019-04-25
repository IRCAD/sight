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

#include <QObject>
#include <QString>
#include <QUrl>

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
class FWGUIQML_CLASS_API LocationDialog : public QObject,
                                          public ::fwGui::dialog::ILocationDialog
{
Q_OBJECT
Q_PROPERTY(bool existing MEMBER m_existing WRITE emitExisting NOTIFY existingChanged)
Q_PROPERTY(QStringList filter MEMBER m_filter WRITE emitFilter NOTIFY filterChanged)
Q_PROPERTY(QString filterSelected MEMBER m_filterSelected)
Q_PROPERTY(QUrl folder MEMBER m_folder WRITE emitFolder NOTIFY folderChanged)
Q_PROPERTY(bool isFolder MEMBER m_isFolder WRITE emitIsFolder NOTIFY isFolderChanged)
Q_PROPERTY(bool multiple MEMBER m_multiple WRITE emitMultiple NOTIFY multipleChanged)
Q_PROPERTY(QString title MEMBER m_title WRITE emitTitle NOTIFY titleChanged)
Q_PROPERTY(bool visible MEMBER m_visible)

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
Q_SIGNALS:
    /// notify the qml of property change
    void existingChanged();
    void filterChanged();
    void folderChanged();
    void isFolderChanged();
    void multipleChanged();
    void titleChanged();
protected:

    ::fwGui::dialog::ILocationDialog::Options m_style;
    ::fwGui::dialog::ILocationDialog::Types m_type;
    std::vector< std::pair< std::string, std::string > > m_filters;

    /// helper to transform m_filters into qml encoding ("BMP and GIF files (*.bmp *.gif);;PNG files (*.png)"
    QStringList fileFilters();

    /// Gets the current extension file selection
    FWGUIQML_API std::string getCurrentSelection() const override;

    std::string m_wildcard;
    ::fwData::location::ILocation::sptr m_location;

    /// option to save a file enable
    bool m_existing;
    /// the filter list and the current filter selected
    QStringList m_filter;
    QString m_filterSelected;
    /// the path of the location at the beginning
    QUrl m_folder;
    /// option to select folder enable
    bool m_isFolder;
    /// option to select multiple file enable
    bool m_multiple;

    /// title
    QString m_title;

    /// boolean to check if dialog closed
    bool m_isFinish;
    bool m_visible;

    /// Setter to QProperty and emit signal
    FWGUIQML_API void emitExisting(const bool&);
    FWGUIQML_API void emitFilter(const QStringList&);
    FWGUIQML_API void emitFolder(const QUrl&);
    FWGUIQML_API void emitIsFolder(const bool&);
    FWGUIQML_API void emitMultiple(const bool&);
    FWGUIQML_API void emitTitle(const QString&);

protected Q_SLOTS:
    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(const QVariant& msg);
};
} // namespace dialog
} // namespace fwGuiQml
