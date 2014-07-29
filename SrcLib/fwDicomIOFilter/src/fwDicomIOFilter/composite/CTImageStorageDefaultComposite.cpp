/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/modifier/SliceThicknessModifier.hpp"
#include "fwDicomIOFilter/sorter/ImagePositionPatientSorter.hpp"
#include "fwDicomIOFilter/sorter/InstanceNumberSorter.hpp"
#include "fwDicomIOFilter/splitter/ImagePositionPatientSplitter.hpp"
#include "fwDicomIOFilter/splitter/SOPClassUIDSplitter.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"
#include "fwDicomIOFilter/composite/CTImageStorageDefaultComposite.hpp"

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::composite::CTImageStorageDefaultComposite );

namespace fwDicomIOFilter
{
namespace composite
{

const std::string CTImageStorageDefaultComposite::s_FILTER_NAME = "CTImageStorage default composite";
const std::string CTImageStorageDefaultComposite::s_FILTER_DESCRIPTION =
        "CTImageStorage default composite.";

//-----------------------------------------------------------------------------

CTImageStorageDefaultComposite::CTImageStorageDefaultComposite(::fwDicomIOFilter::IFilter::Key key) : IComposite()
{
    this->addChild(::fwDicomIOFilter::sorter::InstanceNumberSorter::New());
    this->addChild(::fwDicomIOFilter::sorter::ImagePositionPatientSorter::New());
    this->addChild(::fwDicomIOFilter::splitter::ImagePositionPatientSplitter::New());
    this->addChild(::fwDicomIOFilter::sorter::ImagePositionPatientSorter::New());
    this->addChild(::fwDicomIOFilter::modifier::SliceThicknessModifier::New());
}

//-----------------------------------------------------------------------------

CTImageStorageDefaultComposite::~CTImageStorageDefaultComposite()
{
}

//-----------------------------------------------------------------------------

std::string CTImageStorageDefaultComposite::getName() const
{
    return CTImageStorageDefaultComposite::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string CTImageStorageDefaultComposite::getDescription() const
{
    return CTImageStorageDefaultComposite::s_FILTER_DESCRIPTION;
}

} // namespace composite
} // namespace fwDicomIOFilter
