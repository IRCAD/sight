#ifndef _opSofa_SOFASCENEWRITERSRV_HPP_
#define _opSofa_SOFASCENEWRITERSRV_HPP_

#include <iostream>
#include <fwTools/Failed.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <io/IWriter.hpp>

namespace opSofa
{

/**
 * @brief Service allow to create scn file
 */
class SofaSceneWriterSrv : public ::io::IWriter
{
    public :

        fwCoreServiceClassDefinitionsMacro ( (SofaSceneWriterSrv)(::io::IWriter) ) ;

        SofaSceneWriterSrv() throw();
        virtual ~SofaSceneWriterSrv() throw();
        void configuring() throw ( ::fwTools::Failed );
        void starting() throw ( ::fwTools::Failed );
        void stopping() throw ( ::fwTools::Failed );
        void updating() throw ( ::fwTools::Failed );
        void updating( ::fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed );
        void info( std::ostream & ostr );
        void configureWithIHM();

    private :
        bool writeTrian;
        std::string m_sceneTemplate;
        bool m_useTempPath;

};

}

#endif // _opSofa_SOFASCENEWRITERSRV_HPP_
