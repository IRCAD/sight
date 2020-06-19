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

#include "monitorQt/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <QApplication>
#include <QDialog>
#include <QTreeWidget>

namespace monitorQt
{
namespace action
{
/**
 * @brief   Implements an action to show services registered in factory
 */
class MONITORQT_CLASS_API ClassFactoryRegistryInfo : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceMacro(ClassFactoryRegistryInfo,  ::fwGui::IActionSrv );

    /// Does nothing
    MONITORQT_API ClassFactoryRegistryInfo() noexcept;

    /// Does nothing
    MONITORQT_API virtual ~ClassFactoryRegistryInfo() noexcept;

protected:

    /// Calls classic IAction methods to configure
    void configuring() override;

    /// Prepare dialog box
    void starting() override;

    /// Fill factory information in m_tree and show the dialog
    void updating() override;

    /// Delete dialog box
    void stopping() override;

private:

    /// Dialog shown on update method
    QDialog* m_dialog;

    /// Widget used to show association ( service impl , data impl )
    QTreeWidget* m_tree;

};

} // namespace action
} // namespace monitorQt
