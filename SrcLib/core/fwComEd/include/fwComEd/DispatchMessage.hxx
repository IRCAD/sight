/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DATACOM_DISPATCHMESSAGE_HXX_
#define _DATACOM_DISPATCHMESSAGE_HXX_

// STL
#include <string>
#include <vector>

// Boost
#include <boost/shared_ptr.hpp>

// Virtuals
#include <fwTools/ClassFactoryRegistry.hpp>

#include <fwServices/ObjectMsg.hpp>

// Current module
#include "fwComEd/ICallBack.hpp"
#include "fwComEd/macros.hpp"


namespace fwComEd
{

/**
 * @brief Call the defined callback functors for each modified field.
 *
 * @param[in] message The message containing the field modifications.
 * @param[in] client  The "client" object passed to the callback functor.
 * @param[in] always  Whether to always call the default callback regardless of the
 *                presence of remaining unhandled modifications.
 *
 * @return Whether every field modification has been handled by a callback.
 *
 * @author IRCAD (Research and Development Team).
 */
template< typename T >
bool DispatchMessage(
        const boost::shared_ptr< const fwServices::ObjectMsg > message,
        T* const client, const bool always = false )
{
        const std::vector< std::string > ids = message->getEventIds();
        // Comment by ACH
        // ::fwServices::ObjectMsg::sptr unhandled ( new fwServices::ObjectMsg( message->m_modifiedObject ) );
        bool fullyHandled = true;

        // Call the callback corresponding to every field modification
        for ( std::vector< std::string >::const_iterator iter = ids.begin();
              iter != ids.end();
              ++iter )
        {
                const boost::shared_ptr< ::fwComEd::ICallback > callback = ::fwTools::ClassFactoryRegistry::create< fwComEd::ICallbackFilter< T >, std::string >( *iter );

                if ( callback )
                {
                        (*callback)( message, client );
                        SLM_FATAL("ACH disptach msg used ?"); // If comment this fatal, reactivate cppunit test (CallbackTest)
                }
                else
                {
// Comment by ACH
//                      unhandled->addModif(    *iter,
//                                                                      message->getMsg( *iter ),
//                                                                      message->getProp( *iter ) );
                        OSLM_WARN("MSG filtering  : this event message ( " << *iter << " ) is not handled by DispatchMessage");
                        //fullyHandled = false;
                }
        }

        // If there is a field that hasn't been handled, call the default callback
        if ( always || !fullyHandled )
        {
                SLM_FATAL("ACH :  this cases exist in the framework ?");
//              const boost::shared_ptr< fwComEd::ICallback > callback =
//                  fwTools::ClassFactoryRegistry::create< fwComEd::ICallbackFilter< T >,
//                                                std::string >( defaultCallbackId );
//
//              if ( callback )
//              {
//                      (*callback)( unhandled, client );
//                      fullyHandled = true;
//              }
        }

        return fullyHandled;
}

} // namespace fwComEd

#endif // !_DATACOM_DISPATCHMESSAGE_HXX_
