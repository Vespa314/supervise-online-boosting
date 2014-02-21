#include "Selector.h"
#define sign(x) (((x)>=0)? 1.0f : -1.0f)

class CStrongClassifier
{
public:
	CStrongClassifier(int selector_num,int weakclassifier_num,CvRect tracking_rect,int interationInit);
	~CStrongClassifier();
	void update(IplImage* frame_grey,CvRect ROI,float priorConfidence);
	float val(/*IplImage* grey,*/CvRect rect);
	float sum_alpha();
	CSelector **selector;
	int *mpseudoTarget;
	float *mpseudoLambda;
	bool *merrorMask;
	float *merrors;
	float *malpha;
	int mselector_num;
	int mweakclassifier_num;
	int miteration_num;
};

CStrongClassifier::CStrongClassifier(int selector_num,int weakclassifier_num,CvRect tracking_rect,int interationInit)
{
mpseudoTarget = new int[selector_num];
mpseudoLambda = new float[selector_num];
mselector_num = selector_num;
malpha = new float[selector_num];
merrors = new float[selector_num + interationInit];
merrorMask = new bool[selector_num + interationInit];
selector = new CSelector*[selector_num];

mweakclassifier_num = weakclassifier_num;
miteration_num = interationInit;
for(int i = 0;i < selector_num;i++)
	{
	malpha[i] = 0;
	selector[i] = new CSelector(weakclassifier_num,tracking_rect,interationInit);
	}
}

void CStrongClassifier::update(IplImage* frame_grey,CvRect ROI,float priorConfidence)
{
float value = 0.0f, kvalue = 0.0f;
float sumAlpha = 0;
float scaleFactor = 2.0f;
int curBaseClassifier;
for (curBaseClassifier = 0; curBaseClassifier < mselector_num; curBaseClassifier++)
	{
		for(int i = 0;i < mweakclassifier_num + miteration_num;i++)
		{
		merrors[i] = 0;
		merrorMask[i] = false;
		}
	
		int selectedClassifier;
		
     		//scale
			if (sumAlpha > 0)
				kvalue = value/sum_alpha();//sum_alpha在这里得到所有选择器的alpha和
			else
				kvalue = 0;
			//获得pseudo soft label
			float combinedDecision = tanh(scaleFactor*priorConfidence)-tanh(scaleFactor*kvalue);
			
			int myTarget = static_cast<int>(sign(combinedDecision));
			mpseudoTarget[curBaseClassifier] = myTarget;
			float myImportance = abs(combinedDecision);
			mpseudoLambda[curBaseClassifier] = myImportance;
			
			
			selector[curBaseClassifier]->trainClassifier(/*frame_grey,*/ ROI, myTarget, myImportance, merrorMask);
			selectedClassifier = selector[curBaseClassifier]->selectBestClassifier(merrorMask, myImportance, merrors);
		
			float temp = selector[curBaseClassifier]->weakclassifier[selectedClassifier]->get_eign_value(ROI);
			if(selector[curBaseClassifier]->weakclassifier[selectedClassifier]->mparity*(
				temp - selector[curBaseClassifier]->weakclassifier[selectedClassifier]->mthreshold) > 0)
			value += malpha[curBaseClassifier];
			else
			value -= malpha[curBaseClassifier];
			
sumAlpha +=malpha[curBaseClassifier];
		
			if (merrors[selectedClassifier] >= 0.5)
			malpha[curBaseClassifier] = 0;
			else
			malpha[curBaseClassifier] = logf((1.0f-merrors[selectedClassifier])/merrors[selectedClassifier]);
			
			selector[curBaseClassifier]->replaceWeakestClassifier(merrors,ROI);
		////找到错误率最大的弱分类器，替换之

	}
}

float CStrongClassifier::sum_alpha()
{
	float sum = 0;
for(int i = 0;i < mselector_num;i++)
	{
	sum += malpha[i];
	}
return sum;
}


float CStrongClassifier::val(/*IplImage *grey,*/CvRect rect)
{
float value = 0.0f;
	int curBaseClassifier=0;

	for (curBaseClassifier = 0; curBaseClassifier<mselector_num; curBaseClassifier++)
	{
	int temp = (
		selector[curBaseClassifier]->weakclassifier[selector[curBaseClassifier]->mselectedClassifier]->get_eign_value(rect)-
		selector[curBaseClassifier]->weakclassifier[selector[curBaseClassifier]->mselectedClassifier]->mthreshold
		)
		*selector[curBaseClassifier]->weakclassifier[selector[curBaseClassifier]->mselectedClassifier]->mparity>0 ? 1 : -1;

		value+=temp*malpha[curBaseClassifier];
	}
	return value;
}

CStrongClassifier::~CStrongClassifier()
{
delete []selector;
delete []mpseudoTarget;
delete []mpseudoLambda;
delete []merrorMask;
delete []merrors;
delete []malpha;
}