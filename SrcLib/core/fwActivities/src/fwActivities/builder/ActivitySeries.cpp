/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwActivities/builder/ActivitySeries.hpp"

#include "fwActivities/builder/registry/macros.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Vector.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/UUID.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace fwActivities
{
namespace builder
{

fwActivitiesBuilderRegisterMacro(::fwActivities::builder::ActivitySeries, "::fwActivities::builder::ActivitySeries");

//-----------------------------------------------------------------------------

ActivitySeries::ActivitySeries(::fwActivities::IBuilder::Key key)
{
}

//-----------------------------------------------------------------------------

ActivitySeries::~ActivitySeries()
{
}

//-----------------------------------------------------------------------------

::fwData::Composite::sptr vectorToComposite(const ::fwData::Vector::csptr& vector,
                                            const ::fwActivities::registry::ActivityRequirement& req)
{
    namespace ActReg                    = ::fwActivities::registry;
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();

    OSLM_ASSERT("Each possible items in requirement need to have a matching key", req.keys.size() >= req.maxOccurs );

    ActReg::ActivityRequirement::KeyType::const_iterator iter = req.keys.begin();

    for(const ::fwData::Object::sptr& obj :  *vector)
    {
        const ActReg::ActivityRequirementKey& keyTag = (*iter++);
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
    const ::fwData::Vector::csptr& currentSelection ) const
{
    ::fwMedData::ActivitySeries::sptr actSeries = ::fwMedData::ActivitySeries::New();

    ::fwMedData::Series::sptr series;
    for(const ::fwData::Object::sptr& obj :  *currentSelection)
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
    for(const ActReg::ActivityRequirement& req :  reqVect)
    {
        ::fwData::Vector::sptr vectorType = this->getType(currentSelection, req.type);
        // param is optional (minOccurs==0) or required (minOccurs==1), but is single (maxOccurs == 1)
        if(req.maxOccurs == 1 && req.minOccurs == 1)
        {
            OSLM_ASSERT("No param name "<<req.name<<" with type "<<req.type, !vectorType->empty());
            (*data)[req.name] = (*vectorType)[0];
        }
        else
        {
            SLM_ASSERT("Unknown specified container: '"+req.container+"'.",
                       req.container.empty() ||
                       req.container == "vector" ||
                       req.container == "composite");
            if(req.container == "vector")
            {
                (*data)[req.name] = vectorType;
            }
            else if(req.container == "composite" || req.container.empty())
            {
                (*data)[req.name] = vectorToComposite(vectorType, req);
            }
        }
    }

    return actSeries;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwActivities
