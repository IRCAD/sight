/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _BEGINNERTRAINING_TUTO01_SSTRINGREADER_HPP_
#define _BEGINNERTRAINING_TUTO01_SSTRINGREADER_HPP_

#include <io/IReader.hpp>

#include "beginnerTraining/config.hpp"

namespace beginnerTraining
{
namespace tuto01
{

class BEGINNERTRAINING_CLASS_API SStringReader : public ::io::IReader
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SStringReader)(::io::IReader) ) ;

    /// Overrides
    BEGINNERTRAINING_API virtual void starting() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void stopping() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void updating() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void swapping() throw ( ::fwTools::Failed );

    /// Overrides io method
    BEGINNERTRAINING_API virtual void configureWithIHM();

    /// Overrides io method
    BEGINNERTRAINING_API virtual ::io::IOPathType getIOPathType() const;

protected :

    /// Constructor
    BEGINNERTRAINING_API SStringReader();

    /// Destructor
    BEGINNERTRAINING_API virtual ~SStringReader() throw();
};



} // namespace tuto01
} // namespace beginnerTraining

#endif /*_BEGINNERTRAINING_TUTO01_SSTRINGREADER_HPP_*/

