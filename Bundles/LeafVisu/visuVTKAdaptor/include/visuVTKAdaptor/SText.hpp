/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_STEXT_HPP__
#define __VISUVTKADAPTOR_STEXT_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

#include <string>

class vtkTextActor;
class vtkActor2D;
class vtkTextMapper;

namespace visuVTKAdaptor
{

/**
 * @brief Displays a text
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SText">
        <config renderer="default" text="@patient.name" color="#ff0000" fontSize="16" />
   </service>
   @endcode
 * or
 * @code{.xml}
   <service type="::visuVTKAdaptor::SText">
        <config renderer="default" vAlign='top' hAlign='center'>
        <text>SText to display<text>
   </service>
   @endcode
 *
 *  @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b text : text to display. If a valid camp object attribute path is detected, the associated
 *      value is used. Otherwise, it uses the given value in XML configuration.
 *    - \b color : text color in hexadecimal format : \#rrggbb or \#rrggbbaa (optional, default: white)
 *    - \b fontSize : font size in points of displayed text (optional, default: 20)
 *    - \b hAlign : horizontal alignment (left, center or right, optional, default: 'left')
 *    - \b vAlign : vertical alignment (top, center or bottom, optional, default: 'bottom')
 *
 * - \b text The configuration accepts also a text tag instead of the text attribute
 *   (see second example). This is useful for multiline text. The same rules
 *   that for the attribute are applied.
 */
class VISUVTKADAPTOR_CLASS_API SText : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SText)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SText();
    VISUVTKADAPTOR_API virtual ~SText() noexcept;

    VISUVTKADAPTOR_API virtual void setText(const std::string& str);
    //------------------------------------------------------------------------------

    std::string getText() const
    {
        return m_text;
    }

protected:

    VISUVTKADAPTOR_API virtual void configuring();
    VISUVTKADAPTOR_API virtual void starting();

    //------------------------------------------------------------------------------

    virtual void updating()
    {
    }

    VISUVTKADAPTOR_API virtual void stopping();

    std::string m_text;

    vtkActor2D* m_actor;
    vtkTextMapper* m_mapper;

    /// Font size (in points) applied to VTK text mapper.
    unsigned int m_fontSize;

    /// Vertical alignment
    std::string m_vAlign;

    /// Horizontal alignment
    std::string m_hAlign;

    /// Text color (hex format #ffffff)
    std::string m_textColor;

private:
    void setAlignment();
};

} //namespace visuVTKAdaptor

#endif //__VISUVTKADAPTOR_STEXT_HPP__

