/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMCONVERSION_CUSTOM_GRAPH_HPP__
#define __FWATOMCONVERSION_CUSTOM_GRAPH_HPP__


#include "fwAtomConversion/custom/Mapper.hpp"
#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{
namespace custom
{

class FWATOMCONVERSION_CLASS_API Graph : public Mapper
{
    /**
     * Convert fwData to MetaData
     * @param object the origin fwData
     * @param metaHelper this metaHelper is used to compute internal
     * fwDataObject to metaData
     * @return the mapped metaData
     */
    FWATOMCONVERSION_API virtual ::fwAtoms::Object::sptr
                                 toMeta( ::fwData::Object::sptr object,
                                        ::fwAtomConversion::AtomHelper& metaHelper);

    /**
     * Convert MetaData to fwData
     * @param meta the origin metaData
     * @param metaHelper this metaHelper is used to compute internal
     * metaData
     * @return the new fwData
     **/
    FWATOMCONVERSION_API virtual ::fwData::Object::sptr
                                 fromMeta(::fwAtoms::Object::sptr meta,
                                     ::fwAtomConversion::AtomHelper& metaHelper);

};

}

}

#endif /*  __FWATOMCONVERSION_CUSTOM_GRAPH_HPP__*/
