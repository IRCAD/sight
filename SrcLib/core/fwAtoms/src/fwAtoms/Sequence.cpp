/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtoms/Sequence.hpp"

fwCampImplementMacro((fwAtoms)(Sequence))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwAtoms")
        .base< ::fwAtoms::Base >()
        .property("value", &::fwAtoms::Sequence::getValue);
}

namespace fwAtoms
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

//const Sequence::SequenceType& Sequence::getValue()
Sequence::SequenceType& Sequence::getValue()
{
    return m_value;
}

bool Sequence::isEmpty() const
{
    return m_value.empty();
}

::fwAtoms::Base::sptr Sequence::operator[](unsigned int index)
{
    return m_value[index];
}

void Sequence::set(unsigned int pos, ::fwAtoms::Base::sptr value)
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
