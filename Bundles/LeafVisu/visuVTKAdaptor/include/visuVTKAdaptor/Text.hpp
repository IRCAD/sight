/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
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

class VISUVTKADAPTOR_CLASS_API Text : public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Text)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Text();
    VISUVTKADAPTOR_API virtual ~Text() throw();

    VISUVTKADAPTOR_API virtual void setText(const std::string &str);
    std::string getText() const
    {
        return m_text;
    }

protected:

    VISUVTKADAPTOR_API virtual void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API virtual void doStop() throw(fwTools::Failed);
    virtual void doSwap() throw(fwTools::Failed)
    {
    }
    virtual void doUpdate() throw(fwTools::Failed)
    {
    }

    /**
     * @brief Configure the adaptor
     *
     * Example :
       @code{.xml}
       <adaptor id="text" class="::visuVTKAdaptor::Text" objectId="imageSeries">
        <config renderer="default" text="@patient.name" color="#ff0000" fontSize="16" />
       </adaptor>

       <adaptor id="text2" class="::visuVTKAdaptor::Text" objectId="imageSeries">
        <config renderer="default" vAlign='top' hAlign='center'>
        <text>Text to display<text>
        </config>
       </adaptor>
       @endcode
     *
     * - text : text to display. If a valid Seshat object attribute path is detected, the associated
     *   value is used. Otherwise, it uses the given value in XML configuration.
     * - color : text color in hexadecimal format : \#rrggbb or \#rrggbbaa (optional, default: white)
     * - fontSize : font size in points of displayed text (optional, default: 20)
     * - hAlign : horizontal alignment (left, center or right, optional, default: 'left')
     * - vAlign : vertical alignment (top, center or bottom, optional, default: 'bottom')
     *
     *   The configuration accepts also a text tag instead of the text attribute
     *   (see second example). This is useful for multiline text. The same rules
     *   that for the attribute are applied.
     */
    VISUVTKADAPTOR_API virtual void doConfigure() throw(fwTools::Failed);

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

