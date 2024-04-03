/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <core/tools/failed.hpp>

#include <data/point.hpp>
#include <data/tools/picking_info.hpp>

#include <ui/__/editor.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>

namespace sight::module::ui::qt::viz
{

/**
 * @brief   point_editor service allows to display point information.
 */
class point_editor : public QObject,
                     public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(point_editor, sight::ui::editor);

    /// Constructor. Do nothing.
    point_editor() noexcept;

    /// Destructor. Do nothing.
    ~point_editor() noexcept override;

protected:

    ///This method launches the editor::starting method.
    void starting() override;

    ///This method launches the editor::stopping method.
    void stopping() override;

    void updating() override;

    void configuring() override;

    /// Overrides
    void info(std::ostream& _sstream) override;

private:

    /// Slot: get the interaction information
    void get_interaction(data::tools::picking_info _info);

    QPointer<QLineEdit> m_text_ctrl_x;
    QPointer<QLineEdit> m_text_ctrl_y;
    QPointer<QLineEdit> m_text_ctrl_z;
};

} // namespace sight::module::ui::qt::viz
