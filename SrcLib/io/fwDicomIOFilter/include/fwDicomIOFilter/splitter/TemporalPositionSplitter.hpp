/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWDICOMIOFILTER_SPLITTER_TEMPORALPOSITIONSPLITTER_HPP__
#define __FWDICOMIOFILTER_SPLITTER_TEMPORALPOSITIONSPLITTER_HPP__

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/splitter/TagValueSplitter.hpp"

namespace fwDicomIOFilter
{
namespace splitter
{

/**
 * @brief Filter that uses the TemporalPositionIdentifier tag to split the instances.
 */
class FWDICOMIOFILTER_CLASS_API TemporalPositionSplitter : public ::fwDicomIOFilter::splitter::TagValueSplitter
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (TemporalPositionSplitter)(ISplitter),
                                            (()), ::fwDicomIOFilter::factory::New< TemporalPositionSplitter > );

    /// Constructor
    FWDICOMIOFILTER_API TemporalPositionSplitter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~TemporalPositionSplitter();

    /// Return the name of the filter
    FWDICOMIOFILTER_API virtual std::string getName() const override;

    /// Return the description of the filter
    FWDICOMIOFILTER_API virtual std::string getDescription() const override;

    /// Return true if a configuration is required
    FWDICOMIOFILTER_API virtual bool isConfigurationRequired() const override;

protected:
    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;

};

} // namespace splitter
} // namespace fwDicomIOFilter

#endif /* __FWDICOMIOFILTER_SPLITTER_TEMPORALPOSITIONSPLITTER_HPP__ */
