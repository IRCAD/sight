/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_WIDGET_EQUIPMENTEDITOR_HPP__
#define __UIMEDDATAQT_WIDGET_EQUIPMENTEDITOR_HPP__

#include <QWidget>
#include <QPointer>

#include <fwCore/macros.hpp>

#include "uiMedDataQt/config.hpp"

class QLineEdit;

namespace fwMedData
{
class Series;
class Equipment;
}

namespace uiMedData
{
namespace widget
{

/**
 * @brief   Widget to edit ::fwMedData::Equipment information
 * @class   EquipmentEditor
 * @date    2014.
 */
class UIMEDDATAQT_CLASS_API EquipmentEditor : public QWidget
{
Q_OBJECT

public:

    /// Constructor.
    UIMEDDATAQT_API EquipmentEditor(QWidget *parent = 0);

    /// Destructor
    UIMEDDATAQT_API ~EquipmentEditor();

    /// Set referring series
    UIMEDDATAQT_API void setSeries(SPTR(::fwMedData::Series) series);

    /// Get the current Equipment
    UIMEDDATAQT_API SPTR(::fwMedData::Equipment) getEquipment() const;

    /// Check if the equipment information entered in UI is valid
    UIMEDDATAQT_API bool isValid() const;
protected:

    SPTR(::fwMedData::Series) m_series;

    QPointer<QLineEdit> m_institutionName;

};

} // namespace widget
} // namespace uiMedData

#endif // __UIMEDDATAQT_WIDGET_EQUIPMENTEDITOR_HPP__

