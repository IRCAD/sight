/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __FWATOMCONVERSION_POLICY_DATA_HPP__
#define __FWATOMCONVERSION_POLICY_DATA_HPP__

#include <fwData/Object.hpp>
#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Policy.hpp>
#include <fwData/camp/mapper.hpp>
#include <fwTools/camp/mapper.hpp>

#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/AtomHelper.hpp"
#include "fwAtomConversion/policy/DataHelper.hpp"

namespace fwAtomConversion
{
namespace policy
{


class FWATOMCONVERSION_CLASS_API Data : public ::fwAtoms::Policy
{
public:
    FWATOMCONVERSION_API Data(::fwAtomConversion::AtomHelper& helper);
    FWATOMCONVERSION_API virtual void processAttributes(const Attributes& attributes);
    FWATOMCONVERSION_API virtual void processMetaInfos(const ::fwAtoms::Object::MetaInfos& metaInfos);

    ::fwData::Object::sptr getObject(){ return m_object;};
private:

    void processAttribut(const std::string& name, const Attributes::mapped_type& attribut);
    ::fwData::Object::sptr m_object;
    DataHelper m_helper;
};


}  // namespace policy
}  // namespace fwAtomConversion

#endif /* __FWATOMCONVERSION_POLICY_DATA_HPP__ */
