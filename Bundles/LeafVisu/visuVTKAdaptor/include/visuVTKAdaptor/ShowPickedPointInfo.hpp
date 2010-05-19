/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#ifndef VISUVTKADAPTOR_SHOWPICKEDPOINTINFO_HPP_
#define VISUVTKADAPTOR_SHOWPICKEDPOINTINFO_HPP_

///Require for Text
#include <vtkActor2D.h>

#include "visuVTKAdaptor/Text.hpp"
#include "visuVTKAdaptor/INegatoInteractor.hpp"
#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{

/**
 * @brief Display coordinates and intensity value of the picked point on the negatoscope and nothing if picking lies outside.
 */
class VISUVTKADAPTOR_CLASS_API ShowPickedPointInfo : public ::visuVTKAdaptor::INegatoInteractor
{
public:

    fwCoreServiceClassDefinitionsMacro ( (ShowPickedPointInfo)(::visuVTKAdaptor::INegatoInteractor::Baseclass) ) ;

    ///Constructor
    VISUVTKADAPTOR_API ShowPickedPointInfo() throw() ;
    ///Destructor
    VISUVTKADAPTOR_API virtual ~ShowPickedPointInfo() throw() ;


protected :

    /**
     * Process picking: update the text to be displayed and force rendering update
     * @note Should be done by doUpdate() method in order to preserve the genericity
     */
    VISUVTKADAPTOR_API virtual void doProcessPicking() throw(fwTools::Failed) ;

    /// Does nothing special
    VISUVTKADAPTOR_API virtual void doConfigure() throw(fwTools::Failed) {};
    /// Does nothing special
    VISUVTKADAPTOR_API void doUpdate(fwServices::ObjectMsg::csptr message ) throw(fwTools::Failed) {};

    /// Attach m_pickingCallBack to the interactor (filtering parameter = vtkCommand::AnyEvent)
    VISUVTKADAPTOR_API virtual void doStart() throw(fwTools::Failed);
    /// Detach m_pickingCallBack to the interactor
    VISUVTKADAPTOR_API virtual void doStop() throw(fwTools::Failed);

    /// Text display service
    ::visuVTKAdaptor::Text::wptr                             m_textAdaptor;
} ;

}

#endif /*VISUVTKADAPTOR_SHOWPICKEDPOINTINFO_HPP_*/
