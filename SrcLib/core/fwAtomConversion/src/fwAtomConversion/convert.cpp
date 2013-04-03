/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtomConversion/convert.hpp"
#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/AtomVisitor.hpp"

namespace fwAtomConversion
{

::fwAtoms::Object::sptr convert( ::fwData::Object::sptr data )
{
    DataVisitor::AtomCacheType cache;
    return DataVisitor::convert( data, cache );
}

::fwData::Object::sptr convert( ::fwAtoms::Object::sptr atom )
{
    AtomVisitor::DataCacheType cache;
    return AtomVisitor::convert( atom, cache );
}

} /// end namespace fwAtomConversion
