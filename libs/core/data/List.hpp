/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/Object.hpp"

#include <list>

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (List));

namespace sight::data
{

/**
 * @brief This class defines a list of objects.
 *
 * List contains a list of data::Object.
 */
class DATA_CLASS_API List : public Object
{
public:

    SIGHT_DECLARE_CLASS(List, data::Object, data::factory::New<List>);

    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (List));

    typedef std::list<Object::sptr> ContainerType;

    typedef ContainerType::value_type ValueType;
    typedef ContainerType::reference ReferenceType;
    typedef ContainerType::const_reference ConstReferenceType;
    typedef ContainerType::iterator IteratorType;
    typedef ContainerType::const_iterator ConstIteratorType;
    typedef ContainerType::reverse_iterator ReverseIteratorType;
    typedef ContainerType::const_reverse_iterator ConstReverseIteratorType;
    typedef ContainerType::size_type SizeType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API List(data::Object::Key key);

    /// Destructor
    DATA_API virtual ~List();

    /// boost_foreach/stl compatibility
    /// @{
    typedef ContainerType::value_type value_type;
    typedef ContainerType::iterator iterator;
    typedef ContainerType::const_iterator const_iterator;
    typedef ContainerType::reverse_iterator reverse_iterator;
    typedef ContainerType::const_reverse_iterator const_reverse_iterator;
    typedef ContainerType::size_type size_type;

    typedef List Container;

    IteratorType begin();
    IteratorType end();
    ConstIteratorType begin() const;
    ConstIteratorType end() const;

    ReverseIteratorType rbegin();
    ReverseIteratorType rend();
    ConstReverseIteratorType rbegin() const;
    ConstReverseIteratorType rend() const;

    bool empty() const;
    SizeType size() const;

    ReferenceType front();
    ReferenceType back();
    ConstReferenceType front() const;
    ConstReferenceType back() const;
    /// @}

    /// @brief get/set the list of data::Object
    /// @{
    ContainerType& getContainer();
    const ContainerType& getContainer() const;
    void setContainer(const ContainerType& val);
    /// @}

    /// Defines shallow copy
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

protected:

    ContainerType m_container;
};

//-----------------------------------------------------------------------------

inline List::IteratorType List::begin()
{
    return m_container.begin();
}

//-----------------------------------------------------------------------------

inline List::IteratorType List::end()
{
    return m_container.end();
}

//-----------------------------------------------------------------------------

inline List::ConstIteratorType List::begin() const
{
    return m_container.begin();
}

//-----------------------------------------------------------------------------

inline List::ConstIteratorType List::end() const
{
    return m_container.end();
}

//-----------------------------------------------------------------------------

inline List::ReverseIteratorType List::rbegin()
{
    return m_container.rbegin();
}

//-----------------------------------------------------------------------------

inline List::ReverseIteratorType List::rend()
{
    return m_container.rend();
}

//-----------------------------------------------------------------------------

inline List::ConstReverseIteratorType List::rbegin() const
{
    return m_container.rbegin();
}

//-----------------------------------------------------------------------------

inline List::ConstReverseIteratorType List::rend() const
{
    return m_container.rend();
}

//-----------------------------------------------------------------------------

inline bool List::empty() const
{
    return m_container.empty();
}

//-----------------------------------------------------------------------------

inline List::SizeType List::size() const
{
    return m_container.size();
}

//-----------------------------------------------------------------------------

inline List::ReferenceType List::front()
{
    return m_container.front();
}

//-----------------------------------------------------------------------------

inline List::ReferenceType List::back()
{
    return m_container.back();
}

//-----------------------------------------------------------------------------

inline List::ConstReferenceType List::front() const
{
    return m_container.front();
}

//-----------------------------------------------------------------------------

inline List::ConstReferenceType List::back() const
{
    return m_container.back();
}

//-----------------------------------------------------------------------------

inline List::ContainerType& List::getContainer()
{
    return m_container;
}

//-----------------------------------------------------------------------------

inline const List::ContainerType& List::getContainer() const
{
    return m_container;
}

//-----------------------------------------------------------------------------

inline void List::setContainer(const List::ContainerType& val)
{ \
    m_container = val;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
