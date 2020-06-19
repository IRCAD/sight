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

#pragma once

#include "uiVisuOgre/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <fwRenderOgre/compositor/ChainManager.hpp>
#include <fwRenderOgre/Layer.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QListWidget>
#include <QObject>
#include <QPointer>
#include <QVBoxLayout>

#include <vector>

namespace uiVisuOgre
{

/**
 * @brief   Allows to select an Ogre Compositor and apply it to a layer
 */
class UIVISUOGRE_CLASS_API SCompositorSelector : public QObject,
                                                 public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceMacro(SCompositorSelector, ::fwGui::editor::IEditor);

    /** @} */

    /**
     * @name Slots API
     * @{
     */
    typedef ::fwCom::Slot< void (::fwRenderOgre::Layer::sptr) > InitLayerSlotType;

    /// Slot: Populate the list of available compositors for the selected layer
    UIVISUOGRE_API static const ::fwCom::Slots::SlotKeyType s_INIT_COMPOSITOR_LIST_SLOT;

    /** @} */

    /// Constructor.
    UIVISUOGRE_API SCompositorSelector() noexcept;

    /// Destructor. Does nothing
    UIVISUOGRE_API virtual ~SCompositorSelector() noexcept;

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="SCompositorSelectorInstance" type="::uiVisuOgre::SCompositorSelector">
             <parameter>param</parameter>
        </service>
       @endcode
     * - \b Parameter : parameter description.
     */
    UIVISUOGRE_API virtual void configuring() override;

    /// Sets the connections and the UI elements
    UIVISUOGRE_API virtual void starting() override;

    /// Destroys the connections and cleans the container
    UIVISUOGRE_API virtual void stopping() override;

    /// Does nothing
    UIVISUOGRE_API virtual void updating() override;

private Q_SLOTS:

    /// Slot: called when a layer is selected
    /// Sets the current layer and initializes the compositor list
    void onSelectedLayerItem(int index);

    /// Slot: called when an item of the list widget is checked
    void onSelectedCompositorItem(QListWidgetItem* compositorItem);

private:
    /// Slot: Populate the list of available compositors for the selected layer
    void initCompositorList(::fwRenderOgre::Layer::sptr layer);

    /// Retrieves all the layers from the application thanks to the render services
    void refreshRenderers();

    /// Checks if there is an XML configured compositor chain.
    /// Sets the local compositor chain according to this.
    void synchroniseWithLayerCompositorChain();

    /// Retrieves the available compositors from the compositor resource group and stores them in the list widget
    void updateCompositorList();

    /// Iterates through the compositor chain and checks the enabled compositors
    void checkEnabledCompositors();

    /// Iterates through the compositor chain and unchecks them
    void uncheckCompositors();

    /// Indicates if a compositor is enabled on the layer
    bool isEnabledCompositor(const std::string& compositorName);

    QPointer<QComboBox> m_layersBox;

    QPointer<QListWidget> m_compositorChain;

    std::vector< ::fwRenderOgre::Layer::wptr > m_layers;
    ::fwRenderOgre::Layer::wptr m_currentLayer;

    ::fwRenderOgre::compositor::ChainManager::CompositorChainType m_layerCompositorChain;

    ///Connection service, needed for slot/signal association
    ::fwCom::helper::SigSlotConnection m_connections;
};

} // uiVisuOgre
