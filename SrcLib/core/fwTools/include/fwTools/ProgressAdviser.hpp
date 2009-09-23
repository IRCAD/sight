/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
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
 * @brief 	Class ProgressAdviser
 * @class 	ProgressAdviser
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 * @todo	Complete doxygen comments
 */
class FWTOOLS_CLASS_API ProgressAdviser :  public boost::signals::trackable // allowing to auto disconnect when destructed
																			// specialy use for cascading ProgressAdviser
																			// using ::boost::bind

{
public:

	typedef ::boost::signal< void(float, std::string) >   ProgessSignal; // signal for fctor or function returning void and
	typedef ProgessSignal::slot_type         ProgessHandler; // signal for fctor or function returning void and
															// accepting a float as arguement percent
															// string for information


	FWTOOLS_API ProgressAdviser();
	FWTOOLS_API virtual ~ProgressAdviser();

	/// append a new handler ( a functor accepting a float as argument an returning void)
	FWTOOLS_API void   addHandler(const ProgessHandler &handler);

	/// notify modification for all handler
	FWTOOLS_API void   notifyProgress(float percent,std::string msg="");

protected :

	/// to notify progress simply use m_progressSignal( pourcent ) to notify to Handler
	ProgessSignal m_progressSignal;


};

}

#endif /* PROGRESSADVISER_HPP_ */
