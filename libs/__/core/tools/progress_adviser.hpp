/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include <core/base.hpp>

#include <boost/signals2.hpp>

#include <string>

namespace sight::core::tools
{

/**
 * @brief   Subclasses Inherited from  progress_adviser can notify progression
 * This class is used as base class to add the functionalities of progress notification. To notify a progress then
 * simply call
 * notify_progress() method by given the progression [0, 1.0] and a message. The progress_adviser will automatically
 * call
 * handler (=Observer).
 * Handlers can be added using add_handler(). the Handler of type ProgressHandler can be a function or functor or
 * boost::signal with a signature
 * ( float, std::string ) and returning nothing.
 */
class SIGHT_CORE_CLASS_API progress_adviser : public boost::signals2::trackable
{
public:

    /// define a boost signal signature for the ProgressHandler
    using progress_signal = boost::signals2::signal<void (float, std::string)>; // signal for functor or function
                                                                                // returning void

    /// define the ProgressHandler type
    using progress_handler = progress_signal::slot_type; // signal for functor or function returning void and
                                                         // accepting a float as argument percent
                                                         // string for information

    SIGHT_CORE_API progress_adviser();
    SIGHT_CORE_API virtual ~progress_adviser();

    /*
     * @brief append a new handler ( a functor accepting a float as argument an returning void)
     * @param[in] handler can be either  a function or functor or boost::signal with a signature( float, std::string )
     * and returning nothing
     */
    SIGHT_CORE_API void add_handler(const progress_handler& _handler);

    /// notify modification for all handler
    SIGHT_CORE_API void notify_progress(float _percent, const std::string& _msg = "") const;

protected:

    /// to notify progress simply use m_progressSignal( percent,msg ) to notify to Handler
    progress_signal m_progress_signal;
};

} // namespace sight::core::tools
