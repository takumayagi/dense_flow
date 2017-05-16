#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core/cuda.hpp"
#include "opencv2/cudaoptflow.hpp"

#include <stdio.h>
#include <iostream>
using namespace std;
using namespace cv;
using namespace cv::cuda;

static void convertFlowToImage(const Mat &flow, Mat &img,
        double lb, double hb, int magnitude) {
	#define CAST(v, L, H) ((v) > (H) ? 255 : (v) < (L) ? 0 : cvRound(255*((v) - (L))/((H)-(L))))
	for (int i = 0; i < flow.rows; ++i) {
		for (int j = 0; j < flow.cols; ++j) {
			float x = flow.at<float>(i,j,0);
			float y = flow.at<float>(i,j,1);
            if (magnitude){
                float mag = sqrt(x * x + y * y);
			    img.at<Vec3b>(i,j) = Vec3b(CAST(x, lb, hb),
                    CAST(y, lb, hb), CAST(mag, lb, hb));
            } else {
			    img.at<Vec3b>(i,j) = Vec3b(CAST(x, lb, hb),
                    CAST(y, lb, hb), 0);

            }
		}
	}
	#undef CAST
}

static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,double, const Scalar& color){
    for(int y = 0; y < cflowmap.rows; y += step)
        for(int x = 0; x < cflowmap.cols; x += step)
        {
            const Point2f& fxy = flow.at<Point2f>(y, x);
            line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),
                 color);
            circle(cflowmap, Point(x,y), 2, color, -1);
        }
}

int main(int argc, char** argv){
	// IO operation
	const char* keys =
		{
			"{ v vidFile   | ex2.avi | filename of video }"
			"{ i imgFile   | flow_i  | filename of flow image}"
			"{ b bound     | 15      | specify the maximum of optical flow}"
			"{ t type      | 0       | specify the optical flow algorithm }"
			"{ d device_id | 0       | set gpu id}"
			"{ s step      | 1       | specify the step for frame sampling}"
			"{ m magnitude | 0       | if 0, do not output magnitude}"
		};

	CommandLineParser cmd(argc, argv, keys);
	string vidFile = cmd.get<string>("vidFile");
	string imgFile = cmd.get<string>("imgFile");
	int bound = cmd.get<int>("bound");
    int type  = cmd.get<int>("type");
    int device_id = cmd.get<int>("device_id");
    int step = cmd.get<int>("step");
    int magnitude = cmd.get<int>("magnitude");

	VideoCapture capture(vidFile);
	if(!capture.isOpened()) {
		printf("Could not initialize capturing..\n");
		return -1;
	}

	int frame_num = 0;
	Mat image, prev_image, prev_grey, grey, frame, flow;
	GpuMat frame_0, frame_1, flow_g;

	setDevice(device_id);
    FarnebackOpticalFlow *alg_farn = FarnebackOpticalFlow::create();
	OpticalFlowDual_TVL1 *alg_tvl1 = OpticalFlowDual_TVL1::create();
	BroxOpticalFlow *alg_brox = BroxOpticalFlow::create(0.197f, 50.0f, 0.8f, 10, 77, 10);

	while(true) {
		capture >> frame;
		if(frame.empty())
			break;
		if(frame_num == 0) {
			image.create(frame.size(), CV_8UC3);
			grey.create(frame.size(), CV_8UC1);
			prev_image.create(frame.size(), CV_8UC3);
			prev_grey.create(frame.size(), CV_8UC1);

			frame.copyTo(prev_image);
			cvtColor(prev_image, prev_grey, CV_BGR2GRAY);

			frame_num++;

			int step_t = step;
			while (step_t > 1){
				capture >> frame;
				step_t--;
			}
			continue;
		}

		frame.copyTo(image);
		cvtColor(image, grey, CV_BGR2GRAY);

		frame_0.upload(prev_grey);
		frame_1.upload(grey);

        // GPU optical flow
		switch(type){
		case 0:
			alg_farn->calc(frame_0, frame_1, flow_g);
			break;
		case 1:
			alg_tvl1->calc(frame_0, frame_1, flow_g);
			break;
		case 2:
			GpuMat d_frame0f, d_frame1f;
	        frame_0.convertTo(d_frame0f, CV_32F, 1.0 / 255.0);
	        frame_1.convertTo(d_frame1f, CV_32F, 1.0 / 255.0);
			alg_brox->calc(d_frame0f, d_frame1f, flow_g);
			break;
		}

		flow_g.download(flow);

		// Output optical flow
		Mat img(flow.size(),CV_8UC3);
		convertFlowToImage(flow, img, -bound, bound, magnitude);
		char tmp[20];
		sprintf(tmp,"_%05d.jpg",int(frame_num));
		imwrite(imgFile + tmp, img);

		std::swap(prev_grey, grey);
		std::swap(prev_image, image);
		frame_num = frame_num + 1;

		int step_t = step;
		while (step_t > 1){
			capture >> frame;
			step_t--;
		}
	}
	return 0;
}
