#ifndef __FWMETADATA_BASE_MAP_HPP_
#define __FWMETADATA_BASE_MAP_HPP_


#include <string>
#include <boost/unordered_map.hpp>

#include "fwMetaData/config.hpp"
#include "fwMetaData/Base.hpp"
#include "fwMetaData/Object.hpp"

fwCampAutoDeclareMacro((fwMetaData)(Map), FWMETADATA_API);

namespace fwMetaData
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
class FWMETADATA_CLASS_API Map : public Base
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Map)(::fwMetaData::Map), (()), new Map) ;
    typedef std::map<Base::sptr, Base::sptr> MapType;
    typedef MapType::iterator Iterator;
    typedef MapType::const_iterator cIterator;


    /**
     * @brief Insert a new value in the map.
     * @param key the key, if exist the oldest value is erased.
     * @param value the new value
     */
    FWMETADATA_API void insert(const Base::sptr key, Base::sptr value);

    FWMETADATA_API virtual bool isMapping() const {return true;};

    /**
     * @brief retrieve size of map
     * @return the map size
     */
    FWMETADATA_API unsigned int getSize();

    /**
     *@brief Provide an iterator on the first element
     *@return An iterator on the first element, end() if map is empty.
     */
    FWMETADATA_API Iterator  begin();

    /**
     *@brief Provide a const iterator on the first element
     *@return An iterator on the first element, cEnd() if map is empty.
     */
    FWMETADATA_API cIterator cBegin() const;

    /**
     * @brief Return an iterator after the last element in the map
     */
    FWMETADATA_API Iterator  end();

    /**
     * @brief Return a const iterator after the last element in the map
     */
    FWMETADATA_API cIterator cEnd() const;


    /**
     * @brief test if the map is empty
     * @return true if empty
     */
    FWMETADATA_API bool isEmpty() const;

    //FWMETADATA_API const MapType& getValue() const;
    FWMETADATA_API MapType& getValue();


    /**
     * @brief find an element in the map
     * @param key the key
     * @return an iterator on value pointed by key if exist, else return
     * Map.cEnd()
     */
    FWMETADATA_API MapType::const_iterator find(Base::sptr key) const;


    /**
     * @brief Retrieve map size
     */
    FWMETADATA_API size_t size();


    /**
     * @brief Access to a member with []
     * @param key a key in the map
     * @return the value associated with the key or an empty base if the key
     *         is not in the map.
     */
    FWMETADATA_API Base::sptr operator[](std::string key);

    FWMETADATA_API virtual Base::sptr clone();
protected:
     Map(){};
     MapType m_value;
};

}


#endif /* __FWMETADATA_BASE_MAP_HPP_ */
