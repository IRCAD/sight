/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/SCompositorSelector.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <material/Plugin.hpp>

#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreResource.h>
#include <OGRE/OgreResourceManager.h>

#include <QListWidgetItem>
#include <QWidget>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiVisuOgre::SCompositorSelector, ::fwData::Composite);

const ::fwCom::Slots::SlotKeyType SCompositorSelector::s_INIT_COMPOSITOR_LIST_SLOT = "initCompositorList";

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
    // Empty the list widget
    m_compositorChain->clear();

    // Update the current layer
    m_currentLayer = m_layers[static_cast<size_t>(index)];
    // Update the layer's compositor chain
    this->synchroniseWithLayerCompositorChain();

    // We need the ogre's viewport in order to add the compositors,
    // this is why we have to check the viewport's existence
    if(m_currentLayer.lock()->getViewport())
    {
        // Fill the list widget
        this->updateCompositorList();
        // Iterates through the compositors and checks the enabled ones
        this->checkEnabledCompositors();
    }
}

//------------------------------------------------------------------------------

void SCompositorSelector::onSelectedCompositorItem(QListWidgetItem* compositorItem)
{
    ::std::string compositorName = compositorItem->text().toStdString();
    bool isChecked = (compositorItem->checkState() == ::Qt::Checked);
    m_currentLayer.lock()->updateCompositorState(compositorName, isChecked);
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
}

//------------------------------------------------------------------------------

void SCompositorSelector::synchroniseWithLayerCompositorChain()
{
    m_layerCompositorChain = m_currentLayer.lock()->getCompositorChain();
}

//------------------------------------------------------------------------------

void SCompositorSelector::updateCompositorList()
{
    // Retrieving all compositors
    ::Ogre::ResourceManager::ResourceMapIterator iter = ::Ogre::CompositorManager::getSingleton().getResourceIterator();
    while(iter.hasMoreElements())
    {
        ::Ogre::ResourcePtr compositor = iter.getNext();
        if (compositor->getGroup() == ::material::s_COMPOSITOR_RESOURCEGROUP_NAME)
        {
            QString compositorName = compositor.get()->getName().c_str();
            m_currentLayer.lock()->addAvailableCompositor(compositorName.toStdString());

            QListWidgetItem* newCompositor = new QListWidgetItem(compositorName, m_compositorChain);
            newCompositor->setFlags(newCompositor->flags() | ::Qt::ItemIsUserCheckable);
            newCompositor->setCheckState(::Qt::Unchecked);
        }
    }
}

//------------------------------------------------------------------------------

void SCompositorSelector::checkEnabledCompositors()
{
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
                    m_currentLayer.lock()->updateCompositorState(currentCompositor->text().toStdString(), true);
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
