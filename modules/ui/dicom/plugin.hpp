/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <sight/modules/ui/dicom/config.hpp>

#include <core/runtime/plugin.hpp>

namespace sight::module::ui::dicom
{

struct plugin : public core::runtime::plugin
{
    /// Constructor
    plugin();

    // Overrides
    SIGHT_MODULE_UI_DICOM_API void start() final;

    // Overrides
    SIGHT_MODULE_UI_DICOM_API void stop() noexcept final;
};

} // namespace sight::module::ui::dicom
