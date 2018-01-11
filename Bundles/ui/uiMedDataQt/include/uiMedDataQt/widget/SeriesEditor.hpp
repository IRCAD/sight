/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_WIDGET_SERIESEDITOR_HPP__
#define __UIMEDDATAQT_WIDGET_SERIESEDITOR_HPP__

#include "uiMedDataQt/config.hpp"

#include <fwCore/macros.hpp>

#include <QListWidget>
#include <QPalette>
#include <QPointer>
#include <QWidget>

class QLineEdit;
class QPushButton;

namespace fwMedData
{
class Series;
}

namespace uiMedDataQt
{
namespace widget
{

/**
 * @brief Simple list widget where items can be added/removed from an input widget with add/remove buttons.
 */
class ListInput : public QWidget
{
Q_OBJECT

public:
    /**
     * @name Constructor/Destructor
     * @{ */
    ListInput();
    ~ListInput()
    {
    }
    /**  @} */

    /// Erases all items in the list
    QListWidget* getListWidget()
    {
        return m_performingPhysiciansName;
    }

protected Q_SLOTS:

    /// Triggered when the add button is clicked, if the input text is not empty
    void onAddPhysician();

    /// Triggered when the remove button is clicked, if an item is selected in the list
    void onRemovePhysician();

    /// Triggered when the input text changes
    void onInputChanged(const QString&);

    /// Triggered when the selection in the list changes
    void onSelectionChanged();

protected:
    /// Input widget to edit items
    QPointer<QLineEdit> m_physicianInput;

    /**
     * @name Buttons to add/remove items
     * @{ */
    QPointer<QPushButton> m_addPhysicianButton;
    QPointer<QPushButton> m_removePhysicianButton;
    /**  @} */

    /// List widget
    QPointer<QListWidget> m_performingPhysiciansName;
};

/**
 * @brief   Widget to edit ::fwMedData::Series information
 */
class UIMEDDATAQT_CLASS_API SeriesEditor : public QWidget
{
Q_OBJECT

public:

    /**
     * @name Constructor/Destructor
     * @{ */
    UIMEDDATAQT_API SeriesEditor(QWidget* parent = 0);
    UIMEDDATAQT_API ~SeriesEditor();
    /**  @} */

    /// Set referring series
    UIMEDDATAQT_API void setSeries(SPTR(::fwMedData::Series) series);

    /// Get the current Series
    UIMEDDATAQT_API SPTR(::fwMedData::Series) getSeries();

    /// Check if the equipment information entered in UI is valid
    UIMEDDATAQT_API bool isValid() const;

protected Q_SLOTS:

    /// Triggered when series date text changes
    void onDateChanged(const QString&);

    /// Triggered when series time text changes
    void onTimeChanged(const QString&);

    /// Triggered when series description text changes
    void onDescChanged(const QString&);

protected:

    /// Series to edit information
    SPTR(::fwMedData::Series) m_series;

    /**
     * @name Widgets allowing series information editing.
     * @{ */
    QPointer<QLineEdit> m_modality;
    QPointer<QLineEdit> m_date;
    QPointer<QLineEdit> m_time;
    QPointer<QLineEdit> m_description;
    QPointer<QLineEdit> m_physicianInput;
    QPointer<QPushButton> m_addPhysicianButton;
    QPointer<QPushButton> m_removePhysicianButton;
    QPointer<ListInput> m_physicians;
    /**  @} */

    /// Style applied oon widgets when input errors are detected
    QPalette m_palette;
};

} // namespace widget
} // namespace uiMedDataQt

#endif // __UIMEDDATAQT_WIDGET_SERIESEDITOR_HPP__

