/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
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

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/Composite.hpp>
#include <data/Exception.hpp>
#include <data/helper/Field.hpp>
#include <data/Series.hpp>
#include <data/SeriesSet.hpp>
#include <data/Vector.hpp>

#include <service/macros.hpp>

namespace sight::module::data
{

const core::com::slots::key_t SManage::ADD_SLOT               = "add";
const core::com::slots::key_t SManage::ADD_COPY_SLOT          = "addCopy";
const core::com::slots::key_t SManage::ADD_OR_SWAP_SLOT       = "addOrSwap";
const core::com::slots::key_t SManage::SWAP_OBJ_SLOT          = "swapObj";
const core::com::slots::key_t SManage::REMOVE_SLOT            = "remove";
const core::com::slots::key_t SManage::REMOVE_IF_PRESENT_SLOT = "removeIfPresent";
const core::com::slots::key_t SManage::CLEAR_SLOT             = "clear";

//-----------------------------------------------------------------------------

SManage::SManage() noexcept
{
    new_slot(ADD_SLOT, &SManage::add, this);
    new_slot(ADD_COPY_SLOT, &SManage::addCopy, this);
    new_slot(ADD_OR_SWAP_SLOT, &SManage::addOrSwap, this);
    new_slot(SWAP_OBJ_SLOT, &SManage::swap, this);
    new_slot(REMOVE_SLOT, &SManage::remove, this);
    new_slot(REMOVE_IF_PRESENT_SLOT, &SManage::removeIfPresent, this);
    new_slot(CLEAR_SLOT, &SManage::clear, this);
}

//-----------------------------------------------------------------------------

SManage::~SManage() noexcept =
    default;

//-----------------------------------------------------------------------------

void SManage::configuring()
{
    service::config_t config = this->getConfiguration();

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
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto obj = m_object.lock();
    SIGHT_ASSERT("Object is missing.", obj);

    const auto container = m_container.lock();

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
        helper.addOrSwap(m_fieldName, obj.get_shared());
        helper.notify();
    }
    else
    {
        if(const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared()); composite)
        {
            const auto scoped_emitter = composite->scoped_emit();
            composite->insert_or_assign(m_compositeKey, obj.get_shared());
        }
        else if(const auto vector = std::dynamic_pointer_cast<sight::data::Vector>(container.get_shared()); vector)
        {
            if(const auto& it = std::find(vector->cbegin(), vector->cend(), obj.get_shared()); it == vector->end())
            {
                const auto scoped_emitter = vector->scoped_emit();
                vector->push_back(obj.get_shared());
            }
            else
            {
                SIGHT_WARN("Object already exists in the Vector, does nothing.");
            }
        }
        else if(const auto series_set = std::dynamic_pointer_cast<sight::data::SeriesSet>(container.get_shared());
                series_set)
        {
            const auto series = std::dynamic_pointer_cast<sight::data::Series>(obj.get_shared());
            auto iter         = std::find(series_set->begin(), series_set->end(), series);
            if(iter == series_set->end())
            {
                const auto scoped_emitter = series_set->scoped_emit();
                series_set->push_back(series);
            }
            else
            {
                SIGHT_WARN("Series already exists in the SeriesSet, does nothing.");
            }
        }
        else
        {
            SIGHT_FATAL("Source object is not a Composite or a Vector or a SeriesSet");
        }
    }
}

//-----------------------------------------------------------------------------

// NOLINTNEXTLINE(cppcoreguidelines-noexcept-swap,performance-noexcept-swap)
void SManage::swap()
{
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto obj = m_object.lock();
    SIGHT_ASSERT("Object is missing.", obj);

    const auto container = m_container.lock();

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
        helper.swap(m_fieldName, obj.get_shared());
        helper.notify();
    }
    else if(const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared()); composite)
    {
        const auto scoped_emitter = composite->scoped_emit();
        composite->insert_or_assign(m_compositeKey, obj.get_shared());
    }
    else
    {
        SIGHT_WARN("'swap' slot is only managed for 'composite' or 'fieldHolder'");
    }
}

//-----------------------------------------------------------------------------

void SManage::remove()
{
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto obj = m_object.lock();

    const auto container = m_container.lock();

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
        helper.remove(m_fieldName);
        helper.notify();
    }
    else
    {
        if(const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared()); composite)
        {
            const auto scoped_emitter = composite->scoped_emit();
            composite->erase(m_compositeKey);
        }
        else
        {
            SIGHT_ASSERT("Object is missing.", obj);

            if(const auto vector = std::dynamic_pointer_cast<sight::data::Vector>(container.get_shared()); vector)
            {
                const auto scoped_emitter = vector->scoped_emit();
                vector->remove(obj.get_shared());
            }
            else if(const auto series_set = std::dynamic_pointer_cast<sight::data::SeriesSet>(container.get_shared());
                    series_set)
            {
                const auto series = std::dynamic_pointer_cast<sight::data::Series>(obj.get_shared());

                const auto scoped_emitter = series_set->scoped_emit();
                series_set->remove(series);
            }
            else
            {
                SIGHT_FATAL("Source object is assumed to be a Composite or a Vector or a SeriesSet");
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SManage::removeIfPresent()
{
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto obj = m_object.lock();

    const auto container = m_container.lock();

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
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
    else
    {
        if(const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared()); composite)
        {
            const auto scoped_emitter = composite->scoped_emit();
            composite->erase(m_compositeKey);
        }
        else
        {
            SIGHT_ASSERT("Object is missing.", obj);

            if(const auto vector = std::dynamic_pointer_cast<sight::data::Vector>(container.get_shared()); vector)
            {
                const auto scoped_emitter = composite->scoped_emit();
                vector->remove(obj.get_shared());
            }
            else if(const auto series_set = std::dynamic_pointer_cast<sight::data::SeriesSet>(container.get_shared());
                    series_set)
            {
                const auto series = std::dynamic_pointer_cast<sight::data::Series>(obj.get_shared());
                if(const auto iter = std::find(
                       series_set->begin(),
                       series_set->end(),
                       series
                ); iter != series_set->end())
                {
                    const auto scoped_emitter = series_set->scoped_emit();
                    series_set->erase(iter);
                }
                else
                {
                    SIGHT_WARN("Object does not exist in the SeriesSet, does nothing.");
                }
            }
            else
            {
                SIGHT_FATAL("Source object is assumed to be a Composite or a Vector or a SeriesSetB");
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SManage::clear()
{
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto container = m_container.lock();

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
        helper.clear();
        helper.notify();
    }
    else
    {
        if(const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared()); composite)
        {
            const auto scoped_emitter = composite->scoped_emit();
            composite->clear();
        }
        else if(const auto vector = std::dynamic_pointer_cast<sight::data::Vector>(container.get_shared()); vector)
        {
            const auto scoped_emitter = vector->scoped_emit();
            vector->clear();
        }
        else if(const auto series_set = std::dynamic_pointer_cast<sight::data::SeriesSet>(container.get_shared());
                series_set)
        {
            const auto scoped_emitter = series_set->scoped_emit();
            series_set->clear();
        }
        else
        {
            SIGHT_FATAL("Source object is assumed to be a Composite or a Vector or a SeriesSetB");
        }
    }
}

//------------------------------------------------------------------------------

void SManage::internalAdd(bool _copy)
{
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto object = m_object.lock();
    SIGHT_ASSERT("Object is missing.", object);

    auto obj = object.get_shared();
    if(_copy)
    {
        obj = sight::data::Object::copy(obj);
    }

    const auto container = m_container.lock();

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
        helper.add(m_fieldName, obj);
        helper.notify();
    }
    else
    {
        if(const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared()); composite)
        {
            const auto scoped_emitter = composite->scoped_emit();
            composite->insert_or_assign(m_compositeKey, obj);
        }
        else if(const auto vector = std::dynamic_pointer_cast<sight::data::Vector>(container.get_shared()); vector)
        {
            const auto scoped_emitter = vector->scoped_emit();
            vector->push_back(obj);
        }
        else if(const auto series_set = std::dynamic_pointer_cast<sight::data::SeriesSet>(container.get_shared());
                series_set)
        {
            auto series               = std::dynamic_pointer_cast<sight::data::Series>(obj);
            const auto scoped_emitter = series_set->scoped_emit();
            series_set->push_back(series);
        }
        else
        {
            SIGHT_FATAL("Source object is assumed to be a Composite or a Vector or a SeriesSetB");
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
