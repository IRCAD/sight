#ifndef __FWCAMP_CAMP_MAPPROPERTYIMPL_HXX__
#define __FWCAMP_CAMP_MAPPROPERTYIMPL_HXX__

#include <camp/userobject.hpp>

#include <fwCamp/Mapper/ValueMapper.hpp>

namespace camp
{

namespace detail
{

template <typename A>
MapPropertyImpl<A>::MapPropertyImpl(const std::string& name, const A& accessor) :
    camp::MapProperty(name, camp::mapType<ValueType>())
   ,m_accessor(accessor)
{
}

template <typename A>
std::size_t MapPropertyImpl<A>::getSize(const UserObject& object) const
{
    return Mapper::size(map(object));
}
template <typename A>
void MapPropertyImpl<A>::set(const UserObject& object, const Value& key, const Value& value) const
{
    const typename Mapper::KeyType& typedKey = key.to< typename Mapper::KeyType >();
    const typename Mapper::MappedType& typedValue = value.to< typename Mapper::MappedType >();

    Mapper::set(map(object), typedKey  ,typedValue );
}


template <typename A>
MapProperty::ValuePair MapPropertyImpl<A>::getElement(const UserObject& object, std::size_t index ) const
{

    ValueType p (Mapper::get(map(object),index));
    return ValuePair(p.first, p.second);
}

template <typename A>
typename MapPropertyImpl<A>::MapType&
MapPropertyImpl<A>::map(const UserObject& object) const
{
    return m_accessor.get(object.get<typename A::ClassType>());
}

/*
 * Instanciate map properties
 */
template <typename A>
struct PropertyMapper<A, camp::mappingType>
{
    typedef MapPropertyImpl<A> Type;
};


} // namespace detail
}  // namespace camp

#endif /* __FWCAMP_CAMP_MAPPROPERTYIMPL_HXX__*/
