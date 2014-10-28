/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include "fwAtoms/registry/macros.hpp"
#include "fwAtoms/Sequence.hpp"

fwAtomsRegisterMacro( ::fwAtoms::Sequence );

namespace fwAtoms
{

//------------------------------------------------------------------------------

Base::sptr Sequence::clone() const
{
    Sequence::sptr cloneSeq = Sequence::New();
    cloneSeq->m_value.resize(m_value.size());
    std::transform(m_value.begin(), m_value.end(), cloneSeq->m_value.begin(), boost::bind(&::fwAtoms::Base::clone, _1));
    return cloneSeq;
}

}

