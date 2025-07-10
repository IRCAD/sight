/************************************************************************
 *
 * Copyright (C) 2015-2025 IRCAD France
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

#include "manage.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/exception.hpp>
#include <data/helper/field.hpp>
#include <data/map.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>
#include <data/vector.hpp>

namespace sight::module::data
{

//-----------------------------------------------------------------------------

manage::manage() noexcept
{
    new_signal<signals::empty_t>(signals::FAILED);

    new_slot(slots::ADD, &manage::add, this);
    new_slot(slots::ADD_COPY, &manage::add_copy, this);
    new_slot(slots::ADD_OR_SWAP, &manage::add_or_swap, this);
    new_slot(slots::SWAP_OBJ, &manage::swap, this);
    new_slot(slots::POP_FRONT, &manage::pop_front, this);
    new_slot(slots::REMOVE, &manage::remove, this);
    new_slot(slots::REMOVE_IF_PRESENT, &manage::remove_if_present, this);
    new_slot(slots::CLEAR, &manage::clear, this);
}

//-----------------------------------------------------------------------------

void manage::configuring()
{
    service::config_t config = this->get_config();

    m_map_key    = config.get("mapKey", "");
    m_field_name = config.get("field", "");
}

//-----------------------------------------------------------------------------

void manage::starting()
{
}

//-----------------------------------------------------------------------------

void manage::stopping()
{
}

//-----------------------------------------------------------------------------

void manage::updating()
{
}

//-----------------------------------------------------------------------------

void manage::add()
{
    internal_add(false);
}

//------------------------------------------------------------------------------

void manage::add_copy()
{
    internal_add(true);
}

//-----------------------------------------------------------------------------

void manage::add_or_swap()
{
    const auto obj = m_object.lock();
    SIGHT_ASSERT("Object is missing.", obj);

    const auto container = m_container.lock();

    if(!m_field_name.empty())
    {
        sight::data::helper::field helper(container.get_shared());
        helper.add_or_swap(m_field_name, obj.get_shared());
        helper.notify();
    }
    else
    {
        if(const auto map = std::dynamic_pointer_cast<sight::data::map>(container.get_shared()); map)
        {
            const auto scoped_emitter = map->scoped_emit();
            map->insert_or_assign(m_map_key, obj.get_shared());
        }
        else if(const auto vector = std::dynamic_pointer_cast<sight::data::vector>(container.get_shared()); vector)
        {
            if(const auto it = std::find(vector->cbegin(), vector->cend(), obj.get_shared()); it == vector->end())
            {
                const auto scoped_emitter = vector->scoped_emit();
                vector->push_back(obj.get_shared());
            }
            else
            {
                this->async_emit(signals::FAILED);
                SIGHT_WARN("Object already exists in the Vector, does nothing.");
            }
        }
        else if(const auto series_set = std::dynamic_pointer_cast<sight::data::series_set>(container.get_shared());
                series_set)
        {
            const auto series = std::dynamic_pointer_cast<sight::data::series>(obj.get_shared());
            auto iter         = std::find(series_set->begin(), series_set->end(), series);
            if(iter == series_set->end())
            {
                const auto scoped_emitter = series_set->scoped_emit();
                series_set->push_back(series);
            }
            else
            {
                this->async_emit(signals::FAILED);
                SIGHT_WARN("Series already exists in the series_set, does nothing.");
            }
        }
        else
        {
            SIGHT_FATAL("Source object is not a Map or a Vector or a series_set");
        }
    }
}

//-----------------------------------------------------------------------------

// NOLINTNEXTLINE(cppcoreguidelines-noexcept-swap,performance-noexcept-swap)
void manage::swap()
{
    const auto obj = m_object.lock();
    SIGHT_ASSERT("Object is missing.", obj);

    const auto container = m_container.lock();

    if(!m_field_name.empty())
    {
        sight::data::helper::field helper(container.get_shared());
        helper.swap(m_field_name, obj.get_shared());
        helper.notify();
    }
    else if(const auto map = std::dynamic_pointer_cast<sight::data::map>(container.get_shared()); map)
    {
        const auto scoped_emitter = map->scoped_emit();
        map->insert_or_assign(m_map_key, obj.get_shared());
    }
    else
    {
        this->async_emit(signals::FAILED);
        SIGHT_WARN("'swap' slot is only managed for 'map' or 'fieldHolder'");
    }
}

//-----------------------------------------------------------------------------

void manage::pop_front()
{
    const auto obj = m_object.lock();

    const auto container = m_container.lock();

    if(!m_field_name.empty())
    {
        sight::data::helper::field helper(container.get_shared());

        obj->deep_copy(container->get_field(m_field_name));

        helper.remove(m_field_name);
        helper.notify();
    }
    else
    {
        if(const auto map = std::dynamic_pointer_cast<sight::data::map>(container.get_shared()); map)
        {
            const auto scoped_emitter = map->scoped_emit();
            SIGHT_ASSERT("Can not find element " << std::quoted(m_map_key), map->find(m_map_key) != map->end());
            obj->deep_copy((*map)[m_map_key]);
            map->erase(m_map_key);
        }
        else
        {
            if(const auto vector = std::dynamic_pointer_cast<sight::data::vector>(container.get_shared());
               vector&& !vector->empty())
            {
                const auto scoped_emitter = vector->scoped_emit();
                const auto object         = vector->front();
                obj->deep_copy(object);
                vector->remove(object);
            }
            else if(const auto series_set = std::dynamic_pointer_cast<sight::data::series_set>(container.get_shared());
                    series_set&& !series_set->empty())
            {
                const auto scoped_emitter = series_set->scoped_emit();
                const auto object         = series_set->front();
                obj->deep_copy(object);
                series_set->remove(object);
            }
            else
            {
                SIGHT_INFO("Nothing to pop");
                this->async_emit(signals::FAILED);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void manage::remove()
{
    const auto obj = m_object.lock();

    const auto container = m_container.lock();

    if(!m_field_name.empty())
    {
        sight::data::helper::field helper(container.get_shared());
        helper.remove(m_field_name);
        helper.notify();
    }
    else
    {
        if(const auto map = std::dynamic_pointer_cast<sight::data::map>(container.get_shared()); map)
        {
            const auto scoped_emitter = map->scoped_emit();
            map->erase(m_map_key);
        }
        else
        {
            SIGHT_ASSERT("Object is missing.", obj);

            if(const auto vector = std::dynamic_pointer_cast<sight::data::vector>(container.get_shared()); vector)
            {
                const auto scoped_emitter = vector->scoped_emit();
                vector->remove(obj.get_shared());
            }
            else if(const auto series_set = std::dynamic_pointer_cast<sight::data::series_set>(container.get_shared());
                    series_set)
            {
                const auto series = std::dynamic_pointer_cast<sight::data::series>(obj.get_shared());

                const auto scoped_emitter = series_set->scoped_emit();
                series_set->remove(series);
            }
            else
            {
                SIGHT_FATAL("Source object is assumed to be a Map or a Vector or a series_set");
            }
        }
    }
}

//-----------------------------------------------------------------------------

void manage::remove_if_present()
{
    const auto obj = m_object.lock();

    const auto container = m_container.lock();

    if(!m_field_name.empty())
    {
        sight::data::helper::field helper(container.get_shared());
        try
        {
            helper.remove(m_field_name);
            helper.notify();
        }
        catch(sight::data::exception&)
        {
            // Silently ignore the exception which means the field was not present
        }
    }
    else
    {
        if(const auto map = std::dynamic_pointer_cast<sight::data::map>(container.get_shared()); map)
        {
            const auto scoped_emitter = map->scoped_emit();
            map->erase(m_map_key);
        }
        else
        {
            SIGHT_ASSERT("Object is missing.", obj);

            if(const auto vector = std::dynamic_pointer_cast<sight::data::vector>(container.get_shared()); vector)
            {
                const auto scoped_emitter = map->scoped_emit();
                vector->remove(obj.get_shared());
            }
            else if(const auto series_set = std::dynamic_pointer_cast<sight::data::series_set>(container.get_shared());
                    series_set)
            {
                const auto series = std::dynamic_pointer_cast<sight::data::series>(obj.get_shared());
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
                    this->async_emit(signals::FAILED);
                    SIGHT_WARN("Object does not exist in the series_set, does nothing.");
                }
            }
            else
            {
                SIGHT_FATAL("Source object is assumed to be a Map or a Vector or a SeriesSetB");
            }
        }
    }
}

//-----------------------------------------------------------------------------

void manage::clear()
{
    const auto container = m_container.lock();

    if(!m_field_name.empty())
    {
        sight::data::helper::field helper(container.get_shared());
        helper.clear();
        helper.notify();
    }
    else
    {
        if(const auto map = std::dynamic_pointer_cast<sight::data::map>(container.get_shared()); map)
        {
            const auto scoped_emitter = map->scoped_emit();
            map->clear();
        }
        else if(const auto vector = std::dynamic_pointer_cast<sight::data::vector>(container.get_shared()); vector)
        {
            const auto scoped_emitter = vector->scoped_emit();
            vector->clear();
        }
        else if(const auto series_set = std::dynamic_pointer_cast<sight::data::series_set>(container.get_shared());
                series_set)
        {
            const auto scoped_emitter = series_set->scoped_emit();
            series_set->clear();
        }
        else
        {
            SIGHT_FATAL("Source object is assumed to be a Map or a Vector or a SeriesSetB");
        }
    }
}

//------------------------------------------------------------------------------

void manage::internal_add(bool _copy)
{
    const auto object = m_object.lock();
    SIGHT_ASSERT("Object is missing.", object);

    auto obj = object.get_shared();
    if(_copy)
    {
        obj = sight::data::object::copy(obj);
    }

    const auto container = m_container.lock();

    if(!m_field_name.empty())
    {
        sight::data::helper::field helper(container.get_shared());
        helper.add(m_field_name, obj);
        helper.notify();
    }
    else
    {
        if(const auto map = std::dynamic_pointer_cast<sight::data::map>(container.get_shared()); map)
        {
            const auto scoped_emitter = map->scoped_emit();
            map->insert_or_assign(m_map_key, obj);
        }
        else if(const auto vector = std::dynamic_pointer_cast<sight::data::vector>(container.get_shared()); vector)
        {
            const auto scoped_emitter = vector->scoped_emit();
            vector->push_back(obj);
        }
        else if(const auto series_set = std::dynamic_pointer_cast<sight::data::series_set>(container.get_shared());
                series_set)
        {
            auto series               = std::dynamic_pointer_cast<sight::data::series>(obj);
            const auto scoped_emitter = series_set->scoped_emit();
            series_set->push_back(series);
        }
        else
        {
            SIGHT_FATAL("Source object is assumed to be a Map or a Vector or a SeriesSetB");
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
