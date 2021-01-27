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

#include "guiQt/config.hpp"

#include <gui/dialog/IPulseProgressDialog.hpp>

#include <QPointer>
#include <QProgressDialog>

#include <string>

QT_BEGIN_NAMESPACE
class QProgressDialog;
QT_END_NAMESPACE

namespace sight::guiQt
{
namespace dialog
{
/**
 * @brief   This class allows us to show a pulse progress bar.
 */
class GUIQT_CLASS_API PulseProgressDialog : public gui::dialog::IPulseProgressDialog
{
public:

    fwCoreClassMacro(PulseProgressDialog, gui::dialog::IPulseProgressDialog,
                     gui::factory::New< PulseProgressDialog >)

    GUIQT_API PulseProgressDialog(gui::GuiBaseObject::Key key);

    GUIQT_API virtual ~PulseProgressDialog();

    /// override
    GUIQT_API void setTitle(const std::string& title) override;

    /// override
    GUIQT_API void setMessage(const std::string& message) override;

    GUIQT_API void show() override;

protected:

    QPointer< QProgressDialog >  m_pdialog;

};
} // namespace dialog
} // namespace sight::guiQt
