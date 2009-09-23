/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_VISITOR_IVISITOR_HPP_
#define _FWDATA_VISITOR_IVISITOR_HPP_

#include "fwData/config.hpp"
#include "fwData/Object.hpp"

namespace fwData
{
/**
 * @namespace ::fwData::visitor
 * @brief representation of Design Pattern Vistor
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
namespace visitor
{

/**
 * @brief Interface for Visitor Pattern.
 * @warning : FIXME Visitor store father dataObject information : 2 successives call are not *yet* supported
 *             destroy then restart visitor
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 * @todo implement a reset father two allow 2 successive call
 */
class  FWDATA_CLASS_API IVisitor : public ::fwCore::BaseObject
{
public:
	fwCoreNonInstanciableClassDefinitionsMacro( (IVisitor) );

	FWDATA_API IVisitor();

	FWDATA_API virtual ~IVisitor();

	/**
	 * @brief to be specialized in sub class : define operation to perform on an object
	 */
	FWDATA_API virtual void visit( ::fwTools::Object::sptr obj)=0;

	/**
	 * @brief to be specialized in sub class : this function is called by object which inform
	 *  visitor of all its child. The specialization of this function allow ti change the behavior of the
	 *  visitor scan
	 *  @param[in] src define the father of all childs/fields
	 *  @param[in] children a container of Object which are child of current object
	 */
	FWDATA_API virtual void next( ::fwTools::Object::sptr src, ::fwTools::Object::ChildContainer &children)=0;

protected :

	/**
	 * @brief store information of previous hierarchical position of visitor
	 */
	::fwTools::Object::sptr m_source;


};

} // end namespace visitor

} // end namespace fwData


#endif // _FWDATA_VISITOR_IVISITOR_HPP_
