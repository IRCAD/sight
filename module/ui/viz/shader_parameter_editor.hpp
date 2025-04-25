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

#include <core/com/helper/sig_slot_connection.hpp>

#include <data/reconstruction.hpp>
#include <data/string.hpp>

#include <service/base.hpp>

#include <ui/__/editor.hpp>
#include <ui/qt/container/widget.hpp>

#include <QVBoxLayout>

namespace sight::module::ui::viz
{

/**
 * @brief   Editor allowing to edit each parameters from each shader of a reconstruction
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="sight::module::ui::viz::shader_parameter_editor" >
            <inout key="reconstruction" uid="..." />
        </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b reconstruction [sight::data::reconstruction]: reconstruction whose paremeters should be edited.
 */
class shader_parameter_editor : public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(shader_parameter_editor, sight::ui::editor);

    using editor_implementation_t = std::string;
    using object_classname_t      = std::string;
    using object_id               = std::string;
    using editor_map_t            = std::map<object_classname_t, editor_implementation_t>;

    /// Constructor.
    shader_parameter_editor() noexcept;

    /// Destructor. Destroy UI
    ~shader_parameter_editor() noexcept override;

protected:

    /// Initialize the container
    void configuring() override;

    /// Start the service, slot connections, widget initialization .
    void starting() override;

    /// Destroy the service and the container.
    void stopping() override;

    /// Update the interface.
    void updating() override;

private:

    /// Clear the current container
    void clear();
    /// Retrieves the shader parameters attached to the Reconstruction object and stores them into a collection
    void update_gui_info();
    /// Instanciates the needed ui editors according to the stored informations
    void fill_gui();

    /// Internal class that contain the informations concerning the editor that is created.
    struct shader_editor_info
    {
        std::string uuid;
        sight::ui::qt::container::widget::sptr editor_panel;
        service::base::wptr srv;
        core::com::helper::sig_slot_connection connections;
    };

    shader_editor_info m_editor_info;

    /// Connection to the material
    core::com::helper::sig_slot_connection m_connections;

    QVBoxLayout* m_sizer {};

    data::ptr<data::reconstruction, data::access::inout> m_reconstruction {this, "reconstruction"};
};

} // namespace sight::module::ui::viz
