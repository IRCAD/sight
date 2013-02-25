#include <limits>

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/Convert.hpp>

#include <fwData/Vector.hpp>

#include "fwActivities/registry/Activities.hpp"

namespace fwActivities
{
namespace registry
{

ActivityAppConfigParam::ActivityAppConfigParam(const ConfigType &config) :
    replace(config.get<std::string>("<xmlattr>.replace")),
    by(config.get<std::string>("<xmlattr>.by"))
{
}

//-----------------------------------------------------------------------------

ActivityAppConfig::ActivityAppConfig(const ConfigType &config) :
    id(config.get<std::string>("<xmlattr>.id"))
{
    const ConfigType &configParameters = config.get_child("parameters");
    BOOST_FOREACH( const ConfigType::value_type &v, configParameters.equal_range("parameter") )
    {
        ActivityAppConfigParam parameter( v.second );
        parameters.push_back( parameter );
    }
}

//-----------------------------------------------------------------------------

ActivityRequirement::ActivityRequirement(const ConfigType &config) :
    name(config.get<std::string>("<xmlattr>.name")),
    type(config.get<std::string>("<xmlattr>.type")),
    minOccurs(config.get_optional<unsigned int>("<xmlattr>.minOccurs").get_value_or(1)),
    maxOccurs(config.get_optional<unsigned int>("<xmlattr>.maxOccurs").get_value_or(1))
{

    if (config.get_optional<std::string>("<xmlattr>.maxOccurs").get_value_or("") == "*")
    {
        this->maxOccurs = std::numeric_limits<unsigned int>::max();
    }

    OSLM_ASSERT( "minOccurs value shall be equal or greater than 0 and lower or equal to maxOccurs (" << maxOccurs << ")",
                 0 <= minOccurs && minOccurs <= maxOccurs);
    OSLM_TRACE( "ActivityRequirement : " << name << " : " << type << ", " << minOccurs << "-" << maxOccurs );
}

//-----------------------------------------------------------------------------

ActivityInfo::ActivityInfo(const SPTR(::fwRuntime::Extension) &ext) :
    id(ext->findConfigurationElement("id")->getValue()),
    title(ext->findConfigurationElement("title")->getValue()),
    description(ext->findConfigurationElement("desc")->getValue()),
    icon(ext->findConfigurationElement("icon")->getValue()),
    builderImpl(ext->findConfigurationElement("builder")->getValue()),
    appConfig(::fwRuntime::Convert::toPropertyTree(ext->findConfigurationElement("appConfig")).get_child("appConfig"))
{
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
}

//-----------------------------------------------------------------------------

bool ActivityInfo::usableWith(DataCountType dataCounts) const
{
    bool ok = dataCounts.size() <= m_requirementCount.size();

    if(ok)
    {
        BOOST_FOREACH( const RequirementsMinMaxCount::value_type &reqCount, m_requirementCount )
        {
            const MinMaxType &reqMinMax = reqCount.second;
            unsigned int dataCount = dataCounts[reqCount.first];
            ok = reqMinMax.first <= dataCount && dataCount <= reqMinMax.second;
            if( !ok )
            {
                break;
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
{}

//-----------------------------------------------------------------------------

void Activities::parseBundleInformation()
{
    std::vector< SPTR( ::fwRuntime::Extension ) >  extensions
        = ::fwRuntime::getAllExtensionsForPoint("::fwActivities::registry::Activities");

    this->parseBundleInformation(extensions);

}

void Activities::parseBundleInformation(const std::vector< SPTR( ::fwRuntime::Extension ) > &extensions)
{

    BOOST_FOREACH( const SPTR( ::fwRuntime::Extension ) &ext, extensions )
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
{}

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

    BOOST_FOREACH( Registry::value_type val, m_reg )
    {
        infos.push_back( val.second );
    }

    return infos;
}

//-----------------------------------------------------------------------------

ActivityInfo::DataCountType Activities::getDataCount( const ::fwData::Vector::sptr &data ) const
{
    ActivityInfo::DataCountType dataCount;

    BOOST_FOREACH( const ::fwData::Object::sptr &obj, *data)
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

    BOOST_FOREACH( const Registry::value_type &regValue, m_reg )
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

    BOOST_FOREACH( Registry::value_type val, m_reg )
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
    SLM_ASSERT("The id " <<  extensionId << " is not found in the application configuration parameter registry", iter != m_reg.end());
    return iter->second;
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace fwActivities

