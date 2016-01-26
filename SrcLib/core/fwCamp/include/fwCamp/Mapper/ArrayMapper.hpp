/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCAMP_MAPPER_ARRAYMAPPER_HPP__
#define __FWCAMP_MAPPER_ARRAYMAPPER_HPP__

#include <set>

#include <camp/valuemapper.hpp>
#include <camp/type.hpp>
#include <camp/camptype.hpp>

namespace camp_ext
{
/*
 * Specialization of ArrayMapper for std::set
 */
template <typename T>
struct ArrayMapper<std::set<T> >
{
    typedef T ElementType;

    static bool dynamic()
    {
        return true;
    }

    static std::size_t size(const std::set<T>& arr)
    {
        return arr.size();
    }

    static const T& get(const std::set<T>& arr, std::size_t index)
    {
        typename std::set<T>::const_iterator cIt = arr.begin();
        for(int i = 0; i < index; i++)
        {
            ++cIt;
        }
        return *cIt;
    }

    static void set(std::set<T>& arr, std::size_t index, const T& value)
    {
    }

    static void insert(std::set<T>& arr, std::size_t before, const T& value)
    {
        arr.insert(value);
    }

    static void remove(std::set<T>& arr, std::size_t index)
    {
        typename std::set<T>::const_iterator cIt = arr.begin();
        for(int i = 0; i < index; ++i)
        {
            ++cIt;
        }
        arr.erase(cIt);
    }
};


} //camp_ext


#endif /* __FWCAMP_MAPPER_ARRAYMAPPER_HPP__ */
