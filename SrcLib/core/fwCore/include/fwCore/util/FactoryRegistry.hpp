/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __FWCORE_UTIL_FACTORYREGISTRY_HPP__
#define __FWCORE_UTIL_FACTORYREGISTRY_HPP__

#include "fwCore/mt/types.hpp"

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace fwCore
{
namespace util
{

/**
 * @brief FactoryRegistryBase is a class used to store factories
 * @note This class is thread safe.
 */
template < typename FACTORY_SIGNATURE, typename KEY_TYPE = std::string,
           typename FACTORY_HOLDER                       = std::function< FACTORY_SIGNATURE > >
class FactoryRegistryBase
{
public:
    typedef FACTORY_SIGNATURE FactorySignatureType;
    typedef KEY_TYPE KeyType;

    typedef FACTORY_HOLDER FactoryType;
    typedef std::map< KeyType, FactoryType > RegistryType;
    typedef std::vector<KeyType> KeyVectorType;

    FactoryRegistryBase()
    {
    }

    virtual ~FactoryRegistryBase()
    {
    }

    /**
     * @brief Add a factory to the registry.
     */
    void addFactory(const KeyType& name, FactoryType factory)
    {
        // get exclusive access
        ::fwCore::mt::WriteLock lock(m_mutex);
        m_registry[name] = factory;
    }

    /**
     * @brief returns the factory associated with the key.
     */
    virtual FactoryType getFactory(const KeyType& key) const
    {
        // get shared access
        ::fwCore::mt::ReadLock lock(m_mutex);
        typename RegistryType::const_iterator iter = m_registry.find(key);
        FactoryType factory;
        if(iter != m_registry.end())
        {
            factory = iter->second;
        }
        return factory;
    }

    /**
     * @brief returns the registered factory keys.
     */
    virtual KeyVectorType getFactoryKeys() const
    {
        ::fwCore::mt::ReadLock lock(m_mutex);
        KeyVectorType vectKeys;
        std::transform( m_registry.begin(), m_registry.end(),
                        std::back_inserter(vectKeys),
                        std::bind(&RegistryType::value_type::first, std::placeholders::_1) );
        return vectKeys;
    }

protected:

    RegistryType m_registry;
    mutable ::fwCore::mt::ReadWriteMutex m_mutex;
};

/**
 * @brief FactoryRegistry is a class used to store factories and create instance object with these factories.
 */
template <typename F, typename KEY_TYPE = std::string, typename FACTORY_HOLDER = std::function< F > >
class FactoryRegistry;

template< typename RETURN_TYPE, typename KEY_TYPE, typename FACTORY_HOLDER >
class FactoryRegistry< RETURN_TYPE (), KEY_TYPE, FACTORY_HOLDER > :
    public FactoryRegistryBase < RETURN_TYPE (), KEY_TYPE >
{
typedef RETURN_TYPE (FactorySignatureType)();
typedef FACTORY_HOLDER FactoryType;
typedef RETURN_TYPE ReturnType;
typedef KEY_TYPE KeyType;

public:

    /**
     * @brief Instantiates an object with the factory associated with the specified key.
     * @return Created instance.
     */
    ReturnType create(const KeyType& key) const
    {
        FactoryType factory = this->getFactory(key);

        if(!factory)
        {
            ReturnType obj;
            return obj;
        }
        return factory();
    }
};

template< typename RETURN_TYPE, typename ARG1_TYPE, typename KEY_TYPE, typename FACTORY_HOLDER >
class FactoryRegistry< RETURN_TYPE (ARG1_TYPE), KEY_TYPE, FACTORY_HOLDER > :
    public FactoryRegistryBase < RETURN_TYPE (ARG1_TYPE), KEY_TYPE >
{
typedef RETURN_TYPE (FactorySignatureType)(ARG1_TYPE);
typedef FACTORY_HOLDER FactoryType;
typedef RETURN_TYPE ReturnType;
typedef ARG1_TYPE Arg1Type;
typedef KEY_TYPE KeyType;

public:

    /**
     * @brief Instantiates an object with the factory associated with the specified key, passing arg1 to the factory.
     * @return Created instance.
     */
    ReturnType create(const KeyType& key, Arg1Type &arg1) const
    {
        FactoryType factory = this->getFactory(key);
        ReturnType obj;
        if(factory)
        {
            obj = factory(arg1);
        }
        return obj;
    }
};

} //namespace util
} //namespace fwCore

#endif /* __FWCORE_UTIL_FACTORYREGISTRY_HPP__ */
