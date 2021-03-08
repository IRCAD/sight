/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/ui/viz/config.hpp"

#include <ui/base/IEditor.hpp>

#include <viz/scene3d/compositor/ChainManager.hpp>
#include <viz/scene3d/Layer.hpp>

#include <QComboBox>
#include <QObject>
#include <QPointer>

#include <vector>

namespace sight::module::ui::viz
{

/**
 * @brief   Allows to select the stereo mode of an Ogre Compositor
 */
class MODULE_UI_VIZ_CLASS_API SStereoSelector : public QObject,
                                                public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SStereoSelector, sight::ui::base::IEditor)

    /// Constructor.
    MODULE_UI_VIZ_API SStereoSelector() noexcept;

    /// Destructor. Does nothing
    MODULE_UI_VIZ_API virtual ~SStereoSelector() noexcept;

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="SStereoSelectorInstance" type="sight::module::ui::viz::SStereoSelector">
             <parameter>param</parameter>
        </service>
       @endcode
     * - \b Parameter : parameter description.
     */
    MODULE_UI_VIZ_API void configuring() override;

    /// Sets the connections and the UI elements
    MODULE_UI_VIZ_API void starting() override;

    /// Destroys the connections and cleans the container
    MODULE_UI_VIZ_API void stopping() override;

    /// Does nothing
    MODULE_UI_VIZ_API void updating() override;

private Q_SLOTS:

    /// Slot: called when a layer is selected
    /// Sets the current layer
    void onSelectedLayerItem(int index);

    /// Slot: called when a mode is selected
    void onSelectedModeItem(int index);

private:

    /// Retrieves all the layers from the application thanks to the render services
    void refreshRenderers();

    QPointer<QComboBox> m_layersBox;
    QPointer<QComboBox> m_modeBox;

    std::vector< sight::viz::scene3d::Layer::wptr > m_layers;
    sight::viz::scene3d::Layer::wptr m_currentLayer;
};

} // uiVisuOgre
