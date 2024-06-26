/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <sight/viz/scene3d/config.hpp>

#include <core/com/signal.hpp>

#include <viz/scene3d/interactor/base.hpp>

namespace sight::viz::scene3d::interactor
{

/**
 * @brief Runs picking queries when the user clicks in the scene.
 *
 * Emits a signal with the relevant intersection data when picking succeeds.
 */
class SIGHT_VIZ_SCENE3D_CLASS_API mesh_picker_interactor final : public sight::viz::scene3d::interactor::base
{
public:

    /// Defines the type of signal sent when a picking query succeeded.
    using point_clicked_sig_t = core::com::signal<void (data::tools::picking_info)>;

    /// Initializes the picker.
    SIGHT_VIZ_SCENE3D_API mesh_picker_interactor(
        SPTR(layer)_layer           = nullptr,
        bool _layer_order_dependant = true
    ) noexcept;

    /// Destroys the picker.
    SIGHT_VIZ_SCENE3D_API ~mesh_picker_interactor() noexcept override;

    /// Runs a picking query when a mouse button is released @see mesh_picker_interactor::pick().
    SIGHT_VIZ_SCENE3D_API void button_release_event(mouse_button _button, modifier _mods, int _x, int _y) override;

    /// Runs a picking query when a mouse button is pressed @see mesh_picker_interactor::pick().
    SIGHT_VIZ_SCENE3D_API void button_press_event(mouse_button _button, modifier _mods, int _x, int _y) override;

    /// Sets the signal to be called when picking succeeded.
    SIGHT_VIZ_SCENE3D_API void set_point_clicked_sig(const point_clicked_sig_t::sptr& _sig);

    /// Sets the mask to filter out entities with mismathing query flags.
    SIGHT_VIZ_SCENE3D_API void set_query_mask(std::uint32_t _query_mask);

private:

    /**
     * @brief Triggers a picking query and sends a signal with the corresponding @see data::tools::picking_info.
     *
     * @param _button mouse button pressed.
     * @param _mods keyboard modifiers.
     * @param _x width coordinate of the mouse.
     * @param _y height coordinate of the mouse.
     * @param _pressed whether the button is pressed (true) or released (false).
     */
    void pick(mouse_button _button, modifier _mods, int _x, int _y, bool _pressed);

    /// Picking query mask. Filters out objects with mismatching flags.
    std::uint32_t m_query_mask {0xffffffff};

    /// Defines the signal sent when picking succeeded.
    point_clicked_sig_t::sptr m_point_clicked_sig;
};

} //namespace sight::viz::scene3d::interactor.
