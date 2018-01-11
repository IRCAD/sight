/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_WIDGET_STUDYEDITOR_HPP__
#define __UIMEDDATAQT_WIDGET_STUDYEDITOR_HPP__

#include "uiMedDataQt/config.hpp"

#include <fwCore/macros.hpp>

#include <QPalette>
#include <QPointer>
#include <QWidget>

class QLineEdit;

namespace fwMedData
{
class Series;
class Study;
}

namespace uiMedDataQt
{
namespace widget
{

/**
 * @brief   Widget to edit ::fwMedData::Study information
 */
class UIMEDDATAQT_CLASS_API StudyEditor : public QWidget
{
Q_OBJECT

public:

    /// Constructor.
    UIMEDDATAQT_API StudyEditor(QWidget* parent = 0);

    /// Destructor
    UIMEDDATAQT_API ~StudyEditor();

    /// Set referring series
    UIMEDDATAQT_API void setSeries(SPTR(::fwMedData::Series) series);

    /// Returns the study object created by this editor
    UIMEDDATAQT_API SPTR(::fwMedData::Study) getStudy();

    /// Returns true if the study information given through the editor is valid.
    UIMEDDATAQT_API bool isValid() const;

protected Q_SLOTS:

    /// Triggered when study date text changes
    void onDateChanged(const QString&);

    /// Triggered when study time text changes
    void onTimeChanged(const QString&);

    /// Triggered when study description text changes
    void onDescChanged(const QString&);

protected:

    SPTR(::fwMedData::Series) m_series;

    /**
     * @name Widgets allowing study information editing.
     * @{ */
    QPointer<QLineEdit> m_date;
    QPointer<QLineEdit> m_time;
    QPointer<QLineEdit> m_referringPhysicianName;
    QPointer<QLineEdit> m_description;
    QPointer<QLineEdit> m_patientAge;
    /**  @} */

    /**
     * @name Styles applied oon widgets when input errors are detected
     * @{ */
    QPalette m_paletteDate;
    QPalette m_paletteTime;
    /**  @} */
};

} // namespace widget
} // namespace uiMedDataQt

#endif // __UIMEDDATAQT_WIDGET_STUDYEDITOR_HPP__

