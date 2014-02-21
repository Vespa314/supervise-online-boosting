#include "SemiBoostingTracker.h"
#include <time.h>

#define WINDOW_NAME "Semi Boosting"

CvRect trackingRect;//鼠标圈定的目标
int track_object = false;//false : 未跟踪物体




void on_mouse( int event, int x, int y, int flags, void* param )
{
	switch( event )
	{
	case CV_EVENT_LBUTTONDOWN:
		trackingRect.y = y;
		trackingRect.x = x;
		break;
	case CV_EVENT_LBUTTONUP:
		trackingRect.height =  y - trackingRect.y;
		trackingRect.width = x - trackingRect.x;
		track_object = 2;
		break;
	case CV_EVENT_MOUSEMOVE:

		if (flags == CV_EVENT_FLAG_LBUTTON)
		{
			trackingRect.height = y-trackingRect.y;
			trackingRect.width = x-trackingRect.x;
		}
		break;
	}
}

int track(IplImage* frame,CvRect track_rect,CvCapture* capture,int selector_num,
		   int weakclassifier_num,float overlap,float search_factor,int iterationInit)
{
IplImage* frame_grey;

frame_grey = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
cvCvtColor(frame,frame_grey,CV_BGR2GRAY);
CvRect whole_size;
whole_size.x = whole_size.y = 0;
whole_size.height = cvGetSize(frame).height;
whole_size.width = cvGetSize(frame).width;
//测试积分图
//
//for(int i = 1;i < track_rect.height ;i++)
//	for(int j =1;j < track_rect.width;j++)
//	{
//		CV_IMAGE_ELEM(frame_grey,uchar,i+track_rect.y,j+track_rect.x) = 255*inter_List[track_rect.y+i][track_rect.x+j]/inter_List[track_rect.height+track_rect.y][track_rect.width+track_rect.x];
//		//CV_IMAGE_ELEM(frame_grey,uchar,i+track_rect.y,j+track_rect.x) = inter_List[i][j] + inter_List[i-1][j-1] - inter_List[i-1][j] - inter_List[i][j-1];
//		//if(!(inter_List[i][j] > inter_List[i][j-1] && inter_List[i][j]>inter_List[i-1][j])) cout<<"wrog";
//	}
//
//cvShowImage("WINDOW_NAME2",frame_grey);
//cvWaitKey(0);


cout<<"init tracking...\n";

CSemiBoostingTracker *tracker;
tracker = new CSemiBoostingTracker(frame_grey,track_rect,whole_size,selector_num,weakclassifier_num,overlap,search_factor,iterationInit);


cout<<"start tracking (stop by pressing any key)...\n\n";
bool trackerLost = false;
int timer = 0;
while(cvWaitKey(2) < 0)
	{
	timer = clock();
	frame = cvQueryFrame(capture);
		//if()....
	cvReleaseImage(&frame_grey);
	frame_grey = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
	cvCvtColor(frame,frame_grey,CV_BGR2GRAY);
	
		
	CPatch* patches;

	CvRect searchRegion;
	if (!trackerLost)
		{
			searchRegion = tracker->TrackingROI(search_factor,whole_size,tracker->result_rect);
			patches = new CPatch(searchRegion,whole_size,track_rect,overlap);
		}
	else
		{
		searchRegion = tracker->TrackingROI(search_factor*2.0f,whole_size,tracker->result_rect);
		patches = new CPatch(searchRegion,whole_size,track_rect,overlap);
		/*cvRectangle(frame,cvPoint(searchRegion.x,searchRegion.y),
			cvPoint(searchRegion.x+searchRegion.width,searchRegion.y+searchRegion.height),
			cvScalar(255,0,0),1);*/
		}
	built_iterlist(frame_grey,searchRegion);
	inter_searchRegion = searchRegion;
	if (!tracker->track(frame_grey,patches))
			trackerLost = true;
	else 
			trackerLost = false;
	delete patches;
	
	if(!trackerLost)
	{
		cvRectangle(frame,cvPoint(tracker->result_rect.x,tracker->result_rect.y),
			cvPoint(tracker->result_rect.x+tracker->result_rect.width,tracker->result_rect.y+tracker->result_rect.height),
			cvScalar(0,0,255),2);
	int temp = clock();
	cout<<temp - timer<<"ms        ";
	timer = temp;
	cout<<"confidence:"<<tracker->confidence<<"   prior confidence:"<<tracker->priorConfidence;
	cout<<"tracking....\r";
	}
	else
		{
		int temp = clock();
		cout<<temp - timer<<"ms        ";
		timer = temp;
		cout<<"TRACKER LOST, waiting...                                \r";
		}
	cvShowImage(WINDOW_NAME,frame);
	}
delete tracker;
	return 0;
}

 

int main()
{
CvCapture* capture = cvCaptureFromCAM(0);
 /////////////////////要有tips
if(!capture)
return 0;
IplImage* frame = NULL;
CvRect track_rect;
//IplImage* object_frame = NULL;
cvNamedWindow(WINDOW_NAME,0);
cvSetMouseCallback(WINDOW_NAME, on_mouse, 0 );
while(1)
	{
	frame = cvQueryFrame(capture);
	
	char key = cvWaitKey(2);
	if(key == 27)
		break;
	else if(key == 13 && track_object)
			{
				//object_frame = cvCloneImage(frame);
				//cvSetImageROI(object_frame,trackingRect);
				//cvShowImage("WINDOW_NAME2",object_frame);
				//cout<<"tracking";
				//track(object_frame,capture,100,50,0.99,2.0);
				track_rect = trackingRect;
				if(track(frame,track_rect,capture,100,100,0.99f,2.0f,50) == 0)
					break;
			}
	cvRectangle(frame,cvPoint(trackingRect.x,trackingRect.y),
		cvPoint(trackingRect.x+trackingRect.width,trackingRect.y+trackingRect.height),cvScalar(0,128,255),3);
	cvShowImage(WINDOW_NAME,frame);
	}
cvReleaseCapture(&capture);
cvDestroyWindow(WINDOW_NAME);
return 0;
}