#include "fwMetaData/Sequence.hpp"

namespace fwMetaData
{

void Sequence::append(Base::sptr value)
{
    m_value.push_back(value);
}

Sequence::Iterator Sequence::begin()

{
    return m_value.begin();
}

Sequence::cIterator Sequence::cBegin() const
{
    return m_value.begin();
}

Sequence::Iterator Sequence::end()
{
    return m_value.end();
}

Sequence::cIterator Sequence::cEnd() const
{
    return m_value.end();
}

const Sequence::SequenceType& Sequence::getValue()
{
    return m_value;
}

bool Sequence::isEmpty() const
{
    return m_value.empty();
}

::fwMetaData::Base::sptr Sequence::operator[](unsigned int index)
{
    return m_value[index];
}

void Sequence::set(unsigned int pos, ::fwMetaData::Base::sptr value)
{
    if(pos < m_value.size())
    {
        m_value.assign(pos, value);
    }

}

//------------------------------------------------------------------------------

Base::sptr Sequence::clone()
{
    return this->getSptr();
}
}
