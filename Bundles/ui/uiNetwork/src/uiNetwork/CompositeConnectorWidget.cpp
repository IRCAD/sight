/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiNetwork/ObjectConnectorWidget.hpp"
#include "uiNetwork/CompositeConnectorWidget.hpp"

#include <fwData/registry/detail.hpp>
#include <fwTools/fwID.hpp>

#include <QPushButton>
#include <QString>
#include <QObject>

namespace uiNetwork
{

//-----------------------------------------------------------------------------

CompositeConnectorWidget::CompositeConnectorWidget (QTreeWidget* root,
                                                    ::fwData::Object::sptr obj,
                                                    ConnectorModel const& connectorModel) :
    AObjectConnectorWidget (root, obj, connectorModel)
{
    QHBoxLayout* layout;
    QLabel* idLabel;
    QPushButton* addObjectButton;
    QLabel* typeLabel;

    layout          = new QHBoxLayout();
    idLabel         = new QLabel (QString::fromStdString (obj->getID()));
    typeLabel       = new QLabel (QString::fromStdString (obj->getLeafClassname()));
    addObjectButton = new QPushButton ("Add object");
    layout->addWidget (typeLabel);
    layout->addWidget (idLabel);
    layout->addWidget (addObjectButton);
    this->setLayout (layout);
    QObject::connect (addObjectButton, SIGNAL (clicked()), this, SLOT (onAddObject()));
    QObject::connect(&m_classSelectorDialog, SIGNAL(classnameSelected(std::string const &)),
                     this, SLOT(onClassnameSelected(std::string const &)));
}

//-----------------------------------------------------------------------------

void CompositeConnectorWidget::onAddObject()
{
    m_classSelectorDialog.show();
}

//-----------------------------------------------------------------------------

void CompositeConnectorWidget::onClassnameSelected(std::string const& classname)
{
    ::fwData::Composite::sptr composite;
    ::fwData::Vector::sptr vector;
    ::fwData::Object::sptr newObj;

    newObj = ::fwData::factory::New(classname);

    if (m_obj->getClassname() == ::fwData::Composite::classname())
    {
        composite = ::fwData::Composite::dynamicCast(m_obj);
        composite->getContainer().insert(std::pair<std::string, ::fwData::Object::sptr>(newObj->getID(), newObj));
    }
    else
    {
        vector = ::fwData::Vector::dynamicCast(m_obj);
        vector->getContainer().push_back(newObj);
    }
}

//-----------------------------------------------------------------------------

CompositeConnectorWidget::~CompositeConnectorWidget()
{
}

//-----------------------------------------------------------------------------

bool CompositeConnectorWidget::match (boost::regex const& regex)
{
    return true;
}

//-----------------------------------------------------------------------------

void CompositeConnectorWidget::refresh(boost::regex const& regex)
{
    ItemMapType snapshot;
    ::fwData::Composite::sptr composite;
    ::fwData::Vector::sptr vector;

    snapshot.insert(m_items.begin(), m_items.end());
    if (m_obj != NULL)
    {
        if (m_obj->getClassname() == ::fwData::Composite::classname())
        {
            ::fwData::Composite::iterator it;

            composite = ::fwData::Composite::dynamicCast(m_obj);
            for (it = composite->begin(); it != composite->end(); ++it)
            {
                this->fillTreeWidget(it->second, regex, snapshot);
            }
        }
        else
        {
            ::fwData::Vector::iterator it;

            vector = ::fwData::Vector::dynamicCast(m_obj);
            for (it = vector->begin(); it != vector->end(); ++it)
            {
                this->fillTreeWidget(*it, regex, snapshot);
            }
        }
    }
    this->removeUnusedConnectors(snapshot);
}

//-----------------------------------------------------------------------------

void CompositeConnectorWidget::fillTreeWidget (::fwData::Object::sptr obj,
                                               boost::regex const& regex,
                                               ItemMapType& itemSnapshot)
{
    AObjectConnectorWidget* item;
    ItemMapType::iterator it;

    if (obj != NULL)
    {
        if (m_items.find (obj->getID()) == m_items.end())
        {
            if (obj->getClassname() == ::fwData::Composite::classname()
                || obj->getClassname() == ::fwData::Vector::classname())
            {
                item = new CompositeConnectorWidget (m_root, obj, m_connectorModel);
            }
            else
            {
                item = new ObjectConnectorWidget (m_root, obj, m_connectorModel);
            }
            m_treeItem->addChild (item->getTreeItem());
            m_root->setItemWidget (item->getTreeItem(), 0, item);
            m_items[obj->getID()] = item;
        }
        else
        {
            item = m_items[obj->getID()];
            it   = itemSnapshot.find (obj->getID());
            itemSnapshot.erase (it);
        }
        item->refresh(regex);
        item->getTreeItem()->setHidden (!item->match (regex));
    }
}

//-----------------------------------------------------------------------------

void CompositeConnectorWidget::removeUnusedConnectors (ItemMapType const& removeItems)
{
    QTreeWidgetItem* item;
    ItemMapType::const_iterator it;

    for (it = removeItems.begin(); it != removeItems.end(); ++it)
    {
        m_items.erase(m_items.find(it->first));
        m_root->removeItemWidget(it->second->getTreeItem(), 0);
        item = m_root->takeTopLevelItem(m_root->indexOfTopLevelItem(it->second->getTreeItem()));
        delete item;
    }
}

//-----------------------------------------------------------------------------

} //namespace uiNetwork
