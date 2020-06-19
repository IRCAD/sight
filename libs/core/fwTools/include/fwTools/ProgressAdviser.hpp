/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#ifndef __FWTOOLS_PROGRESSADVISER_HPP__
#define __FWTOOLS_PROGRESSADVISER_HPP__

#include <fwTools/config.hpp>

#include <fwCore/base.hpp>

#include <boost/signals2.hpp>

#include <string>

namespace fwTools
{

/**
 * @brief   Subclasses Inherited from  ProgressAdviser can notify progression
 * This class is used as base class to add the functionalities of progress notification. To notify a progress then
 * simply call
 * notifyProgress() method by given the progression [0, 1.0] and a message. The ProgressAdviser will automatically call
 * handler (=Observer).
 * Handlers can be added using addHandler(). the Handler of type ProgessHandler can be a function or functor or
 * boost::signal with a signature
 * ( float, std::string ) and returning nothing.
 */
class FWTOOLS_CLASS_API ProgressAdviser : public ::boost::signals2::trackable
{
public:

    /// define a boost signal signature for the ProgessHandler
    typedef ::boost::signals2::signal< void (float, std::string) >  ProgessSignal; // signal for fctor or function
                                                                                   // returning void

    /// define the ProgessHandler type
    typedef ProgessSignal::slot_type ProgessHandler; // signal for fctor or function returning void and
                                                     // accepting a float as argument percent
                                                     // string for information

    FWTOOLS_API ProgressAdviser();
    FWTOOLS_API virtual ~ProgressAdviser();

    /*
     * @brief append a new handler ( a functor accepting a float as argument an returning void)
     * @param[in] handler can be either  a function or functor or boost::signal with a signature( float, std::string )
     * and returning nothing
     */
    FWTOOLS_API void addHandler(const ProgessHandler& handler);

    /// notify modification for all handler
    FWTOOLS_API void notifyProgress(float percent, const std::string& msg = "") const;

protected:

    /// to notify progress simply use m_progressSignal( percent,msg ) to notify to Handler
    ProgessSignal m_progressSignal;

};

}

#endif /* __FWTOOLS_PROGRESSADVISER_HPP__ */
