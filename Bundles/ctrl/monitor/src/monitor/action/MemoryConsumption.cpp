/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <boost/lexical_cast.hpp>

#include <fwData/Array.hpp>

#include <fwServices/macros.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "monitor/action/MemoryConsumption.hpp"

namespace monitor
{
namespace action
{

/// Static variable shared by both actions
static std::vector< ::fwData::Array::sptr > memoryConsumer;

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::monitor::action::MemoryConsumption, ::fwData::Object );

//-----------------------------------------------------------------------------

void MemoryConsumption::pushNewArray(size_t memorySizeInBytes)
{
    try
    {
        ::fwData::Array::sptr buffer = ::fwData::Array::New();
        ::fwData::Array::SizeType size(1, memorySizeInBytes);
        buffer->resize(::fwTools::Type::s_UINT8_TYPENAME, size, 1, true);

        OSLM_INFO("Creating a fwData::array consuming "<< memorySizeInBytes/(1024*1024) << " Mo ");

        memoryConsumer.push_back( buffer );
    }
    catch( std::exception &e )
    {
        std::stringstream msg;
        msg << "Cannot allocate buffer (256 Mo) :\n" << e.what() << std::endl;
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Action increase memory",
            msg.str(),
            ::fwGui::dialog::IMessageDialog::CRITICAL);
    }
}

//------------------------------------------------------------------------------

MemoryConsumption::MemoryConsumption( ) noexcept :
    m_isIncreaseMode(true), m_memorySizeInBytes(1024*1024*256)     // 256 Mo
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
            SLM_INFO("Removing one fwData::Array");
            memoryConsumer.pop_back();
        }
    }
}

//------------------------------------------------------------------------------

void MemoryConsumption::configuring()
{
    this->::fwGui::IActionSrv::initialize();

    ::fwRuntime::ConfigurationElement::sptr consumptionCfg;
    consumptionCfg = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("Missing mode tag", consumptionCfg);

    SLM_ASSERT("Missing attribute 'value'", consumptionCfg->hasAttribute("mode"));
    std::string mode = consumptionCfg->getAttributeValue("mode");
    OSLM_ASSERT("Wrong value ("<< mode <<") for mode tag", mode == "increase" || mode == "decrease");
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
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------
void MemoryConsumption::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace basicOpCtrl
