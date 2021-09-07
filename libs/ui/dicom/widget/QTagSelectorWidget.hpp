/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "ui/dicom/config.hpp"
#include "ui/dicom/widget/QHexSpinBox.hpp"

#include <dcmtk/dcmdata/dctagkey.h>

#include <QLabel>
#include <QPointer>
#include <QWidget>

namespace sight::ui::dicom
{

namespace widget
{

/**
 * @brief Widget used to select a dicom tag
 */
class QTagSelectorWidget : public QWidget
{
Q_OBJECT;

public:

    /// Constructor
    QTagSelectorWidget(QWidget* parent = 0);

    /// Destructor
    ~QTagSelectorWidget();

    /// Set tag
    void setTagValue(const DcmTagKey& tag);

    /// Get tag
    DcmTagKey getTag();

protected Q_SLOTS:

    /// Update tag name when selected tag changes
    void updateTagName(int value = 0);

protected:

    /// Group spin box
    QPointer<ui::dicom::widget::QHexSpinBox> m_groupSpinBox;

    /// Tag spin box
    QPointer<ui::dicom::widget::QHexSpinBox> m_elementSpinBox;

    /// tag name label
    QPointer<QLabel> m_tagNameLabel;

    /// Tag
    DcmTagKey m_tag;
};

} // namespace widget

} // namespace sight::ui::dicom
