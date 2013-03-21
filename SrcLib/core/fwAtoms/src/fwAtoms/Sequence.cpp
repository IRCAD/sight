/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwAtoms/registry/macros.hpp"
#include "fwAtoms/Sequence.hpp"

fwCampImplementMacro((fwAtoms)(Sequence))
{
    typedef ::fwAtoms::Sequence::SequenceType& (::fwAtoms::Sequence::* AccessorType) ();
    AccessorType get = &::fwAtoms::Sequence::getValue;

    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwAtoms")
        .base< ::fwAtoms::Base >()
        .property("value", get);
}

fwAtomsRegisterMacro( ::fwAtoms::Sequence );

namespace fwAtoms
{

void Sequence::append(Base::sptr value)
{
    m_value.push_back(value);
}

//------------------------------------------------------------------------------

Sequence::IteratorType Sequence::begin()
{
    return m_value.begin();
}

//------------------------------------------------------------------------------

Sequence::ConstIteratorType Sequence::begin() const
{
    return m_value.begin();
}

//------------------------------------------------------------------------------

Sequence::IteratorType Sequence::end()
{
    return m_value.end();
}

//------------------------------------------------------------------------------

Sequence::ConstIteratorType Sequence::end() const
{
    return m_value.end();
}

//------------------------------------------------------------------------------

const Sequence::SequenceType& Sequence::getValue() const
{
    return m_value;
}

//------------------------------------------------------------------------------

Sequence::SequenceType& Sequence::getValue()
{
    return m_value;
}

//------------------------------------------------------------------------------

bool Sequence::isEmpty() const
{
    return m_value.empty();
}

//------------------------------------------------------------------------------

::fwAtoms::Base::sptr Sequence::operator[](unsigned int index)
{
    return m_value[index];
}

//------------------------------------------------------------------------------

void Sequence::set(unsigned int pos, ::fwAtoms::Base::sptr value)
{
    if(pos < m_value.size())
    {
        m_value.assign(pos, value);
    }
}

//------------------------------------------------------------------------------

Base::sptr Sequence::clone() const
{
    Sequence::sptr cloneSeq = Sequence::New();
    SequenceType& valueVect = cloneSeq->getValue();
    valueVect.resize(m_value.size());
    BOOST_FOREACH(const ValueType &elem, m_value)
    {
        valueVect.push_back( elem->clone() );
    }
    return cloneSeq;
}

}
