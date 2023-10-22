/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include <core/runtime/extension.hpp>
#include <core/runtime/module.hpp>
#include <core/runtime/runtime.hpp>

#include <service/macros.hpp>

#include <QApplication>
#include <QVBoxLayout>

namespace sight::module::ui::debug::action
{

//------------------------------------------------------------------------------

ComponentsTree::ComponentsTree() noexcept =
    default;

//------------------------------------------------------------------------------

ComponentsTree::~ComponentsTree() noexcept =
    default;

//------------------------------------------------------------------------------

void ComponentsTree::updating()
{
    m_treeContainer->clearSelection();
    m_treeContainer->clear();

    for(const auto& module : core::runtime::modules())
    {
        const std::string module_name = module->identifier();
        const bool is_module_enabled  = module->enabled();
        auto* const module_item       = new QTreeWidgetItem();
        if(!is_module_enabled)
        {
            module_item->setBackground(0, QBrush(QColor(155, 155, 155)));
        }

        module_item->setText(0, QString::fromStdString(module_name));
        m_treeContainer->addTopLevelItem(module_item);

        //Extensions
        auto* extensions_item = new QTreeWidgetItem();
        extensions_item->setText(0, QObject::tr("Extensions"));
        module_item->addChild(extensions_item);

        for(const auto& extension : module->extensions())
        {
            const std::string point         = extension->point();
            const bool is_extension_enabled = extension->enabled();
            auto* const item                = new QTreeWidgetItem();
            if(!is_extension_enabled)
            {
                item->setBackground(0, QBrush(QColor(155, 155, 155)));
            }

            item->setText(0, QString::fromStdString(point));
            extensions_item->addChild(item);
        }
    }

    m_dialog->show();
}

//------------------------------------------------------------------------------

void ComponentsTree::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void ComponentsTree::starting()
{
    this->sight::ui::action::actionServiceStarting();

    QWidget* parent = qApp->activeWindow();
    m_dialog = new QDialog(parent);
    m_dialog->setWindowTitle("Components Tree");
    m_treeContainer = new QTreeWidget(m_dialog);
    m_treeContainer->setHeaderHidden(true);

    auto* topsizer = new QVBoxLayout();
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

    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::debug::action
