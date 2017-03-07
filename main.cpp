#include <opencv2/opencv.hpp>
#include <fstream>

using namespace std;

#define workMaxNum 4
#define extractRange 150

class CWork{
	private:
		string findWord;
		string firstWord;	//"<" + findWord + ">";
		string lastWord;	//"</" + findWord + ">";
	public:
		string logDir = "17-02-27_17:16:28/";
		string labelImagePath = logDir + "Labeling.bmp";
		string Image23Path = logDir + "Image23.bmp";
		string xmlPath = logDir + "output.xml";
		int workX[workMaxNum];	//work position x [pixel]
		int workY[workMaxNum];	//work position y [pixel]
		int workDeg[workMaxNum];	//work deg
		int workNum;
		void updateFindWord(string updateWord);
		void findLocation();
};

void CWork::updateFindWord(string updateWord){
	findWord = updateWord;
	firstWord = "<" + findWord + ">";
	lastWord = "</" + findWord + ">";
}
void CWork::findLocation(){
	std::ifstream ifs(xmlPath);
	std::string str;
	if (ifs.fail()){
		std::cerr << "failed" << std::endl;
		return;
	}
	workNum=0;
	updateFindWord("pix_X");
	string::size_type firstWordPos;
	string::size_type lastWordPos;
	int findWordFlag=0;
	while(getline(ifs, str)){	// read every line
		firstWordPos = str.find(firstWord);
		if(firstWordPos != string::npos && findWordFlag==0){	// if find "pix_X"
			lastWordPos = str.find(lastWord);
			workX[workNum] = stoi(str.substr(firstWordPos + firstWord.length(),lastWordPos-firstWordPos-lastWord.length()+1));
			findWordFlag=1;
			updateFindWord("pix_Y");
		}
		else if(firstWordPos != string::npos && findWordFlag==1){	// if find "pix_Y"
			lastWordPos = str.find(lastWord);
			workY[workNum] = stoi(str.substr(firstWordPos + firstWord.length(),lastWordPos-firstWordPos-lastWord.length()+1));
			findWordFlag=2;
			updateFindWord("deg_C");
		}
		else if(firstWordPos != string::npos && findWordFlag==2){	// if find "deg_C"
			lastWordPos = str.find(lastWord);
			workDeg[workNum] = stoi(str.substr(firstWordPos + firstWord.length(),lastWordPos-firstWordPos-lastWord.length()+1));
			workNum++;
			findWordFlag=0;
			updateFindWord("pix_X");
		}
	}
	for(int i=0;i<workMaxNum;i++){
		cout<<workX[i]<<","<<workY[i]<<","<<workDeg[i]<<endl;
	}
}


int main(int argc, const char* argv[]){
	CWork work;
	work.findLocation();

	cv::Mat labelImage = cv::imread(work.labelImagePath);
	cv::Mat Image23 = cv::imread(work.Image23Path);
	if(labelImage.empty()){
	  cout << "Failed to open image file." << endl;
	  return -1; 
	}
	// cv::cvtColor(src_img,gray_img,CV_RGB2GRAY);

	cv::Mat pointImage = labelImage.clone();
	for(int i=0;i<work.workNum;i++){	// pointImage = work centor coordinates circled image
		circle(pointImage, cv::Point(work.workX[i], work.workY[i]), 20, cv::Scalar(200, 200, 200), 2, 2);
	}

	cv::Mat hsvImage,maskImage;
	cv::cvtColor(labelImage,hsvImage,CV_BGR2HSV);				//BGR to HSV

	for(int i=0;i<work.workNum;i++){
		int hue = hsvImage.at<cv::Vec3b>(work.workY[i], work.workX[i])[0];
		int sat = hsvImage.at<cv::Vec3b>(work.workY[i], work.workX[i])[1];
		int val = hsvImage.at<cv::Vec3b>(work.workY[i], work.workX[i])[2];
		// cout<<hue<<sat<<val<<endl;

		// maskImage = extracted with specific hsv values image
		cv::inRange(hsvImage, cv::Scalar(hue,sat,val,0), cv::Scalar(hue,sat,val,0), maskImage);

		cv::Mat extractImage;
		Image23.copyTo(extractImage,maskImage);	//extractImage = Image23 + maskImage
		// cutImage = extractImage cut out in extractRange square
		cv::Mat cutImage(extractImage,cv::Rect(work.workX[i]-extractRange,work.workY[i]-extractRange,extractRange*2,extractRange*2));
		cv::imwrite(work.logDir + to_string(i) + ".bmp",cutImage);	//save cutImage
	}

	// cv::imshow("point", pointImage);
	// cv::imshow("gray", maskImage);
	// cv::waitKey(0);
	// cv::destroyAllWindows();

	return 0;
}
