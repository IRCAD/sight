/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "ui/dicom/widget/qhex_spin_box.hpp"

#include <dcmtk/dcmdata/dctagkey.h>

#include <QLabel>
#include <QPointer>
#include <QWidget>

namespace sight::ui::dicom::widget
{

/**
 * @brief Widget used to select a dicom tag
 */
class q_tag_selector_widget : public QWidget
{
Q_OBJECT;

public:

    /// Constructor
    q_tag_selector_widget(QWidget* _parent = nullptr);

    /// Destructor
    ~q_tag_selector_widget() override;

    /// Set tag
    void set_tag_value(const DcmTagKey& _tag);

    /// Get tag
    DcmTagKey get_tag();

protected Q_SLOTS:

    /// Update tag name when selected tag changes
    void update_tag_name(int _value = 0);

protected:

    /// Group spin box
    QPointer<ui::dicom::widget::q_hex_spin_box> m_group_spin_box;

    /// Tag spin box
    QPointer<ui::dicom::widget::q_hex_spin_box> m_element_spin_box;

    /// tag name label
    QPointer<QLabel> m_tag_name_label;

    /// Tag
    DcmTagKey m_tag;
};

} // namespace sight::ui::dicom::widget
