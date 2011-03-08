/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_EXECUTABLE_HPP_
#define _FWRUNTIME_EXECUTABLE_HPP_

#include <boost/shared_ptr.hpp>

#include "fwRuntime/config.hpp"
#include "fwRuntime/IExecutable.hpp"



namespace fwRuntime
{



/**
 * @brief   Provides a default ::fwRuntime::IExecutable interface implementation.
 * @struct  Executable
 *
 * This class provides a default implementation of the ::fwRuntime::IExecutable
 * interface and is intended to be used as base class for user defined executable
 * classes being exported by bundles.
 *
 * @date    2004-2009
 * @author  IRCAD (Research and Development Team).
 */
struct FWRUNTIME_CLASS_API Executable : public IExecutable
{

    /**
     * @brief   Retrieves the executable's initialization data.
     *
     * @return  a shared pointer to the configuration element being the initialization data of the executable
     *          or null when none
     */
    FWRUNTIME_API const ::boost::shared_ptr< ConfigurationElement > getInitializationData() const throw();


    /**
     * @name    Overrides.
     */
    /// @{

    FWRUNTIME_API virtual ::boost::shared_ptr<Bundle> getBundle() const throw();

    FWRUNTIME_API virtual void setInitializationData( const ::boost::shared_ptr< ConfigurationElement > configuration ) throw();

    /// @}


protected:

    // Overrides
    FWRUNTIME_API virtual void setBundle( ::boost::shared_ptr< Bundle > bundle );


private:

    /**
     * @brief   a pointer to the bundle that owns the executable instance
     */
     ::boost::shared_ptr< Bundle > m_bundle;

    /**
     * @brief   the initialization data of the executable instance
     */
     ::boost::shared_ptr< ConfigurationElement > m_initializationData;

};



} // namespace fwRuntime



#endif //_FWRUNTIME_EXECUTABLE_HPP_
