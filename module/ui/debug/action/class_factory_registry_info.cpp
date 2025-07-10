/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "module/ui/debug/action/class_factory_registry_info.hpp"

#include <core/base.hpp>

#include <service/extension/factory.hpp>
#include <service/macros.hpp>

#include <QHBoxLayout>

namespace sight::module::ui::debug::action
{

//------------------------------------------------------------------------------

class_factory_registry_info::class_factory_registry_info() noexcept =
    default;

//------------------------------------------------------------------------------

class_factory_registry_info::~class_factory_registry_info() noexcept =
    default;

//------------------------------------------------------------------------------

void class_factory_registry_info::updating()
{
    m_tree->clearSelection();
    m_tree->clear();

    const auto& factory_keys = service::extension::factory::get()->get_factory_keys();

    for(const auto& key : factory_keys)
    {
        const auto obj_impl = service::extension::factory::get()->get_service_objects(key);
        auto* srv_item      = new QTreeWidgetItem();
        srv_item->setText(0, QString::fromStdString(key));
        srv_item->setText(1, QString::fromStdString(obj_impl[0]));
        m_tree->addTopLevelItem(srv_item);
    }

    m_dialog->show();
}

//------------------------------------------------------------------------------

void class_factory_registry_info::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void class_factory_registry_info::starting()
{
    this->sight::ui::action::action_service_starting();

    QWidget* parent = qApp->activeWindow();
    m_dialog = new QDialog(parent);
    m_dialog->setWindowTitle("ServiceFactoryRegistry");
    m_dialog->setMinimumSize(800, 600);

    auto* sizer = new QHBoxLayout();
    m_tree = new QTreeWidget(m_dialog);
    QStringList header_list = (QStringList() << "Service" << "Object");
    m_tree->setColumnCount(2);
    m_tree->setHeaderLabels(header_list);
    m_tree->setColumnWidth(0, 300);
    m_tree->setColumnWidth(1, 460);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->setAlternatingRowColors(true);

    sizer->addWidget(m_tree);
    m_dialog->setLayout(sizer);
}

//------------------------------------------------------------------------------

void class_factory_registry_info::stopping()
{
    m_dialog->hide();
    delete m_tree;
    delete m_dialog;

    this->sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::debug::action
