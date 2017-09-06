/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMedDataQt/SSeriesViewer.hpp"

#include <fwCore/base.hpp>

#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwMedData/Series.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>

namespace uiMedDataQt
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::uiMedDataQt::SSeriesViewer, ::fwData::Vector);

//------------------------------------------------------------------------------

SSeriesViewer::SSeriesViewer()
{
}

//------------------------------------------------------------------------------

SSeriesViewer::~SSeriesViewer() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesViewer::info(std::ostream& _sstream )
{
    // Update message
    _sstream << std::string("SSeriesViewer");
}

//------------------------------------------------------------------------------

void SSeriesViewer::starting()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SSeriesViewer::stopping()
{
    if(m_configTemplateManager)
    {
        m_configTemplateManager->stopAndDestroy();
        m_configTemplateManager.reset();
    }
}

//------------------------------------------------------------------------------

void SSeriesViewer::updating()
{
    ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector >();

    if(m_configTemplateManager)
    {
        m_configTemplateManager->stopAndDestroy();
        m_configTemplateManager.reset();
    }

    if(vector->size() == 1)
    {
        ::fwData::Object::sptr obj = vector->front();
        std::string classname             = obj->getClassname();
        SeriesConfigMapType::iterator itr = m_seriesConfigs.find(classname);

        if(itr != m_seriesConfigs.end())
        {
            SeriesConfigInfo info = itr->second;
            std::string configId  = info.configId;

            std::map< std::string, std::string > replaceMap;
            // Generate generic UID
            std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier( this->getID() );
            replaceMap["GENERIC_UID"] = genericUidAdaptor;
            replaceMap["WID_PARENT"]  = m_parentView;
            replaceMap["objectID"]    = obj->getID();

            for(const ReplaceValuesMapType::value_type& elt :  info.extractValues)
            {
                ::fwData::Object::sptr object = ::fwDataCamp::getObject( obj, elt.second );
                OSLM_ASSERT("Object from name "<< elt.second <<" not found", object);
                replaceMap[elt.first] = object->getID();
            }

            for(const ReplaceValuesMapType::value_type& elt :  info.parameters)
            {
                SLM_ASSERT("Value '" << elt.first << "' already used in extracted values.",
                           replaceMap.find(elt.first) == replaceMap.end());
                replaceMap[elt.first] = elt.second;
            }

            // Init manager
            m_configTemplateManager = ::fwServices::IAppConfigManager::New();
            m_configTemplateManager->setConfig( configId, replaceMap );

            // Launch config
            m_configTemplateManager->launch();
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesViewer::configuring()
{
    std::vector < ::fwRuntime::ConfigurationElement::sptr > viewCfg = m_configuration->find("parentView");
    SLM_ASSERT("Missing tag 'parentView'", viewCfg.size() == 1);

    m_parentView = viewCfg[0]->getAttributeValue("wid");
    SLM_ASSERT("'wid' attribute missing for tag 'parentView'.", !m_parentView.empty());

    std::vector < ::fwRuntime::ConfigurationElement::sptr > configsCfg = m_configuration->find("configs");
    SLM_ASSERT("Missing tag 'configs'", configsCfg.size() == 1);

    std::vector < ::fwRuntime::ConfigurationElement::sptr > config = configsCfg[0]->find("config");
    SLM_ASSERT("Missing tag 'config'", !config.empty());

    for(const ::fwRuntime::ConfigurationElement::sptr& elt :  config)
    {
        SeriesConfigInfo info;
        info.configId = elt->getAttributeValue("id");
        SLM_ASSERT("'id' attribute must not be empty", !info.configId.empty());
        std::string seriesType = elt->getAttributeValue("type");
        SLM_ASSERT("'type' attribute must not be empty", !seriesType.empty());
        OSLM_ASSERT("Type " << seriesType << " is already defined.",
                    m_seriesConfigs.find(seriesType) == m_seriesConfigs.end() );

        for(const ::fwRuntime::ConfigurationElement::sptr& extractElt :  elt->find("extract"))
        {
            std::string path = extractElt->getAttributeValue("path");
            SLM_ASSERT("'path' attribute must not be empty", !path.empty());
            std::string pattern = extractElt->getAttributeValue("pattern");
            SLM_ASSERT("'pattern' attribute must not be empty", !pattern.empty());
            info.extractValues[pattern] = path;
        }

        for(const ::fwRuntime::ConfigurationElement::sptr& param :  elt->find("parameter"))
        {
            std::string replace = param->getAttributeValue("replace");
            SLM_ASSERT("'replace' attribute must not be empty", !replace.empty());
            std::string by = param->getAttributeValue("by");
            if(by.empty())
            {
                by = param->getAttributeValue("uid");
            }
            SLM_ASSERT("'by' attribute must not be empty", !by.empty());
            info.parameters[replace] = by;
        }

        m_seriesConfigs[seriesType] = info;
    }

}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SSeriesViewer::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Vector::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Vector::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace uiMedDataQt
