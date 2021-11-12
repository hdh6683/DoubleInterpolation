#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>

#define D_XMax    10
#define D_YMax    10
#define D_ZERO    0
#define D_CLEAR   0
#define D_SET     1

#define SEARCH_ALGORITHM 0		  // 0 ~ 2
#define INTERPOLATION_ALGORITHM 0 // 0 ~ 2
#define POS_CHANGE 0			  // 0 ~ 1

#define INNER_TEST 0
#define RANDOM_TEST 1
#define TEST_COUNT 10000000

typedef unsigned char Uint8;
typedef float float32;
typedef struct {
	Uint8 QAxis;
	Uint8 XSign;
	Uint8 YSign;

	Uint8 Xcnt;
	Uint8 Ycnt;

	Uint8 Xpos;
	Uint8 Ypos;

	float32 Xval;
	float32 Yval;

	float32 Xindex_Div;
	float32 Yindex_Div;

	float32 Xindex;
	float32 Xindex_X1;

	float32 Yindex;
	float32 Yindex_Y1;

	float32 Xdata;
	float32 XYdata;
	float32 XYdata_X1;
	float32 XYdata_Y1;
	float32 XYdata_X1Y1;

	float32 XYdata_Intp_X1;
	float32 XYdata_Intp_Y1;
	float32 XYdata_Intp_Double;

	float32 OutMinMax;
	float32 Out;

	float32* Table;
}sDoubleInterp;

sDoubleInterp Id, Iq;

float32 SpeedTable[D_XMax] = { 0.51, 0.61, 0.71, 0.81, 0.91, 1.01, 1.11, 1.21, 1.31, 1.41 };
float32 TorqueTable[D_YMax] = { 0, 0.37, 0.40, 0.43, 0.48, 0.54, 0.61, 0.71, 0.84, 1.0 };				//seunghun_180404

float32 IqTable[D_YMax][D_XMax] =	/* 10 X 10 */
{
	{0,			0,			0,			0,			0,			0,			0,			0,			0,			0},
	{0.3105,	0.3105,		0.3105,		0.3105,		0.3105,		0.3105,		0.3105,		0.3105,		0.3105,		0.3105},
	{0.334,		0.334,		0.334,		0.334,		0.334,		0.334,		0.334,		0.334,		0.3175,		0.3175},
	{0.35775, 	0.35775, 	0.35775, 	0.35775, 	0.35775, 	0.35775, 	0.35775, 	0.368125,	0.3175,		0.3175},
	{0.395625,	0.395625,	0.395625,	0.395625,	0.395625,	0.395625,	0.3995,		0.368125,	0.3175,		0.3175},
	{0.43975, 	0.43975, 	0.43975, 	0.43975, 	0.43975, 	0.4395,		0.3995,		0.368125,	0.3175,		0.3175},
	{0.4905,	0.4905,		0.4905,		0.4905,		0.485125,	0.4395,		0.3995,		0.368125,	0.3175,		0.3175},
	{0.55925, 	0.55925, 	0.55925, 	0.54125, 	0.485125,	0.4395,		0.3995,		0.368125,	0.3175,		0.3175},
	{0.64325, 	0.64325, 	0.612875,	0.54125, 	0.485125,	0.4395,		0.3995,		0.368125,	0.3175,		0.3175},
	{0.74225, 	0.7032125,	0.612875,	0.54125, 	0.485125,	0.4395,		0.3995,		0.368125,	0.3175,		0.3175}
};


float32 IdTable[D_YMax][D_XMax] =
{
	{0,			0,			0,			0,			0,			0,			0,			0,			0,			0},
	{0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625},
	{0.065,		0.065,		0.065,		0.065,		0.065,		0.065,		0.065,		0.065,		-0.105,		-0.105},
	{0.07125, 	0.07125,	0.07125, 	0.07125, 	0.07125, 	0.07125, 	0.07125,	-0.0875, 	-0.105,		-0.105},
	{0.0875,	0.0875,		0.0875,		0.0875,		0.0875,		0.0875,		-0.07375,	-0.0875, 	-0.105,		-0.105},
	{0.10875, 	0.10875, 	0.10875, 	0.10875, 	0.10875, 	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105},
	{0.13125, 	0.13125, 	0.13125, 	0.13125, 	-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105},
	{0.16875, 	0.16875, 	0.16875, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105},
	{0.22125, 	0.22125, 	0.01375, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105},
	{0.28125, 	0.07125, 	0.01375, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105},
};

float32 DoubleInterpolataion(sDoubleInterp* DI, float32(*Table)[3])
{
#if(INNER_TEST)
	for (int i = 0; i < TEST_COUNT; i++)
	{
#endif
		int i;				// cnt 대신 써먹을 변수
		float32 Xval, Yval; // DI->Xval, DI->Yval 대신 써먹을 변수
		Uint8 Xpos, Ypos;   // DI->Xpos, DI->Ypos 대신 써먹을 변수

		if (DI->Xval >= D_ZERO)	// X값이 0 이상이면 XSign을 0으로 clear
		{
			DI->XSign = D_CLEAR;	// +
		}
		else 
		{	// X값이 0 미만이면 XSign을 1으로 set, X값은 -DI->X값
			DI->XSign = D_SET;		// -
			DI->Xval = -DI->Xval;
		}

		if (DI->Yval >= D_ZERO) 
		{	// Y값이 0 이상이면 XSign을 0으로 clear
			DI->YSign = D_CLEAR;	// +
		}
		else 
		{	// Y값이 0 미만이면 YSign을 1으로 set, Y값은 -DI->Y값
			DI->YSign = D_SET;		// -
			DI->Yval = -DI->Yval;
		}

		if (DI->Xval > SpeedTable[D_XMax - 1])
			DI->Xval = SpeedTable[D_XMax - 1];		// X값이 스피드테이블의 최대값보다 크면 스피드테이블의 최대값으로 입력되게 함
		else if (DI->Xval <= SpeedTable[0])		
			DI->Xval = SpeedTable[0];				// X값이 스피드테이블의 최소값보다 작거나 같으면 스피드테이블의 최소값으로 입력되게 함
		
		if (DI->Yval > TorqueTable[D_YMax - 1])	
			DI->Yval = TorqueTable[D_YMax - 1];		// Y값이 스피드테이블의 최대값보다 크면 스피드테이블의 최대값으로 입력되게 함
		else if (DI->Yval <= TorqueTable[0])		
			DI->Yval = TorqueTable[0];				// Y값이 스피드테이블의 최소값보다 작거나 같으면 스피드테이블의 최소값으로 입력되게 함
		
		Xval = DI->Xval;	// DI->Xval 대신에 Xval 사용함으로써 불필요한 포인터 연산 줄임
		Yval = DI->Yval;	// DI->Yval 대신에 Yval 사용함으로써 불필요한 포인터 연산 줄임
#if(SEARCH_ALGORITHM==0)
		// 기존의 알고리즘
		for (DI->Xcnt = 0; DI->Xcnt <= (D_XMax - 1); DI->Xcnt++)	// Xcnt를 가지고 D_Max(10)번 반복
		{
			if (DI->Xval >= SpeedTable[DI->Xcnt])	// X값이 스피드테이블 안의 X값보다 큰가?
			{
				if (DI->Xcnt == (D_XMax - 1))	// Xcnt가 (10-1)과 같을 때 Xpos는 Xcnt로 결정된다.
				{
					DI->Xpos = DI->Xcnt;
				}
				else if (DI->Xval < SpeedTable[DI->Xcnt + 1])	// X값이 스피드 테이블의 Xcnt+1번째 값보다 작을 때 Xpos는 Xcnt로 결정된다.
				{
					DI->Xpos = DI->Xcnt;
				}
			}
		}

		for (DI->Ycnt = 0; DI->Ycnt <= (D_YMax - 1); DI->Ycnt++)	// Ycnt를 가지고 D_Max(10)번 반복
		{
			if (DI->Yval >= TorqueTable[DI->Ycnt])	// Y값이 스피드테이블 안의 X값보다 큰가?
			{
				if (DI->Ycnt == (D_YMax - 1))	// Ycnt가 (10-1)과 같을 때 Ypos는 Ycnt로 결정된다.
				{
					DI->Ypos = DI->Ycnt;
				}
				else if (DI->Yval < TorqueTable[DI->Ycnt + 1])	// Y값이 스피드 테이블의 Ycnt+1번째 값보다 작을 때 Ypos는 Ycnt로 결정된다.
				{
					DI->Ypos = DI->Ycnt;
				}
			}
	}
#elif(SEARCH_ALGORITHM==1)
		for (i = 0; i <= (D_XMax - 1); i++)	// Xcnt를 가지고 D_Max(10)번 반복
		{
			if (DI->Xval >= SpeedTable[i])	// X값이 스피드테이블 안의 X값보다 큰가?
			{
				if (i == (D_XMax - 1))	// Xcnt가 (10-1)과 같을 때 Xpos는 Xcnt로 결정된다.
				{
					DI->Xpos = i;
				}
				else if (DI->Xval < SpeedTable[i + 1])	// X값이 스피드 테이블의 Xcnt+1번째 값보다 작을 때 Xpos는 Xcnt로 결정된다.
				{
					DI->Xpos = i;
				}
			}
		}

		for (i = 0; i <= (D_YMax - 1); i++)	// Ycnt를 가지고 D_Max(10)번 반복
		{
			if (DI->Yval >= TorqueTable[i])	// Y값이 스피드테이블 안의 X값보다 큰가?
			{
				if (i == (D_YMax - 1))	// Ycnt가 (10-1)과 같을 때 Ypos는 Ycnt로 결정된다.
				{
					DI->Ypos = i;
				}
				else if (DI->Yval < TorqueTable[i + 1])	// Y값이 스피드 테이블의 Ycnt+1번째 값보다 작을 때 Ypos는 Ycnt로 결정된다.
				{
					DI->Ypos = i;
				}
			}
		}

#elif(SEARCH_ALGORITHM==2)
		// 개선한 알고리즘, Xcnt와 Ycnt를 지역변수 i로 바꿈
		for (i = 0; i < 5; i++)	// i를 가지고 5번 반복
		{
			if (Xval < SpeedTable[i + 1])		// X값이 스피드 테이블의 i+1번째 값보다 작을 때 Xpos는 i로 결정된다.
			{
				DI->Xpos = i;
				break;
			}
			else if (Xval >= SpeedTable[9 - i])	// X값이 스피드 테이블의 9-i번째 값 이상일 때 Xpos는 9-i로 결정된다.
			{
				DI->Xpos = 9 - i;
				break;
			}
		}

		for (i = 0; i < 5; i++)	// i를 가지고 5번 반복
		{
			if (Yval < TorqueTable[i + 1])		// Y값이 토크 테이블의 i+1번째 값보다 작을 때 Ypos는 i로 결정된다.
			{
				DI->Ypos = i;
				break;
			}
			else if (Yval >= TorqueTable[9 - i])	// Y값이 토크 테이블의 9-i번째 값 이상일 때 Ypos는 9-i로 결정된다.
			{
				DI->Ypos = 9 - i;
				break;
			}
		}

#endif
		// 여기까지 Xpos와 Ypos를 결정하였다.

#if(POS_CHANGE)
		// Xpos와 Ypos를 대입하여 사용함
		Xpos = DI->Xpos;
		Ypos = DI->Ypos;
		DI->Xindex = SpeedTable[Xpos];			//	Xindex는 스피드테이블의 Xpos번째 값
		DI->Xindex_X1 = SpeedTable[Xpos + 1];	//	Xindex_X1은 스피드테이블의 (Xpos+1)번째 값

		DI->Yindex = TorqueTable[Ypos];			//	Yindex는 스피드테이블의 Ypos번째 값
		DI->Yindex_Y1 = TorqueTable[Ypos + 1];	//	Yindex_Y1은 스피드테이블의 (Ypos+1)번째 값

		//xdata = Table[D_DATAIndex][Xpos];
		DI->XYdata = Table[Ypos][Xpos];			//	XYdata는 테이블의 (Xpos,Ypos) 값

		DI->XYdata_X1 = Table[Ypos][Xpos + 1];	//	XYdata_X1은 테이블의 (Xpos+1,Ypos) 값
		DI->XYdata_Y1 = Table[Ypos + 1][Xpos];	//	XYdata_Y1은 테이블의 (Xpos,Ypos+1) 값

		DI->Xindex_Div = 1. / (DI->Xindex_X1 - DI->Xindex);	//	Xindex_Div = 1 / (Xindex_X1 - Xindex)
		DI->Yindex_Div = 1. / (DI->Yindex_Y1 - DI->Yindex);	//	Xindex_Div = 1 / (Yindex_Y1 - Yindex)
#else
		DI->Xindex = SpeedTable[DI->Xpos];			//	Xindex는 스피드테이블의 Xpos번째 값
		DI->Xindex_X1 = SpeedTable[DI->Xpos + 1];	//	Xindex_X1은 스피드테이블의 (Xpos+1)번째 값

		DI->Yindex = TorqueTable[DI->Ypos];			//	Yindex는 스피드테이블의 Ypos번째 값
		DI->Yindex_Y1 = TorqueTable[DI->Ypos + 1];	//	Yindex_Y1은 스피드테이블의 (Ypos+1)번째 값

		//xdata = Table[D_DATAIndex][Xpos];
		DI->XYdata = Table[DI->Ypos][DI->Xpos];			//	XYdata는 테이블의 (Xpos,Ypos) 값

		DI->XYdata_X1 = Table[DI->Ypos][DI->Xpos + 1];	//	XYdata_X1은 테이블의 (Xpos+1,Ypos) 값
		DI->XYdata_Y1 = Table[DI->Ypos + 1][DI->Xpos];	//	XYdata_Y1은 테이블의 (Xpos,Ypos+1) 값

		DI->Xindex_Div = 1. / (DI->Xindex_X1 - DI->Xindex);	//	Xindex_Div = 1 / (Xindex_X1 - Xindex)
		DI->Yindex_Div = 1. / (DI->Yindex_Y1 - DI->Yindex);	//	Xindex_Div = 1 / (Yindex_Y1 - Yindex)
#endif

#if(INTERPOLATION_ALGORITHM==0)

		//(DI->Xval == 0) 
		//즉 Xval가 0일 때
		if (DI->Xval == SpeedTable[0])		// Xval가 SpeedTable의 0번째 값이랑 같으면
		{
			if (DI->Yval == TorqueTable[0])	// Yval가 TorqueTable[0]랑 같을 때
			{
				DI->XYdata_Intp_Double = DI->XYdata; // XYdata_Intp_Double은 그냥 XYdata이다.

			}
			else if (DI->Yval > TorqueTable[0] && DI->Yval < TorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
			{	// XYdata_Intp_Double은 (((Yval-Yindex)*Yindex_Div)*(XYdata_Y1-XYdata))+XYdata
				DI->XYdata_Intp_Double = (((DI->Yval - DI->Yindex) * DI->Yindex_Div) * (DI->XYdata_Y1 - DI->XYdata)) + DI->XYdata;
			}
			else if (DI->Yval == TorqueTable[D_YMax - 1])	// Yval가 TorqueTable[9]랑 같을 때
			{
				DI->XYdata_Intp_Double = DI->XYdata; // XYdata_Intp_Double은 그냥 XYdata이다.
			}
		}

		//(DI->Xval > 0) && (DI->Xval < SpeedTable[DI->Xpos]) 
		//즉 0 < Xval < SpeedTable[Xpos]
		if ((DI->Xval > SpeedTable[0]) && (DI->Xval < SpeedTable[D_XMax - 1]))	// SpeedTable[0] < Xval < SpeedTable[9]
		{
			if (DI->Yval == TorqueTable[0])	// Yval가 TorqueTable[0]랑 같을 때
			{
				DI->XYdata_Intp_Double = (((DI->Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
				// XYdata_Intp_Double = (((Xval - Xindex) * Xindex_Div) * (XYdata_X1 - XYdata)) + XYdata;
			}
			else if (DI->Yval > TorqueTable[0] && DI->Yval < TorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
			{
				DI->XYdata_X1Y1 = Table[DI->Ypos + 1][DI->Xpos + 1];	//	XYdata_X1Y1=Table[Ypos + 1][Xpos + 1]

				DI->XYdata_Intp_X1 = (((DI->Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
				// XYdata_Intp_X1 = (((Xval - Xindex) * Xindex_Div) * (XYdata_X1 - XYdata)) + XYdata;
				DI->XYdata_Intp_Y1 = (((DI->Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1Y1 - DI->XYdata_Y1)) + DI->XYdata_Y1;
				// XYdata_Intp_Y1 = (((Xval - Xindex) * Xindex_Div) * (XYdata_X1Y1 - XYdata_Y1)) + XYdata_Y1;
				DI->XYdata_Intp_Double = (((DI->Yval - DI->Yindex) * DI->Yindex_Div) * (DI->XYdata_Intp_Y1 - DI->XYdata_Intp_X1)) + DI->XYdata_Intp_X1;
				// XYdata_Intp_Double = (((Yval - Yindex) * Yindex_Div) * (XYdata_Intp_Y1 - XYdata_Intp_X1)) + XYdata_Intp_X1;
			}
			else if (DI->Yval == TorqueTable[D_YMax - 1]) // Yval가 TorqueTable[9]랑 같을 때
			{
				DI->XYdata_Intp_Double = (((DI->Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
				// XYdata_Intp_Double = (((Xval - Xindex) * Xindex_Div) * (XYdata_X1 - XYdata)) + XYdata;
			}
				}


		//(DI->Xval == SpeedTable[DI->Xpos])
		//즉 Xval == SpeedTable[Xpos]
		if (DI->Xval == SpeedTable[D_XMax - 1])	// Xval == SpeedTable[9]
		{
			if (DI->Yval == TorqueTable[0])	// Yval가 TorqueTable[0]랑 같을 때
			{
				DI->XYdata_Intp_Double = DI->XYdata;	//	XYdata_Intp_Double = XYdata;
			}
			else if (DI->Yval > TorqueTable[0] && DI->Yval < TorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
			{
				DI->XYdata_Intp_Double = (((DI->Yval - DI->Yindex) * DI->Yindex_Div) * (DI->XYdata_Y1 - DI->XYdata)) + DI->XYdata;
				//XYdata_Intp_Double = (((Yval - Yindex) * Yindex_Div) * (XYdata_Y1 - XYdata)) + XYdata;
			}
			else if (DI->Yval == TorqueTable[D_YMax - 1])	// Yval가 TorqueTable[9]랑 같을 때
			{
				DI->XYdata_Intp_Double = DI->XYdata;	//	XYdata_Intp_Double = XYdata;
			}
		}

#elif(INTERPOLATION_ALGORITHM==1)

		//(DI->Xval > 0) && (DI->Xval < SpeedTable[DI->Xpos]) 
		//즉 0 < Xval < SpeedTable[Xpos]
		if ((Xval > SpeedTable[0]) && (Xval < SpeedTable[D_XMax - 1]))	// SpeedTable[0] < Xval < SpeedTable[9]
		{
			if (Yval == TorqueTable[0])	// Yval가 TorqueTable[0]랑 같을 때
			{
				DI->XYdata_Intp_Double = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
			}
			else if (Yval > TorqueTable[0] && Yval < TorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
			{
				DI->XYdata_X1Y1 = Table[DI->Ypos + 1][DI->Xpos + 1];	//	XYdata_X1Y1=Table[Ypos + 1][Xpos + 1]

				DI->XYdata_Intp_X1 = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
				DI->XYdata_Intp_Y1 = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1Y1 - DI->XYdata_Y1)) + DI->XYdata_Y1;
				DI->XYdata_Intp_Double = (((Yval - DI->Yindex) * DI->Yindex_Div) * (DI->XYdata_Intp_Y1 - DI->XYdata_Intp_X1)) + DI->XYdata_Intp_X1;
			}
			else if (Yval == TorqueTable[D_YMax - 1]) // Yval가 TorqueTable[9]랑 같을 때
			{
				DI->XYdata_Intp_Double = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
			}
		}
		else // Xval == SpeedTable[9] 또는 Xval == SpeedTable[0]
		{
			if (Yval == TorqueTable[0])	// Yval가 TorqueTable[0]랑 같을 때
			{
				DI->XYdata_Intp_Double = DI->XYdata;	//	XYdata_Intp_Double = XYdata;
			}
			else if (Yval > TorqueTable[0] && Yval < TorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
			{
				DI->XYdata_Intp_Double = (((Yval - DI->Yindex) * DI->Yindex_Div) * (DI->XYdata_Y1 - DI->XYdata)) + DI->XYdata;
			}
			else if (Yval == TorqueTable[D_YMax - 1])	// Yval가 TorqueTable[9]랑 같을 때
			{
				DI->XYdata_Intp_Double = DI->XYdata;	//	XYdata_Intp_Double = XYdata;
			}
		}
#elif(INTERPOLATION_ALGORITHM==2)

		//(DI->Xval > 0) && (DI->Xval < SpeedTable[DI->Xpos]) 
		//즉 0 < Xval < SpeedTable[Xpos]
		if ((Xval > SpeedTable[0]) && (Xval < SpeedTable[D_XMax - 1]))	// SpeedTable[0] < Xval < SpeedTable[9]
		{
			if (Yval > TorqueTable[0] && Yval < TorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
			{
				DI->XYdata_X1Y1 = Table[DI->Ypos + 1][DI->Xpos + 1];	//	XYdata_X1Y1=Table[Ypos + 1][Xpos + 1]

				DI->XYdata_Intp_X1 = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
				DI->XYdata_Intp_Y1 = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1Y1 - DI->XYdata_Y1)) + DI->XYdata_Y1;
				DI->XYdata_Intp_Double = (((Yval - DI->Yindex) * DI->Yindex_Div) * (DI->XYdata_Intp_Y1 - DI->XYdata_Intp_X1)) + DI->XYdata_Intp_X1;
			}
			else // Yval가 TorqueTable[9] 또는 orqueTable[0]랑 같을 때
			{
				DI->XYdata_Intp_Double = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
			}
		}
		else // Xval == SpeedTable[9] 또는 Xval == SpeedTable[0]
		{
			if (Yval > TorqueTable[0] && Yval < TorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
			{
				DI->XYdata_Intp_Double = (((Yval - DI->Yindex) * DI->Yindex_Div) * (DI->XYdata_Y1 - DI->XYdata)) + DI->XYdata;
			}
			else // Yval가 TorqueTable[9]와 TorqueTable[0]랑 같을 때
			{
				DI->XYdata_Intp_Double = DI->XYdata;	//	XYdata_Intp_Double = XYdata;
			}
		}
#endif
		if (DI->QAxis == D_SET)	// QAxis가 1이면
		{
			if (DI->YSign == D_SET)	// QAxis가 1이고 YSign이 1이면 XYdata_Intp_Double은 부호가 바뀐다.
				DI->XYdata_Intp_Double = -DI->XYdata_Intp_Double;
		}

#if(INNER_TEST)
	}
#endif

	return DI->XYdata_Intp_Double;	// XYdata_Intp_Double 리턴
}

int main()
{
	unsigned int start_time, end_time;
#if(RANDOM_TEST)
	start_time = GetTickCount64();
	for (int i = 0; i < TEST_COUNT; i++)
	{
		srand((unsigned)time(NULL));
		Id.Xval = (float)(rand() % 20) / 10;
		Id.Yval = (float)(rand() % 10) / 10;
		Id.QAxis = D_CLEAR; // QAxis는 0이다
#else
	Id.Xval = 1.01;		// X값 설정
	Id.Yval = 0.43;		// Y값 설정
	Id.QAxis = D_CLEAR; // QAxis는 0이다
#endif
#if(INNER_TEST)
	start_time = GetTickCount64();
#endif
	Id.Out = DoubleInterpolataion(&Id, IdTable);
#if(INNER_TEST)
	end_time = GetTickCount64();

#endif

#if(RANDOM_TEST)
	}
	end_time = GetTickCount64();
#endif

#if(INNER_TEST||RANDOM_TEST)
	printf("경과 시간 : %dms", end_time - start_time);
#else
	printf("Id.Out : %f\n", Id.Out);

#endif
	return 0;
}
