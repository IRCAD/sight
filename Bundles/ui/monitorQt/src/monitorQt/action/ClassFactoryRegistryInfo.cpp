/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
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

#include "monitorQt/action/ClassFactoryRegistryInfo.hpp"

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <QHBoxLayout>

namespace monitorQt
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::monitorQt::action::ClassFactoryRegistryInfo, ::fwData::Object );

//------------------------------------------------------------------------------

ClassFactoryRegistryInfo::ClassFactoryRegistryInfo( ) noexcept
{
}

//------------------------------------------------------------------------------

ClassFactoryRegistryInfo::~ClassFactoryRegistryInfo() noexcept
{
}

//------------------------------------------------------------------------------

void ClassFactoryRegistryInfo::updating( )
{
    m_tree->clearSelection();
    m_tree->clear();

    typedef ::fwServices::registry::ServiceFactory ServiceRegistry;
    const ServiceRegistry::KeyVectorType& factoryKeys = ServiceRegistry::getDefault()->getFactoryKeys();

    for( ServiceRegistry::KeyVectorType::value_type key :  factoryKeys )
    {
        const auto objImpl       = ServiceRegistry::getDefault()->getServiceObjects(key);
        QTreeWidgetItem* srvItem = new QTreeWidgetItem();
        srvItem->setText(0, QString::fromStdString(key));
        srvItem->setText(1, QString::fromStdString(objImpl[0]));
        m_tree->addTopLevelItem( srvItem );
    }
    m_dialog->show();
}

//------------------------------------------------------------------------------

void ClassFactoryRegistryInfo::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void ClassFactoryRegistryInfo::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();

    QWidget* parent = qApp->activeWindow();
    m_dialog = new QDialog(parent);
    m_dialog->setWindowTitle("ServiceFactoryRegistry");
    m_dialog->setMinimumSize(800, 600);

    QHBoxLayout* sizer = new QHBoxLayout();
    m_tree = new QTreeWidget( m_dialog );
    QStringList headerList = (QStringList() << "Service" << "Object");
    m_tree->setColumnCount(2);
    m_tree->setHeaderLabels(headerList);
    m_tree->setColumnWidth(0, 300);
    m_tree->setColumnWidth(1, 460);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->setAlternatingRowColors( true );

    sizer->addWidget( m_tree );
    m_dialog->setLayout( sizer );
}

//------------------------------------------------------------------------------

void ClassFactoryRegistryInfo::stopping()
{
    m_dialog->hide();
    delete m_tree;
    delete m_dialog;

    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace monitorQt
