/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_PLUGIN_HPP_
#define _FWRUNTIME_PLUGIN_HPP_

#include <boost/shared_ptr.hpp>

#include "fwRuntime/config.hpp"
#include "fwRuntime/IPlugin.hpp"



namespace fwRuntime
{



/**
 * @brief       Provides a default plugin implementation.
 * @struct      Plugin
 *
 * This class is intended to be sub-classed. The IPlugin::start and IPlugin::stop
 * function must be implemented.
 *
 * @date        2004-2009
 * @author      IRCAD (Research and Development Team).
 */
struct FWRUNTIME_CLASS_API Plugin : public IPlugin
{

        /**
         * @name        Overrides
         */
        //@{
        FWRUNTIME_API ::boost::shared_ptr<Bundle> getBundle() const throw();
        FWRUNTIME_API virtual void setInitializationData(const ::boost::shared_ptr<ConfigurationElement> configuration) throw();
        //@}


protected:

        // Overrides
        FWRUNTIME_API void setBundle( ::boost::shared_ptr<Bundle> bundle) throw();


private:

        /**
         * @brief       a pointer to the bundle the plugin is attached to
         */
         ::boost::shared_ptr<Bundle> m_bundle;

};


} // namespace fwRuntime


#endif //_FWRUNTIME_PLUGIN_HPP_
