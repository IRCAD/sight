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

#include <core/com/signal.hpp>

#include <data/color.hpp>
#include <data/image_series.hpp>
#include <data/real.hpp>
#include <data/string.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/material/standard.hpp>
#include <viz/scene3d/transformable.hpp>

#include <string>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

/**
 * @brief Draws a line through the point fiducials of a configured group.
 *
 * The path is updated when the fiducials change and appears when the group contains at least two valid points.
 * Clearing the line does not remove the fiducials from the image series.
 *
 * @section XML XML Configuration
 * @code{.xml}
 * <service uid="path_adp" type="sight::module::viz::scene3d_qt::adaptor::fiducials::path">
 *     <data image_series="..." />
 *     <config path_group="..." path_color="..." fiducial_size="..." />
 * </service>
 * @endcode
 *
 * @subsection InOut InOut
 * - \b data.image_series [sight::data::image_series]: image containing the path fiducials.
 *
 * @subsection Configuration Configuration
 * - \b path_group (optional, default="Path"): name of the fiducial group used for the path.
 * - \b path_color (optional, default=green): color of the displayed line.
 * - \b fiducial_size (optional, default=10.0): size of the fiducial points.
 * - \b transform (optional): Ogre transform node to which the line is attached.
 *
 * @section Slots Slots
 * - \b create_path(): draws the line from the current group points.
 * - \b clear_path(): removes only the displayed line.
 * - \b clear(): removes the line and point fiducials in the path group.
 * - \b select_group(bool): selects the path group or restores the ordinary group.
 * - \b point_edit_mode_changed(bool): restores the ordinary group when point editing ends.
 * - \b set_landmark_group(std::string): remembers the ordinary group to restore after path editing.
 *
 * @section Signals Signals
 * - \b group_selected(std::string): emits the group selected for editing.
 * - \b path_edit_mode_changed(bool): requests that the path action be unchecked when point editing ends.
 * - \b points_available(bool): whether the path has at least two valid points.
 */
class path final : public sight::viz::scene3d::adaptor,
                   public sight::viz::scene3d::transformable
{
public:

    SIGHT_DECLARE_SERVICE(path, sight::viz::scene3d::adaptor);

    path() noexcept;
    ~path() noexcept final = default;

    struct slots final
    {
        inline static const slot_key_t CLEAR              = "clear";
        inline static const slot_key_t SELECT_GROUP       = "select_group";
        inline static const slot_key_t POINT_EDIT_MODE    = "point_edit_mode_changed";
        inline static const slot_key_t SET_LANDMARK_GROUP = "set_landmark_group";
    };

    struct signals final
    {
        inline static const signal_key_t GROUP_SELECTED = "group_selected";
        using group_selected_t = core::com::signal<void (std::string)>;

        inline static const signal_key_t PATH_EDIT_MODE_CHANGED = "path_edit_mode_changed";
        using path_edit_mode_changed_t = core::com::signal<void (bool)>;

        inline static const signal_key_t POINTS_AVAILABLE = "points_available";
        using points_available_t = core::com::signal<void (bool)>;
    };

    /**
     * @brief SLOT: Clears the displayed path line and all point fiducials in the path group.
     */
    void clear();

    /**
     * @brief SLOT: Selects the path group or restores the ordinary group.
     */
    void select_group(bool _selected);

    /**
     * @brief SLOT: Restores the ordinary group when point edit mode is disabled.
     */
    void point_edit_mode_changed(bool _enabled);

    /**
     * @brief SLOT: Remembers the ordinary group to restore after path editing.
     */
    void set_landmark_group(std::string _group);

protected:

    void configuring() final;
    void starting() final;
    service::connections_t auto_connections() const final;
    void updating() final;
    void stopping() final;

private:

    /**
     * @brief Creates the path line from the current group points.
     */
    void create_path();

    /**
     * @brief Clears the path line from the scene.
     */
    void clear_path();

    Ogre::SceneNode* m_transform_node {nullptr};
    Ogre::ManualObject* m_path_object {nullptr};
    sight::viz::scene3d::material::standard::uptr m_path_material {nullptr};
    std::string m_landmark_group;

    ptr_inout<sight::data::image_series> m_image_series {this, "data.image_series"};
    ptr_in<sight::data::string> m_path_group {this, "config.path_group", "Path"};
    ptr_in<sight::data::color> m_path_color {this, "config.path_color", {0.0, 1.0, 0.0, 1.0}};
    ptr_in<sight::data::real> m_fiducial_size {this, "config.fiducial_size", 10.F};
};

} // namespace sight::module::viz::scene3d_qt::adaptor::fiducials
