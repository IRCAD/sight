/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwRuntime/config.hpp"

#include <memory>

namespace fwRuntime
{
class Module;
}

namespace fwRuntime
{

/**
 * @brief   Implements the base class for all element managed by a module.
 */
class FWRUNTIME_CLASS_API ModuleElement
{
public:
    virtual ~ModuleElement() = default;

    /**
     * @brief   Retrieves the module that owns the extension.
     *
     * @return  a pointer to a module instance
     */
    [[deprecated]] FWRUNTIME_API std::shared_ptr<Module> getBundle() const;

    /**
     * @brief   Retrieves the module that owns the extension.
     *
     * @return  a pointer to a module instance
     */
    FWRUNTIME_API std::shared_ptr<Module> getModule() const;

    /**
     * @brief   Tells if the element is enabled or not.
     *
     * @remark  The element will asks its module enable state. if the module
     *          element is enabled then the program verifies if the module
     *          element is enabled too.
     *
     * @pre     The module must not be null.
     *
     * @return  true or false
     */
    FWRUNTIME_API bool isEnable() const;

    /**
     * @brief       Set if the element is enabled or not.
     *
     * @see         isEnable
     *
     * @param[in]   enable To know if module element is enabled
     */
    FWRUNTIME_API void setEnable(const bool enable);

protected:

    /**
     * @brief   Constructor
     *
     * @remark  The instance will try to guess the module that is being loaded and
     *          consider that module as its owning module
     *
     * @post    The module isn't null.
     * @see     ::fwRuntime::Module::getLoadingmodule()
     */
    FWRUNTIME_API ModuleElement();

    /**
     * @brief       Constructor
     *
     * @post        The module isn't null.
     * @param[in]   module  a shared pointer to the managing module
     */
    FWRUNTIME_API ModuleElement( std::shared_ptr< Module > module );

private:

    std::weak_ptr< Module > m_module;    ///< Weak pointer to the module managing the element
    bool m_enable;                              ///< to know if module element is enabled

};

} // namespace fwRuntime
