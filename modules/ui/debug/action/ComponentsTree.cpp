/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "modules/ui/debug/action/ComponentsTree.hpp"

#include <core/base.hpp>
#include <core/runtime/Extension.hpp>
#include <core/runtime/Module.hpp>
#include <core/runtime/Runtime.hpp>

#include <services/macros.hpp>

#include <QApplication>
#include <QVBoxLayout>

namespace sight::modules::ui::debug
{
namespace action
{

fwServicesRegisterMacro( ::sight::ui::base::IActionSrv, ::sight::modules::debugQt::action::ComponentsTree,
                         ::sight::data::Object )

//------------------------------------------------------------------------------

ComponentsTree::ComponentsTree( ) noexcept
{
}

//------------------------------------------------------------------------------

ComponentsTree::~ComponentsTree() noexcept
{
}

//------------------------------------------------------------------------------

void ComponentsTree::updating( )
{
    m_treeContainer->clearSelection();
    m_treeContainer->clear();

    core::runtime::Runtime* defaultRuntime = core::runtime::Runtime::getDefault();

    for (const auto& module : defaultRuntime->getModules())
    {
        const std::string moduleName      = module->getIdentifier();
        const bool isModuleEnabled        = module->isEnabled();
        QTreeWidgetItem* const moduleItem = new QTreeWidgetItem();
        if(!isModuleEnabled)
        {
            moduleItem->setBackground(0, QBrush(QColor(155, 155, 155)));
        }
        moduleItem->setText(0, QString::fromStdString(moduleName));
        m_treeContainer->addTopLevelItem( moduleItem );

        //Extensions
        QTreeWidgetItem* extensionsItem = new QTreeWidgetItem();
        extensionsItem->setText(0, QObject::tr("Extensions"));
        moduleItem->addChild( extensionsItem );

        for (const auto& extension : module->getExtensions())
        {
            const std::string point       = extension->getPoint();
            const bool isExtensionEnabled = extension->isEnabled();
            QTreeWidgetItem* const item   = new QTreeWidgetItem();
            if(!isExtensionEnabled)
            {
                item->setBackground(0, QBrush(QColor(155, 155, 155)));
            }
            item->setText(0, QString::fromStdString(point));
            extensionsItem->addChild( item );
        }
    }

    m_dialog->show();
}

//------------------------------------------------------------------------------

void ComponentsTree::configuring()
{
    this->sight::ui::base::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void ComponentsTree::starting()
{
    this->sight::ui::base::IActionSrv::actionServiceStarting();

    QWidget* parent = qApp->activeWindow();
    m_dialog = new QDialog(parent);
    m_dialog->setWindowTitle("Components Tree");
    m_treeContainer = new QTreeWidget(m_dialog);
    m_treeContainer->setHeaderHidden(true);

    QVBoxLayout* topsizer = new QVBoxLayout();
    topsizer->addWidget(m_treeContainer);
    topsizer->setContentsMargins(0, 0, 0, 0);
    m_dialog->setLayout(topsizer);
}

//------------------------------------------------------------------------------
void ComponentsTree::stopping()
{
    m_dialog->hide();
    delete m_treeContainer;
    delete m_dialog;

    this->sight::ui::base::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace sight::modules::ui::debug
