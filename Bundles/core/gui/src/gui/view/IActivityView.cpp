/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "gui/view/IActivityView.hpp"

#include <fwActivities/IActivityValidator.hpp>
#include <fwActivities/IValidator.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/UUID.hpp>

namespace gui
{
namespace view
{

const ::fwCom::Slots::SlotKeyType IActivityView::s_LAUNCH_ACTIVITY_SLOT        = "launchActivity";
const ::fwCom::Slots::SlotKeyType IActivityView::s_LAUNCH_ACTIVITY_SERIES_SLOT = "launchActivitySeries";

//-----------------------------------------------------------------------------

IActivityView::IActivityView()
{
    newSlot(s_LAUNCH_ACTIVITY_SLOT, &IActivityView::launchActivity, this);
    newSlot(s_LAUNCH_ACTIVITY_SERIES_SLOT, &IActivityView::launchActivitySeries, this);
}

//-----------------------------------------------------------------------------

IActivityView::~IActivityView()
{
}

//------------------------------------------------------------------------------

void IActivityView::configuring() throw(fwTools::Failed)
{
    this->::fwGui::IGuiContainerSrv::initialize();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;

    ConfigType activityConfig = m_configuration->findConfigurationElement("mainActivity");
    if (activityConfig)
    {
        const std::string id = activityConfig->getAttributeValue("id");
        SLM_ASSERT("main activity 'id' must be defined", !id.empty());
        m_mainActivityId = id;
    }

    ConfigType config = m_configuration->findConfigurationElement("parameters");
    if (config)
    {
        const std::vector <ConfigType> params = config->find("parameter");
        for (ConfigType cfg : params)
        {
            const std::string replace = cfg->getAttributeValue("replace");
            std::string by            = cfg->getAttributeValue("by");
            if(by.empty())
            {
                by = cfg->getAttributeValue("uid");
            }
            SLM_ASSERT("'parameter' tag must contain valid 'replace' and 'by' attributes.",
                       !replace.empty() && !by.empty());
            ParameterType param;
            param.replace = replace;
            param.by      = by;
            m_parameters.push_back(param);
        }
    }
}

//------------------------------------------------------------------------------

void IActivityView::launchActivitySeries(fwMedData::Series::sptr series)
{
    ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::dynamicCast(series);
    if (activitySeries)
    {
        this->launchActivity(activitySeries);
    }
}

//------------------------------------------------------------------------------

bool IActivityView::validateActivity(fwMedData::ActivitySeries::sptr activitySeries) const
{
    bool isValid = true;
    // Applies validator on activity series to check the data
    ::fwActivities::registry::ActivityInfo info;
    info = ::fwActivities::registry::Activities::getDefault()->getInfo(activitySeries->getActivityConfigId());

    // load activity bundle
    std::shared_ptr< ::fwRuntime::Bundle > bundle = ::fwRuntime::findBundle(info.bundleId, info.bundleVersion);
    if (!bundle->isStarted())
    {
        bundle->start();
    }

    for (std::string validatorImpl : info.validatorsImpl)
    {
        /// Process activity validator
        ::fwActivities::IValidator::sptr validator = ::fwActivities::validator::factory::New(validatorImpl);

        ::fwActivities::IActivityValidator::sptr activityValidator =
            ::fwActivities::IActivityValidator::dynamicCast(validator);
        SLM_ASSERT("Validator '" + validatorImpl + "' instantiation failed", activityValidator);

        ::fwActivities::IValidator::ValidationType validation = activityValidator->validate(activitySeries);
        if(!validation.first)
        {
            const std::string message = "The activity '" + info.title + "' can not be launched:\n" + validation.second;
            ::fwGui::dialog::MessageDialog::showMessageDialog("Activity launch",
                                                              message,
                                                              ::fwGui::dialog::IMessageDialog::CRITICAL);
            isValid = false;
            break;
        }
    }

    return isValid;
}

//------------------------------------------------------------------------------

fwMedData::ActivitySeries::sptr IActivityView::createMainActivity() const
{
    ::fwActivities::registry::ActivityInfo info;
    info = ::fwActivities::registry::Activities::getDefault()->getInfo(m_mainActivityId);

    ::fwMedData::ActivitySeries::sptr actSeries = ::fwMedData::ActivitySeries::New();
    if (info.requirements.size() > 0)
    {
        ::fwData::Composite::sptr data = actSeries->getData();
        for (::fwActivities::registry::ActivityRequirement req : info.requirements)
        {
            if ((req.minOccurs == 0 && req.maxOccurs == 0) || req.create)
            {
                (*data)[req.name] = ::fwData::factory::New(req.type);
            }
            else
            {
                ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Main activity",
                    "The main activity " + m_mainActivityId + " can not be launched. \n"
                    "This activity needs parameters that cannot be defined",
                    ::fwGui::dialog::IMessageDialog::INFO);
                return nullptr;
            }
        }
    }

    actSeries->setModality("OT");
    actSeries->setInstanceUID("fwActivities." + ::fwTools::UUID::generateUUID() );

    const ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    actSeries->setDate(::fwTools::getDate(now));
    actSeries->setTime(::fwTools::getTime(now));
    actSeries->setActivityConfigId(info.id);

    return actSeries;
}

//------------------------------------------------------------------------------

void IActivityView::translateParameters( ::fwData::Object::sptr sourceObj, const ParametersType& parameters,
                                         ReplaceMapType& replaceMap )
{
    for(const ParametersType::value_type& param :  parameters)
    {
        if(!param.isSeshat())
        {
            replaceMap[param.replace] = param.by;
        }
        else
        {
            std::string parameterToReplace = param.by;
            if (parameterToReplace.substr(0, 1) == "!")
            {
                parameterToReplace.replace(0, 1, "@");
            }

            ::fwData::Object::sptr obj = ::fwDataCamp::getObject(sourceObj, parameterToReplace);
            SLM_ASSERT("Invalid seshat path : '" + param.by + "'", obj);

            ::fwData::String::sptr stringParameter = ::fwData::String::dynamicCast(obj);

            std::string parameterValue = obj->getID();

            if(stringParameter && param.by.substr(0, 1) == "!")
            {
                parameterValue = stringParameter->getValue();
            }
            replaceMap[param.replace] = parameterValue;
        }
    }
}

} // namespace view
} // namespace gui
