//============================================================================
// Name        : Equirectangular-CPP.cpp
// Author      : Marcelo Kaihara
// Version     :
// Copyright   : 
// Description : Code to convert images into the equirectangular representation based on the calibration results.
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <algorithm>
#include <set>
#include <experimental/filesystem>

// Include files to use OpenCV API
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace fs = std::experimental::filesystem;

class FileConfig {
public:
	std::string ConfigFilePath { "./" };
	std::string RawInputPath { "./" };
	std::string CalPath0 { "./" };
	std::string CalPath1 { "./" };
	std::string EquiOutputPath { "./" };
	bool FlipLR { true };
	bool FlipCameras { false };
	int StartIdx { 0 };
	int StopIdx { 0 };
	FileConfig(std::string s) :
			ConfigFilePath(s) {
	}

};

std::string trim(const std::string& str, const std::string& whitespace = " \t") {
// trim white spaces and tabs from the beginning and end

	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

std::string ToUpper(const std::string &str) {
// Convert to upper case the string

	std::locale loc { };
	std::stringstream ss { };
	for (std::string::size_type i = 0; i < str.length(); ++i)
		ss << std::toupper(str[i], loc);
	return (ss.str());
}

void ProcessConfigFile(FileConfig &FC) {

	// open configuration file
	std::ifstream f(FC.ConfigFilePath);

	if (!f.good()) {
		throw std::runtime_error("Error opening the configuration file \"" + FC.ConfigFilePath + "\".");
	}

	// process configuration file
	while (f.good()) {

		std::string line { };
		std::getline(f, line);

		// remove white spaces from the line
		std::string trimmedLine = trim(line);
		if (trimmedLine == "")
			continue;

		// Ignore lines starting with hash symbol
		const auto strHash = trimmedLine.find_first_of("#");
		if (strHash == 0)
			continue;

		// Extract the command and the argument separated by the symbol "="
		const auto strEqual = trimmedLine.find_first_of("=");
		std::string command = ToUpper(trim(trimmedLine.substr(0, strEqual)));
		std::string argument = trim(trimmedLine.substr(strEqual + 1));

		// Check for commands
		if (command == "RAW_INPUT_PATH") {
			// Path of the input files where the raw and txt files and are placed
			if (argument != "") {
				fs::path p = argument;
				if (!fs::exists(p)) {
					throw(std::runtime_error("Error: the path " + argument + " does not exist."));
				}
				FC.RawInputPath = argument;
			} else {
				throw(std::runtime_error("Error: RAW_INPUT_PATH parameter is empty."));
			}
		} else if (command == "CAL_0_PATH") {
			// Path to the calibration data for camera 0
			if (argument != "") {
				fs::path p = argument;
				if (!fs::exists(p)) {
					throw(std::runtime_error("Error: the path " + argument + " does not exist."));
				}
				FC.CalPath0 = argument;
			} else {
				throw(std::runtime_error("Error: CAL_PATH parameter is empty."));
			}
		} else if (command == "CAL_1_PATH") {
			// Path to the calibration data for camera 1
			if (argument != "") {
				fs::path p = argument;
				if (!fs::exists(p)) {
					throw(std::runtime_error("Error: the path " + argument + " does not exist."));
				}
				FC.CalPath1 = argument;
			} else {
				throw(std::runtime_error("Error: CAL_PATH parameter is empty."));
			}
		} else if (command == "EQUI_OUTPUT_PATH") {
			// Output path where the equirectangular images are saved
			if (argument != "") {
				fs::path p = argument;
				if (!fs::exists(p)) {
					std::cout << "Creating directory " << argument << std::endl;
					fs::create_directory(p);
				}
				FC.EquiOutputPath = argument;
			} else {
				throw(std::runtime_error("Error: EQUI_OUTPUT_PATH parameter is empty."));
			}
		} else if (command == "FLIP_LR") {
			// Flips each of the images
			if (argument != "") {
				argument = ToUpper(argument);
				if (argument == "FALSE") {
					FC.FlipLR = false;
				} else if (argument == "TRUE") {
					FC.FlipLR = true;
				} else {
					throw(std::runtime_error("Error: invalid argument for FLIP_LR parameter."));
				}
			} else {
				throw(std::runtime_error("Error: FLIP_LR parameter is empty."));
			}
		}
		else if (command == "START_IDX") {
			// This is to indicate the starting index for processing the files
			// in case the process was interrupted
			if (argument != "") {
				try {
					FC.StartIdx = stoi(argument);
				} catch (...) {
					throw(std::runtime_error("Error: argument of START_IDX is not valid."));
				}
			} else {
				throw(std::runtime_error("Error: START_IDX parameter is empty."));
			}
		} else if (command == "STOP_IDX") {
			// This is to indicate the stopping index for processing the files
			// in case the process was interrupted
			if (argument != "") {
				try {
					FC.StopIdx = stoi(argument);
				} catch (...) {
					throw(std::runtime_error("Error: argument of STOP_IDX is not valid."));
				}
			} else {
				throw(std::runtime_error("Error: STOP_IDX parameter is empty."));
			}
		}

		else if (command == "FLIP_CAMERAS") {
			// Swaps the camera images (between camera 0 and camera 1)
			if (argument != "") {
				argument = ToUpper(argument);
				if (argument == "FALSE") {
					FC.FlipCameras = false;
				} else if (argument == "TRUE") {
					FC.FlipCameras = true;
				} else {
					throw(std::runtime_error("Error: invalid argument for FLIP_CAMERAS parameter."));
				}
			} else {
				throw(std::runtime_error("Error: START_IDX parameter is empty."));
			}
		}
	}
}

void raw2bmp(std::string imageName, cv::Mat &openCvImage) {
// Converts from raw data to opencv format

	cv::Mat openCvImageRG8;

	std::ifstream myFile(imageName, std::ios::in | std::ios::binary);
	if (myFile) {

		// get length of the file:
		myFile.seekg(0, myFile.end);
		int length = myFile.tellg();
		myFile.seekg(0, myFile.beg);

		char * buffer = new char[length] { };
		myFile.read(buffer, length);

		if (myFile) {

			// Assumes that the images are squares
			int imageHeight = sqrt(length);
			if (imageHeight * imageHeight != length) {
				throw(std::runtime_error("Error in the dimension of the buffer size."));
			}
			int imageWidth { imageHeight };

			openCvImageRG8 = cv::Mat(imageHeight, imageWidth, CV_8UC1, buffer);

			cv::cvtColor(openCvImageRG8, openCvImage, cv::COLOR_BayerRG2RGB);

		} else {
			throw(std::runtime_error("Error: only " + std::to_string(myFile.gcount()) + " could be read."));
		}

		delete[] buffer;

		myFile.close();

	} else {
		throw(std::runtime_error("Error: file could not be opened."));
	}

	openCvImageRG8.release();
}

std::string extractTime (std::string txtPath) {
// This extracts the time stamp from the txt file
// It generates a string of the format 20190319-103444-344878 (year(4)month(2)day(2)-hour(2)minutes(2)second(2)-millisecond(3)microsecond(3))
// Input format is: "Capture Time CPU: 2019-03-19 10:34:41:594:897"

	std::ifstream f(txtPath);
	if (!f.good()) {
		throw(std::runtime_error("Cannot open file " + txtPath + ""));
	}

	int i { 0 };
	std::string line {};
	// The time information is in the 5th line
	while (f.good() && i < 5) {
		std::getline(f, line);
		i++;
	}

	std::stringstream ss { };
	const auto strColon = line.find_first_of(":");
	const auto strDash1 = line.find_first_of("-");
	std::string year = trim(line.substr(strColon + 1, strDash1 - strColon - 1));
	std::string s1 = line.substr(strDash1 + 1);
	const auto strDash2 = s1.find_first_of("-");
	std::string month = trim(s1.substr(0, strDash2));
	std::string s2 = s1.substr(strDash2 + 1);
	const auto strSpace = s2.find_first_of(" ");
	std::string day = trim(s2.substr(0, strSpace));
	std::string s3 = s2.substr(strSpace + 1);
	const auto strCol1 = s3.find_first_of(":");
	std::string h = trim(s3.substr(0, strCol1));
	std::string s4 = s3.substr(strCol1 + 1);
	const auto strCol2 = s4.find_first_of(":");
	std::string m = trim(s4.substr(0, strCol2));
	std::string s5 = s4.substr(strCol2 + 1);
	const auto strCol3 = s5.find_first_of(":");
	std::string s = trim(s5.substr(0, strCol3));
	std::string s6 = s5.substr(strCol3 + 1);
	const auto strCol4 = s6.find_first_of(":");
	std::string ms = trim(s6.substr(0, strCol4));
	std::string s7 = s6.substr(strCol4 + 1);
	std::string us = trim(s7);

	ss << year << month << day << "-";
	ss << h << m << s << "-";
	ss << std::setw(3) << std::setfill('0') << std::stoi(ms);
	ss << std::setw(3) << std::setfill('0') << std::stoi(us);

	return ss.str();
}

void ConvertImages (FileConfig &FC) {
// Performs the conversion of the images

	// Stores the set of indices to process
	std::set<int> imgnums { };

	// Scans all the files from the input folder
	fs::path p = FC.RawInputPath;

	// Stores all the file names as a vector of strings
	std::vector<std::string> lf { };
	for (auto &f : fs::directory_iterator(p)) {
		lf.push_back(f.path());
	}

	// Extracts the image numbers from the files and stores in the set
	for (auto &x : lf) {
		const auto posUnderscore = x.find_last_of("_");
		const auto posDot = x.find_last_of(".");
		std::string number = x.substr(posUnderscore + 1 , posDot - posUnderscore -1);
		try {
			imgnums.insert(stoi(number));
		} catch (...) {
			// some files does not contain numbers, like gps.txt
			// ignore them
		}
	}

	// Report the total number of files to process
	std::cout << "Number of images in the folder: " << imgnums.size() << std::endl;
	std::cout << "Reading from path: " << FC.RawInputPath << std::endl;
	std::cout << "Writing to path: " << FC.EquiOutputPath << std::endl;

	// Read the calibration data
	cv::Mat map_0_1;
	fs::path p0_1 = fs::path(FC.CalPath0) / std::string("map1.xml");
	cv::FileStorage file_0_1(p0_1.string(), cv::FileStorage::READ);
	file_0_1["mat_map1"] >> map_0_1;
	file_0_1.release();
	std::cout << "Read " << p0_1 << std::endl;

	cv::Mat map_0_2;
	fs::path p0_2 = fs::path(FC.CalPath0) / std::string("map2.xml");
	cv::FileStorage file_0_2(p0_2.string(), cv::FileStorage::READ);
	file_0_2["mat_map2"] >> map_0_2;
	file_0_2.release();
	std::cout << "Read " << p0_2 << std::endl;

	cv::Mat map_1_1;
	fs::path p1_1 = fs::path(FC.CalPath1) / std::string("map1.xml");
	cv::FileStorage file_1_1(p1_1.string(), cv::FileStorage::READ);
	file_1_1["mat_map1"] >> map_1_1;
	file_1_1.release();
	std::cout << "Read " << p1_1 << std::endl;

	cv::Mat map_1_2;
	fs::path p1_2 = fs::path(FC.CalPath1) / std::string("map2.xml");
	cv::FileStorage file_1_2(p1_2.string(), cv::FileStorage::READ);
	file_1_2["mat_map2"] >> map_1_2;
	file_1_2.release();
	std::cout << "Read " << p1_2 << std::endl;

	// Process for all the image numbers
	for (auto &n : imgnums) {

		// Skips the index numbers lower than the parameter START_IDX
		if (n < FC.StartIdx) continue;
		// If n is greater than STOP_IDX breaks
		if (FC.StopIdx != 0) {
			if (n > FC.StopIdx) break;
		}

		// Format the strings to match the file names
		std::string img0Name = "0_" + std::to_string(n) + ".raw";
		std::string img1Name = "1_" + std::to_string(n) + ".raw";
		std::string img0TxtName = "img_0_" + std::to_string(n) + ".txt";
		std::string img1TxtName = "img_1_" + std::to_string(n) + ".txt";

		// Path to the image files
		fs::path pathImg0 = p / img0Name;
		fs::path pathImg1 = p / img1Name;
		// Path to the associated text file
		fs::path pathTxt0 = p / img0TxtName;

		// Extract the time information from txt file
		std::string imgTime0 = extractTime (pathTxt0.string());

		// Name of the output equirectangular image
		std::string imgEquiName = imgTime0 + ".bmp";


		// Transformation to equirectangular images
		cv::Mat distorted_0 {};
		// Converts from raw to opencv
		raw2bmp(pathImg0.string(), distorted_0);
		cv::Mat distorted_1 {};
		// Converts from raw to opencv
		raw2bmp(pathImg1.string(), distorted_1);


		// Equirectangular image 0
		cv::Mat undistorted_0{};

		// main remapping function that undistort the images
		cv::remap(distorted_0, undistorted_0, map_0_1, map_0_2, cv::INTER_CUBIC, cv::BORDER_CONSTANT);

		distorted_0.release();

		if (FC.FlipLR) {
		// If FLIP_LR is true flip the image
			flip(undistorted_0, undistorted_0, +1);
		}

		// Equirectangular image 1
		cv::Mat undistorted_1 {};

		// main remapping function that undistort the images
		cv::remap(distorted_1, undistorted_1, map_1_1, map_1_2, cv::INTER_CUBIC, cv::BORDER_CONSTANT);

		distorted_1.release();

		if (FC.FlipLR) {
		// If FLIP_LR is true flip the image
			flip(undistorted_1, undistorted_1, +1);
		}

		// Concatenated equirectnagular image
		cv::Mat undistorted_0_1;

		if (FC.FlipCameras) {
		// If FlipCameras is true, swaps the camera images, left image is camera 1 and right image is camera 0
			cv::hconcat(undistorted_1, undistorted_0, undistorted_0_1);
		} else {
		// Else left image is from camera 0 and right image is from camera 1
			cv::hconcat(undistorted_0, undistorted_1, undistorted_0_1);
		}

		undistorted_0.release();
		undistorted_1.release();

		// Path of the output image
		fs::path pathOut = fs::path(FC.EquiOutputPath) / imgEquiName;

		//save images into file
		imwrite(pathOut.string(), undistorted_0_1);

		undistorted_0_1.release();

		// Print progress
		std::cout << n << " " << std::flush;

	}

	std::cout << std::endl;

}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " config_file.txt" << std::endl;
		return 1;
	}

	std::string cfg = argv[1];

	// Create object File Configuration
	FileConfig FC{cfg};

	// Process the configuration file
	ProcessConfigFile (FC);

	// Convert the images
	ConvertImages(FC);

	return 0;
}
