#include "WeakClassifier.h"

#define FLOAT_MAX 3.4e38f
class CSelector
{
public:
	CSelector(int numweakclassifier,CvRect patchsize,int iterationInit);
	~CSelector();
	void trainClassifier(/*IplImage* frame_grey,*/CvRect ROI,int Target,float Importance,bool* errorMask);
	void replaceWeakestClassifier(float *error,CvRect rect);
	int selectBestClassifier(bool* errorMask,float Importance,float* errors);
	CWeakClassifier** weakclassifier;
	int midxOfNewWeakClassifier;
	int mselectedClassifier;
	float* wCorrect;
	float* wWrong;
	int mweakclassifier_num;
	int miterationInit;
};


CSelector::CSelector(int num_weakclassifier,CvRect patchsize,int iterationInit)
{
mweakclassifier_num = num_weakclassifier;
miterationInit =  iterationInit;
weakclassifier = new CWeakClassifier*[num_weakclassifier + iterationInit];
midxOfNewWeakClassifier = num_weakclassifier;
wCorrect = new float[num_weakclassifier + iterationInit];
wWrong = new float[num_weakclassifier + iterationInit];
for(int i = 0;i < num_weakclassifier + iterationInit;i++)
	{
		weakclassifier[i] = new CWeakClassifier(patchsize);
		wCorrect[i] = 1;
		wWrong[i] = 1;
	}
mselectedClassifier = 0;
}

void CSelector::trainClassifier(/*IplImage* frame_grey,*/CvRect ROI,int Target,float Importance,bool* errorMask)
{
//get poisson value
	double A = 1;
	int K = 0;
	int K_max = 10;
	while (1)
	{
		double U_k = (double)rand()/RAND_MAX;
		A*=U_k;
		if (K > K_max || A<exp(-Importance))
			break;
		K++;
	}
	for (int curK = 0; curK <= K; curK++)
		for (int curWeakClassifier = 0; curWeakClassifier < mweakclassifier_num + miterationInit; curWeakClassifier++)
			errorMask[curWeakClassifier] = weakclassifier[curWeakClassifier]->update (/*frame_grey, */ROI, Target);
}


int CSelector::selectBestClassifier(bool* errorMask,float importance,float* errors)
{
	float minError = FLOAT_MAX;
	int tmp_selectedClassifier = mselectedClassifier;//最开始时初值为0

	for (int curWeakClassifier = 0; curWeakClassifier < mweakclassifier_num + miterationInit; curWeakClassifier++)
	{
		if (errorMask[curWeakClassifier])
		{
			wWrong[curWeakClassifier] +=  importance;
		}
		else
		{
			wCorrect[curWeakClassifier] += importance;
		}

		/*if (errors[curWeakClassifier]==FLOAT_MAX)
		{
			continue;
			cout<<"会终止ma"<<endl;
			cvWaitKey(0);
		}*//////////////////////////////////测试没问题则注释此段

		errors[curWeakClassifier] = wWrong[curWeakClassifier]/(wWrong[curWeakClassifier]+wCorrect[curWeakClassifier]);


		if (curWeakClassifier < mweakclassifier_num)
		{		
			if (errors[curWeakClassifier] < minError)
			{
				minError = errors[curWeakClassifier];
				tmp_selectedClassifier = curWeakClassifier;
			}
		}
	}
	mselectedClassifier = tmp_selectedClassifier;
	return mselectedClassifier;
}


void CSelector::replaceWeakestClassifier(float *errors, CvRect rect)
{
	float maxError = 0.0f;
	int index = -1;

	//search the classifier with the largest error
	for (int curWeakClassifier = 0; curWeakClassifier < mweakclassifier_num; curWeakClassifier++)
	{
		if (errors[curWeakClassifier] > maxError)
		{
			maxError = errors[curWeakClassifier];
			index = curWeakClassifier;
		}
	}

	midxOfNewWeakClassifier++;
	if (midxOfNewWeakClassifier == mweakclassifier_num + miterationInit) 
		midxOfNewWeakClassifier = mweakclassifier_num;

	if (maxError > errors[midxOfNewWeakClassifier])
	{
		delete weakclassifier[index];//如果跑着跑着内存泄漏的话，这里可能没delete
		weakclassifier[index] = weakclassifier[midxOfNewWeakClassifier];
		wWrong[index] = wWrong[midxOfNewWeakClassifier];
		wWrong[midxOfNewWeakClassifier] = 1;
		wCorrect[index] = wCorrect[midxOfNewWeakClassifier];
		wCorrect[midxOfNewWeakClassifier] = 1;
		weakclassifier[midxOfNewWeakClassifier] = new CWeakClassifier(rect);
	}

}

CSelector::~CSelector()
{
delete []weakclassifier;
delete[] wCorrect;
delete[] wWrong;
}