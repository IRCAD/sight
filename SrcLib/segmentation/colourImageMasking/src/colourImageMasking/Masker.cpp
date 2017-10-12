/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "colourImageMasking/Masker.hpp"

using namespace std;
using namespace colourImageMasking;

namespace colourImageMasking
{

// Define the morphological element used
const ::cv::Mat Masker::s_MORPHELEMENT =
    ::cv::getStructuringElement(Masker::s_MORPHTYPE, ::cv::Size(2*Masker::s_MORPHSIZE+1,
                                                                2*Masker::s_MORPHSIZE+1),
                                ::cv::Point(Masker::s_MORPHSIZE, Masker::s_MORPHSIZE));

//------------------------------------------------------------------------------

Masker::Masker(const ColSpace& c, const DetectionMode& d) :
    m_COLORSPACE(c),
    m_DETECTIONMODE(d),
    m_threshold(0.0),
    m_hasSetThreshold(false)
{
}

//------------------------------------------------------------------------------

Masker::~Masker()
{

}

//------------------------------------------------------------------------------

void Masker::trainForegroundModel(const ::cv::Mat& rgbImg, const ::cv::Mat& selectionMask,
                                  const unsigned int numClusters)
{
    const ::cv::Mat s = this->makeTrainingSamples(rgbImg, selectionMask, this->m_COLORSPACE);
    this->m_foregroundModel = this->trainModelFromSamples(s, numClusters);

}

//------------------------------------------------------------------------------

void Masker::trainBackgroundModel(const ::cv::Mat& rgbImg, const ::cv::Mat& selectionMask,
                                  const unsigned int numClusters)
{
    const ::cv::Mat s = this->makeTrainingSamples(rgbImg, selectionMask, this->m_COLORSPACE);
    this->m_backgroundModel = this->trainModelFromSamples(s, numClusters);
}

//------------------------------------------------------------------------------

::cv::Mat Masker::makeMask(const ::cv::Mat& testImg, const ::cv::Size& downSize, ::cv::InputArray testImgMask) const
{
    assert(this->m_hasSetThreshold);

    ::cv::Mat t2, testImgMask2;
    ::cv::resize(testImg, t2, downSize);
    ::cv::Mat m;

    if(!testImgMask.empty())
    {
        ::cv::resize(testImgMask.getMat(), testImgMask2, downSize);
    }

    const ::cv::Mat I = convertColourSpace(t2, this->m_COLORSPACE);

    switch(m_DETECTIONMODE)
    {
        case fgLL:
        {
            ::cv::Mat fgResponse = makeResponseImage(I, this->m_foregroundModel, testImgMask2);
            ::cv::threshold(fgResponse, m, this->m_threshold, 255, ::cv::THRESH_BINARY);
            break;
        }
        case bgLL:
        {
            ::cv::Mat bgResponse = makeResponseImage(I, this->m_backgroundModel, testImgMask2);
            ::cv::threshold(bgResponse, m, this->m_threshold, 255, ::cv::THRESH_BINARY_INV);
            break;
        }
        case LLRatio:
        {
            ::cv::Mat fgResponse = makeResponseImage(I, this->m_foregroundModel, testImgMask2);
            ::cv::Mat bgResponse = makeResponseImage(I, this->m_backgroundModel, testImgMask2);
            ::cv::threshold(fgResponse - bgResponse, m, this->m_threshold, 255, ::cv::THRESH_BINARY);
            break;
        }
    }

    m.convertTo(m, CV_8UC1);

    //get mask back to original size:
    ::cv::resize(m, m, ::cv::Size(testImg.cols, testImg.rows));
    ::cv::threshold(m, m, 125, 255, ::cv::THRESH_BINARY);

    //eliminate mask holes with erosion/dilation
    ::cv::Mat filteredMask1 = removeMaskHoles(m, 3, 2);

    return filteredMask1;
}

//------------------------------------------------------------------------------

void Masker::setThreshold(float t)
{
    this->m_threshold       = t;
    this->m_hasSetThreshold = true;
}

//------------------------------------------------------------------------------

bool Masker::isModelLearned(void)
{
    switch(this->m_DETECTIONMODE)
    {
        case fgLL:
        {
            return !this->m_foregroundModel.empty();
        }
        case bgLL:
        {
            return !this->m_backgroundModel.empty();
        }
        case LLRatio:
        {
            return !this->m_foregroundModel.empty() && !this->m_backgroundModel.empty();
        }
        default:
        {
            return false;
        }
    }
}

//------------------------------------------------------------------------------

::cv::Mat Masker::makeResponseImage(const ::cv::Mat& I, const ::cv::Ptr< ::cv::ml::EM > model,
                                    ::cv::InputArray inImgMask)
{
    const int cn = I.channels();

    ::cv::Mat output = ::cv::Mat::zeros(I.rows, I.cols, CV_32FC1);

    ::cv::Mat sample(cn, 1, CV_32FC1);
    uchar* pixelPtr = (uchar*)I.data;

    const int h = I.rows;
    const int w = I.cols;

    bool usesFilterMask = !inImgMask.empty();
    ::cv::Mat filterMask;
    if (usesFilterMask)
    {
        filterMask = inImgMask.getMat();
    }

    for(int i = 0; i < h; ++i )
    {
        for(int j = 0; j < w; ++j)
        {
            if(usesFilterMask)
            {
                if (filterMask.at<uchar>(i, j) == 0)
                {
                    continue;
                }
            }

            for(int chnInxs = 0; chnInxs < cn; ++chnInxs)
            {
                sample.at<float>(chnInxs) = pixelPtr[i*w*cn + j*cn + chnInxs];
            }

            output.at<float>(i, j) = static_cast<float>(model->predict2( sample, ::cv::noArray() )[0]);
        }
    }
    return output;
}

//------------------------------------------------------------------------------

::cv::Mat Masker::convertColourSpace(const ::cv::Mat& src, const ColSpace& c)
{
    ::cv::Mat output;
    switch(c)
    {
        case RGB:
        {
            src.copyTo(output);
            break;
        }
        case HSv:
        {
            ::cv::cvtColor(src, output, CV_BGR2HSV);
            ::cv::Mat s[3]; //destination array
            split(output, s);//split source
            std::vector< ::cv::Mat > array_to_merge;
            array_to_merge.push_back(s[0]);
            array_to_merge.push_back(s[1]);
            array_to_merge.push_back(s[1]); //we remove v channel for illumination invariance. This is done here by
                                            // duplicating the S channel
            ::cv::merge(array_to_merge, output);
            break;
        }
        case lAB:
        {
            ::cv::cvtColor(src, output, CV_BGR2Lab);
            ::cv::Mat s[3]; //destination array
            split(output, s);//split source
            std::vector< ::cv::Mat > array_to_merge;
            array_to_merge.push_back(s[1]);
            array_to_merge.push_back(s[2]);
            array_to_merge.push_back(s[1]);
            ::cv::merge(array_to_merge, output);
            break;
        }
        case yCrCb:
        {
            ::cv::cvtColor(src, output, CV_BGR2YCrCb);
            ::cv::Mat s[3]; //destination array
            split(output, s);//split source
            std::vector< ::cv::Mat > array_to_merge;
            array_to_merge.push_back(s[1]);
            array_to_merge.push_back(s[2]);
            array_to_merge.push_back(s[1]);
            ::cv::merge(array_to_merge, output);
            break;
        }
    }
    return output;
}

//------------------------------------------------------------------------------

::cv::Ptr< ::cv::ml::EM > Masker::trainModelFromSamples(const ::cv::Mat& samples, const unsigned int numClusters)
{
    ::cv::Ptr < ::cv::ml::EM > m = ::cv::ml::EM::create();
    m->setClustersNumber(numClusters);
    m->setCovarianceMatrixType(::cv::ml::EM::COV_MAT_SPHERICAL);
    m->setTermCriteria(::cv::TermCriteria(::cv::TermCriteria::COUNT + ::cv::TermCriteria::EPS, 300, 0.1));
    m->trainEM(samples, ::cv::noArray(), ::cv::noArray(), ::cv::noArray());
    return m;
}

//------------------------------------------------------------------------------

::cv::Mat Masker::makeTrainingSamples(const ::cv::Mat& t, const ::cv::Mat& mask, const ColSpace& c)
{
    ::cv::Mat trainImg = Masker::convertColourSpace(t, c);

    //gather training samples:
    const int h  = trainImg.rows;
    const int w  = trainImg.cols;
    const int cn = trainImg.channels();

    ::cv::Mat samples(h*w, cn, CV_64FC1);

    int sampleCount = 0;

    for(int i = 0; i < h; ++i)
    {
        for(int j = 0; j < w; ++j)
        {
            if(mask.at<uchar>(i, j) > 0)
            {
                for(int chnInxs = 0; chnInxs < cn; ++chnInxs)
                {
                    samples.at<double>(sampleCount,
                                       chnInxs) = static_cast<double>(trainImg.at< ::cv::Vec3b >(i, j)[chnInxs]);
                }
                sampleCount++;
            }
        }
    }

    samples = samples.rowRange(0, sampleCount);

    return samples;
}

//------------------------------------------------------------------------------

::cv::Mat Masker::removeMaskHoles(const ::cv::Mat& m, size_t filterSize, size_t n)
{
    ::cv::Mat mask;
    m.copyTo(mask);

    ::cv::Mat k = s_MORPHELEMENT.clone();
    k.setTo(1);

    for (size_t i = 0; i < n; i++)
    {
        ::cv::erode(mask, mask, k);
    }

    for (size_t i = 0; i < n; i++)
    {
        ::cv::dilate(mask, mask, k);
    }

    ::cv::morphologyEx(mask, mask, ::cv::MORPH_OPEN, s_MORPHELEMENT);
    ::cv::dilate(mask, mask, s_MORPHELEMENT);

    return mask;
}

//------------------------------------------------------------------------------

class Parallel_predict : public ::cv::ParallelLoopBody
{
private:
    ::cv::Mat* m_sample;
    ::cv::Mat* m_output;
public:
    Parallel_predict(::cv::Mat* sample, ::cv::Mat* output) :
        m_sample(sample),
        m_output(output)
    {
    }

    //------------------------------------------------------------------------------

    virtual void operator()(const ::cv::Range& r) const
    {
    }
};

//------------------------------------------------------------------------------

}
