#include "Feacture.h"


class CWeakClassifier
{
public:
	CWeakClassifier(CvRect patchsize);
	~CWeakClassifier();

	bool update(/*IplImage* frame_grey,*/CvRect ROI,int Target);
	float get_eign_value(CvRect);
	CFeacture *feacture;
	double mthreshold;
	int mparity;


	float pos_mean;
	float pos_sigma;
	float pos_P_mean;
	float pos_P_sigma;
	float pos_R_mean;
	float pos_R_sigma;

	float neg_mean;
	float neg_sigma;
	float neg_P_mean;
	float neg_P_sigma;
	float neg_R_mean;
	float neg_R_sigma;
};

CWeakClassifier::CWeakClassifier(CvRect patchsize)
{
feacture = new CFeacture(patchsize);

pos_mean = feacture->m_initMean;
pos_sigma = feacture->m_initSigma;
pos_P_mean = 1000;
pos_R_mean = 0.01f;
pos_P_sigma = 1000;
pos_R_sigma = 0.01f;

neg_mean = feacture->m_initMean;
neg_sigma = feacture->m_initSigma;
neg_P_mean = 1000;
neg_R_mean = 0.01f;
neg_P_sigma = 1000;
neg_R_sigma = 0.01f;

mthreshold = 0;
mparity = 0;
}

float CWeakClassifier::get_eign_value(CvRect ROI)
{
float egin_value = 0;
for(int i = 0;i < feacture->m_numAreas;i++)//¼ÆËãhaarÌØÕ÷Öµ
	{
	float temp;
	temp = inter_List[feacture->m_scaleAreas[i].y+ROI.y-inter_searchRegion.y][feacture->m_scaleAreas[i].x+ROI.x-inter_searchRegion.x]
	+ inter_List[feacture->m_scaleAreas[i].y + feacture->m_scaleAreas[i].height+ROI.y-inter_searchRegion.y][feacture->m_scaleAreas[i].x + feacture->m_scaleAreas[i].width+ROI.x-inter_searchRegion.x]
	- inter_List[feacture->m_scaleAreas[i].y + feacture->m_scaleAreas[i].height+ROI.y-inter_searchRegion.y][feacture->m_scaleAreas[i].x +ROI.x-inter_searchRegion.x]
	- inter_List[feacture->m_scaleAreas[i].y+ROI.y-inter_searchRegion.y][feacture->m_scaleAreas[i].x + feacture->m_scaleAreas[i].width+ROI.x-inter_searchRegion.x];
	egin_value += temp * feacture->m_scaleWeights[i];
	}
return egin_value;
}

bool CWeakClassifier::update(/*IplImage* frame_grey,*/CvRect ROI,int Target)
{
	float egin_value = get_eign_value(ROI);
	
	if(Target == 1)//kalman filter
	{
	float K;
	float minFactor = 0.001f; 
	K = pos_P_mean/(pos_P_mean+pos_R_mean);
	if (K < minFactor) 
		K = minFactor;
	pos_mean = K*egin_value + (1.0f-K)*pos_mean;
	pos_P_mean = pos_P_mean*pos_R_mean/(pos_P_mean+pos_R_mean);
	K = pos_P_sigma/(pos_P_sigma+pos_R_sigma);
	if (K < minFactor) 
		K = minFactor;
	float tmp_sigma = K*(pos_mean-egin_value)*(pos_mean-egin_value) + (1.0f-K)*pos_sigma*pos_sigma;
	pos_P_sigma = pos_P_sigma*pos_R_mean/(pos_P_sigma+pos_R_sigma);
	pos_sigma = static_cast<float>( sqrt(tmp_sigma) );
	if (pos_sigma <= 1.0f) 
		pos_sigma = 1.0f;
	}
	else
	{
	float K;
	float minFactor = 0.001f; 
	K = neg_P_mean/(neg_P_mean+neg_R_mean);
	if (K < minFactor) 
		K = minFactor;
	neg_mean = K*egin_value + (1.0f-K)*neg_mean;
	neg_P_mean = neg_P_mean*neg_R_mean/(neg_P_mean+neg_R_mean);
	K = neg_P_sigma/(neg_P_sigma+neg_R_sigma);
	if (K < minFactor) 
		K = minFactor;
	float tmp_sigma = K*(neg_mean-egin_value)*(neg_mean-egin_value) + (1.0f-K)*neg_sigma*neg_sigma;
	neg_P_sigma = neg_P_sigma*neg_R_mean/(neg_P_sigma+neg_R_sigma);

	neg_sigma = static_cast<float>( sqrt(tmp_sigma) );
	if (neg_sigma <= 1.0f) neg_sigma = 1.0f;
	}
	mthreshold = (pos_mean + neg_mean)/2.0f;
	mparity = (pos_mean > neg_mean) ? 1 : -1;
return (((mparity*(egin_value-mthreshold))>0 ? 1 : -1) != Target);
}

CWeakClassifier::~CWeakClassifier()
{
delete feacture;
}


void built_iterlist(IplImage* grey,CvRect ROI)
{
	int heigth = ROI.height;
	int width = ROI.width;
	if(inter_List != NULL)
	delete []inter_List;

	inter_List = new float*[heigth];
	for(int i = 0;i < heigth;i++)
	{
		inter_List[i] = new float[width];
		for(int j = 0;j < width;j++)
		{
			float temp = CV_IMAGE_ELEM(grey,uchar,i + ROI.y,j + ROI.x);
		if(i == 0 && j == 0)
			inter_List[i][j] = temp;
		else if(i == 0)
			inter_List[i][j] = inter_List[i][j - 1] + temp;
		else if(j == 0)
			inter_List[i][j] = inter_List[i - 1][j] + temp;
		else
		inter_List[i][j] = inter_List[i - 1][j] + inter_List[i][j - 1] - inter_List[i - 1][j - 1] + temp;
		}
	}

}
