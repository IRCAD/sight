/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "uiVisuOgre/SCompositorSelector.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreResource.h>
#include <OGRE/OgreResourceManager.h>

#include <QListWidgetItem>
#include <QWidget>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiVisuOgre::SCompositorSelector, ::fwData::Composite)

const ::fwCom::Slots::SlotKeyType SCompositorSelector::s_INIT_COMPOSITOR_LIST_SLOT = "initCompositorList";

static const std::string s_COMPOSITOR_RESOURCEGROUP_NAME = "compositorsPostFX";

//------------------------------------------------------------------------------

SCompositorSelector::SCompositorSelector() noexcept
{
    newSlot(s_INIT_COMPOSITOR_LIST_SLOT, &SCompositorSelector::initCompositorList, this);
}

//------------------------------------------------------------------------------

SCompositorSelector::~SCompositorSelector() noexcept
{
}

//------------------------------------------------------------------------------

void SCompositorSelector::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    m_layersBox       = new QComboBox();
    m_compositorChain = new QListWidget();

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_layersBox);
    layout->addWidget(m_compositorChain);

    qtContainer->setLayout( layout );

    this->refreshRenderers();

    QObject::connect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));
    QObject::connect(m_compositorChain, SIGNAL(itemChanged(QListWidgetItem*)), this,
                     SLOT(onSelectedCompositorItem(QListWidgetItem*)));
}

//------------------------------------------------------------------------------

void SCompositorSelector::stopping()
{
    m_connections.disconnect();

    this->destroy();
}

//------------------------------------------------------------------------------

void SCompositorSelector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SCompositorSelector::updating()
{
}

//------------------------------------------------------------------------------

void SCompositorSelector::onSelectedLayerItem(int index)
{
    m_currentLayer                    = m_layers[static_cast<size_t>(index)];
    ::fwRenderOgre::Layer::sptr layer = m_currentLayer.lock();

    if(layer)
    {
        // Set current context
        layer->getRenderService()->makeCurrent();

        // Empty the list widget
        m_compositorChain->clear();

        // Update the current layer
        m_layerCompositorChain = layer->getCompositorChain();

        // We need the ogre's viewport in order to add the compositors,
        // this is why we have to check the viewport's existence
        if(layer->getViewport())
        {
            // Fill the list widget
            this->updateCompositorList();
            // Iterates through the compositors and checks the enabled ones
            this->checkEnabledCompositors();
        }
    }
}

//------------------------------------------------------------------------------

void SCompositorSelector::onSelectedCompositorItem(QListWidgetItem* compositorItem)
{
    const ::std::string compositorName = compositorItem->text().toStdString();
    const bool isChecked               = (compositorItem->checkState() == ::Qt::Checked);
    ::fwRenderOgre::Layer::sptr layer = m_currentLayer.lock();

    if(layer)
    {
        layer->getRenderService()->makeCurrent();
        layer->updateCompositorState(compositorName, isChecked);
    }
}

//------------------------------------------------------------------------------

void SCompositorSelector::initCompositorList(fwRenderOgre::Layer::sptr layer)
{
    m_currentLayer = m_layers[0];

    if(layer == m_currentLayer.lock())
    {
        onSelectedLayerItem(0);
    }
}

//------------------------------------------------------------------------------

void SCompositorSelector::refreshRenderers()
{
    m_layersBox->clear();

    // Fill layer box with all enabled layers
    ::fwServices::registry::ObjectService::ServiceVectorType renderers =
        ::fwServices::OSR::getServices("::fwRenderOgre::SRender");

    for(auto srv : renderers)
    {
        ::fwRenderOgre::SRender::sptr render = ::fwRenderOgre::SRender::dynamicCast(srv);

        for(auto& layerMap : render->getLayers())
        {
            const std::string id       = layerMap.first;
            const std::string renderID = render->getID();
            m_layersBox->addItem(QString::fromStdString(renderID + " : " + id));
            m_layers.push_back(layerMap.second);

            m_connections.connect(layerMap.second, ::fwRenderOgre::Layer::s_INIT_LAYER_SIG,
                                  this->getSptr(), s_INIT_COMPOSITOR_LIST_SLOT);
        }
    }

    if(!m_layers.empty())
    {
        this->onSelectedLayerItem(0);
    }
}

//------------------------------------------------------------------------------

void SCompositorSelector::updateCompositorList()
{
    ::fwRenderOgre::Layer::sptr layer = m_currentLayer.lock();

    if(layer)
    {
        layer->getRenderService()->makeCurrent();

        // Retrieving all compositors
        ::Ogre::ResourceManager::ResourceMapIterator iter =
            ::Ogre::CompositorManager::getSingleton().getResourceIterator();
        while(iter.hasMoreElements())
        {
            ::Ogre::ResourcePtr compositor = iter.getNext();
            if (compositor->getGroup() == s_COMPOSITOR_RESOURCEGROUP_NAME)
            {
                QString compositorName = compositor.get()->getName().c_str();
                layer->addAvailableCompositor(compositorName.toStdString());

                QListWidgetItem* newCompositor = new QListWidgetItem(compositorName, m_compositorChain);
                newCompositor->setFlags(newCompositor->flags() | ::Qt::ItemIsUserCheckable);
                newCompositor->setCheckState(::Qt::Unchecked);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SCompositorSelector::checkEnabledCompositors()
{
    ::fwRenderOgre::Layer::sptr layer = m_currentLayer.lock();

    if(layer)
    {
        layer->getRenderService()->makeCurrent();

        if(!m_layerCompositorChain.empty())
        {
            for(int i(0); i < m_compositorChain->count(); ++i)
            {
                QListWidgetItem* currentCompositor = m_compositorChain->item(i);
                std::string currentCompositorName  = currentCompositor->text().toStdString();

                auto layerCompositor = std::find_if(m_layerCompositorChain.begin(),
                                                    m_layerCompositorChain.end(),
                                                    ::fwRenderOgre::compositor::ChainManager::FindCompositorByName(
                                                        currentCompositorName));

                if(layerCompositor != m_layerCompositorChain.end())
                {
                    if(layerCompositor->second)
                    {
                        currentCompositor->setCheckState(::Qt::Checked);
                        layer->updateCompositorState(currentCompositor->text().toStdString(), true);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void SCompositorSelector::uncheckCompositors()
{
    for(int i(0); i < m_compositorChain->count(); ++i)
    {
        QListWidgetItem* currentCompositor = m_compositorChain->item(i);
        currentCompositor->setCheckState(::Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

bool SCompositorSelector::isEnabledCompositor(const std::string& compositorName)
{
    auto layerCompositor = std::find_if(m_layerCompositorChain.begin(),
                                        m_layerCompositorChain.end(),
                                        ::fwRenderOgre::compositor::ChainManager::FindCompositorByName(
                                            compositorName));

    if(layerCompositor != m_layerCompositorChain.end())
    {
        return layerCompositor->second;
    }

    return false;
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
