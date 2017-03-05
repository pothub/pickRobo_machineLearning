#include <opencv2/opencv.hpp>
#include <fstream>

using namespace std;

string src_directory_path = "17-02-27_17:16:28/";
string src_image_path = src_directory_path + "Labeling.bmp";
string raw_image_path = src_directory_path + "Image23.bmp";
string xml_path = src_directory_path + "output.xml";

int work1_x = 524,work1_y = 278;
int work2_x = 722,work2_y = 376;
int work3_x = 234,work3_y = 742;
int extract_range = 150;

int main(int argc, const char* argv[]){
	std::ifstream ifs(xml_path);
	std::string str;
	if (ifs.fail()){
		std::cerr << "failed" << std::endl;
		return -1;
	}
	while (getline(ifs, str)){
		string ex_word = "pix_X";
		string index_word = "<" + ex_word + ">";
		string last_word = "</" + ex_word + ">";
		string::size_type index_num = str.find(index_word);
		string::size_type last_num = str.find(last_word);
		if( index_num != string::npos ){
			cout << str.substr(index_num + index_word.length(),last_num-index_num-last_word.length()+1) << endl;
		}
	}
	return 0;

	cv::Mat src_img = cv::imread(src_image_path);
	cv::Mat raw_img = cv::imread(raw_image_path);
	// if(src_img.empty()){
	//   std::cerr << "Failed to open image file." << std::endl;
	//   return -1; 
	// }
	cv::Mat gray_img,mask_img;
	// cv::cvtColor(src_img,gray_img,CV_RGB2GRAY);

	cv::Mat pointed_img = src_img.clone();
	circle(pointed_img, cv::Point(work1_x, work1_y), 20, cv::Scalar(200, 200, 200), 2, 2);
	circle(pointed_img, cv::Point(work2_x, work2_y), 20, cv::Scalar(200, 200, 200), 2, 2);
	circle(pointed_img, cv::Point(work3_x, work3_y), 20, cv::Scalar(200, 200, 200), 2, 2);

	cv::Mat hsv_img;
	cv::cvtColor(src_img,hsv_img,CV_BGR2HSV);				//BGR to HSV
	int hue = hsv_img.at<cv::Vec3b>(work1_y, work1_x)[0];
	int sat = hsv_img.at<cv::Vec3b>(work1_y, work1_x)[1];
	int val = hsv_img.at<cv::Vec3b>(work1_y, work1_x)[2];
	// cout<<hue<<sat<<val<<endl;
	// make mask_img with a specific HSV
	cv::inRange(hsv_img, cv::Scalar(hue,sat,val,0), cv::Scalar(hue,sat,val,0), mask_img);

	cv::Mat extract_img;
	raw_img.copyTo(extract_img,mask_img);	//extract_img = raw_img+mask_img
	cv::Mat cut_img(extract_img,cv::Rect(work1_x-extract_range,work1_y-extract_range,
				extract_range*2,extract_range*2));

	// cv::imshow("src", src_img);
	// cv::imshow("gray", mask_img);
	// cv::imshow("ex", extract_img);
	// cv::imshow("cut", cut_img);
	cv::imwrite("pointed.bmp",pointed_img);
	// cv::imwrite("c.bmp",extract_img);
	cv::imwrite("1.bmp",cut_img);
	cv::waitKey(0);
	cv::destroyAllWindows();

	return 0;
}
