/**
 * © 2013 Kornel Lesiński. All rights reserved.
 * Based on Public Domain code by Rich Geldreich.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * • Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * • Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Note: jpge.cpp/h and jpgd.cpp/h are completely standalone, i.e. they do not have any dependencies to each other.
#include "jpge.h"
//#include "jpgd.h"
//#include "stb_image.c"
//#include <ctype.h>
#include "stdint.h"
#include "stdio.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//#if defined(_MSC_VER)
//#define strcasecmp _stricmp
//#else
//#define strcpy_s(d, c, s) strcpy(d, s)
//#endif

//static void get_pixel(int *pDst, const uint8 *pSrc, bool luma_only, int num_comps)
//{
//    int r, g, b;
//    if (num_comps == 1) {
//        r = g = b = pSrc[0];
//    } else if (luma_only) {
//        const int YR = 19595, YG = 38470, YB = 7471;
//        r = g = b = (pSrc[0] * YR + pSrc[1] * YG + pSrc[2] * YB + 32768) / 65536;
//    } else {
//        r = pSrc[0]; g = pSrc[1]; b = pSrc[2];
//    }
//    pDst[0] = r; pDst[1] = g; pDst[2] = b;
//}

//// Compute image error metrics.
//static void image_compare(image_compare_results &results, int width, int height, const uint8 *pComp_image, int comp_image_comps, const uint8 *pUncomp_image_data, int uncomp_comps, bool luma_only)
//{
//    double hist[256];
//    memset(hist, 0, sizeof(hist));

//    const uint first_channel = 0, num_channels = 3;
//    for (int y = 0; y < height; y++) {
//        for (int x = 0; x < width; x++) {
//            int a[3]; get_pixel(a, pComp_image + (y * width + x) * comp_image_comps, luma_only, comp_image_comps);
//            int b[3]; get_pixel(b, pUncomp_image_data + (y * width + x) * uncomp_comps, luma_only, uncomp_comps);
//            for (uint c = 0; c < num_channels; c++)
//                hist[labs(a[first_channel + c] - b[first_channel + c])]++;
//        }
//    }

//    results.max_err = 0;
//    double sum = 0.0f, sum2 = 0.0f;
//    for (uint i = 0; i < 256; i++) {
//        if (!hist[i])
//            continue;
//        if (i > results.max_err)
//            results.max_err = i;
//        double x = i * hist[i];
//        sum += x;
//        sum2 += i * x;
//    }

//    // See http://bmrc.berkeley.edu/courseware/cs294/fall97/assignment/psnr.html
//    double total_values = width * height;

//    results.mean = sum / total_values;
//    results.mean_squared = sum2 / total_values;

//    results.root_mean_squared = sqrt(results.mean_squared);

//    if (!results.root_mean_squared)
//        results.peak_snr = 1e+10f;
//    else
//        results.peak_snr = log10(255.0f / results.root_mean_squared) * 20.0f;
//}

//// Simple exhaustive test. Tries compressing/decompressing image using all supported quality, subsampling, and Huffman optimization settings.
//static int exhausive_compression_test(const char *pSrc_filename, bool use_jpgd)
//{
//    int status = EXIT_SUCCESS;

//    // Load the source image.
//    const int req_comps = 3; // request RGB image
//    int width = 0, height = 0, actual_comps = 0;
//    uint8 *pImage_data = stbi_load(pSrc_filename, &width, &height, &actual_comps, req_comps);
//    if (!pImage_data) {
//        log_printf("Failed loading file \"%s\"!\n", pSrc_filename);
//        return EXIT_FAILURE;
//    }

//    log_printf("Source file: \"%s\" Image resolution: %ix%i Actual comps: %i\n", pSrc_filename, width, height, actual_comps);

//    int orig_buf_size = width * height * 3; // allocate a buffer that's hopefully big enough (this is way overkill for jpeg)
//    if (orig_buf_size < 1024) orig_buf_size = 1024;
//    void *pBuf = malloc(orig_buf_size);

//    uint8 *pUncomp_image_data = NULL;

//    double max_err = 0, bpq_sum=0; int bpq_num=0;
//    double lowest_psnr = 9e+9;
//    double threshold_psnr = 9e+9;
//    double threshold_max_err = 0.0f;

//    image_compare_results prev_results;

//    for (uint quality_factor = 12; quality_factor <= 100; quality_factor+=11) {
//        for (uint subsampling = 0; subsampling <= jpge::H2V2; subsampling++) {
//                // Fill in the compression parameter structure.
//                jpge::params params;
//                params.m_quality = quality_factor;
//                params.m_subsampling = static_cast<jpge::subsampling_t>(subsampling);

//                int comp_size = orig_buf_size;
//                if (!jpge::compress_image_to_jpeg_file_in_memory(pBuf, comp_size, width, height, req_comps, pImage_data, params)) {
//                    status = EXIT_FAILURE;
//                    goto failure;
//                }

//                int uncomp_width = 0, uncomp_height = 0, uncomp_actual_comps = 0, uncomp_req_comps = 3;
//                free(pUncomp_image_data);
//                if (use_jpgd)
//                    pUncomp_image_data = jpgd::decompress_jpeg_image_from_memory((const stbi_uc *)pBuf, comp_size, &uncomp_width, &uncomp_height, &uncomp_actual_comps, uncomp_req_comps);
//                else
//                    pUncomp_image_data = stbi_load_from_memory((const stbi_uc *)pBuf, comp_size, &uncomp_width, &uncomp_height, &uncomp_actual_comps, uncomp_req_comps);
//                if (!pUncomp_image_data) {
//                    status = EXIT_FAILURE;
//                    goto failure;
//                }

//                if ((uncomp_width != width) || (uncomp_height != height)) {
//                    status = EXIT_FAILURE;
//                    goto failure;
//                }

//                image_compare_results results;
//                image_compare(results, width, height, pImage_data, req_comps, pUncomp_image_data, uncomp_req_comps, (params.m_subsampling == jpge::Y_ONLY) || (actual_comps == 1) || (uncomp_actual_comps == 1));
//                double bpq = comp_size*results.mean/results.peak_snr/100;
//                log_printf("Q: %3u, S%u, Size: %7u, Error Max:% 5.0f, Mean:% 6.2f, RMSE:%6.2f, PSNR:%7.3f, BPQ:%6.0f\n",
//                           quality_factor, subsampling, comp_size, results.max_err, results.mean, results.root_mean_squared, results.peak_snr, bpq);
//                if (results.max_err > max_err) max_err = results.max_err;
//                if (results.peak_snr < lowest_psnr) lowest_psnr = results.peak_snr;
//                if (quality_factor < 99 && quality_factor > 35) {
//                    bpq_sum += bpq;
//                    bpq_num++;
//                }

//                if (quality_factor == 12) {
//                    if (results.peak_snr < threshold_psnr)
//                        threshold_psnr = results.peak_snr;
//                    if (results.max_err > threshold_max_err)
//                        threshold_max_err = results.max_err;
//                } else {
//                    // Couple empirically determined tests - worked OK on my test data set.
//                    if ((results.peak_snr < (threshold_psnr - 3.0f)) || (results.peak_snr < 6.0f)) {
//                        status = EXIT_FAILURE;
//                        goto failure;
//                    }
//                }

//                prev_results = results;
//        }
//    }

//    log_printf("Max error: %.0f Lowest PSNR: %.3f, BPQ: %.0f\n", max_err, lowest_psnr, bpq_sum/bpq_num);

//failure:
//    free(pImage_data);
//    free(pBuf);
//    free(pUncomp_image_data);

//    log_printf((status == EXIT_SUCCESS) ? "Success.\n" : "Exhaustive test failed!\n");
//    return status;
//}

int main(int arg_c, char *ppArgs[])
{
    printf("jpge example app\n");

    // Parse command line.
    int subsampling = -1;
    bool use_jpgd = true;

    // Test jpge
    if (arg_c < 3)
    {
       printf("not enough parameters");
       return EXIT_FAILURE;
    }

    float quality_factor = 95.0f;

    // Load the source image.
    const std::string srcFilename(ppArgs[1]);
    const std::string dstFilename(ppArgs[2]);
    cv::Mat srcMat = cv::imread(srcFilename);
    cv::Mat srcMatRGB;
    cv::cvtColor(srcMat, srcMatRGB, CV_BGR2RGB);
    u_int8_t *pImage_data = srcMatRGB.data;

    const int width = srcMat.cols;
    const int height = srcMat.rows;
    const int numChannels = srcMat.channels();
    printf("Source file: \"%s\", size=%dx%d, channels=%d", srcFilename.c_str(), width, height, numChannels);

    //---------------------------------------------------------------------------------
    // Fill in the compression parameter structure.
    jpge::params params;
    params.m_quality = quality_factor;
    params.m_subsampling = jpge::H2V2;// (subsampling < 0) ? ((actual_comps == 1) ? jpge::Y_ONLY : jpge::H2V2) : static_cast<jpge::subsampling_t>(subsampling);
    printf("jpge subsampling: %d", (int)params.m_subsampling);
    // Now create the JPEG file.
#if 0
    //
    // compress directly to file
    //
    bool success = jpge::compress_image_to_jpeg_file(dstFilename.c_str(), width, height, numChannels, pImage_data, params);
    if (!success)
    {
       printf("Failed writing to output file!\n");
       return EXIT_FAILURE;
    }
#else
    //
    // compress to buffer
    //
    int buf_size = width * height * 3; // allocate a buffer that's hopefully big enough (this is way overkill for jpeg)
    if (buf_size < 1024) buf_size = 1024;
    void *pBuf = malloc(buf_size);
    bool success = jpge::compress_image_to_jpeg_file_in_memory(pBuf, buf_size, width, height, numChannels, pImage_data, params);
    if (!success)
    {
       printf("Failed compressing to jpeg file in memory!\n");
       return EXIT_FAILURE;
    }

    // write buffer to file (to test)
    FILE *pFile = fopen(dstFilename.c_str(), "wb");
    if (!pFile)
    {
        printf("Failed creating file \"%s\"!\n", dstFilename.c_str());
        return EXIT_FAILURE;
    }

    if (fwrite(pBuf, buf_size, 1, pFile) != 1)
    {
        printf("Failed writing to output file!\n");
        return EXIT_FAILURE;
    }

    if (fclose(pFile) == EOF)
    {
        printf("Failed writing to output file!\n");
        return EXIT_FAILURE;
    }
#endif


    //---------------------------------------------------------------------------------
    // check written file
    cv::Mat dstMat = cv::imread(dstFilename);
    cv::imwrite("destination.png", dstMat);
    return EXIT_SUCCESS;
}
