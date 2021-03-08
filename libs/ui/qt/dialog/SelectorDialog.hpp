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

#include <ui/base/dialog/ISelectorDialog.hpp>

#include <QPushButton>
#include <QVector>

#include <vector>

namespace sight::ui::qt
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   SelectorDialog allowing the choice of an element among severals (_selections)
 */
class UI_QT_CLASS_API SelectorDialog : public ui::base::dialog::ISelectorDialog
{
public:

    SIGHT_DECLARE_CLASS(SelectorDialog, ui::base::dialog::ISelectorDialog, ui::base::factory::New< SelectorDialog >)

    UI_QT_API SelectorDialog(ui::base::GuiBaseObject::Key key);

    UI_QT_API virtual ~SelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    UI_QT_API void setSelections(std::vector< std::string > _selections) override;

    /**
     * @brief Sets the selector title.
     */
    UI_QT_API void setTitle(std::string _title) override;

    /**
     * @brief Show the selector and return the selection.
     */
    UI_QT_API std::string show() override;

    /// Set the message
    UI_QT_API void setMessage(const std::string& msg) override;

    /// Add a custom button to this dialog
    UI_QT_API void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

private:

    std::vector< std::string > m_selections;

    /// Dialog box message
    std::string m_message;
    std::string m_title;

    /// Stores custom buttons
    QVector< QPushButton* > m_customButtons;
};

} // namespace dialog
} // namespace sight::ui::qt
