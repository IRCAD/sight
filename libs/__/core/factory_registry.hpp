/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#pragma once

#include "core/mt/types.hpp"

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace sight::core
{

/**
 * @brief FactoryRegistryBase is a class used to store factories
 * @note This class is thread safe.
 */
template<typename FACTORY_SIGNATURE, typename KEY_TYPE = std::string,
         typename FACTORY_HOLDER = std::function<FACTORY_SIGNATURE> >
class factory_registry_base
{
public:

    typedef FACTORY_SIGNATURE factory_signature_type;
    typedef KEY_TYPE key_type;

    typedef FACTORY_HOLDER factory_type;
    typedef std::map<key_type, factory_type> registry_type;
    typedef std::vector<key_type> key_vector_type;

    factory_registry_base()
    = default;

    virtual ~factory_registry_base()
    = default;

    /**
     * @brief Add a factory to the registry.
     */
    void add_factory(const key_type& _name, factory_type _factory)
    {
        // get exclusive access
        core::mt::write_lock lock(m_mutex);
        m_registry[_name] = _factory;
    }

    /**
     * @brief returns the factory associated with the key.
     */
    virtual factory_type get_factory(const key_type& _key) const
    {
        // get shared access
        core::mt::read_lock lock(m_mutex);
        auto iter = m_registry.find(_key);
        factory_type factory;
        if(iter != m_registry.end())
        {
            factory = iter->second;
        }

        return factory;
    }

    /**
     * @brief returns the registered factory keys.
     */
    virtual key_vector_type get_factory_keys() const
    {
        core::mt::read_lock lock(m_mutex);
        key_vector_type vect_keys;
        std::transform(
            m_registry.begin(),
            m_registry.end(),
            std::back_inserter(vect_keys),
            [](const auto& _e){return _e.first;});
        return vect_keys;
    }

protected:

    registry_type m_registry;
    mutable core::mt::read_write_mutex m_mutex;
};

/**
 * @brief FactoryRegistry is a class used to store factories and create instance object with these factories.
 */
template<typename F, typename KEY_TYPE = std::string, typename FACTORY_HOLDER = std::function<F> >
class factory_registry;

template<typename RETURN_TYPE, typename KEY_TYPE, typename FACTORY_HOLDER>
class factory_registry<RETURN_TYPE(), KEY_TYPE, FACTORY_HOLDER>:
    public factory_registry_base<RETURN_TYPE(),
                                 KEY_TYPE>
{
typedef RETURN_TYPE (factory_signature_type)();
typedef FACTORY_HOLDER factory_type;
typedef RETURN_TYPE return_type;
typedef KEY_TYPE key_type;

public:

    /**
     * @brief Instantiates an object with the factory associated with the specified key.
     * @return Created instance.
     */
    return_type create(const key_type& _key) const
    {
        factory_type factory = this->get_factory(_key);

        if(!factory)
        {
            return_type obj;
            return obj;
        }

        return factory();
    }
};

template<typename RETURN_TYPE, typename ARG1_TYPE, typename KEY_TYPE, typename FACTORY_HOLDER>
class factory_registry<RETURN_TYPE(ARG1_TYPE), KEY_TYPE, FACTORY_HOLDER>:
    public factory_registry_base<RETURN_TYPE(ARG1_TYPE),
                                 KEY_TYPE>
{
typedef RETURN_TYPE (factory_signature_type)(ARG1_TYPE);
typedef FACTORY_HOLDER factory_type;
typedef RETURN_TYPE return_type;
typedef ARG1_TYPE arg1type;
typedef KEY_TYPE key_type;

public:

    /**
     * @brief Instantiates an object with the factory associated with the specified key, passing arg1 to the factory.
     * @return Created instance.
     */
    return_type create(const key_type& _key, arg1type& _arg1) const
    {
        factory_type factory = this->get_factory(_key);
        return_type obj;
        if(factory)
        {
            obj = factory(_arg1);
        }

        return obj;
    }
};

} //namespace sight::core
