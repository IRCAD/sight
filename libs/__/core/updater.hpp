/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#pragma once

#include "core/macros.hpp"

#include <bitset>

namespace sight::core
{

/**
 * @brief A base class for lazy instantiation, constructed on first demand.
 */
template<std::size_t NUM_BITSETS>
class updater
{
public:

    virtual ~updater() = default;

protected:

    /// Function called when the concrete update is performed
    virtual void do_update() = 0;

    /// If in lazy mode, update later when calling do_update(), otherwise do_update() now
    void lazy_update();

    /// If in lazy mode, set a bit flag and update later when calling do_update(), otherwise do_update() now
    template<class E>
    void lazy_update(E _flag);

    /// Calls do_update() if any bit flag is set
    void request_update();

    /**
     * @brief Tell if an update is required
     * @return True if the update is required, false otherwise
     */
    [[nodiscard]] bool update_needed() const;

    /**
     * @brief Tell if an update is required for a specific flag
     *
     * @tparam E Enumeration containing the specific flag
     * @param _flag Flag to be updated
     * @return True if the update is required, false otherwise
     */
    template<class E>
    [[nodiscard]] bool update_needed(E _flag) const;

    /**
     * @brief Request the updater to work in lazy mode
     *
     * @param _lazy True if lazy, false otherwise
     */
    void set_lazy(bool _lazy);

    /// Tells the updater the update has been done. This functions is intended to be called at the end
    /// of the implementation of do_update().
    void update_done();

private:

    bool m_lazy {false};

    /// Flag when the data was modified
    std::bitset<NUM_BITSETS> m_update_flags {0x0};
};

//------------------------------------------------------------------------------

template<std::size_t NUM_BITSETS>
inline void updater<NUM_BITSETS>::lazy_update()
{
    m_update_flags.set();
    if(!m_lazy)
    {
        do_update();
    }
}

//------------------------------------------------------------------------------

template<std::size_t NUM_BITSETS>
template<class E>
inline void updater<NUM_BITSETS>::lazy_update(E _flag)
{
    m_update_flags.set(static_cast<std::size_t>(_flag));
    if(!m_lazy)
    {
        do_update();
    }
}

//------------------------------------------------------------------------------

template<std::size_t NUM_BITSETS>
inline void updater<NUM_BITSETS>::request_update()
{
    if(update_needed())
    {
        this->do_update();
    }
}

//------------------------------------------------------------------------------

template<std::size_t NUM_BITSETS>
inline bool updater<NUM_BITSETS>::update_needed() const
{
    return m_update_flags.any();
}

//------------------------------------------------------------------------------

template<std::size_t NUM_BITSETS>
template<class E>
inline bool updater<NUM_BITSETS>::update_needed(E _flag) const
{
    return m_update_flags.test(static_cast<std::size_t>(_flag));
}

//------------------------------------------------------------------------------

template<std::size_t NUM_BITSETS>
inline void updater<NUM_BITSETS>::set_lazy(bool _lazy)
{
    m_lazy = _lazy;
}

//------------------------------------------------------------------------------

template<std::size_t NUM_BITSETS>
inline void updater<NUM_BITSETS>::update_done()
{
    m_update_flags.reset();
}

} //namespace sight::core
