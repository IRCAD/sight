/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQt/config.hpp"

#include <fwGui/dialog/IPulseProgressDialog.hpp>

#include <QPointer>
#include <QProgressDialog>

#include <string>

QT_BEGIN_NAMESPACE
class QProgressDialog;
QT_END_NAMESPACE

namespace fwGuiQt
{
namespace dialog
{
/**
 * @brief   This class allows us to show a pulse progress bar.
 */
class FWGUIQT_CLASS_API PulseProgressDialog : public ::fwGui::dialog::IPulseProgressDialog
{
public:

    fwCoreClassMacro(PulseProgressDialog, ::fwGui::dialog::IPulseProgressDialog,
                     ::fwGui::factory::New< PulseProgressDialog >);

    FWGUIQT_API PulseProgressDialog(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~PulseProgressDialog();

    /// override
    FWGUIQT_API void setTitle(const std::string& title) override;

    /// override
    FWGUIQT_API void setMessage(const std::string& message) override;

    FWGUIQT_API void show() override;

protected:

    QPointer< QProgressDialog >  m_pdialog;

};
} // namespace dialog
} // namespace fwGuiQt
