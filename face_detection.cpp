#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <mutex>

using namespace std;
using namespace cv;

mutex f_mtx;
class Face{
 private:
	char* original_face;
	char* unique_key;
	int num;
 public:
	Face(char* file_path,char* key_value){
		int len=strlen(file_path)+1;
		original_face=new char[len];
		strcpy(original_face,file_path);
		len=strlen(key_value)+1;
		unique_key=new char[len];
		strcpy(unique_key,key_value);
	}
	/*
	int GetFaceNum(){
		return num;
	}
	*/
	void GetNum(int x){
		num=x;
	}
	void NumMinus(){
		num--;
	}
	/*
	void AddPicture(char* path){
		int len=strlen(path)+1;
	}
	*/
	~Face(){
		delete original_face;
		delete unique_key;
		cout<<"Face Delete"<<endl;
	}
};

class FaceManager{
 private:
	Face* face_data[5];		//관리해주는 사람 얼굴 수
	char* face_compare[20];		//CCTV에서 검출된 얼굴 수
	int data_count;		//관리 얼굴 총 갯수
	int compare_count;	//비교 얼굴 총 갯수
 public:
	FaceManager():data_count(0),compare_count(0){}
	~FaceManager(){
		int i;
		for(i=0;i<compare_count;i++){
			delete face_compare[i];
		}
	}
	int GetCompareCount(){
		return compare_count;
	}
	int GetDataCount(){
		return data_count;
	}
	void AddFace(Face* face){
		f_mtx.lock();		//face_data에 접근하는 뮤텍스
		face->GetNum(data_count);
		face_data[data_count++]=face;
		f_mtx.unlock();
		cout<<"Face add: "<<data_count<<endl;
	}
	void AddCompareFace(char* path){
		int len=strlen(path)+1;
		face_compare[compare_count]=new char[len];
		strcpy(face_compare[compare_count],path);
		compare_count++;
		cout<<"Compare Face: "<<compare_count<<endl;
	}
	void RemoveData(int face_num){ //mutex걸어야함
		int select_num=face_num;	//지울때 선택하는 번호
		face_data[select_num]->~Face();
		if(data_count>1 && select_num != (data_count-1)){
			int i;
			f_mtx.lock();
			for(i=select_num; i<(data_count-1); i++){
				face_data[i+1]->NumMinus();
				face_data[i]=face_data[i+1];
			}
			f_mtx.unlock();
		}
		data_count--;
		cout<<"FaceData delete"<<endl;
	}
	void CompareFaceInit(){
		int i;
		cout<<"compare_face_data Delete"<<endl;
		f_mtx.lock();
		for(i=0;i<compare_count;i++){
			//cout<<"delete 실행 전"<<endl;
			delete face_compare[i];
			//cout<<"delete 실행 후"<<endl;
		}
		cout<<"실행 완료"<<endl;
		compare_count=0;
		f_mtx.unlock();
	}
	/*
	~FaceManager(){
		int i;
		for(i=0;i<compare_count;i++){
			delete face_compare;
		}
	}
	*/
/*
	void CompareFace(){	//thread 생성해서 작동
		int i,j;
		for(i=0;i<data_count;i++){
			for(j=0;j<compare_count;j++){
				bool tf=true; //얼굴 비교 확인용 (참으로 가정)
				//
				//외부 솔루션을 통한 얼굴 비교
				//
				if(tf){	//i,j가 같은 얼굴이라고 들어온 경우
					//socket을 통하여 face_data[i] 데이터 전송 이때도 mutex 걸어야함
					RemoveData(face_data[i]);
				}
			}
		}
		CompareFaceInit();
		compare_count=0;

	}
*/
};
class TimeManagement{
 private:
	clock_t start;
	clock_t end;
 public:
	void TimeStartReset(){
		start=0;
	}
	void TimeStart(){
		start=clock();
	}
	int TimeEnd(){
		if(start!=0){
			int time;
			end=clock();
			time=((int)(end-start)/1000000);
			cout<<"Time: "<<time<<endl;
			return time;
		}
		return 0;
	}
};
void dataReceive(FaceManager* fm){
	/*
	socket을 통해서 큰 보드에서 사진경로와 키값 데이터를 받는 소스를 구현
	*/

	//임시 코드
	int i=0;
	cout<<"thread create"<<endl;
	char face_path[]="/home/pi/ansehen/lee_face.jpg";
	char key[]="0123456789";
	Face* leeho = new Face(face_path,key);
	cout<<"object created!"<<endl;
	fm->AddFace(leeho);
	cout<<"add leeho"<<endl;
	//thread로 socket을 통해 데이터(original_face+unique_key)를 받는 부분
	while(i<1000000000){
		i++;
	}
	cout<<"thread end"<<endl;
	cout<<"---------------------------"<<endl;
}
void KairosCommunication(FaceManager* fm){ //타이머 종료, 일정 사진이 찍힌경우
	int i,j;
	int i_max=fm->GetDataCount();
	int j_max=fm->GetCompareCount();
	cout<<"i_max: "<<i_max<<", j_max: "<<j_max<<endl;
	bool tf=true;
	for(i=0; i<i_max;i++){
		cout<<"i: "<<i<<endl;
		for(j=0;j<j_max;j++){
			cout<<"j: "<<j<<endl;
			/*
			외부 솔루션을 통한 해결(kairos)
			*/
			if(tf){
				cout<<"exe. in if"<<endl;
				//socket을 통하여 face_data[i] 데이터 전송
				fm->RemoveData(i);
				//tf=false;
				break;
			}
		}
	}
	fm->CompareFaceInit();
}
#define CAM_WIDTH 480
#define CAM_HEIGHT 300
//int face_num=0;
//int count_num=0;
//int image_num=0;

FaceManager* fm=new FaceManager;
TimeManagement timer;
int main()
{
    VideoCapture cap(0);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAM_WIDTH);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);

    if(!cap.isOpened()){
        cerr << "Can't Open Camera" << endl;
        return -1;
    }

    namedWindow("Face", 1);

    //cascadeclassifier 클랙스
    CascadeClassifier face_classifier;

    //얼굴 인식 xml 로딩
    thread face_receive(&dataReceive,fm);
    face_classifier.load("/home/pi/opencv_src/opencv/data/haarcascades/haarcascade_frontalface_default.xml");
    while(1){
        Mat frame_original;
        Mat frame;
	Mat face_image;
        try{
            //카메라로부터 이미지 얻어오기
            cap >> frame_original;
        }catch(Exception& e){
            cerr << "Execption occurred." << endl;
        }

        if(1){
            try{
                Mat grayframe;
                //gray scale로 변환
                cvtColor(frame_original, grayframe, CV_BGR2GRAY);
                //histogram 얻기
                equalizeHist(grayframe, grayframe);

                //이미지 표시용 변수
                vector<Rect> faces;

                //얼굴의 위치와 영역을 탐색한다.

//detectMultiScale(const Mat& image, vector<Rect>& objects, double scaleFactor=1.1,
//              int minNeighbors=3, int flags=0, Size minSize=Size(), Size maxSize=Size())
//image 실제 이미지
//objects 얼굴 검출 위치와 영역 변수
//scaleFactor 이미지 스케일
//minNeighbors 얼굴 검출 후보들의 갯수
//flags 이전 cascade와 동일하다 cvHaarDetectObjects 함수 에서
//      새로운 cascade에서는 사용하지 않는다.
//minSize 가능한 최소 객체 사이즈
//maxSize 가능한 최대 객체 사이즈
                /*face_classifier.detectMultiScale(grayframe, faces,
                    1.1,
                    3,
                    CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE,
                    Size(30, 30));*/
                face_classifier.detectMultiScale(grayframe, faces,
                    1.1,
                    3,
                    0,//CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE,
                    Size(30, 30));
		cout<<"얼굴 인식 전"<<endl;
		if(timer.TimeEnd()>5){
			timer.TimeStartReset();
			cout<<"Time out"<<endl;
			cout<<"compare start!!"<<endl;
			thread faceTimeComparison(&KairosCommunication,fm);
			faceTimeComparison.join();
			//얼굴비교 시작
		}
                for(int i=0;i<faces.size();i++){
                    Point lb(faces[i].x + faces[i].width,
                        faces[i].y + faces[i].height);
                    Point tr(faces[i].x, faces[i].y);
//rectangle(Mat& img, Point pt1, Point pt2, const Scalar& color, int thickness=1, int lineType=8, int shift=0)
//img 적용할 이미지
//pt1 그릴 상자의 꼭지점
//pt2 pt1의 반대편 꼭지점
//color 상자의 색상
//thickness 상자의 라인들의 두께 음수 또는 CV_FILLED를 주면 상자를 채운다.
//lineType 라인의 모양 line()함수 확인하기
//shift ?? Number of fractional bits in the point coordinates.
//포인트 좌표의 분수 비트의 수??
		    cout<<"얼굴 확인 됨"<<endl;

		    timer.TimeStart();

		    char savefile[100];
		    cap>>frame;
		    cap>>face_image;
		    Rect rect(tr.x,tr.y,(lb.x-tr.x)+10,(lb.y-tr.y)+10);
		    face_image=face_image(rect);
		    //imshow("image",face_image);
		    int compare_face_num=fm->GetCompareCount();
		    sprintf(savefile,"face_image %d.jpg",compare_face_num);
		    
		    char compare_path[]="/home/pi/ansehen/";
		    strcat(compare_path,savefile);
		    fm->AddCompareFace(compare_path);
		    cout<<savefile<<endl;
		
		    imwrite(savefile,face_image);
		    imshow("CCTV",frame);
		    if(compare_face_num>5){
			timer.TimeStartReset();
			cout<<"compare start!!"<<endl;
			thread faceComparison(&KairosCommunication,fm);
			faceComparison.join();
		    }
		    //sprintf(savefile,"image %d_%d.jpg",face_num,count_num++);
		    //imwrite(savefile,frame);
                    rectangle(frame_original, lb, tr, Scalar(0, 255, 0), 3, 4, 0);
		    waitKey(500);
                }
//윈도우에 이미지 그리기
                imshow("Face", frame_original);
            }catch(Exception& e){

                cerr << "Exception occurred. face" << endl;
            }
//키 입력 대기
            if(waitKey(10) >= 0) break;
        }
    }
    face_receive.join();
    return 0;
}
