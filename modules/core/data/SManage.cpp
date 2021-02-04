/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
 * Copyright (C) 2015-2018 IHU Strasbourg
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

#include "SManage.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/Composite.hpp>
#include <data/Exception.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>
#include <data/tools/helper/Composite.hpp>
#include <data/tools/helper/Field.hpp>
#include <data/tools/helper/SeriesDB.hpp>
#include <data/tools/helper/Vector.hpp>
#include <data/Vector.hpp>

#include <services/macros.hpp>

namespace sight::modules::data
{

const core::com::Slots::SlotKeyType SManage::s_ADD_SLOT               = "add";
const core::com::Slots::SlotKeyType SManage::s_ADD_COPY_SLOT          = "addCopy";
const core::com::Slots::SlotKeyType SManage::s_ADD_OR_SWAP_SLOT       = "addOrSwap";
const core::com::Slots::SlotKeyType SManage::s_SWAP_OBJ_SLOT          = "swapObj";
const core::com::Slots::SlotKeyType SManage::s_REMOVE_SLOT            = "remove";
const core::com::Slots::SlotKeyType SManage::s_REMOVE_IF_PRESENT_SLOT = "removeIfPresent";
const core::com::Slots::SlotKeyType SManage::s_CLEAR_SLOT             = "clear";

const services::IService::KeyType s_COMPOSITE_INOUT    = "composite";
const services::IService::KeyType s_VECTOR_INOUT       = "vector";
const services::IService::KeyType s_SERIESDB_INOUT     = "seriesDB";
const services::IService::KeyType s_FIELD_HOLDER_INOUT = "fieldHolder";
const services::IService::KeyType s_OBJECT_INOUT       = "object";

//-----------------------------------------------------------------------------

SManage::SManage() noexcept
{
    newSlot(s_ADD_SLOT, &SManage::add, this);
    newSlot(s_ADD_COPY_SLOT, &SManage::addCopy, this);
    newSlot(s_ADD_OR_SWAP_SLOT, &SManage::addOrSwap, this);
    newSlot(s_SWAP_OBJ_SLOT, &SManage::swap, this);
    newSlot(s_REMOVE_SLOT, &SManage::remove, this);
    newSlot(s_REMOVE_IF_PRESENT_SLOT, &SManage::removeIfPresent, this);
    newSlot(s_CLEAR_SLOT, &SManage::clear, this);
}

//-----------------------------------------------------------------------------

SManage::~SManage() noexcept
{
}

//-----------------------------------------------------------------------------

void SManage::configuring()
{
    services::IService::ConfigType config = this->getConfigTree();

    m_compositeKey = config.get("compositeKey", "");
    m_fieldName    = config.get("field", "");
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
    internalAdd(false);
}

//------------------------------------------------------------------------------

void SManage::addCopy()
{
    internalAdd(true);
}

//-----------------------------------------------------------------------------

void SManage::addOrSwap()
{
    SLM_ASSERT("Service is not started", this->isStarted());

    sight::data::Object::sptr obj = this->getInOut< sight::data::Object >(s_OBJECT_INOUT);
    SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);

    sight::data::Composite::sptr composite = this->getInOut< sight::data::Composite >(s_COMPOSITE_INOUT);
    sight::data::Vector::sptr vector       = this->getInOut< sight::data::Vector >(s_VECTOR_INOUT);
    sight::data::SeriesDB::sptr seriesDB   = this->getInOut< sight::data::SeriesDB >(s_SERIESDB_INOUT);
    sight::data::Object::sptr fieldHolder  = this->getInOut< sight::data::Object >(s_FIELD_HOLDER_INOUT);

    SLM_ASSERT("Target object is missing, required one of 'composite', 'vector', 'seriesDB', or 'fieldHolder'",
               vector || composite || seriesDB || fieldHolder);
    if (composite)
    {
        SLM_ASSERT("Only one target object is managed", !vector && !seriesDB && !fieldHolder);
        sight::data::tools::helper::Composite helper( composite );
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
        SLM_ASSERT("Only one target object is managed", !composite && !seriesDB && !fieldHolder);
        auto iter = std::find(vector->begin(), vector->end(), obj);
        if (iter == vector->end())
        {
            sight::data::tools::helper::Vector helper( vector );
            helper.add(obj);
            helper.notify();
        }
        SLM_WARN_IF("Object already exists in the Vector, does nothing.", iter != vector->end());
    }
    else if (seriesDB)
    {
        sight::data::Series::sptr series = sight::data::Series::dynamicCast(obj);
        SLM_ASSERT("Target object is a SeriesDB, so object must be a Series.", series);
        SLM_ASSERT("Only one target object is managed", !composite && !vector && !fieldHolder);

        auto iter = std::find(seriesDB->begin(), seriesDB->end(), series);
        if (iter == seriesDB->end())
        {
            sight::data::tools::helper::SeriesDB helper( seriesDB );
            helper.add(series);
            helper.notify();
        }
        SLM_WARN_IF("Object already exists in the SeriesDB, does nothing.", iter != seriesDB->end());
    }
    else if (fieldHolder)
    {
        SLM_ASSERT("Only one target object is managed",  !vector && !composite && !seriesDB);
        sight::data::tools::helper::Field helper( fieldHolder );
        helper.addOrSwap(m_fieldName, obj);
        helper.notify();
    }
}

//-----------------------------------------------------------------------------

void SManage::swap()
{
    SLM_ASSERT("Service is not started", this->isStarted());

    sight::data::Object::sptr obj = this->getInOut< sight::data::Object >(s_OBJECT_INOUT);
    SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);

    sight::data::Composite::sptr composite = this->getInOut< sight::data::Composite >(s_COMPOSITE_INOUT);
    sight::data::Object::sptr fieldHolder  = this->getInOut< sight::data::Object >(s_FIELD_HOLDER_INOUT);

    SLM_ASSERT("'swap' slot is only managed for 'composite' or 'fieldHolder'",
               composite || fieldHolder);
    if (composite)
    {
        SLM_ASSERT("Only one target object is managed",  !fieldHolder);
        sight::data::tools::helper::Composite helper( composite );
        helper.swap(m_compositeKey, obj);
        helper.notify();
    }
    else if (fieldHolder)
    {
        SLM_ASSERT("Only one target object is managed",  !composite);
        sight::data::tools::helper::Field helper( fieldHolder );
        helper.swap(m_fieldName, obj);
        helper.notify();
    }
}

//-----------------------------------------------------------------------------

void SManage::remove()
{
    SLM_ASSERT("Service is not started", this->isStarted());

    sight::data::Object::sptr obj = this->getInOut< sight::data::Object >(s_OBJECT_INOUT);

    sight::data::Composite::sptr composite = this->getInOut< sight::data::Composite >(s_COMPOSITE_INOUT);
    sight::data::Vector::sptr vector       = this->getInOut< sight::data::Vector >(s_VECTOR_INOUT);
    sight::data::SeriesDB::sptr seriesDB   = this->getInOut< sight::data::SeriesDB >(s_SERIESDB_INOUT);
    sight::data::Object::sptr fieldHolder  = this->getInOut< sight::data::Object >(s_FIELD_HOLDER_INOUT);

    SLM_ASSERT("Target object is missing, required one of 'composite', 'vector', 'seriesDB', or 'fieldHolder'",
               vector || composite || seriesDB || fieldHolder);
    if (composite)
    {
        SLM_ASSERT("Only one target object is managed", !vector && !seriesDB && !fieldHolder);
        sight::data::tools::helper::Composite helper( composite );
        helper.remove(m_compositeKey);
        helper.notify();
    }
    else if (vector)
    {
        SLM_ASSERT("Only one target object is managed", !composite && !seriesDB && !fieldHolder);
        SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);
        sight::data::tools::helper::Vector helper( vector );
        helper.remove(obj);
        helper.notify();
    }
    else if (seriesDB)
    {
        SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);
        sight::data::Series::sptr series = sight::data::Series::dynamicCast(obj);
        SLM_ASSERT("Target object is a SeriesDB, so object must be a Series.", series);
        SLM_ASSERT("Only one target object is managed", !composite && !vector && !fieldHolder);

        sight::data::tools::helper::SeriesDB helper( seriesDB );
        helper.remove(series);
        helper.notify();
    }
    else if (fieldHolder)
    {
        SLM_ASSERT("Only one target object is managed",  !vector && !composite && !seriesDB);
        sight::data::tools::helper::Field helper( fieldHolder );
        helper.remove(m_fieldName);
        helper.notify();
    }
}

//-----------------------------------------------------------------------------

void SManage::removeIfPresent()
{
    SLM_ASSERT("Service is not started", this->isStarted());

    sight::data::Object::sptr obj = this->getInOut< sight::data::Object >(s_OBJECT_INOUT);

    sight::data::Composite::sptr composite = this->getInOut< sight::data::Composite >(s_COMPOSITE_INOUT);
    sight::data::Vector::sptr vector       = this->getInOut< sight::data::Vector >(s_VECTOR_INOUT);
    sight::data::SeriesDB::sptr seriesDB   = this->getInOut< sight::data::SeriesDB >(s_SERIESDB_INOUT);
    sight::data::Object::sptr fieldHolder  = this->getInOut< sight::data::Object >(s_FIELD_HOLDER_INOUT);

    SLM_ASSERT("Target object is missing, required one of 'composite', 'vector', 'seriesDB', or 'fieldHolder'",
               vector || composite || seriesDB || fieldHolder);
    if (composite)
    {
        SLM_ASSERT("Only one target object is managed", !vector && !seriesDB && !fieldHolder);
        sight::data::tools::helper::Composite helper( composite );
        if (composite->find(m_compositeKey) != composite->end())
        {
            helper.remove(m_compositeKey);
            helper.notify();
        }
    }
    else if (vector)
    {
        SLM_ASSERT("Only one target object is managed", !composite && !seriesDB && !fieldHolder);
        SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);
        auto iter = std::find(vector->begin(), vector->end(), obj);
        if (iter != vector->end())
        {
            sight::data::tools::helper::Vector helper( vector );
            helper.remove(obj);
            helper.notify();
        }
        SLM_WARN_IF("Object does not exist in the Vector, does nothing.", iter == vector->end());
    }
    else if (seriesDB)
    {
        SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);
        sight::data::Series::sptr series = sight::data::Series::dynamicCast(obj);
        SLM_ASSERT("Target object is a SeriesDB, so object must be a Series.", series);
        SLM_ASSERT("Only one target object is managed", !composite && !vector && !fieldHolder);

        auto iter = std::find(seriesDB->begin(), seriesDB->end(), series);
        if (iter != seriesDB->end())
        {
            sight::data::tools::helper::SeriesDB helper( seriesDB );
            helper.remove(series);
            helper.notify();
        }
        SLM_WARN_IF("Object does not exist in the SeriesDB, does nothing.", iter == seriesDB->end());
    }
    else if (fieldHolder)
    {
        SLM_ASSERT("Only one target object is managed",  !vector && !composite && !seriesDB);
        sight::data::tools::helper::Field helper( fieldHolder );
        try
        {
            helper.remove(m_fieldName);
            helper.notify();
        }
        catch(sight::data::Exception&)
        {
            // Silently ignore the exception which means the field was not present
        }
    }
}

//-----------------------------------------------------------------------------

void SManage::clear()
{
    SLM_ASSERT("Service is not started", this->isStarted());

    sight::data::Composite::sptr composite = this->getInOut< sight::data::Composite >(s_COMPOSITE_INOUT);
    sight::data::Vector::sptr vector       = this->getInOut< sight::data::Vector >(s_VECTOR_INOUT);
    sight::data::SeriesDB::sptr seriesDB   = this->getInOut< sight::data::SeriesDB >(s_SERIESDB_INOUT);
    sight::data::Object::sptr fieldHolder  = this->getInOut< sight::data::Object >(s_FIELD_HOLDER_INOUT);

    SLM_ASSERT("Target object is missing, required one of 'composite', 'vector', 'seriesDB', or 'fieldHolder'",
               vector || composite || seriesDB || fieldHolder);
    if (composite)
    {
        SLM_ASSERT("Only one target object is managed", !vector && !seriesDB && !fieldHolder);
        sight::data::tools::helper::Composite helper( composite );
        helper.clear();
        helper.notify();
    }
    else if (vector)
    {
        SLM_ASSERT("Only one target object is managed", !composite && !seriesDB && !fieldHolder);
        sight::data::tools::helper::Vector helper( vector );
        helper.clear();
        helper.notify();
    }
    else if (seriesDB)
    {
        sight::data::tools::helper::SeriesDB helper( seriesDB );
        helper.clear();
        helper.notify();
    }
    else if (fieldHolder)
    {
        SLM_ASSERT("Only one target object is managed",  !vector && !composite && !seriesDB);
        sight::data::tools::helper::Field helper( fieldHolder );
        helper.clear();
        helper.notify();
    }
}

//------------------------------------------------------------------------------

void SManage::internalAdd(bool _copy)
{
    SLM_ASSERT("Service is not started", this->isStarted());

    sight::data::Object::sptr obj = this->getInOut< sight::data::Object >(s_OBJECT_INOUT);
    SLM_ASSERT("Object '" + s_OBJECT_INOUT + "' is missing.", obj);

    if(_copy)
    {
        obj = sight::data::Object::copy(obj);
    }

    sight::data::Composite::sptr composite = this->getInOut< sight::data::Composite >(s_COMPOSITE_INOUT);
    sight::data::Vector::sptr vector       = this->getInOut< sight::data::Vector >(s_VECTOR_INOUT);
    sight::data::SeriesDB::sptr seriesDB   = this->getInOut< sight::data::SeriesDB >(s_SERIESDB_INOUT);
    sight::data::Object::sptr fieldHolder  = this->getInOut< sight::data::Object >(s_FIELD_HOLDER_INOUT);

    SLM_ASSERT("Target object is missing, required one of 'composite', 'vector', 'seriesDB', or 'fieldHolder'",
               vector || composite || seriesDB || fieldHolder);
    if (composite)
    {
        SLM_ASSERT("Only one target object is managed", !vector && !seriesDB && !fieldHolder);
        sight::data::tools::helper::Composite helper( composite );
        helper.add(m_compositeKey, obj);
        helper.notify();
    }
    else if (vector)
    {
        SLM_ASSERT("Only one target object is managed", !composite && !seriesDB && !fieldHolder);
        sight::data::tools::helper::Vector helper( vector );
        helper.add(obj);
        helper.notify();
    }
    else if (seriesDB)
    {
        sight::data::Series::sptr series = sight::data::Series::dynamicCast(obj);
        SLM_ASSERT("Target object is a SeriesDB, so object must be a Series.", series);
        SLM_ASSERT("Only one target object is managed", !composite && !vector && !fieldHolder);
        sight::data::tools::helper::SeriesDB helper( seriesDB );
        helper.add(series);
        helper.notify();
    }
    else if (fieldHolder)
    {
        SLM_ASSERT("Only one target object is managed",  !vector && !composite && !seriesDB);
        sight::data::tools::helper::Field helper( fieldHolder );
        helper.add(m_fieldName, obj);
        helper.notify();
    }
}

//-----------------------------------------------------------------------------

} // ctrlSelection
