/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/SManage.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/helper/Composite.hpp>
#include <fwDataTools/helper/Vector.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

namespace ctrlSelection
{

const ::fwCom::Slots::SlotKeyType SManage::s_ADD_OR_SWAP_SLOT       = "addOrSwap";
const ::fwCom::Slots::SlotKeyType SManage::s_ADD_SLOT               = "add";
const ::fwCom::Slots::SlotKeyType SManage::s_SWAP_OBJ_SLOT          = "swapObj";
const ::fwCom::Slots::SlotKeyType SManage::s_REMOVE_SLOT            = "remove";
const ::fwCom::Slots::SlotKeyType SManage::s_REMOVE_IF_PRESENT_SLOT = "removeIfPresent";

const ::fwServices::IService::KeyType s_COMPOSITE_INOUT = "composite";
const ::fwServices::IService::KeyType s_VECTOR_INOUT    = "vector";
const ::fwServices::IService::KeyType s_SERIESDB_INOUT  = "seriesDB";
const ::fwServices::IService::KeyType s_OBJECT_INOUT    = "object";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::ctrlSelection::SManage, ::fwData::Composite );

//-----------------------------------------------------------------------------

SManage::SManage() noexcept
{
    newSlot(s_ADD_OR_SWAP_SLOT, &SManage::addOrSwap, this);
    newSlot(s_ADD_SLOT, &SManage::add, this);
    newSlot(s_SWAP_OBJ_SLOT, &SManage::swap, this);
    newSlot(s_REMOVE_SLOT, &SManage::remove, this);
    newSlot(s_REMOVE_IF_PRESENT_SLOT, &SManage::removeIfPresent, this);
}

//-----------------------------------------------------------------------------

SManage::~SManage() noexcept
{
}

//-----------------------------------------------------------------------------

void SManage::configuring()
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;

    ConfigType cfg = m_configuration->findConfigurationElement("compositeKey");
    if (cfg)
    {
        m_compositeKey = cfg->getValue();
        SLM_ASSERT("Missing 'compositeKey' value", !m_compositeKey.empty());
    }
}

//-----------------------------------------------------------------------------

void SManage::starting()
{
}

//-----------------------------------------------------------------------------

void SManage::stopping()
{
}

//-----------------------------------------------------------------------------

void SManage::updating()
{
}

//-----------------------------------------------------------------------------

void SManage::add()
{
    ::fwData::Object::sptr obj = this->getInOut< ::fwData::Object >(s_OBJECT_INOUT);
    SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);

    ::fwData::Composite::sptr composite  = this->getInOut< ::fwData::Composite >(s_COMPOSITE_INOUT);
    ::fwData::Vector::sptr vector        = this->getInOut< ::fwData::Vector >(s_VECTOR_INOUT);
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);

    SLM_ASSERT("Target object is missing, required one of 'composite', 'vector' or 'seriesDB'",
               vector || composite || seriesDB);
    if (composite)
    {
        SLM_ASSERT("Only one target object is managed", !vector && !seriesDB);
        ::fwDataTools::helper::Composite helper( composite );
        helper.add(m_compositeKey, obj);
        helper.notify();
    }
    else if (vector)
    {
        SLM_ASSERT("Only one target object is managed", !composite && !seriesDB);
        ::fwDataTools::helper::Vector helper( vector );
        helper.add(obj);
        helper.notify();
    }
    else if (seriesDB)
    {
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
        SLM_ASSERT("Target object is a SeriesDB, so object must be a Series.", series);
        SLM_ASSERT("Only one target object is managed", !composite && !vector);
        ::fwMedDataTools::helper::SeriesDB helper( seriesDB );
        helper.add(series);
        helper.notify();
    }
}

//-----------------------------------------------------------------------------

void SManage::addOrSwap()
{
    ::fwData::Object::sptr obj = this->getInOut< ::fwData::Object >(s_OBJECT_INOUT);
    SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);

    ::fwData::Composite::sptr composite  = this->getInOut< ::fwData::Composite >(s_COMPOSITE_INOUT);
    ::fwData::Vector::sptr vector        = this->getInOut< ::fwData::Vector >(s_VECTOR_INOUT);
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);

    SLM_ASSERT("Target object is missing, required one of 'composite', 'vector' or 'seriesDB'",
               vector || composite || seriesDB);
    if (composite)
    {
        SLM_ASSERT("Only one target object is managed", !vector && !seriesDB);
        ::fwDataTools::helper::Composite helper( composite );
        if (composite->find(m_compositeKey) == composite->end())
        {
            helper.add(m_compositeKey, obj);
        }
        else
        {
            helper.swap(m_compositeKey, obj);
        }

        helper.notify();
    }
    else if (vector)
    {
        SLM_ASSERT("Only one target object is managed", !composite && !seriesDB);
        auto iter = std::find(vector->begin(), vector->end(), obj);
        if (iter == vector->end())
        {
            ::fwDataTools::helper::Vector helper( vector );
            helper.add(obj);
            helper.notify();
        }
        SLM_WARN_IF("Object already exists in the Vector, does nothing.", iter != vector->end());
    }
    else if (seriesDB)
    {
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
        SLM_ASSERT("Target object is a SeriesDB, so object must be a Series.", series);
        SLM_ASSERT("Only one target object is managed", !composite && !vector);

        auto iter = std::find(seriesDB->begin(), seriesDB->end(), series);
        if (iter == seriesDB->end())
        {
            ::fwMedDataTools::helper::SeriesDB helper( seriesDB );
            helper.add(series);
            helper.notify();
        }
        SLM_WARN_IF("Object already exists in the SeriesDB, does nothing.", iter != seriesDB->end());
    }
}

//-----------------------------------------------------------------------------

void SManage::swap()
{
    ::fwData::Object::sptr obj = this->getInOut< ::fwData::Object >(s_OBJECT_INOUT);
    SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);

    ::fwData::Composite::sptr composite = this->getInOut< ::fwData::Composite >(s_COMPOSITE_INOUT);
    SLM_ASSERT("'swap' slot is only managed for 'composite'.", composite);
    ::fwDataTools::helper::Composite helper( composite );
    helper.swap(m_compositeKey, obj);
    helper.notify();
}

//-----------------------------------------------------------------------------

void SManage::remove()
{
    ::fwData::Object::sptr obj = this->getInOut< ::fwData::Object >(s_OBJECT_INOUT);
    SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);

    ::fwData::Composite::sptr composite  = this->getInOut< ::fwData::Composite >(s_COMPOSITE_INOUT);
    ::fwData::Vector::sptr vector        = this->getInOut< ::fwData::Vector >(s_VECTOR_INOUT);
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);

    SLM_ASSERT("Target object is missing, required one of 'composite', 'vector' or 'seriesDB'",
               vector || composite || seriesDB);
    if (composite)
    {
        SLM_ASSERT("Only one target object is managed", !vector && !seriesDB);
        ::fwDataTools::helper::Composite helper( composite );
        helper.remove(m_compositeKey);
        helper.notify();
    }
    else if (vector)
    {
        SLM_ASSERT("Only one target object is managed", !composite && !seriesDB);
        ::fwDataTools::helper::Vector helper( vector );
        helper.remove(obj);
        helper.notify();
    }
    else if (seriesDB)
    {
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
        SLM_ASSERT("Target object is a SeriesDB, so object must be a Series.", series);
        SLM_ASSERT("Only one target object is managed", !composite && !vector);

        ::fwMedDataTools::helper::SeriesDB helper( seriesDB );
        helper.remove(series);
        helper.notify();
    }
}

//-----------------------------------------------------------------------------

void SManage::removeIfPresent()
{
    ::fwData::Object::sptr obj = this->getInOut< ::fwData::Object >(s_OBJECT_INOUT);
    SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);

    ::fwData::Composite::sptr composite  = this->getInOut< ::fwData::Composite >(s_COMPOSITE_INOUT);
    ::fwData::Vector::sptr vector        = this->getInOut< ::fwData::Vector >(s_VECTOR_INOUT);
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);

    SLM_ASSERT("Target object is missing, required one of 'composite', 'vector' or 'seriesDB'",
               vector || composite || seriesDB);
    if (composite)
    {
        SLM_ASSERT("Only one target object is managed", !vector && !seriesDB);
        ::fwDataTools::helper::Composite helper( composite );
        if (composite->find(m_compositeKey) != composite->end())
        {
            helper.remove(m_compositeKey);
            helper.notify();
        }
    }
    else if (vector)
    {
        SLM_ASSERT("Only one target object is managed", !composite && !seriesDB);
        auto iter = std::find(vector->begin(), vector->end(), obj);
        if (iter != vector->end())
        {
            ::fwDataTools::helper::Vector helper( vector );
            helper.remove(obj);
            helper.notify();
        }
        SLM_WARN_IF("Object does not exist in the Vector, does nothing.", iter == vector->end());
    }
    else if (seriesDB)
    {
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);
        SLM_ASSERT("Target object is a SeriesDB, so object must be a Series.", series);
        SLM_ASSERT("Only one target object is managed", !composite && !vector);

        auto iter = std::find(seriesDB->begin(), seriesDB->end(), series);
        if (iter != seriesDB->end())
        {
            ::fwMedDataTools::helper::SeriesDB helper( seriesDB );
            helper.remove(series);
            helper.notify();
        }
        SLM_WARN_IF("Object does not exist in the SeriesDB, does nothing.", iter == seriesDB->end());
    }
}

//-----------------------------------------------------------------------------

} // ctrlSelection
