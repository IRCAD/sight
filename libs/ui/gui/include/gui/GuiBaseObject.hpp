/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "gui/config.hpp"
#include "gui/factory/new.hpp"
#include "gui/registry/detail.hpp"

#include <core/base.hpp>

namespace sight::gui
{

/**
 * @brief Base class for all fwGui's classes
 */
class GUI_CLASS_API GuiBaseObject : public ::sight::core::BaseObject
{

public:

    typedef ::sight::gui::factory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            gui::registry::get()->addFactory(functorKey, &gui::factory::New<T>);
        }
    };

    fwCoreClassMacro(GuiBaseObject, core::BaseObject)

protected:
    /**
     * @name Constructor/Destructor
     * @{ */

    GuiBaseObject()
    {
    }
    virtual ~GuiBaseObject()
    {
    }

    /**  @} */

};

} // namespace sight::gui
