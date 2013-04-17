/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/Study.hpp"

fwDataRegisterMacro( ::fwData::Study );

namespace fwData
{

//------------------------------------------------------------------------------

Study::Study(::fwData::Object::Key key) :
    m_sHospital(""),
    m_sModality(""),
    m_sAcquisitionZone(""),
    m_sRISId(""),
    m_sUID(""),
    m_i32DbID(-1)
{}

//------------------------------------------------------------------------------

Study::~Study()
{}

//------------------------------------------------------------------------------

void Study::shallowCopy(const Object::csptr &_source )
{
    Study::csptr other = Study::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_sHospital = other->m_sHospital;
    m_sModality = other->m_sModality;
    m_sAcquisitionZone = other->m_sAcquisitionZone;
    m_sRISId = other->m_sRISId;
    m_sUID = other->m_sUID;
    m_i32DbID = other->m_i32DbID;
    m_date = other->m_date;
    m_time = other->m_time;
    m_description = other->m_description;
    m_attrAcquisitions = other->m_attrAcquisitions;
}

//------------------------------------------------------------------------------

void Study::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    Study::csptr other = Study::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
    m_sHospital = other->m_sHospital;
    m_sModality = other->m_sModality;
    m_sAcquisitionZone = other->m_sAcquisitionZone;
    m_sRISId = other->m_sRISId;
    m_sUID = other->m_sUID;
    m_i32DbID = other->m_i32DbID;
    m_date = other->m_date;
    m_time = other->m_time;
    m_description = other->m_description;
    m_attrAcquisitions.clear();
    m_attrAcquisitions.reserve(other->m_attrAcquisitions.size());
    BOOST_FOREACH(const AcquisitionContainerType::value_type &obj, other->m_attrAcquisitions)
    {
        m_attrAcquisitions.push_back( ::fwData::Object::copy(obj, cache) );
    }
}

//------------------------------------------------------------------------------

Study::AcquisitionContainerType::size_type  Study::getNumberOfAcquisitions() const
{
    return m_attrAcquisitions.size();
}

//------------------------------------------------------------------------------

void Study::addAcquisition( ::fwData::Acquisition::sptr acquisition )
{
    m_attrAcquisitions.push_back( acquisition );
}

//------------------------------------------------------------------------------

} // namespace fwData

