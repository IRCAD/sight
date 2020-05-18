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

#include "ioDicom/config.hpp"

#include <fwDicomIOFilter/IFilter.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QShortcut>
#include <QWidget>

namespace ioDicom
{

/**
 * @brief   This editor service is used to select and apply filters to Dicom series.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioDicom::SFilterSelectionEditor">
            <in key="selection" uid="..." />
            <inout key="target" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selection [::fwData::Vector]: selection.
 * @subsection In-Out In-Out:
 * - \b target [::fwMedData::SeriesDB]: seriesDB where to put the selection.
 */

class IODICOM_CLASS_API SFilterSelectionEditor : public QObject,
                                                 public ::fwGui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceMacro(SFilterSelectionEditor,  ::fwGui::editor::IEditor );

    typedef std::map< std::string, ::fwDicomIOFilter::IFilter::sptr > FilterMapType;

    /**
     * @brief Constructor
     */
    IODICOM_API SFilterSelectionEditor() noexcept;

    /**
     * @brief Destructor
     */
    IODICOM_API virtual ~SFilterSelectionEditor() noexcept;

private Q_SLOTS:
    /// Slot called when a new filter type is selected in the context menu
    IODICOM_API void addFilter(int filterTypeIndex);

    /// Slot called when the add filter button is pressed
    IODICOM_API void addFilterAtTheEnd();

    /// Slot called when the remove filter button is pressed
    IODICOM_API void removeFilter();

    /// Slot called when a new filter is selected
    IODICOM_API void updateButtonStatus(int filterIndex);

    /// Slot called when the configure filter button is pressed
    IODICOM_API void configureFilter();

    /// Slot called when the split filter button is pressed
    IODICOM_API void splitFilter();

    /// Slot called when the apply filters button is pressed
    IODICOM_API void applyFilters();

    /// Slot used to display the context menu on a filter
    IODICOM_API void showContextMenuForSelectedFilter(const QPoint& pos);

    /// Slot used to display a warning when ignore errors checkbox is checked
    IODICOM_API void onForceChecked(int state);
protected:

    /// Do nothing.
    IODICOM_API virtual void configuring() override;

    /// Override
    IODICOM_API virtual void starting() override;

    /// Override
    IODICOM_API virtual void stopping() override;

    /// Override
    IODICOM_API void updating() override;

    /// Override
    IODICOM_API void info(std::ostream& _sstream ) override;

    /// Fill the combobox with the list of available filters
    IODICOM_API void fillAvailableFilters();

    /**
     * @brief Create and add a filter in a specific position
     * @param[in] filterTypeIndex Filter type index in the combobox
     * @param[in] position Position in the list
     */
    IODICOM_API void addFilter(int filterTypeIndex, int position);

    /**
     * @brief Sort filters
     * @param[in] a First filter
     * @param[in] b Second filter
     */
    static bool sortFilters(const ::fwDicomIOFilter::IFilter::sptr& a, const ::fwDicomIOFilter::IFilter::sptr& b);

    static QIcon getFilterIcon(::fwDicomIOFilter::IFilter::sptr filter);

    static std::string getFilterDescription(::fwDicomIOFilter::IFilter::sptr filter);

    /// Combo box displaying the available filters
    QPointer< QComboBox > m_availableFilterListWidget;

    /// Selected filters list widget
    QPointer< QListWidget > m_selectedFilterListWidget;

    /// Button used to add a filter in the list
    QPointer< QPushButton > m_addFilterButton;

    /// Button used to remove a filter in the list
    QPointer< QPushButton > m_removeFilterButton;

    /// Button used to configure a filter in the list
    QPointer< QPushButton > m_configureFilterButton;

    /// Button used to apply filters of the list
    QPointer< QPushButton > m_applyFiltersButton;

    /// Button used to split a composite filter
    QPointer< QPushButton > m_splitFilterButton;

    /// Delete key shortcut
    QPointer< QShortcut > m_deleteShortcut;

    /// Forced apply check box
    QPointer< QCheckBox > m_forcedApplyCheckBox;

    /// Map used to store filters and their keys
    FilterMapType m_filtersMap;

    /// Destination SeriesDB ID
    std::string m_destinationSeriesDBID;

    /// Destination SeriesDB
    ::fwMedData::SeriesDB::sptr m_destinationSeriesDB;

};

} // namespace ioDicom
