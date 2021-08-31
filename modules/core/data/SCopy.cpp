/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SCopy.hpp"

#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/reflection/exception/NullPointer.hpp>
#include <data/reflection/exception/ObjectNotFound.hpp>
#include <data/reflection/getObject.hpp>

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

namespace sight::module::data
{

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

void SCopy::configuring()
{
    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;

    m_hasExtractTag = false;
    const ConfigurationType inCfg = m_configuration->findConfigurationElement("in");
    SIGHT_ASSERT("One 'in' tag is required.", inCfg);

    const std::vector<ConfigurationType> inoutCfg = m_configuration->find("inout");
    const std::vector<ConfigurationType> outCfg   = m_configuration->find("out");
    SIGHT_ASSERT("One 'inout' or one 'out' tag is required.", inoutCfg.size() + outCfg.size() == 1);

    const std::vector<ConfigurationType> extractCfg = inCfg->find("extract");
    SIGHT_ASSERT("Only one 'extract' tag is authorized.", extractCfg.size() <= 1);
    if(extractCfg.size() == 1)
    {
        ConfigurationType cfg = extractCfg[0];
        SIGHT_ASSERT("Missing attribute 'from'.", cfg->hasAttribute("from"));
        m_path          = cfg->getAttributeValue("from");
        m_hasExtractTag = true;
    }

    const ConfigurationType modeConfig = m_configuration->findConfigurationElement("mode");
    if(modeConfig)
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
            SIGHT_ERROR("Mode " + mode + " unknown. It should be either 'copyOnStart' or 'copyOnUpdate'");
        }
    }
}

//-----------------------------------------------------------------------------

void SCopy::starting()
{
    if(m_mode == ModeType::START)
    {
        this->copy();
    }
}

//-----------------------------------------------------------------------------

void SCopy::updating()
{
    if(m_mode == ModeType::UPDATE)
    {
        this->copy();
    }
    else
    {
        SIGHT_WARN("Object copy was request but the mode is to 'copyOnStart'");
    }
}

//-----------------------------------------------------------------------------

void SCopy::stopping()
{
    // Unregister output
    m_outTarget = nullptr;
}

//-----------------------------------------------------------------------------

void SCopy::copy()
{
    // Check if we use inout or output.
    bool create = false;
    {
        const auto targetLock = m_target.lock();
        if(!targetLock)
        {
            create = true;
        }
    }

    // Extract the object.
    const auto sourceObject = m_source.lock();

    sight::data::Object::csptr source;
    if(m_hasExtractTag)
    {
        sight::data::Object::sptr object;
        try
        {
            object = sight::data::reflection::getObject(sourceObject.get_shared(), m_path, true);
        }
        catch(const sight::data::reflection::exception::ObjectNotFound&)
        {
            SIGHT_WARN("Object from '" + m_path + "' not found");
        }
        catch(const sight::data::reflection::exception::NullPointer&)
        {
            SIGHT_WARN("Can't get object from '" + m_path + "'");
        }
        catch(std::exception& _e)
        {
            SIGHT_FATAL("Unhandled exception: " << _e.what());
        }

        SIGHT_WARN_IF("Object from '" + m_path + "' not found", !object);
        if(object)
        {
            source = object;
        }
    }
    else
    {
        source = sourceObject.get_shared();
    }

    if(source)
    {
        const auto setOutputData =
            [&]()
            {
                if(create)
                {
                    // Set the data as output.
                    sight::data::Object::sptr target = sight::data::Object::copy(source);
                    m_outTarget = target;
                }
                else
                {
                    // Copy the object to the inout.
                    const auto target = m_target.lock();
                    target->deepCopy(source);

                    auto sig = target->signal<sight::data::Object::ModifiedSignalType>(
                        sight::data::Object::s_MODIFIED_SIG
                    );
                    {
                        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                        sig->asyncEmit();
                    }
                }
            };

        if(source != sourceObject.get_shared())
        {
            setOutputData();
        }
        else
        {
            setOutputData();
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data.
