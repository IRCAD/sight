/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ui/qt/config.hpp"

#include <ui/base/dialog/IMultiSelectorDialog.hpp>

#include <QDialog>
#include <QString>
#include <QWidget>

#include <vector>

namespace sight::ui::qt
{

namespace dialog
{

//------------------------------------------------------------------------------

/**
 * @brief   Allows choosing multiple elements among several selections.
 */

class UI_QT_CLASS_API MultiSelectorDialog : public ui::base::dialog::IMultiSelectorDialog,
                                            public QDialog
{
public:

    SIGHT_DECLARE_CLASS(
        MultiSelectorDialog,
        ui::base::dialog::IMultiSelectorDialog,
        ui::base::factory::New<MultiSelectorDialog>
    )

    UI_QT_API MultiSelectorDialog(ui::base::GuiBaseObject::Key key);

    UI_QT_API virtual ~MultiSelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    UI_QT_API void setSelections(Selections _selections) override;

    /**
     * @brief Sets the selector title.
     */
    UI_QT_API void setTitle(std::string _title) override;

    /**
     * @brief Show the selector and return the selection.
     */
    UI_QT_API Selections show() override;

    UI_QT_API void setMessage(const std::string& msg) override;

private:

    Selections m_selections;
    /// Dialog box message
    std::string m_message;
    std::string m_title;
};

} // namespace dialog

} // namespace sight::ui::qt
