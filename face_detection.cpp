#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <stdio.h>

using namespace std;
using namespace cv;

#define CAM_WIDTH 480
#define CAM_HEIGHT 300 
int main(int argc, char *argv[])
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
    face_classifier.load("/home/pi/opencv_src/opencv/data/haarcascades/haarcascade_frontalface_default.xml");
    int test=0;
    int num_set=0;
    while(1){
        bool frame_valid = true;

        Mat frame_original;
        Mat frame;

        try{
            //카메라로부터 이미지 얻어오기
            cap >> frame_original;
        }catch(Exception& e){
            cerr << "Execption occurred." << endl;
            frame_valid = false;
        }

        if(frame_valid){
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
		    char savefile[100];
		    cap>>frame;
		    imshow("frame",frame);
		    sprintf(savefile,"image%d.jpg",test++);
		    imwrite(savefile,frame);
		    std::cout<<num_set<<std::endl;
                    rectangle(frame_original, lb, tr, Scalar(0, 255, 0), 3, 4, 0);
		    waitKey(1000);
		    num_set++;
		    if(num_set>=8){
		    	return 0;
		    }
		    /*CvCapture* cvCreateCameraCapture(int index);
		    int cvGrabFrame(CvCapture* capture);
		    IplImage* cvRetrieveFrame(CvCapture* capture);
		    void cvRelease(CvCapture*& capture);
		    */
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

    return 0;
}
