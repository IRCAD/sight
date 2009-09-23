/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_STANDARDBUFFER_HPP_
#define _FWDATA_STANDARDBUFFER_HPP_

#include <fwCore/base.hpp>

#include "fwData/IBufferDelegate.hpp"
#include "fwData/config.hpp"


namespace fwData
{
/**
 * @class 	StandardBuffer
 * @brief 	This class defines a standard buffer. It is the default buffer class used in ::fwData::Image.
 * @see 	IBufferDelegate
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */


class FWDATA_CLASS_API StandardBuffer : public ::fwCore::BaseObject, public IBufferDelegate
{
public:
	fwCoreClassDefinitionsWithFactoryMacro( (StandardBuffer)(IBufferDelegate::Baseclass), (()), new StandardBuffer ) ;

	/// Constructor
	FWDATA_API StandardBuffer();

	/// Destructor
	FWDATA_API virtual ~StandardBuffer();

	/// Get buffer
	FWDATA_API virtual void *getBuffer();

	/// Set buffer
	FWDATA_API virtual void setBuffer(void *);
};

}

#endif /*_FWDATA_STANDARDBUFFER_HPP_*/
