/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTERQT_WIDGET_QHEXSPINBOX_HPP__
#define __FWDICOMIOFILTERQT_WIDGET_QHEXSPINBOX_HPP__

#include <QRegExpValidator>
#include <QSpinBox>
#include <QWidget>

#include "fwDicomIOFilterQt/config.hpp"

namespace fwDicomIOFilterQt
{
namespace widget
{

/**
 * @class QHexSpinBox
 * @brief QSpinBox for hexadecimal values
 * @date  2014.
 */
class QHexSpinBox : public QSpinBox
{
    Q_OBJECT;

public:
    /// Constructor
    QHexSpinBox(QWidget* parent=0);

protected:
    /// Override
    QValidator::State validate(QString &text, int &pos) const;

    /// Override
    int valueFromText(const QString &text) const;

    /// Override
    QString textFromValue(int value) const;


    QRegExpValidator* m_validator;


};

} // namespace widget
} // namespace fwDicomIOFilterQt


#endif /* __FWDICOMIOFILTERQT_WIDGET_QHEXSPINBOX_HPP__ */
