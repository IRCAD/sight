/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _opSofa_SOFASCENEREADERSRV_HPP_
#define _opSofa_SOFASCENEREADERSRV_HPP_

#include <iostream>
#include <fwTools/Failed.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <io/IReader.hpp>

namespace opSofa
{

/**
 * @brief Service allow to read file scn
 */
class SofaSceneReaderSrv : public ::io::IReader
{
    public :
        fwCoreServiceClassDefinitionsMacro ( (SofaSceneReaderSrv)(::io::IReader) ) ;

        SofaSceneReaderSrv() throw();
        virtual ~SofaSceneReaderSrv() throw();
        void configuring() throw ( ::fwTools::Failed );
        void starting() throw ( ::fwTools::Failed );
        void stopping() throw ( ::fwTools::Failed );
        void updating() throw ( ::fwTools::Failed );
        void receiving( ::fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed );
        void info( std::ostream & ostr );
        void configureWithIHM();

};

}

#endif // _opSofa_SOFASCENEREADERSRV_HPP_
