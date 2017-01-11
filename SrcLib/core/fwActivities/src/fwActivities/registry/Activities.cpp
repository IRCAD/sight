/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwActivities/registry/Activities.hpp"

#include <fwData/Vector.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/Convert.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/Runtime.hpp>

#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <limits>

namespace fwActivities
{
namespace registry
{

ActivityAppConfigParam::ActivityAppConfigParam(const ConfigType &config) :
    replace(config.get<std::string>("<xmlattr>.replace"))
{
    by = config.get_optional<std::string>("<xmlattr>.uid").get_value_or("");
    if(by.empty())
    {
        by = config.get<std::string>("<xmlattr>.by");
    }

}

//-----------------------------------------------------------------------------

ActivityAppConfig::ActivityAppConfig(const ConfigType &config) :
    id(config.get<std::string>("<xmlattr>.id"))
{
    if(config.count("parameters") == 1 )
    {
        const ConfigType &configParameters = config.get_child("parameters");
        BOOST_FOREACH( const ConfigType::value_type &v,  configParameters.equal_range("parameter") )
        {
            ActivityAppConfigParam parameter( v.second );
            parameters.push_back( parameter );
        }
    }
    OSLM_ASSERT("At most 1 <parameters> tag is allowed", config.count("parameters") < 2);
}

//-----------------------------------------------------------------------------

ActivityRequirementKey::ActivityRequirementKey(const ConfigType &config) :
    key(config.get_value<std::string>()),
    path(config.get_optional<std::string>("<xmlattr>.path").get_value_or(""))
{
}

//-----------------------------------------------------------------------------

ActivityRequirement::ActivityRequirement(const ConfigType &config) :
    name(config.get<std::string>("<xmlattr>.name")),
    type(config.get<std::string>("<xmlattr>.type")),
    container(config.get_optional<std::string>("<xmlattr>.container").get_value_or("")),
    description(config.get_optional<std::string>("desc").get_value_or("")),
    validator(config.get_optional<std::string>("validator").get_value_or("")),
    minOccurs(config.get_optional<unsigned int>("<xmlattr>.minOccurs").get_value_or(1)),
    maxOccurs(config.get_optional<unsigned int>("<xmlattr>.maxOccurs").get_value_or(1))
{
    BOOST_FOREACH( const ConfigType::value_type &v, config.equal_range("key") )
    {
        keys.push_back(ActivityRequirementKey(v.second));
    }

    if (config.get_optional<std::string>("<xmlattr>.maxOccurs").get_value_or("") == "*")
    {
        this->maxOccurs = std::numeric_limits<unsigned int>::max();
    }

    std::string createStr = config.get_optional<std::string>("<xmlattr>.create").get_value_or("false");
    SLM_ASSERT("'create' attribute must be 'true' or 'false'", createStr == "true" || createStr == "false");
    create = (createStr == "true");
    SLM_ASSERT("Create option is only available if minOccurs = 0 and maxOccurs = 1",
               !create || (minOccurs == 0 && maxOccurs == 1));

    OSLM_ASSERT(
        "minOccurs value shall be equal or greater than 0 and lower or equal to maxOccurs (" << maxOccurs << ")",
        0 <= minOccurs && minOccurs <= maxOccurs);
    OSLM_TRACE( "ActivityRequirement : " << name << " : " << type << ", " << minOccurs << "-" << maxOccurs
                                         << "(" << description << ")");
}

//-----------------------------------------------------------------------------

ActivityInfo::ActivityInfo(const SPTR(::fwRuntime::Extension) &ext) :
    id(ext->findConfigurationElement("id")->getValue()),
    title(ext->findConfigurationElement("title")->getValue()),
    description(ext->findConfigurationElement("desc")->getValue()),
    icon(ext->findConfigurationElement("icon")->getValue()),
    tabInfo(title),
    bundleId(ext->getBundle()->getIdentifier()),
    bundleVersion(ext->getBundle()->getVersion().string()),
    appConfig(::fwRuntime::Convert::toPropertyTree(ext->findConfigurationElement("appConfig")).get_child("appConfig"))
{
    if(ext->findConfigurationElement("tabinfo"))
    {
        tabInfo = ext->findConfigurationElement("tabinfo")->getValue();
    }


    ::fwRuntime::ConfigurationElement::sptr req = ext->findConfigurationElement("requirements");
    for(  ::fwRuntime::ConfigurationElementContainer::Iterator elem = req->begin();
          elem != req->end();
          ++elem )
    {
        ActivityRequirement requirement( ::fwRuntime::Convert::toPropertyTree(*elem).get_child("requirement") );
        requirements.push_back( requirement );

        MinMaxType &minMax = m_requirementCount[requirement.type];

        minMax.first += requirement.minOccurs;

        if (requirement.maxOccurs <  (std::numeric_limits<unsigned int>::max() - minMax.second) )
        {
            minMax.second += requirement.maxOccurs;
        }
        else
        {
            minMax.second = std::numeric_limits<unsigned int>::max();
        }
    }

    ::fwRuntime::ConfigurationElement::csptr builderCfg = ext->findConfigurationElement("builder");
    if (builderCfg)
    {
        builderImpl = builderCfg->getValue();
    }
    else
    {
        builderImpl = "::fwActivities::builder::ActivitySeriesInitData";
    }

    // backward compatibility
    ::fwRuntime::ConfigurationElement::csptr validatorCfg = ext->findConfigurationElement("validator");
    if(validatorCfg)
    {
        std::string validatorImplStr = validatorCfg->getValue();
        if(!validatorImplStr.empty())
        {
            validatorsImpl.push_back( validatorImplStr );
        }
    }

    ::fwRuntime::ConfigurationElement::sptr validatorsCfg = ext->findConfigurationElement("validators");
    if( validatorsCfg )
    {
        auto validators = ::fwRuntime::Convert::toPropertyTree(validatorsCfg).get_child("validators");
        BOOST_FOREACH( auto const &validator, validators.equal_range("validator") )
        {
            validatorsImpl.push_back( validator.second.get_value<std::string>() );
        }
    }

    // Set Default validator if none is defined
    if (validatorsImpl.empty())
    {
        validatorsImpl.push_back("::fwActivities::validator::DefaultActivity");
    }
}

//-----------------------------------------------------------------------------

bool ActivityInfo::usableWith(DataCountType dataCounts) const
{
    bool ok = dataCounts.size() <= m_requirementCount.size();

    if(ok)
    {
        for( const RequirementsMinMaxCount::value_type &reqCount :  m_requirementCount )
        {
            const MinMaxType &reqMinMax  = reqCount.second;
            DataCountType::iterator iter = dataCounts.find(reqCount.first);
            if (iter != dataCounts.end())
            {
                unsigned int dataCount = iter->second;
                ok = dataCount && reqMinMax.first <= dataCount && dataCount <= reqMinMax.second;
                dataCounts.erase(iter);
            }
            else
            {
                ok = (reqMinMax.first == 0);
            }
            if( !ok )
            {
                break;
            }
        }

        if(ok)
        {
            for( const DataCountType::value_type &dataCount :  dataCounts )
            {
                if(m_requirementCount.find(dataCount.first) == m_requirementCount.end())
                {
                    ok = false;
                    break;
                }
            }
        }
    }

    return ok;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

Activities::sptr Activities::s_activities = Activities::New();

//-----------------------------------------------------------------------------

Activities::sptr Activities::getDefault()
{
    return s_activities;
}

//-----------------------------------------------------------------------------

Activities::~Activities()
{
}

//-----------------------------------------------------------------------------

void Activities::parseBundleInformation()
{
    std::vector< SPTR( ::fwRuntime::Extension ) >  extensions
        = ::fwRuntime::getAllExtensionsForPoint("::fwActivities::registry::Activities");

    this->parseBundleInformation(extensions);

}

void Activities::parseBundleInformation(const std::vector< SPTR( ::fwRuntime::Extension ) > &extensions)
{

    for( const SPTR( ::fwRuntime::Extension ) &ext :  extensions )
    {
        OSLM_DEBUG("Parsing <" << ext->getBundle()->getIdentifier() << "> Activities");
        ActivityInfo info(ext);

        ::fwCore::mt::WriteLock lock(m_registryMutex);
        SLM_ASSERT("The id " <<  info.id << "(" << info.title << ")"
                             << " already exists in the Activities registry", m_reg.find( info.id ) == m_reg.end());
        m_reg.insert( Registry::value_type(info.id, info) );
    }
}

//-----------------------------------------------------------------------------

Activities::Activities()
{
}

//-----------------------------------------------------------------------------

void Activities::clearRegistry()
{
    ::fwCore::mt::WriteLock lock(m_registryMutex);
    m_reg.clear();
}

//-----------------------------------------------------------------------------

bool Activities::hasInfo( const std::string & extensionId ) const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    Registry::const_iterator iter = m_reg.find( extensionId );
    return iter != m_reg.end();
}

//-----------------------------------------------------------------------------

std::vector< ActivityInfo > Activities::getInfos() const
{
    std::vector< ActivityInfo > infos;

    ::fwCore::mt::ReadLock lock(m_registryMutex);

    for( Registry::value_type val :  m_reg )
    {
        infos.push_back( val.second );
    }

    return infos;
}

//-----------------------------------------------------------------------------

ActivityInfo::DataCountType Activities::getDataCount( const ::fwData::Vector::sptr &data ) const
{
    ActivityInfo::DataCountType dataCount;

    for( const ::fwData::Object::sptr &obj :  *data)
    {
        ++dataCount[obj->getClassname()];
    }

    return dataCount;
}

//-----------------------------------------------------------------------------

std::vector< ActivityInfo > Activities::getInfos( const ::fwData::Vector::sptr &data ) const
{
    ActivityInfo::DataCountType dataCount = this->getDataCount(data);
    std::vector< ActivityInfo > infos;

    ::fwCore::mt::ReadLock lock(m_registryMutex);

    for( const Registry::value_type &regValue :  m_reg )
    {
        const ActivityInfo &activity = regValue.second;
        if (activity.usableWith(dataCount))
        {
            infos.push_back(activity);
        }
    }

    return infos;
}

//-----------------------------------------------------------------------------

std::vector< std::string > Activities::getKeys() const
{
    std::vector< std::string > keys;

    ::fwCore::mt::ReadLock lock(m_registryMutex);

    for( Registry::value_type val :  m_reg )
    {
        keys.push_back( val.first );
    }

    return keys;
}

//-----------------------------------------------------------------------------

const ActivityInfo Activities::getInfo( const std::string & extensionId ) const
{
    ::fwCore::mt::ReadLock lock(m_registryMutex);
    Registry::const_iterator iter = m_reg.find( extensionId );
    SLM_ASSERT("The id " <<  extensionId << " is not found in the application configuration parameter registry",
               iter != m_reg.end());
    return iter->second;
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace fwActivities

