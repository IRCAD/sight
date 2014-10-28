/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_TEXT_HPP__
#define __VISUVTKADAPTOR_TEXT_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <string>


class vtkTextActor;
class vtkActor2D;
class vtkTextMapper;

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API Text: public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Text)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API Text();
    VISUVTKADAPTOR_API virtual ~Text() throw();

    VISUVTKADAPTOR_API virtual void setText(std::string str);
    virtual std::string getText(){return m_text;};

protected:

    VISUVTKADAPTOR_API virtual void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API virtual void doStop() throw(fwTools::Failed);
    virtual void doSwap() throw(fwTools::Failed){};
    virtual void doUpdate() throw(fwTools::Failed){};
    virtual void doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed){};

    /**
     * @brief Configure the adaptor
     *
     * Example :
     @verbatim
     <adaptor id="text" class="::visuVTKAdaptor::Text" objectId="imageSeries">
        <config renderer="default" text="@patient.name" fontSize="16" />
     </adaptor>
     @endverbatim
     *
     * - text : text to display. If a valid Seshat object attribute path is detected, the associated
     *   value is used. Otherwise, it uses the given value in XML configuration.
     * - fontSize : font size in points of displayed text (not mandatory, default value is set to 20)
     * - hAlign : horizontal alignment ('left' or 'right', not mandatory, default value is set to 'left')
     * - vAlign : vertical alignment ('top' or 'bottom', not mandatory, default value is set to 'bottom')
     */
    VISUVTKADAPTOR_API virtual void configuring() throw(fwTools::Failed);

    std::string m_text;

    vtkActor2D* m_actor;
    vtkTextMapper * m_mapper;

    /// Font size (in points) applied to VTK text mapper.
    unsigned int m_fontSize;

    /// Vertical alignment
    std::string m_vAlign;

    /// Horizontal alignment
    std::string m_hAlign;

private:
    void setAlignment();
};

} //namespace visuVTKAdaptor

#endif //__VISUVTKADAPTOR_TEXT_HPP__

