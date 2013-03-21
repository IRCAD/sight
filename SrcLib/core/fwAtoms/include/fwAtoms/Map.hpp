/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_BASE_MAP_HPP_
#define __FWATOMS_BASE_MAP_HPP_


#include <string>
#include <boost/unordered_map.hpp>

#include "fwAtoms/config.hpp"
#include "fwAtoms/Base.hpp"
#include "fwAtoms/Object.hpp"
#include "fwAtoms/factory/new.hpp"

fwCampAutoDeclareMacro((fwAtoms)(Map), FWATOMS_API);

namespace fwAtoms
{

/**
 *
 * @brief Map is a container for mapping representation.
 *
 * For exemple, this metadata is used to bind :
 * \li std::map
 * \li boost::unordered_map
 * \li fwData::Graph (a part)
 * \li boost graph
 */
class FWATOMS_CLASS_API Map : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Map)(::fwAtoms::Base), (()), ::fwAtoms::factory::New< Map >) ;

    typedef std::map<Base::sptr, Base::sptr> MapType;

    typedef MapType::key_type KeyType;
    typedef MapType::mapped_type MappedType;
    typedef MapType::value_type ValueType;
    typedef MapType::iterator IteratorType;
    typedef MapType::const_iterator ConstIteratorType;
    typedef MapType::reverse_iterator ReverseIteratorType;
    typedef MapType::const_reverse_iterator ConstReverseIteratorType;
    typedef MapType::size_type SizeType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    Map(::fwAtoms::Base::Key key)
    {}

    /**
     * @brief   Destructor
     */
    virtual ~Map()
    {}

    /**
     * @brief Insert a new value in the map.
     * @param key the key, if exist the oldest value is erased.
     * @param value the new value
     */
    FWATOMS_API void insert(const Base::sptr key, Base::sptr value);

    virtual bool isMapping() const {return true;};

    /**
     * @brief retrieve size of map
     * @return the map size
     */
    FWATOMS_API size_t getSize() const;

    /**
     *@brief Provide an iterator on the first element
     *@return An iterator on the first element, end() if map is empty.
     */
    FWATOMS_API IteratorType begin();

    /**
     *@brief Provide a const iterator on the first element
     *@return An iterator on the first element, end() if map is empty.
     */
    FWATOMS_API ConstIteratorType begin() const;

    /**
     * @brief Return an iterator after the last element in the map
     */
    FWATOMS_API IteratorType end();

    /**
     * @brief Return a const iterator after the last element in the map
     */
    FWATOMS_API ConstIteratorType end() const;


    /**
     * @brief test if the map is empty
     * @return true if empty
     */
    FWATOMS_API bool isEmpty() const;

    FWATOMS_API MapType& getValue();

    FWATOMS_API const MapType& getValue() const;


    /**
     * @brief find an element in the map
     * @param key the key
     * @return an iterator on value pointed by key if exist, else return
     * Map.end()
     */
    FWATOMS_API MapType::const_iterator find(Base::sptr key) const;


    /**
     * @brief Retrieve map size
     */
    FWATOMS_API size_t size();


    /**
     * @brief Access to a member with []
     * @param key a key in the map
     * @return the value associated with the key or an empty base if the key
     *         is not in the map.
     */
    FWATOMS_API Base::sptr operator[](std::string key);

    FWATOMS_API virtual Base::sptr clone() const;

protected:
     MapType m_value;
};

}


#endif /* __FWATOMS_BASE_MAP_HPP_ */
