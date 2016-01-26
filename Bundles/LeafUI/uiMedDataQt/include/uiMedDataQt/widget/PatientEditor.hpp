/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_WIDGET_PATIENTEDITOR_HPP__
#define __UIMEDDATAQT_WIDGET_PATIENTEDITOR_HPP__

#include <QWidget>
#include <QPointer>

#include <fwCore/macros.hpp>

#include "uiMedDataQt/config.hpp"

class QLineEdit;
class QComboBox;

namespace fwMedData
{
class Series;
class Patient;
}

namespace uiMedData
{
namespace widget
{

/**
 * @brief   Widget to edit ::fwMedData::Patient information
 * @class   PatientEditor
 * @date    2014.
 */
class UIMEDDATAQT_CLASS_API PatientEditor : public QWidget
{
Q_OBJECT

public:

    /// Constructor.
    UIMEDDATAQT_API PatientEditor(QWidget *parent = 0);

    /// Destructor
    UIMEDDATAQT_API ~PatientEditor();

    /// Set referring series
    UIMEDDATAQT_API void setSeries(SPTR(::fwMedData::Series) series);

    /// Returns the patient object created by this editor
    UIMEDDATAQT_API SPTR(::fwMedData::Patient) getPatient() const;

    /// Returns true if the patient information given through the editor is valid.
    UIMEDDATAQT_API bool isValid() const;

protected Q_SLOTS:
    void onNameChanged(const QString&);
    void onBirthChanged(const QString&);

protected:

    SPTR(::fwMedData::Series) m_series;

    QPointer< QLineEdit > m_txtName;
    QPointer< QLineEdit > m_txtBirthdate;
    QPointer< QComboBox > m_cbSex;

    QPalette m_paletteName;
    QPalette m_paletteBirthdate;
};

} // namespace widget
} // namespace uiMedData

#endif // __UIMEDDATAQT_WIDGET_PATIENTEDITOR_HPP__

