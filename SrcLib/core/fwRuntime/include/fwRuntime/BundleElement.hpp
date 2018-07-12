/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRuntime/config.hpp"

#include <memory>

namespace fwRuntime
{
struct Bundle;
}

namespace fwRuntime
{

/**
 * @brief   Implements the base class for all element managed by a bundle.
 */
struct BundleElement
{

    /**
     * @brief   Retrieves the bundle that owns the extension.
     *
     * @return  a pointer to a bundle instance
     */
    FWRUNTIME_API std::shared_ptr<Bundle> getBundle() const;

    /**
     * @brief   Tells if the element is enabled or not.
     *
     * @remark  The element will asks its bundle enable state. if the bundle
     *          element is enabled then the program verifies if the bundle
     *          element is enabled too.
     *
     * @pre     The bundle must not be null.
     *
     * @return  true or false
     */
    FWRUNTIME_API bool isEnable() const;

    /**
     * @brief       Set if the element is enabled or not.
     *
     * @see         isEnable
     *
     * @param[in]   enable To know if bundle element is enabled
     */
    FWRUNTIME_API void setEnable(const bool enable);

    protected:

        /**
         * @brief   Constructor
         *
         * @remark  The instance will try to guess the bundle that is being loaded and
         *          consider that bundle as its owning bundle
         *
         * @post    The bundle isn't null.
         * @see     ::fwRuntime::Bundle::getLoadingBundle()
         */
        BundleElement();

        /**
         * @brief       Constructor
         *
         * @post        The bundle isn't null.
         * @param[in]   bundle  a shared pointer to the managing bundle
         */
        BundleElement( std::shared_ptr< Bundle > bundle );

    private:

        std::weak_ptr< Bundle >  m_bundle;///< Weak pointer to the bundle managing the element
        bool m_enable;                          ///< to know if bundle element is enabled

};

} // namespace fwRuntime
