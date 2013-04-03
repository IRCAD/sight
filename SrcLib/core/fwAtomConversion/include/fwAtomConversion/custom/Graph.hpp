/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_CUSTOM_GRAPH_HPP__
#define __FWATOMCONVERSION_CUSTOM_GRAPH_HPP__


#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/custom/Mapper.hpp"

namespace fwAtomConversion
{
namespace custom
{

/// ???
class FWATOMCONVERSION_CLASS_API Graph : public Mapper
{
    /// ???
    FWATOMCONVERSION_API virtual ::fwAtoms::Object::sptr convert ( ::fwData::Object::sptr object,
                                                                   DataVisitor::AtomCacheType & cache );

    /// ???
    FWATOMCONVERSION_API virtual ::fwData::Object::sptr convert ( ::fwAtoms::Object::sptr atom,
                                                                  AtomVisitor::DataCacheType & cache );

};

}
}

#endif /*  __FWATOMCONVERSION_CUSTOM_GRAPH_HPP__*/
