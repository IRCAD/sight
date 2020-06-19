/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGui/config.hpp"
#include "fwGui/factory/new.hpp"
#include "fwGui/registry/detail.hpp"

#include <fwCore/base.hpp>

namespace fwGui
{

/**
 * @brief Base class for all fwGui's classes
 */
class FWGUI_CLASS_API GuiBaseObject : public ::fwCore::BaseObject
{

public:

    typedef ::fwGui::factory::Key Key;

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
            ::fwGui::registry::get()->addFactory(functorKey, &::fwGui::factory::New<T>);
        }
    };

    fwCoreClassMacro(GuiBaseObject, ::fwCore::BaseObject);

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

} // namespace fwGui
