#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define D_XMax    10
#define D_YMax    10
#define D_ZERO    0
#define D_CLEAR   0
#define D_SET     1

#define INTEGER_OPTIMIZE		0 // 정수 최적화 0,1
#define SEARCH_ALGORITHM		3 // 0 ~ 4	0:original	1:for문 최적화	2:for문+루프변수	3:이진탐색	4:이진트리 최적화 5:다운카운팅 루프(가장최적)
#define INTERPOLATION_ALGORITHM 2 // 0 ~ 3	0:original	1:조건문 간소화	2:조건문 최종 간소화(가장 최적)	3:조건에 따라 필요한 계산만 함
#define TABLE_CHANGE			0 // 0 ~ 3  0:original	1:연결리스트 사용	2:logic_table 함수	3:11x11배열 사용

#define PRINT_TEST				0
#define INNER_TEST				0
#define RANDOM_TEST				0
#define TEST_COUNT				/*32*/184320	// INNER_TEST 시행하였을 때 원래 알고리즘이 20ms가 나오는 반복횟수
#define INTERPOLATION_TIME		0 // TEST_COUNT 사용
#define SEARCH_TIME				0 // TEST_COUNT 사용

inline int compare(int middle, float searchnum, float* table);
//int binsearch(float searchnum, float* table);
clock_t start_time, end_time;

typedef unsigned char Uint8;
typedef char int8;
typedef float float32;
typedef int int32;
typedef unsigned int Uint32;

typedef struct node {
	int data;
	struct node* left;
	struct node* right;
}node;

node* treePointer;

node* insert(node* root, int data)
{
	if (root == NULL)
	{
		root = (node*)malloc(sizeof(node));
		root->right = root->left = NULL;
		root->data = data;
		return root;
	}
	else
	{
		if (data < root->data)
			root->left = insert(root->left, data);
		else
			root->right = insert(root->right, data);
	} return root;
}

// 배열을 연결 리스트로 구현하기 위함
typedef struct id_node {
	float32 data;
	Uint8 x_max;
	//Uint8 y_max;
	struct id_node* next;
}id_node;

id_node* idPointerTable[D_YMax+1];

void add_node(id_node** head_node, float32 data, Uint8 x_max) //리스트의 마지막 노드에 삽입
{
	id_node* new_node;
	id_node* last_node;
	new_node = (id_node*)malloc(sizeof(id_node));
	new_node->data = data;
	new_node->x_max = x_max;
	//new_node->y_max = y_max;
	new_node->next = NULL;

	if (*head_node == NULL) {        //현재 리스트가 공백인 경우
		*head_node = new_node;
		return;
	}

	last_node = *head_node;
	while (last_node->next != NULL) last_node = last_node->next;
	last_node->next = new_node;
}



void init_id_node(void)
{
#if 1 // add_node x축 방향으로만 추가한 경우
	add_node(&idPointerTable[0], 0, 10);

	add_node(&idPointerTable[1], 0.05625, 10);

	add_node(&idPointerTable[2], 0.065, 7);
	add_node(&idPointerTable[2], -0.105, 10);

	add_node(&idPointerTable[3], 0.07125, 6);
	add_node(&idPointerTable[3], -0.0875, 7);
	add_node(&idPointerTable[3], -0.105, 10);

	add_node(&idPointerTable[4], 0.0875, 5);
	add_node(&idPointerTable[4], -0.07375, 6);
	add_node(&idPointerTable[4], -0.0875, 7);
	add_node(&idPointerTable[4], -0.105, 10);

	add_node(&idPointerTable[5], 0.10875, 4);
	add_node(&idPointerTable[5], -0.06625, 5);
	add_node(&idPointerTable[5], -0.07375, 6);
	add_node(&idPointerTable[5], -0.0875, 7);
	add_node(&idPointerTable[5], -0.105, 10);

	add_node(&idPointerTable[6], 0.13125, 3);
	add_node(&idPointerTable[6], -0.04625, 4);
	add_node(&idPointerTable[6], -0.06625, 5);
	add_node(&idPointerTable[6], -0.07375, 6);
	add_node(&idPointerTable[6], -0.0875, 7);
	add_node(&idPointerTable[6], -0.105, 10);

	add_node(&idPointerTable[7], 0.16875, 2);
	add_node(&idPointerTable[7], -0.02, 3);
	add_node(&idPointerTable[7], -0.04625, 4);
	add_node(&idPointerTable[7], -0.06625, 5);
	add_node(&idPointerTable[7], -0.07375, 6);
	add_node(&idPointerTable[7], -0.0875, 7);
	add_node(&idPointerTable[7], -0.105, 10);

	add_node(&idPointerTable[8], 0.22125, 1);
	add_node(&idPointerTable[8], 0.01375, 2);
	add_node(&idPointerTable[8], -0.02, 3);
	add_node(&idPointerTable[8], -0.04625, 4);
	add_node(&idPointerTable[8], -0.06625, 5);
	add_node(&idPointerTable[8], -0.07375, 6);
	add_node(&idPointerTable[8], -0.0875, 7);
	add_node(&idPointerTable[8], -0.105, 10);

	add_node(&idPointerTable[9], 0.28125, 0);
	add_node(&idPointerTable[9], 0.07125, 1);
	add_node(&idPointerTable[9], 0.01375, 2);
	add_node(&idPointerTable[9], -0.02, 3);
	add_node(&idPointerTable[9], -0.04625, 4);
	add_node(&idPointerTable[9], -0.06625, 5);
	add_node(&idPointerTable[9], -0.07375, 6);
	add_node(&idPointerTable[9], -0.0875, 7);
	add_node(&idPointerTable[9], -0.105, 10);

	add_node(&idPointerTable[10], 0.28125, 0);
	add_node(&idPointerTable[10], 0.07125, 1);
	add_node(&idPointerTable[10], 0.01375, 2);
	add_node(&idPointerTable[10], -0.02, 3);
	add_node(&idPointerTable[10], -0.04625, 4);
	add_node(&idPointerTable[10], -0.06625, 5);
	add_node(&idPointerTable[10], -0.07375, 6);
	add_node(&idPointerTable[10], -0.0875, 7);
	add_node(&idPointerTable[10], -0.105, 10);
#else // add_node x축과 y축 방향으로 추가한 경우
	add_node(idPointerTable[0], 0, 9, 0);

	add_node(idPointerTable[1], 0.5625, 9, 1);

	add_node(idPointerTable[2], 0.065, 7, 2);
	add_node(idPointerTable[2], -0.105, 9,9);

	add_node(idPointerTable[3], 0.07125, 6, 3);
	add_node(idPointerTable[3], -0.0875, 7, 9);

	add_node(idPointerTable[4], 0.0875, 5, 4);
	add_node(idPointerTable[4], -0.07375, 6, 9);

	add_node(idPointerTable[5], 0.10875, 4, 5);
	add_node(idPointerTable[5], -0.06625, 5,9);

	add_node(idPointerTable[6], 0.13125, 3, 6);
	add_node(idPointerTable[6], -0.04625, 4, 9);

	add_node(idPointerTable[7], 0.16875, 2, 7);
	add_node(idPointerTable[7], -0.02, 3, 9);

	add_node(idPointerTable[8], 0.22125, 1, 8);
	add_node(idPointerTable[8], 0.01375, 2,9);

	add_node(idPointerTable[9], 0.28125, 0, 9);
	add_node(idPointerTable[9], 0.07125, 1, 9);
#endif
}


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

float32 SpeedTable[D_XMax] = { 0.51, 0.61, 0.71, 0.81, 0.91, 1.01, 1.11, 1.21, 1.31, 1.41};	// 기존의 인덱스 범위 초과하는 오류 발생.
float32 TorqueTable[D_YMax] = { 0, 0.37, 0.40, 0.43, 0.48, 0.54, 0.61, 0.71, 0.84, 1.0};		// 기존의 인덱스 범위 초과하는 오류 발생.		
//seunghun_180404

//	10만(10⁵)배 정수화 테이블
int32 IntSpeedTable[D_XMax] = { 51000, 61000, 71000, 81000, 91000, 101000, 111000, 121000, 131000, 141000 };
int32 IntTorqueTable[D_YMax] = {	0, 37000, 40000, 43000, 48000, 54000, 61000, 71000, 84000, 100000 };

float32 RandomTable[32] = { 0,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1,1.05,1.1,1.15,1.2,1.25,1.3,1.35,1.4,1.45,1.5,1.55 };

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
	{0.28125, 	0.07125, 	0.01375, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105}
};

float32 SpeedTable_plus[D_XMax + 1] = { 0.51, 0.61, 0.71, 0.81, 0.91, 1.01, 1.11, 1.21, 1.31, 1.41, 1.41 };	// 기존의 인덱스 범위 초과하는 오류 발생.
float32 TorqueTable_plus[D_YMax + 1] = { 0, 0.37, 0.40, 0.43, 0.48, 0.54, 0.61, 0.71, 0.84, 1.0, 1.0 };		// 기존의 인덱스 범위 초과하는 오류 발생.		

float32 IdTable_plus[D_YMax + 1][D_XMax + 1] =	// 최승훈 책임님이랑 얘기한 사항 : 11x11 배열로 만들어야 오류가 없다 -> 11x11 배열로 수정함.
{
	{0,			0,			0,			0,			0,			0,			0,			0,			0,			0,			0},
	{0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625, 	0.05625,	0.5625},
	{0.065,		0.065,		0.065,		0.065,		0.065,		0.065,		0.065,		0.065,		-0.105,		-0.105,		-0.105},
	{0.07125, 	0.07125,	0.07125, 	0.07125, 	0.07125, 	0.07125, 	0.07125,	-0.0875, 	-0.105,		-0.105,		-0.105},
	{0.0875,	0.0875,		0.0875,		0.0875,		0.0875,		0.0875,		-0.07375,	-0.0875, 	-0.105,		-0.105,		-0.105},
	{0.10875, 	0.10875, 	0.10875, 	0.10875, 	0.10875, 	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105,		-0.105},
	{0.13125, 	0.13125, 	0.13125, 	0.13125, 	-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105,		-0.105},
	{0.16875, 	0.16875, 	0.16875, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105,		-0.105},
	{0.22125, 	0.22125, 	0.01375, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105,		-0.105},
	{0.28125, 	0.07125, 	0.01375, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105,		-0.105},
	{0.28125, 	0.07125, 	0.01375, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105,		-0.105}
};

float32 ReducedIdTable[D_YMax+1][D_XMax] =
{
	{0},
	{0.05625},
	{0.065,		-0.105,		-0.105},
	{0.07125, 	-0.0875,	-0.105,		-0.105},
	{0.0875,	-0.07375,	-0.0875,	-0.105,		-0.105},
	{0.10875, 	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105},
	{0.13125, 	-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105},
	{0.16875, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105},
	{0.22125, 	0.01375, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105},
	{0.28125, 	0.07125, 	0.01375, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105},
	{0.28125, 	0.07125, 	0.01375, 	-0.02,		-0.04625,	-0.06625,	-0.07375,	-0.0875, 	-0.105,		-0.105},
};

inline float32 logic_table(int8 x_pos, int8 y_pos)
{
	if (y_pos >= 2)
	{
		if (x_pos > (9 - y_pos))
		{
			return ReducedIdTable[y_pos][x_pos + y_pos - 9];
		}
	}
	else 
	{
		return ReducedIdTable[y_pos][0];
	}
}

int32 IntIdTable[D_YMax][D_XMax] =	//	10만(10⁵)배 정수화 테이블
{
	{0,     0,      0,      0,      0,      0,      0,      0,      0,      0},
	{5625,  5625,   5625,   5625,   5625,   5625,   5625,   5625,   5625,   5625},	//5625
	{6500,  6500,   6500,   6500,   6500,   6500,   6500,   6500,   -10500, -10500},
	{7125,  7125,   7125,   7125,   7125,   7125,   7125,   -8750,  -10500, -10500},
	{8750,  8750,   8750,   8750,   8750,   8750,   -7374,  -8750,  -10500, -10500},
	{10875, 10875,  10875,  10875,  10875,  -6624,  -7374,  -8750,  -10500, -10500},
	{13124, 13124,  13124,  13124,  -4625,  -6624,  -7374,  -8750,  -10500, -10500},
	{16875, 16875,  16875,  -2000,  -4625,  -6624,  -7374,  -8750,  -10500, -10500},
	{22125, 22125,  1375,   -2000,  -4625,  -6624,  -7374,  -8750,  -10500, -10500},
	{28125, 7125,   1375,   -2000,  -4625,  -6624,  -7374,  -8750,  -10500, -10500},
};

float32 IntDoubleInterpolataion(sDoubleInterp* DI, int32(*Table)[3])
{
	int32 Xindex, Xindex_X1, Yindex, Yindex_Y1, XYdata, XYdata_X1, XYdata_Y1, XYdata_X1Y1, XYdata_Intp_X1, XYdata_Intp_Y1, XYdata_Intp_Double;
	int32 Xval, Yval;
	Uint8 Xpos, Ypos;						// DI->Xpos, DI->Ypos 대신 써먹을 변수
#if(INNER_TEST)
	start_time = clock();

	for (int i = 0; i < TEST_COUNT; i++)
	{
#endif
		register unsigned int i;				// cnt 대신 써먹을 변수


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
		{
			DI->Xval = SpeedTable[D_XMax - 1];		// X값이 스피드테이블의 최대값보다 크면 스피드테이블의 최대값으로 입력되게 함
		}
		else if (DI->Xval <= SpeedTable[0])
		{
			DI->Xval = SpeedTable[0];				// X값이 스피드테이블의 최소값보다 작거나 같으면 스피드테이블의 최소값으로 입력되게 함
		}

		if (DI->Yval > TorqueTable[D_YMax - 1])
		{
			DI->Yval = TorqueTable[D_YMax - 1];		// Y값이 스피드테이블의 최대값보다 크면 스피드테이블의 최대값으로 입력되게 함
		}
		else if (DI->Yval <= TorqueTable[0])
		{
			DI->Yval = TorqueTable[0];				// Y값이 스피드테이블의 최소값보다 작거나 같으면 스피드테이블의 최소값으로 입력되게 함
		}

		Xval = (DI->Xval) * 100000;	// 100000배한 정수를 사용
		Yval = (DI->Yval) * 100000;	// 100000배한 정수를 사용


		// for문 개선 + Xcnt와 Ycnt를 지역변수 i로 바꿈
		for (i = 0; i < 5; i++)	// i를 가지고 5번 반복
		{
			if (Xval < IntSpeedTable[i + 1])		// X값이 스피드 테이블의 i+1번째 값보다 작을 때 Xpos는 i로 결정된다.
			{
				DI->Xpos = i;
				break;
			}
			else if (Xval >= IntSpeedTable[9 - i])	// X값이 스피드 테이블의 9-i번째 값 이상일 때 Xpos는 9-i로 결정된다.
			{
				DI->Xpos = 9 - i;
				break;
			}
		}

		for (i = 0; i < 5; i++)	// i를 가지고 5번 반복
		{
			if (Yval < IntTorqueTable[i + 1])		// Y값이 토크 테이블의 i+1번째 값보다 작을 때 Ypos는 i로 결정된다.
			{
				DI->Ypos = i;
				break;
			}
			else if (Yval >= IntTorqueTable[9 - i])	// Y값이 토크 테이블의 9-i번째 값 이상일 때 Ypos는 9-i로 결정된다.
			{
				DI->Ypos = 9 - i;
				break;
			}
		}
		// 여기까지 Xpos와 Ypos를 결정하였다.


		Xindex = IntSpeedTable[DI->Xpos];			//	Xindex는 스피드테이블의 Xpos번째 값
		Xindex_X1 = IntSpeedTable[DI->Xpos + 1];	//	Xindex_X1은 스피드테이블의 (Xpos+1)번째 값

		Yindex = IntTorqueTable[DI->Ypos];			//	Yindex는 스피드테이블의 Ypos번째 값
		Yindex_Y1 = IntTorqueTable[DI->Ypos + 1];	//	Yindex_Y1은 스피드테이블의 (Ypos+1)번째 값

		//xdata = Table[D_DATAIndex][Xpos];
		XYdata = Table[DI->Ypos][DI->Xpos];			//	XYdata는 테이블의 (Xpos,Ypos) 값

		XYdata_X1 = Table[DI->Ypos][DI->Xpos + 1];	//	XYdata_X1은 테이블의 (Xpos+1,Ypos) 값
		XYdata_Y1 = Table[DI->Ypos + 1][DI->Xpos];	//	XYdata_Y1은 테이블의 (Xpos,Ypos+1) 값

		DI->Xindex_Div = 1. / (Xindex_X1 - Xindex);	//	Xindex_Div = 1 / (Xindex_X1 - Xindex)
		DI->Yindex_Div = 1. / (Yindex_Y1 - Yindex);	//	Xindex_Div = 1 / (Yindex_Y1 - Yindex)


		//(DI->Xval > 0) && (DI->Xval < SpeedTable[DI->Xpos]) 
		//즉 0 < Xval < SpeedTable[Xpos]
		if ((Xval > IntSpeedTable[0]) && (Xval < IntSpeedTable[D_XMax - 1]))	// SpeedTable[0] < Xval < SpeedTable[9]
		{
			if (Yval > IntTorqueTable[0] && Yval < IntTorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
			{
				XYdata_X1Y1 = Table[DI->Ypos + 1][DI->Xpos + 1];	//	XYdata_X1Y1=Table[Ypos + 1][Xpos + 1]

				XYdata_Intp_X1 = (((Xval - Xindex) * DI->Xindex_Div) * (XYdata_X1 - XYdata)) + XYdata;
				XYdata_Intp_Y1 = (((Xval - Xindex) * DI->Xindex_Div) * (XYdata_X1Y1 - XYdata_Y1)) + XYdata_Y1;
				XYdata_Intp_Double = (((Yval - Yindex) * DI->Yindex_Div) * (XYdata_Intp_Y1 - XYdata_Intp_X1)) + XYdata_Intp_X1;
			}
			else // Yval가 TorqueTable[9] 또는 orqueTable[0]랑 같을 때
			{
				XYdata_Intp_Double = (((Xval - Xindex) * DI->Xindex_Div) * (XYdata_X1 - XYdata)) + XYdata;
			}
		}
		else // Xval == SpeedTable[9] 또는 Xval == SpeedTable[0]
		{
			if (Yval > IntTorqueTable[0] && Yval < IntTorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
			{
				XYdata_Intp_Double = (((Yval - Yindex) * DI->Yindex_Div) * (XYdata_Y1 - XYdata)) + XYdata;
			}
			else // Yval가 TorqueTable[9]와 TorqueTable[0]랑 같을 때
			{
				XYdata_Intp_Double = XYdata;	//	XYdata_Intp_Double = XYdata;
			}
		}
		DI->XYdata_Intp_Double = XYdata_Intp_Double;

		if (DI->QAxis == D_SET)	// QAxis가 1이면
		{
			if (DI->YSign == D_SET)	// QAxis가 1이고 YSign이 1이면 XYdata_Intp_Double은 부호가 바뀐다.
				DI->XYdata_Intp_Double = -DI->XYdata_Intp_Double;
		}

#if(INNER_TEST)

}
	end_time = clock();
#endif
	return DI->XYdata_Intp_Double/100000;	// XYdata_Intp_Double 리턴
}

float32 DoubleInterpolataion(sDoubleInterp* DI, float32(*Table)[10])	//	211119 최승훈 책임님이랑 이야기 후 float32(*Table)[11])로 수정함.
{
#if 0
	float32 speedTable[D_XMax] = { 0.51, 0.61, 0.71, 0.81, 0.91, 1.01, 1.11, 1.21, 1.31, 1.41 };	// 지역 배열
	float32 torqueTable[D_YMax] = { 0, 0.37, 0.40, 0.43, 0.48, 0.54, 0.61, 0.71, 0.84, 1.0 };		// 지역 배열	
	//임시로 작성한 배열
#endif
	float32 Xval, Yval; // DI->Xval, DI->Yval 대신 써먹을 변수

	Uint8 Xpos, Ypos;   // DI->Xpos, DI->Ypos 대신 써먹을 변수
	Uint8 XposExist;
	Uint8 YposExist;
	Uint32 middle;
	Uint32 left;
	Uint32 right;
	int32 compare_num;

#if(SEARCH_ALGORITHM==4)
	node* temp;
#endif
#if(INNER_TEST)
	start_time = clock();

	for (int i = 0; i < TEST_COUNT; i++)
	{
#endif
#if(SEARCH_ALGORITHM==3||SEARCH_ALGORITHM==4)
		XposExist = 0;
		YposExist = 0;
#endif

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


		if (DI->Xval >= SpeedTable[D_XMax - 1])
		{
			DI->Xval = SpeedTable[D_XMax - 1];		// X값이 스피드테이블의 최대값보다 크면 스피드테이블의 최대값으로 입력되게 함
#if(SEARCH_ALGORITHM==3||SEARCH_ALGORITHM==4)
			DI->Xpos = 9;
			XposExist = 1;
#endif
		}
		else if (DI->Xval <= SpeedTable[0])
		{
			DI->Xval = SpeedTable[0];				// X값이 스피드테이블의 최소값보다 작거나 같으면 스피드테이블의 최소값으로 입력되게 함
			
#if(SEARCH_ALGORITHM==3||SEARCH_ALGORITHM==4)
			DI->Xpos = 0;
			XposExist = 1;
#endif
		}

		if (DI->Yval >= TorqueTable[D_YMax - 1])
		{
			DI->Yval = TorqueTable[D_YMax - 1];		// Y값이 스피드테이블의 최대값보다 크면 스피드테이블의 최대값으로 입력되게 함
#if(SEARCH_ALGORITHM==3||SEARCH_ALGORITHM==4)
			DI->Ypos = 9;
			YposExist = 1;
#endif
		}
		else if (DI->Yval <= TorqueTable[0])
		{
			DI->Yval = TorqueTable[0];				// Y값이 스피드테이블의 최소값보다 작거나 같으면 스피드테이블의 최소값으로 입력되게 함
#if(SEARCH_ALGORITHM==3||SEARCH_ALGORITHM==4)
			DI->Ypos = 0;
			YposExist = 1;
#endif
		}


		Xval = DI->Xval;	// DI->Xval 대신에 Xval 사용함으로써 불필요한 포인터 연산 줄임
		Yval = DI->Yval;	// DI->Yval 대신에 Yval 사용함으로써 불필요한 포인터 연산 줄임

#if(SEARCH_TIME)
		clock_t search_start, search_end;
		search_start = clock();
		for (int i = 0; i < TEST_COUNT; i++)
		{
#endif
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
		// for문 개선하여 앞뒤로 찾는 알고리즘
		for (int i = 0; i <= (D_XMax - 1); i++)	// Xcnt를 가지고 D_Max(10)번 반복
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

		for (int i = 0; i <= (D_YMax - 1); i++)	// Ycnt를 가지고 D_Max(10)번 반복
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
		// for문 개선 + Xcnt와 Ycnt를 지역변수 i로 바꿈
		for (register unsigned int i = 0; i < 5; i++)	// i를 가지고 5번 반복
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

		for (register unsigned int i = 0; i < 5; i++)	// i를 가지고 5번 반복
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
#elif(SEARCH_ALGORITHM==3)
		// 이진 탐색 알고리즘
#if(SEARCH_TIME)
		XposExist = 0;
		YposExist = 0;
#endif



		// left = 0, right = n-1 로 전달
		left = 0, right = 9;
		while (!XposExist)
		{
			middle = (left + right)>> 1;
#if 1
			// switch로 구현
			compare_num = ((Xval < SpeedTable[middle]) ? -1 : ((SpeedTable[middle] <= Xval) && (Xval < SpeedTable[middle + 1])) ? 0 : 1);
			
			switch (compare_num)
			{
			case  -1:
				right = middle - 1;
				break;
			case   1:
				left = middle + 1;
				break;
			default:
				DI->Xpos = middle;
				XposExist = 1;
				break;
			}
#else
			// if로 구현
			compare_num = ((Xval < SpeedTable[middle]) ? -1 : ((SpeedTable[middle] <= Xval) && (Xval < SpeedTable[middle + 1])) ? 0 : 1);

			if (compare_num < 0)
			{
				right = middle - 1;
			}
			else
			{
#if 1
				if (compare_num)
				{
					left = middle + 1;
				}
				else
				{
					DI->Xpos = middle;
					break;
				}
#else
				if (!compare_num)
				{
					DI->Ypos = middle;
					break;
				}
				else
				{
					left = middle + 1;
				}
#endif
			}
#endif
		}

		/*printf("Xpos : %d\n", DI->Xpos);*/


		left = 0, right = 9;
		while (!YposExist)
		{
			middle = (left + right) >> 1;
			compare_num = ((Yval < TorqueTable[middle]) ? -1 : ((TorqueTable[middle] <= Yval) && (Yval < TorqueTable[middle + 1])) ? 0 : 1);
#if 1
			switch (compare_num)
			{
			case  -1:
				right = middle - 1;
				break;
			case   1:
				left = middle + 1;
				break;
			default:
				DI->Ypos = middle;
				YposExist = 1;
				break;
			}
#else

			if (compare_num < 0)
			{
				right = middle - 1;
			}
			else
			{
#if 1
				if (compare_num)
				{
					left = middle + 1;
				}
				else
				{
					DI->Ypos = middle;
					break;
				}
#else
				if (!compare_num)
				{
					DI->Ypos = middle;
					break;
				}
				else
				{
					left = middle + 1;
				}
#endif
			}
#endif
		}


		/*printf("Ypos : %d\n", DI->Ypos);*/

#elif(SEARCH_ALGORITHM==4)
#if(SEARCH_TIME)
		XposExist = 0;
		YposExist = 0;
#endif

		temp = treePointer;
		while (!XposExist)
		{
			compare_num = ((Xval < SpeedTable[temp->data]) ? -1 : ((SpeedTable[temp->data] <= Xval) && (Xval < SpeedTable[temp->data + 1])) ? 0 : 1);
			switch (compare_num)
			{
			case  -1:
				temp = temp->left;
				//right = middle - 1;
				break;
			case   1:
				temp = temp->right;
				break;
				//left = middle + 1;
			default:
				DI->Xpos = temp->data;
				XposExist = 1;
				break;
			}
		}
		

		temp = treePointer;
		while (!YposExist)
		{
			compare_num = ((Yval < TorqueTable[temp->data]) ? -1 : ((TorqueTable[temp->data] <= Yval) && (Yval < TorqueTable[temp->data + 1])) ? 0 : 1);
			switch (compare_num)
			{
			case  -1:
				temp = temp->left;
				//right = middle - 1;
				break;
			case   1:
				temp = temp->right;
				break;
				//left = middle + 1;
			default:
				DI->Ypos = temp->data;
				YposExist = 1;
				break;
			}
		}

#elif(SEARCH_ALGORITHM==5)
		// for문 개선 + Xcnt와 Ycnt를 지역변수 i로 바꿈
		for (register unsigned int i = 5; i; i--)	// i를 가지고 5번 반복
		{
			if (Xval < SpeedTable[6 - i])		// X값이 스피드 테이블의 i+1번째 값보다 작을 때 Xpos는 i로 결정된다.
			{
				DI->Xpos = 5 - i;
				break;
			}
			else if (Xval >= SpeedTable[i + 4])	// X값이 스피드 테이블의 9-i번째 값 이상일 때 Xpos는 9-i로 결정된다.
			{
				DI->Xpos = i + 4;
				break;
			}
		}

		for (register unsigned int i = 5; i; i--)	// i를 가지고 5번 반복
		{
			if (Yval < TorqueTable[6 - i])		// Y값이 토크 테이블의 i+1번째 값보다 작을 때 Ypos는 i로 결정된다.
			{
				DI->Ypos = 5 - i;
				break;
			}
			else if (Yval >= TorqueTable[i + 4])	// Y값이 토크 테이블의 9-i번째 값 이상일 때 Ypos는 9-i로 결정된다.
			{
				DI->Ypos = i + 4;
				break;
			}
		}
#endif
#if(SEARCH_TIME)
		}
		search_end = clock();
		printf("Search를 %d번 반복 : %dms\n", TEST_COUNT, search_end - search_start);
#endif

		// 여기까지 Xpos와 Ypos를 결정하였다.

#if(TABLE_CHANGE==1)
		DI->Xindex = SpeedTable[DI->Xpos];			//	Xindex는 스피드테이블의 Xpos번째 값
		DI->Xindex_X1 = SpeedTable[DI->Xpos + 1];	//	Xindex_X1은 스피드테이블의 (Xpos+1)번째 값

		DI->Yindex = TorqueTable[DI->Ypos];			//	Yindex는 스피드테이블의 Ypos번째 값
		DI->Yindex_Y1 = TorqueTable[DI->Ypos + 1];	//	Yindex_Y1은 스피드테이블의 (Ypos+1)번째 값


		//xdata = Table[D_DATAIndex][Xpos];


		id_node* node_pointer;

		for (node_pointer = idPointerTable[DI->Ypos]; node_pointer; node_pointer = node_pointer->next)
		{
			if ((DI->Xpos) <= (node_pointer->x_max)) // DI->XYdata = Table[DI->Ypos][DI->Xpos];
			{
				DI->XYdata = node_pointer->data;
#if(PRINT_TEST)
				printf("DI->XYdata : %f\n", DI->XYdata);	// test위해 삽입
#endif
				break;
			}
		}
		
		for (node_pointer = idPointerTable[DI->Ypos]; node_pointer; node_pointer = node_pointer->next)
		{
			if ((DI->Xpos + 1) <= (node_pointer->x_max))
			{
				DI->XYdata_X1 = node_pointer->data;	 // DI->XYdata_X1 = Table[DI->Ypos][DI->Xpos + 1];		
#if(PRINT_TEST)
				printf("DI->XYdata_X1 : %f\n", DI->XYdata_X1);	// test위해 삽입
#endif
				break;
			}
		}
		
		for (node_pointer = idPointerTable[(DI->Ypos) + 1]; node_pointer; node_pointer = node_pointer->next)
		{
			if ((DI->Xpos) <= (node_pointer->x_max))
			{
				DI->XYdata_Y1 = node_pointer->data;  // DI->XYdata_Y1 = Table[DI->Ypos + 1][DI->Xpos];
#if(PRINT_TEST)
				printf("DI->XYdata_Y1 : %f\n", DI->XYdata_Y1);	// test위해 삽입
#endif
				break;
			}
		}

#elif(TABLE_CHANGE==2)
		DI->Xindex = SpeedTable[DI->Xpos];			//	Xindex는 스피드테이블의 Xpos번째 값
#if (INTERPOLATION_ALGORITHM != 3)
		DI->Xindex_X1 = SpeedTable[DI->Xpos + 1];	//	Xindex_X1은 스피드테이블의 (Xpos+1)번째 값
		DI->Yindex_Y1 = TorqueTable[DI->Ypos + 1];	//	Yindex_Y1은 스피드테이블의 (Ypos+1)번째 값
#endif

		DI->Yindex = TorqueTable[DI->Ypos];			//	Yindex는 스피드테이블의 Ypos번째 값



		//xdata = Table[D_DATAIndex][Xpos];
		DI->XYdata = logic_table(DI->Xpos, DI->Ypos);
		//DI->XYdata = Table[DI->Ypos][DI->Xpos];			//	XYdata는 테이블의 (Xpos,Ypos) 값

		DI->XYdata_X1 = logic_table(DI->Xpos + 1, DI->Ypos);
		DI->XYdata_Y1 = logic_table(DI->Xpos, DI->Ypos + 1);
		//DI->XYdata_X1 = Table[DI->Ypos][DI->Xpos + 1];	//	XYdata_X1은 테이블의 (Xpos+1,Ypos) 값
		//DI->XYdata_Y1 = Table[DI->Ypos + 1][DI->Xpos];	//	XYdata_Y1은 테이블의 (Xpos,Ypos+1) 값

#if(PRINT_TEST)
		printf("DI->XYdata : %f\n", DI->XYdata);	// test위해 삽입
		printf("DI->XYdata_X1 : %f\n", DI->XYdata_X1);	// test위해 삽입
		printf("DI->XYdata_Y1 : %f\n", DI->XYdata_Y1);	// test위해 삽입
#endif

#elif(TABLE_CHANGE==3)
//	11 x 11 배열을 사용하려 하였음.
		DI->Xindex = SpeedTable_plus[DI->Xpos];			//	Xindex는 스피드테이블의 Xpos번째 값
		DI->Xindex_X1 = SpeedTable_plus[DI->Xpos + 1];	//	Xindex_X1은 스피드테이블의 (Xpos+1)번째 값

		DI->Yindex = TorqueTable_plus[DI->Ypos];			//	Yindex는 스피드테이블의 Ypos번째 값
		DI->Yindex_Y1 = TorqueTable_plus[DI->Ypos + 1];	//	Yindex_Y1은 스피드테이블의 (Ypos+1)번째 값


		//xdata = Table[D_DATAIndex][Xpos];
		DI->XYdata = IdTable_plus[DI->Ypos][DI->Xpos];
		//DI->XYdata = Table[DI->Ypos][DI->Xpos];			//	XYdata는 테이블의 (Xpos,Ypos) 값

		DI->XYdata_X1 = IdTable_plus[DI->Ypos][DI->Xpos + 1];
		DI->XYdata_Y1 = IdTable_plus[DI->Ypos + 1][DI->Xpos];
		//DI->XYdata_X1 = Table[DI->Ypos][DI->Xpos + 1];	//	XYdata_X1은 테이블의 (Xpos+1,Ypos) 값
		//DI->XYdata_Y1 = Table[DI->Ypos + 1][DI->Xpos];	//	XYdata_Y1은 테이블의 (Xpos,Ypos+1) 값

#if(PRINT_TEST)
		printf("DI->XYdata : %f\n", DI->XYdata);	// test위해 삽입
		printf("DI->XYdata_X1 : %f\n", DI->XYdata_X1);	// test위해 삽입
		printf("DI->XYdata_Y1 : %f\n", DI->XYdata_Y1);	// test위해 삽입
#endif

#else
		DI->Xindex = SpeedTable[DI->Xpos];			//	Xindex는 스피드테이블의 Xpos번째 값
		DI->Xindex_X1 = SpeedTable[DI->Xpos + 1];	//	Xindex_X1은 스피드테이블의 (Xpos+1)번째 값

		DI->Yindex = TorqueTable[DI->Ypos];			//	Yindex는 스피드테이블의 Ypos번째 값
		DI->Yindex_Y1 = TorqueTable[DI->Ypos + 1];	//	Yindex_Y1은 스피드테이블의 (Ypos+1)번째 값

		//xdata = Table[D_DATAIndex][Xpos];
		DI->XYdata = Table[DI->Ypos][DI->Xpos];			//	XYdata는 테이블의 (Xpos,Ypos) 값
		
		DI->XYdata_X1 = Table[DI->Ypos][DI->Xpos + 1];	//	XYdata_X1은 테이블의 (Xpos+1,Ypos) 값
		DI->XYdata_Y1 = Table[DI->Ypos + 1][DI->Xpos];	//	XYdata_Y1은 테이블의 (Xpos,Ypos+1) 값

#if(PRINT_TEST)
		printf("DI->XYdata : %f\n", DI->XYdata);	// test위해 삽입
		printf("DI->XYdata_X1 : %f\n", DI->XYdata_X1);	// test위해 삽입
		printf("DI->XYdata_Y1 : %f\n", DI->XYdata_Y1);	// test위해 삽입
#endif

#endif


#if(INTERPOLATION_TIME)
		clock_t inter_start, inter_end;
		inter_start = clock();
		for (int i = 0; i < TEST_COUNT; i++)
		{
#endif

#if(INTERPOLATION_ALGORITHM!=2&&INTERPOLATION_ALGORITHM!=3)
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
				DI->Xindex_Div = 1.f / (DI->Xindex_X1 - DI->Xindex);	//	Xindex_Div = 1 / (Xindex_X1 - Xindex)
				DI->Yindex_Div = 1.f / (DI->Yindex_Y1 - DI->Yindex);	//	Xindex_Div = 1 / (Yindex_Y1 - Yindex)
				if (Yval > TorqueTable[0] && Yval < TorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
				{
#if(TABLE_CHANGE==1)
					for (node_pointer = idPointerTable[(DI->Ypos) + 1]; node_pointer; node_pointer = node_pointer->next)
					{
						if ((DI->Xpos + 1) <= (node_pointer->x_max))
						{
							DI->XYdata_X1Y1 = node_pointer->data;// DI->XYdata_X1Y1 = Table[DI->Ypos + 1][DI->Xpos + 1];
#if(PRINT_TEST)
							printf("DI->XYdata_X1Y1 : %f\n", DI->XYdata_X1Y1);	// test위해 삽입
#endif
							break;
						}
					}
#else
					DI->XYdata_X1Y1 = Table[DI->Ypos + 1][DI->Xpos + 1];	//	XYdata_X1Y1=Table[Ypos + 1][Xpos + 1]
#if(PRINT_TEST)
					printf("DI->XYdata_X1Y1 : %f\n", DI->XYdata_X1Y1);	// test위해 삽입
#endif
#endif

					DI->XYdata_Intp_X1 = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
					DI->XYdata_Intp_Y1 = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1Y1 - DI->XYdata_Y1)) + DI->XYdata_Y1;
					DI->XYdata_Intp_Double = (((Yval - DI->Yindex) * DI->Yindex_Div) * (DI->XYdata_Intp_Y1 - DI->XYdata_Intp_X1)) + DI->XYdata_Intp_X1;
				}
				else // Yval가 TorqueTable[9] 또는 TorqueTable[0]랑 같을 때
				{
					DI->XYdata_Intp_Double = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
				}
			}
			else // Xval == SpeedTable[9] 또는 Xval == SpeedTable[0]
			{
				if (Yval > TorqueTable[0] && Yval < TorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
				{
					DI->XYdata_Intp_Double = (((Yval - DI->Yindex) * (1.f / (DI->Yindex_Y1 - DI->Yindex))) * (DI->XYdata_Y1 - DI->XYdata)) + DI->XYdata;
				}
				else // Yval가 TorqueTable[9]와 TorqueTable[0]랑 같을 때
				{
					DI->XYdata_Intp_Double = DI->XYdata;	//	XYdata_Intp_Double = XYdata;
				}
			}

#elif(INTERPOLATION_ALGORITHM==3)
		/*DI->Yindex_Div = (1.f / (DI->Yindex_Y1 - DI->Yindex));*/

		//즉 0 < Xval < SpeedTable[Xpos]
			if ((Xval > SpeedTable[0]) && (Xval < SpeedTable[D_XMax - 1]))	// SpeedTable[0] < Xval < SpeedTable[9]
			{
				DI->Xindex_X1 = SpeedTable[DI->Xpos + 1];	//	Xindex_X1은 스피드테이블의 (Xpos+1)번째 값
				DI->Xindex_Div = 1.f / (DI->Xindex_X1 - DI->Xindex);	//	Xindex_Div = 1 / (Xindex_X1 - Xindex)

				if (Yval > TorqueTable[0] && Yval < TorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
				{
					DI->Yindex_Div = (1.f / (DI->Yindex_Y1 - DI->Yindex));

					DI->Yindex_Y1 = TorqueTable[DI->Ypos + 1];	//	Yindex_Y1은 스피드테이블의 (Ypos+1)번째 값
					DI->XYdata_X1Y1 = Table[DI->Ypos + 1][DI->Xpos + 1];	//	XYdata_X1Y1=Table[Ypos + 1][Xpos + 1]
#if(PRINT_TEST)
					printf("DI->XYdata_X1Y1 : %f\n", DI->XYdata_X1Y1);	// test위해 삽입
#endif

					DI->XYdata_Intp_X1 = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
					DI->XYdata_Intp_Y1 = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1Y1 - DI->XYdata_Y1)) + DI->XYdata_Y1;
					DI->XYdata_Intp_Double = (((Yval - DI->Yindex) * DI->Yindex_Div) * (DI->XYdata_Intp_Y1 - DI->XYdata_Intp_X1)) + DI->XYdata_Intp_X1;
				}
				else // Yval가 TorqueTable[9] 또는 TorqueTable[0]랑 같을 때
				{
					DI->XYdata_Intp_Double = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
				}
			}
			else // Xval == SpeedTable[9] 또는 Xval == SpeedTable[0]
			{
				if (Yval > TorqueTable[0] && Yval < TorqueTable[D_YMax - 1])	// TorqueTable[0] < Yval < TorqueTable[9]
				{
					DI->Yindex_Div = (1.f / (DI->Yindex_Y1 - DI->Yindex));
					DI->XYdata_Intp_Double = (((Yval - DI->Yindex) * DI->Yindex_Div) * (DI->XYdata_Y1 - DI->XYdata)) + DI->XYdata;
				}
				else // Yval가 TorqueTable[9]와 TorqueTable[0]랑 같을 때
				{
					DI->XYdata_Intp_Double = DI->XYdata;	//	XYdata_Intp_Double = XYdata;
				}
			}
#else
//	inf값 나오는 경우 있어서 보류

			DI->XYdata_X1Y1 = Table[DI->Ypos + 1][DI->Xpos + 1];	//	XYdata_X1Y1=Table[Ypos + 1][Xpos + 1]

			DI->XYdata_Intp_X1 = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1 - DI->XYdata)) + DI->XYdata;
			DI->XYdata_Intp_Y1 = (((Xval - DI->Xindex) * DI->Xindex_Div) * (DI->XYdata_X1Y1 - DI->XYdata_Y1)) + DI->XYdata_Y1;
			DI->XYdata_Intp_Double = (((Yval - DI->Yindex) * DI->Yindex_Div) * (DI->XYdata_Intp_Y1 - DI->XYdata_Intp_X1)) + DI->XYdata_Intp_X1;

#endif
#if(INTERPOLATION_TIME)
		}
		inter_end = clock();
		printf("Interpolation 계산을 %d번 반복 : %dms\n", TEST_COUNT, inter_end - inter_start);
#endif
		if (DI->QAxis == D_SET)	// QAxis가 1이면
		{
			if (DI->YSign == D_SET)	// QAxis가 1이고 YSign이 1이면 XYdata_Intp_Double은 부호가 바뀐다.
				DI->XYdata_Intp_Double = -DI->XYdata_Intp_Double;
		}

#if(INNER_TEST)

	}
	end_time = clock();
#endif
#if(PRINT_TEST)
	printf("DI->XYdata_Intp_Double : %f\n", DI->XYdata_Intp_Double);
#endif
	return DI->XYdata_Intp_Double;	// XYdata_Intp_Double 리턴
}

inline int compare(int middle, float searchnum,float* table)
{
	return ((searchnum < table[middle]) ? -1 : ((table[middle] <= searchnum) && (searchnum < table[middle + 1])) ? 0 : 1);
}

int main()
{
#if(SEARCH_ALGORITHM==4)
	treePointer = insert(treePointer, 4);
	treePointer = insert(treePointer, 1);
	treePointer = insert(treePointer, 0);
	treePointer = insert(treePointer, 2);
	treePointer = insert(treePointer, 3);
	treePointer = insert(treePointer, 7);
	treePointer = insert(treePointer, 5);
	treePointer = insert(treePointer, 6);
	treePointer = insert(treePointer, 8);
	treePointer = insert(treePointer, 9);
	//			4	
	//		1		7
	//	  0	  2   5	  8
	//		   3   6	9
#endif
#if(TABLE_CHANGE)
	init_id_node();
#endif
#if(RANDOM_TEST)
	srand((unsigned)time(NULL));
	start_time = clock();
	for (int i = 0; i < TEST_COUNT; i++)
	{
		Id.Xval = RandomTable[i % 32];
		Id.Yval = RandomTable[i % 32];
		Id.QAxis = D_CLEAR; // QAxis는 0이다
#else
	Id.Xval = 0.52;		// X값 설정
	Id.Yval = 0.1;		// Y값 설정
	Id.QAxis = D_CLEAR; // QAxis는 0이다

	// index 4의 중간값
	//Id.Xval = 0.91;		// X값 설정
	//Id.Yval = 0.48;		// Y값 설정
	//Id.QAxis = D_CLEAR; // QAxis는 0이다

	// 상한 경계 너머 값
	//Id.Xval = 1.51;		// X값 범위 외
	//Id.Yval = 1.23;		// Y값 범위 외
	//Id.QAxis = D_CLEAR; // QAxis는 0이다
#endif
#if(INTEGER_OPTIMIZE)
	Id.Out = IntDoubleInterpolataion(&Id, IntIdTable);
#else
	Id.Out = DoubleInterpolataion(&Id, IdTable);
#endif

#if(RANDOM_TEST)
	}
	end_time = clock();
#endif

#if(INNER_TEST||RANDOM_TEST)
	printf("경과 시간 : %dms", end_time - start_time);
#else
	printf("Id.Out : %f\n", Id.Out);

#endif
	return 0;
}
