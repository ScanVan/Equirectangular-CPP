# Equirectangular-CPP

This repository contains a tool to convert the raw data extracted using CameraImageAcquisition-CPP.

The usage of the program is the following:

./Equirectangular-CPP config_file.txt

Where config_file.txt is a text file with the following parameters.

# Lines starting with # are comments and will be omitted.
# Path to the raw data input folder
RAW_INPUT_PATH = /path_to_raw_data_folder
# Path to the calibration folder of camera 0
CAL_0_PATH = /path_to_calibration_folder_of_camera_0
# Path to the calibration folder of camera 1
CAL_1_PATH = /path_to_calibration_folder_of_camera_1
# Path to the output folder where the equirectangular images will be placed
EQUI_OUTPUT_PATH = /path_to_output_folder_of_converted_equirectangular_images
# Flip the images of each of the cameras. Accepts True or False
FLIP_LR = True
# Flip the camera images camera 0 <-> camera 1. Accepts True or False
FLIP_CAMERAS = False
# Starting index for the conversion. The first index is 0
START_IDX = 0
# Stop index. The process will stop after processing index STOP_IDX. If equal to 0,
# it will process until the end.
STOP_IDX = 0
