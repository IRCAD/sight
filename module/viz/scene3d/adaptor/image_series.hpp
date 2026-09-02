/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <core/com/helper/sig_slot_connection.hpp>
#include <core/com/signal.hpp>

#include <data/image.hpp>
#include <data/series_set.hpp>
#include <data/transfer_function.hpp>

#include <viz/scene3d/adaptor.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief Displays every image series in a series set through independent child adaptors.
 *
 * One child adaptor is created for each image series. The representation can be
 * `volume`, `negato2d` or `negato3d`; visibility is controlled independently for
 * every image.
 *
 * @section Signals Signals
 * - \b picked_voxel(string): forwards the voxel selected in a child negato.
 *
 * @section Slots Slots
 * - \b set_image_visibility(string, bool): changes one image visibility.
 * - \b toggle_widgets(bool): shows or hides volume widgets.
 * - \b set_bool_parameter(bool, string): updates a volume boolean parameter.
 * - \b set_int_parameter(int, string): updates a volume integer parameter.
 * - \b set_double_parameter(double, string): updates a volume floating-point parameter.
 * - \b update_clipping_box(): updates volume clipping boxes.
 * - \b update_slices_from_world(double, double, double): updates slices from a world point.
 * - \b set_slice_index(int, int, int): redraws after a slice index change.
 * - \b set_slice_type(int, int): redraws after an orientation change.
 * - \b set_transparency(double): changes negato3d transparency.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::image_series" auto_connect="true">
        <data series="${series_set}" />
        <data tf="${transfer_function}" />
        <config representation="negato2d" orientation="axial" />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b data.series [sight::data::series_set]: image series to display.
 * - \b data.tf [sight::data::transfer_function]: transfer function shared by children.
 *
 * @subsection Configuration Configuration
 * - \b representation (optional, volume/negato2d/negato3d, default=volume): child adaptor type.
 * - \b orientation (optional, axial/frontal/sagittal, default=axial): negato2d orientation.
 */
class image_series final : public sight::viz::scene3d::adaptor
{
public:

    SIGHT_DECLARE_SERVICE(image_series, sight::viz::scene3d::adaptor);

    struct signals
    {
        /// Signal emitted when a child negato picks a voxel.
        using picked_voxel_t = core::com::signal<void (std::string)>;
        static inline const signal_key_t PICKED_VOXEL = "picked_voxel";
    };

    struct slots
    {
        static inline const slot_key_t SET_IMAGE_VISIBILITY     = "set_image_visibility";
        static inline const slot_key_t TOGGLE_WIDGETS           = "toggle_widgets";
        static inline const slot_key_t SET_BOOL_PARAMETER       = "set_bool_parameter";
        static inline const slot_key_t SET_INT_PARAMETER        = "set_int_parameter";
        static inline const slot_key_t SET_DOUBLE_PARAMETER     = "set_double_parameter";
        static inline const slot_key_t UPDATE_CLIPPING_BOX      = "update_clipping_box";
        static inline const slot_key_t UPDATE_SLICES_FROM_WORLD = "update_slices_from_world";
        static inline const slot_key_t SET_SLICE_INDEX          = "set_slice_index";
        static inline const slot_key_t SET_SLICE_TYPE           = "set_slice_type";
        static inline const slot_key_t SET_TRANSPARENCY         = "set_transparency";
        static inline const slot_key_t FORWARD_PICKED_VOXEL     = "forward_picked_voxel";
    };

    image_series() noexcept;

    ~image_series() noexcept final = default;

    void set_visible(bool _visible) override;

protected:

    void configuring() final;

    void starting() final;

    void stopping() final;

    void updating() final;

    service::connections_t auto_connections() const final;

private:

    enum class representation_t : std::uint8_t
    {
        volume,
        negato2d,
        negato3d
    };

    void set_image_visibility(std::string _image_id, bool _visible);
    void toggle_widgets(bool _visible);
    void set_bool_parameter(bool _value, std::string _key);
    void set_int_parameter(int _value, std::string _key);
    void set_double_parameter(double _value, std::string _key);
    void update_clipping_box();
    void update_slices_from_world(double _x, double _y, double _z);
    void set_slice_index(int _axial_index, int _frontal_index, int _sagittal_index);
    void set_slice_type(int _from, int _to);
    void set_transparency(double _transparency);
    void forward_picked_voxel(std::string _text);

    template<typename ... Args>
    void forward_to_children(const std::string& _slot, const Args& ... _args);

    data::ptr<data::series_set, data::access::in> m_series {this, "data.series"};
    data::ptr<data::transfer_function, data::access::inout> m_tf {this, "data.tf"};
    std::unordered_map<std::string, data::transfer_function::sptr> m_transfer_functions;
    representation_t m_representation {representation_t::volume};
    std::string m_orientation {"axial"};
    bool m_selection_initialized {false};
    std::unordered_map<std::string, bool> m_image_visibility;
    std::unordered_map<std::string, sight::viz::scene3d::adaptor::sptr> m_children;
    std::unordered_map<std::string, data::image::sptr> m_masks;
    core::com::helper::sig_slot_connection m_child_connections;
};

} // namespace sight::module::viz::scene3d::adaptor
