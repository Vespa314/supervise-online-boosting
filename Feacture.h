#include "highgui.h"
#include "cv.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#define INITSIGMA( numAreas ) ( static_cast<float>( sqrt( 256.0f*256.0f / 12.0f * (numAreas) ) ) );
float **inter_List = NULL;//积分图
CvRect inter_searchRegion;
class CFeacture
{
public:
	CFeacture(CvRect patchSize);
	~CFeacture();

	//int //m_type;
	int m_numAreas;
	int* m_weights;
	float m_initMean;
	float m_initSigma;

	CvRect* m_areas;     // areas within the patch over which to compute the feature
	CvSize m_initSize;   // size of the patch used during training
	CvSize m_curSize;    // size of the patches currently under investigation
	//float m_scaleFactorHeight;  // scaling factor in vertical direction
	//float m_scaleFactorWidth;   // scaling factor in horizontal direction
	CvRect* m_scaleAreas;// areas after scaling
	float* m_scaleWeights; // weights after scaling
	float m_response;
	//CvPoint m_offset;
};



CFeacture::CFeacture(CvRect patchSize)
:m_areas(NULL), m_weights(NULL), m_scaleAreas(NULL), m_scaleWeights(NULL)
{
	CvPoint position;
	CvSize baseDim;
	CvSize sizeFactor;
	int area;

	CvSize minSize;
	minSize.height = minSize.width = 3;
	int minArea = 9;

	//m_offset.x = patchSize.x;
	//m_offset.y = patchSize.y;

	bool valid = false;
	while (!valid)
	{
		//chosse position and scale
		position.y = rand()%(patchSize.height);
		position.x = rand()%(patchSize.width);

		baseDim.width = (int) ((1-sqrt(1-(float)rand()/RAND_MAX))*patchSize.width);
		baseDim.height = (int) ((1-sqrt(1-(float)rand()/RAND_MAX))*patchSize.height);
	
		//select types
		//float probType[11] = {0.0909f, 0.0909f, 0.0909f, 0.0909f, 0.0909f, 0.0909f, 0.0909f, 0.0909f, 0.0909f, 0.0909f, 0.0950f};
		float probType[11] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};//各种特征选择优先级??
		float prob = (float)rand()/RAND_MAX;

		if (prob < probType[0]) 
		{
			//check if feature is valid
			sizeFactor.height = 2;
			sizeFactor.width = 1;
			if (position.y + baseDim.height*sizeFactor.height >= patchSize.height ||
				position.x + baseDim.width*sizeFactor.width >= patchSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minArea)
				continue;

			//m_type = 1;
			m_numAreas = 2;
			m_weights = new int[m_numAreas];
			m_weights[0] = 1;
			m_weights[1] = -1;
			m_areas = new CvRect[m_numAreas];
			m_areas[0].x = position.x;
			m_areas[0].y = position.y;
			m_areas[0].height = baseDim.height;
			m_areas[0].width = baseDim.width;
			m_areas[1].x = position.x;
			m_areas[1].y = position.y+baseDim.height;
			m_areas[1].height = baseDim.height;
			m_areas[1].width = baseDim.width;
			m_initMean = 0;//??
			m_initSigma = INITSIGMA( m_numAreas ); //??什么原理？
			//#define INITSIGMA( numAreas ) ( static_cast<float>( sqrt( 256.0f*256.0f / 12.0f * (numAreas) ) ) );
			valid = true;

		}
		else if (prob < probType[0]+probType[1]) 
		{
			//check if feature is valid
			sizeFactor.height = 1;
			sizeFactor.width = 2;
			if (position.y + baseDim.height*sizeFactor.height >= patchSize.height ||
				position.x + baseDim.width*sizeFactor.width >= patchSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minArea)
				continue;

			//m_type = 2;
			m_numAreas = 2;
			m_weights = new int[m_numAreas];
			m_weights[0] = 1;
			m_weights[1] = -1;
			m_areas = new CvRect[m_numAreas];
			m_areas[0].x = position.x;
			m_areas[0].y = position.y;
			m_areas[0].height = baseDim.height;
			m_areas[0].width = baseDim.width;
			m_areas[1].x = position.x+baseDim.width;
			m_areas[1].y = position.y;
			m_areas[1].height = baseDim.height;
			m_areas[1].width = baseDim.width;
			m_initMean = 0;
			m_initSigma = INITSIGMA( m_numAreas ); 
			valid = true;

		}
		else if (prob < probType[0]+probType[1]+probType[2]) 
		{
			//check if feature is valid
			sizeFactor.height = 4;
			sizeFactor.width = 1;
			if (position.y + baseDim.height*sizeFactor.height >= patchSize.height ||
				position.x + baseDim.width*sizeFactor.width >= patchSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minArea)
				continue;

			//m_type = 3;
			m_numAreas = 3;
			m_weights = new int[m_numAreas];
			m_weights[0] = 1;
			m_weights[1] = -2;
			m_weights[2] = 1;
			m_areas = new CvRect[m_numAreas];
			m_areas[0].x = position.x;
			m_areas[0].y = position.y;
			m_areas[0].height = baseDim.height;
			m_areas[0].width = baseDim.width;
			m_areas[1].x = position.x;
			m_areas[1].y = position.y+baseDim.height;
			m_areas[1].height = 2*baseDim.height;
			m_areas[1].width = baseDim.width;
			m_areas[2].y = position.y+3*baseDim.height;
			m_areas[2].x = position.x;
			m_areas[2].height = baseDim.height;
			m_areas[2].width = baseDim.width;
			m_initMean = 0;
			m_initSigma = INITSIGMA( m_numAreas ); 
			valid = true;
		}
		else if (prob < probType[0]+probType[1]+probType[2]+probType[3])
		{
		//check if feature is valid
			sizeFactor.height = 1;
			sizeFactor.width = 4;
			if (position.y + baseDim.height*sizeFactor.height >= patchSize.height ||
				position.x + baseDim.width*sizeFactor.width >= patchSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minArea)
				continue;

			//m_type = 4;
			m_numAreas = 3;
			m_weights = new int[m_numAreas];
			m_weights[0] = 1;
			m_weights[1] = -2;
			m_weights[2] = 1;
			m_areas = new CvRect[m_numAreas];
			m_areas[0].x = position.x;
			m_areas[0].y = position.y;
			m_areas[0].height = baseDim.height;
			m_areas[0].width = baseDim.width;
			m_areas[1].x = position.x+baseDim.width;
			m_areas[1].y = position.y;
			m_areas[1].height = baseDim.height;
			m_areas[1].width = 2*baseDim.width;
			m_areas[2].y = position.y;
			m_areas[2].x = position.x+3*baseDim.width;
			m_areas[2].height = baseDim.height;
			m_areas[2].width = baseDim.width;
			m_initMean = 0;
			m_initSigma = INITSIGMA( m_numAreas ); 
			valid = true;
		}
		else if (prob < probType[0]+probType[1]+probType[2]+probType[3]+probType[4])
		{
		//check if feature is valid
			sizeFactor.height = 2;
			sizeFactor.width = 2;
			if (position.y + baseDim.height*sizeFactor.height >= patchSize.height ||
				position.x + baseDim.width*sizeFactor.width >= patchSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minArea)
				continue;

			//m_type = 5;
			m_numAreas = 4;
			m_weights = new int[m_numAreas];
			m_weights[0] = 1;
			m_weights[1] = -1;
			m_weights[2] = -1;
			m_weights[3] = 1;
			m_areas = new CvRect[m_numAreas];
			m_areas[0].x = position.x;
			m_areas[0].y = position.y;
			m_areas[0].height = baseDim.height;
			m_areas[0].width = baseDim.width;
			m_areas[1].x = position.x+baseDim.width;
			m_areas[1].y = position.y;
			m_areas[1].height = baseDim.height;
			m_areas[1].width = baseDim.width;
			m_areas[2].y = position.y+baseDim.height;
			m_areas[2].x = position.x;
			m_areas[2].height = baseDim.height;
			m_areas[2].width = baseDim.width;
			m_areas[3].y = position.y+baseDim.height;
			m_areas[3].x = position.x+baseDim.width;
			m_areas[3].height = baseDim.height;
			m_areas[3].width = baseDim.width;
			m_initMean = 0;
			m_initSigma = INITSIGMA( m_numAreas ); 
			valid = true;
		}
		else if (prob < probType[0]+probType[1]+probType[2]+probType[3]+probType[4]+probType[5])
		{
			//check if feature is valid
			sizeFactor.height = 3;
			sizeFactor.width = 3;
			if (position.y + baseDim.height*sizeFactor.height >= patchSize.height ||
				position.x + baseDim.width*sizeFactor.width >= patchSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minArea)
				continue;

			//m_type = 6;
			m_numAreas = 2;
			m_weights = new int[m_numAreas];
			m_weights[0] = 1;
			m_weights[1] = -9;
			m_areas = new CvRect[m_numAreas];
			m_areas[0].x = position.x;
			m_areas[0].y = position.y;
			m_areas[0].height = 3*baseDim.height;
			m_areas[0].width = 3*baseDim.width;
			m_areas[1].x = position.x+baseDim.width;
			m_areas[1].y = position.y+baseDim.height;
			m_areas[1].height = baseDim.height;
			m_areas[1].width = baseDim.width;
			m_initMean = -8*128;
			m_initSigma = INITSIGMA( m_numAreas ); 
			valid = true;
		}
		else if (prob< probType[0]+probType[1]+probType[2]+probType[3]+probType[4]+probType[5]+probType[6]) 
		{
			//check if feature is valid
			sizeFactor.height = 3;
			sizeFactor.width = 1;
			if (position.y + baseDim.height*sizeFactor.height >= patchSize.height ||
				position.x + baseDim.width*sizeFactor.width >= patchSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minArea)
				continue;

			//m_type = 7;
			m_numAreas = 3;
			m_weights = new int[m_numAreas];
			m_weights[0] = 1;
			m_weights[1] = -2;
			m_weights[2] = 1;
			m_areas = new CvRect[m_numAreas];
			m_areas[0].x = position.x;
			m_areas[0].y = position.y;
			m_areas[0].height = baseDim.height;
			m_areas[0].width = baseDim.width;
			m_areas[1].x = position.x;
			m_areas[1].y = position.y+baseDim.height;
			m_areas[1].height = baseDim.height;
			m_areas[1].width = baseDim.width;
			m_areas[2].y = position.y+baseDim.height*2;
			m_areas[2].x = position.x;
			m_areas[2].height = baseDim.height;
			m_areas[2].width = baseDim.width;
			m_initMean = 0;
			m_initSigma = INITSIGMA( m_numAreas ); 
			valid = true;
		}
		else if (prob < probType[0]+probType[1]+probType[2]+probType[3]+probType[4]+probType[5]+probType[6]+probType[7])
		{
		//check if feature is valid
			sizeFactor.height = 1;
			sizeFactor.width = 3;
			if (position.y + baseDim.height*sizeFactor.height >= patchSize.height ||
				position.x + baseDim.width*sizeFactor.width >= patchSize.width)
				continue;

			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;

			if (area < minArea)
				continue;

			//m_type = 8;
			m_numAreas = 3;
			m_weights = new int[m_numAreas];
			m_weights[0] = 1;
			m_weights[1] = -2;
			m_weights[2] = 1;
			m_areas= new CvRect[m_numAreas];
			m_areas[0].x = position.x;
			m_areas[0].y = position.y;
			m_areas[0].height = baseDim.height;
			m_areas[0].width = baseDim.width;
			m_areas[1].x = position.x+baseDim.width;
			m_areas[1].y = position.y;
			m_areas[1].height = baseDim.height;
			m_areas[1].width = baseDim.width;
			m_areas[2].y = position.y;
			m_areas[2].x = position.x+2*baseDim.width;
			m_areas[2].height = baseDim.height;
			m_areas[2].width = baseDim.width;
			m_initMean = 0;
			m_initSigma = INITSIGMA( m_numAreas ); 
			valid = true;
		}
		else if (prob < probType[0]+probType[1]+probType[2]+probType[3]+probType[4]+probType[5]+probType[6]+probType[7]+probType[8])
		{
		//check if feature is valid
			sizeFactor.height = 3;
			sizeFactor.width = 3;
			if (position.y + baseDim.height*sizeFactor.height >= patchSize.height ||
				position.x + baseDim.width*sizeFactor.width >= patchSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minArea)
				continue;

			//m_type = 9;
			m_numAreas = 2;
			m_weights = new int[m_numAreas];
			m_weights[0] = 1;
			m_weights[1] = -2;
			m_areas = new CvRect[m_numAreas];
			m_areas[0].x = position.x;
			m_areas[0].y = position.y;
			m_areas[0].height = 3*baseDim.height;
			m_areas[0].width = 3*baseDim.width;
			m_areas[1].x = position.x+baseDim.width;
			m_areas[1].y = position.y+baseDim.height;
			m_areas[1].height = baseDim.height;
			m_areas[1].width = baseDim.width;
			m_initMean = 0;
			m_initSigma = INITSIGMA( m_numAreas ); 
			valid = true;
		}
		else if (prob< probType[0]+probType[1]+probType[2]+probType[3]+probType[4]+probType[5]+probType[6]+probType[7]+probType[8]+probType[9]) 
		{
			//check if feature is valid
			sizeFactor.height = 3;
			sizeFactor.width = 1;
			if (position.y + baseDim.height*sizeFactor.height >= patchSize.height ||
				position.x + baseDim.width*sizeFactor.width >= patchSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minArea)
				continue;

			//m_type = 10;
			m_numAreas = 3;
			m_weights = new int[m_numAreas];
			m_weights[0] = 1;
			m_weights[1] = -1;
			m_weights[2] = 1;
			m_areas = new CvRect[m_numAreas];
			m_areas[0].x = position.x;
			m_areas[0].y = position.y;
			m_areas[0].height = baseDim.height;
			m_areas[0].width = baseDim.width;
			m_areas[1].x = position.x;
			m_areas[1].y = position.y+baseDim.height;
			m_areas[1].height = baseDim.height;
			m_areas[1].width = baseDim.width;
			m_areas[2].y = position.y+baseDim.height*2;
			m_areas[2].x = position.x;
			m_areas[2].height = baseDim.height;
			m_areas[2].width = baseDim.width;
			m_initMean = 128;//这个又是怎么计算得到的？？
			m_initSigma = INITSIGMA( m_numAreas ); 
			valid = true;
		}
		else if (prob < probType[0]+probType[1]+probType[2]+probType[3]+probType[4]+probType[5]+probType[6]+probType[7]+probType[8]+probType[9]+probType[10])
		{
		//check if feature is valid
			sizeFactor.height = 1;
			sizeFactor.width = 3;
			if (position.y + baseDim.height*sizeFactor.height >= patchSize.height ||
				position.x + baseDim.width*sizeFactor.width >= patchSize.width)
				continue;
			area = baseDim.height*sizeFactor.height*baseDim.width*sizeFactor.width;
			if (area < minArea)
				continue;

			//m_type = 11;
			m_numAreas = 3;
			m_weights = new int[m_numAreas];
			m_weights[0] = 1;
			m_weights[1] = -1;
			m_weights[2] = 1;
			m_areas = new CvRect[m_numAreas];
			m_areas[0].x = position.x;
			m_areas[0].y = position.y;
			m_areas[0].height = baseDim.height;
			m_areas[0].width = baseDim.width;
			m_areas[1].x = position.x+baseDim.width;
			m_areas[1].y = position.y;
			m_areas[1].height = baseDim.height;
			m_areas[1].width = baseDim.width;
			m_areas[2].y = position.y;
			m_areas[2].x = position.x+2*baseDim.width;
			m_areas[2].height = baseDim.height;
			m_areas[2].width = baseDim.width;
			m_initMean = 128;
			m_initSigma = INITSIGMA( m_numAreas ); 
			valid = true;
		}
		else
			assert (false);	
	}

	m_initSize.height = patchSize.height;
	m_initSize.width = patchSize.width;
	m_curSize = m_initSize;
	m_scaleAreas = new CvRect[m_numAreas];
	m_scaleWeights = new float[m_numAreas];
	for (int curArea = 0; curArea<m_numAreas; curArea++) {
		//m_areas[curArea].x += m_offset.x;
		//m_areas[curArea].y += m_offset.y;
		m_scaleAreas[curArea] = m_areas[curArea];	
		m_scaleWeights[curArea] = (float)m_weights[curArea] /
			(float)(m_areas[curArea].width*m_areas[curArea].height);
	}
}

CFeacture::~CFeacture()
{
delete[] m_weights;
delete[] m_areas;
delete[] m_scaleAreas;
delete[] m_scaleWeights;
}