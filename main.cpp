#include <opencv2/opencv.hpp>

using namespace std;

string src_image_path = "17-02-27_17:16:28/Labeling.bmp";
string raw_image_path = "17-02-27_17:16:28/Image23.bmp";
int work1_x = 524,work1_y = 278;
int work2_x = 722,work2_y = 376;
int work3_x = 234,work3_y = 742;

int main(int argc, const char* argv[]){
  cv::Mat src_img = cv::imread(src_image_path);
  cv::Mat raw_img = cv::imread(raw_image_path);
  // if(src_img.empty()){
  //   std::cerr << "Failed to open image file." << std::endl;
  //   return -1; 
  // }
  cv::Mat gray_img,mask_img;
  cv::cvtColor(src_img,gray_img,CV_RGB2GRAY);

  circle(src_img, cv::Point(work1_x, work1_y), 20, cv::Scalar(200, 200, 200), 2, 2);
  circle(src_img, cv::Point(work2_x, work2_y), 20, cv::Scalar(200, 200, 200), 2, 2);
  circle(src_img, cv::Point(work3_x, work3_y), 20, cv::Scalar(200, 200, 200), 2, 2);

  int intensity = gray_img.at<unsigned char>(work2_y, work2_x);
  cv::inRange(gray_img,intensity,intensity,mask_img);	//threshold in range

  cv::Mat extract_img;
  raw_img.copyTo(extract_img,mask_img);

  cv::imshow("src", src_img);
  cv::imshow("gray", mask_img);
  cv::imshow("ex", extract_img);
  cv::imwrite("src.bmp",src_img);
  cv::imwrite("c.bmp",extract_img);
  cv::waitKey(0);
  cv::destroyAllWindows();

  return 0;
}
