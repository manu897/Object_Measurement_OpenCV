#include "Preprocessor.hpp"
#include <opencv2/imgproc.hpp>

Preprocessor::Preprocessor(const AppConfig& config)
    : config_(config)
{
    kernelDilate_ = cv::getStructuringElement(cv::MORPH_RECT, config_.morphKernelSize);
    kernelErode_  = cv::getStructuringElement(cv::MORPH_RECT, config_.morphKernelSize);
}

PreprocessedFrame Preprocessor::process(const cv::Mat&        frame,
                                         const ThresholdParams& thresholds) const
{
    PreprocessedFrame result;

    cv::cvtColor(frame, result.gray, cv::COLOR_BGR2GRAY);

    cv::GaussianBlur(result.gray,
                     result.blurred,
                     config_.gaussianKernel,
                     config_.gaussianSigma, 0);

    cv::Canny(result.blurred,
              result.canny,
              thresholds.thresh1,
              thresholds.thresh2);

    cv::dilate(result.canny,  result.dilated, kernelDilate_);
    cv::erode (result.dilated, result.eroded,  kernelErode_);

    return result;
}
