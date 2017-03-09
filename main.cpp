#include <opencv2/opencv.hpp>
#include <fstream>

using namespace std;

#define workMaxNum 5
#define extractRange 150

void errorMes(string mes){
	cerr<<mes<<endl;
	exit(0);
}

class CWork{
	private:
		string findWord;
		string firstWord;	//"<" + findWord + ">";
		string lastWord;	//"</" + findWord + ">";
	public:
		string listPath = "../log/list.txt";
		string logDir;			// /home/xxx/program/log/17-02-27_16:30:32/
		string labelImagePath;	//logDir + "Labeling.bmp";
		string Image23Path;		//logDir + "Image23.bmp";
		string xmlPath;			//logDir + "output.xml";
		int posX[workMaxNum];	//work position x [pixel]
		int posY[workMaxNum];	//work position y [pixel]
		int deg[workMaxNum];	//work deg
		int workNum;
		void updateFilePath(string dirPath);
		void updateFindWord(string updateWord);
		void findLocation(string dirPath);
};

void CWork::updateFilePath(string dirPath){
	logDir = dirPath;
	labelImagePath = logDir + "Labeling.bmp";
	Image23Path = logDir + "Image23.bmp";
	xmlPath = logDir + "output.xml";
}
void CWork::updateFindWord(string updateWord){
	findWord = updateWord;
	firstWord = "<" + findWord + ">";
	lastWord = "</" + findWord + ">";
}
void CWork::findLocation(string dirPath){
	updateFilePath(dirPath);

	ifstream ifs(xmlPath);
	if (ifs.fail()) errorMes("ifs failed");

	workNum=0;
	updateFindWord("pix_X");
	string::size_type firstWordPos;
	int findWordFlag=0;
	int startExtractPos;
	int endExtractPos;
	string str;
	while(getline(ifs, str)){	// read every line
		firstWordPos = str.find(firstWord);
		if(firstWordPos != string::npos && findWordFlag==0){	// if find "pix_X"
			startExtractPos = firstWordPos + firstWord.length();
			endExtractPos = str.find(lastWord)-firstWordPos-lastWord.length()+1;
			posX[workNum] = stoi(str.substr(startExtractPos,endExtractPos));	//extract num
			findWordFlag=1;
			updateFindWord("pix_Y");
		}
		else if(firstWordPos != string::npos && findWordFlag==1){	// if find "pix_Y"
			startExtractPos = firstWordPos + firstWord.length();
			endExtractPos = str.find(lastWord)-firstWordPos-lastWord.length()+1;
			posY[workNum] = stoi(str.substr(startExtractPos,endExtractPos));	//extract num
			findWordFlag=2;
			updateFindWord("deg_C");
		}
		else if(firstWordPos != string::npos && findWordFlag==2){	// if find "deg_C"
			startExtractPos = firstWordPos + firstWord.length();
			endExtractPos = str.find(lastWord)-firstWordPos-lastWord.length()+1;
			deg[workNum] = stoi(str.substr(startExtractPos,endExtractPos));	//extract num
			workNum++;
			findWordFlag=0;
			updateFindWord("pix_X");
		}
	}
	for(int i=0;i<workNum;i++){
		cout<<posX[i]<<","<<posY[i]<<","<<deg[i]<<endl;
	}
}


int main(int argc, const char* argv[]){
	CWork work;
	ifstream ifs(work.listPath);
	if (ifs.fail()) errorMes("ifs failed");
	string listLine;
	while(getline(ifs, listLine)){	// read every line
		cout<<listLine<<endl;
		work.findLocation(listLine);

		cv::Mat labelImage = cv::imread(work.labelImagePath);
		cv::Mat Image23 = cv::imread(work.Image23Path);
		if(labelImage.empty()) errorMes("Failed to open image file.");

		cv::Mat pointImage = labelImage.clone();
		for(int i=0;i<work.workNum;i++){	// pointImage = work centor coordinates circled image
			circle(pointImage, cv::Point(work.posX[i], work.posY[i]),
				   	20, cv::Scalar(200, 200, 200), 2, 2);
		}
		cv::imwrite(work.logDir + "pointImage.bmp",pointImage);

		cv::Mat hsvImage,maskImage;
		cv::cvtColor(labelImage,hsvImage,CV_BGR2HSV);				//BGR to HSV

		for(int i=0;i<work.workNum;i++){
			int hue = hsvImage.at<cv::Vec3b>(work.posY[i], work.posX[i])[0];
			int sat = hsvImage.at<cv::Vec3b>(work.posY[i], work.posX[i])[1];
			int val = hsvImage.at<cv::Vec3b>(work.posY[i], work.posX[i])[2];
			// cout<<hue<<sat<<val<<endl;

			// maskImage = extracted with specific hsv values image
			cv::inRange(hsvImage,cv::Scalar(hue,sat,val,0),cv::Scalar(hue,sat,val,0),maskImage);

			cv::Mat extractImage;
			Image23.copyTo(extractImage,maskImage);	//extractImage = Image23 + maskImage
			// cutImage = extractImage cut out in extractRange square
			int startRectPos = work.posX[i]-extractRange;
			int endRectPos = work.posY[i]-extractRange;
			cv::Mat cutImage(extractImage,cv::Rect(startRectPos,endRectPos,extractRange*2,extractRange*2));
			cv::imwrite(work.logDir + to_string(i) + ".bmp",cutImage);	//save cutImage
		}
	}

	// cv::imshow("point", pointImage);
	// cv::imshow("gray", maskImage);
	// cv::waitKey(0);
	// cv::destroyAllWindows();

	return 0;
}
