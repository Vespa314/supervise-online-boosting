#include "cv.h"
#include "highgui.h"

class CPatch
{
public:
	CPatch(CvRect imageROI, CvRect validROI, CvRect patchSize, float overlap);
	~CPatch();

	CvRect* patches;
	int num;
	CvRect m_patchGrid;
	CvRect m_rectUpperLeft;
	CvRect m_rectUpperRight;
	CvRect m_rectLowerLeft;
	CvRect m_rectLowerRight;
};

CPatch::~CPatch()
{
delete[] patches;
}

CPatch::CPatch(CvRect imageROI, CvRect validROI,CvRect patchSize, float overlap)
{
CvRect ROI;
//如果validROI不是整个画面，则采取下面注释段,否则用:ROI = imageROI;
//如果要有整个画面，那么在跟踪丢失时可以全画面查找
/*if ((validROI == imageROI))
		ROI = imageROI;
	else
		setCheckedROI(imageROI, validROI);*/
ROI = imageROI;	

	int stepCol = int(floor((1-overlap) * patchSize.width+0.5));
	int stepRow = int(floor((1-overlap) * patchSize.height+0.5));
	if (stepCol <= 0) stepCol = 1;
	if (stepRow <= 0) stepRow = 1;
	
	m_patchGrid.height = ((int)((float)(ROI.height-patchSize.height)/stepRow)+1);
	m_patchGrid.width = ((int)((float)(ROI.width-patchSize.width)/stepCol)+1);

	num = m_patchGrid.width * m_patchGrid.height;
	patches = new CvRect[num];
	int curPatch = 0;

	m_rectUpperLeft = m_rectUpperRight = m_rectLowerLeft = m_rectLowerRight = patchSize;//大小一样
	m_rectUpperLeft.y = ROI.y;
	m_rectUpperLeft.x = ROI.x;
	m_rectUpperRight.y = ROI.y;
	m_rectUpperRight.x = ROI.x+ROI.width-patchSize.width;
	m_rectLowerLeft.y = ROI.y+ROI.height-patchSize.height;
	m_rectLowerLeft.x = ROI.x;
	m_rectLowerRight.y = ROI.y+ROI.height-patchSize.height;
	m_rectLowerRight.x = ROI.x+ROI.width-patchSize.width;

	int numPatchesX=0; 
	int numPatchesY=0;
	for (int curRow=0; curRow< ROI.height-patchSize.height+1; curRow+=stepRow)
	{
		numPatchesY++;

		for (int curCol=0; curCol< ROI.width-patchSize.width+1; curCol+=stepCol)
		{
			if(curRow == 0)
				numPatchesX++;

			patches[curPatch].width = patchSize.width;
			patches[curPatch].height = patchSize.height;
			patches[curPatch].y = curRow+ROI.y;
			patches[curPatch].x = curCol+ROI.x;
			curPatch++;
		}
	}
} 