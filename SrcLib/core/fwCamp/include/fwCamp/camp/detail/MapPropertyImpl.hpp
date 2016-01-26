/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCAMP_CAMP_DETAIL_MAPPROPERTYIMPL_HPP__
#define __FWCAMP_CAMP_DETAIL_MAPPROPERTYIMPL_HPP__

#include "fwCamp/camp/MapValueMapper.hpp"
#include "fwCamp/camp/MapMapper.hpp"
#include "fwCamp/camp/MapProperty.hpp"

namespace camp
{
namespace detail
{

template <typename A>
class MapPropertyImpl : public MapProperty
{
public:

    typedef MapProperty::ValuePair ValuePair;
    typedef MapProperty::ObjectPair ObjectPair;

    /**
     * \brief Construct the property
     *
     * \param name Name of the property
     * \param accessor Object used to access the actual C++ property
     */
    MapPropertyImpl(const std::string& name, const A& accessor);

protected:

    /**
     * \see ArrayProperty::getSize
     */
    virtual std::size_t getSize(const UserObject& object) const;

    MapProperty::ValuePair getElement(const UserObject& object, std::size_t index ) const;

    virtual void set(const UserObject& object, const Value& key, const Value& value) const;

private:

    typedef typename boost::remove_reference<typename A::DataType>::type MapType;
    typedef camp_ext::MapMapper<MapType> Mapper;
    typedef typename Mapper::ValueType ValueType;

    /**
     * \brief Retrieve a reference to the array
     *
     * \param object Owner object
     *
     * \return Reference to the underlying array
     */
    MapType& map(const UserObject& object) const;

    A m_accessor; ///< Object used to access the actual C++ property
};


} // namespace detail
}  // namespace camp

#include "fwCamp/camp/detail/MapPropertyImpl.hxx"

#endif /* __FWCAMP_CAMP_DETAIL_MAPPROPERTYIMPL_HPP__ */
