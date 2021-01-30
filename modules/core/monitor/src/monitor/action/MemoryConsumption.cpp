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

#include "monitor/action/MemoryConsumption.hpp"

#include <data/Array.hpp>

#include <services/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

#include <sstream>

namespace monitor
{
namespace action
{

/// Static variable shared by both actions
static std::vector< data::Array::sptr > memoryConsumer;

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::ui::base::IActionSrv, ::monitor::action::MemoryConsumption, ::sight::data::Object )

//-----------------------------------------------------------------------------

void MemoryConsumption::pushNewArray(size_t memorySizeInBytes)
{
    try
    {
        data::Array::sptr buffer = data::Array::New();
        data::Array::SizeType size(1, memorySizeInBytes);
        buffer->resize(size, core::tools::Type::s_UINT8_TYPENAME, true);

        SLM_INFO("Creating a data::array consuming "<< memorySizeInBytes/(1024*1024) << " Mo ");

        memoryConsumer.push_back( buffer );
    }
    catch( std::exception& e )
    {
        std::stringstream msg;
        msg << "Cannot allocate buffer ("<< memorySizeInBytes/(1024*1024) <<" Mo) :\n" << e.what() << std::endl;
        ui::base::dialog::MessageDialog::show(
            "Action increase memory",
            msg.str(),
            ui::base::dialog::IMessageDialog::CRITICAL);
    }
}

//------------------------------------------------------------------------------

MemoryConsumption::MemoryConsumption( ) noexcept :
    m_isIncreaseMode(true),
    m_memorySizeInBytes(1024*1024*256)                             // 256 Mo
{
}

//------------------------------------------------------------------------------

MemoryConsumption::~MemoryConsumption() noexcept
{
}

//------------------------------------------------------------------------------

void MemoryConsumption::updating()
{
    if(m_isIncreaseMode)
    {
        this->pushNewArray(m_memorySizeInBytes);
    }
    else
    {
        if( !memoryConsumer.empty() )
        {
            SLM_INFO("Removing one data::Array");
            memoryConsumer.pop_back();
        }
    }
}

//------------------------------------------------------------------------------

void MemoryConsumption::configuring()
{
    this->ui::base::IActionSrv::initialize();

    core::runtime::ConfigurationElement::sptr consumptionCfg;
    consumptionCfg = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("Missing mode tag", consumptionCfg);

    SLM_ASSERT("Missing attribute 'value'", consumptionCfg->hasAttribute("mode"));
    std::string mode = consumptionCfg->getAttributeValue("mode");
    SLM_ASSERT("Wrong value ("<< mode <<") for mode tag", mode == "increase" || mode == "decrease");
    m_isIncreaseMode = (mode == "increase");

    if(m_isIncreaseMode && consumptionCfg->hasAttribute("value"))
    {
        std::string value = consumptionCfg->getAttributeValue("value");
        size_t sizeInMo   = ::boost::lexical_cast<size_t>(value);
        m_memorySizeInBytes = sizeInMo * 1024 * 1024;
    }
}

//------------------------------------------------------------------------------

void MemoryConsumption::starting()
{
    this->ui::base::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------
void MemoryConsumption::stopping()
{
    this->ui::base::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace basicOpCtrl
