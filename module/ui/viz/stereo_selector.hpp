/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <ui/__/editor.hpp>

#include <viz/scene3d/compositor/chain_manager.hpp>
#include <viz/scene3d/layer.hpp>

#include <QComboBox>
#include <QObject>
#include <QPointer>

#include <vector>

namespace sight::module::ui::viz
{

/**
 * @brief   Allows to select the stereo mode of an Ogre Compositor
 */
class stereo_selector : public QObject,
                        public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(stereo_selector, sight::ui::editor);

    /// Constructor.
    stereo_selector() noexcept;

    /// Destructor. Does nothing
    ~stereo_selector() noexcept override;

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="stereo_selectorInstance" type="sight::module::ui::viz::stereo_selector">
             <parameter>param</parameter>
        </service>
       @endcode
     * - \b Parameter : parameter description.
     */
    void configuring() override;

    /// Sets the connections and the UI elements
    void starting() override;

    /// Destroys the connections and cleans the container
    void stopping() override;

    /// Does nothing
    void updating() override;

private Q_SLOTS:

    /// Slot: called when a layer is selected
    /// Sets the current layer
    void on_selected_layer_item(int _index);

    /// Slot: called when a mode is selected
    void on_selected_mode_item(int _index);

private:

    /// Retrieves all the layers from the application thanks to the render services
    void refresh_renderers();

    QPointer<QComboBox> m_layers_box;
    QPointer<QComboBox> m_mode_box;

    std::vector<sight::viz::scene3d::layer::wptr> m_layers;
    sight::viz::scene3d::layer::wptr m_current_layer;
};

} // namespace sight::module::ui::viz
