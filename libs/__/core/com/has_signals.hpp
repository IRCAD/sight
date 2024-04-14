/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <sight/core/config.hpp>

#include "core/com/signals.hpp"

namespace sight::core::com
{

struct signal_base;

/**
 * @brief   This class proposes a mapping between a key_t and a SignalBase.
 */
class SIGHT_CORE_CLASS_API has_signals
{
public:

    using sptr  = std::shared_ptr<has_signals>;
    using csptr = std::shared_ptr<const has_signals>;

    has_signals()          = default;
    virtual ~has_signals() = default;

    [[nodiscard]] SPTR(signal_base) signal(const signals::signal_key_type& _key) const
    {
        return m_signals[_key];
    }

    template<typename signal_type>
    [[nodiscard]] SPTR(signal_type) signal(const signals::signal_key_type& _key) const
    {
        SPTR(signal_type) signal = std::dynamic_pointer_cast<signal_type>(this->signal(_key));
        return signal;
    }

    template<typename signal_type>
    SPTR(signal_type) new_signal(const signals::signal_key_type& _key)
    {
        SPTR(signal_type) sig = std::make_shared<signal_type>();
        m_signals(_key, sig);
        return sig;
    }

protected:

    /// Copy constructor forbidden
    has_signals(const has_signals&);

    /// Copy operator forbidden
    has_signals& operator=(const has_signals&);

    signals m_signals;
};

} // namespace sight::core::com
