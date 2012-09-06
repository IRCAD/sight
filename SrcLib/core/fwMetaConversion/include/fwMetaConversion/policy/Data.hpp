/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __FWMETACONVERSION_POLICY_DATA_HPP__
#define __FWMETACONVERSION_POLICY_DATA_HPP__

#include <fwData/Object.hpp>
#include <fwMetaData/Blob.hpp>
#include <fwMetaData/Policy.hpp>
#include <fwData/camp/mapper.hpp>
#include <fwTools/camp/mapper.hpp>

#include "fwMetaConversion/config.hpp"
#include "fwMetaConversion/MetaHelper.hpp"
#include "fwMetaConversion/policy/DataHelper.hpp"

namespace fwMetaConversion {
namespace policy {


class FWMETACONVERSION_CLASS_API Data : public ::fwMetaData::Policy
{
public:
    FWMETACONVERSION_API Data(::fwMetaConversion::MetaHelper& helper);
    FWMETACONVERSION_API virtual void processAttributes(const Attributes& attributes);
    FWMETACONVERSION_API virtual void processMetaInfos(const ::fwMetaData::Object::MetaInfos& metaInfos);

    ::fwData::Object::sptr getObject(){ return m_object;};
private:


    void processAttribut(const std::string& name, const Attributes::mapped_type& attribut);
    ::fwData::Object::sptr m_object;
    DataHelper m_helper;
};


}  // namespace policy
}  // namespace fwMetaConversion

#endif /* __FWMETACONVERSION_POLICY_DATA_HPP__ */
