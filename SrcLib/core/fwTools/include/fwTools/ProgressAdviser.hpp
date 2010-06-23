/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef PROGRESSADVISER_HPP_
#define PROGRESSADVISER_HPP_

#include <string>
#include <boost/signal.hpp>

#include <fwTools/config.hpp>


namespace fwTools {

/**
 * @brief   Subclasses Inherited from  ProgressAdviser can notify progression
 * This class is used as base class to add the functionalities of progress notification. To notify a progress then simply call
 * notifyProgress() method by given the progression [0, 1.0] and a message. The ProgressAdviser will automatically call handler (=Observer).
 * Handlers can be added using addHandler(). the Handler of type ProgessHandler can be a function or functor or boost::signal with a signature
 * ( float, std::string ) and returning nothing.
 * @class   ProgressAdviser
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWTOOLS_CLASS_API ProgressAdviser :  public boost::signals::trackable // allowing to auto disconnect when destructed
                                                                            // specialy use for cascading ProgressAdviser
                                                                            // using ::boost::bind

{
public:

    /// define a boost signal signature for the ProgessHandler
    typedef ::boost::signal< void(float, std::string) >   ProgessSignal; // signal for fctor or function returning void and
    /// define the ProgessHandler type
    typedef ProgessSignal::slot_type         ProgessHandler; // signal for fctor or function returning void and
                                                            // accepting a float as arguement percent
                                                            // string for information


    FWTOOLS_API ProgressAdviser();
    FWTOOLS_API virtual ~ProgressAdviser();

    /*
     * @brief append a new handler ( a functor accepting a float as argument an returning void)
     * @param[in] handler can be either  a function or functor or boost::signal with a signature( float, std::string ) and returning nothing
     */
    FWTOOLS_API void   addHandler(const ProgessHandler &handler);

    /// notify modification for all handler
    FWTOOLS_API void   notifyProgress(float percent,std::string msg="");

protected :

    /// to notify progress simply use m_progressSignal( percent,msg ) to notify to Handler
    ProgessSignal m_progressSignal;


};

}

#endif /* PROGRESSADVISER_HPP_ */
