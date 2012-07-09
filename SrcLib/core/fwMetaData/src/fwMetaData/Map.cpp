#include "fwTools/Factory.hpp"
#include "fwMetaData/Map.hpp"

namespace fwMetaData
{

void Map::insert(const Base::sptr key, Base::sptr value)
{
    m_value[key] = value;
}

unsigned int Map::getSize()
{
    return m_value.size();
}

Map::Iterator Map::begin()

{
    return m_value.begin();
}

Map::cIterator Map::cBegin() const
{
    return m_value.begin();
}

Map::Iterator Map::end()
{
    return m_value.end();
}

Map::cIterator Map::cEnd() const
{
    return m_value.end();
}

const Map::MapType& Map::getValue() const
{
    return m_value;
}

Map::MapType& Map::getValue()
{
    return m_value;
}

bool Map::isEmpty() const
{
    return m_value.empty();
}

Base::sptr Map::operator[](std::string index)
{
    MapType::const_iterator cIt = m_value.begin();
    Base::sptr result;

    for(;cIt != m_value.end(); ++cIt)
    {
        if(cIt->first->getString() == index)
        {
            result = cIt->second;
            break;
        }
    }

    return result;
}



//------------------------------------------------------------------------------

Base::sptr Map::clone()
{
    return this->getSptr();
}


Map::MapType::const_iterator Map::find(Base::sptr key) const
{
    return m_value.find(key);

}

size_t Map::size()
{
    return m_value.size();
}


}
