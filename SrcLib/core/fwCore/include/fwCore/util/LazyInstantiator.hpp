/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCORE_UTIL_LAZYINSTANTIATOR_HPP__
#define __FWCORE_UTIL_LAZYINSTANTIATOR_HPP__

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/once.hpp>
#include <boost/noncopyable.hpp>

#include "fwCore/macros.hpp"

namespace fwCore
{
namespace util
{

struct LazyInstantiatorDefaultTag {};

/**
 * @brief A base class for lazy instantiation, constructed on first demand.
 * @tparam INSTANCE_TYPE Type of object to instantiate.
 * @tparam TAG Tag used to allow several LazyInstantiator for one INSTANCE_TYPE.
 */
template< typename INSTANCE_TYPE, typename TAG = LazyInstantiatorDefaultTag >
class LazyInstantiator : ::boost::noncopyable
{
public:

    typedef INSTANCE_TYPE InstanceType;
    typedef TAG TagType;
    typedef ::boost::shared_ptr<InstanceType> InstanceSptrType;

    /// Returns the singleton instance. This method is thread safe.
    static InstanceSptrType getInstance()
    {
        static ::boost::once_flag flag = BOOST_ONCE_INIT;
        ::boost::call_once(&initInstance, flag);
        return instance();
    }

protected:

    /// Initializes the singleton instance
    static void initInstance()
    {
        instance();
    }

    /// Initializes once and returns the instance (not thread-safe)
    static InstanceSptrType instance()
    {
        static InstanceSptrType s_instance;
        if(!s_instance)
        {
            s_instance = ::boost::make_shared< InstanceType >();
        }
        return s_instance;
    }
};

} //namespace util
} //namespace fwCore

#endif /* __FWCORE_UTIL_LAZYINSTANTIATOR_HPP__ */
