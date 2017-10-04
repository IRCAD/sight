/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __BASICREGISTRATION_SIMAGESSUBSTRACT_HPP__
#define __BASICREGISTRATION_SIMAGESSUBSTRACT_HPP__

#include "basicRegistration/config.hpp"

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QPushButton;

namespace basicRegistration
{

/**
 * @brief Compute the substraction of two images.

 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::basicRegistration::SImagesSubstract">
       <in key="image1" uid="..." />
       <in key="image2" uid="..." />
       <inout key="result" uid="..." />
   </service>
   @endcode
 * @subsection In In
 * - \b image1 [::fwData::Image]: first image.
 * - \b image2 [::fwData::Image]: second image.
 * @subsection InOut InOut
 * - \b result [::fwData::Image]: substract image.
 */

class BASICREGISTRATION_CLASS_API SImagesSubstract : public QObject,
                                                     public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SImagesSubstract)(::gui::editor::IEditor) );

    BASICREGISTRATION_API SImagesSubstract() noexcept;

    BASICREGISTRATION_API virtual ~SImagesSubstract() noexcept;

protected:

    BASICREGISTRATION_API virtual void configuring() override;

    /// Overrides
    BASICREGISTRATION_API virtual void starting() override;

    /// Overrides
    BASICREGISTRATION_API virtual void stopping() override;

    /// Overrides
    BASICREGISTRATION_API virtual void updating() override;

    /// Overrides
    BASICREGISTRATION_API virtual void swapping() override;

private Q_SLOTS:

    /// Compute the subtraction between two images.
    void OnCompute();

private:
    QPointer< QPushButton > mpComputeButton;
};

} // namespace basicRegistration

#endif /*__BASICREGISTRATION_SIMAGESSUBSTRACT_HPP__*/

