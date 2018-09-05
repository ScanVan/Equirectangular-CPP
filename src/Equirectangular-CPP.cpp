//============================================================================
// Name        : Equirectangular-CPP.cpp
// Author      : Marcelo Kaihara
// Version     :
// Copyright   : 
// Description : Code to convert images into the equirectangular representation based on the calibration results.
//============================================================================

#include <iostream>

// Include files to use OpenCV API
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main() {
	std::string path_map_0 = "/home/scanvan/dev/remoteStorage/calibration/camera_40008603-40009302/20180904-162307_FirstCalibration/calibration_40008603";
	std::string path_map_1 = "/home/scanvan/dev/remoteStorage/calibration/camera_40008603-40009302/20180904-162307_FirstCalibration/calibration_40009302";

	cv::Mat map_0_1;
	std::string path_map_0_1 = path_map_0 + "/map1.xml";
	cv::FileStorage file_0_1(path_map_0_1, cv::FileStorage::READ);
	file_0_1["mat_map1"] >> map_0_1;
	file_0_1.release();
	std::cout << "Read " << path_map_0_1 << std::endl;

	cv::Mat map_0_2;
	std::string path_map_0_2 = path_map_0 + "/map2.xml";
	cv::FileStorage file_0_2(path_map_0_2, cv::FileStorage::READ);
	file_0_2["mat_map2"] >> map_0_2;
	file_0_2.release();
	std::cout << "Read " << path_map_0_2 << std::endl;


	cv::Mat map_1_1;
	std::string path_map_1_1 = path_map_1 + "/map1.xml";
	cv::FileStorage file_1_1(path_map_1_1, cv::FileStorage::READ);
	file_1_1["mat_map1"] >> map_1_1;
	file_1_1.release();
	std::cout << "Read " << path_map_1_1 << std::endl;

	cv::Mat map_1_2;
	std::string path_map_1_2 = path_map_1 + "/map2.xml";
	cv::FileStorage file_1_2(path_map_1_2, cv::FileStorage::READ);
	file_1_2["mat_map2"] >> map_1_2;
	file_1_2.release();
	std::cout << "Read " << path_map_1_2 << std::endl;


	std::string exp1 = "20180904-164403_HESOutdoorCircular";
	std::string exp2 = "20180904-164727_HESOutdoorLinear";
	std::string exp3 = "20180904-170536_HESIndoorLinear";

	std::string exp {exp3};

	for (int i { 1 }; i < 13; ++i) {

		std::ostringstream s_0 {};
		s_0 << "/home/scanvan/dev/remoteStorage/record/camera_40008603-40009302/" << exp << "/sensor_40008603/" << "0_" << i
				<< ".bmp";
		std::string imageFileName_0(s_0.str());

		cv::Mat distorted_0 = imread(imageFileName_0, cv::IMREAD_COLOR);

		std::cout << imageFileName_0 << std::endl << std::flush;

		cv::Mat undistorted_0;

		// main remapping function that undistort the images
		cv::remap(distorted_0, undistorted_0, map_0_1, map_0_2, cv::INTER_CUBIC, cv::BORDER_CONSTANT);


		std::ostringstream s_1 {};
		s_1 << "/home/scanvan/dev/remoteStorage/record/camera_40008603-40009302/" << exp << "/sensor_40009302/" << "1_" << i
				<< ".bmp";
		std::string imageFileName_1(s_1.str());

		cv::Mat distorted_1 = imread(imageFileName_1, cv::IMREAD_COLOR);

		std::cout << imageFileName_1 << std::endl << std::flush;

		cv::Mat undistorted_1;

		// main remapping function that undistort the images
		cv::remap(distorted_1, undistorted_1, map_1_1, map_1_2, cv::INTER_CUBIC, cv::BORDER_CONSTANT);


		cv::Mat undistorted_0_1;

		cv::hconcat(undistorted_0, undistorted_1, undistorted_0_1);

		std::ostringstream s2 {};
		s2 << "/home/scanvan/dev/remoteStorage/record/camera_40008603-40009302/" << exp << "/" << "equi_" << i << ".bmp";
		std::string outputFileName(s2.str());

		//save images into file
		imwrite(outputFileName, undistorted_0_1);
		std::cout << outputFileName << std::endl << std::flush;

	}

	return 0;
}
