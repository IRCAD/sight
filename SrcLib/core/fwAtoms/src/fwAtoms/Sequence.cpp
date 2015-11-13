/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtoms/registry/macros.hpp"
#include "fwAtoms/Sequence.hpp"

#include <algorithm>
#include <functional>

fwAtomsRegisterMacro( ::fwAtoms::Sequence );

namespace fwAtoms
{

//------------------------------------------------------------------------------

Base::sptr Sequence::clone() const
{
    Sequence::sptr cloneSeq = Sequence::New();
    cloneSeq->m_value.resize(m_value.size());
    std::transform(m_value.begin(), m_value.end(), cloneSeq->m_value.begin(),
                   std::bind(&::fwAtoms::Base::clone, std::placeholders::_1));
    return cloneSeq;
}

}

