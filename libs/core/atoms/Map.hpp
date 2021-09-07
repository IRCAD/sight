/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "atoms/Base.hpp"
#include "atoms/config.hpp"
#include "atoms/factory/new.hpp"
#include "atoms/Object.hpp"

#include <map>
#include <string>

namespace sight::atoms
{

/**
 *
 * @brief Map is a container for mapping representation.
 *
 * For exemple, this metadata is used to bind :
 * \li std::map
 * \li ::boost::unordered_map
 * \li data::Graph (a part)
 * \li boost graph
 */
class ATOMS_CLASS_API Map : public Base
{
public:

    SIGHT_DECLARE_CLASS(Map, atoms::Base, atoms::factory::New<Map>);

    typedef std::map<std::string, Base::sptr> MapType;

    typedef MapType::key_type KeyType;
    typedef MapType::mapped_type MappedType;
    typedef MapType::value_type ValueType;
    typedef MapType::iterator IteratorType;
    typedef MapType::const_iterator ConstIteratorType;
    typedef MapType::reverse_iterator ReverseIteratorType;
    typedef MapType::const_reverse_iterator ConstReverseIteratorType;
    typedef MapType::size_type SizeType;

    /// boost_foreach/stl compatibility
    /// @{
    typedef MapType::key_type key_type;
    typedef MapType::mapped_type mapped_type;
    typedef MapType::value_type value_type;
    typedef MapType::iterator iterator;
    typedef MapType::const_iterator const_iterator;
    typedef MapType::reverse_iterator reverse_iterator;
    typedef MapType::const_reverse_iterator const_reverse_iterator;
    typedef MapType::size_type size_type;
    /// @}

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Map(atoms::Base::Key)
    {
    }

    /**
     * @brief   Destructor
     */
    virtual ~Map()
    {
    }

    /**
     * @brief Insert a new value in the map.
     * @param key the key, if exist the oldest value is erased.
     * @param value the new value
     */
    std::pair<IteratorType, bool> insert(const std::string& key, const Base::sptr& value)
    {
        return m_value.insert(ValueType(key, value));
    }

    /**
     * @brief retrieve size of map
     * @return the map size
     */
    size_t size() const
    {
        return m_value.size();
    }

    /**
     *@brief Provide an iterator on the first element
     *@return An iterator on the first element, end() if map is empty.
     */
    IteratorType begin()
    {
        return m_value.begin();
    }

    /**
     *@brief Provide a const iterator on the first element
     *@return An iterator on the first element, end() if map is empty.
     */
    ConstIteratorType begin() const
    {
        return m_value.begin();
    }

    /**
     * @brief Return an iterator after the last element in the map
     */
    IteratorType end()
    {
        return m_value.end();
    }

    /**
     * @brief Return a const iterator after the last element in the map
     */
    ConstIteratorType end() const
    {
        return m_value.end();
    }

    //! clear the map
    void clear()
    {
        m_value.clear();
    }

    /**
     * @brief test if the map is empty
     * @return true if empty
     */
    bool empty() const
    {
        return m_value.empty();
    }

    /**
     * @brief returns atom's map
     */
    const MapType& getValue() const
    {
        return m_value;
    }

    /**
     * @brief find an element in the map
     * @param key the key
     * @return an iterator on value pointed by key if exist, else return
     * Map.end()
     */
    MapType::const_iterator find(const std::string& key) const
    {
        return m_value.find(key);
    }

    /**
     * @brief Access to a member with []
     * @param key a key in the map
     * @return the value associated with the key or an empty base if the key
     *         is not in the map.
     */
    Base::sptr operator[](const std::string& key)
    {
        return m_value[key];
    }

    /**
     * @brief Returns a clone object
     */
    ATOMS_API Base::sptr clone() const override;

    /**
     * @brief returns Atom type
     */
    atoms::Base::AtomType type() const override
    {
        return atoms::Base::MAP;
    }

protected:

    MapType m_value;
};

} // namespace sight::atoms
