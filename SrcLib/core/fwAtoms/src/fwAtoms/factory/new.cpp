/* ***** BEGIN LICENSE BLOCK *****
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtoms/Object.hpp"
#include "fwAtoms/factory/new.hpp"

namespace fwAtoms
{

namespace factory
{

::fwAtoms::Base::sptr New( const ::fwAtoms::registry::KeyType & classname )
{
    return ::fwAtoms::registry::get()->create(classname);
}

} // namespace factory

} // namespace fwAtoms


