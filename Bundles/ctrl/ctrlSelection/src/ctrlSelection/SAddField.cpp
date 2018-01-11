/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/SAddField.hpp"

#include <fwDataTools/helper/Field.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

fwServicesRegisterMacro(::fwServices::IController, ::ctrlSelection::SAddField);

namespace ctrlSelection
{

// ----------------------------------------------------------------------------

SAddField::SAddField() noexcept
{
}

// ----------------------------------------------------------------------------

SAddField::~SAddField() noexcept
{
}

// ----------------------------------------------------------------------------

void SAddField::configuring()
{
    auto srvConfig = this->getConfigTree();

    BOOST_FOREACH(const ::fwServices::IService::ConfigType::value_type &v, srvConfig.equal_range("inout"))
    {
        const ::fwServices::IService::ConfigType& inout     = v.second;
        const ::fwServices::IService::ConfigType& attrInOut = inout.get_child("<xmlattr>");
        if(attrInOut.get("group", "" ) == "source")
        {
            BOOST_FOREACH(const ::fwServices::IService::ConfigType::value_type &v, inout.equal_range("key"))
            {
                const ::fwServices::IService::ConfigType& specAssoc = v.second;
                const ::fwServices::IService::ConfigType& attr      = specAssoc.get_child("<xmlattr>");
                const std::string field                             = attr.get("field", "");
                m_fields.push_back(field);
            }
        }
    }
}

// ----------------------------------------------------------------------------

void SAddField::starting()
{
}

// ----------------------------------------------------------------------------

void SAddField::stopping()
{
}

// ----------------------------------------------------------------------------

void SAddField::updating()
{
    ::fwData::Object::sptr target = this->getInOut< ::fwData::Object>("target");
    SLM_ASSERT("Missing 'target' object", target);

    ::fwDataTools::helper::Field helper(target);
    size_t i = 0;
    for(const auto& elt : m_fields)
    {
        ::fwData::Object::sptr fieldObject = this->getInOut< ::fwData::Object>("source", i++);
        SLM_ASSERT("'source' object for field '" + elt + "'not found.", fieldObject);
        helper.setField(elt, fieldObject);
    }
    helper.notify();
}

// ----------------------------------------------------------------------------

}  // namespace ctrlSelection
