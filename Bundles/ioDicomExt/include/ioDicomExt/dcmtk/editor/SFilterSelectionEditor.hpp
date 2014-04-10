/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_DCMTK_EDITOR_SFILTERSELECTIONEDITOR_HPP__
#define __IODICOMEXT_DCMTK_EDITOR_SFILTERSELECTIONEDITOR_HPP__

#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QShortcut>
#include <QWidget>

#include <gui/editor/IEditor.hpp>

#include <fwDicomIOFilter/IFilter.hpp>
#include <fwMedData/SeriesDB.hpp>

#include "ioDicomExt/config.hpp"

namespace ioDicomExt
{

namespace dcmtk
{

namespace editor
{

/**
 * @brief   This editor service is used to select and apply filters to Dicom series.
 * @class   SFilterSelectionEditor
 * @date    2014.
 */
class IODICOMEXT_CLASS_API SFilterSelectionEditor : public QObject, public ::gui::editor::IEditor
{
Q_OBJECT;

public :

    fwCoreServiceClassDefinitionsMacro ( (SFilterSelectionEditor)( ::gui::editor::IEditor ) ) ;

    typedef std::map< std::string, ::fwDicomIOFilter::IFilter::sptr > FilterMapType;

    /**
     * @brief Constructor
     */
    IODICOMEXT_API SFilterSelectionEditor() throw();

    /**
     * @brief Destructor
     */
    IODICOMEXT_API virtual ~SFilterSelectionEditor() throw();

private Q_SLOTS:
    /// Slot called when a new filter type is selected in the context menu
    IODICOMEXT_API void addFilter(int filterTypeIndex);

    /// Slot called when the add filter button is pressed
    IODICOMEXT_API void addFilterAtTheEnd();

    /// Slot called when the remove filter button is pressed
    IODICOMEXT_API void removeFilter();

    /// Slot called when a new filter is selected
    IODICOMEXT_API void updateButtonStatus(int filterIndex);

    /// Slot called when the configure filter button is pressed
    IODICOMEXT_API void configureFilter();

    /// Slot called when the split filter button is pressed
    IODICOMEXT_API void splitFilter();

    /// Slot called when the apply filters button is pressed
    IODICOMEXT_API void applyFilters();

    /// Slot used to display the context menu on a filter
    IODICOMEXT_API void showContextMenuForSelectedFilter(const QPoint &pos);

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
     @verbatim
     <service uid="filterSelectionEditor" impl="::ioDicomExt::dcmtk::editor::SFilterSelectionEditor"
         autoConnect="yes">
     </service>
     @endverbatim
    */
    IODICOMEXT_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void updating() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void info(std::ostream &_sstream ) ;

    /// Fill the combobox with the list of available filters
    IODICOMEXT_API void fillAvailableFilters();

    /**
     * @brief Create and add a filter in a specific position
     * @param[in] filterTypeIndex Filter type index in the combobox
     * @param[in] position Position in the list
     */
    IODICOMEXT_API void addFilter(int filterTypeIndex, int position);

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

} // namespace editor
} // namespace dcmtk
} // namespace ioDicomExt

#endif // __IODICOMEXT_DCMTK_EDITOR_SFILTERSELECTIONEDITOR_HPP__
