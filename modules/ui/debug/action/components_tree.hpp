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

#include <ui/__/action.hpp>

#include <QDialog>
#include <QObject>
#include <QPointer>
#include <QTreeWidget>

namespace sight::module::ui::debug::action
{

/**
 * @brief  Implements an action that show module information.
 */
class components_tree : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(components_tree, sight::ui::action);

    /// Does nothing
    components_tree() noexcept;

    /// Does nothing
    ~components_tree() noexcept override;

protected:

    /// Calls classic action methods to configure
    void configuring() override;

    /// Prepare dialog box
    void starting() override;

    /// Fill factory information in m_tree and show the dialog
    void updating() override;

    /// Delete dialog box
    void stopping() override;

private:

    /// Dialog shown on update method
    QPointer<QDialog> m_dialog;

    /// Widget used to show association ( service impl , data impl )
    QPointer<QTreeWidget> m_tree_container;
};

} // namespace sight::module::ui::debug::action
