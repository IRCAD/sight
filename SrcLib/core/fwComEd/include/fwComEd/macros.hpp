/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_MACROS_HPP_
#define _FWCOMED_MACROS_HPP_

#include <boost/preprocessor/cat.hpp>
#include <boost/shared_ptr.hpp>

#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

#include "fwComEd/ICallBack.hpp"


/**
 * @namespace	fwComEd
 * @brief		This namespace contains the classes used in communication edition.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 *
 * All specific message for ::fwData::Objects derived from ::fwServices::ObjectMsg are in this namespace.
 */
namespace fwComEd
{

/**
 * @brief The ID of the callback invoked when there is a field not handeled by
 *        a specific callback.
 */
extern FWCOMED_API const std::string defaultCallbackId;


/**
 * @brief Macro to call before the callback contents.
 *
 * @param name       The name of the defined callback class.
 * @param fieldId    The field ID this callback will be associated to.
 * @param clientType The class name of the object to be acted upon by the
 *                   callback.
 */
#define BEGIN_CALLBACK( name, fieldId, clientType )                     \
	struct name : public fwComEd::ICallbackFilter< clientType >         \
	{                                                                   \
		name() throw() {}                                               \
		virtual ~name() throw() {}                                      \
		virtual void operator()(                                        \
			boost::shared_ptr< const fwServices::ObjectMsg > message,   \
			void* rawClient ) throw( fwTools::Failed )                  \
		{                                                               \
			clientType* const client =                                  \
				reinterpret_cast< clientType* >( rawClient );           \
			assert( client );

/**
 * @brief Macro to call after the callback contents.
 *
 * @param name       The name of the defined callback class.
 * @param fieldId    The field ID this callback will be associated to.
 * @param clientType The class name of the object to be acted upon by the
 *                   callback.
 */
#define END_CALLBACK( name, fieldId, clientType )                  \
		}                                                          \
	};                                                             \
	REGISTER_BINDING( fwComEd::ICallbackFilter< clientType >, \
	                  name, std::string, fieldId )

/**
 * @brief Macro to call inside the callback contents to retrieve a field.
 * @param fieldId The ID of the field to retrieve.
 * @author Benjamin Gaillard
 */
#define CALLBACK_GET_FIELD( fieldId )                    \
	boost::dynamic_pointer_cast< ::fwTools::Field >(          \
		message->m_modifiedObject->getField( fieldId ) )

/**
 * @brief Build a unique callback name from the client class name.
 * @param clientClass The client class name.
 * @note Internal use only!
 */
#define _MAKE_CALLBACK_NAME( clientClass ) \
	BOOST_PP_CAT( _ ## clientClass ## _Callback_, __LINE__ )

/**
 * @brief "Register" a method to be called as a callback method with the message
 *        as parameter.
 *
 * Register a callback method which take a single argument of type
 * boost::shared_ptr< const fwServices::ObjectMsg >, being the message that
 * caused the callback to be called.
 *
 * This macro actually creates a callback class (derived from ICallback) that
 * will be instanciated when a message references a modification to a field
 * whose ID is the given one.  This is a helper macro which free programmers
 * from the burden of creating dozens of almost identical classes.
 *
 * @param fieldId      The field ID for which the method will be called.
 * @param clientClass  The class containing the method.
 * @param clientMethod The method name.
 */
#define REGISTER_CALLBACK( fieldId, clientClass, clientMethod )                \
	BEGIN_CALLBACK( _MAKE_CALLBACK_NAME( clientClass ), fieldId, clientClass ) \
		client->clientMethod( message );                                       \
	END_CALLBACK  ( _MAKE_CALLBACK_NAME( clientClass ), fieldId, clientClass )

/**
 * @brief "Register" a default callback method.
 *
 * Register a callback method which take a single argument of type
 * boost::shared_ptr< const fwServices::ObjectMsg >, being the message that
 * caused the callback to be called.  This method will be called whenever there
 * is a field modification which hasn't been handeled by another callback.
 *
 * @param clientClass  The class containing the method.
 * @param clientMethod The method name.
 *
 * @see REGISTER_CALLBACK
 */
#define REGISTER_DEFAULT_CALLBACK( clientClass, clientMethod) \
	REGISTER_CALLBACK( ::fwComEd::defaultCallbackId, clientClass, clientMethod )

} // namespace fwComEd

#endif // _FWCOMED_MACROS_HPP_
