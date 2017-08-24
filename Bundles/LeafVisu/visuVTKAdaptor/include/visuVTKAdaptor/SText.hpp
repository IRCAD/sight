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
   <service type="::visuVTKAdaptor::SText" autoConnect="yes">
        <in key="object" uid="..." />
        <config renderer="default" color="#ff4ae6" fontSize="20" fontFamily="courier" />
   </service>
   @endcode
 * or
 * @code{.xml}
   <service type="::visuVTKAdaptor::SText">
        <in key="object" uid="..." />
        <config renderer="default" text="@patient.name" color="#ff0000" fontSize="16" fontFamily="arial" />
   </service>
   @endcode
 * or
 * @code{.xml}
   <service type="::visuVTKAdaptor::SText">
        <config renderer="default" vAlign='top' hAlign='center' italic="false" bold="false" shadow="false" >
        <text>SText to display<text>
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b object [::fwData::Object] (optional): object displayed or used to find the camp path.
 *
 *  @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b text : text to display. If a valid camp object attribute path is detected, the associated
 *      value is used. Otherwise, the attribute is displayed as it is.
 *    - \b color : text color in hexadecimal format : \#rrggbb or \#rrggbbaa (optional, default: white)
 *    - \b fontSize : font size in points of displayed text (optional, default: 20)
 *    - \b fontFamily : font family (arial, courier or times, optional, default: 'courier')
 *    - \b italic : enables italic font (true or false, optional, default: 'false')
 *    - \b bold : enables bold font (true or false, optional, default: 'false')
 *    - \b shadow : enables text shadow (true or false, optional, default: 'false')
 *    - \b hAlign : horizontal alignment (left, center or right, optional, default: 'left')
 *    - \b vAlign : vertical alignment (top, center or bottom, optional, default: 'bottom')
 *
 * - \b text The configuration also accepts a text tag instead of the text attribute
 *   (see second example). This is useful for multiline text. The rules are the same as for the attributes.
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

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Object::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const;

protected:

    VISUVTKADAPTOR_API virtual void configuring();
    VISUVTKADAPTOR_API virtual void starting();
    VISUVTKADAPTOR_API virtual void updating();
    VISUVTKADAPTOR_API virtual void stopping();

    /// Displayed text.
    std::string m_text;

    /// Seshat path, empty if not used.
    std::string m_path;

    vtkActor2D* m_actor;
    vtkTextMapper* m_mapper;

    /// Font size (in points) applied to VTK text mapper.
    unsigned int m_fontSize;

    /// Font family. Only 'arial', 'courier' and 'times' are supported for now.
    std::string m_fontFamily;

    /// Enables italic font.
    bool m_italic;

    /// Enables bold font.
    bool m_bold;

    /// Enables text shadow.
    bool m_shadow;

    /// Vertical alignment
    std::string m_vAlign;

    /// Horizontal alignment
    std::string m_hAlign;

    /// Text color (hex format #ffffff)
    std::string m_textColor;

private:

    /// Set text position.
    void setAlignment();

    /// Sets the font size, family and options.
    void setStyle();

    /// Computes the text using the path and/or the object.
    void updateText();
};

} //namespace visuVTKAdaptor

#endif //__VISUVTKADAPTOR_STEXT_HPP__

