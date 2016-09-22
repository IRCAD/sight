/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlCamp/SCopy.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Object.hpp>

#include <fwDataCamp/getObject.hpp>
#include <fwDataCamp/exception/ObjectNotFound.hpp>


namespace ctrlCamp
{

fwServicesRegisterMacro(::ctrlCamp::ICamp, ::ctrlCamp::SCopy, ::fwData::Object);

const ::fwServices::IService::KeyType s_SOURCE_INPUT = "source";
const ::fwServices::IService::KeyType s_TARGET_INOUT = "target";

//-----------------------------------------------------------------------------

SCopy::SCopy() :
    m_mode(ModeType::UPDATE)
{

}

//-----------------------------------------------------------------------------

SCopy::~SCopy()
{
}

//-----------------------------------------------------------------------------

void SCopy::configuring() throw( ::fwTools::Failed )
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    m_hasExtractTag = false;
    if(!this->isVersion2())
    {
        const ConfigurationType srcConfig = m_configuration->findConfigurationElement("source");
        SLM_ASSERT("element 'source' is missing.", srcConfig);
        m_source = srcConfig->getValue();
        SLM_ASSERT("Missing source.", srcConfig->getValue() != "");

        const ConfigurationType tgtConfig = m_configuration->findConfigurationElement("target");
        if (tgtConfig)
        {
            m_target = tgtConfig->getValue();

        }
    }
    else
    {
        const ConfigurationType inCfg = m_configuration->findConfigurationElement("in");
        SLM_ASSERT("One 'in' tag is required.", inCfg);

        const std::vector< ConfigurationType > inoutCfg = m_configuration->find("inout");
        const std::vector< ConfigurationType > outCfg   = m_configuration->find("out");
        SLM_ASSERT("One 'inout' or one 'out' tag is required.", inoutCfg.size() +  outCfg.size() == 1);

        const std::vector< ConfigurationType > extractCfg = inCfg->find("extract");
        SLM_ASSERT("Only one 'extract' tag is authorized.", extractCfg.size() <= 1);
        if (extractCfg.size() == 1)
        {
            ConfigurationType cfg = extractCfg[0];
            SLM_ASSERT("Missing attribute 'from'.", cfg->hasAttribute("from"));
            m_path          = cfg->getAttributeValue("from");
            m_hasExtractTag = true;
        }

    }

    const ConfigurationType modeConfig = m_configuration->findConfigurationElement("mode");
    if (modeConfig)
    {
        auto mode = modeConfig->getValue();
        if(mode == "copyOnStart")
        {
            m_mode = ModeType::START;
        }
        else if(mode == "copyOnUpdate")
        {
            m_mode = ModeType::UPDATE;
        }
        else
        {
            SLM_ERROR("Mode " + mode + " unknown. It should be either 'copyOnStart' or 'copyOnUpdate'");
        }
    }
}

//-----------------------------------------------------------------------------

void SCopy::starting() throw( ::fwTools::Failed )
{
    if(m_mode == ModeType::START)
    {
        this->copy();
    }
}

//-----------------------------------------------------------------------------

void SCopy::updating() throw( ::fwTools::Failed )
{
    if(m_mode == ModeType::UPDATE)
    {
        this->copy();
    }
    else
    {
        SLM_ERROR("Object copy was request but the mode is to 'copyOnStart'");
    }
}

//-----------------------------------------------------------------------------

void SCopy::stopping() throw( ::fwTools::Failed )
{
    // Unregister output
    this->setOutput(s_TARGET_INOUT, nullptr);
}

//-----------------------------------------------------------------------------

void SCopy::copy()
{
    bool create = false;
    ::fwData::Object::sptr target;
    ::fwData::Object::csptr source;
    if (m_target.empty())
    {
        if(this->isVersion2())
        {
            target = this->getInOut< ::fwData::Object >(s_TARGET_INOUT);
            if(!target)
            {
                create = true;
            }
        }
        else
        {
            target = this->getObject< ::fwData::Object >();
        }
    }
    else
    {
        if (m_target.substr(0,1) == "@")
        {
            target = ::fwDataCamp::getObject(this->getObject(), m_target);
            SLM_ASSERT("Invalid seshat path : '" + m_target + "'", target);
        }
        else
        {
            ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_target);
            SLM_ASSERT("Object '" + m_target + "' is not found", obj);
            target = ::fwData::Object::dynamicCast(obj);
            SLM_ASSERT("Object '" + m_target + "' is not a fwData::Object (" + obj->getClassname() + ")", target);
        }
    }
    if (m_source.substr(0,1) == "@")
    {
        source = ::fwDataCamp::getObject(this->getObject(), m_source);
        SLM_ERROR_IF("Invalid seshat path '" + m_source + "' or object does not exist", !source);
    }
    else
    {
        if(this->isVersion2())
        {
            ::fwData::Object::csptr sourceObject = this->getInput< ::fwData::Object >(s_SOURCE_INPUT);
            if (m_hasExtractTag)
            {
                ::fwData::Object::sptr object;
                try
                {
                    object = ::fwDataCamp::getObject( sourceObject, m_path, true );
                }
                catch(::fwDataCamp::exception::ObjectNotFound& nf)
                {
                    SLM_WARN("Object from '"+ m_path +"' not found");
                }
                catch(std::exception& e)
                {
                    OSLM_FATAL("Unhandled exception: " << e.what());
                }

                SLM_WARN_IF("Object from '"+ m_path +"' not found", !object);
                if(object)
                {
                    source = object;
                }
            }
            else
            {
                source = sourceObject;
            }
        }
        else
        {
            ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_source);
            SLM_ASSERT("Object '" + m_source + "' is not found", obj);
            source = ::fwData::Object::dynamicCast(obj);
            SLM_ERROR_IF(
                "Object '" + m_source + "' is not a valid fwData::Object (" + obj->getClassname() + ") or does not exist",
                !source);
        }
    }

    if(source)
    {
        if(create)
        {
            target = ::fwData::Object::copy(source);
            this->setOutput(s_TARGET_INOUT, target);
        }
        else
        {
            ::fwData::mt::ObjectWriteLock lock(target);

            // copy the object
            target->deepCopy(source);

            auto sig = target->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
    }
}

//-----------------------------------------------------------------------------


} // namespace ctrlCamp
