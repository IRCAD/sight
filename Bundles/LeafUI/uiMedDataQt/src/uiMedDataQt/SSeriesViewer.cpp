/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwData/Vector.hpp>

#include <fwComEd/helper/Composite.hpp>

#include <fwMedData/Series.hpp>

#include "uiMedDataQt/SSeriesViewer.hpp"
#include <fwData/Composite.hpp>


namespace uiMedData
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController , ::uiMedData::SSeriesViewer , ::fwData::Vector);

//------------------------------------------------------------------------------

SSeriesViewer::SSeriesViewer()
{
}

//------------------------------------------------------------------------------

SSeriesViewer::~SSeriesViewer() throw()
{
}

//------------------------------------------------------------------------------

void SSeriesViewer::info(std::ostream &_sstream )
{
    // Update message
    _sstream << std::string("SSeriesViewer");
}

//------------------------------------------------------------------------------

void SSeriesViewer::starting() throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void SSeriesViewer::receiving( ::fwServices::ObjectMsg::csptr msg ) throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void SSeriesViewer::stopping() throw(::fwTools::Failed)
{
    if(m_configTemplateManager)
    {
        m_configTemplateManager->stopAndDestroy();
        m_configTemplateManager.reset();
    }
}

//------------------------------------------------------------------------------

void SSeriesViewer::updating() throw(::fwTools::Failed)
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
        std::string classname = obj->getClassname();
        SeriesConfigMapType::iterator itr = m_seriesConfigs.find(classname);

        std::map< std::string, std::string > finalMap;
        // Generate generic UID
        std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier( this->getID() );
        finalMap["GENERIC_UID"] = genericUidAdaptor;
        finalMap["WID_PARENT"] = m_parentView;

        std::string seriesKey = "series";
        finalMap["seriesKey"] = seriesKey;

        if(itr != m_seriesConfigs.end())
        {
            std::string configId =  itr->second;
            // Init manager
            ::fwRuntime::ConfigurationElement::csptr config =
                   ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig( configId, finalMap );
            m_configTemplateManager = ::fwServices::AppConfigManager::New();
            m_configTemplateManager->setConfig( config );

            // Launch config
            m_configTemplateManager->launch();

            ::fwData::Composite::sptr root = m_configTemplateManager->getConfigRoot< ::fwData::Composite >();

            ::fwComEd::helper::Composite compoHelper(root);

            compoHelper.add(seriesKey, obj);
            compoHelper.notify(this->getSptr());
        }
        else
        {
        }
    }
    else
    {
    }
}

//------------------------------------------------------------------------------

void SSeriesViewer::configuring() throw(::fwTools::Failed)
{
    std::vector < ::fwRuntime::ConfigurationElement::sptr > viewCfg = m_configuration->find("parentView");
    SLM_ASSERT("Missing tag 'parentView'", viewCfg.size() == 1);
    m_parentView = viewCfg[0]->getValue();
    SLM_ASSERT("'parentView' value must not be empty ", !m_parentView.empty());

    std::vector < ::fwRuntime::ConfigurationElement::sptr > configsCfg = m_configuration->find("configs");
    SLM_ASSERT("Missing tag 'configs'", configsCfg.size() == 1);

    std::vector < ::fwRuntime::ConfigurationElement::sptr > config = configsCfg[0]->find("config");
    SLM_ASSERT("Missing tag 'config'", !config.empty());

    BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr elt, config)
    {
        std::string id = elt->getAttributeValue("id");
        SLM_ASSERT("'id' attribute must not be empty", !id.empty());
        std::string seriesType = elt->getAttributeValue("type");
        SLM_ASSERT("'type' attribute must not be empty", !seriesType.empty());
        OSLM_ASSERT("Type " << seriesType << " is already defined.",
                    m_seriesConfigs.find(seriesType)== m_seriesConfigs.end() );
        m_seriesConfigs[seriesType] = id;
    }

}

//------------------------------------------------------------------------------

} // namespace uiMedData
