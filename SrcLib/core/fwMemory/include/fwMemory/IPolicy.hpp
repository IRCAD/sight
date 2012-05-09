/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWMEMORY_IPOLICY_HPP_
#define FWMEMORY_IPOLICY_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <fwTools/IBufferManager.hpp>

#include "fwMemory/BufferInfo.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{

/**
 * @brief Defines the memory dump policy interface
 */
class FWMEMORY_CLASS_API IPolicy
{
public :

    typedef SPTR(IPolicy) sptr;
    typedef std::vector<std::string> ParamNamesType;



    virtual void allocationRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size ) = 0 ;
    virtual void setRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size ) = 0 ;
    virtual void reallocateRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType newSize ) = 0 ;
    virtual void destroyRequest( BufferInfo &info, void **buffer ) = 0 ;
    virtual void lockRequest( BufferInfo &info, void **buffer ) = 0 ;
    virtual void unlockRequest( BufferInfo &info, void **buffer ) = 0 ;

    virtual void dumpSuccess( BufferInfo &info, void **buffer ) = 0 ;
    virtual void restoreSuccess( BufferInfo &info, void **buffer ) = 0 ;

    virtual void setManager(::fwTools::IBufferManager::sptr manager) = 0;

    virtual void refresh() = 0;

    virtual bool setParam(const std::string &name, const std::string &value) = 0;
    virtual ParamNamesType getParamNames() const = 0;

    static IPolicy::sptr createPolicy(std::string name);

    template< typename POLICY >
    struct Register
    {
        Register(const std::string &name)
        {
            s_factories[name] = &POLICY::New;
        }
    };

protected:
    typedef boost::function< IPolicy::sptr () > PolicyFactoryType;

    typedef std::map<std::string, PolicyFactoryType> FactoriesMap;
    static FactoriesMap s_factories;

};

} // namespace fwMemory

#endif // FWMEMORY_IPOLICY_HPP_

