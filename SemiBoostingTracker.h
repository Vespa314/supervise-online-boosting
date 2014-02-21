#include "StrongClassifier.h"
#include "Patch.h"
#define FLOAT_MAX 3.4e38f
class CSemiBoostingTracker
{
public:
	CSemiBoostingTracker(IplImage* frame_grey,CvRect track_rect,CvRect whole_size,
		int selector_num,int weakclassifier_num,float overlap,float search_factor,int iterationInit);
	~CSemiBoostingTracker();//最后要写

	 CStrongClassifier *mselector;
	 CStrongClassifier *mclassifier_offline;
	
	 bool track(IplImage* grey,CPatch* patches);
	 void smooth(IplImage* grey,CPatch* patches);
	 CvRect TrackingROI(float search_factor,CvRect whole_size,CvRect track_rect);
	
	float* m_confidences;
	int m_sizeConfidences;
	int m_numDetections;
	int* m_idxDetections;
	//int m_sizeDetections;
	int m_idxBestDetection;
	float m_maxConfidence;
	CvMat *m_confMatrix;
	CvMat *m_confMatrixSmooth;
	
	float confidence;
	float priorConfidence;
	CvRect result_rect;
};

CSemiBoostingTracker::CSemiBoostingTracker(IplImage* frame_grey,CvRect track_rect,CvRect whole_size,int selector_num,
										   int weakclassifier_num,float overlap,float search_factor,int iterationInit)
{
mselector = new CStrongClassifier(selector_num,weakclassifier_num,track_rect,iterationInit);
mclassifier_offline = new CStrongClassifier(selector_num,weakclassifier_num,track_rect,iterationInit);
//以下注释段为测试特征提取正确
//cvSetImageROI(frame_grey,track_rect);
//for(int i = 0;i < weakclassifier_num + iterationInit;i++)
//	{
//		for(int j = 0;j < mselector->selector[0]->weakclassifier[i]->feacture->m_numAreas;j++)
//		{
//		CvRect trackingRect;
//		trackingRect = mselector->selector[0]->weakclassifier[i]->feacture->m_scaleAreas[j];
//		cvRectangle(frame_grey,cvPoint(trackingRect.x,trackingRect.y),
//		cvPoint(trackingRect.x+trackingRect.width,trackingRect.y+trackingRect.height),cvScalar(255,0,0),1);
//		}
//	}
//cvShowImage("??1",frame_grey);
//cvWaitKey(0);
/*
for(int i = 0;i < weakclassifier_num + iterationInit;i++)
	{
		for(int j = 0;j < mclassifier_offline->selector[0]->weakclassifier[i]->feacture->m_numAreas;j++)
		{
		CvRect trackingRect;
		trackingRect = mclassifier_offline->selector[0]->weakclassifier[i]->feacture->m_scaleAreas[j];
		cvRectangle(frame_grey,cvPoint(trackingRect.x,trackingRect.y),
		cvPoint(trackingRect.x+trackingRect.width,trackingRect.y+trackingRect.height),cvScalar(255,0,0),1);
		}
	}
cvShowImage("??2",frame_grey);
cvWaitKey(0);
*/
	
	m_confidences = NULL;
	m_sizeConfidences = 0;
	m_maxConfidence = -FLOAT_MAX;
	m_numDetections = 0;
	m_idxDetections = NULL;
	m_idxBestDetection = -1;
	m_confMatrix = cvCreateMat(1,1,CV_32FC1);
	m_confMatrixSmooth = cvCreateMat(1,1,CV_32FC1);
result_rect = track_rect;
CvRect trackingROI = TrackingROI(search_factor,whole_size,result_rect);
/*cvRectangle(frame_grey,cvPoint(trackingROI.x,trackingROI.y),
		cvPoint(trackingROI.x+trackingROI.width,trackingROI.y+trackingROI.height),cvScalar(255,0,0),1);
cvShowImage("??3",frame_grey);
cvWaitKey(0);*/

built_iterlist(frame_grey,trackingROI);
inter_searchRegion = trackingROI;
CPatch* patches = new CPatch(trackingROI,whole_size,track_rect,overlap);
//this->m_confidences = new float[patches->m_patchGrid.height*patches->m_patchGrid.width];
//检查四个角
/*trackingROI = patches->m_rectLowerLeft;
cvRectangle(frame_grey,cvPoint(trackingROI.x,trackingROI.y),
		cvPoint(trackingROI.x+trackingROI.width,trackingROI.y+trackingROI.height),cvScalar(255,0,0),1);

trackingROI = patches->m_rectLowerRight;
cvRectangle(frame_grey,cvPoint(trackingROI.x,trackingROI.y),
		cvPoint(trackingROI.x+trackingROI.width,trackingROI.y+trackingROI.height),cvScalar(255,0,0),1);

trackingROI = patches->m_rectUpperLeft;
cvRectangle(frame_grey,cvPoint(trackingROI.x,trackingROI.y),
		cvPoint(trackingROI.x+trackingROI.width,trackingROI.y+trackingROI.height),cvScalar(255,0,0),1);

trackingROI = patches->m_rectUpperRight;
cvRectangle(frame_grey,cvPoint(trackingROI.x,trackingROI.y),
		cvPoint(trackingROI.x+trackingROI.width,trackingROI.y+trackingROI.height),cvScalar(255,0,0),1);
cvShowImage("??4",frame_grey);
cvWaitKey(0);*/
//检查所有patches
/*for(int i = 0;i < patches->num;i++)
	{
	IplImage *temp;
	temp = cvCloneImage(frame_grey);
	trackingROI = patches->patches[i];
	cvRectangle(temp,cvPoint(trackingROI.x,trackingROI.y),
		cvPoint(trackingROI.x+trackingROI.width,trackingROI.y+trackingROI.height),cvScalar(255,0,0),1);
	cvShowImage("??5",temp);
	cvWaitKey(2);
	}*/


for (int curInitStep = 0; curInitStep < iterationInit; curInitStep++)
	{
		cout<<"\rinit tracker..."<<int(((float)curInitStep)/(iterationInit-1)*100)<<"%";
		mselector->update (frame_grey, patches->m_rectUpperLeft, -1);//角落一定不是正样本
		mselector->update (frame_grey, track_rect, 1);//正样本priorconfidence为1
		mselector->update (frame_grey, patches->m_rectUpperRight, -1);
		mselector->update (frame_grey, track_rect, 1);
		mselector->update (frame_grey, patches->m_rectLowerLeft, -1);
		mselector->update (frame_grey, track_rect, 1);
		mselector->update (frame_grey, patches->m_rectLowerRight, -1);
		mselector->update (frame_grey, track_rect, 1);
	}
	printf (" done.\n");
	//one (first) shot learning
	iterationInit = 50;
	for (int curInitStep = 0; curInitStep < iterationInit; curInitStep++)
	{
		
		cout<<"\rinit tracker(offline classifier)..."<<int(((float)curInitStep)/(iterationInit-1)*100)<<"%";
		mclassifier_offline->update (frame_grey, track_rect, 1);
		mclassifier_offline->update (frame_grey, patches->m_rectUpperLeft, -1);
		mclassifier_offline->update (frame_grey, track_rect, 1);
		mclassifier_offline->update (frame_grey, patches->m_rectUpperRight, -1);
		mclassifier_offline->update (frame_grey, track_rect, 1);
		mclassifier_offline->update (frame_grey, patches->m_rectLowerLeft, -1);
		mclassifier_offline->update (frame_grey, track_rect, 1);
		mclassifier_offline->update (frame_grey, patches->m_rectLowerRight, -1);
	}
	printf (" done.\n");
	
	delete patches;
	confidence = -1;
	priorConfidence = -1;
}


CvRect CSemiBoostingTracker::TrackingROI(float search_factor,CvRect whole_size,CvRect track_rect)
{
CvRect searchRegion;
//在不超过范围情况下设为trackedPatch的search_factor倍大小	
/****************************************/
	searchRegion.y = (int)(track_rect.y-((float)track_rect.height*search_factor-track_rect.height)/2);
	if (searchRegion.y < 0) searchRegion.y = 0;
	searchRegion.x = (int)(track_rect.x-((float)track_rect.width*search_factor-track_rect.width)/2);
	if (searchRegion.x < 0) searchRegion.x = 0;
	searchRegion.height = (int)(track_rect.height*search_factor);
	searchRegion.width = (int)(track_rect.width*search_factor);

if (searchRegion.y+searchRegion.height > whole_size.height)
	searchRegion.height = whole_size.height-searchRegion.y;
if (searchRegion.x+searchRegion.width > whole_size.width)
	searchRegion.width = whole_size.width-searchRegion.x;
/****************************************/

return searchRegion;
}

void CSemiBoostingTracker::smooth(IplImage *grey, CPatch *patches)
{
	const float minMargin = 0;
	int numPatches = patches->num;
	if(m_confidences != NULL) delete[]m_confidences;
		m_confidences = new float[numPatches];

	m_numDetections = 0;
	m_idxBestDetection = -1;
	m_maxConfidence = -FLOAT_MAX;
	int numBaseClassifiers = mselector->mselector_num;

	CvSize patchGrid;
	patchGrid.height = patches->m_patchGrid.height;
	patchGrid.width = patches->m_patchGrid.width;
	//令m_confMatrix，m_confMatrixSmooth大小等于patchGrid
	 if((patchGrid.width != m_confMatrix->cols) || (patchGrid.height != m_confMatrix->rows)) 	 
	 {
		cvReleaseMat(&m_confMatrix);
		cvReleaseMat(&m_confMatrixSmooth);
		m_confMatrix = cvCreateMat(patchGrid.height,patchGrid.width,CV_32FC1);
	    m_confMatrixSmooth = cvCreateMat(patchGrid.height,patchGrid.width,CV_32FC1);
	 }

	 int curPatch = 0;
	// Eval and filter
	for(int row = 0; row < patchGrid.height; row++)
		for( int col = 0; col < patchGrid.width; col++) 
		{
			m_confidences[curPatch] = mselector->val(patches->patches[curPatch]) ; 
			// fill matrix
			cvmSet(m_confMatrix,row,col,m_confidences[curPatch]);
			curPatch++;
		}
		cvSmooth(m_confMatrix,m_confMatrixSmooth,CV_GAUSSIAN,3);

		curPatch = 0;
		for(int row = 0; row < patchGrid.height; row++) 
		{
			for( int col = 0; col < patchGrid.width; col++) 
			{
			// fill matrix
			m_confidences[curPatch] = float(cvmGet(m_confMatrixSmooth,row,col));
			if (m_confidences[curPatch] > m_maxConfidence)
				{
				m_maxConfidence = m_confidences[curPatch];
				m_idxBestDetection = curPatch;
				}
			if (m_confidences[curPatch] > minMargin) //自己设临界值
				{
					m_numDetections++;
				}
			curPatch++;
		}
	}
		//cout<<m_numDetections<<endl;
	if(m_idxDetections != NULL)
		delete []m_idxDetections;
	m_idxDetections = new int[m_numDetections];

	int curDetection = -1;
	for (int curPatch=0; curPatch < numPatches; curPatch++)
		if (m_confidences[curPatch]>minMargin) 
			m_idxDetections[++curDetection]=curPatch;

}


bool CSemiBoostingTracker::track(IplImage *grey,CPatch *patches)
{
	smooth(grey,patches);
	if(m_numDetections <= 0)
		{
		confidence = 0;
		priorConfidence = 0;
		return false;
		}
	result_rect = patches->patches[m_idxBestDetection];
	confidence = m_maxConfidence / this->mselector->sum_alpha();
	float off;

	CvRect tmp = patches->m_rectUpperLeft;
	off = this->mclassifier_offline->val(tmp)/this->mclassifier_offline->sum_alpha();
	this->mselector->update(grey,tmp,off);	
	priorConfidence = this->mclassifier_offline->val(result_rect)/this->mclassifier_offline->sum_alpha();
	this->mselector->update(grey,result_rect,priorConfidence);

	tmp = patches->m_rectLowerLeft;
	off = this->mclassifier_offline->val(tmp)/this->mclassifier_offline->sum_alpha();
	this->mselector->update(grey,tmp,off);	
	priorConfidence = this->mclassifier_offline->val(result_rect)/this->mclassifier_offline->sum_alpha();
	this->mselector->update(grey,result_rect,priorConfidence);

	tmp = patches->m_rectUpperRight;
	off = this->mclassifier_offline->val(tmp)/this->mclassifier_offline->sum_alpha();
	this->mselector->update(grey,tmp,off);	
	priorConfidence = this->mclassifier_offline->val(result_rect)/this->mclassifier_offline->sum_alpha();
	this->mselector->update(grey,result_rect,priorConfidence);
	
	tmp = patches->m_rectLowerRight;
	off = this->mclassifier_offline->val(tmp)/this->mclassifier_offline->sum_alpha();
	this->mselector->update(grey,tmp,off);	
	priorConfidence = this->mclassifier_offline->val(result_rect)/this->mclassifier_offline->sum_alpha();
	this->mselector->update(grey,result_rect,priorConfidence);
		
	return true;
	
}

CSemiBoostingTracker::~CSemiBoostingTracker()
{
delete mselector;
delete mclassifier_offline;
delete []m_confidences;
delete []m_idxDetections;
}
