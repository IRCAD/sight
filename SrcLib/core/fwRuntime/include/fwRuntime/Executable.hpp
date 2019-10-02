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

#include "fwRuntime/config.hpp"
#include "fwRuntime/IExecutable.hpp"

namespace fwRuntime
{

/**
 * @brief   Provides a default ::fwRuntime::IExecutable interface implementation.
 *
 * This class provides a default implementation of the ::fwRuntime::IExecutable
 * interface and is intended to be used as base class for user defined executable
 * classes being exported by bundles.
 *
 */
struct FWRUNTIME_CLASS_API Executable : public IExecutable
{

    /**
     * @brief   Retrieves the executable's initialization data.
     *
     * @return  a shared pointer to the configuration element being the initialization data of the executable
     *          or null when none
     */
    FWRUNTIME_API const std::shared_ptr< ConfigurationElement > getInitializationData() const noexcept;

    /**
     * @name    Overrides.
     */
    /// @{

    FWRUNTIME_API virtual std::shared_ptr<Bundle> getBundle() const noexcept;

    FWRUNTIME_API virtual void setInitializationData( const std::shared_ptr< ConfigurationElement > configuration )
    noexcept;

    /// @}

    protected:

        // Overrides
        FWRUNTIME_API virtual void setBundle( std::shared_ptr< Bundle > bundle );

    private:

        /**
         * @brief   a pointer to the bundle that owns the executable instance
         */
        std::shared_ptr< Bundle > m_bundle;

        /**
         * @brief   the initialization data of the executable instance
         */
        std::shared_ptr< ConfigurationElement > m_initializationData;

};

} // namespace fwRuntime
