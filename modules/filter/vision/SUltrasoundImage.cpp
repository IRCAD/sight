/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "SUltrasoundImage.hpp"

#include <io/opencv/Image.hpp>

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <core/Profiling.hpp>

#include <data/fieldHelper/MedicalImageHelpers.hpp>

#include <service/macros.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

namespace sight::module::filter::vision
{

static const std::string s_THRESHOLD_MIN       = "threshold.min";
static const std::string s_US_DEPTH            = "us.depth";
static const std::string s_DEBUG_US_EXTRACTION = "debugUSExtraction";
static const std::string s_ECHO_SHAPE          = "shape";

const core::com::Slots::SlotKeyType s_UPDATE_INT_EXTRACTION_PARAMETERS_SLOT    = "updateIntExtractionParameter";
const core::com::Slots::SlotKeyType s_UPDATE_DOUBLE_EXTRACTION_PARAMETERS_SLOT = "updateDoubleExtractionParameter";
const core::com::Slots::SlotKeyType s_RESET_ECHO_PLANE_SLOT                    = "resetEchoPlane";
const core::com::Slots::SlotKeyType s_CHANGE_DEBUG_ECHO_PLANE_SLOT             = "changeDebugEchoPlane";
const core::com::Slots::SlotKeyType s_CHANGE_ECHO_SHAPE_SLOT                   = "changeEchoShape";

static const core::com::Signals::SignalKeyType s_INTEGER_CHANGED_SIG = "intChanged";

static const std::string s_THRESHOLD_MIN_CONFIG = "thresholdMin";
static const std::string s_US_DEPTH_CONFIG      = "USDepth";
static const std::string s_DEBUG_CONFIG         = "debug";
static const std::string s_CONVEX_SHAPE         = "convex";

// -----------------------------------------------------------------------------

SUltrasoundImage::SUltrasoundImage() noexcept
{
    newSlot(s_UPDATE_INT_EXTRACTION_PARAMETERS_SLOT, &SUltrasoundImage::updateIntExtractionParameter, this);
    newSlot(s_UPDATE_DOUBLE_EXTRACTION_PARAMETERS_SLOT, &SUltrasoundImage::updateDoubleExtractionParameter, this);
    newSlot(s_RESET_ECHO_PLANE_SLOT, &SUltrasoundImage::resetEchoPlane, this);
    newSlot(s_CHANGE_DEBUG_ECHO_PLANE_SLOT, &SUltrasoundImage::changeDebugEchoPlane, this);
    newSlot(s_CHANGE_ECHO_SHAPE_SLOT, &SUltrasoundImage::changeEchoShape, this);

    newSignal<IntegerChangedSignalType>(s_INTEGER_CHANGED_SIG);

    // Extraction corner reference points of US image initialization.
    for(size_t i = 0 ; i < m_echoRefPoints.size() ; ++i)
    {
        m_echoRefPoints[i] = {{0.0, 0.0, 0.0}};
    }
}

// -----------------------------------------------------------------------------

SUltrasoundImage::~SUltrasoundImage() noexcept
{
}

// -----------------------------------------------------------------------------

void SUltrasoundImage::configuring()
{
    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_thresholdMin = config.get<int>(s_THRESHOLD_MIN_CONFIG, m_thresholdMin);
    m_usDepth      = config.get<double>(s_US_DEPTH_CONFIG, m_usDepth);
    m_debug        = config.get<bool>(s_DEBUG_CONFIG, false);
    m_convexShape  = config.get<bool>(s_CONVEX_SHAPE, true);
}

// -----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SUltrasoundImage::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_ULTRASOUND_IMAGE_INPUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

// -----------------------------------------------------------------------------

void SUltrasoundImage::starting()
{
}

// -----------------------------------------------------------------------------

void SUltrasoundImage::stopping()
{
}

// -----------------------------------------------------------------------------

void SUltrasoundImage::updating()
{
    // HACK: Const cast to avoid a useless copy, fix this by overloading the `moveToCv` function to
    // take a `data::Image::cpstr` and output `const cv::Mat`.
    const auto constImage        = m_ultrasoudImage.lock();
    data::Image::sptr inputImage = data::Image::constCast(constImage.get_shared());
    SIGHT_ASSERT("Missing input frame.", inputImage);

    const bool isValid = data::fieldHelper::MedicalImageHelpers::checkImageValidity(inputImage);
    if(!isValid)
    {
        return;
    }

    const cv::Mat src = io::opencv::Image::moveToCv(inputImage);

    m_probeSettings.matrixWidth = 1024;
    m_probeSettings.matrixDepth = 1024;
    m_probeSettings.simuEchoOn  = false;

    if(!m_probeSettingsInitialized)
    {
        this->process(src);

        if(!m_probeSettingsInitialized)
        {
            return;
        }

        this->updateBeamExtractionMap();

        // Compute a scaling factor between the pixel read from the image to the real world size.
        // This factor must be set in the user interface, by reading the current probe depth
        // US depth is in centimeters, and tracking device uses millimeters.
        const double scaling = m_usDepth * 10.0 / m_probeSettings.depth;

        const int scaledDepth = static_cast<int>(std::lround(m_probeSettings.depth * scaling));
        const int scaledWidth = static_cast<int>(std::lround(m_probeSettings.width * scaling));

        this->signal<IntegerChangedSignalType>(s_INTEGER_CHANGED_SIG)->asyncEmit(scaledDepth, "depth");
        this->signal<IntegerChangedSignalType>(s_INTEGER_CHANGED_SIG)->asyncEmit(scaledWidth, "width");
        this->signal<IntegerChangedSignalType>(s_INTEGER_CHANGED_SIG)->asyncEmit(m_probeSettings.angle, "angle");
    }

    cv::Mat srcGray;
    cv::cvtColor(src, srcGray, cv::COLOR_RGB2GRAY);

    auto outputImage = m_extractedUltrasoundBeam.lock();
    SIGHT_ASSERT("Missing output frame.", outputImage);

    const data::Image::Size outputSize = {{m_probeSettings.matrixWidth, m_probeSettings.matrixDepth, 0}};

    if(outputImage->getSize() != outputSize || outputImage->getType() != core::tools::Type::s_UINT8
       || outputImage->getNumberOfComponents() != 1)
    {
        outputImage->setSpacing({{1., 1., 0.}});
        outputImage->setOrigin({{0., 0., 0.}});
        outputImage->resize(outputSize, core::tools::Type::s_UINT8, data::Image::PixelFormat::GRAY_SCALE);
    }

    cv::Mat remapResult = io::opencv::Image::moveToCv(outputImage.get_shared());
    cv::remap(srcGray, remapResult, m_extractionMap, cv::Mat(), cv::INTER_LINEAR);

    auto sig = outputImage->signal<data::Image::BufferModifiedSignalType>(data::Image::s_BUFFER_MODIFIED_SIG);
    sig->asyncEmit();
}

// -----------------------------------------------------------------------------

cv::Vec2d SUltrasoundImage::computeLineEquation(const cv::Vec2d& p1, const cv::Vec2d& p2) const
{
    double diff = p2[0] - p1[0];
    cv::Vec2d eq;
    eq[0] = eq[1] = 0.0;
    if(std::abs(diff) > std::numeric_limits<double>::epsilon())
    {
        const double diffY = p2[1] - p1[1];
        if(std::abs(diffY) < std::numeric_limits<double>::epsilon())
        {
            // It means that it's a horizontal line.
            eq[0] = std::numeric_limits<double>::infinity();
            eq[1] = p1[1];
        }
        else
        {
            eq[0] = (p2[1] - p1[1]) / diff;
            eq[1] = p1[1] - eq[0] * p1[0];
        }
    }
    else
    {
        // It means that it's a vertical line.
        eq[0] = p1[0];
        eq[1] = std::numeric_limits<double>::infinity();
    }

    return eq;
}

// -----------------------------------------------------------------------------

cv::Vec2d SUltrasoundImage::computeLinesIntersection(const cv::Vec2d& eql1, const cv::Vec2d& eql2) const
{
    // { y = a1x + b1
    // { y = a2x + b2
    // a1x + b1 = a2x + b2
    // (a1 - a2)x = b2 - b1

    double diff = (eql1[0] - eql2[0]);
    cv::Vec2d inter;
    inter[0] = inter[1] = 0.0;

    if(std::abs(diff) > std::numeric_limits<double>::epsilon())
    {
        inter[0] = (eql2[1] - eql1[1]) / diff;
        inter[1] = eql1[0] * inter[0] + eql1[1];
    }

    return inter;
}

// -----------------------------------------------------------------------------

std::vector<cv::Vec2d> SUltrasoundImage::computeLineCircleIntersection(
    const cv::Vec2d& eql,
    const cv::Vec2d& center,
    const double R
) const
{
    // Work on:
    // { ax + b = y
    // { (x - x_c)^2 + (y - y_c)^2 = R^2
    // And you get:
    // x^2 * (1 + a^2) + x * ( 2 * ( a * ( b - y_c ) - x_c ) ) + ( (b - y_c)^2 + x_c^2 - R^2 ) = 0
    // Which is in the form au Ax + By + C = 0, 2nd order polynomial

    std::vector<cv::Vec2d> inter;

    double A = 1.0 + std::pow(eql[0], 2);
    double B = 2.0 * (eql[0] * (eql[1] - center[1]) - center[0]);
    double C = std::pow(eql[1] - center[1], 2) + std::pow(center[0], 2) - std::pow(R, 2);

    double delta = std::pow(B, 2) - 4.0 * A * C;

    // We only handle the case where we are 2 intersections.
    if(delta > std::numeric_limits<double>::epsilon())
    {
        double x, y;
        x = (-B - sqrt(delta)) / (2.0 * A);
        y = eql[0] * x + eql[1];
        inter.push_back(cv::Vec2d(x, y));

        x = (-B + sqrt(delta)) / (2.0 * A);
        y = eql[0] * x + eql[1];
        inter.push_back(cv::Vec2d(x, y));
    }

    return inter;
}

// -----------------------------------------------------------------------------

void SUltrasoundImage::process(const cv::Mat& input)
{
    // Process the input image with OpenCV.
    cv::Mat gray, thresh, lowThresh, canny, stats, centroids, labels, binaryConvex;

    cv::cvtColor(input, gray, cv::COLOR_RGB2GRAY);

    cv::threshold(gray, thresh, m_thresholdMin, 255, cv::THRESH_BINARY);

    cv::connectedComponentsWithStats(thresh, labels, stats, centroids);

    int maxPixelsRow(0);
    // Get max label (except 0 which is the entire image) and associated position in the stats table
    // maxPixelsRow == stat table row position == label value in labels image
    for(int i = 1, maxPixelsValue = 0 ; i < stats.rows ; ++i)
    {
        if(stats.at<int>(i, cv::CC_STAT_AREA) > maxPixelsValue)
        {
            maxPixelsValue = stats.at<int>(i, cv::CC_STAT_AREA);
            maxPixelsRow   = i;
        }
    }

    // Get object with label = maxPixelRow un labels image.
    // This step extract a binary image with the convex shape
    cv::compare(labels, maxPixelsRow, binaryConvex, cv::CMP_EQ);

    // 0, 1 to detect the edges of the US plane in the binary image
    cv::Canny(binaryConvex, canny, 0, 1);

    std::vector<cv::Vec4d> lines;
    // 1 is the resolution in pixels; PI/180 = 1 degree is the resolution in radians;
    // 80 is the minimum number of intersections to detect a line;
    // 30 is the minimum number of points that can form a line
    // 10 is the maximum gap between two points to be consider in the same line
    // for more information, check
    // http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/hough_lines/hough_lines.html
    cv::HoughLinesP(canny, lines, 1, CV_PI / 180, 80, 30, 10);

    std::vector<cv::Vec2d> foundLines;

    // Compute a new threshold to do dilatation and erosion on it
    cv::threshold(gray, lowThresh, 2, 255, cv::THRESH_BINARY);
    // Construct element type
    int elementType = cv::MORPH_CROSS;
    // Choose element size
    int elementDilateSize(3), elementErodeSize(5);
    // Construct dilatation and erosion elements
    cv::Mat elementDilate = cv::getStructuringElement(
        elementType,
        cv::Size(2 * elementDilateSize + 1, 2 * elementDilateSize + 1),
        cv::Point(elementDilateSize, elementDilateSize)
    );
    cv::Mat elementErode = cv::getStructuringElement(
        elementType,
        cv::Size(2 * elementErodeSize + 1, 2 * elementErodeSize + 1),
        cv::Point(elementErodeSize, elementErodeSize)
    );
    // Perform dilatation then erosion (closing)
    cv::dilate(lowThresh, lowThresh, elementDilate);
    cv::erode(lowThresh, lowThresh, elementErode);

    // Compute final lines
    for(size_t i = 0 ; i < lines.size() ; i++)
    {
        cv::Vec4d l = lines.at(i);

        cv::Vec2d pt1(l[0], l[1]);
        cv::Vec2d pt2(l[2], l[3]);

        /* Compute the associated line equation */
        const cv::Vec2d eq = computeLineEquation(pt1, pt2);

        // Get greyscale value under the two line ends
        uchar pt1Value = lowThresh.at<uchar>(static_cast<int>(pt1[1]), static_cast<int>(pt1[0]));
        uchar pt2Value = lowThresh.at<uchar>(static_cast<int>(pt2[1]), static_cast<int>(pt2[0]));

        /* Ensure the slope is empirically ok to avoid horizontal and vertical lines */
        /* Check also if the values under the two line points aren't inside the US image*/
        if(m_convexShape)
        {
            if(std::abs(eq[0]) > 0.5 && std::abs(eq[0]) < 10 && pt1Value < 255 && pt2Value < 255)
            {
                cv::line(
                    input,
                    cv::Point2d(pt1[0], pt1[1]),
                    cv::Point2d(pt2[0], pt2[1]),
                    cv::Scalar(0, 0, 255, 255),
                    1,
                    4
                );

                cv::line(
                    lowThresh,
                    cv::Point2d(pt1[0], pt1[1]),
                    cv::Point2d(pt2[0], pt2[1]),
                    cv::Scalar(0, 0, 255, 255),
                    1,
                    4
                );

                foundLines.push_back(eq);
            }
        }
        else
        {
            if(std::isinf(eq[1])) // Vertical lines
            {
                cv::line(
                    input,
                    cv::Point2d(pt1[0], pt1[1]),
                    cv::Point2d(pt2[0], pt2[1]),
                    cv::Scalar(0, 0, 255, 255),
                    1,
                    4
                );
                foundLines.push_back(eq);
            }

            if(std::isinf(eq[0])) // Horizontal lines
            {
                cv::line(
                    input,
                    cv::Point2d(pt1[0], pt1[1]),
                    cv::Point2d(pt2[0], pt2[1]),
                    cv::Scalar(255, 0, 0, 255),
                    1,
                    4
                );
                foundLines.push_back(eq);
            }
        }
    }

    if(m_debug)
    {
        cv::imshow("Extracted image", input);
    }

    // If we did not find any intersection.
    // We keep return from the function and keep the previous parameters
    // as we won't be able to compute new ones.
    if(foundLines.size() < 2)
    {
        SIGHT_ERROR("Less than 2 lines were found (can't compute an intersection).");
        return;
    }

    /* Display information about the intersection lines found */
    SIGHT_DEBUG("Number of lines found: " << foundLines.size());
    for(size_t i = 0 ; i < foundLines.size() ; i++)
    {
        cv::Vec2f v = foundLines.at(i);
        SIGHT_DEBUG(" - " << v[0] << " * x + " << v[1]);
    }

    if(m_convexShape)
    {
        m_probeSettings = processConvexShape(input, foundLines, thresh, binaryConvex);
    }
    else
    {
        m_probeSettings = processRectangularShape(foundLines);
    }

    m_probeSettingsInitialized = true;

    SIGHT_DEBUG("echoShapeOn: " << m_probeSettings.echoShapeOn);
    SIGHT_DEBUG("depth: " << m_probeSettings.depth);
    SIGHT_DEBUG("width: " << m_probeSettings.width);
    SIGHT_DEBUG("angle: " << m_probeSettings.angle);
    SIGHT_DEBUG("deltaDepth: " << m_probeSettings.deltaDepth);
    SIGHT_DEBUG(
        "centerPosition: " << m_probeSettings.centerPosition[0] << " " << m_probeSettings.centerPosition[1] << " "
        << m_probeSettings.centerPosition[2]
    );
    SIGHT_DEBUG("direction: " << m_probeSettings.direction[0] << " " << m_probeSettings.direction[1]);
    SIGHT_DEBUG("normal: " << m_probeSettings.normal[0] << " " << m_probeSettings.normal[1]);
}

// -----------------------------------------------------------------------------

SUltrasoundImage::ProbeSimulationSettings SUltrasoundImage::processRectangularShape(
    std::vector<cv::Vec2d> lines
)
{
    // We initialize a probeSettings structure with the previous parameters.
    ProbeSimulationSettings probeSettings = m_probeSettings;

    int nbHorizontalLines(0), nbVerticalLines(0);
    cv::Vec2d meanCenter(0.0, 0.0);
    for(size_t i = 0 ; i < lines.size() ; i++)
    {
        if(std::isinf(lines.at(i)[0]))
        {
            meanCenter[1] += lines.at(i)[1];
            nbHorizontalLines++;
        }
        else
        {
            meanCenter[0] += lines.at(i)[0];
            nbVerticalLines++;
        }
    }

    meanCenter[0] /= nbVerticalLines;
    meanCenter[1] /= nbHorizontalLines;

    double meanLeftX(0.0), meanRightX(0.0), meanTopY(0.0), meanBottomY(0.0);
    int nbLeftLines(0), nbTopLines(0);
    for(size_t i = 0 ; i < lines.size() ; i++)
    {
        cv::Vec2d currentLine = lines.at(i);

        // It means that it's a vertical line
        if(std::isinf(currentLine[1]))
        {
            if(currentLine[0] < meanCenter[0])
            {
                meanLeftX += currentLine[0];
                nbLeftLines++;
            }
            else
            {
                meanRightX += currentLine[0];
            }
        }
        else // a horizontal line
        {
            if(currentLine[1] < meanCenter[1])
            {
                meanTopY += currentLine[1];
                nbTopLines++;
            }
            else
            {
                meanBottomY += currentLine[1];
            }
        }
    }

    if(nbLeftLines == 0 || nbLeftLines == nbVerticalLines)
    {
        SIGHT_WARN("There is only on stack of vertical lines. We can't extract a rectangular shape.");
        return probeSettings;
    }

    if(nbTopLines == 0 || nbTopLines == nbHorizontalLines)
    {
        SIGHT_WARN("There is only on stack of horizontal lines. We can't extract a rectangular shape.");
        return probeSettings;
    }

    meanLeftX  /= nbLeftLines;
    meanRightX /= (nbVerticalLines - nbLeftLines);

    meanTopY    /= nbTopLines;
    meanBottomY /= (nbHorizontalLines - nbTopLines);

    // meanCenter need to be updated because, if we have more line on one side, it will shift the center to this side
    meanCenter[0] = (meanLeftX + meanRightX) / 2.0;
    meanCenter[1] = (meanBottomY + meanTopY) / 2.0;

    // Fill the probeSettings accordingly to extracted lines
    probeSettings.centerPosition[0] = meanCenter[0];
    probeSettings.centerPosition[1] = meanTopY;
    probeSettings.centerPosition[2] = 0.0;
    probeSettings.direction         = {{0.0, 1.0, 0.0}};
    probeSettings.deltaDepth        = 0;
    probeSettings.normal[0]         = 1.0;
    probeSettings.normal[1]         = 0.0;
    probeSettings.normal[2]         = 0.0;
    probeSettings.angle             = 0;
    probeSettings.depth             = static_cast<int>(meanBottomY - meanTopY);
    probeSettings.width             = static_cast<int>(meanRightX - meanLeftX);
    probeSettings.echoShapeOn       = false;

    SIGHT_DEBUG("nbHorizontalLines: " << nbHorizontalLines);
    SIGHT_DEBUG("nbVerticalLines: " << nbVerticalLines);
    SIGHT_DEBUG("meanLeftX: " << meanLeftX);
    SIGHT_DEBUG("meanRightX: " << meanRightX);
    SIGHT_DEBUG("meanTopY: " << meanTopY);
    SIGHT_DEBUG("meanBottomY: " << meanBottomY);
    SIGHT_DEBUG("nbLeftLines: " << nbLeftLines);
    SIGHT_DEBUG("nbRightLines: " << (nbVerticalLines - nbLeftLines));
    SIGHT_DEBUG("nbTopLines: " << nbTopLines);
    SIGHT_DEBUG("nbBottomLines: " << (nbHorizontalLines - nbTopLines));

    return probeSettings;
}

// -----------------------------------------------------------------------------

SUltrasoundImage::ProbeSimulationSettings SUltrasoundImage::processConvexShape(
    cv::Mat input,
    std::vector<cv::Vec2d> lines,
    cv::Mat thresh,
    cv::Mat binaryConvex
)
{
    // We initialize a probeSettings structure with the previous parameters.
    ProbeSimulationSettings probeSettings = m_probeSettings;

    // Compute the intersection between the lines found.
    cv::Vec2d ascLine;
    cv::Vec2d descLine;

    int discardCount = 0;

    cv::Vec2d meanInter;
    int nbPoints = 0;
    meanInter[0] = meanInter[1] = 0.0;

    // Compute all the intersection to find the closest one to the a specific point.
    for(size_t i = 0 ; i < lines.size() ; i++)
    {
        for(size_t j = 0 ; j < lines.size() ; j++)
        {
            /* Avoid auto intersecting lines */
            if(i == j)
            {
                continue;
            }

            const cv::Vec2d line1 = lines.at(i);
            const cv::Vec2d line2 = lines.at(j);
            // The two lines must have a slope that is of different signs (one positive and one negative)
            // to be candidates for ultrasound center computation.
            if((line1[0] > 0 && line2[0] > 0) || (line1[0] < 0 && line2[0] < 0))
            {
                discardCount++;
                continue;
            }

            const cv::Vec2d inter = computeLinesIntersection(line1, line2);

            meanInter[0] += inter[0];
            meanInter[1] += inter[1];
            nbPoints++;

            /* Sort the ascending and descending line */
            if(line1[0] > 0 && line2[0] < 0)
            {
                ascLine  = line1;
                descLine = line2;
            }
            else
            {
                ascLine  = line2;
                descLine = line1;
            }
        }
    }

    cv::Vec2d center;
    center[0] = meanInter[0] / static_cast<double>(nbPoints);
    center[1] = meanInter[1] / static_cast<double>(nbPoints);

    // Check if the center found is withing the correct bounds of the image
    // and if it is not a nan.
    if(center[0] < 0 || center[0] >= input.rows || std::isnan(center[0]))
    {
        return probeSettings;
    }

    probeSettings.centerPosition[0] = center[0];
    probeSettings.centerPosition[1] = center[1];
    probeSettings.centerPosition[2] = 0.0;

    SIGHT_DEBUG("Discarded " << discardCount << " line(s) intersection with same sign lines ");

    // Compute the inner radius.
    // Go straight down from the center of the circle previously found.
    int iIn;
    for(iIn = 0 ; iIn < thresh.rows ; iIn++)
    {
        // You can now access the pixel value
        uchar v = thresh.at<uchar>(iIn, static_cast<int>(center[0]));

        if(v != 0)
        {
            break;
        }
    }

    double innerRadius;
    cv::Vec2d interInnerCircle = center;
    interInnerCircle[1] = static_cast<double>(iIn);

    probeSettings.direction = {{0.0, 1.0, 0.0}};

    innerRadius = std::abs(interInnerCircle[1] - center[1]);
    SIGHT_DEBUG("innerRadius=" << innerRadius);

    probeSettings.deltaDepth = static_cast<int>(std::nearbyint(innerRadius));

    // Compute the intersection between the lines and the inner circle
    // Compute the intersection with the descending line.
    const std::vector<cv::Vec2d> lcInterDesc = computeLineCircleIntersection(descLine, center, innerRadius);

    if(lcInterDesc.size() != 2)
    {
        SIGHT_DEBUG("No Circle-Line intersection detected " << lcInterDesc.size());
    }
    else
    {
        // Look for the point with the highest y coordinate (the one that is in our picture).
        if(lcInterDesc[0][1] > lcInterDesc[1][1])
        {
            m_echoRefPoints[0][0] = lcInterDesc[0][0];
            m_echoRefPoints[0][1] = lcInterDesc[0][1];
            m_echoRefPoints[0][2] = 0.0;
        }
        else
        {
            m_echoRefPoints[0][0] = lcInterDesc[1][0];
            m_echoRefPoints[0][1] = lcInterDesc[1][1];
            m_echoRefPoints[0][2] = 0.0;
        }
    }

    // Compute the intersection with the ascending line.
    const std::vector<cv::Vec2d> lcInterAsc = computeLineCircleIntersection(ascLine, center, innerRadius);

    if(lcInterAsc.size() != 2)
    {
        SIGHT_DEBUG("No Circle-Line intersection detected " << lcInterAsc.size());
    }
    else
    {
        // Look for the point with the highest y coordinate (the one that is in our picture).
        if(lcInterAsc[0][1] > lcInterAsc[1][1])
        {
            m_echoRefPoints[1][0] = lcInterAsc[0][0];
            m_echoRefPoints[1][1] = lcInterAsc[0][1];
            m_echoRefPoints[1][2] = 0.0;
        }
        else
        {
            m_echoRefPoints[1][0] = lcInterAsc[1][0];
            m_echoRefPoints[1][1] = lcInterAsc[1][1];
            m_echoRefPoints[1][2] = 0.0;
        }
    }

    SIGHT_DEBUG("A:" << m_echoRefPoints[0][0] << " " << m_echoRefPoints[0][1]);
    SIGHT_DEBUG("B:" << m_echoRefPoints[1][0] << " " << m_echoRefPoints[1][1]);

    fwVec3d AmC, BmC;
    ::glm::dvec3 BmA;

    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        BmA[i] = m_echoRefPoints[1][i] - m_echoRefPoints[0][i];

        AmC[i] = m_echoRefPoints[0][i] - probeSettings.centerPosition[i];
        BmC[i] = m_echoRefPoints[1][i] - probeSettings.centerPosition[i];
    }

    BmA = ::glm::normalize(BmA);

    ::glm::dvec3 v1, v2;

    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        probeSettings.normal[i] = BmA[i];

        v1[i] = AmC[i];
        v2[i] = BmC[i];
    }

    v1                  = ::glm::normalize(v1);
    v2                  = ::glm::normalize(v2);
    probeSettings.angle = static_cast<int>(std::nearbyint(::glm::degrees(::glm::angle(v1, v2))));

    // Compute the outer radius.
    int iOut;
    for(iOut = binaryConvex.rows - 1 ; iOut >= 0 ; iOut--)
    {
        // You can now access the pixel value
        uchar v = binaryConvex.at<uchar>(iOut, static_cast<int>(center[0]));

        if(v != 0)
        {
            break;
        }
    }

    cv::Vec2d interOuterCircle = center;
    interOuterCircle[1] = iOut;

    double outerRadius = std::abs(interOuterCircle[1] - center[1]);

    SIGHT_DEBUG("outerRadius=" << outerRadius);

    const std::vector<cv::Vec2d> c1 = computeLineCircleIntersection(ascLine, center, outerRadius);
    const std::vector<cv::Vec2d> c2 = computeLineCircleIntersection(descLine, center, outerRadius);
    // compute start and end angles of the bottom arc of the US image
    int startAngle = static_cast<int>(computeArcAngle(center, c1));
    int endAngle   = static_cast<int>(computeArcAngle(center, c2));

    // 90 degrees because openCV needs angle from the horizontal
    startAngle = 90 - std::abs(startAngle) + 30; // +30 degrees to avoid right text on the US image extraction
    endAngle   = (90 + std::abs(endAngle)) - 10; // -10 degrees to avoid left dot on the US image extraction

    std::vector<cv::Point2d> arcPoints;
    // get points list along an arc equation to check if there is data != 0 under this arc
    cv::ellipse2Poly(
        cv::Point2d(center[0], center[1]),
        cv::Size2d(
            outerRadius,
            outerRadius
        ),
        0,
        startAngle,
        endAngle,
        1,
        arcPoints
    );
    bool dataUnderArc = isDataUnderArc(thresh, arcPoints);

    // go further the bottom of the US image
    while(dataUnderArc)
    {
        outerRadius += 30; // 30: big step to find the bottom pixel of the US image != 0
        cv::ellipse2Poly(
            cv::Point2d(center[0], center[1]),
            cv::Size2d(
                outerRadius,
                outerRadius
            ),
            0,
            startAngle,
            endAngle,
            1,
            arcPoints
        );
        dataUnderArc = isDataUnderArc(thresh, arcPoints);
    }

    // go backward to find the bottom pixel of the US image != 0
    while(!dataUnderArc)
    {
        outerRadius--;
        cv::ellipse2Poly(
            cv::Point2d(center[0], center[1]),
            cv::Size2d(
                outerRadius,
                outerRadius
            ),
            0,
            startAngle,
            endAngle,
            1,
            arcPoints
        );
        dataUnderArc = isDataUnderArc(thresh, arcPoints);
    }

    cv::circle(
        input,
        cv::Point2d(center[0], center[1]),
        static_cast<int>(innerRadius),
        cv::Scalar(
            0,
            255,
            0,
            255
        ),
        1,
        cv::LINE_AA
    );

    cv::ellipse(
        input,
        cv::Point2d(center[0], center[1]),
        cv::Size2d(
            outerRadius,
            outerRadius
        ),
        0.0,
        startAngle,
        endAngle,
        cv::Scalar(255, 255, 0, 255),
        1,
        cv::LINE_AA
    );

    if(m_debug)
    {
        cv::imshow("Extracted image", input);
    }

    // Compute the intersection between the lines and the outer circle.
    // Compute the intersection with the descending line.
    const std::vector<cv::Vec2d> lcExterDesc = computeLineCircleIntersection(descLine, center, outerRadius);

    if(lcExterDesc.size() != 2)
    {
        SIGHT_DEBUG("No Circle-Line intersection detected " << lcExterDesc.size());
    }
    else
    {
        /* Look for the point with the highest y coordinate (the one that is in our picture) */
        if(lcExterDesc[0][1] > lcExterDesc[1][1])
        {
            m_echoRefPoints[2][0] = lcExterDesc[0][0];
            m_echoRefPoints[2][1] = lcExterDesc[0][1];
            m_echoRefPoints[2][2] = 0.0;
        }
        else
        {
            m_echoRefPoints[2][0] = lcExterDesc[1][0];
            m_echoRefPoints[2][1] = lcExterDesc[1][1];
            m_echoRefPoints[2][2] = 0.0;
        }
    }

    // Compute the intersection with the ascending line.
    const std::vector<cv::Vec2d> lcExterAsc = computeLineCircleIntersection(ascLine, center, outerRadius);

    if(lcExterAsc.size() != 2)
    {
        SIGHT_DEBUG("No Circle-Line intersection detected " << lcExterAsc.size());
    }
    else
    {
        // Look for the point with the highest y coordinate (the one that is in our picture).
        if(lcExterAsc[0][1] > lcExterAsc[1][1])
        {
            m_echoRefPoints[3][0] = lcExterAsc[0][0];
            m_echoRefPoints[3][1] = lcExterAsc[0][1];
            m_echoRefPoints[3][2] = 0.0;
        }
        else
        {
            m_echoRefPoints[3][0] = lcExterAsc[1][0];
            m_echoRefPoints[3][1] = lcExterAsc[1][1];
            m_echoRefPoints[3][2] = 0.0;
        }
    }

    SIGHT_DEBUG("C:" << m_echoRefPoints[2][0] << " " << m_echoRefPoints[2][1]);
    SIGHT_DEBUG("D:" << m_echoRefPoints[3][0] << " " << m_echoRefPoints[3][1]);

    for(std::uint8_t i = 0 ; i < 3 ; i++)
    {
        v1[i] = m_echoRefPoints[2][i] - m_echoRefPoints[0][i];
        v2[i] = m_echoRefPoints[3][i] - m_echoRefPoints[2][i];
    }

    probeSettings.depth = static_cast<int>(std::nearbyint(::glm::length(v1)));
    probeSettings.width = static_cast<int>(std::nearbyint(::glm::length(v2)));

    probeSettings.echoShapeOn = true;

    return probeSettings;
}

// -----------------------------------------------------------------------------

void SUltrasoundImage::updateDoubleExtractionParameter(double val, std::string key)
{
    if(key == s_US_DEPTH)
    {
        m_usDepth = val;

        // Mark the frame to be updated and update it
        m_probeSettingsInitialized = false;
        this->updating();
    }
    else
    {
        SIGHT_WARN("Unknown key '" << key << "' in " << s_UPDATE_DOUBLE_EXTRACTION_PARAMETERS_SLOT);
    }
}

// -----------------------------------------------------------------------------

void SUltrasoundImage::updateIntExtractionParameter(int val, std::string key)
{
    if(key == s_THRESHOLD_MIN)
    {
        m_thresholdMin = val;

        // Mark the frame to be updated and update it
        m_probeSettingsInitialized = false;
        this->updating();
    }
    else
    {
        SIGHT_WARN("Unknown key '" << key << "' in " << s_UPDATE_INT_EXTRACTION_PARAMETERS_SLOT);
    }
}

// -----------------------------------------------------------------------------

void SUltrasoundImage::resetEchoPlane()
{
    // Mark the frame to be updated and update it
    m_probeSettingsInitialized = false;
    this->updating();
}

// -----------------------------------------------------------------------------

double SUltrasoundImage::computeArcAngle(const cv::Vec2d& center, const std::vector<cv::Vec2d>& arcPoint) const
{
    cv::Vec2d arcPointMinY = arcPoint[1];
    if(arcPoint[0][1] > arcPoint[1][1])
    {
        arcPointMinY[0] = arcPoint[0][0];
        arcPointMinY[1] = arcPoint[0][1];
    }

    const double opposit = center[0] - arcPointMinY[0];
    const double side    = -center[1] + arcPointMinY[1];
    return ::glm::degrees(std::atan2(opposit, side));
}

// -----------------------------------------------------------------------------

bool SUltrasoundImage::isDataUnderArc(const cv::Mat& input, const std::vector<cv::Point2d>& points) const
{
    for(size_t i = 0 ; i < points.size() ; ++i)
    {
        if(input.at<uchar>(static_cast<int>(points[i].y), static_cast<int>(points[i].x)) != 0)
        {
            return true;
        }
    }

    return false;
}

// -----------------------------------------------------------------------------

void SUltrasoundImage::changeDebugEchoPlane(bool debug, std::string key)
{
    if(key == s_DEBUG_US_EXTRACTION)
    {
        m_debug = debug;
    }
    else
    {
        SIGHT_WARN("Unknown key '" << key << "' in " << s_CHANGE_DEBUG_ECHO_PLANE_SLOT);
    }
}

// -----------------------------------------------------------------------------

void SUltrasoundImage::changeEchoShape(bool echoShape, std::string key)
{
    if(key == s_ECHO_SHAPE)
    {
        m_convexShape              = echoShape;
        m_probeSettingsInitialized = false;
    }
    else
    {
        SIGHT_WARN("Unknown key '" << key << "' in " << s_CHANGE_ECHO_SHAPE_SLOT);
    }
}

// -----------------------------------------------------------------------------

void SUltrasoundImage::updateBeamExtractionMap()
{
    const cv::Size mapSize(static_cast<int>(m_probeSettings.matrixWidth),
                           static_cast<int>(m_probeSettings.matrixDepth));
    m_extractionMap = cv::Mat(mapSize, CV_32FC2);

    const float halfAngle  = static_cast<float>(m_probeSettings.angle) * 0.5f;
    const float angleBegin = ::glm::radians(90.f - halfAngle);
    const float angleEnd   = ::glm::radians(90.f + halfAngle);

    const float angleStep = (angleEnd - angleBegin) / static_cast<float>(mapSize.width - 1);

    const float depthStep = static_cast<float>(m_probeSettings.depth) / static_cast<float>(mapSize.height - 1);

    const float widthRatio = static_cast<float>(m_probeSettings.width) / static_cast<float>(mapSize.width);

    for(int i = 0 ; i < mapSize.width ; ++i)
    {
        fwVec3d currentDirection = m_probeSettings.direction;
        fwVec3d currentCenter    = m_probeSettings.centerPosition;

        // Compute the beam direction, center and angle at the current column.
        if(m_probeSettings.echoShapeOn)
        {
            const int iReverse       = mapSize.width - i - 1;
            const float currentAngle = angleBegin + angleStep * static_cast<float>(iReverse);

            currentDirection = std::cos(currentAngle) * m_probeSettings.normal
                               + std::sin(currentAngle) * m_probeSettings.direction;
        }
        else
        {
            const float distanceToCenterLine = static_cast<float>(i) * widthRatio
                                               - static_cast<float>(m_probeSettings.width) * 0.5f;

            currentCenter = m_probeSettings.centerPosition + distanceToCenterLine * m_probeSettings.normal;
        }

        for(int j = 0 ; j < mapSize.height ; ++j)
        {
            const float distanceToCenter = static_cast<float>(j) * depthStep
                                           + static_cast<float>(m_probeSettings.deltaDepth);
            const fwVec3d posReal = currentCenter + distanceToCenter * currentDirection;

            m_extractionMap.at<cv::Vec2f>(j, i) = cv::Vec2f(
                static_cast<float>(posReal[0]),
                static_cast<float>(posReal[1])
            );
        }
    }
}

// -----------------------------------------------------------------------------

} // cvSegmentation
