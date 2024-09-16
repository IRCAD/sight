/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include "data/fiducials_series.hpp"

#include <data/point_list.hpp>

#include <geometry/data/point_list.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/interactor/base.hpp>
#include <viz/scene3d/material.hpp>
#include <viz/scene3d/text.hpp>

#include <Ogre.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <QPushButton>

#include <memory>
#include <optional>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

/**
 * @brief This adaptor displays shapes retrieved from the fiducials.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::fiducials::shape" auto_connect="true" >
        <inout key="image" uid="..." />
        <inout key="mask" uid="..." />
        <config fontSource="DejaVuSans.ttf" fontSize="32" radius="4.5" priority="2" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::image]: image containing the fiducials.
 *
 * @subsection Configuration Configuration:
 * - \b fontSize (optional, int, default=16): font size in points.
 * - \b radius (optional, float, default=4.5): size of the spheres.
 * - \b interactive (optional, bool, default=true): enables interactions with shapes.
 * - \b priority (optional, int, default=2): priority of the interactor.
 * - \b queryMask (optional, uint32, default=0xFFFFFFFF): mask used to filter out entities
 *   when the shape is auto snapped.
 * - \b queryFlags (optional, uint32, default=0x40000000): mask applied to the spheres.
 *
 * @section Slots Slots
 * - \b removeAll(): remove all shapes.
 * - \b removeShape(): removes shapes contained in the image from the scene manager.
 * - \b updateVisibility(bool): sets whether shapes are shown or not.
 * - \b updateModifiedShape(data::point_list::sptr): renders shapes in other negato2d and negato3d adaptors.
 * - \b toggleVisibility(): toggles whether shapes are shown or not.
 * - \b show(): shows shape.
 * - \b hide(): hides shape.
 * - \b restrict_to_current_slice(): only show shapes for the current slice.
 * - \b color (optional, string, default=""): color applied to the rulers, generated if empty.
 *
 * @section Signals Signals
 * - \b toolDeactivate(): signal set when the tool is deactivated.
 *
 *
 */
class shape final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::interactor::base
{
public:

    struct signals
    {
        using key_t = sight::core::com::signals::key_t;
        static inline const key_t DEACTIVATE_SHAPE_TOOL = "deactivate_shape_tool";

        using void_signal_t = sight::core::com::signal<void ()>;
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(shape, sight::viz::scene3d::adaptor);

    /// Initialize slots.
    shape() noexcept;

    /// Destroys the adaptor.
    ~shape() noexcept override = default;

    struct slots final
    {
        using key_t = sight::core::com::slots::key_t;

        inline static const key_t REMOVE_ALL            = "remove_all";
        inline static const key_t REMOVE_SHAPES         = "remove_shapes";
        inline static const key_t ACTIVATE_SHAPE_TOOL   = "activate_shape_tool";
        inline static const key_t UPDATE_MODIFIED_SHAPE = "update_modified_shape";

        inline static const key_t RESTRICT_TO_CURRENT_SLICE = "restrict_to_current_slice";
    };

    /**
     * @brief Retrieves the picked shape and stores the result in m_pickedData.
     * @param _button mouse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void button_press_event(mouse_button _button, modifier _mod, int _x, int _y) override;

    /**
     * @brief Moves a shape stored in m_pickedData.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    void mouse_move_event(
        mouse_button _button,
        modifier _mod,
        int _x,
        int _y,
        int _dx,
        int _dy
    ) override;

    /// Resets m_pickedData.
    void button_release_event(
        mouse_button _button,
        modifier _mod,
        int _x,
        int _y
    ) override;

    /// catch escape to go out of add shape mode
    void key_press_event(
        int _key,
        modifier _mods,
        int _mouseX,
        int _mouseY
    ) final;

    /// Hides the contextual menu when the mouse wheel is used
    void wheel_event(modifier _mods, double _angleDelta, int _x, int _y) final;

protected:

    /// Configures the service.
    void configuring() override;

    /// Adds the interactor to the layer and creates the material.
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     *
     * Connect data::image::MODIFIED_SIG to service::slots::UPDATE
     */
    connections_t auto_connections() const override;

    /// Updates materials and all shapes.
    void updating() override;

    /// Removes the interactor from the layer and destroys Ogre resources.
    void stopping() override;

    /**
     * @brief Sets shapes visibility.
     * @param _visible the visibility status of shapes.
     */
    void set_visible(bool _visible) override;

    void set_visible(std::string _id, bool _visible);

private:

    /// TODO UPDATE
    /// Stores Ogre resources used to display a shape.
    /// Two spheres each attached to a node, a label to display millimeters,
    /// one line rendered with the depth check and a dashed line rendered without depth check.
    /// The point list is used to update each points when the interactor move a shape sphere,
    /// it is retrieved from the image via a field.
    struct shape_data
    {
        data::point_list::sptr m_point_list;
        std::vector<Ogre::SceneNode*> m_nodes {};
        std::vector<Ogre::ManualObject*> m_spheres {};
        std::vector<Ogre::ManualObject*> m_lines {};
        std::vector<Ogre::ManualObject*> m_dashed_lines {};
        Ogre::SceneNode* m_label_node {};
        sight::viz::scene3d::text::sptr m_label;
    };

    /// Stores picking information.
    struct picked_data
    {
        shape_data* m_data;
        bool m_first;
    };

    /// Map each shape to there related list ID.
    using shape_map = std::map<core::id::type, shape_data>;

    class delete_bin_button_when_focus_out : public QObject
    {
    public:

        explicit delete_bin_button_when_focus_out(shape* _shape);
        bool eventFilter(QObject* o, QEvent* e) override;
        shape* m_shape;
    };

    /// Saves the created shapes to image fiducials
    void update_image_fiducials(data::point_list::sptr _pl);

    /// SLOT: Remove all shapes.
    void remove_all();

    /// Retrieves shapes from the image and remove them from the scene.
    void remove_shapes();

    /// Activates the shape tool by changing the cursor and updating a boolean
    void activate_shape_tool(bool _activate);

    /**
     * @brief Creates a shape and add it into m_shapes.
     * @param _pl The point list used to create the shape.
     */
    void create_shape(data::point_list::sptr _pl);

    // TODO
    void create_mask(data::point_list::sptr _pl);

    /**
     * @brief Updates a shape.
     * @param _data Shape to update.
     * @param _begin New begin position.
     * @param _end New end position
     */
    void update_shape(const shape_data* _data, Ogre::Vector3 _begin, Ogre::Vector3 _end);

    /**
     * @brief Destroys a shape from its ID and remove it from m_shapes.
     * @param _id ID of the shape.
     */
    void destroy_shape(core::id::type _id);

    /// Slot: Allows visualizing the creation and/or modification of shape fiducials of an image in multiple
    /// adaptors(Negato2d/Negato3d) by displaying the shapes synchronously.
    void update_modified_shape(data::point_list::sptr _pl);

    void set_cursor(QCursor cursor);

    void remove_shape(data::point_list::sptr pl);

    void update_from_fiducials();

    void restrict_to_current_slice();

    /// Defines the radius of spheres.
    float m_sphere_radius {3.5F};

    /// Defines the font size in points.
    std::size_t m_font_size {12};

    /// Defines whether or not interactions are enabled with shapes.
    bool m_interactive {true};

    /// Defines whether to start using the shape creation tool or not.
    bool m_tool_activated {false};

    /// Defines whether the shapes are actually visible or not.
    bool m_visible {true};

    /// Defines the priority of the interactor.
    int m_priority {2};

    /// Counter to swap color at each new shape
    int m_color_index {0};

    /// Color extracted from the configuration
    std::string m_config_color {""};

    /// Defines the mask used to filter out entities when the shape is auto snapped.
    std::uint32_t m_query_mask {0xFFFFFFFF};

    /// Defines the mask used to filter shapes, it optimizes the ray launched to retrieve the picked shape.
    std::uint32_t m_query_flag {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Defines the material name with no depth check for spheres.
    std::string m_sphere_material_name;

    /// Defines the material name with depth check for lines.
    std::string m_line_material_name;

    /// Defines the material name with no depth check for dashed lines.
    std::string m_dashed_line_material_name;

    /// Contains the material with no depth check for spheres.
    sight::viz::scene3d::material::uptr m_sphere_material {nullptr};

    /// Contains the material with depth check for lines.
    sight::viz::scene3d::material::uptr m_line_material {nullptr};

    /// Contains the material with no depth check for dashed lines.
    sight::viz::scene3d::material::uptr m_dashed_line_material {nullptr};

    // Keep a map of the shapes associated to each image slice
    std::map<std::size_t, std::vector<core::id::type> > m_index_to_shapes {};

    /// Stores all generated shapes.
    shape_map m_shapes {};
    std::vector<data::point::sptr> m_points {};

    // Cached fiducials from the input image
    std::vector<sight::data::fiducials_series::fiducial> m_cached_fiducials;

    QPushButton* m_bin_button {nullptr};

    std::unique_ptr<delete_bin_button_when_focus_out> m_eventFilter {nullptr};

    static constexpr std::string_view s_IMAGE_INOUT = "image";
    sight::data::ptr<sight::data::image, sight::data::access::inout> m_image {this, s_IMAGE_INOUT};

    static constexpr std::string_view s_MASK_INOUT = "mask";
    sight::data::ptr<sight::data::image, sight::data::access::inout> m_mask {this, s_MASK_INOUT};
};

} // namespace sight::module::viz::scene3d_qt::adaptor
