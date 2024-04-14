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

#include <sight/ex_collapsible_section/config.hpp>

#include <ui/__/editor.hpp>

namespace ex_collapsible_section
{

/**
 * @brief   A service that use the ui/qt/widget/collapsible_section widget.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="ex_collapsible_section::ex_collapsible_section_editor" >
        </service>
   @endcode
 */

class SIGHT_EX_COLLAPSIBLE_SECTION_CLASS_API ex_collapsible_section_editor : public sight::ui::editor
{
public:

    SIGHT_DECLARE_SERVICE(ex_collapsible_section_editor, sight::ui::editor);

    /// Constructor.
    SIGHT_EX_COLLAPSIBLE_SECTION_API ex_collapsible_section_editor() noexcept = default;

    /// Destructor. Destroy UI.
    SIGHT_EX_COLLAPSIBLE_SECTION_API ~ex_collapsible_section_editor() noexcept override = default;

protected:

    /// Initialize the container.
    SIGHT_EX_COLLAPSIBLE_SECTION_API void configuring() override;

    /// Start the service, slot connections, widget initialization.
    SIGHT_EX_COLLAPSIBLE_SECTION_API void starting() override;

    /// Destroy the service and the container.
    SIGHT_EX_COLLAPSIBLE_SECTION_API void stopping() override;

    /// Update the interface.
    SIGHT_EX_COLLAPSIBLE_SECTION_API void updating() override;
};

} // namespace ex_collapsible_section.
