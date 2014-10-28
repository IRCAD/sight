/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <fwData/Vector.hpp>
#include <fwData/Composite.hpp>

#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Equipment.hpp>

#include <fwTools/UUID.hpp>
#include <fwTools/dateAndTime.hpp>

#include <fwDataCamp/getObject.hpp>

#include "fwActivities/builder/registry/macros.hpp"
#include "fwActivities/builder/ActivitySeries.hpp"

namespace fwActivities
{
namespace builder
{

fwActivitiesBuilderRegisterMacro(::fwActivities::builder::ActivitySeries, "::fwActivities::builder::ActivitySeries");

//-----------------------------------------------------------------------------

ActivitySeries::ActivitySeries(::fwActivities::IBuilder::Key key)
{}

//-----------------------------------------------------------------------------

ActivitySeries::~ActivitySeries()
{}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr vectorToComposite(::fwData::Vector::sptr vector,
                                            const ::fwActivities::registry::ActivityRequirement &req)
{
    namespace ActReg = ::fwActivities::registry;
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();

    OSLM_ASSERT("Each possible items in requirement need to have a maching key", req.keys.size() >= req.maxOccurs );

    ActReg::ActivityRequirement::KeyType::const_iterator iter = req.keys.begin();

    BOOST_FOREACH(const ::fwData::Object::sptr &obj, *vector)
    {
        const ActReg::ActivityRequirementKey &keyTag = (*iter++);
        if(keyTag.path.empty())
        {
            (*composite)[keyTag.key] = obj;
        }
        else
        {
            (*composite)[keyTag.key] = ::fwDataCamp::getObject( obj, keyTag.path );
        }
    }

    return composite;
}

//-----------------------------------------------------------------------------

::fwMedData::ActivitySeries::sptr ActivitySeries::buildData(
        const ::fwActivities::registry::ActivityInfo& activityInfo,
        ::fwData::Vector::sptr currentSelection ) const
{
    ::fwMedData::ActivitySeries::sptr actSeries = ::fwMedData::ActivitySeries::New();

    ::fwMedData::Series::sptr series;
    BOOST_FOREACH(const ::fwData::Object::sptr& obj, *currentSelection)
    {
        series = ::fwMedData::Series::dynamicCast(obj);
        if(series)
        {
            break;
        }
    }

    if(series)
    {
        actSeries->setPatient( ::fwData::Object::copy(series->getPatient()) );
        actSeries->setStudy( ::fwData::Object::copy(series->getStudy()) );
        actSeries->setEquipment( ::fwData::Object::copy(series->getEquipment()) );
    }

    actSeries->setModality("OT");
    actSeries->setInstanceUID("fwActivities." + ::fwTools::UUID::generateUUID() );

    ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    actSeries->setDate(::fwTools::getDate(now));
    actSeries->setTime(::fwTools::getTime(now));

    actSeries->setActivityConfigId(activityInfo.id);
    ::fwData::Composite::sptr data = actSeries->getData();

    namespace ActReg = ::fwActivities::registry;

    ActReg::ActivityInfo::RequirementsType reqVect = activityInfo.requirements;
    BOOST_FOREACH(const ActReg::ActivityRequirement& req, reqVect)
    {
        ::fwData::Vector::sptr vectorType = this->getType(currentSelection, req.type);
        // param is optional (minOccurs==0) or required (minOccurs==1), but is single (maxOccurs == 1)
        if(req.maxOccurs == 1 && req.minOccurs == 1)
        {
            OSLM_ASSERT("No param name "<<req.name<<" with type "<<req.type, !vectorType->empty());
            (*data)[req.name] = (*vectorType)[0];
        }
        else //optional single parameter
        {
            (*data)[req.name] = vectorToComposite(vectorType, req);
        }
    }

    return actSeries;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwActivities



