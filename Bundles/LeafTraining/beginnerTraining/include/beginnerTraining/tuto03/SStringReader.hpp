/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __BEGINNERTRAINING_TUTO03_SSTRINGREADER_HPP__
#define __BEGINNERTRAINING_TUTO03_SSTRINGREADER_HPP__

#include "beginnerTraining/config.hpp"

#include <io/IReader.hpp>

namespace beginnerTraining
{
namespace tuto03
{

/**
 * @brief   Reader that pushes the content of a text file in a ::fwData::String.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::beginnerTraining::tuto03::SStringReader">
            <inout key="targetString" uid="..." />
            <file>...</file>
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b string [::fwData::String]: output string.
 * @subsection Configuration Configuration:
 * - \b file: input file path.
 */
class BEGINNERTRAINING_CLASS_API SStringReader : public ::io::IReader
{
public:

    fwCoreServiceClassDefinitionsMacro( (SStringReader)(::io::IReader) );

    BEGINNERTRAINING_API SStringReader();
    BEGINNERTRAINING_API virtual ~SStringReader() noexcept;

protected:

    /// Overrides
    BEGINNERTRAINING_API virtual void starting() override;

    /// Overrides
    BEGINNERTRAINING_API virtual void stopping() override;

    /// Overrides
    BEGINNERTRAINING_API virtual void updating() override;

    /// Overrides io method
    BEGINNERTRAINING_API virtual void configureWithIHM() override;

    /// Overrides io method
    BEGINNERTRAINING_API virtual ::io::IOPathType getIOPathType() const override;

    /// Notify modification of associated data
    BEGINNERTRAINING_API void notifyMessage();
};

} // namespace tuto03
} // namespace beginnerTraining

#endif /*__BEGINNERTRAINING_TUTO03_SSTRINGREADER_HPP__*/

