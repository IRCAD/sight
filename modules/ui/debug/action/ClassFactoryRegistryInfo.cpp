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

#include "modules/ui/debug/action/ClassFactoryRegistryInfo.hpp"

#include <core/base.hpp>

#include <service/extension/Factory.hpp>
#include <service/macros.hpp>

#include <QHBoxLayout>

namespace sight::module::ui::debug::action
{

//------------------------------------------------------------------------------

ClassFactoryRegistryInfo::ClassFactoryRegistryInfo() noexcept =
    default;

//------------------------------------------------------------------------------

ClassFactoryRegistryInfo::~ClassFactoryRegistryInfo() noexcept =
    default;

//------------------------------------------------------------------------------

void ClassFactoryRegistryInfo::updating()
{
    m_tree->clearSelection();
    m_tree->clear();

    using ServiceRegistry = service::extension::Factory;
    const ServiceRegistry::KeyVectorType& factoryKeys = ServiceRegistry::getDefault()->get_factory_keys();

    for(const ServiceRegistry::KeyVectorType::value_type& key : factoryKeys)
    {
        const auto objImpl = ServiceRegistry::getDefault()->getServiceObjects(key);
        auto* srvItem      = new QTreeWidgetItem();
        srvItem->setText(0, QString::fromStdString(key));
        srvItem->setText(1, QString::fromStdString(objImpl[0]));
        m_tree->addTopLevelItem(srvItem);
    }

    m_dialog->show();
}

//------------------------------------------------------------------------------

void ClassFactoryRegistryInfo::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void ClassFactoryRegistryInfo::starting()
{
    this->sight::ui::action::actionServiceStarting();

    QWidget* parent = qApp->activeWindow();
    m_dialog = new QDialog(parent);
    m_dialog->setWindowTitle("ServiceFactoryRegistry");
    m_dialog->setMinimumSize(800, 600);

    auto* sizer = new QHBoxLayout();
    m_tree = new QTreeWidget(m_dialog);
    QStringList headerList = (QStringList() << "Service" << "Object");
    m_tree->setColumnCount(2);
    m_tree->setHeaderLabels(headerList);
    m_tree->setColumnWidth(0, 300);
    m_tree->setColumnWidth(1, 460);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->setAlternatingRowColors(true);

    sizer->addWidget(m_tree);
    m_dialog->setLayout(sizer);
}

//------------------------------------------------------------------------------

void ClassFactoryRegistryInfo::stopping()
{
    m_dialog->hide();
    delete m_tree;
    delete m_dialog;

    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::debug::action
