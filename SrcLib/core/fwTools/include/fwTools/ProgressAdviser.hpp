/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
