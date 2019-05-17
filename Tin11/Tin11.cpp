// Tin11.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <osgViewer/Viewer>
#include <iostream>
#include <string>
#include <sstream>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Geometry>
#include <osgUtil/Optimizer>
#include <osgUtil/DelaunayTriangulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/CameraManipulator>
#include <math.h>
#include <vector>
#include <osg/ShapeDrawable>
#include <fstream>
#include <osg/LineWidth>
#include <osgUtil/SmoothingVisitor>
#include <osg/Light>
#include <osg/LightSource>
#include <osgShadow/SoftShadowMap>  
#include <osgShadow/ShadowedScene> 
#include <osgShadow/ShadowVolume>  
#include <osgShadow/ShadowTexture>  
#include <osgShadow/ParallelSplitShadowMap>  
#include <osgShadow/MinimalShadowMap>  
#include <osgShadow/StandardShadowMap>  
#include <osgShadow/ViewDependentShadowTechnique>  
#include <osg/Switch>
#include <osgGA/GUIEventHandler>
#include <osgDB/readFile>
#include <osgViewer/ViewerEventHandlers>
#include <osg/ValueObject>
#include <osg/PositionAttitudeTransform>
//#include "WinNls.h"
#include <Windows.h>
#include <WinNls.h>
#include <ctime>
#include  <iomanip>
//#include <thread>
#include <pthread.h>  


using namespace std;

//#pragma comment(lib,"x86/pthreadVC2.lib")  

typedef vector<double> VECTORDOUBLE;
typedef vector<int> VECTORINT;
#define PI 3.14159f

bool ifDrag = false;
float circle[72][2]={0};
//float barOX = 50.f;
float barPor = 50.f;//上一点击位置
static unsigned int distancePor = 0;//上一状态下视点与模型的距离
static unsigned int distanceOri = 0;//原始距离
int slide = 0;
int triNum = 0;//三角形的数量

//标识阴影接收对象    
const int ReceivesShadowTraversalMask=0x1;    
//标识阴影投影对象    
const int CastsShadowTraversalMask=0x2;

static double minXAll;
static double maxXAll;
static double minYAll;
static double maxYAll;

double XSizeAll;
double YSizeAll;

//static osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();//三角形点数组

//double random(double start, double end);//申明random

class MyVector
{
	public:
		double X;
        double Y;
        double Z;
		MyVector()
		{

		}
		MyVector(double x, double y, double z)
        {
            X = x;
            Y = y;
            Z = z;
        
        }
};

class Color
{
public:
	Color()
	{
		m_R = 0.f;
		m_G = 0.f;
		m_B = 0.f;
	}

	Color(float R , float G , float B)
	{
		m_R = R;
		m_G = G;
		m_B = B;
	}
	float getR()
	{
		return m_R;
	}
	float getG()
	{
		return m_G;
	}
	float getB()
	{
		return m_B;
	}
	//getCellColor(float min, float max, float value);
	 /*void setR(float r)
	{
		m_R = r;
	}
	 void setG(float g)
	 {
		 m_G = g;
	 }
	 void setB(float b)
	 {
		 m_B = b;
	 }*/
protected:
	float m_R;
	float m_G;
	float m_B;
};


class MyPoint {
        
	public: 
		MyVector normal;
		bool ifnormal;
		double qem1[4][4];
		double minqem1;
		int minqemindex;
		int index;
		bool change;//是否需要改变
		bool ifdelete;//是否删除
		bool ifCon;//是否边界点
		Color color;
		
		MyPoint()
		{
			ifnormal = false;
			minqemindex = -1;
			minqem1 = 100;
			change = false;
			ifdelete = false;
			ifCon = false;
		}

		MyPoint(double x, double y, double z)
        {
            X = x;
            Y = y;
            Z = z;
			
        }

		void qemclear()
		{
			for(int i = 0 ; i < 4 ; i++)
			{
				for(int j = 0 ; j < 4 ; j++)
				{
					qem1[i][j] = 0;
				}
			}
		}

		double getX()
		{
			return X;
		}

		void setX(double x)
		{
			X = x;
		}

		double getY()
		{
			return Y;
		}

		void setY(double y)
		{
			Y = y;
		}

		double getZ()
		{
			return Z;
		}

		void setZ(double z)
		{
			Z = z;
		}

		void setColor(Color mColor)
		{
			color = mColor;
		}

		

	private:

		 double X;
         double Y;
         double Z;

         
    
    
    
    };



class MyLine
{
public:
    int LeftOrTopTri;
    int RightOrBottomTri;
    int LeftPoint;//左边的点x一定小于右边的点x
    int RightPoint;
    //bool ifCon;
	//double leftqem;//左点的二次误差测度
	//double rightqem;//右点的二次误差测度
	bool ifHave;//
	
    MyLine()
    {
        LeftOrTopTri = -1;
        RightOrBottomTri = -1;
        LeftPoint = -1;
        RightPoint = -1;  
		ifHave = false;
		//ifCon = false;
    
    }
    
	int getLeftOrTopTri()
	{
		return LeftOrTopTri;
	}
	void setLeftOrTopTri(int leftortoptri)
	{
		LeftOrTopTri = leftortoptri;
	}

	int getRightOrBottomTri()
	{
		return RightOrBottomTri;
	}
	void setRightOrBottomTri(int rightorbottomtri)
	{
		RightOrBottomTri = rightorbottomtri;
	}

	int getLeftPoint()
	{
		return LeftPoint;
	}
	void setLeftPoint(int leftpoint)
	{
		LeftPoint = leftpoint;
	}
    
	int setRightPoint()
	{
		return RightPoint;
	}
	void getRightPoint(int rightpoint)
	{
		RightPoint = rightpoint;
	}

};

class MyNormal:public MyPoint
{

};

typedef vector<MyPoint> VECTORMYPOINT;
typedef vector<MyLine> VECTORMYLINE;
typedef vector<MyNormal> VECTORMYNORMAL;
static VECTORMYLINE* lineFile = new VECTORMYLINE;//线文件
static VECTORMYLINE* convexLine = new VECTORMYLINE;//凸壳边界线文件
static VECTORINT* convexP1 = new VECTORINT;//凸壳点索引
static VECTORINT* convexP2 = new VECTORINT;
static MyPoint centerP;




//struct unionMyPoint
//{
//	VECTORMYPOINT* pointPart;
//	//VECTORDOUBLE* indexes1;
//};



class MyTri
{
public:
    int Line1;
    int Line2;
    int Line3;
	int p1;
	int p2;
	int p3;
	MyNormal normal;
    bool Remove;
	bool noActive;
	bool change;
	int index;
	int allIndex;//此三角形在总三角形数组中对应的编号
	int changeIndex;
    MyTri()
    {
        Line1 = -1;
        Line2 = -1;
        Line3 = -1;
		p1 = -1;
		p2 = -1;
		p3 = -1;
		noActive = false;
		change = false;
		changeIndex = -1;
		allIndex = -1;
    
    
    }
    MyTri(int line1,int line2,int line3)
    {
        Line1 = line1;
        Line2 = line2;
        Line3 = line3;


    }
	bool ifExist(int x ,VECTORINT* SearPoint)//查找
	{
		VECTORINT::iterator it;
		//it=find(SearPoint->begin(),SearPoint->end(),x);
		it = find(SearPoint->begin() , SearPoint->end() , x);
		if (it!=SearPoint->end())
		{
		//vec中存在value值
			return true;
		}
		else
		{
		//vec中不存在value值
			return false;
		}
	}


    int ExistLine(VECTORMYLINE* sonLinePart, int Line)//如果存在该线,返回的是另外一个点的索引
    {
        int index = -1;//如果没找到返回-1
		
        if (Line1 == Line || Line2 == Line || Line3 == Line)
        {
            VECTORINT* SearPoint = new VECTORINT;//线的另一个点的索引
			SearPoint->push_back(sonLinePart->at(Line1).LeftPoint);
			SearPoint->push_back(sonLinePart->at(Line1).RightPoint);
			
			if (!ifExist(sonLinePart->at(Line2).LeftPoint , SearPoint))
            {
                //SearPoint.Add(lineFile[Line2].LeFtPoint);
				SearPoint->push_back(sonLinePart->at(Line2).LeftPoint);

            }
			else if (!ifExist(sonLinePart->at(Line2).RightPoint , SearPoint))
            {
                //SearPoint.Add(lineFile[Line2].RigHtPoint);
				SearPoint->push_back(sonLinePart->at(Line2).RightPoint);

            }
            else if (!ifExist(sonLinePart->at(Line3).LeftPoint , SearPoint))
            {
                //SearPoint.Add(lineFile[Line3].LeFtPoint);
				SearPoint->push_back(sonLinePart->at(Line3).LeftPoint);

            }
            else if (!ifExist(sonLinePart->at(Line3).RightPoint , SearPoint))
            {
                //SearPoint.Add(lineFile[Line3].RigHtPoint);
				SearPoint->push_back(sonLinePart->at(Line3).RightPoint);

            }
			else
			{
				delete SearPoint;
				return index;
			}
            for(unsigned int i = 0; i < SearPoint->size(); i++)
            {

				if (sonLinePart->at(Line).LeftPoint == SearPoint->at(i) || sonLinePart->at(Line).RightPoint == SearPoint->at(i))
                {

                    continue;

                }
                else
                {
                    index = SearPoint->at(i);
					break;

                }



            }
			delete SearPoint;


        }
		

        return index;
    }
    
    
    VECTORINT* GetPointIndex(VECTORMYLINE* lineFile)
    {
        VECTORINT* index =new VECTORINT;
        /*index.Add(lineFile[Line1].LeFtPoint);
        index.Add(lineFile[Line1].RigHtPoint);*/
		index->push_back(lineFile->at(Line1).LeftPoint);
		index->push_back(lineFile->at(Line1).RightPoint);
		if (!ifExist(lineFile->at(Line2).LeftPoint , index))
        {
            //index.Add(lineFile[Line2].LeFtPoint);
			index->push_back(lineFile->at(Line2).LeftPoint);
        
        }
		if (!ifExist(lineFile->at(Line2).RightPoint , index))
		{
            //index.Add(lineFile[Line2].RigHtPoint);
			index->push_back(lineFile->at(Line2).RightPoint);

        }



        return index;
    
    
    
    
    }


};

class PointRage
{
public:
	//VECTORINT* RageIndex;
	PointRage()
	{
		RageIndex = new VECTORINT;
	
	}

	void setPointIndex(int index)
	{
		pointIndex = index;
	}

	int getPointIndex()
	{
		return pointIndex;
	}

	void insertRageIndex(int i)
	{
		RageIndex->push_back(i);

	}

	int getRageIndex(int i)
	{
		return RageIndex->at(i);
	}

	int getRageSize()
	{
		return RageIndex->size();
	}

	void clear()
	{
		RageIndex->clear();
	}

	VECTORINT* getAllRageIndex()
	{
		return RageIndex;
	}

	void copy(VECTORINT* RageIndex1)
	{
		unsigned int rageindexsize = RageIndex1->size();
		for(unsigned int i = 0 ; i < rageindexsize ; i++)
		{
			RageIndex->push_back(RageIndex1->at(i));
		}
	}

private:
	int pointIndex;
	VECTORINT* RageIndex;


};

typedef vector<MyTri> VECTORMYTRI;

class SamePointTri
{
public:
	//VECTORINT* RageIndex;
	SamePointTri()
	{
		sameTri = new VECTORMYTRI;
	
	}

	void setPointIndex(int index)
	{
		pointIndex = index;
	}

	int getPointIndex()
	{
		return pointIndex;
	}

	void insertSameTri(MyTri tri)
	{
		sameTri->push_back(tri);
		
	}

	MyTri getTri(int i)
	{
		return sameTri->at(i);
	}

	int getsameTriSize()
	{
		return sameTri->size();
	}

	void clear()
	{
		sameTri->clear();
	}

	VECTORMYTRI* getAllTri()
	{
		return sameTri;
	}

	void copy(VECTORMYTRI* sameTri1)
	{
		unsigned int sameTri1size = sameTri1->size();
		for(unsigned int i = 0 ; i < sameTri1size ; i++)
		{
			sameTri->push_back(sameTri1->at(i));
		}
	}

private:
	int pointIndex;
	VECTORMYTRI* sameTri;


};

typedef vector<SamePointTri> VECTORSAMEPOINTTRI;

class ChangeData
{
public:
	/*MyPoint newPoint;
	MyPoint oldPoint;*/
	VECTORMYTRI* noActiveTri;
	VECTORMYTRI* changeTri;
	VECTORMYTRI* originTri;
	/*VECTORMYTRI* delTri;
	VECTORMYTRI* insertTri;*/
	ChangeData()
	{
		noActiveTri = new VECTORMYTRI;
		changeTri  = new VECTORMYTRI;
		originTri = new VECTORMYTRI;
		/*delTri = new VECTORMYTRI;
		insertTri = new VECTORMYTRI;*/
	}
	void clear()
	{
		changeTri->clear();
		noActiveTri->clear();
		originTri->clear();
		/*delTri->clear();
		insertTri->clear();*/
	}

	void insertNoActive(MyTri tri)
	{
		noActiveTri->push_back(tri);
	}

	void insertChange(MyTri tri)
	{
		changeTri->push_back(tri);
	}

	void insertOrigin(MyTri tri)
	{
		originTri->push_back(tri);
	}

	/*void pushDelTri(MyTri tri)
	{
		delTri->push_back(tri);
	}

	void pushInsertTri(MyTri tri)
	{
		insertTri->push_back(tri);
	}*/
};

class ChangeTriIndex
{
public:
	int index;
	int newIndex;
	int allIndex;
	bool ifdelete;
	/*ChangeTriIndex()
	{
		ifdelete = false;
		newIndex = -1;
	}*/
	ChangeTriIndex(int i , bool ifd , int j):index(i),newIndex(i),ifdelete(ifd),allIndex(j){}
	void changeIndex(int index1)
	{
		newIndex = index1;
	}

};

typedef vector<PointRage> VECTORPOINTRAGE;
typedef vector<ChangeData> VECTORCHANGEDATA;
typedef vector<ChangeTriIndex> VECTORCHANGETRII;
static VECTORCHANGEDATA* changeDataL = new VECTORCHANGEDATA;//第一次的改变信息
static VECTORCHANGETRII* changeTriIndex = new VECTORCHANGETRII;//边折叠后各变化点的新的编号
static VECTORDOUBLE* quadraticError = new VECTORDOUBLE;//二次误差平均值

//static VECTORCHANGETRII* changeSonPartTriIndex0 = new VECTORCHANGETRII;
//static VECTORCHANGETRII* changeSonPartTriIndex1 = new VECTORCHANGETRII;
//static VECTORCHANGETRII* changeSonPartTriIndex2 = new VECTORCHANGETRII;
//static VECTORCHANGETRII* changeSonPartTriIndex3 = new VECTORCHANGETRII;

static VECTORCHANGETRII* changeSonPartTriIndexAll = new VECTORCHANGETRII;

static VECTORDOUBLE* quadraticError0 = new VECTORDOUBLE;//子块0对应的二次误差平均值
static VECTORDOUBLE* quadraticError1 = new VECTORDOUBLE;
static VECTORDOUBLE* quadraticError2 = new VECTORDOUBLE;
static VECTORDOUBLE* quadraticError3 = new VECTORDOUBLE;

static VECTORMYPOINT* pointPart0 = new VECTORMYPOINT;//子块0的点数据
static VECTORMYPOINT* pointPart1 = new VECTORMYPOINT;
static VECTORMYPOINT* pointPart2 = new VECTORMYPOINT;
static VECTORMYPOINT* pointPart3 = new VECTORMYPOINT;

static VECTORINT* convexSonPart0 = new VECTORINT;//子块0的凸壳边界
static VECTORINT* convexSonPart1 = new VECTORINT;
static VECTORINT* convexSonPart2 = new VECTORINT;
static VECTORINT* convexSonPart3 = new VECTORINT;

static VECTORMYTRI* triSonPart0 = new VECTORMYTRI;//子块0的三角形数组
static VECTORMYTRI* triSonPart1 = new VECTORMYTRI;
static VECTORMYTRI* triSonPart2 = new VECTORMYTRI;
static VECTORMYTRI* triSonPart3 = new VECTORMYTRI;

static VECTORMYTRI* triSonPartTemp0 = new VECTORMYTRI;//子块0的临时三角形数组
static VECTORMYTRI* triSonPartTemp1 = new VECTORMYTRI;
static VECTORMYTRI* triSonPartTemp2 = new VECTORMYTRI;
static VECTORMYTRI* triSonPartTemp3 = new VECTORMYTRI;

static VECTORMYLINE* lineSonPart0 = new VECTORMYLINE;//子块0的线数组
static VECTORMYLINE* lineSonPart1 = new VECTORMYLINE;
static VECTORMYLINE* lineSonPart2 = new VECTORMYLINE;
static VECTORMYLINE* lineSonPart3 = new VECTORMYLINE;
static VECTORCHANGEDATA* changeSonPartData0 = new VECTORCHANGEDATA;
static VECTORCHANGEDATA* changeSonPartData1 = new VECTORCHANGEDATA;
static VECTORCHANGEDATA* changeSonPartData2 = new VECTORCHANGEDATA;
static VECTORCHANGEDATA* changeSonPartData3 = new VECTORCHANGEDATA;

static int cornerPoint0[4]; //子块0的四个角点
static int cornerPoint1[4];
static int cornerPoint2[4];
static int cornerPoint3[4];

static VECTORMYPOINT* fourPointAll = new VECTORMYPOINT;


static VECTORMYLINE* convexZeroToOneLine = new VECTORMYLINE;//子块0边界与子块1边界之间生成的线数组,此处线左右点对应的编号为convexZeroToOnePoint数组的编号，下面一样
static VECTORMYLINE* convexTwoToThreeLine = new VECTORMYLINE;//子块2边界与子块3边界之间生成的线数组
static VECTORMYLINE* convexTopToButtomLine = new VECTORMYLINE;//子块0、1合并后与子块2、3合并后之间生成的线数组

static VECTORMYPOINT* convexZeroToOnePoint = new VECTORMYPOINT;//同上
static VECTORMYPOINT* convexTwoToThreePoint = new VECTORMYPOINT;
static VECTORMYPOINT* convexTopToButtomPoint = new VECTORMYPOINT;

static VECTORMYTRI* convexZeroToOneTri = new VECTORMYTRI;//各个子块缝隙之间生成的三角形数组，三角形三点的编号为convexZeroToOnePoint数组的编号，下面一样
//static VECTORMYTRI* convexZeroToOneTempTri = new VECTORMYTRI;
static VECTORMYTRI* convexTwoToThreeTri = new VECTORMYTRI;
//static VECTORMYTRI* convexTwoToThreeTempTri = new VECTORMYTRI;
static VECTORMYTRI* convexTopToButtomTri = new VECTORMYTRI;
//static VECTORMYTRI* convexTopToButtomTempTri = new VECTORMYTRI;


class ChangeNewData
{
public:
	//VECTORINT* changeNum;
	VECTORCHANGETRII* changeTriIndex;
	ChangeNewData()
	{
		changeTriIndex = new VECTORCHANGETRII;
		//changeNum = new VECTORINT;
	}

	void copy(VECTORCHANGETRII* changeTrii)
	{
		unsigned int changeTriiSize = changeTrii->size();
		for(unsigned int i = 0 ; i < changeTriiSize ; i++)
		{
			changeTriIndex->push_back(changeTrii->at(i));
		}
	}

	void insertChangeTrii(ChangeTriIndex changetriindex)
	{
		changeTriIndex->push_back(changetriindex);
	}
};

class JudgeLine
{
public:
	MyLine createL1;
	MyLine createL2;
	bool ifComplete;
	/*bool createL1;
	bool createL2;*/
	JudgeLine()
	{
		ifComplete = false;
	}

	JudgeLine(MyLine line1 , MyLine line2):createL1(line1) , createL2(line2)
	{
		/*createL1.ifHave = false;
		createL2.ifHave = false;*/
		ifComplete = false;
	}

	void setIfCom(bool ifcom)
	{
		ifComplete = ifcom;
	}

	/*void setLine1True()
	{
		createL1 = true;
	}

	void setLine2True()
	{
		createL2 = true;
	}*/

};


typedef vector<ChangeNewData> VECTORCHANGENEWD;
static VECTORCHANGENEWD* changeNewData = new VECTORCHANGENEWD;
static VECTORCHANGENEWD* changeNewDataAll = new VECTORCHANGENEWD;

//static VECTORCHANGENEWD* changeSonPartNewData0 = new VECTORCHANGENEWD;
//static VECTORCHANGENEWD* changeSonPartNewData1 = new VECTORCHANGENEWD;
//static VECTORCHANGENEWD* changeSonPartNewData2 = new VECTORCHANGENEWD;
//static VECTORCHANGENEWD* changeSonPartNewData3 = new VECTORCHANGENEWD;
static VECTORCHANGENEWD* changeSonPartNewDataAll = new VECTORCHANGENEWD;



struct unionMyPoint
{
	VECTORMYPOINT* sonPointPart;
	int partIndex;
};

struct unionIndex
{
	//VECTORMYPOINT* sonPointPart;
	int partIndex;
};





//static double windowHeight = 0;

//static osgText::Text* title = new osgText::Text;
//static osg::StateAttribute::GLModeValue value = osg::StateAttribute::ON;

osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;//纹理贴图
osg::ref_ptr<osg::Geometry> geom1 = new osg::Geometry();

//osg::ref_ptr<osg::Geometry> geom2 = new osg::Geometry();
//osg::ref_ptr<osg::Geometry> geom3 = new osg::Geometry();
//osg::ref_ptr<osg::Geometry> geom4 = new osg::Geometry();
//osg::ref_ptr<osg::Geometry> geom5 = new osg::Geometry();
//osg::ref_ptr<osg::Geometry> geom6 = new osg::Geometry();
//osg::ref_ptr<osg::Geometry> geom7 = new osg::Geometry();
//osg::ref_ptr<osg::Geometry> geom8 = new osg::Geometry();
//osg::ref_ptr<osg::Geometry> geom9 = new osg::Geometry();
//osg::ref_ptr<osg::Geometry> geom10 = new osg::Geometry();
//osg::ref_ptr<osg::Geometry> geom11 = new osg::Geometry();

typedef vector<ChangeNewData> VECTORCHANGENEWDATA;

static VECTORCHANGENEWDATA* changeNewDataL = new VECTORCHANGENEWDATA;//边折叠后整理完的点的编号

class HUD : public osg::Referenced
{
private:
    osg::ref_ptr< osg::Camera > _camera;
    osg::ref_ptr< osgText::Text > _modeText;    
    osg::ref_ptr< osg::PositionAttitudeTransform > _titlePAT;
	osg::ref_ptr< osg::Switch > coordinateSwitch; 
	osg::ref_ptr< osg::PositionAttitudeTransform > _coordinatePAT;
	osg::ref_ptr< osg::PositionAttitudeTransform > _slideBarPAT;
	osg::ref_ptr< osg::Switch > slideBallSwitch; 
	osg::ref_ptr< osg::PositionAttitudeTransform > _slideBallPAT;
	osg::ref_ptr< osgText::Text > _coordText;

public:
    HUD(){
        _camera = createCamera();
        //_camera->addChild( createText() );
		_camera->addChild( createCoordinate() );
		_camera->addChild(createSlideBall());
    }

    osg::Camera* createCamera( void )
    {
  //      osg::Camera* camera=new osg::Camera;

  //      //camera->setViewport(0,0,1440,810);
  //      camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
  //      camera->setProjectionMatrixAsOrtho2D(0,1536,0,1024);
		////camera->setViewMatrix(osg::Matrix::identity());
  //      camera->setRenderOrder(osg::Camera::POST_RENDER);
		//camera->setProjectionResizePolicy(osg::Camera::FIXED);
  //      camera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
  //      camera->setClearMask(GL_DEPTH_BUFFER_BIT);

  //      return camera;




		osg::Camera* camera = new osg::Camera;

		camera->setViewport(0 , 0 , 3000 , 2000);
		camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
		camera->setProjectionMatrixAsOrtho2D(0,3000,0,2000);
        camera->setRenderOrder(osg::Camera::POST_RENDER);
        camera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		camera->setProjectionResizePolicy(osg::Camera::FIXED);
        camera->setClearMask(GL_DEPTH_BUFFER_BIT);
		camera->setClearColor(osg::Vec4(1.f , 1.f , 1.f , 1.0f));

		return camera;
    }

    osg::Node* createText( void )
    {
        osg::Geode* textGeode = new osg::Geode;
		textGeode->setDataVariance(osg::Object::DYNAMIC);
		//osgGA::GUIEventAdapter& ea;
        osgText::Text* title = new osgText::Text;
		/*osgText::Text* title1 = new osgText::Text;
		osgText::Text* title2 = new osgText::Text;
		osgText::Text* title3 = new osgText::Text;
		osgText::Text* title4 = new osgText::Text;*/


        title->setFont("fonts/simkai.ttf");
        title->setCharacterSize(20);
        title->setLineSpacing(0.4f);
		title->setText(L"TIN构建及简化(P键为多边形填充,O键为线框,I键关闭纹理,U键开启纹理)");
		
		
		textGeode->addDrawable( title );
        _modeText = new osgText::Text;
        _modeText->setFont("fonts/simhei.ttf");
        //_modeText->setCharacterSize(18);
        _modeText->setPosition( osg::Vec3f(0.f, -10.f, 0.f ) );
        _modeText->setDataVariance(osg::Object::DYNAMIC);
        textGeode->addDrawable( _modeText.get() );

        _titlePAT = new osg::PositionAttitudeTransform;
		//double height = ea.getWindowHeight();
        _titlePAT->setPosition( osg::Vec3f( 10, 640, 0.f ) );
        _titlePAT->addChild(textGeode);

        return _titlePAT;
    }

	

	osg::Switch* createCoordinate(void)//绘制进度条
	{
		coordinateSwitch = new osg::Switch;
		coordinateSwitch->setName("coordinateSwitch");
		osg::Geode* textGeode = new osg::Geode;
		//coordinateSwitch->setAllChildrenOff();
		_coordinatePAT = new osg::PositionAttitudeTransform;
		osg::Geode* coordinateGeode = new osg::Geode;
		//osgText::Text* text = new osgText::Text;
		/*text->setFont("fonts/simkai.ttf");
        text->setCharacterSize(20);
        text->setLineSpacing(0.4f);
		text->setText("原始格网");
		textGeode->addDrawable( text );*/
		_coordText = new osgText::Text;

		_coordText->setFont("fonts/simhei.ttf");
		_coordText->setCharacterSize(18);
        _coordText->setPosition( osg::Vec3f(50.f, 105.f, 0.f ) );
		_coordText->setText(L"原始格网");
		_coordText->setDataVariance(osg::Object::DYNAMIC);
		_coordText->setBackdropType(osgText::Text::OUTLINE);
		textGeode->addDrawable( _coordText.get() );


		osg::Vec3f p1 , p2;
		
		
		{
			_slideBarPAT = new osg::PositionAttitudeTransform;
			osg::ref_ptr<osg::Geode> slideBar_GEO = new osg::Geode;
			p1 = osg::Vec3f( 50.0f, 50.0f, 0.f);
			p2 = osg::Vec3f( 1050.f, 55.f, 0.f);
			slideBar_GEO->addDrawable(createSide(p1 , p2 , osg::Vec4(1.f , 0.647f , 0.f , 1.f)));
			_slideBarPAT->addChild(slideBar_GEO);
			_coordinatePAT->addChild(textGeode);
			_coordinatePAT->addChild(_slideBarPAT);

		}




		coordinateSwitch->addChild( _coordinatePAT );

		return coordinateSwitch;

	}

	osg::Switch* createSlideBall(void)//绘制滑动球
	{
		slideBallSwitch = new osg::Switch;
		slideBallSwitch->setName("slideBallSwitch");
		//indexLineSwitch->setAllChildrenOff();
		osg::Vec3f p;

		_slideBallPAT = new osg::PositionAttitudeTransform;
		osg::Geode* slideBallGeode = new osg::Geode;
		p = osg::Vec3f(50.f , 52.5f , 0.f);
		slideBallGeode->addDrawable(createM(p , osg::Vec4f(1.f , 0.647f , 0.f , 1.f)));
		_slideBallPAT->addChild(slideBallGeode);
		slideBallSwitch->addChild(_slideBallPAT);
		return slideBallSwitch;
	}

	void Init_Circle(float Radius) //用三角形逼近画圆
	{
	
		float angle=0;
		
		for(int i=0;i<72;i++)
		
		{
		
		circle[i][0]=Radius*cos(angle)+50.f;
		
		circle[i][1]=Radius*sin(angle)+52.5f;
		
		angle+=2.0*PI/72.0f;
		
		}
	}

	osg::ref_ptr<osg::Geometry> createSide(const osg::Vec3f &p_left_bottom , const osg::Vec3f &p_right_top , osg::Vec4 color)
	{
		osg::ref_ptr<osg::Geometry> geoSide = new osg::Geometry;

		osg::ref_ptr<osg::Vec3Array> keyPoint = new osg::Vec3Array;
		geoSide->setVertexArray(keyPoint.get());
		keyPoint->push_back(p_left_bottom);
		keyPoint->push_back(osg::Vec3f(p_left_bottom.x() , p_right_top.y() , 0.f));
		keyPoint->push_back(p_right_top);
		keyPoint->push_back(osg::Vec3f(p_right_top.x() , p_left_bottom.y() , 0.f));

		for(unsigned int i = 0 ; i < 11 ; i++)
		{
			keyPoint->push_back(osg::Vec3(p_left_bottom.x() + 100 * i - 2.5f , p_left_bottom.y() - 5.f , 0.f));
			keyPoint->push_back(osg::Vec3(p_left_bottom.x() + 100 * i + 2.5f , p_left_bottom.y() - 5.f , 0.f));
			keyPoint->push_back(osg::Vec3(p_left_bottom.x() + 100 * i + 2.5f , p_left_bottom.y() + 10.f , 0.f));
			keyPoint->push_back(osg::Vec3(p_left_bottom.x() + 100 * i - 2.5f , p_left_bottom.y() + 10.f , 0.f));
		}

		osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;
		geoSide->setColorArray(c.get());
		geoSide->setColorBinding(osg::Geometry::BIND_OVERALL);
		c->push_back(color);

		osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
		geoSide->setNormalArray(n.get());
		geoSide->setNormalBinding(osg::Geometry::BIND_OVERALL);
		n->push_back(osg::Vec3(0.f , -1.f , 0.f));

		geoSide->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS , 0 , 48));

		osg::StateSet* stateset = geoSide->getOrCreateStateSet();

		stateset->setMode(GL_BLEND , osg::StateAttribute::ON);
		stateset->setMode(GL_LIGHTING , osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
		stateset->setRenderBinDetails(11 , "RenderBin");

		return geoSide.get();


	}

	osg::ref_ptr<osg::Geometry> createM(const osg::Vec3f &P , osg::Vec4f color)
	{
		osg::ref_ptr<osg::Geometry> geoM = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> keyPoint = new osg::Vec3Array;
		Init_Circle(7.5f);
		geoM->setVertexArray(keyPoint.get());
		for(int i = 0 ; i < 72 ; i++)
		{
			keyPoint->push_back(osg::Vec3(circle[i][0] , circle[i][1] , 0.f));
		}
		//keyPoint->push_back(P);

		osg::ref_ptr<osg::Vec4Array> c = new osg::Vec4Array;
		geoM->setColorArray(c.get());
		geoM->setColorBinding(osg::Geometry::BIND_OVERALL);
		c->push_back(color);

		osg::ref_ptr<osg::Vec3Array> n = new osg::Vec3Array;
		geoM->setNormalArray(n.get());
		geoM->setNormalBinding(osg::Geometry::BIND_OVERALL);
		n->push_back(osg::Vec3(0.f , -1.f , 0.f));

		geoM->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON , 0 , 72));
		osg::StateSet* stateset = geoM->getOrCreateStateSet();

		stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
		stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
		stateset->setRenderBinDetails(11, "RenderBin");

		return geoM.get();

	}

    void setSceneText( const std::string& preset )
    {
		std::wstring strWideChar;
		gb2312ToUnicode(preset, strWideChar);
		_coordText->setText( strWideChar.c_str() );

    }

	void gb2312ToUnicode(const std::string& src, std::wstring& result)//格式转换
	{
		int n = MultiByteToWideChar( CP_ACP, 0, src.c_str(), -1, NULL, 0 );
		result.resize(n);
		::MultiByteToWideChar( CP_ACP, 0, src.c_str(), -1, (LPWSTR)result.c_str(), result.length());
	}

    osg::Camera* getHudCamera(void)
    {
        return _camera.get();
    }

	void setSlideBallSwtichOn()
	{
		slideBallSwitch->setAllChildrenOn();
	}

	void setSlideBallPosition(const osg::Vec3& position)
	{
		_slideBallPAT->setPosition(position);
	}

	void setSlideBallSwitchOff()
	{
		slideBallSwitch->setAllChildrenOff();
	}
};

string getstring ( const double n );
osg::Vec3 worldToScreen(osg::Vec3 world , osg::ref_ptr<osgViewer::Viewer> viewer);
osg::Vec3 screenToWorld(osg::Vec3 screen , osg::ref_ptr<osgViewer::Viewer> viewer);

static VECTORMYPOINT* rPoint;//所有点的数组
static double xSize;
static double ySize;
int minX;
int minY;
int maxX;
int maxY;
double minZ;
double maxZ;
int stateP = 0;//地形的状态 0为面，1为线

class KeyboradHandler:public osgGA::GUIEventHandler
{
public:
	KeyboradHandler(HUD* HUD):_HUD(HUD){}

	virtual bool handle(const osgGA::GUIEventAdapter& ea , osgGA::GUIActionAdapter& aa , osg::Object* , osg::NodeVisitor*) 
	{
		osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		osg::Switch* root = dynamic_cast<osg::Switch*>(viewer->getSceneData());
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(root->getChild(0)->asGroup()->getChild(0)->asGeode()->getDrawable(0));
		//osg::Geometry* geom = dynamic_cast<osg::Geometry*>(root->getChild(0)->asGroup()->getChild(0)->asGroup()->getChild(0)->asGeode()->getDrawable(0));
		

		if(!viewer) return false;
		/*if(!root) return false;
		if(!geom) return false;*/

		osg::Vec3Array* v = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
		osg::Vec3Array* nc = dynamic_cast<osg::Vec3Array*>(geom->getNormalArray());
		osg::Vec2Array* texcoords = dynamic_cast<osg::Vec2Array*>(geom->getTexCoordArray(0));
		
		switch(ea.getEventType())
		{
		case osgGA::GUIEventAdapter::KEYDOWN:
			if(ea.getKey() == osgGA::GUIEventAdapter::KEY_Space)
			{
				int width = ea.getWindowWidth();
				int height = ea.getWindowHeight();
				//windowHeight = height;
				viewer->requestWarpPointer(width*0.5 , height*0.5);

			}
			if(ea.getKey() == osgGA::GUIEventAdapter::KEY_P)
			{
				if(stateP == 0)
				{
					osg::ref_ptr<osg::Node> node1 = new osg::Node();
					node1 = root->getChild(0)->asGroup()->getChild(0);
					osg::ref_ptr<osg::StateSet> stateset=node1->getOrCreateStateSet();  
					osg::ref_ptr<osg::PolygonMode> polyMode=new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);  
					stateset->setAttribute(polyMode);
					stateP = 1;
				}
				else if(stateP == 1)
				{
					osg::ref_ptr<osg::Node> node1 = new osg::Node();
					node1 = root->getChild(0)->asGroup()->getChild(0);
					osg::ref_ptr<osg::StateSet> stateset=node1->getOrCreateStateSet();  
					osg::ref_ptr<osg::PolygonMode> polyMode=new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);  
					stateset->setAttribute(polyMode);
					stateP = 0;
				}
				else
				{
					cout<<"按键P出现错误"<<endl;
				}
			}
			if(ea.getKey() == osgGA::GUIEventAdapter::KEY_U)
			{
					
				geom->getOrCreateStateSet()->setTextureAttributeAndModes(0 , texture , osg::StateAttribute::ON);
					
			}
			if(ea.getKey() == osgGA::GUIEventAdapter::KEY_I)
			{
				geom->getOrCreateStateSet()->setTextureAttributeAndModes(0 , texture , osg::StateAttribute::OFF);
			}
			
			break;
		case osgGA::GUIEventAdapter::PUSH:
		case osgGA::GUIEventAdapter::DRAG:
			{
				if(ea.getX()>50.f && ea.getX()<1050.f&&ea.getY()>0.f&&ea.getY()<105.f)
				{
					//ifDrag = true;
					//if(ifDrag)
					//{
					//	//_HUD->setSlideBallSwtichOn();
					//	//_HUD->setIndexLinePosition(osg::Vec3(x-50.f , 0.f , 0.f));
					//	_HUD->setSlideBallPosition(osg::Vec3(ea.getX()-50.f , 0.f , 0.f));
					//	//Pick(ea.getX() , ea.getY() , viewer);
					//	return true;
					//}
					/*if(ea.getY() > 40.f && ea.getY() < 65.f)
					{
						return true;
					}
					else
					{
						return false;

					}*/
					//return true;
					float dValue;//当前点击位置与上一点击位置差值
					unsigned int barPorInt = int(barPor);
					float barNow;//当前点击位置的X
					barNow = ea.getX();
					unsigned int barNowInt = int(barNow);

					

					dValue = ea.getX() - barPorInt;
					//int dValueInt = int(dValue);//差值取整
					
					osg::Vec3d eye,center,up;
					viewer->getCamera()->getViewMatrixAsLookAt(eye,center,up);

					osg::Vec3 x = worldToScreen(osg::Vec3(center.x(), center.y(), center.z()), viewer);
					osg::Vec3 y = worldToScreen(osg::Vec3(center.x(), center.y(), center.z() + 0.44), viewer);

					//osg::Vec3 y = worldToScreen(osg::Vec3(pointPart0->at(1).getX(), pointPart0->at(1).getY(), pointPart0->at(1).getZ()), viewer);

					double length = sqrt((x.x() - y.x())*(x.x() - y.x()) + (x.y() - y.y())*(x.y() - y.y()) /*+ (x.z() - y.z())*(x.z() - y.z())*/);


					if(dValue > 0)
					{
						for(unsigned int i = (barPorInt - 50)*4 ; i < (barNowInt - 50)*4 ; i++)
						{
							/************************
							子块0
							**********************/
							unsigned int changeTriSize = changeSonPartData0->at(i).changeTri->size();
							for(unsigned int j = 0 ; j < changeTriSize ; j++)
							{
								v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);

								v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p3).getX() ,
									pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p3).getY() , 4*pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p3).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p2).getX() ,
									pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p2).getY() , 4*pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p2).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p1).getX() ,
									pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p1).getY() , 4*pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p1).getZ()));

								nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);

								nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p3).normal.X , 
									pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p3).normal.Y , pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p3).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p2).normal.X , 
									pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p2).normal.Y , pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p2).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p1).normal.X , 
									pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p1).normal.Y , pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p1).normal.Z));

								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);

								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p3).getX() - minXAll)/XSizeAll , 
									(pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p3).getY() - minYAll)/YSizeAll));
								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p2).getX() - minXAll)/XSizeAll , 
									(pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p2).getY() - minYAll)/YSizeAll));
								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p1).getX() - minXAll)/XSizeAll , 
									(pointPart0->at(changeSonPartData0->at(i).changeTri->at(j).p1).getY() - minYAll)/YSizeAll));

							}
							v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize + 1).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize + 1).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize + 1).newIndex)*3);

							nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize + 1).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize + 1).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize + 1).newIndex)*3);

							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize + 1).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize + 1).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSize + 1).newIndex)*3);

							/**************
							子块1
							**************/
							unsigned int changeTriSize1 = changeSonPartData1->at(i).changeTri->size();
							for(unsigned int j = 0 ; j < changeTriSize1 ; j++)
							{
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);

								v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p3).getX() ,
									pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p3).getY() , 4*pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p3).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p2).getX() ,
									pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p2).getY() , 4*pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p2).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p1).getX() ,
									pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p1).getY() , 4*pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p1).getZ()));

								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);

								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p3).normal.X , 
									pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p3).normal.Y , pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p3).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p2).normal.X , 
									pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p2).normal.Y , pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p2).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p1).normal.X , 
									pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p1).normal.Y , pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p1).normal.Z));

								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);

								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p3).getX() - minXAll)/XSizeAll , 
									(pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p3).getY() - minYAll)/YSizeAll));							  
								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p2).getX() - minXAll)/XSizeAll , 
									(pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p2).getY() - minYAll)/YSizeAll));							  
								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p1).getX() - minXAll)/XSizeAll , 
									(pointPart1->at(changeSonPartData1->at(i).changeTri->at(j).p1).getY() - minYAll)/YSizeAll));

							}
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1 + 1).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1 + 1).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1 + 1).newIndex)*3);

							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1 + 1).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1 + 1).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1 + 1).newIndex)*3);

							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1 + 1).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1 + 1).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSize1 + 1).newIndex)*3);

							/*************
							子块2
							***************/
							unsigned int changeTriSize2 = changeSonPartData2->at(i).changeTri->size();
							for(unsigned int j = 0 ; j < changeTriSize2 ; j++)
							{
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);

								v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p3).getX() ,
									pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p3).getY() , 4*pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p3).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p2).getX() ,
									pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p2).getY() , 4*pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p2).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p1).getX() ,
									pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p1).getY() , 4*pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p1).getZ()));

								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);

								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p3).normal.X , 
									pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p3).normal.Y , pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p3).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p2).normal.X , 
									pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p2).normal.Y , pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p2).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p1).normal.X , 
									pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p1).normal.Y , pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p1).normal.Z));

								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);

								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p3).getX() - minXAll)/XSizeAll , 
									(pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p3).getY() - minYAll)/YSizeAll));							  
								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p2).getX() - minXAll)/XSizeAll , 
									(pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p2).getY() - minYAll)/YSizeAll));							  
								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p1).getX() - minXAll)/XSizeAll , 
									(pointPart2->at(changeSonPartData2->at(i).changeTri->at(j).p1).getY() - minYAll)/YSizeAll));

							}
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2 + 1).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2 + 1).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2 + 1).newIndex)*3);

							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2 + 1).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2 + 1).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2 + 1).newIndex)*3);

							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2 + 1).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2 + 1).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSize2 + 1).newIndex)*3);

							/***************
							子块3
							**************/
							unsigned int changeTriSize3 = changeSonPartData3->at(i).changeTri->size();
							for(unsigned int j = 0 ; j < changeTriSize3 ; j++)
							{
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);

								v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p3).getX() ,
									pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p3).getY() , 4*pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p3).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p2).getX() ,
									pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p2).getY() , 4*pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p2).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p1).getX() ,
									pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p1).getY() , 4*pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p1).getZ()));

								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);

								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p3).normal.X , 
									pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p3).normal.Y , pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p3).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p2).normal.X , 
									pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p2).normal.Y , pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p2).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p1).normal.X , 
									pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p1).normal.Y , pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p1).normal.Z));

								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);

								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p3).getX() - minXAll)/XSizeAll , 
									(pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p3).getY() - minYAll)/YSizeAll));							  
								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p2).getX() - minXAll)/XSizeAll , 
									(pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p2).getY() - minYAll)/YSizeAll));							  
								texcoords->insert(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p1).getX() - minXAll)/XSizeAll , 
									(pointPart3->at(changeSonPartData3->at(i).changeTri->at(j).p1).getY() - minYAll)/YSizeAll));

							}
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3 + 1).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3 + 1).newIndex)*3);
							v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3 + 1).newIndex)*3);

							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3 + 1).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3 + 1).newIndex)*3);
							nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3 + 1).newIndex)*3);

							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3 + 1).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3 + 1).newIndex)*3);
							texcoords->erase(texcoords->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSize3 + 1).newIndex)*3);


							triNum -= 8;



							/*unsigned int changeTriSize = changeDataL->at(i).changeTri->size();
							for(unsigned int j = 0 ; j < changeTriSize ; j++)
							{
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));

								v->insert(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex) )*3) , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p3).getX() , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p3).getY() , rPoint->at(changeDataL->at(i).changeTri->at(j).p3).getZ()));
								v->insert(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex) )*3) , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p2).getX() , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p2).getY() , rPoint->at(changeDataL->at(i).changeTri->at(j).p2).getZ()));
								v->insert(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex) )*3) , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p1).getX() , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p1).getY() , rPoint->at(changeDataL->at(i).changeTri->at(j).p1).getZ()));

								nc->erase(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);

								nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p3).normal.X , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p3).normal.Y , rPoint->at(changeDataL->at(i).changeTri->at(j).p3).normal.Z));
								nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p2).normal.X , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p2).normal.Y , rPoint->at(changeDataL->at(i).changeTri->at(j).p2).normal.Z));
								nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p1).normal.X , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p1).normal.Y , rPoint->at(changeDataL->at(i).changeTri->at(j).p1).normal.Z));

								texcoords->erase(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);

								texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).changeTri->at(j).p3).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).changeTri->at(j).p3).getY() - rPoint->at(minY).getY())/(ySize)));
								texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).changeTri->at(j).p2).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).changeTri->at(j).p2).getY() - rPoint->at(minY).getY())/(ySize)));
								texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).changeTri->at(j).p1).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).changeTri->at(j).p1).getY() - rPoint->at(minY).getY())/(ySize)));


								
							}
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex )*3));
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex )*3));
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex )*3));
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex )*3));
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex )*3));
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex )*3));

							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));
							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));
							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));

							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));
							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));
							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));*/

							//triNum -= 2;

						}
					}
					else
					{
						for(unsigned int i = ((barPorInt - 50)*4 - 1) ; i >= (barNowInt - 50)*4; i--)
						{
							/****************
							子块3
							****************/

							unsigned int changeTriSizeR3 = changeSonPartData3->at(i).changeTri->size();
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3 + 1).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p3).getX() , 
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p3).getY() , 4*pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p3).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3 + 1).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p2).getX() , 
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p2).getY() , 4*pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p2).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3 + 1).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p1).getX() , 
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p1).getY() , 4*pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p1).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p3).getX() , 
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p3).getY() , 4*pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p3).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p2).getX() , 
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p2).getY() , 4*pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p2).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p1).getX() , 
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p1).getY() , 4*pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p1).getZ()));

							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3 + 1).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p3).getX() ,
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p3).getY() , pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p3).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3 + 1).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p2).getX() ,
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p2).getY() , pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p2).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3 + 1).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p1).getX() ,
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p1).getY() , pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(1).p1).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p3).getX() ,
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p3).getY() , pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p3).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p2).getX() ,
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p2).getY() , pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p2).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(changeTriSizeR3).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p1).getX() ,
								pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p1).getY() , pointPart3->at(changeSonPartData3->at(i).noActiveTri->at(0).p1).getZ()));

							for(int j = (changeTriSizeR3 - 1) ; j >= 0 ; j--)
							{
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);

								v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p3).getX() , 
									pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p3).getY() , 4*pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p3).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p2).getX() , 
									pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p2).getY() , 4*pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p2).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p1).getX() , 
									pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p1).getY() , 4*pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p1).getZ()));

								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3);

								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p3).normal.X , 
									pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p3).normal.Y , pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p3).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p2).normal.X , 
									pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p2).normal.Y , pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p2).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 3).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p1).normal.X , 
									pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p1).normal.Y , pointPart3->at(changeSonPartData3->at(i).originTri->at(j).p1).normal.Z));
							}

							/**************
							子块2
							**************/
							unsigned int changeTriSizeR2 = changeSonPartData2->at(i).changeTri->size();
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2 + 1).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p3).getX() , 
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p3).getY() , 4*pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p3).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2 + 1).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p2).getX() , 
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p2).getY() , 4*pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p2).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2 + 1).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p1).getX() , 
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p1).getY() , 4*pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p1).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p3).getX() , 
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p3).getY() , 4*pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p3).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p2).getX() , 
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p2).getY() , 4*pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p2).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p1).getX() , 
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p1).getY() , 4*pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p1).getZ()));

							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2 + 1).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p3).getX() ,
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p3).getY() , pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p3).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2 + 1).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p2).getX() ,
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p2).getY() , pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p2).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2 + 1).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p1).getX() ,
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p1).getY() , pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(1).p1).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p3).getX() ,
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p3).getY() , pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p3).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p2).getX() ,
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p2).getY() , pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p2).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(changeTriSizeR2).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p1).getX() ,
								pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p1).getY() , pointPart2->at(changeSonPartData2->at(i).noActiveTri->at(0).p1).getZ()));

							for(int j = (changeTriSizeR2 - 1) ; j >= 0 ; j--)
							{
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);

								v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p3).getX() , 
									pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p3).getY() , 4*pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p3).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p2).getX() , 
									pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p2).getY() , 4*pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p2).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p1).getX() , 
									pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p1).getY() , 4*pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p1).getZ()));

								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3);

								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p3).normal.X , 
									pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p3).normal.Y , pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p3).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p2).normal.X , 
									pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p2).normal.Y , pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p2).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 2).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p1).normal.X , 
									pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p1).normal.Y , pointPart2->at(changeSonPartData2->at(i).originTri->at(j).p1).normal.Z));
							}

							/**************
							子块1
							**************/
							unsigned int changeTriSizeR1 = changeSonPartData1->at(i).changeTri->size();
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1 + 1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p3).getX() , 
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p3).getY() , 4*pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p3).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1 + 1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p2).getX() , 
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p2).getY() , 4*pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p2).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1 + 1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p1).getX() , 
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p1).getY() , 4*pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p1).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p3).getX() , 
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p3).getY() , 4*pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p3).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p2).getX() , 
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p2).getY() , 4*pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p2).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p1).getX() , 
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p1).getY() , 4*pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p1).getZ()));

							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1 + 1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p3).getX() ,
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p3).getY() , pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p3).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1 + 1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p2).getX() ,
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p2).getY() , pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p2).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1 + 1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p1).getX() ,
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p1).getY() , pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(1).p1).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p3).getX() ,
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p3).getY() , pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p3).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p2).getX() ,
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p2).getY() , pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p2).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(changeTriSizeR1).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p1).getX() ,
								pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p1).getY() , pointPart1->at(changeSonPartData1->at(i).noActiveTri->at(0).p1).getZ()));

							for(int j = (changeTriSizeR1 - 1) ; j >= 0 ; j--)
							{
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);

								v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p3).getX() , 
									pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p3).getY() , 4*pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p3).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p2).getX() , 
									pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p2).getY() , 4*pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p2).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p1).getX() , 
									pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p1).getY() , 4*pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p1).getZ()));

								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3);

								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p3).normal.X , 
									pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p3).normal.Y , pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p3).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p2).normal.X , 
									pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p2).normal.Y , pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p2).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i + 1).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p1).normal.X , 
									pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p1).normal.Y , pointPart1->at(changeSonPartData1->at(i).originTri->at(j).p1).normal.Z));
							}

							/************
							子块0
							**************/
							unsigned int changeTriSizeR = changeSonPartData0->at(i).changeTri->size();
							v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p3).getX() , 
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p3).getY() , 4*pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p3).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p2).getX() , 
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p2).getY() , 4*pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p2).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p1).getX() , 
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p1).getY() , 4*pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p1).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p3).getX() , 
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p3).getY() , 4*pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p3).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p2).getX() , 
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p2).getY() , 4*pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p2).getZ()));
							v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p1).getX() , 
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p1).getY() , 4*pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p1).getZ()));

							nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p3).getX() ,
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p3).getY() , pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p3).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p2).getX() ,
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p2).getY() , pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p2).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p1).getX() ,
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p1).getY() , pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(1).p1).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p3).getX() ,
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p3).getY() , pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p3).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p2).getX() ,
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p2).getY() , pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p2).getZ()));
							nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p1).getX() ,
								pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p1).getY() , pointPart0->at(changeSonPartData0->at(i).noActiveTri->at(0).p1).getZ()));

							for(int j = (changeTriSizeR - 1) ; j >= 0 ; j--)
							{
								v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);
								v->erase(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);

								v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p3).getX() , 
									pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p3).getY() , 4*pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p3).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p2).getX() , 
									pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p2).getY() , 4*pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p2).getZ()));
								v->insert(v->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p1).getX() , 
									pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p1).getY() , 4*pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p1).getZ()));

								nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3);

								nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p3).normal.X , 
									pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p3).normal.Y , pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p3).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p2).normal.X , 
									pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p2).normal.Y , pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p2).normal.Z));
								nc->insert(nc->begin() + (changeNewDataAll->at(4*i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p1).normal.X , 
									pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p1).normal.Y , pointPart0->at(changeSonPartData0->at(i).originTri->at(j).p1).normal.Z));
							}


							/*int changeTriSizeR = changeDataL->at(i).changeTri->size();

							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).getZ()));
							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).getZ()));
							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).getZ()));
							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).getZ()));
							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).getZ()));
							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).getZ()));

							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).normal.Z));
							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).normal.Z));
							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).normal.Z));
							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).normal.Z));
							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).normal.Z));
							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).normal.Z));



							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).getY() - rPoint->at(minY).getY())/(ySize)));
							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).getY() - rPoint->at(minY).getY())/(ySize)));
							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).getY() - rPoint->at(minY).getY())/(ySize)));
							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).getY() - rPoint->at(minY).getY())/(ySize)));
							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).getY() - rPoint->at(minY).getY())/(ySize)));
							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).getY() - rPoint->at(minY).getY())/(ySize)));

							
							for(int j = (changeTriSizeR - 1); j >=0 ; j--)
							{
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));


								v->insert(v->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p3).getX() , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p3).getY() , rPoint->at(changeDataL->at(i).originTri->at(j).p3).getZ()));
								v->insert(v->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p2).getX() , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p2).getY() , rPoint->at(changeDataL->at(i).originTri->at(j).p2).getZ()));
								v->insert(v->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p1).getX() , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p1).getY() , rPoint->at(changeDataL->at(i).originTri->at(j).p1).getZ()));

								nc->erase(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);

								
								nc->insert(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p3).normal.X , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p3).normal.Y , rPoint->at(changeDataL->at(i).originTri->at(j).p3).normal.Z));
								nc->insert(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p2).normal.X , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p2).normal.Y , rPoint->at(changeDataL->at(i).originTri->at(j).p2).normal.Z));
								nc->insert(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p1).normal.X , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p1).normal.Y , rPoint->at(changeDataL->at(i).originTri->at(j).p1).normal.Z));


								texcoords->erase(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);


								texcoords->insert(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).originTri->at(j).p3).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).originTri->at(j).p3).getY() - rPoint->at(minY).getY())/(ySize)));
								texcoords->insert(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).originTri->at(j).p2).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).originTri->at(j).p2).getY() - rPoint->at(minY).getY())/(ySize)));
								texcoords->insert(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).originTri->at(j).p1).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).originTri->at(j).p1).getY() - rPoint->at(minY).getY())/(ySize)));

								

							}*/

							
							
							
							triNum += 8;

						}
					}

					v->dirty();
					nc->dirty();
					string num = getstring(4*(barNow-50));
					_HUD->setSceneText(num);
					//geom->dirtyDisplayList();
					//geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES , 0 , 3 * triNum));
					geom->setPrimitiveSet(0 , new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES , 0 , 3 * triNum));


					if(4*(barNow-50) == 792)
					{
						cout <<"当化简次数为"<<num<<"时，"<<"三角形的个数为："<< triNum << endl;
					}

					if(4*(barNow-50) == 1596)
					{
						cout <<"当化简次数为"<<num<<"时，"<<"三角形的个数为："<< triNum << endl;
					}

					if(4*(barNow-50) == 2796)
					{
						cout <<"当化简次数为"<<num<<"时，"<<"三角形的个数为："<< triNum << endl;
					}

					if(4*(barNow-50) == 3192)
					{
						cout <<"当化简次数为"<<num<<"时，"<<"三角形的个数为："<< triNum << endl;
					}

					if(4*(barNow-50) == 3992)
					{
						cout <<"当化简次数为"<<num<<"时，"<<"三角形的个数为："<< triNum << endl;
					}

					barPor = ea.getX();

					_HUD->setSlideBallPosition(osg::Vec3(ea.getX()-50.f , 0.f , 0.f));
					//cout<<ea.getX()<<endl;
					//slide = ((int)((barPor-50.f)*2)/100)*10;
					return true;

				//	switch(slide)
				//	{
				//	case 0:
				//		{
				//			root->setValue(0 , true);
				//			root->setValue(1 , false);
				//			root->setValue(2 , false);
				//			root->setValue(3 , false);
				//			root->setValue(4 , false);
				//			root->setValue(5 , false);
				//			root->setValue(6 , false);
				//			root->setValue(7 , false);
				//			root->setValue(8 , false);
				//			root->setValue(9 , false);
				//			root->setValue(10 , false);
				//			root->setValue(11 , true);
				//			_HUD->setSceneText("原始格网");
				//			//root->setValue(12 , true);
				//			//break;
				//			return true;
				//			
				//		}
				//	case 10:
				//		{
				//			root->setValue(0 , false);
				//		    root->setValue(1 , true);
				//		    root->setValue(2 , false);
				//		    root->setValue(3 , false);
				//		    root->setValue(4 , false);
				//		    root->setValue(5 , false);
				//		    root->setValue(6 , false);
				//		    root->setValue(7 , false);
				//		    root->setValue(8 , false);
				//		    root->setValue(9 , false);
				//		    root->setValue(10 , false);
				//			root->setValue(11 , true);
				//			
				//			_HUD->setSceneText("化简100次");
				//			//break;
				//			return true;
				//		}
				//	case 20:
				//		{
				//			root->setValue(0 , false);
				//		    root->setValue(1 , false);
				//		    root->setValue(2 , true);
				//		    root->setValue(3 , false);
				//		    root->setValue(4 , false);
				//		    root->setValue(5 , false);
				//		    root->setValue(6 , false);
				//		    root->setValue(7 , false);
				//		    root->setValue(8 , false);
				//		    root->setValue(9 , false);
				//		    root->setValue(10 , false);
				//			root->setValue(11 , true);
				//			
				//			_HUD->setSceneText("化简200次");
				//			//break;
				//			return true;
				//		
				//		}
				//	case 30:
				//		{
				//			root->setValue(0 , false);
				//		    root->setValue(1 , false);
				//		    root->setValue(2 , false);
				//		    root->setValue(3 , true);
				//		    root->setValue(4 , false);
				//		    root->setValue(5 , false);
				//		    root->setValue(6 , false);
				//		    root->setValue(7 , false);
				//		    root->setValue(8 , false);
				//		    root->setValue(9 , false);
				//		    root->setValue(10 , false);
				//			root->setValue(11, true);
				//			_HUD->setSceneText("化简300次");
				//			
				//			//break;
				//			return true;
				//		
				//		}
				//	case 40:
				//		{
				//			root->setValue(0 , false);
				//		    root->setValue(1 , false);
				//		    root->setValue(2 , false);
				//		    root->setValue(3 , false);
				//		    root->setValue(4 , true);
				//		    root->setValue(5 , false);
				//		    root->setValue(6 , false);
				//		    root->setValue(7 , false);
				//		    root->setValue(8 , false);
				//		    root->setValue(9 , false);
				//		    root->setValue(10 , false);
				//			root->setValue(11 , true);
				//			
				//			_HUD->setSceneText("化简400次");
				//			//break;
				//			return true;
				//		
				//		}
				//	case 50:
				//		{
				//			root->setValue(0 , false);
				//		    root->setValue(1 , false);
				//		    root->setValue(2 , false);
				//		    root->setValue(3 , false);
				//		    root->setValue(4 , false);
				//		    root->setValue(5 , true);
				//		    root->setValue(6 , false);
				//		    root->setValue(7 , false);
				//		    root->setValue(8 , false);
				//		    root->setValue(9 , false);
				//		    root->setValue(10 , false);
				//			root->setValue(11 , true);
				//			
				//			_HUD->setSceneText("化简500次");
				//			//break;
				//			return true;
				//			
				//		}
				//	case 60:
				//		{
				//			root->setValue(0 , false);
				//		    root->setValue(1 , false);
				//		    root->setValue(2 , false);
				//		    root->setValue(3 , false);
				//		    root->setValue(4 , false);
				//		    root->setValue(5 , false);
				//		    root->setValue(6 , true);
				//		    root->setValue(7 , false);
				//		    root->setValue(8 , false);
				//		    root->setValue(9 , false);
				//		    root->setValue(10 , false);
				//			root->setValue(11 , true);
				//			
				//			_HUD->setSceneText("化简600次");
				//			//break;
				//			return true;
				//			
				//		}
				//	case 70:
				//		{
				//			root->setValue(0 , false);
				//		    root->setValue(1 , false);
				//		    root->setValue(2 , false);
				//		    root->setValue(3 , false);
				//		    root->setValue(4 , false);
				//		    root->setValue(5 , false);
				//		    root->setValue(6 , false);
				//		    root->setValue(7 , true);
				//		    root->setValue(8 , false);
				//		    root->setValue(9 , false);
				//		    root->setValue(10 , false);
				//			root->setValue(11 , true);
				//			
				//			_HUD->setSceneText("化简700次");
				//			//break;
				//			return true;
				//		
				//		}
				//	case 80:
				//		{
				//			root->setValue(0 , false);
				//		    root->setValue(1 , false);
				//		    root->setValue(2 , false);
				//		    root->setValue(3 , false);
				//		    root->setValue(4 , false);
				//		    root->setValue(5 , false);
				//		    root->setValue(6 , false);
				//		    root->setValue(7 , false);
				//		    root->setValue(8 , true);
				//		    root->setValue(9 , false);
				//		    root->setValue(10 , false);
				//			root->setValue(11 , true);
				//			
				//			_HUD->setSceneText("化简800次");
				//			//break;
				//			return true;
				//			
				//		}
				//	case 90:
				//		{
				//			root->setValue(0 , false);
				//		    root->setValue(1 , false);
				//		    root->setValue(2 , false);
				//		    root->setValue(3 , false);
				//		    root->setValue(4 , false);
				//		    root->setValue(5 , false);
				//		    root->setValue(6 , false);
				//		    root->setValue(7 , false);
				//		    root->setValue(8 , false);
				//		    root->setValue(9 , true);
				//		    root->setValue(10 , false);
				//			root->setValue(11 , true);
				//			_HUD->setSceneText("化简900次");
				//			
				//			//break;
				//			return true;
				//		
				//		}
				//	case 100:
				//		{
				//			root->setValue(0 , false);
				//		    root->setValue(1 , false);
				//		    root->setValue(2 , false);
				//		    root->setValue(3 , false);
				//		    root->setValue(4 , false);
				//		    root->setValue(5 , false);
				//		    root->setValue(6 , false);
				//		    root->setValue(7 , false);
				//		    root->setValue(8 , false);
				//		    root->setValue(9 , false);
				//		    root->setValue(10 , true);
				//			root->setValue(11 , true);
				//			_HUD->setSceneText("化简1000次");
				//			
				//			//break;
				//			return true;

				//			
				//		}
				//	default:return true;



				//	


				//	//return true;
				//}
				}
				else
				{
					return false;
				}
			}
		case osgGA::GUIEventAdapter::RELEASE:
			{
				ifDrag = false;
				return false;
			}
		default:break;

		}

		return false;
	}

	void Pick(float x , float y , osgViewer::Viewer* viewer)
	{
		osgUtil::LineSegmentIntersector::Intersections intersections;
		if(viewer->computeIntersections(x , y , intersections))
		{
			for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();hitr != intersections.end();++hitr)
			{
				osg::NodePath np = hitr->nodePath;
				for(int i = np.size() - 1 ; i >= 0 ; --i)
				{
					if(np[i]->getName() == "coordinateSwitch")
					{
						//_HUD->setIndexLineSwtichOn();
						_HUD->setSlideBallSwtichOn();
						//_HUD->setIndexLinePosition(osg::Vec3(x-50.f , 0.f , 0.f));
						_HUD->setSlideBallPosition(osg::Vec3(x-50.f , 0.f , 0.f));
					}

				}
			}
		}
	}
protected:
	//osgViewer::Viewer& _viewer;
	osg::ref_ptr<HUD> _HUD;
	//VECTORMYPOINT* _rPoint;

};

class ZoomManipulator : public osgGA::CameraManipulator
{
public:
	//构造函数传入节点计算包围盒 
	ZoomManipulator(osg::Node *node)
	{
		_root = node;
		computeHomePosition();

		_eye = _homeEye;
		_direction = _homeCenter - _homeEye;
		_up = _homeUp;
		distancePor = int(sqrt((_eye.x() - centerP.getX())*(_eye.x() - centerP.getX()) + (_eye.y() - centerP.getY())*(_eye.y() - centerP.getY()) + (_eye.z() - centerP.getZ())*(_eye.z() - centerP.getZ())));
		distanceOri = int(sqrt((_eye.x() - centerP.getX())*(_eye.x() - centerP.getX()) + (_eye.y() - centerP.getY())*(_eye.y() - centerP.getY()) + (_eye.z() - centerP.getZ())*(_eye.z() - centerP.getZ())));
		

	}
	//所有漫游器都必须实现的4个纯虚函数  
    virtual void setByMatrix(const osg::Matrixd& matrix){}  
    virtual void setByInverseMatrix(const osg::Matrixd& matrix){}  
    virtual osg::Matrixd getMatrix() const{return osg::Matrix();}  
    virtual osg::Matrixd getInverseMatrix() const
	{
		osg::Matrix mat;
		mat.makeLookAt(_eye , _eye + _direction , _up);
		return mat;
	}

	//获取传入节点，用于使用CameraManipulator中的computeHomePosition  
    virtual const osg::Node* getNode() const { return _root; }  
    virtual osg::Node* getNode() { return _root;  } 

	bool ZoomManipulator::handle(const osgGA::GUIEventAdapter& ea , osgGA::GUIActionAdapter& aa)
	{
		osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		osg::Switch* root = dynamic_cast<osg::Switch*>(viewer->getSceneData());
		osg::Geometry* geom = dynamic_cast<osg::Geometry*>(root->getChild(0)->asGroup()->getChild(0)->asGeode()->getDrawable(0));

		if(!viewer) return false;
		/*if(!root) return false;
		if(!geom) return false;*/

		osg::Vec3Array* v = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
		osg::Vec3Array* nc = dynamic_cast<osg::Vec3Array*>(geom->getNormalArray());
		osg::Vec2Array* texcoords = dynamic_cast<osg::Vec2Array*>(geom->getTexCoordArray(0));
		switch(ea.getEventType())
		{
		case(osgGA::GUIEventAdapter::SCROLL):
			{
				//osgViewer::Viewer * viewer = dynamic_cast<osgViewer::Viewer*>(&us);
				/*osg::Camera * camera = viewer->getCamera();
				osg::Matrix MVPW = camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix();
				osg::Matrix inverseMVPW = osg::Matrix::inverse(MVPW);
				osg::Vec3 mouseWorld = osg::Vec3(ea.getX(), ea.getY(), 0) * inverseMVPW;
				osg::Vec3 direction = mouseWorld - _eye;  
				direction.normalize(); */

				//osgViewer::Viewer * viewer = dynamic_cast<osgViewer::Viewer*>(&us);
				osg::Camera * camera = viewer->getCamera();
				//为了获取任一时刻相机的位置，需要首先将漫游器关闭，然后获取相机参数，获取参数后再次将漫游器添加上去。
				osg::Vec3d eye,center,up;
 
				//关闭漫游器
				//view->setCameraManipulator(NULL);
 
				//获取相机参数
				camera->getViewMatrixAsLookAt(eye,center,up);


				/****************************************
				以part1（0号子块）作为切换依据进行视距切换，屏幕长度为2像素，并在对应SVO要求下求得对应的视距，
				同时求解part2，part3，part4屏幕像素为2时的空间长度SVO阈值
				****************************************/

				//首先模拟part1的SVO向量，其与屏幕平行，与视线方向垂直
				double part1X = 0.25;//x值,假设为0.25
				double part1Y;
				double part1Z;

				double centerPart1X = 562323.375;//part1子块的中点X坐标
				double centerPart1Y = 1825901.499992;
				double centerPart1Z = 10.391985;

				double x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4,x5,y5,z5,x6,y6,z6;//一共6个状态的xyz坐标，分别对应6个SVO下的模拟点的坐标
				double part1SVO = 0.420002298;//SVO
				double part2SVO = 0.500051157;
				double part3SVO = 0.600016641;
				double part4SVO = 0.70003962;
				double part5SVO = 0.800771278;
				double part6SVO = 0.978359331;

				double x1Child1, y1Child1, z1Child1;
				double x1Child2, y1Child2, z1Child2;
				double x1Child3, y1Child3, z1Child3;
				double x1Child4, y1Child4, z1Child4;
				double x1Child5, y1Child5, z1Child5;
				double x1Child6, y1Child6, z1Child6;
				double x1Child7, y1Child7, z1Child7;

				double part1SVOChild1 = 0.427232478;
				double part1SVOChild2 = 0.435868593;
				double part1SVOChild3 = 0.4450462;
				double part1SVOChild4 = 0.454840664;
				double part1SVOChild5 = 0.465391964;
				double part1SVOChild6 = 0.476792659;
				double part1SVOChild7 = 0.488895234;

				double x2Child1, y2Child1, z2Child1; 
				double x2Child2, y2Child2, z2Child2;
				double x2Child3, y2Child3, z2Child3;
				double x2Child4, y2Child4, z2Child4;

				double part2SVOChild1 = 0.516339007;
				double part2SVOChild2 = 0.534488073;
				double part2SVOChild3 = 0.55215918;
				double part2SVOChild4 = 0.570992469;

				double x3Child1, y3Child1, z3Child1;
				double x3Child2, y3Child2, z3Child2;
				double x3Child3, y3Child3, z3Child3;

				double part3SVOChild1 = 0.615145739;
				double part3SVOChild2 = 0.641560582;
				double part3SVOChild3 = 0.672253598;
				
				double x5Child1, y5Child1, z5Child1;
				double part5SVOChild1 = 0.859066214;





				//实现方向，与SVO模拟线段垂直
				double directionX = eye.x() - center.x();
				double directionY = eye.y() - center.y();
				double directionZ = eye.z() - center.z();

				//partSVO1
				double dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part1SVO,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x1 = centerPart1X - part1X;
				y1 = centerPart1Y - part1Y;
				z1 = centerPart1Z - part1Z;

				//part1SVOPart1
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part1SVOChild1,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x1Child1 = centerPart1X - part1X;
				y1Child1 = centerPart1Y - part1Y;
				z1Child1 = centerPart1Z - part1Z;

				//part1SVOPart2
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part1SVOChild2,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x1Child2 = centerPart1X - part1X;
				y1Child2 = centerPart1Y - part1Y;
				z1Child2 = centerPart1Z - part1Z;

				//part1SVOPart3
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part1SVOChild3,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x1Child3 = centerPart1X - part1X;
				y1Child3 = centerPart1Y - part1Y;
				z1Child3 = centerPart1Z - part1Z;

				//part1SVOPart4
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part1SVOChild4,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x1Child4 = centerPart1X - part1X;
				y1Child4 = centerPart1Y - part1Y;
				z1Child4 = centerPart1Z - part1Z;

				//part1SVOPart5
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part1SVOChild5,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x1Child5 = centerPart1X - part1X;
				y1Child5 = centerPart1Y - part1Y;
				z1Child5 = centerPart1Z - part1Z;

				//part1SVOPart6
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part1SVOChild6,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x1Child6 = centerPart1X - part1X;
				y1Child6 = centerPart1Y - part1Y;
				z1Child6 = centerPart1Z - part1Z;

				//part1SVOPart7
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part1SVOChild7,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x1Child7 = centerPart1X - part1X;
				y1Child7 = centerPart1Y - part1Y;
				z1Child7 = centerPart1Z - part1Z;


				//partSVO2
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part2SVO,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x2 = centerPart1X - part1X;
				y2 = centerPart1Y - part1Y;
				z2 = centerPart1Z - part1Z;

				//part2SVOPart1
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part2SVOChild1,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x2Child1 = centerPart1X - part1X;
				y2Child1 = centerPart1Y - part1Y;
				z2Child1 = centerPart1Z - part1Z;

				//part2SVOPart2
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part2SVOChild2,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x2Child2 = centerPart1X - part1X;
				y2Child2 = centerPart1Y - part1Y;
				z2Child2 = centerPart1Z - part1Z;

				//part2SVOPart3
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part2SVOChild3,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x2Child3 = centerPart1X - part1X;
				y2Child3 = centerPart1Y - part1Y;
				z2Child3 = centerPart1Z - part1Z;

				//part2SVOPart4
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part2SVOChild4,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x2Child4 = centerPart1X - part1X;
				y2Child4 = centerPart1Y - part1Y;
				z2Child4 = centerPart1Z - part1Z;


				//partSVO3
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part3SVO,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x3 = centerPart1X - part1X;
				y3 = centerPart1Y - part1Y;
				z3 = centerPart1Z - part1Z;

				//part3SVOPart1
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part3SVOChild1,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x3Child1 = centerPart1X - part1X;
				y3Child1 = centerPart1Y - part1Y;
				z3Child1 = centerPart1Z - part1Z;

				//part3SVOPart2
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part3SVOChild2,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x3Child2 = centerPart1X - part1X;
				y3Child2 = centerPart1Y - part1Y;
				z3Child2 = centerPart1Z - part1Z;

				//part3SVOPart3
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part3SVOChild3,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x3Child3 = centerPart1X - part1X;
				y3Child3 = centerPart1Y - part1Y;
				z3Child3 = centerPart1Z - part1Z;

				//partSVO4
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part4SVO,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x4 = centerPart1X - part1X;
				y4 = centerPart1Y - part1Y;
				z4 = centerPart1Z - part1Z;

				//partSVO5
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part5SVO,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x5 = centerPart1X - part1X;
				y5 = centerPart1Y - part1Y;
				z5 = centerPart1Z - part1Z;

				//part5SVOPart1
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part5SVOChild1,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x5Child1 = centerPart1X - part1X;
				y5Child1 = centerPart1Y - part1Y;
				z5Child1 = centerPart1Z - part1Z;

				//partSVO6
				dierta = 4*pow(directionX,2)*pow(directionZ,2)*pow(part1X,2) - 4*(pow(directionZ,2)+pow(directionY,2))*(pow(part1X,2)*pow(directionX,2) + pow(part1X,2)*pow(directionY,2)-pow(part6SVO,2)*pow(directionY,2));//b^2-4ac
				part1Z = (-2*directionX*directionZ*part1X + sqrt(dierta))/(2*(pow(directionZ,2)+pow(directionY,2)));//求解
				part1Y = (-directionZ*part1Z - part1X*directionX)/directionY;

				x6 = centerPart1X - part1X;
				y6 = centerPart1Y - part1Y;
				z6 = centerPart1Z - part1Z;

				osg::Vec3 pointScreen1SVO1 = worldToScreen(osg::Vec3(centerPart1X, centerPart1Y, centerPart1Z), viewer);//求出子块中心点在屏幕上的位置
				osg::Vec3 pointScreen2SVO1 = worldToScreen(osg::Vec3(x1, y1, z1), viewer);//求得点在屏幕中的位置
				double lengthSVO1 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2SVO1.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2SVO1.y()), 2));//屏幕中的长度

				osg::Vec3 pointScreen2Child1SVO1 = worldToScreen(osg::Vec3(x1Child1, y1Child1, z1Child1), viewer);//求得点在屏幕中的位置
				double lengthChild1SVO1 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child1SVO1.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child1SVO1.y()), 2));//屏幕中的长度

				osg::Vec3 pointScreen2Child2SVO1 = worldToScreen(osg::Vec3(x1Child2, y1Child2, z1Child2), viewer);//求得点在屏幕中的位置
				double lengthChild2SVO1 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child2SVO1.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child2SVO1.y()), 2));//屏幕中的长度

				osg::Vec3 pointScreen2Child3SVO1 = worldToScreen(osg::Vec3(x1Child3, y1Child3, z1Child3), viewer);//求得点在屏幕中的位置
				double lengthChild3SVO1 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child3SVO1.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child3SVO1.y()), 2));//屏幕中的长度

				osg::Vec3 pointScreen2Child4SVO1 = worldToScreen(osg::Vec3(x1Child4, y1Child4, z1Child4), viewer);//求得点在屏幕中的位置
				double lengthChild4SVO1 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child4SVO1.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child4SVO1.y()), 2));//屏幕中的长度

				osg::Vec3 pointScreen2Child5SVO1 = worldToScreen(osg::Vec3(x1Child5, y1Child5, z1Child5), viewer);//求得点在屏幕中的位置
				double lengthChild5SVO1 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child5SVO1.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child5SVO1.y()), 2));//屏幕中的长度

				osg::Vec3 pointScreen2Child6SVO1 = worldToScreen(osg::Vec3(x1Child6, y1Child6, z1Child6), viewer);//求得点在屏幕中的位置
				double lengthChild6SVO1 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child6SVO1.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child6SVO1.y()), 2));//屏幕中的长度

				osg::Vec3 pointScreen2Child7SVO1 = worldToScreen(osg::Vec3(x1Child7, y1Child7, z1Child7), viewer);//求得点在屏幕中的位置
				double lengthChild7SVO1 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child7SVO1.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child7SVO1.y()), 2));//屏幕中的长度

				//osg::Vec3 pointScreen1SVO2 = worldToScreen(osg::Vec3(centerPart1X, centerPart1Y, centerPart1Z), viewer);
				osg::Vec3 pointScreen2SVO2 = worldToScreen(osg::Vec3(x2, y2, z2), viewer);
				double lengthSVO2 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2SVO2.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2SVO2.y()), 2));

				osg::Vec3 pointScreen2Child1SVO2 = worldToScreen(osg::Vec3(x2Child1, y2Child1, z2Child1), viewer);
				double lengthChild1SVO2 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child1SVO2.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child1SVO2.y()), 2));

				osg::Vec3 pointScreen2Child2SVO2 = worldToScreen(osg::Vec3(x2Child2, y2Child2, z2Child2), viewer);
				double lengthChild2SVO2 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child2SVO2.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child2SVO2.y()), 2));

				osg::Vec3 pointScreen2Child3SVO2 = worldToScreen(osg::Vec3(x2Child3, y2Child3, z2Child3), viewer);
				double lengthChild3SVO2 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child3SVO2.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child3SVO2.y()), 2));

				osg::Vec3 pointScreen2Child4SVO2 = worldToScreen(osg::Vec3(x2Child4, y2Child4, z2Child4), viewer);
				double lengthChild4SVO2 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child4SVO2.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child4SVO2.y()), 2));

				//osg::Vec3 pointScreen1SVO3 = worldToScreen(osg::Vec3(centerPart1X, centerPart1Y, centerPart1Z), viewer);
				osg::Vec3 pointScreen2SVO3 = worldToScreen(osg::Vec3(x3, y3, z3), viewer);
				double lengthSVO3 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2SVO3.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2SVO3.y()), 2));

				osg::Vec3 pointScreen2Child1SVO3 = worldToScreen(osg::Vec3(x3Child1, y3Child1, z3Child1), viewer);
				double lengthChild1SVO3 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child1SVO3.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child1SVO3.y()), 2));

				osg::Vec3 pointScreen2Child2SVO3 = worldToScreen(osg::Vec3(x3Child2, y3Child2, z3Child2), viewer);
				double lengthChild2SVO3 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child2SVO3.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child2SVO3.y()), 2));

				osg::Vec3 pointScreen2Child3SVO3 = worldToScreen(osg::Vec3(x3Child3, y3Child3, z3Child3), viewer);
				double lengthChild3SVO3 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child3SVO3.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child3SVO3.y()), 2));

				osg::Vec3 pointScreen2SVO4 = worldToScreen(osg::Vec3(x4, y4, z4), viewer);
				double lengthSVO4 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2SVO4.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2SVO4.y()), 2));

				osg::Vec3 pointScreen2SVO5 = worldToScreen(osg::Vec3(x5, y5, z5), viewer);
				double lengthSVO5 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2SVO5.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2SVO5.y()), 2));

				osg::Vec3 pointScreen2Child1SVO5 = worldToScreen(osg::Vec3(x5Child1, y5Child1, z5Child1), viewer);
				double lengthChild1SVO5 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2Child1SVO5.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2Child1SVO5.y()), 2));

				osg::Vec3 pointScreen2SVO6 = worldToScreen(osg::Vec3(x6, y6, z6), viewer);
				double lengthSVO6 = sqrt(pow((pointScreen1SVO1.x() - pointScreen2SVO6.x()), 2) + pow((pointScreen1SVO1.y() - pointScreen2SVO6.y()), 2));

				double distance = sqrt(pow((eye.x() - centerPart1X), 2) + pow((eye.y() - centerPart1Y), 2) + pow((eye.z() - centerPart1Z), 2));

				//开始求解其它三个子块的视距和SVO阈值
				double centerPart2X = 562684.110168;//part2
				double centerPart2Y = 1825895.070976;
				double centerPart2Z = 32.589771;

				double centerPart3X = 562316.625;//part3
				double centerPart3Y = 1825476.5;
				double centerPart3Z = 12.176299;

				double centerPart4X = 562693.611816;//part4
				double centerPart4Y = 1825506.522644;
				double centerPart4Z = 38.713871;

				//Part2
				osg::Vec3 pointScreenCenter2 = worldToScreen(osg::Vec3(centerPart2X, centerPart2Y, centerPart2Z), viewer);//part2的center在屏幕中的位置
				osg::Vec3 pointScreenPoint2;//模拟的点在屏幕中的位置，其和center的长度为2像素
				pointScreenPoint2.x() = pointScreenCenter2.x();
				pointScreenPoint2.y() = pointScreenCenter2.y() + 2;
				pointScreenPoint2.z() = pointScreenCenter2.z();

				osg::Vec3 pointWorldPart2 = screenToWorld(pointScreenPoint2, viewer);

				//Part3
				osg::Vec3 pointScreenCenter3 = worldToScreen(osg::Vec3(centerPart3X, centerPart3Y, centerPart3Z), viewer);//part3的center在屏幕中的位置
				osg::Vec3 pointScreenPoint3;//模拟的点在屏幕中的位置，其和center的长度为2像素
				pointScreenPoint3.x() = pointScreenCenter3.x();
				pointScreenPoint3.y() = pointScreenCenter3.y() + 2;
				pointScreenPoint3.z() = pointScreenCenter3.z();

				osg::Vec3 pointWorldPart3 = screenToWorld(pointScreenPoint3, viewer);

				//Part4
				osg::Vec3 pointScreenCenter4 = worldToScreen(osg::Vec3(centerPart4X, centerPart4Y, centerPart4Z), viewer);//part3的center在屏幕中的位置
				osg::Vec3 pointScreenPoint4;//模拟的点在屏幕中的位置，其和center的长度为2像素
				pointScreenPoint4.x() = pointScreenCenter4.x();
				pointScreenPoint4.y() = pointScreenCenter4.y() + 2;
				pointScreenPoint4.z() = pointScreenCenter4.z();

				osg::Vec3 pointWorldPart4 = screenToWorld(pointScreenPoint4, viewer);


				








				



				if(ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP)
				{
					//_eye -= direction * 1.0;
					//unsigned int distanceNow = int(sqrt((_eye.x() - centerP.getX())*(_eye.x() - centerP.getX()) + (_eye.y() - centerP.getY())*(_eye.y() - centerP.getY()) + (_eye.z() - centerP.getZ())*(_eye.z() - centerP.getZ())));
		
					////cout<<getstring(_eye.x())<<","<<getstring(_eye.y())<<","<<getstring(_eye.z())<<endl;
					//cout<<getstring(distancePor)<<endl;
					//cout<<getstring(distanceNow)<<endl;
					
					/*if(distancePor>=distanceOri)
					{
					for(unsigned int i = distancePor - distanceOri ; i < distanceNow - distanceOri ; i++)
						{
							unsigned int changeTriSize = changeDataL->at(i).changeTri->size();
							for(unsigned int j = 0 ; j < changeTriSize ; j++)
							{
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));

								v->insert(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex) )*3) , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p3).getX() , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p3).getY() , rPoint->at(changeDataL->at(i).changeTri->at(j).p3).getZ()));
								v->insert(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex) )*3) , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p2).getX() , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p2).getY() , rPoint->at(changeDataL->at(i).changeTri->at(j).p2).getZ()));
								v->insert(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex) )*3) , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p1).getX() , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p1).getY() , rPoint->at(changeDataL->at(i).changeTri->at(j).p1).getZ()));

								nc->erase(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);

								nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p3).normal.X , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p3).normal.Y , rPoint->at(changeDataL->at(i).changeTri->at(j).p3).normal.Z));
								nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p2).normal.X , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p2).normal.Y , rPoint->at(changeDataL->at(i).changeTri->at(j).p2).normal.Z));
								nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).changeTri->at(j).p1).normal.X , 
									rPoint->at(changeDataL->at(i).changeTri->at(j).p1).normal.Y , rPoint->at(changeDataL->at(i).changeTri->at(j).p1).normal.Z));

								texcoords->erase(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);

								texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).changeTri->at(j).p3).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).changeTri->at(j).p3).getY() - rPoint->at(minY).getY())/(ySize)));
								texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).changeTri->at(j).p2).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).changeTri->at(j).p2).getY() - rPoint->at(minY).getY())/(ySize)));
								texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).changeTri->at(j).p1).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).changeTri->at(j).p1).getY() - rPoint->at(minY).getY())/(ySize)));


								
							}
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex )*3));
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex )*3));
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex )*3));
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex )*3));
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex )*3));
							v->erase(v->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex )*3));

							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));
							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));
							nc->erase(nc->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));

							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize).newIndex)*3));
							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));
							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));
							texcoords->erase(texcoords->begin() + ((changeNewData->at(i).changeTriIndex->at(changeTriSize + 1).newIndex)*3));

							triNum -= 2;

						}
					v->dirty();
					nc->dirty();
					geom->setPrimitiveSet(0 , new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES , 0 , 3 * triNum));
					distancePor = distanceNow;
					}*/
				}
				else if(ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_DOWN)
				{
					/*_eye += direction * 1.f;
					unsigned int distanceNow = int(sqrt((_eye.x() - centerP.getX())*(_eye.x() - centerP.getX()) + (_eye.y() - centerP.getY())*(_eye.y() - centerP.getY()) + (_eye.z() - centerP.getZ())*(_eye.z() - centerP.getZ())));
					*///cout<<getstring(distanceNow)<<endl;
					/*if(distanceNow>=distanceOri)
					{
					for(unsigned int i = ((distancePor - distanceOri) - 1) ; i >= (distanceNow - distanceOri); i--)
						{
							int changeTriSizeR = changeDataL->at(i).changeTri->size();

							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).getZ()));
							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).getZ()));
							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).getZ()));
							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).getZ()));
							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).getZ()));
							v->insert(v->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).getX() , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).getY() , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).getZ()));

							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).normal.Z));
							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).normal.Z));
							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).normal.Z));
							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).normal.Z));
							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).normal.Z));
							nc->insert(nc->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).normal.X , 
								rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).normal.Y , rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).normal.Z));



							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p3).getY() - rPoint->at(minY).getY())/(ySize)));
							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p2).getY() - rPoint->at(minY).getY())/(ySize)));
							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR + 1).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(1).p1).getY() - rPoint->at(minY).getY())/(ySize)));
							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p3).getY() - rPoint->at(minY).getY())/(ySize)));
							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p2).getY() - rPoint->at(minY).getY())/(ySize)));
							texcoords->insert(texcoords->begin() + (changeNewData->at(i).changeTriIndex->at(changeTriSizeR).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).getX() - rPoint->at(minX).getX())/(xSize) , 
								(rPoint->at(changeDataL->at(i).noActiveTri->at(0).p1).getY() - rPoint->at(minY).getY())/(ySize)));

							
							for(int j = (changeTriSizeR - 1); j >=0 ; j--)
							{
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));
								v->erase(v->begin() + (((changeNewData->at(i).changeTriIndex->at(j).newIndex))*3));


								v->insert(v->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p3).getX() , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p3).getY() , rPoint->at(changeDataL->at(i).originTri->at(j).p3).getZ()));
								v->insert(v->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p2).getX() , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p2).getY() , rPoint->at(changeDataL->at(i).originTri->at(j).p2).getZ()));
								v->insert(v->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p1).getX() , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p1).getY() , rPoint->at(changeDataL->at(i).originTri->at(j).p1).getZ()));

								nc->erase(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								nc->erase(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);

								
								nc->insert(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p3).normal.X , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p3).normal.Y , rPoint->at(changeDataL->at(i).originTri->at(j).p3).normal.Z));
								nc->insert(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p2).normal.X , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p2).normal.Y , rPoint->at(changeDataL->at(i).originTri->at(j).p2).normal.Z));
								nc->insert(nc->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec3(rPoint->at(changeDataL->at(i).originTri->at(j).p1).normal.X , 
									rPoint->at(changeDataL->at(i).originTri->at(j).p1).normal.Y , rPoint->at(changeDataL->at(i).originTri->at(j).p1).normal.Z));


								texcoords->erase(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);
								texcoords->erase(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3);


								texcoords->insert(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).originTri->at(j).p3).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).originTri->at(j).p3).getY() - rPoint->at(minY).getY())/(ySize)));
								texcoords->insert(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).originTri->at(j).p2).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).originTri->at(j).p2).getY() - rPoint->at(minY).getY())/(ySize)));
								texcoords->insert(texcoords->begin()+(changeNewData->at(i).changeTriIndex->at(j).newIndex)*3 , osg::Vec2((rPoint->at(changeDataL->at(i).originTri->at(j).p1).getX() - rPoint->at(minX).getX())/(xSize) , 
									(rPoint->at(changeDataL->at(i).originTri->at(j).p1).getY() - rPoint->at(minY).getY())/(ySize)));

								

							}

							
							
							
							triNum += 2;

						}
					v->dirty();
					nc->dirty();
					geom->setPrimitiveSet(0 , new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES , 0 , 3 * triNum));
					distancePor = distanceNow;
					}*/
				}

					
				
				
			}
		default:
			return false;
		}
	}





public:
	osg::Vec3 _eye; //视点位置
	osg::Vec3 _direction; //视点方向
	osg::Vec3 _up; //向上方向
	osg::Node* _root;
};

//几何体更新遍历，重载函数update().当几何体更新时，其函数被调用。可以参考  
//include/osgUtil/UpdateVisitor, handle_geode_callback()中的调用过程。  
//struct DrawableUpdateCallback : public osg::Drawable::UpdateCallback  
//{  
//    virtual void update(osg::NodeVisitor*, osg::Drawable* drawable)  
//    {  
//        //std::cout<<"Drawable update callback "<<drawable<<std::endl;  
//    }  
//};  





//VECTORDOUBLE* DisRect = new VECTORDOUBLE;//每个点的距离数组




static VECTORMYTRI* TriFile = new VECTORMYTRI;//三角形文件
static VECTORMYTRI* TriFile1 = new VECTORMYTRI;//用于改变的三角形文件
int newPoint;//新添加的边界点
bool ifhave;//是否需要向外添加边界点
bool ifhave1;//是否需要向内添加边界点
static VECTORPOINTRAGE* pointRage;//每一个点范围
static VECTORSAMEPOINTTRI* samePointT = new VECTORSAMEPOINTTRI;//每一个点对应的三角形
static VECTORSAMEPOINTTRI* samePointT1 = new VECTORSAMEPOINTTRI;//用于改变的三角形信息

static VECTORPOINTRAGE* sonPartPointRage0 = new VECTORPOINTRAGE;//子块0点集各个点的搜索范围
static VECTORPOINTRAGE* sonPartPointRage1 = new VECTORPOINTRAGE;
static VECTORPOINTRAGE* sonPartPointRage2 = new VECTORPOINTRAGE;
static VECTORPOINTRAGE* sonPartPointRage3 = new VECTORPOINTRAGE;

static VECTORSAMEPOINTTRI* samePointAboutTri0 = new VECTORSAMEPOINTTRI;//子块0中每个点对应的相邻三角形，编号与点集编号对应
static VECTORSAMEPOINTTRI* samePointAboutTri1 = new VECTORSAMEPOINTTRI;
static VECTORSAMEPOINTTRI* samePointAboutTri2 = new VECTORSAMEPOINTTRI;
static VECTORSAMEPOINTTRI* samePointAboutTri3 = new VECTORSAMEPOINTTRI;





static VECTORMYTRI* changeTriLV1 = new VECTORMYTRI;//第一阶段改变的三角形
static VECTORMYPOINT* changePLV1 = new VECTORMYPOINT;//第一阶段改变的点
static VECTORINT* deletePoint = new VECTORINT;//删除的点

static VECTORMYPOINT* changeSonPartPLV0 = new VECTORMYPOINT;
static VECTORMYPOINT* changeSonPartPLV1 = new VECTORMYPOINT;
static VECTORMYPOINT* changeSonPartPLV2 = new VECTORMYPOINT;
static VECTORMYPOINT* changeSonPartPLV3 = new VECTORMYPOINT;



/***************
删除的点
****************/
static VECTORINT* deleteSonPoint0 = new VECTORINT;
static VECTORINT* deleteSonPoint1 = new VECTORINT;
static VECTORINT* deleteSonPoint2 = new VECTORINT;
static VECTORINT* deleteSonPoint3 = new VECTORINT;

//static osg::ref_ptr<osg::StateSet> statesetAll = new osg::StateSet();//光照全局模型
//static osg::ref_ptr<osg::Vec4> position = new osg::Vec4;



void DealTra(MyLine pMyLine ,VECTORMYPOINT* rPoint);//申明DealTra
void AddLineRule(int p1, int p2 , VECTORMYPOINT* rPoint);//申明
int jugleLeftOrRight(MyPoint p1 , MyPoint p2 , MyPoint p);
double GetVerDis(MyPoint p,MyPoint p1,MyPoint p2);//声明计算点到直线距离
double Get3dDis(MyPoint p, MyPoint p1);//计算两点的距离
//LISTMYPOINT pointFile;//点文件
void convexRefine(VECTORMYPOINT* rPoint , double dMax , VECTORPOINTRAGE* pointRage , VECTORINT* convexPoint);//边界添点
double getMaxDistance(VECTORINT* convexP2 , VECTORMYPOINT* rPoint);//求边界最大距离
VECTORPOINTRAGE* getPointRage(VECTORMYPOINT* rPoint);
bool ifExistcos(int x ,VECTORINT* result);
void DealTriangle(MyLine pMyLine , VECTORMYPOINT* rPoint);
JudgeLine* DelDelaunay(MyLine pMyLine , VECTORMYPOINT* rPoint);
VECTORMYTRI* getSonPartSamePointTri(VECTORMYTRI* triSonPart , int p);
void changeDataSonPartSave(VECTORMYPOINT* pointPart , VECTORMYPOINT* pointPartQem , int index);
VECTORINT* getSonPartRelatePoint(int p , int index);

VECTORINT* getRelatedPoint(int p);//获得p点相关联的点
void setSonPartQEM(int p , int index);//求最小二次误差
VECTORMYPOINT* qemSort(VECTORMYPOINT* pointPart);
void saveSonPartChangeData(int index);
void saveSonPartError(int index);
void setSonQemData(int index);



int getMinYpoint(VECTORMYPOINT* sonPointPart)//获得Y值最小点的索引,改返回值为位置顺序编号，而非index
{
	double minY = sonPointPart->at(0).getY();
	int minYindex = 0;
	unsigned int sonPointPartSize = sonPointPart->size();
	unsigned i = 0;
	for(i; i < sonPointPartSize ; i++)
	{
		if(sonPointPart->at(i).getY() < minY)
		{
			minY = sonPointPart->at(i).getY();
			minYindex = i;
		}
		else
		{
			continue;
		}
	}
	return minYindex;
}

VECTORINT* getMinYindex(VECTORMYPOINT* sonPointPart)//获得所有Y值最小的点的索引
{
	VECTORINT* MinYindex = new VECTORINT;
	//double minY = rPoint->at(0).getY();
	int minYindex = 0;
	unsigned sonPointPartSize = sonPointPart->size();
	minYindex = getMinYpoint(sonPointPart);
	MinYindex->push_back(minYindex);
	unsigned int i = minYindex + 1;
	for(i; i < sonPointPartSize ; i++)
	{
		if(sonPointPart->at(i).getY() == sonPointPart->at(minYindex).getY())
		{
			MinYindex->push_back(i);
		}
		else
		{
			continue;
		}
	}
	return MinYindex;
}

int getMaxYpoint(VECTORMYPOINT* sonPartPoint)//获得Y值最大点的索引
{
	double maxY = sonPartPoint->at(0).getY();
	int maxYindex = 0;
	unsigned int sonPartPointSize = sonPartPoint->size();
	unsigned i = 0;
	for(i; i < sonPartPointSize ; i++)
	{
		if(sonPartPoint->at(i).getY() > maxY)
		{
			maxY = sonPartPoint->at(i).getY();
			maxYindex = i;
		}
		else
		{
			continue;
		}
	}
	return maxYindex;
}

VECTORINT* getMaxYindex(VECTORMYPOINT* sonPointPart)//获得所有Y值最大的点的索引
{
	VECTORINT* MaxYindex = new VECTORINT;
	//double minY = rPoint->at(0).getY();
	int maxYindex = 0;
	unsigned sonPointPartSize = sonPointPart->size();
	maxYindex = getMaxYpoint(sonPointPart);
	MaxYindex->push_back(maxYindex);
	unsigned int i = maxYindex + 1;
	for(i ; i < sonPointPartSize ; i++)
	{
		if(sonPointPart->at(i).getY() == sonPointPart->at(maxYindex).getY())
		{
			MaxYindex->push_back(i);
		}
		else
		{
			continue;
		}
	}
	return MaxYindex;
}

int getMinXpoint(VECTORMYPOINT* sonPartPoint)//获得X值最小点的索引
{
	double minX = sonPartPoint->at(0).getX();
	int minXindex = 0;
	unsigned int sonPartPointSize = sonPartPoint->size();
	unsigned i = 0;
	for(i; i < sonPartPointSize ; i++)
	{
		if(sonPartPoint->at(i).getX() < minX)
		{
			minX = sonPartPoint->at(i).getX();
			minXindex = i;
		}
		else
		{
			continue;
		}
	}
	return minXindex;
}

VECTORINT* getMinXindex(VECTORMYPOINT* sonPointPart)//获得所有X值最小的点的索引
{
	VECTORINT* MinXindex = new VECTORINT;
	//double minY = rPoint->at(0).getY();
	int minXindex = 0;
	unsigned sonPointPartSize = sonPointPart->size();
	minXindex = getMinXpoint(sonPointPart);
	MinXindex->push_back(minXindex);
	unsigned int i = minXindex + 1;
	for(i ; i < sonPointPartSize ; i++)
	{
		if(sonPointPart->at(i).getX() == sonPointPart->at(minXindex).getX())
		{
			MinXindex->push_back(i);
		}
		else
		{
			continue;
		}
	}
	return MinXindex;
}

int getMaxXpoint(VECTORMYPOINT* sonPartPoint)//获得X值最大点的索引
{
	double maxX = sonPartPoint->at(0).getX();
	int maxXindex = 0;
	unsigned int sonPartPointSize = sonPartPoint->size();
	unsigned i = 0;
	for(i; i < sonPartPointSize ; i++)
	{
		if(sonPartPoint->at(i).getX() > maxX)
		{
			maxX = sonPartPoint->at(i).getX();
			maxXindex = i;
		}
		else
		{
			continue;
		}
	}
	return maxXindex;
}

VECTORINT* getMaxXindex(VECTORMYPOINT* sonPointPart)//获得所有X值最大的点的索引
{
	VECTORINT* MaxXindex = new VECTORINT;
	int maxXindex = 0;
	unsigned sonPointPartSize = sonPointPart->size();
	maxXindex = getMaxXpoint(sonPointPart);
	MaxXindex->push_back(maxXindex);
	unsigned int i = maxXindex + 1;
	for(i ; i < sonPointPartSize ; i++)
	{
		if(sonPointPart->at(i).getX() == sonPointPart->at(maxXindex).getX())
		{
			MaxXindex->push_back(i);
		}
		else
		{
			continue;
		}
	}
	return MaxXindex;
}

VECTORINT* getRightPoint(VECTORMYPOINT* sonPartPoint , int p1 , int p2)//获得p1p2线段右边的所有点的索引，返回值为位置顺序编号，而非index
{
	VECTORINT* rightPointindex = new VECTORINT;
	unsigned int sonPartPointsize = sonPartPoint->size();
	for(unsigned int i = 0 ; i < sonPartPointsize ; i++)
	{
		if(jugleLeftOrRight(sonPartPoint->at(p1) , sonPartPoint->at(p2) , sonPartPoint->at(i)) != -1)
		{
			rightPointindex->push_back(i);
		}
		else
		{
			continue;
		}
	}
	return rightPointindex;
}

/******************
convexHave为目前的边界点数组
convexAll为上一次的所有边界点数组
***************/

int getFarthestPoint(int p1 , int p2 , VECTORMYPOINT* sonPartPoint , VECTORINT* convexHave , VECTORINT* convexAll)//获得最远点索引
{
	VECTORINT* rightPointindex;
	rightPointindex = getRightPoint(sonPartPoint , p1 , p2);
	int fartherPoint = -1;
	double fartherdistance = 0;
	double distance = 0;
	unsigned rightPointindexsize = rightPointindex->size();
	for(unsigned int i = 0 ; i < rightPointindexsize ; i++)
	{
		
		distance = GetVerDis(sonPartPoint->at(rightPointindex->at(i)) , sonPartPoint->at(p1) , sonPartPoint->at(p2));
		if(fartherdistance < distance)
		{
			if(!ifExistcos(rightPointindex->at(i) , convexHave) && !ifExistcos(rightPointindex->at(i) , convexAll))
			{
			
				fartherdistance = distance;
				fartherPoint = rightPointindex->at(i);
			}
		

		}
	}
	delete rightPointindex;
	return fartherPoint;
}

void addnewConvex(VECTORMYPOINT* rPoint , VECTORINT* convexPoint)//进行第一轮边界细化，把初始多边形外面的点加入到多边形边界上,通过加入最远点
{
	newPoint = -2;
	VECTORINT* convexP3 = new VECTORINT;
	unsigned convexPointsize = convexPoint->size() - 1;
	convexP3->push_back(convexPoint->at(0));
	ifhave = false;
	for(unsigned int i = 0 ; i < convexPointsize ; i++)
	{
		newPoint = getFarthestPoint(convexPoint->at(i) , convexPoint->at(i+1) , rPoint , convexP3 , convexPoint);
		if(newPoint!=-1)
		{
			ifhave = true;
			convexP3->push_back(newPoint);//如果有点可以加入则加入
			convexP3->push_back(convexPoint->at(i+1));
			
		}
		else
		{
			convexP3->push_back(convexPoint->at(i+1));//没有点可以加入则压入后面原本的边界点
			
		}
	}
	convexP1->clear();//全局边界点
	convexP1->assign(convexP3->begin(), convexP3->end());
	if(ifhave)
	{
		addnewConvex(rPoint , convexP3);//一直递归直到每条边的右侧都没有可加入的点为止
		delete convexP3;
	}
	else
	{
		delete convexP3;
		return;
	}
	
}

void getConvexIndex(VECTORMYPOINT* rPoint)
{

	VECTORINT* convexPoint = new VECTORINT;
	VECTORINT* MinYindex;
	VECTORINT* MaxYindex;
	VECTORINT* MinXindex;
	VECTORINT* MaxXindex;
	/*VECTORINT* zeroP = new VECTORINT;*/
	double dMax;
	
	pointRage = getPointRage(rPoint);
	/*for(unsigned int i = 0 ; i < pointRage->size() ; i++)
	{
		if(pointRage->at(i).getRageSize() == 0)
		{
			zeroP->push_back(pointRage->at(i).getPointIndex());
		}
	}*/



	MinYindex = getMinYindex(rPoint);
	MaxYindex = getMaxYindex(rPoint);
	MinXindex = getMinXindex(rPoint);
	MaxXindex = getMaxXindex(rPoint);
	convexPoint->insert(convexPoint->end() , MinYindex->begin() , MinYindex->end());
	convexPoint->insert(convexPoint->end() , MaxXindex->begin() , MaxXindex->end());
	convexPoint->insert(convexPoint->end() , MaxYindex->begin() , MaxYindex->end());
	convexPoint->insert(convexPoint->end() , MinXindex->begin() , MinXindex->end());//获得初始边界点
	convexPoint->push_back(MinYindex->at(0));//把第一个点压入最后
	addnewConvex(rPoint , convexPoint);
	dMax = getMaxDistance(convexP1 , rPoint);
	convexPoint->clear();
	convexPoint->assign(convexP1->begin(), convexP1->end());
	convexP2->assign(convexP1->begin() , convexP1->end());
	convexRefine(rPoint , dMax , pointRage , convexPoint);
	delete MinYindex;
	delete MaxYindex;
	delete MinXindex;
	delete MaxXindex;
	delete convexPoint;
	return;
	
	
}

double getMaxDistance(VECTORINT* convexFinal , VECTORMYPOINT* sonPointPart)//求边界最大距离,如果大于dMax，则向内查找角度最大的点进行细化
{
    int i, j;
    double area = 0;
	double P = 0;//密度
	double d = 0;//离散点的平均距离
	double dMax = 0;//边界的最大距离
	int N = convexFinal->size() - 1;
	for (i = 0; i<N; i++) 
	{
        j = (i + 1) % N;
		area += (sonPointPart->at(convexFinal->at(i)).getX()) * (sonPointPart->at(convexFinal->at(j)).getY());
		area -= (sonPointPart->at(convexFinal->at(i)).getY()) * (sonPointPart->at(convexFinal->at(j)).getX());
        
    }
    area /= 2;
	P = (sonPointPart->size())/(abs(area));
	d = 1/(sqrt(P));
	dMax = 2*d;
    return dMax;
}

VECTORDOUBLE* getConvexPointDis(VECTORMYPOINT* sonPointPart , VECTORINT* convexPoint)//多边形的边界点距离
{
	VECTORDOUBLE* convexPointDis = new VECTORDOUBLE;
	unsigned int convexP2size = convexPoint->size() - 1;
	for(unsigned i = 0 ; i < convexP2size ; i++)
	{
		convexPointDis->push_back(Get3dDis(sonPointPart->at(convexPoint->at(i)) , sonPointPart->at(convexPoint->at(i + 1))));

	}
	return convexPointDis;
}

double getAllMaxDistance(VECTORMYPOINT* sonPointPart)//首先获得每个点与其他点的最小值集合，再挑出最大值
{
	unsigned sonPointPartsize = sonPointPart->size();
	double minDis = Get3dDis(sonPointPart->at(0) , sonPointPart->at(1));
	double maxDis = 0;
	double distance = 0;
	VECTORDOUBLE* minDisV = new VECTORDOUBLE; 

	for(unsigned int i = 0 ; i < sonPointPartsize ; i++)
	{
		for(unsigned int j = 0 ; j < sonPointPartsize ; j++)
		{
			if(j != i)
			{
				distance = Get3dDis(sonPointPart->at(i) , sonPointPart->at(j));
				if(distance < minDis)
				{
					minDis = distance;
				}
			}
		}
		minDisV->push_back(minDis);
	}

	for(unsigned int k = 0 ; k < minDisV->size() ; k++)
	{
		if(minDisV->at(k) > maxDis)
		{
			maxDis = minDisV->at(k);
		}
	}
	delete minDisV;
	/*unsigned int i;
	unsigned int j;
	for(i = 0 ; i < rPointsize - 1 ; i++)
	{
		for(j = i + 1 ; j < rPointsize ; j++)
		{
			distance = Get3dDis(rPoint->at(i) , rPoint->at(j));
			if(distance < minDis)
			{
				minDis = distance;
			}
			else
			{
				continue;
			}
		}

		if(maxDis < minDis)
		{
			maxDis = minDis;
		}
	}*/
	return maxDis;
}

VECTORPOINTRAGE* getPointRage(VECTORMYPOINT* sonPointPart)//获得每一个点一定距离范围内的点
{
	unsigned int sonPointParttsize = sonPointPart->size();
	PointRage* pointR;
	//PointRage pointR1;
	VECTORPOINTRAGE* pointRage = new VECTORPOINTRAGE;
	double maxDis = getAllMaxDistance(sonPointPart);
	double distance = 0;
	for(unsigned i = 0 ; i < sonPointParttsize ; i++)
	{
		pointR = new PointRage;
		pointR->clear();
		
		pointR->setPointIndex(i);
		for( unsigned j = 0 ; j < sonPointParttsize ; j++)
		{
			
			distance = Get3dDis(sonPointPart->at(i) , sonPointPart->at(j));
			if(distance <= 7 * maxDis && i!=j)
			{
				pointR->insertRageIndex(j);
			}
			else
			{
				continue;
			}
		}
		
		pointRage->push_back(*pointR);
		delete[] pointR;
	}
	return pointRage;
}

double getMidpointDis(MyPoint p1 , MyPoint p2 , MyPoint p3)
{
	double x = (p1.getX() + p2.getX())/2;
	double y = (p1.getY() + p2.getY())/2;
	double dis = (x - p3.getX())*(x - p3.getX())+(y - p3.getY())*(y - p3.getY());
	return sqrt(dis);
}

double getAngle(MyPoint p1 , MyPoint p2 , MyPoint p3)//角度
{
	double P3 , P2 , P1 , cos3 , angle;

	double pp3 = (p2.getX() - p1.getX())*(p2.getX() - p1.getX())+(p2.getY() - p1.getY())*(p2.getY() - p1.getY());
	double pp1 = (p3.getX() - p2.getX())*(p3.getX() - p2.getX())+(p3.getY() - p2.getY())*(p3.getY() - p2.getY());
	double pp2 = (p1.getX() - p3.getX())*(p1.getX() - p3.getX())+(p1.getY() - p3.getY())*(p1.getY() - p3.getY());

	P3 = sqrt(pp3);
	P1 = sqrt(pp1);
	P2 = sqrt(pp2);


	/*P3 = sqrt((p2.getX() - p1.getX())*(p2.getX() - p1.getX())+(p2.getY() - p1.getY())*(p2.getY() - p1.getY()));
	P1 = sqrt((p3.getX() - p2.getX())*(p3.getX() - p2.getX())+(p3.getY() - p2.getY())*(p3.getY() - p2.getY()));
	P2 = sqrt((p1.getX() - p3.getX())*(p1.getX() - p3.getX())+(p1.getY() - p3.getY())*(p1.getY() - p3.getY()));*/
	if(P1!=0&&P2!=0)
	{
		cos3 = (P1*P1 + P2*P2 - P3*P3)/(2 * P1 * P2);
		angle = acos(cos3);
	}
	else
	{
		angle = -1;
	}

	return angle;
	
}

int getLeftMinIndexR(int p1 , int p2 , VECTORMYPOINT* sonPointPart , VECTORINT* convexPoint , VECTORINT* convexPointBackups)//查找两点之间左边角度最大的点，这里弃用范围搜索
{
	double Lagangle = 0;
	int min = -1;
	unsigned int sonPointPartSize = sonPointPart->size();
	for(unsigned int i = 0 ; i < sonPointPartSize ; i++)
	{
		if(jugleLeftOrRight(sonPointPart->at(p1) , sonPointPart->at(p2) , sonPointPart->at(i)) == -1&&!ifExistcos(i , convexPoint)&&!ifExistcos(i , convexPointBackups))
		{
			double ang = getAngle(sonPointPart->at(p1) , sonPointPart->at(p2) , sonPointPart->at(i));
			if(Lagangle < ang)
			{
				Lagangle = ang;
				min = i;
			}
		}
	}
	if(min!=-1)
	{
		return min;
	}
	else
	{
		return -1;
	}
}

int getLeftMinIndex(VECTORINT* Point , int p1 , int p2 , VECTORMYPOINT* rPoint , VECTORINT* convexPoint)//Point是搜索范围内的点,找到该范围内组成角度最大的点
{
	
	double Lagangle = 0;

	
	int min = -1;
	unsigned Pointsize = Point->size();
	for(unsigned i = 0 ; i < Pointsize ; i++)
	{
		if(jugleLeftOrRight(rPoint->at(p1) , rPoint->at(p2) , rPoint->at(Point->at(i))) == -1&&!ifExistcos(Point->at(i) , convexPoint))
		{
			if(Lagangle < getAngle(rPoint->at(p1) , rPoint->at(p2) , rPoint->at(Point->at(i))))
			{
				Lagangle = getAngle(rPoint->at(p1) , rPoint->at(p2) , rPoint->at(Point->at(i)));
				min = i;
			}
			else
			{
				continue;
			}
		}
		else
		{
			continue;
		}
	}
	
	if(min!=-1)
	{
		return Point->at(min);
	}
	else
	{
		return -1;
	}
	

}


void convexRefine(VECTORMYPOINT* rPoint , double dMax , VECTORPOINTRAGE* pointRage , VECTORINT* convexPoint)//边界添点
{
	VECTORDOUBLE* convexPointDis;
	VECTORINT* convexPoint2 = new VECTORINT;
	VECTORINT* pointRagei1 = new VECTORINT;
	VECTORINT* pointRagei2 = new VECTORINT;
	VECTORINT* pointRagei = new VECTORINT;
	ifhave1 = false;
	int min = -1;
	convexPointDis = getConvexPointDis(rPoint , convexPoint);//边界线的长度集合
	unsigned int convexPointsize = convexPoint->size() - 1;
	convexPoint2->push_back(convexPoint->at(0));//备份边界点
	for(unsigned i = 0 ; i < convexPointsize ; i++)
	{
		//分别获得线段两端点搜索范围内的点，并合并
		/*pointRagei1->assign(pointRage->at(convexPoint->at(i)).getAllRageIndex()->begin() , pointRage->at(convexPoint->at(i)).getAllRageIndex()->end());
		pointRagei2->assign(pointRage->at(convexPoint->at(i + 1)).getAllRageIndex()->begin() , pointRage->at(convexPoint->at(i + 1)).getAllRageIndex()->end());
		
		pointRagei->clear();
		pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
		pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());*/
		if(convexPointDis->at(i) > dMax)//当大于标准值时，添加角度最大的点
		{
			ifhave1 = true;
			min = getLeftMinIndexR(convexPoint->at(i) , convexPoint->at(i+1) , rPoint , convexPoint , convexPoint2);
			if(min!=-1)
			{
				convexPoint2->push_back(min);
				convexPoint2->push_back(convexPoint->at(i+1));
				
				
			}
			else
			{
				convexPoint2->push_back(convexPoint->at(i+1));
				
			}

		}
		else
		{
			convexPoint2->push_back(convexPoint->at(i+1));//否则只添加对应下一个端点
			
		}

			
		/*pointRagei1->clear();
		pointRagei2->clear();*/

	}
	convexP1->clear();
	convexP1->assign(convexPoint2->begin(), convexPoint2->end());//全局更新边界点
	if(ifhave1)
	{
		convexRefine(rPoint , dMax , pointRage , convexPoint2);//进行递归直到所有线段都小于标准值为止
		delete convexPointDis;
		delete convexPoint2;
		delete pointRagei1;
		delete pointRagei2;
		delete pointRagei;
	}
	else
	{
		delete convexPointDis;
		delete convexPoint2;
		delete pointRagei1;
		delete pointRagei2;
		delete pointRagei;
		return;
	}
	
}

//double getCos(MyPoint p1 , MyPoint p2)//获得cos值
//{
//	double cosAng = -2;
//	if(p1.getX()==p2.getX()&&p1.getY()==p2.getY()&&p1.getZ()==p2.getZ())
//	{
//	
//		
//		return cosAng;
//	}
//	else
//	{
//		cosAng = (p2.getX() - p1.getX())/(sqrt((p2.getX() - p1.getX())*(p2.getX() - p1.getX()) + (p2.getY() - p1.getY())*(p2.getY() - p1.getY())));
//
//		return cosAng;
//	}
//
//}

bool comp(const double &a,const double &b)
{
    return a>b;
}

bool comS(const double &a , const double &b)
{
	return a<b;
}

bool ifExistcos(int x ,VECTORINT* result)
{
	VECTORINT::iterator it;
		//it=find(SearPoint->begin(),SearPoint->end(),x);
		it = find(result->begin() , result->end() , x);
		if (it!=result->end())
		{
		//vec中存在value值
			return true;
		}
		else
		{
		//vec中不存在value值
			return false;
		}
}

//VECTORINT* GetCosIndex(VECTORDOUBLE* CosAngle)//得到垂直cos索引的分布
//{
//    VECTORINT* result = new VECTORINT;
//    VECTORDOUBLE* temp = new  VECTORDOUBLE;
//	//VECTORDOUBLE* temp1 = new VECTORDOUBLE; 
//	unsigned int CosAnglelength = CosAngle->size();
//	for (unsigned int i = 0; i < CosAnglelength; i++)
//    {
//        //temp.Add(VerTi[i]); 
//		temp->push_back(CosAngle->at(i));
//    }
//	sort(temp->begin() , temp->end() , comp);
//	unsigned int templength = temp->size();
//	for (unsigned int i = 0; i < templength; i++)
//    {
//		//if(temp->at(i)==-1)continue;
//		for (unsigned int j = 0; j <CosAnglelength; j++)
//        {
//			if (temp->at(i) == CosAngle->at(j))
//            { 
//				
//				if(!ifExistcos(j , result))
//				{
//					result->push_back(j);
//					break;
//				}
//				else
//				{
//					continue;
//				}
//            }
//        
//        
//        }
//    }
//    return result;
//    
//}


//这个问题的判定可以通过向量叉积来解决  
//设要判断点P(x,y) 是在线段AB的哪侧  A（x1，y1） B（x2，y2）的那个方向  
//建造两个向量PA  与PB  则 PA（x1-x，y1-y） PB（x2 -x，y2-y）  
//则二者叉乘  x1y2 - x2y1 如果小于0在左边 大于0在右边等于0 在直线上  
int jugleLeftOrRight(MyPoint p1 , MyPoint p2 , MyPoint p)
{
	double pAx , pAy , pBx , pBy , crossXY;
	int LeftOrRight = 0;
	pAx = p1.getX() - p.getX();
	pAy = p1.getY() - p.getY();
	pBx = p2.getX() - p.getX();
	pBy = p2.getY() - p.getY();
	crossXY = pAx*pBy - pBx*pAy;
	if(crossXY > 0)
	{
		LeftOrRight = -1;//在左边
	}
	if(crossXY < 0)
	{
		LeftOrRight = 1;//在右边 
	}
	else
	{
		return LeftOrRight;//在线上
	}
	return LeftOrRight;
}

double Get3dDis(MyPoint p, MyPoint p1)//计算两点的距离
{
    double result=0;
	/*double resultXY = (p.getX() - p1.getX()) * (p.getX() - p1.getX()) + (p.getY() - p1.getY()) * (p.getY() - p1.getY());
	result = sqrt(resultXY + (p.getZ() - p1.getZ())*(p.getZ() - p1.getZ()));*/

	result = sqrt((p.getX() - p1.getX()) * (p.getX() - p1.getX()) + (p.getY() - p1.getY()) * (p.getY() - p1.getY()));

    return result;

        
        
}

//int JudgeX(MyPoint point1, MyPoint point2)// 判断哪一个点在左边 -1 代表 point1.X小
//{
//
//	if (point1.getX() < point2.getX())
//    {
//        return -1;
//    }
//	else if(point1.getX() > point2.getX())
//    {
//
//        return 1;
//    
//    }
//	else
//	{
//		return 0;
//	}
//
//
//}

int GetLineIndex(MyLine pMyLine)//在点文件中找到线的索引
{
    int index = -1;
	unsigned int lineFilelength = lineFile->size();
	for (unsigned int i = 0; i < lineFilelength; i++)
    {
		if (pMyLine.LeftPoint == lineFile->at(i).LeftPoint && pMyLine.RightPoint == lineFile->at(i).RightPoint)
        {
            index = i;
            break;
        }
    
    
    }
    return index;


}

VECTORINT* GetRmvPoint(int pMyLine)//返回形成三角形的另外一个点
{
    VECTORINT* RmvPoint = new VECTORINT;
	unsigned int TriFilelength = TriFile->size();
	for (unsigned int i = 0; i < TriFilelength; i++)
    {
		if (TriFile->at(i).ExistLine(lineFile,pMyLine) != -1) {

			RmvPoint->push_back(TriFile->at(i).ExistLine(lineFile, pMyLine));
        
        
        }
    
    
    }


    return RmvPoint;
}

bool ifExistInt(int x ,VECTORINT* RmvPoint)
{
	VECTORINT::iterator it;
		//it=find(SearPoint->begin(),SearPoint->end(),x);
		it = find(RmvPoint->begin() , RmvPoint->end() , x);
		if (it!=RmvPoint->end())
		{
		//vec中存在value值
			return true;
		}
		else
		{
		//vec中不存在value值
			return false;
		}
}

double GetVerDis(MyPoint p,MyPoint p1,MyPoint p2)//计算点到直线距离
{
 

    //计算点到直线的距离
    double A, B, C;
	double d = 0;
	A = p1.getY() -p2.getY();
	B = p2.getX() - p1.getX();
	C = p1.getX() * p2.getY() - p2.getX() * p1.getY();
	d = abs(p.getX() * A + p.getY() * B + C) /sqrt(A * A + B * B);
    return d;




}

MyPoint GetCircumCenter(MyPoint point1 , MyPoint point2 , MyPoint point3)//求圆心坐标,都是在xy坐标轴内进行计算
{
	
            MyPoint CircumCenter;
            double a, b, c;
            a = Get3dDis(point2, point3);//2,3
            b = Get3dDis(point1, point3);//1,3
            c = Get3dDis(point2, point1);//1,2

            double A, B, C;

        
            A = acos((b * b + c * c - a * a) / (2 * b * c));
            B = acos((a * a + c * c - b * b) / (2 * a * c));
            C = acos((b * b + a * a - c * c) / (2 * b * a));

            CircumCenter.setX((point1.getX() * sin(2 * A) + point2.getX() * sin(2 * B) + point3.getX() * sin(2 * C)) / (sin(2 * A) + sin(2 * B) + sin(2 * C)));
		    CircumCenter.setY((point1.getY() * sin(2 * A) + point2.getY() * sin(2 * B) + point3.getY() * sin(2 * C)) / (sin(2 * A) + sin(2 * B) + sin(2 * C)));
			CircumCenter.setZ(0);
            return CircumCenter;

}

double GetR(MyPoint Center, MyPoint myPoint)//求圆的半径
{
    double r = 0;
    r = Get3dDis(Center, myPoint);
    return r;
    
}

bool JudgeRule(double r, MyPoint Center, VECTORINT* RmvPoint , VECTORMYPOINT* rPoint , VECTORINT* pointRagei , MyLine pMyLine , int p)//是否符合空外接圆准则
{
    bool result = true;
    bool Exist;
	unsigned int pointRageisize = pointRagei->size();
	for (unsigned int i = 0; i < pointRageisize; i++)
    {
        Exist = false;
		for (unsigned int j = 0; j < RmvPoint->size(); j++)//该边所形成的上一个三角形对应点不参加普判断
        {
			if (i == RmvPoint->at(j))
            {
                Exist = true;
                break;

            }           
        
        }
		if (!Exist&&pointRagei->at(i)!=p&&pointRagei->at(i)!=pMyLine.LeftPoint&&pointRagei->at(i)!=pMyLine.RightPoint)//三角形三个顶点都不参加判断，因为三角形上的点计算出来的d和r有一些差异
        {
            double d = Get3dDis(Center, rPoint->at(pointRagei->at(i)));
            if (d < r)
            {
                result = false;
                return result;


            }
            else
            {
                continue;
            
            
            }
        
        }
		else
		{
			continue;
		}
    
    
    }
    return result;
}

MyLine AddLinePoint(int p1,int p2,VECTORMYPOINT* sonPointPart)//形成新的线段
{
	MyLine pMyLine;
	if(sonPointPart->at(p1).getX() < sonPointPart->at(p2).getX())
	{
		pMyLine.LeftPoint = p1;
		pMyLine.RightPoint = p2;
	}
	else if(sonPointPart->at(p1).getX() > sonPointPart->at(p2).getX())
	{
		pMyLine.LeftPoint = p2;
		pMyLine.RightPoint = p1;
	}
	else
	{
		if(sonPointPart->at(p1).getY() < sonPointPart->at(p2).getY())
		{
			pMyLine.LeftPoint = p1;
			pMyLine.RightPoint = p2;
		}
		else
		{
			pMyLine.LeftPoint = p2;
			pMyLine.RightPoint = p1;
		}
	}

    /*if (JudgeX(rPoint->at(p1), rPoint->at(p2)) == -1)
    {
        pMyLine.LeftPoint = p1;
        pMyLine.RightPoint = p2;


    }
    else if(JudgeX(rPoint->at(p1), rPoint->at(p2)) == 1)
    {
        pMyLine.LeftPoint = p2;
        pMyLine.RightPoint = p1;


    }
	else if(JudgeX(rPoint->at(p1), rPoint->at(p2)) == 0)
	{
		if(rPoint->at(p1).getY() < )
	}*/
	return pMyLine;


}

bool ExistTri(VECTORMYTRI* triSonPart, MyTri CreaTri)//判断TriFile中是否已经存在该三角形
{
 
	bool index = false;
	unsigned int triSonPartlength = triSonPart->size();
	for (unsigned int i = 0; i < triSonPartlength; i++)
    {
		if(triSonPart->at(i).p1 == CreaTri.p1 || triSonPart->at(i).p1 == CreaTri.p2 || triSonPart->at(i).p1 == CreaTri.p3)
		{
			if(triSonPart->at(i).p2 == CreaTri.p1 || triSonPart->at(i).p2 == CreaTri.p2 || triSonPart->at(i).p2 == CreaTri.p3)
			{
				if(triSonPart->at(i).p3 == CreaTri.p1 || triSonPart->at(i).p3 == CreaTri.p2 || triSonPart->at(i).p3 == CreaTri.p3)
				{
					index = true;
				}
			}
		}


		/*if (CreaTri.Line1 == TriFile->at(i).Line1 || CreaTri.Line1 == TriFile->at(i).Line2||CreaTri.Line1 == TriFile->at(i).Line3)
        {
            if(CreaTri.Line2 == TriFile->at(i).Line1 || CreaTri.Line2 == TriFile->at(i).Line2||CreaTri.Line2 == TriFile->at(i).Line3)
			{
				if(CreaTri.Line3 == TriFile->at(i).Line1 || CreaTri.Line3 == TriFile->at(i).Line2||CreaTri.Line3 == TriFile->at(i).Line3)
				{
					index = true;
					break;
				}
				else
				{
					continue;
				}
			}
			else
			{
				continue;
			}
        }
		else
		{
			continue;
		}*/
    
    
    }
    return index;
}

bool ExistLine(VECTORMYLINE* lineSonPart, MyLine CreaLine1)//判断lineFile中是否存在该线
{

	bool index = false;
	unsigned int lineSonPartlength = lineSonPart->size();
	for (unsigned int i = 0; i < lineSonPartlength; i++)
    {
		if (CreaLine1.LeftPoint == lineSonPart->at(i).LeftPoint && CreaLine1.RightPoint == lineSonPart->at(i).RightPoint)
        {
            index = true;
            break;
        }
    
    
    }
    return index;
}

void AddTriRule(MyTri CreaTri, int p , VECTORMYPOINT* rPoint)//添加三角形规则
{
    AddLineRule(CreaTri.Line1, p , rPoint);
    AddLineRule(CreaTri.Line2, p , rPoint);
    AddLineRule(CreaTri.Line3, p , rPoint);
   
}

void AddLineRule(int p1, int p2 , VECTORMYPOINT* rPoint)//添加点的三角形拓扑关系p2是三角形索引，p1是线段索引
{
    int index,p1Index,p2Index;
	index=TriFile->at(p2).ExistLine(lineFile, p1);
	p1Index = lineFile->at(p1).LeftPoint;
	p2Index = lineFile->at(p1).RightPoint;
    double A, B, C;
	A = rPoint->at(p1Index).getY() - rPoint->at(p2Index).getY();
	B = rPoint->at(p2Index).getX() - rPoint->at(p1Index).getX();
	C = (rPoint->at(p1Index).getX())*(rPoint->at(p2Index).getY()) - (rPoint->at(p2Index).getX()*rPoint->at(p1Index).getY());
    double panduan;
    panduan = (rPoint->at(index).getX())*A + (rPoint->at(index).getY())*B+C;
    if (panduan > 0)
    {
        lineFile->at(p1).LeftOrTopTri = p2;

    }
    else
    {
        lineFile->at(p1).RightOrBottomTri = p2;
    
    }
   
}

void DealTin(VECTORMYPOINT* rPoint , VECTORINT* convexPoints)//找到边界线中距离最短的边作为起始线
{
	clock_t start , finish;
	//JudgeLine *judge;
	start = clock();
	MyLine pMyLine;
	int minL = 0;
	double distance , distanceMin;
	unsigned convexPointsize = convexPoints->size() - 2;
	unsigned int i = 0;
	for(i; i < convexPointsize ; i++)
	{
		
		lineFile->push_back(AddLinePoint(convexPoints->at(i) , convexPoints->at(i + 1) , rPoint));//先存入边界线到lineFile，等到生成三角形时在存入拓扑关系
		lineFile->at(i).ifHave = true;//边界边可以作为三角形的边，但不作为生长的边
		/*if(lineFile->at(i).LeftPoint == 296||lineFile->at(i).RightPoint == 296)
		{
			cout<<"296"<<endl;
		}*/
	}
	lineFile->push_back(AddLinePoint(convexPoints->at(convexPointsize) , convexPoints->at(0) , rPoint));
	//lineFile->at(i).ifHave = true;
	distanceMin = Get3dDis(rPoint->at(convexPoints->at(0)) , rPoint->at(convexPoints->at(1)));
	for(unsigned int i = 0 ; i < lineFile->size() ; i++)
	{
		distance = Get3dDis(rPoint->at(lineFile->at(i).LeftPoint) , rPoint->at(lineFile->at(i).RightPoint));
		if(distanceMin > distance)
		{
			distanceMin = distance;
			minL = i;
		}
	}
	pMyLine = lineFile->at(minL);
	//lineFile->at(minL).ifHave = false;//起始边应该参与生长
	//judge = new JudgeLine(lineFile->at(minL) , lineFile->at(0));
	DealTriangle(pMyLine , rPoint);

	//while(!judge->ifComplete)
	//{
	//	if(judge->createL1.ifHave&&judge->createL2.ifHave)
	//	{
	//		//unsigned int i = 0;
	//		bool ifAll = true;//是否遍历lineFile中所有线
	//		for(unsigned int i = 0 ; i < lineFile->size() ; i++)
	//		{
	//			if(!lineFile->at(i).ifHave)
	//			{
	//				ifAll = false;
	//				judge = DelDelaunay(lineFile->at(i) , rPoint);
	//				break;
	//			}
	//		}
	//		if(ifAll)
	//		{
	//			judge->ifComplete = true;
	//		}
	//	}
	//	else if(judge->createL1.ifHave&&!judge->createL2.ifHave)
	//	{
	//		judge = DelDelaunay(judge->createL2 , rPoint);
	//	}
	//	else
	//	{
	//		judge = DelDelaunay(judge->createL1 , rPoint);
	//		lineFile->at(minL).ifHave = true;
	//	}
	//}
	finish = clock();
	cout<<"构建Delaunay三角网花费的时间："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;
	
	
     

}

//非递归方式查找边界点
bool addSonPartnewConvex1(VECTORMYPOINT* sonPointPart , int mode)//进行第一轮边界细化，把初始多边形外面的点加入到多边形边界上,通过加入最远点
{
	newPoint = -2;
	VECTORINT* convexTemp = new VECTORINT;//临时边界点
	//VECTORINT* convexFinal = new VECTORINT;//最终边界点返回
	

	ifhave = false;
	
	
	switch(mode)
	{
	case 0:
		{
			unsigned convexPointsize = convexSonPart0->size() - 1;
			convexTemp->push_back(convexSonPart0->at(0));


			for(unsigned int i = 0 ; i < convexPointsize ; i++)
			{
				newPoint = getFarthestPoint(convexSonPart0->at(i) , convexSonPart0->at(i+1) , sonPointPart , convexTemp , convexSonPart0);
				if(newPoint!=-1)
				{
					ifhave = true;
					convexTemp->push_back(newPoint);//如果有点可以加入则加入
					convexTemp->push_back(convexSonPart0->at(i+1));
			
				}
				else
				{
					convexTemp->push_back(convexSonPart0->at(i+1));//没有点可以加入则压入后面原本的边界点
			
				}
			}


			convexSonPart0->clear();
			convexSonPart0->assign(convexTemp->begin() , convexTemp->end());
			delete convexTemp;
			return ifhave;
			break;
		}
	case 1:
		{
			unsigned convexPointsize = convexSonPart1->size() - 1;
			convexTemp->push_back(convexSonPart1->at(0));


			for(unsigned int i = 0 ; i < convexPointsize ; i++)
			{
				newPoint = getFarthestPoint(convexSonPart1->at(i) , convexSonPart1->at(i+1) , sonPointPart , convexTemp , convexSonPart1);
				if(newPoint!=-1)
				{
					ifhave = true;
					convexTemp->push_back(newPoint);//如果有点可以加入则加入
					convexTemp->push_back(convexSonPart1->at(i+1));
			
				}
				else
				{
					convexTemp->push_back(convexSonPart1->at(i+1));//没有点可以加入则压入后面原本的边界点
			
				}
			}

			convexSonPart1->clear();
			convexSonPart1->assign(convexTemp->begin() , convexTemp->end());
			delete convexTemp;
			return ifhave;
			break;
		}
	case 2:
		{
			unsigned convexPointsize = convexSonPart2->size() - 1;
			convexTemp->push_back(convexSonPart2->at(0));


			for(unsigned int i = 0 ; i < convexPointsize ; i++)
			{
				newPoint = getFarthestPoint(convexSonPart2->at(i) , convexSonPart2->at(i+1) , sonPointPart , convexTemp , convexSonPart2);
				if(newPoint!=-1)
				{
					ifhave = true;
					convexTemp->push_back(newPoint);//如果有点可以加入则加入
					convexTemp->push_back(convexSonPart2->at(i+1));
			
				}
				else
				{
					convexTemp->push_back(convexSonPart2->at(i+1));//没有点可以加入则压入后面原本的边界点
			
				}
			}

			convexSonPart2->clear();
			convexSonPart2->assign(convexTemp->begin() , convexTemp->end());
			delete convexTemp;
			return ifhave;
			break;
		}
	case 3:
		{
			unsigned convexPointsize = convexSonPart3->size() - 1;
			convexTemp->push_back(convexSonPart3->at(0));


			for(unsigned int i = 0 ; i < convexPointsize ; i++)
			{
				newPoint = getFarthestPoint(convexSonPart3->at(i) , convexSonPart3->at(i+1) , sonPointPart , convexTemp , convexSonPart3);
				if(newPoint!=-1)
				{
					ifhave = true;
					convexTemp->push_back(newPoint);//如果有点可以加入则加入
					convexTemp->push_back(convexSonPart3->at(i+1));
			
				}
				else
				{
					convexTemp->push_back(convexSonPart3->at(i+1));//没有点可以加入则压入后面原本的边界点
			
				}
			}

			convexSonPart3->clear();
			convexSonPart3->assign(convexTemp->begin() , convexTemp->end());
			delete convexTemp;
			return ifhave;
			break;
		}
	default:
		{
			cout<<"在进行边界点查找是出错，mode = "<< mode << endl;
			delete convexTemp;
			return ifhave;
			break;
		}
	}
	
}

VECTORINT* addSonPartnewConvex(VECTORMYPOINT* sonPointPart , VECTORINT* convexPoint)//进行第一轮边界细化，把初始多边形外面的点加入到多边形边界上,通过加入最远点
{
	newPoint = -2;
	VECTORINT* convexTemp = new VECTORINT;//临时边界点
	VECTORINT* convexFinal = new VECTORINT;//最终边界点返回
	unsigned convexPointsize = convexPoint->size() - 1;
	convexTemp->push_back(convexPoint->at(0));

	ifhave = false;
	for(unsigned int i = 0 ; i < convexPointsize ; i++)
	{
		newPoint = getFarthestPoint(convexPoint->at(i) , convexPoint->at(i+1) , sonPointPart , convexTemp , convexPoint);
		if(newPoint!=-1)
		{
			ifhave = true;
			convexTemp->push_back(newPoint);//如果有点可以加入则加入
			convexTemp->push_back(convexPoint->at(i+1));
			
		}
		else
		{
			convexTemp->push_back(convexPoint->at(i+1));//没有点可以加入则压入后面原本的边界点
			
		}
	}
	//convexP1->clear();//全局边界点
	//convexP1->assign(convexP3->begin(), convexP3->end());
	if(ifhave)
	{
		convexFinal->clear();
		convexFinal = addSonPartnewConvex(sonPointPart , convexTemp);//一直递归直到每条边的右侧都没有可加入的点为止
		delete convexTemp;
		return convexFinal;
	}
	else
	{
		convexFinal->clear();
		convexFinal->assign(convexTemp->begin() , convexTemp->end());
		delete convexTemp;
		return convexFinal;
	}
	
}

bool sonPartConvexRefine1(VECTORMYPOINT* sonPointPart , double dMax ,  int mode)//边界添点
{
	VECTORDOUBLE* convexPointDis;
	VECTORINT* convexPointBackups = new VECTORINT;//临时边界点
	ifhave1 = false;
	int min = -1;

	switch(mode)
	{
	case 0:
		{
			convexPointDis = getConvexPointDis(sonPointPart , convexSonPart0);//边界线的长度集合
			unsigned int convexPointsize = convexSonPart0->size() - 1;
			convexPointBackups->push_back(convexSonPart0->at(0));//备份边界点
			for(unsigned i = 0 ; i < convexPointsize ; i++)
			{
		
				if(convexPointDis->at(i) > dMax)//当大于标准值时，添加角度最大的点
				{
					ifhave1 = true;
					min = getLeftMinIndexR(convexSonPart0->at(i) , convexSonPart0->at(i+1) , sonPointPart , convexSonPart0 , convexPointBackups);//返回的是该子块内位置顺序，而非index
					if(min!=-1)
					{
						convexPointBackups->push_back(min);
						convexPointBackups->push_back(convexSonPart0->at(i+1));
				
				
					}
					else
					{
						convexPointBackups->push_back(convexSonPart0->at(i+1));
				
					}

				}
				else
				{
					convexPointBackups->push_back(convexSonPart0->at(i+1));//否则只添加对应下一个端点
			
				}

			
			

			}
			convexSonPart0->clear();
			convexSonPart0->assign(convexPointBackups->begin() , convexPointBackups->end());
			delete convexPointDis;
			delete convexPointBackups;
			return ifhave1;
			break;
		}
	case 1:
		{
			convexPointDis = getConvexPointDis(sonPointPart , convexSonPart1);//边界线的长度集合
			unsigned int convexPointsize = convexSonPart1->size() - 1;
			convexPointBackups->push_back(convexSonPart1->at(0));//备份边界点
			for(unsigned i = 0 ; i < convexPointsize ; i++)
			{
		
				if(convexPointDis->at(i) > dMax)//当大于标准值时，添加角度最大的点
				{
					ifhave1 = true;
					min = getLeftMinIndexR(convexSonPart1->at(i) , convexSonPart1->at(i+1) , sonPointPart , convexSonPart1 , convexPointBackups);//返回的是该子块内位置顺序，而非index
					if(min!=-1)
					{
						convexPointBackups->push_back(min);
						convexPointBackups->push_back(convexSonPart1->at(i+1));
				
				
					}
					else
					{
						convexPointBackups->push_back(convexSonPart1->at(i+1));
				
					}

				}
				else
				{
					convexPointBackups->push_back(convexSonPart1->at(i+1));//否则只添加对应下一个端点
			
				}

			
			

			}
			convexSonPart1->clear();
			convexSonPart1->assign(convexPointBackups->begin() , convexPointBackups->end());
			delete convexPointDis;
			delete convexPointBackups;
			return ifhave1;
			break;
		}
	case 2:
		{
			convexPointDis = getConvexPointDis(sonPointPart , convexSonPart2);//边界线的长度集合
			unsigned int convexPointsize = convexSonPart2->size() - 1;
			convexPointBackups->push_back(convexSonPart2->at(0));//备份边界点
			for(unsigned i = 0 ; i < convexPointsize ; i++)
			{
		
				if(convexPointDis->at(i) > dMax)//当大于标准值时，添加角度最大的点
				{
					ifhave1 = true;
					min = getLeftMinIndexR(convexSonPart2->at(i) , convexSonPart2->at(i+1) , sonPointPart , convexSonPart2 , convexPointBackups);//返回的是该子块内位置顺序，而非index
					if(min!=-1)
					{
						convexPointBackups->push_back(min);
						convexPointBackups->push_back(convexSonPart2->at(i+1));
				
				
					}
					else
					{
						convexPointBackups->push_back(convexSonPart2->at(i+1));
				
					}

				}
				else
				{
					convexPointBackups->push_back(convexSonPart2->at(i+1));//否则只添加对应下一个端点
			
				}

			
			

			}
			convexSonPart2->clear();
			convexSonPart2->assign(convexPointBackups->begin() , convexPointBackups->end());
			delete convexPointDis;
			delete convexPointBackups;
			return ifhave1;
			break;
		}
	case 3:
		{
			convexPointDis = getConvexPointDis(sonPointPart , convexSonPart3);//边界线的长度集合
			unsigned int convexPointsize = convexSonPart3->size() - 1;
			convexPointBackups->push_back(convexSonPart3->at(0));//备份边界点
			for(unsigned i = 0 ; i < convexPointsize ; i++)
			{
		
				if(convexPointDis->at(i) > dMax)//当大于标准值时，添加角度最大的点
				{
					ifhave1 = true;
					min = getLeftMinIndexR(convexSonPart3->at(i) , convexSonPart3->at(i+1) , sonPointPart , convexSonPart3 , convexPointBackups);//返回的是该子块内位置顺序，而非index
					if(min!=-1)
					{
						convexPointBackups->push_back(min);
						convexPointBackups->push_back(convexSonPart3->at(i+1));
				
				
					}
					else
					{
						convexPointBackups->push_back(convexSonPart3->at(i+1));
				
					}

				}
				else
				{
					convexPointBackups->push_back(convexSonPart3->at(i+1));//否则只添加对应下一个端点
			
				}

			
			

			}
			convexSonPart3->clear();
			convexSonPart3->assign(convexPointBackups->begin() , convexPointBackups->end());
			delete convexPointDis;
			delete convexPointBackups;
			return ifhave1;
			break;
		}
	default:
		{
			cout<<"在进行边界的第二轮细化是出错，mode = "<<mode<<endl;
			return ifhave1;
			break;
		}

	}
	
}

VECTORINT* sonPartConvexRefine(VECTORMYPOINT* sonPointPart , double dMax ,  VECTORINT* convexPoint)//边界添点
{
	VECTORDOUBLE* convexPointDis;
	VECTORINT* convexPointFinal = new VECTORINT;//临时边界点
	VECTORINT* convexPointBackups = new VECTORINT;
	/*VECTORINT* pointRagei1 = new VECTORINT;
	VECTORINT* pointRagei2 = new VECTORINT;
	VECTORINT* pointRagei = new VECTORINT;*/
	ifhave1 = false;
	int min = -1;
	convexPointDis = getConvexPointDis(sonPointPart , convexPoint);//边界线的长度集合
	unsigned int convexPointsize = convexPoint->size() - 1;
	convexPointBackups->push_back(convexPoint->at(0));//备份边界点
	for(unsigned i = 0 ; i < convexPointsize ; i++)
	{
		//分别获得线段两端点搜索范围内的点，并合并
		/*pointRagei1->assign(pointRage->at(convexPoint->at(i)).getAllRageIndex()->begin() , pointRage->at(convexPoint->at(i)).getAllRageIndex()->end());
		pointRagei2->assign(pointRage->at(convexPoint->at(i + 1)).getAllRageIndex()->begin() , pointRage->at(convexPoint->at(i + 1)).getAllRageIndex()->end());
		
		pointRagei->clear();
		pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
		pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());*/
		if(convexPointDis->at(i) > dMax)//当大于标准值时，添加角度最大的点
		{
			ifhave1 = true;
			min = getLeftMinIndexR(convexPoint->at(i) , convexPoint->at(i+1) , sonPointPart , convexPoint , convexPointBackups);//返回的是该子块内位置顺序，而非index
			/*if(ifExistcos(i , convexPoint))
			{
				int i = 0;
			}*/
			if(min!=-1)
			{
				convexPointBackups->push_back(min);
				convexPointBackups->push_back(convexPoint->at(i+1));
				
				
			}
			else
			{
				convexPointBackups->push_back(convexPoint->at(i+1));
				
			}

		}
		else
		{
			convexPointBackups->push_back(convexPoint->at(i+1));//否则只添加对应下一个端点
			
		}

			
		/*pointRagei1->clear();
		pointRagei2->clear();*/

	}
	//convexPointTemp->clear();
	//convexPointTemp->assign(convexPointBackups->begin(), convexPointBackups->end());//全局更新边界点
	if(ifhave1)
	{
		convexPointFinal->clear();
		convexPointFinal = sonPartConvexRefine(sonPointPart , dMax  , convexPointBackups);//进行递归直到所有线段都小于标准值为止
		delete convexPointDis;
		delete convexPointBackups;
		return convexPointFinal;
		/*delete pointRagei1;
		delete pointRagei2;
		delete pointRagei;*/
	}
	else
	{
		convexPointFinal->assign(convexPointBackups->begin() , convexPointBackups->end());
		delete convexPointDis;
		delete convexPointBackups;
		/*delete pointRagei1;
		delete pointRagei2;
		delete pointRagei;*/
		return convexPointFinal;;
	}
	
}

void getSonPointConvexIndex(VECTORMYPOINT* sonPointPart , int sonPartIndex)
{

	//VECTORINT* convexPoint = new VECTORINT;//临时边界点
	//VECTORINT* convexFinal = new VECTORINT;//最终边界点
	//VECTORINT* convexDeleteFinal = new VECTORINT;//最终删减完的边界点，返回值
	/*VECTORINT* MinYindex;
	VECTORINT* MaxYindex;
	VECTORINT* MinXindex;
	VECTORINT* MaxXindex;*/
	/*VECTORINT* zeroP = new VECTORINT;*/
	double dMax;
	
	//pointRage = getPointRage(sonPointPart);
	switch(sonPartIndex)
	{
	case 0:
		{
			sonPartPointRage0 = getPointRage(sonPointPart);
			break;
		}
	case 1:
		{
			sonPartPointRage1 = getPointRage(sonPointPart);
			break;
		}
	case 2:
		{
			sonPartPointRage2 = getPointRage(sonPointPart);
			break;
		}
	case 3:
		{
			sonPartPointRage3 = getPointRage(sonPointPart);
			break;
		}
	default:
		cout<<"子块"<<sonPointPart<<"搜索范围确定时出错"<<endl;
	}


	/*if(sonPartIndex == 3)
	{
		int i = 0;
	}*/

	
	
	bool ifhave = true;
	bool ifhave1 = true;

	switch(sonPartIndex)
	{
	case 0:
		{
			/*MinYindex = getMinYindex(sonPointPart);
			MaxYindex = getMaxYindex(sonPointPart);
			MinXindex = getMinXindex(sonPointPart);
			MaxXindex = getMaxXindex(sonPointPart);*/
			int minYI = getMinYpoint(sonPointPart);
			int maxYI = getMaxYpoint(sonPointPart);
			int minXI = getMinXpoint(sonPointPart);
			int maxXI = getMaxXpoint(sonPointPart);

			//convexSonPart0->insert(convexSonPart0->end() , MinYindex->begin() , MinYindex->end());
			//convexSonPart0->insert(convexSonPart0->end() , MaxXindex->begin() , MaxXindex->end());
			//convexSonPart0->insert(convexSonPart0->end() , MaxYindex->begin() , MaxYindex->end());
			//convexSonPart0->insert(convexSonPart0->end() , MinXindex->begin() , MinXindex->end());//获得初始边界点
			convexSonPart0->push_back(minYI);
			convexSonPart0->push_back(maxXI);
			convexSonPart0->push_back(maxYI);
			convexSonPart0->push_back(minXI);
			convexSonPart0->push_back(minYI);//把第一个点压入最后
			while(ifhave)
			{
				ifhave = addSonPartnewConvex1(sonPointPart , 0);
			}
			dMax = getMaxDistance(convexSonPart0 , sonPointPart);
			while(ifhave1)
			{
				ifhave1 = sonPartConvexRefine1(sonPointPart , dMax , 0);
			}

			/*delete MinYindex;
			delete MaxYindex;
			delete MinXindex;
			delete MaxXindex;*/
			break;

		}
	case 1:
		{
			/*MinYindex = getMinYindex(sonPointPart);
			MaxYindex = getMaxYindex(sonPointPart);
			MinXindex = getMinXindex(sonPointPart);
			MaxXindex = getMaxXindex(sonPointPart);*/
			int minYI = getMinYpoint(sonPointPart);
			int maxYI = getMaxYpoint(sonPointPart);
			int minXI = getMinXpoint(sonPointPart);
			int maxXI = getMaxXpoint(sonPointPart);

			//convexSonPart1->insert(convexSonPart1->end() , MinYindex->begin() , MinYindex->end());
			//convexSonPart1->insert(convexSonPart1->end() , MaxXindex->begin() , MaxXindex->end());
			//convexSonPart1->insert(convexSonPart1->end() , MaxYindex->begin() , MaxYindex->end());
			//convexSonPart1->insert(convexSonPart1->end() , MinXindex->begin() , MinXindex->end());//获得初始边界点
			convexSonPart1->push_back(minYI);
			convexSonPart1->push_back(maxXI);
			convexSonPart1->push_back(maxYI);
			convexSonPart1->push_back(minXI);
			convexSonPart1->push_back(minYI);//把第一个点压入最后
			while(ifhave)
			{
				ifhave = addSonPartnewConvex1(sonPointPart , 1);
			}
			dMax = getMaxDistance(convexSonPart1 , sonPointPart);
			while(ifhave1)
			{
				ifhave1 = sonPartConvexRefine1(sonPointPart , dMax , 1);
			}

			/*delete MinYindex;
			delete MaxYindex;
			delete MinXindex;
			delete MaxXindex;*/
			break;
		}
	case 2:
		{
			/*MinYindex = getMinYindex(sonPointPart);
			MaxYindex = getMaxYindex(sonPointPart);
			MinXindex = getMinXindex(sonPointPart);
			MaxXindex = getMaxXindex(sonPointPart);*/
			int minYI = getMinYpoint(sonPointPart);
			int maxYI = getMaxYpoint(sonPointPart);
			int minXI = getMinXpoint(sonPointPart);
			int maxXI = getMaxXpoint(sonPointPart);

			//convexSonPart2->insert(convexSonPart2->end() , MinYindex->begin() , MinYindex->end());
			//convexSonPart2->insert(convexSonPart2->end() , MaxXindex->begin() , MaxXindex->end());
			//convexSonPart2->insert(convexSonPart2->end() , MaxYindex->begin() , MaxYindex->end());
			//convexSonPart2->insert(convexSonPart2->end() , MinXindex->begin() , MinXindex->end());//获得初始边界点

			convexSonPart2->push_back(minYI);
			convexSonPart2->push_back(maxXI);
			convexSonPart2->push_back(maxYI);
			convexSonPart2->push_back(minXI);
			convexSonPart2->push_back(minYI);//把第一个点压入最后
			while(ifhave)
			{
				ifhave = addSonPartnewConvex1(sonPointPart , 2);
			}
			dMax = getMaxDistance(convexSonPart2 , sonPointPart);
			while(ifhave1)
			{
				ifhave1 = sonPartConvexRefine1(sonPointPart , dMax , 2);
			}

			/*delete MinYindex;
			delete MaxYindex;
			delete MinXindex;
			delete MaxXindex;*/
			break;
		}
	case 3:
		{
			/*MinYindex = getMinYindex(sonPointPart);
			MaxYindex = getMaxYindex(sonPointPart);
			MinXindex = getMinXindex(sonPointPart);
			MaxXindex = getMaxXindex(sonPointPart);*/

			int minYI = getMinYpoint(sonPointPart);
			int maxYI = getMaxYpoint(sonPointPart);
			int minXI = getMinXpoint(sonPointPart);
			int maxXI = getMaxXpoint(sonPointPart);
			//convexSonPart3->insert(convexSonPart3->end() , MinYindex->begin() , MinYindex->end());
			//convexSonPart3->insert(convexSonPart3->end() , MaxXindex->begin() , MaxXindex->end());
			//convexSonPart3->insert(convexSonPart3->end() , MaxYindex->begin() , MaxYindex->end());
			//convexSonPart3->insert(convexSonPart3->end() , MinXindex->begin() , MinXindex->end());//获得初始边界点

			convexSonPart3->push_back(minYI);
			convexSonPart3->push_back(maxXI);
			convexSonPart3->push_back(maxYI);
			convexSonPart3->push_back(minXI);
			convexSonPart3->push_back(minYI);//把第一个点压入最后
			while(ifhave)
			{
				ifhave = addSonPartnewConvex1(sonPointPart , 3);
			}
			dMax = getMaxDistance(convexSonPart3 , sonPointPart);
			while(ifhave1)
			{
				ifhave1 = sonPartConvexRefine1(sonPointPart , dMax , 3);
			}

			/*delete MinYindex;
			delete MaxYindex;
			delete MinXindex;
			delete MaxXindex;*/
			break;
		}
	default:
		{
			cout<<"边界点选取是出错，sonPartIndex = "<< sonPartIndex << endl;
			break;
		}
	}
	//convexPoint->insert(convexPoint->end() , MinYindex->begin() , MinYindex->end());
	//convexPoint->insert(convexPoint->end() , MaxXindex->begin() , MaxXindex->end());
	//convexPoint->insert(convexPoint->end() , MaxYindex->begin() , MaxYindex->end());
	//convexPoint->insert(convexPoint->end() , MinXindex->begin() , MinXindex->end());//获得初始边界点
	//convexPoint->push_back(MinYindex->at(0));//把第一个点压入最后


	
	//delete convexPoint;
	
}

//void *PrintHello(void *threadid)
//{
//	struct unionDouble *tid = ((struct unionDouble*)threadid);
//	VECTORDOUBLE* indexes = tid->indexes1;
//	//cout<<"hello"<<tid<<endl;
//	for(int i = 0 ; i < indexes->size() ; i++)
//	{
//		cout<<"hello"<<indexes->at(i)<<endl;
//	}
//	pthread_exit(NULL);
//	return 0;
//}


//子块中查找线的索引，此处sonPartIndex为子块的编号，所有线中点的索引对应的是子块各自的书序编号并非index属性
int GetSonPartLineIndex(MyLine pMyLine , int sonPartIndex)
{
    int index = -1;


	switch(sonPartIndex)
	{
	case 0:
		{
			unsigned int sonPartLineSize0 = lineSonPart0->size();
			for(unsigned int i = 0 ; i < sonPartLineSize0 ; i++)
			{
				if(pMyLine.LeftPoint == lineSonPart0->at(i).LeftPoint && pMyLine.RightPoint == lineSonPart0->at(i).RightPoint)
				{
					index = i;
					break;
				}
			}
			break;
		}

	case 1:
		{
			unsigned int sonPartLineSize1 = lineSonPart1->size();
			for(unsigned int i = 0 ; i < sonPartLineSize1 ; i++)
			{
				if(pMyLine.LeftPoint == lineSonPart1->at(i).LeftPoint && pMyLine.RightPoint == lineSonPart1->at(i).RightPoint)
				{
					index = i;
					break;
				}
			}
			break;
		}

	case 2:
		{
			unsigned int sonPartLineSize2 = lineSonPart2->size();
			for(unsigned int i = 0 ; i < sonPartLineSize2 ; i++)
			{
				if(pMyLine.LeftPoint == lineSonPart2->at(i).LeftPoint && pMyLine.RightPoint == lineSonPart2->at(i).RightPoint)
				{
					index = i;
					break;
				}
			}
			break;
		}
		
	case 3:
		{
			unsigned int sonPartLineSize3 = lineSonPart3->size();
			for(unsigned int i = 0 ; i < sonPartLineSize3 ; i++)
			{
				if(pMyLine.LeftPoint == lineSonPart3->at(i).LeftPoint && pMyLine.RightPoint == lineSonPart3->at(i).RightPoint)
				{
					index = i;
					break;
				}
			}
			break;
		}
	case 4:
		{
			unsigned int convexZeroToOneLineSize = convexZeroToOneLine->size();
			for(unsigned int i = 0 ; i < convexZeroToOneLineSize ; i++)
			{
				if(pMyLine.LeftPoint == convexZeroToOneLine->at(i).LeftPoint && pMyLine.RightPoint == convexZeroToOneLine->at(i).RightPoint)
				{
					index = i;
					break;
				}
			}
			break;
		}
	case 5:
		{
			unsigned int convexTwoToThreeLineSize = convexTwoToThreeLine->size();
			for(unsigned int i = 0 ; i < convexTwoToThreeLineSize ; i++)
			{
				if(pMyLine.LeftPoint == convexTwoToThreeLine->at(i).LeftPoint && pMyLine.RightPoint == convexTwoToThreeLine->at(i).RightPoint)
				{
					index = i;
					break;
				}
			}
			break;
		}
	case 6:
		{
			unsigned int convexTopToButtomLineSize = convexTopToButtomLine->size();
			for(unsigned int i = 0 ; i < convexTopToButtomLineSize ; i++)
			{
				if(pMyLine.LeftPoint == convexTopToButtomLine->at(i).LeftPoint && pMyLine.RightPoint == convexTopToButtomLine->at(i).RightPoint)
				{
					index = i;
					break;
				}
			}
			break;
		}

	default:
		cout<<"子块"<<sonPartIndex<<"在查找边的索引时出错"<<endl;
		break;
	}


    return index;


}

VECTORINT* GetSonPartRmvPoint(int pMyLine , int sonPartIndex)//返回形成三角形的另外一个点
{
    VECTORINT* RmvPoint = new VECTORINT;

	switch(sonPartIndex)
	{
	case 0:
		{
			unsigned int sonPartTriFileSize = triSonPart0->size();
			for(unsigned int i = 0 ; i < sonPartTriFileSize ; i++)
			{
				int index = triSonPart0->at(i).ExistLine(lineSonPart0 , pMyLine);
				if(index != -1)
				{
					RmvPoint->push_back(index);
				}
			}
			break;
		}

	case 1:
		{
			unsigned int sonPartTriFileSize = triSonPart1->size();
			for(unsigned int i = 0 ; i < sonPartTriFileSize ; i++)
			{
				int index = triSonPart1->at(i).ExistLine(lineSonPart1 , pMyLine);
				if(index != -1)
				{
					RmvPoint->push_back(index);
				}
			}
			break;
		}

	case 2:
		{
			unsigned int sonPartTriFileSize = triSonPart2->size();
			for(unsigned int i = 0 ; i < sonPartTriFileSize ; i++)
			{
				int index = triSonPart2->at(i).ExistLine(lineSonPart2 , pMyLine);
				if(index != -1)
				{
					RmvPoint->push_back(index);
				}
			}
			break;
		}
	case 3:
		{
			unsigned int sonPartTriFileSize = triSonPart3->size();
			for(unsigned int i = 0 ; i < sonPartTriFileSize ; i++)
			{
				int index = triSonPart3->at(i).ExistLine(lineSonPart3 , pMyLine);
				if(index != -1)
				{
					RmvPoint->push_back(index);
				}
			}
			break;
		}
	case 4:
		{
			unsigned int convexZeroToOneTriSize = convexZeroToOneTri->size();
			for(unsigned int i = 0 ; i < convexZeroToOneTriSize ; i++)
			{
				int index = convexZeroToOneTri->at(i).ExistLine(convexZeroToOneLine , pMyLine);
				if(index != -1)
				{
					RmvPoint->push_back(index);
				}
			}
			break;
		}
	case 5:
		{
			unsigned int convexTwoToThreeTriSize = convexTwoToThreeTri->size();
			for(unsigned int i = 0 ; i < convexTwoToThreeTriSize ; i++)
			{
				int index = convexTwoToThreeTri->at(i).ExistLine(convexTwoToThreeLine , pMyLine);
				if(index != -1)
				{
					RmvPoint->push_back(index);
				}
			}
			break;
		}
	case 6:
		{
			unsigned int convexTopToButtomTriSize = convexTopToButtomTri->size();
			for(unsigned int i = 0 ; i < convexTopToButtomTriSize ; i++)
			{
				int index = convexTopToButtomTri->at(i).ExistLine(convexTopToButtomLine , pMyLine);
				if(index != -1)
				{
					RmvPoint->push_back(index);
				}
			}
			break;
		}
	default:
		cout<<"查找子块"<<sonPartIndex<<"线对应三角形的另一个点时出错"<<endl;
		break;

	}
	/*unsigned int TriFilelength = TriFile->size();
	for (unsigned int i = 0; i < TriFilelength; i++)
    {
		if (TriFile->at(i).ExistLine(lineFile,pMyLine) != -1) {

			RmvPoint->push_back(TriFile->at(i).ExistLine(lineFile, pMyLine));
        
        
        }
    
    
    }
*/

    return RmvPoint;
}

/*********************
在边界缝隙之间进行构网时，对对应边进行相应第三点角度获取
*****************/
VECTORDOUBLE* getConvexAngle(int p1 , int p2 , VECTORMYPOINT* convexPointPart , int p3)
{
	VECTORDOUBLE* angle = new VECTORDOUBLE;
	int leftOrRight;
	unsigned int convexPointPartSize = convexPointPart->size();
	if(p3 != -1)
	{
		leftOrRight = jugleLeftOrRight(convexPointPart->at(p1) , convexPointPart->at(p2) , convexPointPart->at(p3));
	}
	else
	{
		leftOrRight = -2;
	}
	if(leftOrRight != -2)
	{
		for(unsigned int i = 0 ; i < convexPointPartSize ; i++)
		{
			if((jugleLeftOrRight(convexPointPart->at(p1) , convexPointPart->at(p2) , convexPointPart->at(i))!=leftOrRight)&&i!=p1&&i!=p2)
			{
				angle->push_back(getAngle(convexPointPart->at(p1) , convexPointPart->at(p2) , convexPointPart->at(i)));
				//angle->push_back(getMidpointDis(convexPointPart->at(p1) , convexPointPart->at(p2) , convexPointPart->at(i)));
			}
		}
	}
	else
	{
		for(unsigned int i = 0 ; i < convexPointPartSize ; i++)
		{
			angle->push_back(getAngle(convexPointPart->at(p1) , convexPointPart->at(p2) , convexPointPart->at(i)));
			//angle->push_back(getMidpointDis(convexPointPart->at(p1) , convexPointPart->at(p2) , convexPointPart->at(i)));
		}
	}
	return angle;
}

/*******************
边界缝隙范围内进行所有点的角度计算并存储
p1为pMyLine的左点，p2为PMyLine的右点，convexPointPart为缝隙范围内的点数组
***********************/
VECTORDOUBLE* getConvexAllAngle(int p1 , int p2 , VECTORMYPOINT* convexPointPart)//获得该范围内所有点的角度，与pointRagei个数相对应，方便排序是记录位置索引
{
	VECTORDOUBLE* rageAngle = new VECTORDOUBLE;
	unsigned int convexPointPartsize = convexPointPart->size();
	for(unsigned int i = 0 ; i < convexPointPartsize ; i++)
	{
		//rageAngle->push_back(getMidpointDis(convexPointPart->at(p1) , convexPointPart->at(p2) , convexPointPart->at(i)));
		rageAngle->push_back(getAngle(convexPointPart->at(p1) , convexPointPart->at(p2) , convexPointPart->at(i)));
	}
	return rageAngle;
}

VECTORDOUBLE* getSonPartRageAllAngle(int p1 , int p2 , VECTORINT* pointRagei , VECTORMYPOINT* sonPointPart)//获得该范围内所有点的角度，与pointRagei个数相对应，方便排序是记录位置索引
{
	VECTORDOUBLE* rageAngle = new VECTORDOUBLE;
	unsigned int pointRageisize = pointRagei->size();
	for(unsigned int i = 0 ; i < pointRageisize ; i++)
	{
		/*if(p1 == 1120 && p2 == 1114 && i == 76)
		{
			int i = 0;
		}*/
		rageAngle->push_back(getAngle(sonPointPart->at(p1) , sonPointPart->at(p2) , sonPointPart->at(pointRagei->at(i))));
	}
	return rageAngle;
}

VECTORDOUBLE* getSonPartRageAngle(int p1 , int p2 , VECTORINT* pointRagei , VECTORMYPOINT* sonPointPart , int p3)//获得搜索范围内角度数组，包括方向因素
{
	VECTORDOUBLE* rageAngle = new VECTORDOUBLE;
	unsigned int pointRageisize = pointRagei->size();
	int leftOrright;
	if(p3!=-1)
	{
		leftOrright = jugleLeftOrRight(sonPointPart->at(p1) , sonPointPart->at(p2) , sonPointPart->at(p3));//三角形中另外一个点对于直线的位置，搜索时取另外一边的范围
	}
	else
	{
		leftOrright = -2;//边界线
	}
	if(leftOrright!=-2)
	{
		for(unsigned int i = 0 ; i < pointRageisize ; i++)
		{
			if((jugleLeftOrRight(sonPointPart->at(p1) , sonPointPart->at(p2) , sonPointPart->at(pointRagei->at(i)))!=leftOrright)&&pointRagei->at(i)!=p1&&pointRagei->at(i)!=p2)//搜索相反位置且不为端点
			{
				rageAngle->push_back(getAngle(sonPointPart->at(p1) , sonPointPart->at(p2) , sonPointPart->at(pointRagei->at(i))));
			}
			else
			{
				continue;
			}
			
		}
	}
	else
	{
		for(unsigned int i = 0 ; i < pointRageisize ; i++)
		{

			if(pointRagei->at(i)!=p1&&pointRagei->at(i)!=p2)
			{
				rageAngle->push_back(getAngle(sonPointPart->at(p1) , sonPointPart->at(p2) , sonPointPart->at(pointRagei->at(i))));
			}
			else
			{
				continue;
			}
			
			
		}

	}
	return rageAngle;
}

/********************
对angleVector中的角度进行排序，返回result中存储的是convexpointPart的编号
**********************/
VECTORINT* getConvexAngleIndex(VECTORDOUBLE* angleVector , VECTORDOUBLE* angleAllVector , VECTORMYPOINT* convexPointPart , int p1 , int p2 , int p3)
{
	VECTORINT* result = new VECTORINT;//结果
	unsigned int angleVectorSize = angleVector->size();
	VECTORDOUBLE* temp = new  VECTORDOUBLE;//临时存储角度数组
	int leftOrright;//判断上一个点关于边的位置
	if(p3!=-1)
	{
		leftOrright = jugleLeftOrRight(convexPointPart->at(p1) , convexPointPart->at(p2) , convexPointPart->at(p3));//三角形中另外一个点对于直线的位置，搜索时取另外一边的范围
	}
	else
	{
		leftOrright = -2;//p3为-1即边界线没有对应形成三角形的点则搜索全部范围
	}
	for (unsigned int i = 0; i < angleVectorSize; i++)
    {
        temp->push_back(angleVector->at(i));
	}
	sort(temp->begin() , temp->end() , comp);
	unsigned int tempSize = temp->size();
	unsigned int angleAllVectorSize = angleAllVector->size();
	for(unsigned int i = 0 ; i < tempSize ; i++)
	{
		for(unsigned int j = 0 ; j < angleAllVectorSize ; j++)
		{
			if(temp->at(i) == angleAllVector->at(j))
			{
				if(leftOrright != -2)
				{
					if(jugleLeftOrRight(convexPointPart->at(p1) , convexPointPart->at(p2) , convexPointPart->at(j)) != leftOrright)
					{
						if(!ifExistcos(j , result) && j!=p1 && j!=p2)//如果已经查找过该点则跳过该点查找下一个对应角度的点，且不能为线段端点
						{
							result->push_back(j);
							break;
						}
					}
					
				}
				else
				{
					if(!ifExistcos(j , result) && j!=p1 && j !=p2)//边界线段查找所有范围内
					{
						result->push_back(j);
						break;
					}
				}
			}
		}
	}

	delete temp;
	return result;


}

//rageAngle为搜索范围内对应方向上的第三点内角，当为边界线段时，搜索范围为线的两个方向，当为非边界时，搜索范围为线的一个方向
//rageAllAngle为搜索范围内所有点的内角，没有方向因素，与pointRagei存储的范围点相对应，方便排序
//sonPointPart点的数组
//p1 ， p2为线的两个端点，p3为线的上一个第三点索引
VECTORINT* getSonPartAngleIndex(VECTORDOUBLE* rageAngle , VECTORDOUBLE* rageAllAngle , VECTORINT* pointRagei , VECTORMYPOINT* sonPointPart , int p1 , int p2 , int p3)
{
	VECTORINT* result = new VECTORINT;//结果
	unsigned int rageAngleSize = rageAngle->size();
	VECTORDOUBLE* temp = new  VECTORDOUBLE;//临时存储角度数组
	int leftOrright;//判断上一个点关于边的位置
	if(p3!=-1)
	{
		leftOrright = jugleLeftOrRight(sonPointPart->at(p1) , sonPointPart->at(p2) , sonPointPart->at(p3));//三角形中另外一个点对于直线的位置，搜索时取另外一边的范围
	}
	else
	{
		leftOrright = -2;//p3为-1即边界线没有对应形成三角形的点则搜索全部范围
	}
	for (unsigned int i = 0; i < rageAngleSize; i++)
    {
        temp->push_back(rageAngle->at(i));
	}
	sort(temp->begin() , temp->end() , comp);
	unsigned int tempSize = temp->size();
	unsigned int rageAllAngleSize = rageAllAngle->size();
	for(unsigned int i = 0 ; i < tempSize ; i++)
	{
		for(unsigned int j = 0 ; j < rageAllAngleSize ; j++)
		{
			if(temp->at(i) == rageAllAngle->at(j))
			{
				if(leftOrright != -2)
				{
					if(jugleLeftOrRight(sonPointPart->at(p1) , sonPointPart->at(p2) , sonPointPart->at(pointRagei->at(j)))!=leftOrright)
					{
						if(!ifExistcos(pointRagei->at(j) , result)&&pointRagei->at(j)!=p1&&pointRagei->at(j)!=p2)//如果已经查找过该点则跳过该点查找下一个对应角度的点，且不能为线段端点
						{
							result->push_back(pointRagei->at(j));
							break;
						}
					}
					
				}
				else
				{
					if(!ifExistcos(pointRagei->at(j) , result)&&pointRagei->at(j)!=p1&&pointRagei->at(j)!=p2)//边界线段查找所有范围内
					{
						result->push_back(pointRagei->at(j));
						break;
					}
				}
			}
		}
	}

	delete temp;
	return result;


}

//r为半径
//center为外接圆圆心
//RmvPoint为基线对应的已有第三点，可能为0,1或2
//sonPointPart为目标点集
//pMyLine基线
//p目标点
bool convexJudgeRule(double r , MyPoint center , VECTORINT* RmvPoint , VECTORMYPOINT* convexPointPart , MyLine pMyLine , int p)
{
	bool result = true;
	bool Exist;
	unsigned int convexPointPartSize = convexPointPart->size();
	for(unsigned int i = 0 ; i < convexPointPartSize ; i++)
	{
		Exist = false;
		for(unsigned int j = 0; j < RmvPoint->size(); j++)
		{
			if(i == RmvPoint->at(j))
			{
				Exist = true;
                break;
			}
		}
		if(!Exist && i != p&& i != pMyLine.LeftPoint && i != pMyLine.RightPoint)
		{
			double d = Get3dDis(center , convexPointPart->at(i));
			if(d < r)
			{
				result = false;
                return result;
			}
		}
	}

	return result;
}


//r为半径
//center为外接圆圆心
//RmvPoint为基线对应的已有第三点，可能为0,1或2
//sonPointPart为目标点集
//pointRagei该基线范围内的搜索点集
//pMyLine基线
//p目标点
bool sonPartJudgeRule(double r , MyPoint center , VECTORINT* RmvPoint , VECTORMYPOINT* sonPointPart , VECTORINT* pointRagei , MyLine pMyLine , int p)
{
	bool result = true;
	bool Exist;
	unsigned int pointRageiSize = pointRagei->size();
	for(unsigned int i = 0 ; i < pointRageiSize ; i++)
	{
		Exist = false;
		for (unsigned int j = 0; j < RmvPoint->size(); j++)//该边所形成的上一个三角形对应点不参加普判断
        {
			if (pointRagei->at(i) == RmvPoint->at(j))
            {
                Exist = true;
                break;

            }           
        
        }
		if(!Exist && pointRagei->at(i) != p && pointRagei->at(i) != pMyLine.LeftPoint && pointRagei->at(i) != pMyLine.RightPoint)
		{
			double d = Get3dDis(center, sonPointPart->at(pointRagei->at(i)));
			if(d < r)
			{
				result = false;
                return result;
			}
		}
	}
	return result;
}

//int getSonPartLineIndex(MyLine pMyLine , int sonPartIndex)
//{
//	int index = -1;
//	switch(sonPartIndex)
//	{
//	case 0:
//		{
//			unsigned int lineSonPartSize0 = lineSonPart0->size();
//			for(unsigned int i = 0 ; i < lineSonPartSize0 ; i++)
//			{
//				if(pMyLine.LeftPoint == lineSonPart0->at(i).LeftPoint && pMyLine.RightPoint == lineSonPart0->at(i).RightPoint)
//				{
//					index = i;
//					break;
//				}
//			}
//		}
//	case 1:
//		{
//			unsigned int lineSonPartSize1 = lineSonPart1->size();
//			for(unsigned int i = 0 ; i < lineSonPartSize1 ; i++)
//			{
//				if(pMyLine.LeftPoint == lineSonPart1->at(i).LeftPoint && pMyLine.RightPoint == lineSonPart1->at(i).RightPoint)
//				{
//					index = i;
//					break;
//				}
//			}
//		}
//	case 2:
//		{
//			unsigned int lineSonPartSize2 = lineSonPart2->size();
//			for(unsigned int i = 0 ; i < lineSonPartSize2 ; i++)
//			{
//				if(pMyLine.LeftPoint == lineSonPart2->at(i).LeftPoint && pMyLine.RightPoint == lineSonPart2->at(i).RightPoint)
//				{
//					index = i;
//					break;
//				}
//			}
//		}
//	case 3:
//		{
//			unsigned int lineSonPartSize3 = lineSonPart3->size();
//			for(unsigned int i = 0 ; i < lineSonPartSize3 ; i++)
//			{
//				if(pMyLine.LeftPoint == lineSonPart3->at(i).LeftPoint && pMyLine.RightPoint == lineSonPart3->at(i).RightPoint)
//				{
//					index = i;
//					break;
//				}
//			}
//		}
//	default:
//		cout<<"在线数组"<<sonPartIndex<<"中查找对应线段的index时出错"<<endl;
//	}
//
//	return index;
//}

/***************************************
非递归的方式构网 
***********************************/

JudgeLine* DealSonPartTriangle1(MyLine pMyLine , VECTORMYPOINT* sonPointPart , int sonPartIndex)//构建三角网
{
	 JudgeLine* judge;
	 MyLine CreaLine1;
     MyLine CreaLine2;
	 VECTORINT* pointRagei1 = new VECTORINT;//左点搜索范围临时
	 VECTORINT* pointRagei2 = new VECTORINT;//右点搜索范围临时
	 VECTORINT* pointRagei = new VECTORINT; //总的搜索范围
	 //VECTORDOUBLE* Angle = new VECTORDOUBLE;//每个点与直线所构成的角度数组
	 VECTORINT* AngleIndex;//Angle进行排序的垂直矩阵
	 VECTORDOUBLE* RageAngle;//搜索范围内对应方向的角度
	 VECTORDOUBLE* RageAllAngle;//搜索范围内所有角度
	 int LineIndex;



	 switch(sonPartIndex) 
	 {
	 case 0:
		 {
			 
			 LineIndex = GetSonPartLineIndex(pMyLine , 0);//在lineFlie中找到pMyLine索引
			 

			 VECTORINT* RmvPoint;
			 RmvPoint = GetSonPartRmvPoint(LineIndex , 0);
			 //bool createL1 = false;//判断新生成的边是不是已经存入了数组
			 //bool createL2 = false;
			 bool Finish = false;

			 

			 


			 

			 //为了防止sonPartPointRage0中的数值发生改变，使用assign方法进行赋值
			 pointRagei1->assign(sonPartPointRage0->at(pMyLine.LeftPoint).getAllRageIndex()->begin() , sonPartPointRage0->at(pMyLine.LeftPoint).getAllRageIndex()->end());
			 pointRagei2->assign(sonPartPointRage0->at(pMyLine.RightPoint).getAllRageIndex()->begin() , sonPartPointRage0->at(pMyLine.RightPoint).getAllRageIndex()->end());
			 pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
			 pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());
			 RageAllAngle = getSonPartRageAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart);
			 if(RmvPoint->size() == 0)
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , -1);
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , -1);
			 }
			 else
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , RmvPoint->at(0));
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
			 }

			 unsigned int AngleIndexSize = AngleIndex->size();
			 unsigned int i;
			 for(i = 0 ; i < AngleIndexSize ; i++)
			 {
				 MyPoint center;//外接圆圆心
				 center = GetCircumCenter(sonPointPart->at(AngleIndex->at(i)) , sonPointPart->at(pMyLine.LeftPoint) , sonPointPart->at(pMyLine.RightPoint));
				 double r;//半径
				 r = GetR(center , sonPointPart->at(AngleIndex->at(i)));
				 if(sonPartJudgeRule(r , center , RmvPoint , sonPointPart , pointRagei , pMyLine , AngleIndex->at(i)))
				 {

					 /********************************
					 判断新生成的线段是不是符合要求
					 **********************************/
					 
					 CreaLine1 = AddLinePoint(AngleIndex->at(i), pMyLine.LeftPoint , sonPointPart);
					 CreaLine2 = AddLinePoint(AngleIndex->at(i), pMyLine.RightPoint ,sonPointPart);

					 if(ExistLine(lineSonPart0 , CreaLine1) && ExistLine(lineSonPart0 , CreaLine2))//两条边都已经存在，所以不许再次存入，但并不代表三角形已经存在
					 {
						 /*createL1 = true;
						 createL2 = true;*/
						 CreaLine1.ifHave = true;
						 CreaLine2.ifHave = true;
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else if(ExistLine(lineSonPart0 , CreaLine1) && !ExistLine(lineSonPart0 , CreaLine2))//CreaLine1存在，所以不必存入，CreaLine2不存在，进行存储
					 {
						 //createL1 = true;
						 CreaLine1.ifHave = true;
						 lineSonPart0->push_back(CreaLine2);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else if(!ExistLine(lineSonPart0 , CreaLine1) && ExistLine(lineSonPart0 , CreaLine2))
					 {
						 //createL2 = true;
						 CreaLine2.ifHave = true;
						 lineSonPart0->push_back(CreaLine1);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else if(!ExistLine(lineSonPart0, CreaLine1)&&!ExistLine(lineSonPart0, CreaLine2))
					 {
						 lineSonPart0->push_back(CreaLine1);
						 lineSonPart0->push_back(CreaLine2);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else
					 {
						 cout<< "存入子块"<<sonPartIndex<<"线文件时出错！"<< endl;
					 }


					 /***************************
					 判断新生成的三角形是不是符合要求
					 *****************************/
					 MyTri CreaTri;
					 CreaTri.Line1 = GetSonPartLineIndex(CreaLine1 , 0);
					 CreaTri.Line2 = GetSonPartLineIndex(CreaLine2 , 0);
					 CreaTri.Line3 = LineIndex;

					 CreaTri.p1 = CreaLine1.LeftPoint;
					 CreaTri.p2 = CreaLine1.RightPoint;
					 if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
					 {
						 CreaTri.p3 = CreaLine2.RightPoint;
					 }
					 else
					 {
						 CreaTri.p3 = CreaLine2.LeftPoint;
					 }

					 if(ExistTri(triSonPart0 , CreaTri))
					 {
						 delete RmvPoint;//如果新三角形已存在则不存入
			 			 delete AngleIndex;
						 delete RageAngle;
						 delete RageAllAngle;
			 			 delete pointRagei;
						 delete pointRagei1;
						 delete pointRagei2;
						 lineSonPart0->at(LineIndex).ifHave = true;
			 			 return judge;            
					 }
					 lineSonPart0->at(LineIndex).ifHave = true;
					 triSonPart0->push_back(CreaTri);
					 triSonPartTemp0->push_back(CreaTri);

					 Finish = true;
					 CreaLine1 = lineSonPart0->at(CreaTri.Line1);
					 CreaLine2 = lineSonPart0->at(CreaTri.Line2);
					 break;
					
				 }
			 }

			 
			 /***************************
			 对新生成的三角形的两条基边进行新的生长，如果Finish为false，则该生长线下无法生成新的三角形，停止生长
			 如果AngleIndex存储的角度顺序数组已经从开始找到最后，都没找到合适的第三点，则停止生长
			 两个条件其实时相同意思
			 ***************************/
			 if(Finish && i < AngleIndexSize)
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 return judge;

				 //if(createL1&&!createL2)//由于CreaLine1已经存储了，所以不可能将其作为基边进行生长，因为每条边最多只能包括两个三角形
				 //{
					// DealSonPartTriangle(CreaLine2 , sonPointPart , 0);
				 //}
				 //else if(createL2&&!createL1)
				 //{
					// DealSonPartTriangle(CreaLine1 , sonPointPart , 0);
				 //}
				 //else if(!createL1&&!createL2)
				 //{
					// DealSonPartTriangle(CreaLine1 , sonPointPart , 0);
					// DealSonPartTriangle(CreaLine2 , sonPointPart , 0);
				 //}
				 //else
				 //{
					// return;
				 //}

			 }
			 else
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 lineSonPart0->at(LineIndex).ifHave = true;
				 judge = new JudgeLine(lineSonPart0->at(0) , lineSonPart0->at(1));
				 //judge->setIfCom(true);
				 return judge;
			 }
			 break;
			

		 }
	 case 1:
		 {
			 LineIndex = GetSonPartLineIndex(pMyLine , 1);//在lineFlie中找到pMyLine索引
			 VECTORINT* RmvPoint;
			 RmvPoint = GetSonPartRmvPoint(LineIndex , 1);
			 //bool createL1 = false;//判断新生成的边是不是已经存入了数组
			 //bool createL2 = false;
			 bool Finish = false;

			 /*pointRagei1 = sonPartPointRage1->at(pMyLine.LeftPoint).getAllRageIndex();
			 pointRagei2 = sonPartPointRage1->at(pMyLine.RightPoint).getAllRageIndex();*/

			 /*if(pMyLine.LeftPoint == 73 && pMyLine.RightPoint == 622)
			 {
				 int i = 0;
			 }*/

			 pointRagei1->assign(sonPartPointRage1->at(pMyLine.LeftPoint).getAllRageIndex()->begin() , sonPartPointRage1->at(pMyLine.LeftPoint).getAllRageIndex()->end());
			 pointRagei2->assign(sonPartPointRage1->at(pMyLine.RightPoint).getAllRageIndex()->begin() , sonPartPointRage1->at(pMyLine.RightPoint).getAllRageIndex()->end());

			 pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
			 pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());
			 RageAllAngle = getSonPartRageAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart);
			 if(RmvPoint->size() == 0)
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , -1);
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , -1);
			 }
			 else
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , RmvPoint->at(0));
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
			 }

			 unsigned int AngleIndexSize = AngleIndex->size();
			 unsigned int j;
			 for(j = 0 ; j < AngleIndexSize ; j++)
			 {
				 MyPoint center;//外接圆圆心
				 center = GetCircumCenter(sonPointPart->at(AngleIndex->at(j)) , sonPointPart->at(pMyLine.LeftPoint) , sonPointPart->at(pMyLine.RightPoint));
				 double r;//半径
				 r = GetR(center , sonPointPart->at(AngleIndex->at(j)));
				 if(sonPartJudgeRule(r , center , RmvPoint , sonPointPart , pointRagei , pMyLine , AngleIndex->at(j)))
				 {

					 /********************************
					 判断新生成的线段是不是符合要求
					 **********************************/
					 
					 CreaLine1 = AddLinePoint(AngleIndex->at(j), pMyLine.LeftPoint , sonPointPart);
					 CreaLine2 = AddLinePoint(AngleIndex->at(j), pMyLine.RightPoint ,sonPointPart);

					 if(ExistLine(lineSonPart1 , CreaLine1) && ExistLine(lineSonPart1 , CreaLine2))//两条边都已经存在，所以不许再次存入，但并不代表三角形已经存在
					 {
						 /*createL1 = true;
						 createL2 = true;*/
						 CreaLine1.ifHave = true;
						 CreaLine2.ifHave = true;
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else if(ExistLine(lineSonPart1 , CreaLine1) && !ExistLine(lineSonPart1 , CreaLine2))//CreaLine1存在，所以不必存入，CreaLine2不存在，进行存储
					 {
						 /*createL1 = true;*/
						 CreaLine1.ifHave = true;
						 lineSonPart1->push_back(CreaLine2);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else if(!ExistLine(lineSonPart1 , CreaLine1) && ExistLine(lineSonPart1 , CreaLine2))
					 {
						 //createL2 = true;
						 CreaLine2.ifHave = true;
						 lineSonPart1->push_back(CreaLine1);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else if(!ExistLine(lineSonPart1, CreaLine1)&&!ExistLine(lineSonPart1, CreaLine2))
					 {
						 lineSonPart1->push_back(CreaLine1);
						 lineSonPart1->push_back(CreaLine2);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else
					 {
						 cout<< "存入子块"<<1<<"线文件时出错！"<< endl;
					 }


					 /***************************
					 判断新生成的三角形是不是符合要求
					 *****************************/
					 MyTri CreaTri;
					 CreaTri.Line1 = GetSonPartLineIndex(CreaLine1 , 1);
					 CreaTri.Line2 = GetSonPartLineIndex(CreaLine2 , 1);
					 CreaTri.Line3 = LineIndex;

					 CreaTri.p1 = CreaLine1.LeftPoint;
					 CreaTri.p2 = CreaLine1.RightPoint;
					 if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
					 {
						 CreaTri.p3 = CreaLine2.RightPoint;
					 }
					 else
					 {
						 CreaTri.p3 = CreaLine2.LeftPoint;
					 }

					 if(ExistTri(triSonPart1 , CreaTri))
					 {
						 delete RmvPoint;//如果新三角形已存在则不存入
			 			 delete AngleIndex;
						 delete RageAngle;
						 delete RageAllAngle;
			 			 delete pointRagei;
						 delete pointRagei1;
						 delete pointRagei2;
			 			 lineSonPart1->at(LineIndex).ifHave = true;
			 			 return judge;
					 }
					 lineSonPart1->at(LineIndex).ifHave = true;
					 triSonPart1->push_back(CreaTri);
					 triSonPartTemp1->push_back(CreaTri);

					 Finish = true;
					 CreaLine1 = lineSonPart1->at(CreaTri.Line1);
					 CreaLine2 = lineSonPart1->at(CreaTri.Line2);
					 break;
					
				 }
			 }
			 
			 /***************************
			 对新生成的三角形的两条基边进行新的生长，如果Finish为false，则该生长线下无法生成新的三角形，停止生长
			 如果AngleIndex存储的角度顺序数组已经从开始找到最后，都没找到合适的第三点，则停止生长
			 两个条件其实时相同意思
			 ***************************/
			 if(Finish && j < AngleIndexSize)
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 return judge;

				 //if(createL1&&!createL2)//由于CreaLine1已经存储了，所以不可能将其作为基边进行生长，因为每条边最多只能包括两个三角形
				 //{
					// DealSonPartTriangle(CreaLine2 , sonPointPart , 1);
				 //}
				 //else if(createL2&&!createL1)
				 //{
					// DealSonPartTriangle(CreaLine1 , sonPointPart , 1);
				 //}
				 //else if(!createL1&&!createL2)
				 //{
					// DealSonPartTriangle(CreaLine1 , sonPointPart , 1);
					// DealSonPartTriangle(CreaLine2 , sonPointPart , 1);
				 //}
				 //else
				 //{
					// return;
				 //}

			 }
			 else
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle; 
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 lineSonPart1->at(LineIndex).ifHave = true;
				 judge = new JudgeLine(lineSonPart1->at(0) , lineSonPart1->at(1));
				 return judge;
			 }
			 break;//switch下不一样
			

		 }
		 case 2:
		 {
			 LineIndex = GetSonPartLineIndex(pMyLine , 2);//在lineFlie中找到pMyLine索引
			 VECTORINT* RmvPoint;
			 RmvPoint = GetSonPartRmvPoint(LineIndex , 2);
			 //bool createL1 = false;//判断新生成的边是不是已经存入了数组
			 //bool createL2 = false;
			 bool Finish = false;

			 /*pointRagei1 = sonPartPointRage2->at(pMyLine.LeftPoint).getAllRageIndex();
			 pointRagei2 = sonPartPointRage2->at(pMyLine.RightPoint).getAllRageIndex();*/
			 pointRagei1->assign(sonPartPointRage2->at(pMyLine.LeftPoint).getAllRageIndex()->begin() , sonPartPointRage2->at(pMyLine.LeftPoint).getAllRageIndex()->end());
			 pointRagei2->assign(sonPartPointRage2->at(pMyLine.RightPoint).getAllRageIndex()->begin() , sonPartPointRage2->at(pMyLine.RightPoint).getAllRageIndex()->end());

			 pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
			 pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());
			 RageAllAngle = getSonPartRageAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart);
			 if(RmvPoint->size() == 0)
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , -1);
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , -1);
			 }
			 else
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , RmvPoint->at(0));
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
			 }

			 unsigned int AngleIndexSize = AngleIndex->size();
			 unsigned int l;
			 for(l = 0 ; l < AngleIndexSize ; l++)
			 {
				 MyPoint center;//外接圆圆心
				 center = GetCircumCenter(sonPointPart->at(AngleIndex->at(l)) , sonPointPart->at(pMyLine.LeftPoint) , sonPointPart->at(pMyLine.RightPoint));
				 double r;//半径
				 r = GetR(center , sonPointPart->at(AngleIndex->at(l)));
				 if(sonPartJudgeRule(r , center , RmvPoint , sonPointPart , pointRagei , pMyLine , AngleIndex->at(l)))
				 {

					 /********************************
					 判断新生成的线段是不是符合要求
					 **********************************/
					 
					 CreaLine1 = AddLinePoint(AngleIndex->at(l), pMyLine.LeftPoint , sonPointPart);
					 CreaLine2 = AddLinePoint(AngleIndex->at(l), pMyLine.RightPoint ,sonPointPart);

					 if(ExistLine(lineSonPart2 , CreaLine1) && ExistLine(lineSonPart2 , CreaLine2))//两条边都已经存在，所以不许再次存入，但并不代表三角形已经存在
					 {
						 /*createL1 = true;
						 createL2 = true;*/
						 CreaLine1.ifHave = true;
						 CreaLine2.ifHave = true;
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else if(ExistLine(lineSonPart2 , CreaLine1) && !ExistLine(lineSonPart2 , CreaLine2))//CreaLine1存在，所以不必存入，CreaLine2不存在，进行存储
					 {
						 //createL1 = true;
						 CreaLine1.ifHave = true;
						 lineSonPart2->push_back(CreaLine2);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else if(!ExistLine(lineSonPart2 , CreaLine1) && ExistLine(lineSonPart2 , CreaLine2))
					 {
						 //createL2 = true;
						 CreaLine2.ifHave = true;
						 lineSonPart2->push_back(CreaLine1);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else if(!ExistLine(lineSonPart2 , CreaLine1)&&!ExistLine(lineSonPart2 , CreaLine2))
					 {
						 lineSonPart2->push_back(CreaLine1);
						 lineSonPart2->push_back(CreaLine2);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else
					 {
						 cout<< "存入子块"<<2<<"线文件时出错！"<< endl;
					 }


					 /***************************
					 判断新生成的三角形是不是符合要求
					 *****************************/
					 MyTri CreaTri;
					 CreaTri.Line1 = GetSonPartLineIndex(CreaLine1 , 2);
					 CreaTri.Line2 = GetSonPartLineIndex(CreaLine2 , 2);
					 CreaTri.Line3 = LineIndex;

					 CreaTri.p1 = CreaLine1.LeftPoint;
					 CreaTri.p2 = CreaLine1.RightPoint;
					 if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
					 {
						 CreaTri.p3 = CreaLine2.RightPoint;
					 }
					 else
					 {
						 CreaTri.p3 = CreaLine2.LeftPoint;
					 }

					 if(ExistTri(triSonPart2 , CreaTri))
					 {
						 delete RmvPoint;//如果新三角形已存在则不存入
			 			 delete AngleIndex;
						 delete RageAngle;
						 delete RageAllAngle;
			 			 delete pointRagei;
						 delete pointRagei1;
						 delete pointRagei2;
			 			 lineSonPart2->at(LineIndex).ifHave = true;
			 			 return judge;            
					 }
					 lineSonPart2->at(LineIndex).ifHave = true;
					 triSonPart2->push_back(CreaTri);
					 triSonPartTemp2->push_back(CreaTri);

					 Finish = true;
					 CreaLine1 = lineSonPart2->at(CreaTri.Line1);
					 CreaLine2 = lineSonPart2->at(CreaTri.Line2);
					 break;
					
				 }
			 }
			 
			 /***************************
			 对新生成的三角形的两条基边进行新的生长，如果Finish为false，则该生长线下无法生成新的三角形，停止生长
			 如果AngleIndex存储的角度顺序数组已经从开始找到最后，都没找到合适的第三点，则停止生长
			 两个条件其实时相同意思
			 ***************************/
			 if(Finish && l < AngleIndexSize)
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 return judge;

				 //if(createL1&&!createL2)//由于CreaLine1已经存储了，所以不可能将其作为基边进行生长，因为每条边最多只能包括两个三角形
				 //{
					// DealSonPartTriangle(CreaLine2 , sonPointPart , 2);
				 //}
				 //else if(createL2&&!createL1)
				 //{
					// DealSonPartTriangle(CreaLine1 , sonPointPart , 2);
				 //}
				 //else if(!createL1&&!createL2)
				 //{
					// DealSonPartTriangle(CreaLine1 , sonPointPart , 2);
					// DealSonPartTriangle(CreaLine2 , sonPointPart , 2);
				 //}
				 //else
				 //{
					// return;
				 //}

			 }
			 else
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 lineSonPart2->at(LineIndex).ifHave = true;
				 judge = new JudgeLine(lineSonPart2->at(0) , lineSonPart2->at(1));

				 return judge;
			 }
			 break;
			

		 }
		 case 3:
		 {
			 LineIndex = GetSonPartLineIndex(pMyLine , 3);//在lineFlie中找到pMyLine索引
			 VECTORINT* RmvPoint;
			 RmvPoint = GetSonPartRmvPoint(LineIndex , 3);
			 //bool createL1 = false;//判断新生成的边是不是已经存入了数组
			 //bool createL2 = false;
			 bool Finish = false;

			 /*pointRagei1 = sonPartPointRage3->at(pMyLine.LeftPoint).getAllRageIndex();
			 pointRagei2 = sonPartPointRage3->at(pMyLine.RightPoint).getAllRageIndex();*/
			 pointRagei1->assign(sonPartPointRage3->at(pMyLine.LeftPoint).getAllRageIndex()->begin() , sonPartPointRage3->at(pMyLine.LeftPoint).getAllRageIndex()->end());
			 pointRagei2->assign(sonPartPointRage3->at(pMyLine.RightPoint).getAllRageIndex()->begin() , sonPartPointRage3->at(pMyLine.RightPoint).getAllRageIndex()->end());

			 pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
			 pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());
			 RageAllAngle = getSonPartRageAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart);
			 if(RmvPoint->size() == 0)
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , -1);
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , -1);
			 }
			 else
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , RmvPoint->at(0));
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
			 }

			 unsigned int AngleIndexSize = AngleIndex->size();
			 unsigned int k;


			 for(k = 0 ; k < AngleIndexSize ; k++)
			 {
				 MyPoint center;//外接圆圆心
				 center = GetCircumCenter(sonPointPart->at(AngleIndex->at(k)) , sonPointPart->at(pMyLine.LeftPoint) , sonPointPart->at(pMyLine.RightPoint));
				 double r;//半径
				 r = GetR(center , sonPointPart->at(AngleIndex->at(k)));
				 if(sonPartJudgeRule(r , center , RmvPoint , sonPointPart , pointRagei , pMyLine , AngleIndex->at(k)))
				 {

					 /********************************
					 判断新生成的线段是不是符合要求
					 **********************************/
					 
					 CreaLine1 = AddLinePoint(AngleIndex->at(k), pMyLine.LeftPoint , sonPointPart);
					 CreaLine2 = AddLinePoint(AngleIndex->at(k), pMyLine.RightPoint ,sonPointPart);

					 if(ExistLine(lineSonPart3 , CreaLine1) && ExistLine(lineSonPart3 , CreaLine2))//两条边都已经存在，所以不许再次存入，但并不代表三角形已经存在
					 {
						 /*createL1 = true;
						 createL2 = true;*/
						 CreaLine1.ifHave = true;
						 CreaLine2.ifHave = true;
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else if(ExistLine(lineSonPart3 , CreaLine1) && !ExistLine(lineSonPart3 , CreaLine2))//CreaLine1存在，所以不必存入，CreaLine2不存在，进行存储
					 {
						 //createL1 = true;
						 CreaLine1.ifHave = true;
						 lineSonPart3->push_back(CreaLine2);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);

					 }
					 else if(!ExistLine(lineSonPart3 , CreaLine1) && ExistLine(lineSonPart3 , CreaLine2))
					 {
						 //createL2 = true;
						 CreaLine2.ifHave = true;
						 lineSonPart3->push_back(CreaLine1);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else if(!ExistLine(lineSonPart3 , CreaLine1)&&!ExistLine(lineSonPart3 , CreaLine2))
					 {
						 lineSonPart3->push_back(CreaLine1);
						 lineSonPart3->push_back(CreaLine2);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					 }
					 else
					 {
						 cout<< "存入子块"<<3<<"线文件时出错！"<< endl;
					 }


					 /***************************
					 判断新生成的三角形是不是符合要求
					 *****************************/
					 MyTri CreaTri;
					 CreaTri.Line1 = GetSonPartLineIndex(CreaLine1 , 3);
					 CreaTri.Line2 = GetSonPartLineIndex(CreaLine2 , 3);
					 CreaTri.Line3 = LineIndex;

					 CreaTri.p1 = CreaLine1.LeftPoint;
					 CreaTri.p2 = CreaLine1.RightPoint;
					 if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
					 {
						 CreaTri.p3 = CreaLine2.RightPoint;
					 }
					 else
					 {
						 CreaTri.p3 = CreaLine2.LeftPoint;
					 }

					 if(ExistTri(triSonPart3 , CreaTri))
					 {
						 delete RmvPoint;//如果新三角形已存在则不存入
			 			 delete AngleIndex;
						 delete RageAngle;
						 delete RageAllAngle;
			 			 delete pointRagei;
						 delete pointRagei1;
						 delete pointRagei2;
						 lineSonPart3->at(LineIndex).ifHave = true;
			 			 return judge;
			 			             
					 }
					 lineSonPart3->at(LineIndex).ifHave = true;
					 triSonPart3->push_back(CreaTri);
					 triSonPartTemp3->push_back(CreaTri);

					 Finish = true;
					 CreaLine1 = lineSonPart3->at(CreaTri.Line1);
					 CreaLine2 = lineSonPart3->at(CreaTri.Line2);
					 break;
					
				 }
			 }
			 
			 /***************************
			 对新生成的三角形的两条基边进行新的生长，如果Finish为false，则该生长线下无法生成新的三角形，停止生长
			 如果AngleIndex存储的角度顺序数组已经从开始找到最后，都没找到合适的第三点，则停止生长
			 两个条件其实时相同意思
			 ***************************/
			 if(Finish && k < AngleIndexSize)
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 return judge;

				 //if(createL1&&!createL2)//由于CreaLine1已经存储了，所以不可能将其作为基边进行生长，因为每条边最多只能包括两个三角形
				 //{
					// DealSonPartTriangle(CreaLine2 , sonPointPart , 3);
				 //}
				 //else if(createL2&&!createL1)
				 //{
					// DealSonPartTriangle(CreaLine1 , sonPointPart , 3);
				 //}
				 //else if(!createL1&&!createL2)
				 //{
					// DealSonPartTriangle(CreaLine1 , sonPointPart , 3);
					// DealSonPartTriangle(CreaLine2 , sonPointPart , 3);
				 //}
				 //else
				 //{
					// return;
				 //}

			 }
			 else
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 lineSonPart3->at(LineIndex).ifHave = true;
				 judge = new JudgeLine(lineSonPart3->at(0) , lineSonPart3->at(1));

				 return judge;
			 }
			 break;
			

		 }

		 default:
			 cout<<"再构网时出错，子块为："<<sonPartIndex<<endl;
			 JudgeLine* judge = new JudgeLine();
			 return judge;
			 break;
		 

	 }



	 
}

/**********************
非递归方式在边界缝隙之间构网
mode为哪一区域，0为子块0与子块1之间的缝隙，1为子块2与子块3之间的区域，2为上合并子块与下合并子块之间的区域
***********************/
JudgeLine* DealConvexTriangle(MyLine pMyLine , VECTORMYPOINT* convexPointPart , int mode)
{
	JudgeLine* judge;
	MyLine CreaLine1;
    MyLine CreaLine2;
	VECTORINT* AngleIndex;//Angle进行排序的垂直矩阵
	VECTORDOUBLE* angleAllVector;//对应的所有点的角度数组，没有方向因素限制,为了用来确定最后角度对应的位置
	VECTORDOUBLE* angleVector;//对应的角度数组
	int LineIndex;

	switch(mode)
	{
	case 0:
		{
			LineIndex = GetSonPartLineIndex(pMyLine , 4);

			VECTORINT* RmvPoint;
			RmvPoint = GetSonPartRmvPoint(LineIndex , 4);
			bool Finish = false;
			angleAllVector = getConvexAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , convexPointPart);
 			if(RmvPoint->size() == 0)
			{
				angleVector = getConvexAngle(pMyLine.LeftPoint , pMyLine.RightPoint , convexPointPart , -1);
				AngleIndex = getConvexAngleIndex(angleVector , angleAllVector , convexPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , -1);
			}
			else
			{
				angleVector = getConvexAngle(pMyLine.LeftPoint , pMyLine.RightPoint , convexPointPart , RmvPoint->at(0));
				AngleIndex = getConvexAngleIndex(angleVector , angleAllVector , convexPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
			}
			unsigned int AngleIndexSize = AngleIndex->size();
			unsigned int i;
			for(i = 0 ; i < AngleIndexSize ; i++)
			{
				if(angleAllVector->at(i) < 0)
				{
					continue;
				}
				MyPoint center;
				center = GetCircumCenter(convexPointPart->at(AngleIndex->at(i)) , convexPointPart->at(pMyLine.LeftPoint) , convexPointPart->at(pMyLine.RightPoint));
				double r;
				r = GetR(center , convexPointPart->at(AngleIndex->at(i)));
				/*if(convexJudgeRule(r , center , RmvPoint , convexPointPart , pMyLine , AngleIndex->at(i)))
				{*/
					CreaLine1 = AddLinePoint(AngleIndex->at(i), pMyLine.LeftPoint , convexPointPart);
					CreaLine2 = AddLinePoint(AngleIndex->at(i), pMyLine.RightPoint ,convexPointPart);
					if(ExistLine(convexZeroToOneLine , CreaLine1) && ExistLine(convexZeroToOneLine , CreaLine2))//两条边都已经存在，所以不许再次存入，但并不代表三角形已经存在
					{ 
						CreaLine1.ifHave = true;
						CreaLine2.ifHave = true;
						judge = new JudgeLine(CreaLine1 , CreaLine2);
					}
					else if(ExistLine(convexZeroToOneLine , CreaLine1) && !ExistLine(convexZeroToOneLine , CreaLine2))
					{
						CreaLine1.ifHave = true;
						convexZeroToOneLine->push_back(CreaLine2);
						judge = new JudgeLine(CreaLine1 , CreaLine2);
					}
					else if(!ExistLine(convexZeroToOneLine , CreaLine1) && ExistLine(convexZeroToOneLine , CreaLine2))
					{
						 CreaLine2.ifHave = true;
						 convexZeroToOneLine->push_back(CreaLine1);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					}
					else if(!ExistLine(convexZeroToOneLine, CreaLine1)&&!ExistLine(convexZeroToOneLine, CreaLine2))
					{
						 continue;
						 convexZeroToOneLine->push_back(CreaLine1);
						 convexZeroToOneLine->push_back(CreaLine2);
						 judge = new JudgeLine(CreaLine1 , CreaLine2);
					}
					else
					{
						 cout<< "存入缝隙间子块"<<mode<<"线文件时出错！"<< endl;
					}

					MyTri CreaTri;
					CreaTri.Line1 = GetSonPartLineIndex(CreaLine1 , 4);
					CreaTri.Line2 = GetSonPartLineIndex(CreaLine2 , 4);
					CreaTri.Line3 = LineIndex;

					CreaTri.p1 = CreaLine1.LeftPoint;
					CreaTri.p2 = CreaLine1.RightPoint;
					if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
					{
						CreaTri.p3 = CreaLine2.RightPoint;
					}
					else
					{
						CreaTri.p3 = CreaLine2.LeftPoint;
					}
					if(ExistTri(convexZeroToOneTri , CreaTri))
					{
						 delete RmvPoint;//如果新三角形已存在则不存入
			 			 delete AngleIndex;
						 delete angleAllVector;
						 delete angleVector;
						 convexZeroToOneLine->at(LineIndex).ifHave = true;
			 			 return judge;            
					}
					convexZeroToOneLine->at(LineIndex).ifHave = true;
					convexZeroToOneTri->push_back(CreaTri);
					Finish = true;
					CreaLine1 = convexZeroToOneLine->at(CreaTri.Line1);
					CreaLine2 = convexZeroToOneLine->at(CreaTri.Line2);
					break;
				//}
				
			}
			if(Finish && i < AngleIndexSize)
			 {
				delete RmvPoint;
				delete AngleIndex;
				delete angleAllVector;
				delete angleVector;
				return judge;
			}
			else
			{
				delete RmvPoint;
				delete AngleIndex;
				delete angleAllVector;
				delete angleVector;
				convexZeroToOneLine->at(LineIndex).ifHave = true;
				judge = new JudgeLine(convexZeroToOneLine->at(0) , convexZeroToOneLine->at(1));
				return judge;
			}
			break;
		}
	case 1:
		{
			LineIndex = GetSonPartLineIndex(pMyLine , 5);

			VECTORINT* RmvPoint;
			RmvPoint = GetSonPartRmvPoint(LineIndex , 5);
			bool Finish = false;
			angleAllVector = getConvexAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , convexPointPart);
			if(RmvPoint->size() == 0)
			{
				angleVector = getConvexAngle(pMyLine.LeftPoint , pMyLine.RightPoint , convexPointPart , -1);
				AngleIndex = getConvexAngleIndex(angleVector , angleAllVector , convexPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , -1);
			}
			else
			{
				angleVector = getConvexAngle(pMyLine.LeftPoint , pMyLine.RightPoint , convexPointPart , RmvPoint->at(0));
				AngleIndex = getConvexAngleIndex(angleVector , angleAllVector , convexPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
			}
			unsigned int AngleIndexSize = AngleIndex->size();
			unsigned int i;
			for(i = 0 ; i < AngleIndexSize ; i++)
			{
				if(angleAllVector->at(i) < 0)
				{
					continue;
				}
				CreaLine1 = AddLinePoint(AngleIndex->at(i), pMyLine.LeftPoint , convexPointPart);
				CreaLine2 = AddLinePoint(AngleIndex->at(i), pMyLine.RightPoint ,convexPointPart);
				if(ExistLine(convexTwoToThreeLine , CreaLine1) && ExistLine(convexTwoToThreeLine , CreaLine2))//两条边都已经存在，所以不许再次存入，但并不代表三角形已经存在
				{ 
					CreaLine1.ifHave = true;
					CreaLine2.ifHave = true;
					judge = new JudgeLine(CreaLine1 , CreaLine2);
				}
				else if(ExistLine(convexTwoToThreeLine , CreaLine1) && !ExistLine(convexTwoToThreeLine , CreaLine2))
				{
					CreaLine1.ifHave = true;
					convexTwoToThreeLine->push_back(CreaLine2);
					judge = new JudgeLine(CreaLine1 , CreaLine2);
				}
				else if(!ExistLine(convexTwoToThreeLine , CreaLine1) && ExistLine(convexTwoToThreeLine , CreaLine2))
				{
					 CreaLine2.ifHave = true;
					 convexTwoToThreeLine->push_back(CreaLine1);
					 judge = new JudgeLine(CreaLine1 , CreaLine2);
				}
				else if(!ExistLine(convexTwoToThreeLine, CreaLine1)&&!ExistLine(convexTwoToThreeLine, CreaLine2))
				{
					 continue;
					 convexTwoToThreeLine->push_back(CreaLine1);
					 convexTwoToThreeLine->push_back(CreaLine2);
					 judge = new JudgeLine(CreaLine1 , CreaLine2);
				}
				else
				{
					 cout<< "存入缝隙间子块"<<mode<<"线文件时出错！"<< endl;
				}

				MyTri CreaTri;
				CreaTri.Line1 = GetSonPartLineIndex(CreaLine1 , 5);
				CreaTri.Line2 = GetSonPartLineIndex(CreaLine2 , 5);
				CreaTri.Line3 = LineIndex;

				CreaTri.p1 = CreaLine1.LeftPoint;
				CreaTri.p2 = CreaLine1.RightPoint;
				if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
				{
					CreaTri.p3 = CreaLine2.RightPoint;
				}
				else
				{
					CreaTri.p3 = CreaLine2.LeftPoint;
				}
				if(ExistTri(convexTwoToThreeTri , CreaTri))
				{
					 delete RmvPoint;//如果新三角形已存在则不存入
			 		 delete AngleIndex;
					 delete angleAllVector;
					 delete angleVector;
					 convexTwoToThreeLine->at(LineIndex).ifHave = true;
			 		 return judge;            
				}
				convexTwoToThreeLine->at(LineIndex).ifHave = true;
				convexTwoToThreeTri->push_back(CreaTri);
				Finish = true;
				CreaLine1 = convexTwoToThreeLine->at(CreaTri.Line1);
				CreaLine2 = convexTwoToThreeLine->at(CreaTri.Line2);
				break;
			}
			if(Finish && i < AngleIndexSize)
			 {
				delete RmvPoint;
				delete AngleIndex;
				delete angleAllVector;
				delete angleVector;
				return judge;
			}
			else
			{
				delete RmvPoint;
				delete AngleIndex;
				delete angleAllVector;
				delete angleVector;
				convexTwoToThreeLine->at(LineIndex).ifHave = true;
				judge = new JudgeLine(convexTwoToThreeLine->at(0) , convexTwoToThreeLine->at(1));
				return judge;
			}
			break;
		}
	case 2:
		{
			LineIndex = GetSonPartLineIndex(pMyLine , 6);

			VECTORINT* RmvPoint;
			RmvPoint = GetSonPartRmvPoint(LineIndex , 6);
			bool Finish = false;
			angleAllVector = getConvexAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , convexPointPart);
			if(RmvPoint->size() == 0)
			{
				angleVector = getConvexAngle(pMyLine.LeftPoint , pMyLine.RightPoint , convexPointPart , -1);
				AngleIndex = getConvexAngleIndex(angleVector , angleAllVector , convexPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , -1);
			}
			else
			{
				angleVector = getConvexAngle(pMyLine.LeftPoint , pMyLine.RightPoint , convexPointPart , RmvPoint->at(0));
				AngleIndex = getConvexAngleIndex(angleVector , angleAllVector , convexPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
			}
			unsigned int AngleIndexSize = AngleIndex->size();
			unsigned int i;
			for(i = 0 ; i < AngleIndexSize ; i++)
			{
				if(angleAllVector->at(i) < 0)
				{
					continue;
				}
				CreaLine1 = AddLinePoint(AngleIndex->at(i), pMyLine.LeftPoint , convexPointPart);
				CreaLine2 = AddLinePoint(AngleIndex->at(i), pMyLine.RightPoint ,convexPointPart);
				if(ExistLine(convexTopToButtomLine , CreaLine1) && ExistLine(convexTopToButtomLine , CreaLine2))//两条边都已经存在，所以不许再次存入，但并不代表三角形已经存在
				{ 
					CreaLine1.ifHave = true;
					CreaLine2.ifHave = true;
					judge = new JudgeLine(CreaLine1 , CreaLine2);
				}
				else if(ExistLine(convexTopToButtomLine , CreaLine1) && !ExistLine(convexTopToButtomLine , CreaLine2))
				{
					CreaLine1.ifHave = true;
					convexTopToButtomLine->push_back(CreaLine2);
					judge = new JudgeLine(CreaLine1 , CreaLine2);
				}
				else if(!ExistLine(convexTopToButtomLine , CreaLine1) && ExistLine(convexTopToButtomLine , CreaLine2))
				{
					 CreaLine2.ifHave = true;
					 convexTopToButtomLine->push_back(CreaLine1);
					 judge = new JudgeLine(CreaLine1 , CreaLine2);
				}
				else if(!ExistLine(convexTopToButtomLine, CreaLine1)&&!ExistLine(convexTopToButtomLine, CreaLine2))
				{
					 continue;
					 convexTopToButtomLine->push_back(CreaLine1);
					 convexTopToButtomLine->push_back(CreaLine2);
					 judge = new JudgeLine(CreaLine1 , CreaLine2);
				}
				else
				{
					 cout<< "存入缝隙间子块"<<mode<<"线文件时出错！"<< endl;
				}

				MyTri CreaTri;
				CreaTri.Line1 = GetSonPartLineIndex(CreaLine1 , 6);
				CreaTri.Line2 = GetSonPartLineIndex(CreaLine2 , 6);
				CreaTri.Line3 = LineIndex;

				CreaTri.p1 = CreaLine1.LeftPoint;
				CreaTri.p2 = CreaLine1.RightPoint;
				if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
				{
					CreaTri.p3 = CreaLine2.RightPoint;
				}
				else
				{
					CreaTri.p3 = CreaLine2.LeftPoint;
				}
				if(ExistTri(convexTopToButtomTri , CreaTri))
				{
					 delete RmvPoint;//如果新三角形已存在则不存入
			 		 delete AngleIndex;
					 delete angleAllVector;
					 delete angleVector;
					 convexTopToButtomLine->at(LineIndex).ifHave = true;
			 		 return judge;            
				}
				convexTopToButtomLine->at(LineIndex).ifHave = true;
				convexTopToButtomTri->push_back(CreaTri);
				Finish = true;
				CreaLine1 = convexTopToButtomLine->at(CreaTri.Line1);
				CreaLine2 = convexTopToButtomLine->at(CreaTri.Line2);
				break;
			}
			if(Finish && i < AngleIndexSize)
			 {
				delete RmvPoint;
				delete AngleIndex;
				delete angleAllVector;
				delete angleVector;
				//judge->ifComplete = true;
				return judge;
			}
			else
			{
				delete RmvPoint;
				delete AngleIndex;
				delete angleAllVector;
				delete angleVector;
				convexTopToButtomLine->at(LineIndex).ifHave = true;
				judge = new JudgeLine(convexTopToButtomLine->at(0) , convexTopToButtomLine->at(1));
				return judge;
			}
			break;
		}
		default:
		{
			cout<<"子块缝隙间构网出错！"<<endl;
			JudgeLine* judge = new JudgeLine();
			return judge;
			break;
		}
	}
	

}


void DealSonPartTriangle(MyLine pMyLine , VECTORMYPOINT* sonPointPart , int sonPartIndex)//构建三角网
{

	 MyLine CreaLine1;
     MyLine CreaLine2;
	 VECTORINT* pointRagei1 = new VECTORINT;//左点搜索范围临时
	 VECTORINT* pointRagei2 = new VECTORINT;//右点搜索范围临时
	 VECTORINT* pointRagei = new VECTORINT; //总的搜索范围
	 //VECTORDOUBLE* Angle = new VECTORDOUBLE;//每个点与直线所构成的角度数组
	 VECTORINT* AngleIndex;//Angle进行排序的垂直矩阵
	 VECTORDOUBLE* RageAngle;//搜索范围内对应方向的角度
	 VECTORDOUBLE* RageAllAngle;//搜索范围内所有角度
	 int LineIndex;



	 switch(sonPartIndex) 
	 {
	 case 0:
		 {
			 /*if(pMyLine.LeftPoint == 1120 && pMyLine.RightPoint == 1114)
			 {
				 int i = 0;
			 }*/
			 LineIndex = GetSonPartLineIndex(pMyLine , 0);//在lineFlie中找到pMyLine索引
			 

			 VECTORINT* RmvPoint;
			 RmvPoint = GetSonPartRmvPoint(LineIndex , 0);
			 bool createL1 = false;//判断新生成的边是不是已经存入了数组
			 bool createL2 = false;
			 bool Finish = false;

			 

			 /*pointRagei1 = sonPartPointRage0->at(pMyLine.LeftPoint).getAllRageIndex();
			 pointRagei2 = sonPartPointRage0->at(pMyLine.RightPoint).getAllRageIndex();*/


			 /*if(pMyLine.LeftPoint == 1120 && pMyLine.RightPoint == 1114)
			 {
				 int i = 0;
			 }*/


			 //为了防止sonPartPointRage0中的数值发生改变，使用assign方法进行赋值
			 pointRagei1->assign(sonPartPointRage0->at(pMyLine.LeftPoint).getAllRageIndex()->begin() , sonPartPointRage0->at(pMyLine.LeftPoint).getAllRageIndex()->end());
			 pointRagei2->assign(sonPartPointRage0->at(pMyLine.RightPoint).getAllRageIndex()->begin() , sonPartPointRage0->at(pMyLine.RightPoint).getAllRageIndex()->end());
			 pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
			 pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());
			 RageAllAngle = getSonPartRageAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart);
			 if(RmvPoint->size() == 0)
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , -1);
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , -1);
			 }
			 else
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , RmvPoint->at(0));
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
			 }

			 unsigned int AngleIndexSize = AngleIndex->size();
			 unsigned int i;
			 for(i = 0 ; i < AngleIndexSize ; i++)
			 {
				 MyPoint center;//外接圆圆心
				 center = GetCircumCenter(sonPointPart->at(AngleIndex->at(i)) , sonPointPart->at(pMyLine.LeftPoint) , sonPointPart->at(pMyLine.RightPoint));
				 double r;//半径
				 r = GetR(center , sonPointPart->at(AngleIndex->at(i)));
				 if(sonPartJudgeRule(r , center , RmvPoint , sonPointPart , pointRagei , pMyLine , AngleIndex->at(i)))
				 {

					 /********************************
					 判断新生成的线段是不是符合要求
					 **********************************/
					 
					 CreaLine1 = AddLinePoint(AngleIndex->at(i), pMyLine.LeftPoint , sonPointPart);
					 CreaLine2 = AddLinePoint(AngleIndex->at(i), pMyLine.RightPoint ,sonPointPart);

					 if(ExistLine(lineSonPart0 , CreaLine1) && ExistLine(lineSonPart0 , CreaLine2))//两条边都已经存在，所以不许再次存入，但并不代表三角形已经存在
					 {
						 createL1 = true;
						 createL2 = true;
					 }
					 else if(ExistLine(lineSonPart0 , CreaLine1) && !ExistLine(lineSonPart0 , CreaLine2))//CreaLine1存在，所以不必存入，CreaLine2不存在，进行存储
					 {
						 createL1 = true;
						 lineSonPart0->push_back(CreaLine2);
					 }
					 else if(!ExistLine(lineSonPart0 , CreaLine1) && ExistLine(lineSonPart0 , CreaLine2))
					 {
						 createL2 = true;
						 lineSonPart0->push_back(CreaLine1);
					 }
					 else if(!ExistLine(lineSonPart0, CreaLine1)&&!ExistLine(lineSonPart0, CreaLine2))
					 {
						 lineSonPart0->push_back(CreaLine1);
						 lineSonPart0->push_back(CreaLine2);
					 }
					 else
					 {
						 cout<< "存入子块"<<sonPartIndex<<"线文件时出错！"<< endl;
					 }


					 /***************************
					 判断新生成的三角形是不是符合要求
					 *****************************/
					 MyTri CreaTri;
					 CreaTri.Line1 = GetSonPartLineIndex(CreaLine1 , 0);
					 CreaTri.Line2 = GetSonPartLineIndex(CreaLine2 , 0);
					 CreaTri.Line3 = LineIndex;

					 CreaTri.p1 = CreaLine1.LeftPoint;
					 CreaTri.p2 = CreaLine1.RightPoint;
					 if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
					 {
						 CreaTri.p3 = CreaLine2.RightPoint;
					 }
					 else
					 {
						 CreaTri.p3 = CreaLine2.LeftPoint;
					 }

					 if(ExistTri(triSonPart0 , CreaTri))
					 {
						 delete RmvPoint;//如果新三角形已存在则不存入
			 			 delete AngleIndex;
						 delete RageAngle;
						 delete RageAllAngle;
			 			 delete pointRagei;
						 delete pointRagei1;
						 delete pointRagei2;
			 			 return;            
					 }

					 triSonPart0->push_back(CreaTri);
					 triSonPartTemp0->push_back(CreaTri);

					 Finish = true;
					 CreaLine1 = lineSonPart0->at(CreaTri.Line1);
					 CreaLine2 = lineSonPart0->at(CreaTri.Line2);
					 break;
					
				 }
			 }
			 
			 /***************************
			 对新生成的三角形的两条基边进行新的生长，如果Finish为false，则该生长线下无法生成新的三角形，停止生长
			 如果AngleIndex存储的角度顺序数组已经从开始找到最后，都没找到合适的第三点，则停止生长
			 两个条件其实时相同意思
			 ***************************/
			 if(Finish && i < AngleIndexSize)
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;

				 if(createL1&&!createL2)//由于CreaLine1已经存储了，所以不可能将其作为基边进行生长，因为每条边最多只能包括两个三角形
				 {
					 DealSonPartTriangle(CreaLine2 , sonPointPart , 0);
				 }
				 else if(createL2&&!createL1)
				 {
					 DealSonPartTriangle(CreaLine1 , sonPointPart , 0);
				 }
				 else if(!createL1&&!createL2)
				 {
					 DealSonPartTriangle(CreaLine1 , sonPointPart , 0);
					 DealSonPartTriangle(CreaLine2 , sonPointPart , 0);
				 }
				 else
				 {
					 return;
				 }

			 }
			 else
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 return;
			 }
			 break;
			

		 }
	 case 1:
		 {
			 LineIndex = GetSonPartLineIndex(pMyLine , 1);//在lineFlie中找到pMyLine索引
			 VECTORINT* RmvPoint;
			 RmvPoint = GetSonPartRmvPoint(LineIndex , 1);
			 bool createL1 = false;//判断新生成的边是不是已经存入了数组
			 bool createL2 = false;
			 bool Finish = false;

			 /*pointRagei1 = sonPartPointRage1->at(pMyLine.LeftPoint).getAllRageIndex();
			 pointRagei2 = sonPartPointRage1->at(pMyLine.RightPoint).getAllRageIndex();*/

			 /*if(pMyLine.LeftPoint == 73 && pMyLine.RightPoint == 622)
			 {
				 int i = 0;
			 }*/

			 pointRagei1->assign(sonPartPointRage1->at(pMyLine.LeftPoint).getAllRageIndex()->begin() , sonPartPointRage1->at(pMyLine.LeftPoint).getAllRageIndex()->end());
			 pointRagei2->assign(sonPartPointRage1->at(pMyLine.RightPoint).getAllRageIndex()->begin() , sonPartPointRage1->at(pMyLine.RightPoint).getAllRageIndex()->end());

			 pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
			 pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());
			 RageAllAngle = getSonPartRageAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart);
			 if(RmvPoint->size() == 0)
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , -1);
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , -1);
			 }
			 else
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , RmvPoint->at(0));
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
			 }

			 unsigned int AngleIndexSize = AngleIndex->size();
			 unsigned int i;
			 for(i = 0 ; i < AngleIndexSize ; i++)
			 {
				 MyPoint center;//外接圆圆心
				 center = GetCircumCenter(sonPointPart->at(AngleIndex->at(i)) , sonPointPart->at(pMyLine.LeftPoint) , sonPointPart->at(pMyLine.RightPoint));
				 double r;//半径
				 r = GetR(center , sonPointPart->at(AngleIndex->at(i)));
				 if(sonPartJudgeRule(r , center , RmvPoint , sonPointPart , pointRagei , pMyLine , AngleIndex->at(i)))
				 {

					 /********************************
					 判断新生成的线段是不是符合要求
					 **********************************/
					 
					 CreaLine1 = AddLinePoint(AngleIndex->at(i), pMyLine.LeftPoint , sonPointPart);
					 CreaLine2 = AddLinePoint(AngleIndex->at(i), pMyLine.RightPoint ,sonPointPart);

					 if(ExistLine(lineSonPart1 , CreaLine1) && ExistLine(lineSonPart1 , CreaLine2))//两条边都已经存在，所以不许再次存入，但并不代表三角形已经存在
					 {
						 createL1 = true;
						 createL2 = true;
					 }
					 else if(ExistLine(lineSonPart1 , CreaLine1) && !ExistLine(lineSonPart1 , CreaLine2))//CreaLine1存在，所以不必存入，CreaLine2不存在，进行存储
					 {
						 createL1 = true;
						 lineSonPart1->push_back(CreaLine2);
					 }
					 else if(!ExistLine(lineSonPart1 , CreaLine1) && ExistLine(lineSonPart1 , CreaLine2))
					 {
						 createL2 = true;
						 lineSonPart1->push_back(CreaLine1);
					 }
					 else if(!ExistLine(lineSonPart1, CreaLine1)&&!ExistLine(lineSonPart1, CreaLine2))
					 {
						 lineSonPart1->push_back(CreaLine1);
						 lineSonPart1->push_back(CreaLine2);
					 }
					 else
					 {
						 cout<< "存入子块"<<1<<"线文件时出错！"<< endl;
					 }


					 /***************************
					 判断新生成的三角形是不是符合要求
					 *****************************/
					 MyTri CreaTri;
					 CreaTri.Line1 = GetSonPartLineIndex(CreaLine1 , 1);
					 CreaTri.Line2 = GetSonPartLineIndex(CreaLine2 , 1);
					 CreaTri.Line3 = LineIndex;

					 CreaTri.p1 = CreaLine1.LeftPoint;
					 CreaTri.p2 = CreaLine1.RightPoint;
					 if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
					 {
						 CreaTri.p3 = CreaLine2.RightPoint;
					 }
					 else
					 {
						 CreaTri.p3 = CreaLine2.LeftPoint;
					 }

					 if(ExistTri(triSonPart1 , CreaTri))
					 {
						 delete RmvPoint;//如果新三角形已存在则不存入
			 			 delete AngleIndex;
						 delete RageAngle;
						 delete RageAllAngle;
			 			 delete pointRagei;
						 delete pointRagei1;
						 delete pointRagei2;
			 			 return;            
					 }

					 triSonPart1->push_back(CreaTri);
					 triSonPartTemp1->push_back(CreaTri);

					 Finish = true;
					 CreaLine1 = lineSonPart1->at(CreaTri.Line1);
					 CreaLine2 = lineSonPart1->at(CreaTri.Line2);
					 break;
					
				 }
			 }
			 
			 /***************************
			 对新生成的三角形的两条基边进行新的生长，如果Finish为false，则该生长线下无法生成新的三角形，停止生长
			 如果AngleIndex存储的角度顺序数组已经从开始找到最后，都没找到合适的第三点，则停止生长
			 两个条件其实时相同意思
			 ***************************/
			 if(Finish && i < AngleIndexSize)
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;

				 if(createL1&&!createL2)//由于CreaLine1已经存储了，所以不可能将其作为基边进行生长，因为每条边最多只能包括两个三角形
				 {
					 DealSonPartTriangle(CreaLine2 , sonPointPart , 1);
				 }
				 else if(createL2&&!createL1)
				 {
					 DealSonPartTriangle(CreaLine1 , sonPointPart , 1);
				 }
				 else if(!createL1&&!createL2)
				 {
					 DealSonPartTriangle(CreaLine1 , sonPointPart , 1);
					 DealSonPartTriangle(CreaLine2 , sonPointPart , 1);
				 }
				 else
				 {
					 return;
				 }

			 }
			 else
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle; 
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 return;
			 }
			 break;//switch下不一样
			

		 }
		 case 2:
		 {
			 LineIndex = GetSonPartLineIndex(pMyLine , 2);//在lineFlie中找到pMyLine索引
			 VECTORINT* RmvPoint;
			 RmvPoint = GetSonPartRmvPoint(LineIndex , 2);
			 bool createL1 = false;//判断新生成的边是不是已经存入了数组
			 bool createL2 = false;
			 bool Finish = false;

			 /*pointRagei1 = sonPartPointRage2->at(pMyLine.LeftPoint).getAllRageIndex();
			 pointRagei2 = sonPartPointRage2->at(pMyLine.RightPoint).getAllRageIndex();*/
			 pointRagei1->assign(sonPartPointRage2->at(pMyLine.LeftPoint).getAllRageIndex()->begin() , sonPartPointRage2->at(pMyLine.LeftPoint).getAllRageIndex()->end());
			 pointRagei2->assign(sonPartPointRage2->at(pMyLine.RightPoint).getAllRageIndex()->begin() , sonPartPointRage2->at(pMyLine.RightPoint).getAllRageIndex()->end());

			 pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
			 pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());
			 RageAllAngle = getSonPartRageAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart);
			 if(RmvPoint->size() == 0)
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , -1);
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , -1);
			 }
			 else
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , RmvPoint->at(0));
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
			 }

			 unsigned int AngleIndexSize = AngleIndex->size();
			 unsigned int i;
			 for(i = 0 ; i < AngleIndexSize ; i++)
			 {
				 MyPoint center;//外接圆圆心
				 center = GetCircumCenter(sonPointPart->at(AngleIndex->at(i)) , sonPointPart->at(pMyLine.LeftPoint) , sonPointPart->at(pMyLine.RightPoint));
				 double r;//半径
				 r = GetR(center , sonPointPart->at(AngleIndex->at(i)));
				 if(sonPartJudgeRule(r , center , RmvPoint , sonPointPart , pointRagei , pMyLine , AngleIndex->at(i)))
				 {

					 /********************************
					 判断新生成的线段是不是符合要求
					 **********************************/
					 
					 CreaLine1 = AddLinePoint(AngleIndex->at(i), pMyLine.LeftPoint , sonPointPart);
					 CreaLine2 = AddLinePoint(AngleIndex->at(i), pMyLine.RightPoint ,sonPointPart);

					 if(ExistLine(lineSonPart2 , CreaLine1) && ExistLine(lineSonPart2 , CreaLine2))//两条边都已经存在，所以不许再次存入，但并不代表三角形已经存在
					 {
						 createL1 = true;
						 createL2 = true;
					 }
					 else if(ExistLine(lineSonPart2 , CreaLine1) && !ExistLine(lineSonPart2 , CreaLine2))//CreaLine1存在，所以不必存入，CreaLine2不存在，进行存储
					 {
						 createL1 = true;
						 lineSonPart2->push_back(CreaLine2);
					 }
					 else if(!ExistLine(lineSonPart2 , CreaLine1) && ExistLine(lineSonPart2 , CreaLine2))
					 {
						 createL2 = true;
						 lineSonPart2->push_back(CreaLine1);
					 }
					 else if(!ExistLine(lineSonPart2 , CreaLine1)&&!ExistLine(lineSonPart2 , CreaLine2))
					 {
						 lineSonPart2->push_back(CreaLine1);
						 lineSonPart2->push_back(CreaLine2);
					 }
					 else
					 {
						 cout<< "存入子块"<<2<<"线文件时出错！"<< endl;
					 }


					 /***************************
					 判断新生成的三角形是不是符合要求
					 *****************************/
					 MyTri CreaTri;
					 CreaTri.Line1 = GetSonPartLineIndex(CreaLine1 , 2);
					 CreaTri.Line2 = GetSonPartLineIndex(CreaLine2 , 2);
					 CreaTri.Line3 = LineIndex;

					 CreaTri.p1 = CreaLine1.LeftPoint;
					 CreaTri.p2 = CreaLine1.RightPoint;
					 if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
					 {
						 CreaTri.p3 = CreaLine2.RightPoint;
					 }
					 else
					 {
						 CreaTri.p3 = CreaLine2.LeftPoint;
					 }

					 if(ExistTri(triSonPart2 , CreaTri))
					 {
						 delete RmvPoint;//如果新三角形已存在则不存入
			 			 delete AngleIndex;
						 delete RageAngle;
						 delete RageAllAngle;
			 			 delete pointRagei;
						 delete pointRagei1;
						 delete pointRagei2;
			 			 return;            
					 }

					 triSonPart2->push_back(CreaTri);
					 triSonPartTemp2->push_back(CreaTri);

					 Finish = true;
					 CreaLine1 = lineSonPart2->at(CreaTri.Line1);
					 CreaLine2 = lineSonPart2->at(CreaTri.Line2);
					 break;
					
				 }
			 }
			 
			 /***************************
			 对新生成的三角形的两条基边进行新的生长，如果Finish为false，则该生长线下无法生成新的三角形，停止生长
			 如果AngleIndex存储的角度顺序数组已经从开始找到最后，都没找到合适的第三点，则停止生长
			 两个条件其实时相同意思
			 ***************************/
			 if(Finish && i < AngleIndexSize)
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;

				 if(createL1&&!createL2)//由于CreaLine1已经存储了，所以不可能将其作为基边进行生长，因为每条边最多只能包括两个三角形
				 {
					 DealSonPartTriangle(CreaLine2 , sonPointPart , 2);
				 }
				 else if(createL2&&!createL1)
				 {
					 DealSonPartTriangle(CreaLine1 , sonPointPart , 2);
				 }
				 else if(!createL1&&!createL2)
				 {
					 DealSonPartTriangle(CreaLine1 , sonPointPart , 2);
					 DealSonPartTriangle(CreaLine2 , sonPointPart , 2);
				 }
				 else
				 {
					 return;
				 }

			 }
			 else
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 return;
			 }
			 break;
			

		 }
		 case 3:
		 {
			 LineIndex = GetSonPartLineIndex(pMyLine , 3);//在lineFlie中找到pMyLine索引
			 VECTORINT* RmvPoint;
			 RmvPoint = GetSonPartRmvPoint(LineIndex , 3);
			 bool createL1 = false;//判断新生成的边是不是已经存入了数组
			 bool createL2 = false;
			 bool Finish = false;

			 /*pointRagei1 = sonPartPointRage3->at(pMyLine.LeftPoint).getAllRageIndex();
			 pointRagei2 = sonPartPointRage3->at(pMyLine.RightPoint).getAllRageIndex();*/
			 pointRagei1->assign(sonPartPointRage3->at(pMyLine.LeftPoint).getAllRageIndex()->begin() , sonPartPointRage3->at(pMyLine.LeftPoint).getAllRageIndex()->end());
			 pointRagei2->assign(sonPartPointRage3->at(pMyLine.RightPoint).getAllRageIndex()->begin() , sonPartPointRage3->at(pMyLine.RightPoint).getAllRageIndex()->end());

			 pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
			 pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());
			 RageAllAngle = getSonPartRageAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart);
			 if(RmvPoint->size() == 0)
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , -1);
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , -1);
			 }
			 else
			 {
				 RageAngle = getSonPartRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , sonPointPart , RmvPoint->at(0));
				 AngleIndex = getSonPartAngleIndex(RageAngle , RageAllAngle , pointRagei , sonPointPart , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
			 }

			 unsigned int AngleIndexSize = AngleIndex->size();
			 unsigned int i;
			 for(i = 0 ; i < AngleIndexSize ; i++)
			 {
				 MyPoint center;//外接圆圆心
				 center = GetCircumCenter(sonPointPart->at(AngleIndex->at(i)) , sonPointPart->at(pMyLine.LeftPoint) , sonPointPart->at(pMyLine.RightPoint));
				 double r;//半径
				 r = GetR(center , sonPointPart->at(AngleIndex->at(i)));
				 if(sonPartJudgeRule(r , center , RmvPoint , sonPointPart , pointRagei , pMyLine , AngleIndex->at(i)))
				 {

					 /********************************
					 判断新生成的线段是不是符合要求
					 **********************************/
					 
					 CreaLine1 = AddLinePoint(AngleIndex->at(i), pMyLine.LeftPoint , sonPointPart);
					 CreaLine2 = AddLinePoint(AngleIndex->at(i), pMyLine.RightPoint ,sonPointPart);

					 if(ExistLine(lineSonPart3 , CreaLine1) && ExistLine(lineSonPart3 , CreaLine2))//两条边都已经存在，所以不许再次存入，但并不代表三角形已经存在
					 {
						 createL1 = true;
						 createL2 = true;
					 }
					 else if(ExistLine(lineSonPart3 , CreaLine1) && !ExistLine(lineSonPart3 , CreaLine2))//CreaLine1存在，所以不必存入，CreaLine2不存在，进行存储
					 {
						 createL1 = true;
						 lineSonPart3->push_back(CreaLine2);
					 }
					 else if(!ExistLine(lineSonPart3 , CreaLine1) && ExistLine(lineSonPart3 , CreaLine2))
					 {
						 createL2 = true;
						 lineSonPart3->push_back(CreaLine1);
					 }
					 else if(!ExistLine(lineSonPart3 , CreaLine1)&&!ExistLine(lineSonPart3 , CreaLine2))
					 {
						 lineSonPart3->push_back(CreaLine1);
						 lineSonPart3->push_back(CreaLine2);
					 }
					 else
					 {
						 cout<< "存入子块"<<3<<"线文件时出错！"<< endl;
					 }


					 /***************************
					 判断新生成的三角形是不是符合要求
					 *****************************/
					 MyTri CreaTri;
					 CreaTri.Line1 = GetSonPartLineIndex(CreaLine1 , 3);
					 CreaTri.Line2 = GetSonPartLineIndex(CreaLine2 , 3);
					 CreaTri.Line3 = LineIndex;

					 CreaTri.p1 = CreaLine1.LeftPoint;
					 CreaTri.p2 = CreaLine1.RightPoint;
					 if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
					 {
						 CreaTri.p3 = CreaLine2.RightPoint;
					 }
					 else
					 {
						 CreaTri.p3 = CreaLine2.LeftPoint;
					 }

					 if(ExistTri(triSonPart3 , CreaTri))
					 {
						 delete RmvPoint;//如果新三角形已存在则不存入
			 			 delete AngleIndex;
						 delete RageAngle;
						 delete RageAllAngle;
			 			 delete pointRagei;
						 delete pointRagei1;
						 delete pointRagei2;
			 			 return;            
					 }

					 triSonPart3->push_back(CreaTri);
					 triSonPartTemp3->push_back(CreaTri);

					 Finish = true;
					 CreaLine1 = lineSonPart3->at(CreaTri.Line1);
					 CreaLine2 = lineSonPart3->at(CreaTri.Line2);
					 break;
					
				 }
			 }
			 
			 /***************************
			 对新生成的三角形的两条基边进行新的生长，如果Finish为false，则该生长线下无法生成新的三角形，停止生长
			 如果AngleIndex存储的角度顺序数组已经从开始找到最后，都没找到合适的第三点，则停止生长
			 两个条件其实时相同意思
			 ***************************/
			 if(Finish && i < AngleIndexSize)
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;

				 if(createL1&&!createL2)//由于CreaLine1已经存储了，所以不可能将其作为基边进行生长，因为每条边最多只能包括两个三角形
				 {
					 DealSonPartTriangle(CreaLine2 , sonPointPart , 3);
				 }
				 else if(createL2&&!createL1)
				 {
					 DealSonPartTriangle(CreaLine1 , sonPointPart , 3);
				 }
				 else if(!createL1&&!createL2)
				 {
					 DealSonPartTriangle(CreaLine1 , sonPointPart , 3);
					 DealSonPartTriangle(CreaLine2 , sonPointPart , 3);
				 }
				 else
				 {
					 return;
				 }

			 }
			 else
			 {
				 delete RmvPoint;
				 delete AngleIndex;
				 delete RageAngle;
				 delete RageAllAngle;
				 delete pointRagei;
				 delete pointRagei1;
				 delete pointRagei2;
				 return;
			 }
			 break;
			

		 }

		 default:
			 cout<<"再构网时出错，子块为："<<sonPartIndex<<endl;
			 break;
		 

	 }



	 //LineIndex = GetLineIndex(pMyLine);//在lineFlie中找到pMyLine索引
	 //VECTORINT* RmvPoint;
	 ///*if(pMyLine.LeftPoint == 5207)
	 //{
		// cout<<getstring(TriFile->size())<<endl;
		// cout<<getstring(pointRage->at(pMyLine.LeftPoint).getRageSize())<<endl;
	 //}*/
	 //RmvPoint = GetRmvPoint(LineIndex);//LineIndex所对应三角形的另一个点的索引
	 //bool createL1 = false;
	 //bool createL2 = false;
	 //bool Finish = false;
	 //
	 //pointRagei1->assign(pointRage->at(pMyLine.LeftPoint).getAllRageIndex()->begin() , pointRage->at(pMyLine.LeftPoint).getAllRageIndex()->end());
	 //pointRagei2->assign(pointRage->at(pMyLine.RightPoint).getAllRageIndex()->begin() , pointRage->at(pMyLine.RightPoint).getAllRageIndex()->end());
	 //pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
  //   pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());	//对应搜索范围的点
	 //RageAllAngle = getRageAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , rPoint);//整个搜索范围内的点的角度，与pointRagei对应
	 //if(RmvPoint->size() == 0)
	 //{
		// RageAngle = getRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , rPoint , -1);//边界线对应搜索点的角度
		// AngleIndex = GetAngleIndex(RageAngle , RageAllAngle ,  pointRagei , rPoint , pMyLine.LeftPoint , pMyLine.RightPoint , -1);//排序
	 //}
	 //else
	 //{
		// RageAngle = getRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , rPoint , RmvPoint->at(0));
		// AngleIndex = GetAngleIndex(RageAngle , RageAllAngle ,  pointRagei , rPoint , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
	 //}
	 ////AngleIndex = GetAngleIndex(RageAngle , RageAllAngle ,  pointRagei , rPoint , pMyLine.LeftPoint , pMyLine.RightPoint ,);

	 //unsigned int AngleIndexsize = AngleIndex->size();
	 //unsigned int i;
	 ///*try{*/
	 //for(i = 0 ; i < AngleIndexsize ; i++)
	 //{
		// MyPoint Center;
		// Center = GetCircumCenter(rPoint->at(AngleIndex->at(i)), rPoint->at(pMyLine.LeftPoint), rPoint->at(pMyLine.RightPoint));
		// double r;   
		// r = GetR(Center, rPoint->at(AngleIndex->at(i)));
		// if (JudgeRule(r, Center, RmvPoint , rPoint , pointRagei , pMyLine , AngleIndex->at(i)))
		// {
		//	 CreaLine1 = AddLinePoint(AngleIndex->at(i), pMyLine.LeftPoint , rPoint);
		//	 CreaLine2 = AddLinePoint(AngleIndex->at(i), pMyLine.RightPoint ,rPoint);
		//	 if (ExistLine(lineFile, CreaLine1) && ExistLine(lineFile, CreaLine2))
  //           { 
		//	 	/*delete RmvPoint;
		//	 	delete AngleIndex;
		//		delete RageAngle;
		//		delete RageAllAngle;
		//	 	delete pointRagei;
		//		delete pointRagei1;
		//		delete pointRagei2;
		//	 	return; */
		//		 createL1 = true;//两条线都已存入lineFile,但有可能这两条线所组成三角形没存入
		//		 createL2 = true;
		//	 }
		//	 else if(ExistLine(lineFile, CreaLine1)&&!ExistLine(lineFile, CreaLine2))
		//	 {
		//		 createL1 = true;//CreaLine1存入
		//		 lineFile->push_back(CreaLine2);
		//	 }
		//	 else if(!ExistLine(lineFile, CreaLine1)&&ExistLine(lineFile, CreaLine2))
		//	 {
		//		 createL2 = true;//CreaLine2存入
		//		 lineFile->push_back(CreaLine1);  
		//	 }
		//	 else if(!ExistLine(lineFile, CreaLine1)&&!ExistLine(lineFile, CreaLine2))
		//	 {
		//		lineFile->push_back(CreaLine1);  //都没存入
		//		lineFile->push_back(CreaLine2);
		//	 }
		//	 else 
		//	 {
		//		 cout<<"存入线时出错"<<endl;
		//	 }
		//	 MyTri CreaTri;
		//	 CreaTri.Line1 = GetLineIndex(CreaLine1);
  //           CreaTri.Line2 = GetLineIndex(CreaLine2);
  //           CreaTri.Line3 = LineIndex;

		//	 CreaTri.p1 = CreaLine1.LeftPoint;
		//	 CreaTri.p2 = CreaLine1.RightPoint;
		//	 if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
		//	 {
		//		 CreaTri.p3 = CreaLine2.RightPoint;
		//	 }
		//	 else
		//	 {
		//		  CreaTri.p3 = CreaLine2.LeftPoint;
		//	 }
		//	 if (ExistTri(TriFile, CreaTri))
  //           {
		//		delete RmvPoint;//如果新三角形已存在则不存入
		//	 	delete AngleIndex;
		//		delete RageAngle;
		//		delete RageAllAngle;
		//	 	delete pointRagei;
		//		delete pointRagei1;
		//		delete pointRagei2;
		//	 	return;             
  //           
  //           }

		//	 
		//	/* if(CreaLine2.LeftPoint != CreaTri.p1)
		//	 {
		//		 CreaTri.p2 = CreaLine2.LeftPoint;
		//	 }
		//	 else
		//	 {
		//		 CreaTri.p2 = CreaLine2.RightPoint;
		//	 }*/

		//	 /*if(lineFile->at(LineIndex).LeftPoint != CreaTri.p1&&lineFile->at(LineIndex).LeftPoint != CreaTri.p2)
		//	 {
		//		 CreaTri.p3 = lineFile->at(LineIndex).LeftPoint;
		//	 }
		//	 else
		//	 {
		//		 CreaTri.p3 = lineFile->at(LineIndex).RightPoint;
		//	 }*/


		//	 TriFile->push_back(CreaTri);
		//	 TriFile1->push_back(CreaTri);
		//	 //AddTriRule(CreaTri, TriFile->size() - 1 , rPoint);//添加拓扑
		//	 
		//	 Finish = true;
		//	 CreaLine1 = lineFile->at(CreaTri.Line1);
		//	 CreaLine2 = lineFile->at(CreaTri.Line2);
  //           break;

		// }
		// else
  //       {
		// 	
  //           continue;
  //       
  //       
  //       }


	 //}
	 ///*}
	 //catch (std::out_of_range &exc) {  
  //      std::cerr << exc.what() << " Line:" << __LINE__ << " File:" << __FILE__ << endl;  
		//cout<<getstring(TriFile->size())<<endl;
  //  } */



	 //if (Finish && i < AngleIndexsize)
  //   {
		// 
		// /*if(TriFile->size() == 1995)
		// {
		//	 cout<<getstring(TriFile->size())<<endl;
		// }*/
		////delete RmvPoint;
		//delete RmvPoint;
		//delete AngleIndex;
		//delete RageAngle;
		//delete RageAllAngle;
		//delete pointRagei;
		//delete pointRagei1;
		//delete pointRagei2;
		///*if(CreaLine1.LeftPoint == 313||CreaLine2.LeftPoint == 313)
		//{
		//	cout<<getstring(TriFile->size())<<endl;
		//}*/
		//if(createL1&&!createL2)
		//{
		//	DealTriangle(CreaLine2 , rPoint);
		//}
		//if(createL2&&!createL1)
		//{
		//	DealTriangle(CreaLine1 , rPoint);
		//}
		//if(!createL1&&!createL2)
		//{
		//	DealTriangle(CreaLine1 , rPoint);
		//	DealTriangle(CreaLine2 , rPoint);
		//}
		//else
		//{
		//	return;//两条线都已经存过lineFile，不必继续查找新点
		//}
  //      



  //   }
  //  else
  //  {
		////delete RmvPoint;
		//delete RmvPoint;
		//delete AngleIndex;
		//delete RageAngle;
		//delete RageAllAngle;
		//delete pointRagei;
		//delete pointRagei1;
		//delete pointRagei2;
  //      return;
  //  
  //  
  //  }

}

void DealConvexTin()
{
	clock_t start , finish;
	start = clock();
	JudgeLine* judge;
	JudgeLine* judge1;
	JudgeLine* judge2;
	VECTORMYPOINT* convexTemp0 = new VECTORMYPOINT;
	VECTORMYPOINT* convexTemp1 = new VECTORMYPOINT;
	/*VECTORMYPOINT* convexTemp2 = new VECTORMYPOINT;
	VECTORMYPOINT* convexTemp3 = new VECTORMYPOINT;*/
	VECTORMYPOINT* convexTemp4 = new VECTORMYPOINT;
	//VECTORMYPOINT* convexTemp5 = new VECTORMYPOINT;


	

	for(int i = cornerPoint0[1] ; i <= cornerPoint0[2] ; i++)
	{
		convexTemp0->push_back(pointPart0->at(convexSonPart0->at(i)));
	}

	
	for(int j = cornerPoint1[3] ; j <= cornerPoint1[0] ; j++)
	{
		convexTemp0->push_back(pointPart1->at(convexSonPart1->at(j)));
	}
	

	for(int k = cornerPoint2[1] ; k <= cornerPoint2[2] ; k++)
	{
		convexTemp1->push_back(pointPart2->at(convexSonPart2->at(k)));
	}
	

	for(int l = cornerPoint3[3] ; l <= cornerPoint3[0] ; l++)
	{
		convexTemp1->push_back(pointPart3->at(convexSonPart3->at(l)));
	}
	

	/*********************
	0、1和2、3缝隙间点集的构建
	******************/
	for(unsigned int j = cornerPoint0[0] ; j < convexSonPart0->size() - 1 ; j++)
	{
		convexTemp4->push_back(pointPart0->at(convexSonPart0->at(j)));
	}
	for(int i = 0 ; i <= cornerPoint0[1] ; i++)
	{
		convexTemp4->push_back(pointPart0->at(convexSonPart0->at(i)));
	}

	for(unsigned int l = cornerPoint1[0] ; l < convexSonPart1->size() - 1 ; l++)
	{
		convexTemp4->push_back(pointPart1->at(convexSonPart1->at(l)));
	}
	for(int k = 0 ; k <= cornerPoint1[1] ; k++)
	{
		convexTemp4->push_back(pointPart1->at(convexSonPart1->at(k)));
	}
	
	for(int i = cornerPoint3[2] ; i <= cornerPoint3[3] ; i++)
	{
		convexTemp4->push_back(pointPart3->at(convexSonPart3->at(i)));
	}
	for(int j = cornerPoint2[2] ; j <= cornerPoint2[3] ; j++)
	{
		convexTemp4->push_back(pointPart2->at(convexSonPart2->at(j)));
	}

	/*for(unsigned int i = 0 ; i < convexTemp4->size() ; i++)
	{
		for(unsigned int j = i + 1 ; j < convexTemp4->size() ; j++)
		{
			if(convexTemp4->at(i).getX() == convexTemp4->at(j).getX()
				&&convexTemp4->at(i).getY() == convexTemp4->at(j).getY()
				&&convexTemp4->at(i).getZ() == convexTemp4->at(j).getZ())
			{
				int k = 0;
			}
		}
	}*/

	/*************************
	子块0与1之间缝隙边界点的顺序为0子块右下角开始向上，顺时针方向存储
	***************************/
	convexZeroToOnePoint->assign(convexTemp0->begin() , convexTemp0->end());
	//convexZeroToOnePoint->erase(convexZeroToOnePoint->begin() + 19)
	//convexZeroToOnePoint->insert(convexZeroToOnePoint->end() , convexTemp1->begin() , convexTemp1->end());
	for(unsigned int k = 0 ; k < convexZeroToOnePoint->size() - 1 ; k++)
	{
		convexZeroToOneLine->push_back(AddLinePoint(k , k + 1 , convexZeroToOnePoint));//先存入边界线到lineSonPart，等到生成三角形时在存入拓扑关系
		convexZeroToOneLine->at(k).ifHave = true;
	}
	convexZeroToOneLine->push_back(AddLinePoint(convexZeroToOnePoint->size() - 1 , 0 , convexZeroToOnePoint));
	convexZeroToOneLine->at(convexZeroToOnePoint->size() - 1).ifHave = true;
	double dmaxZTO = Get3dDis(convexZeroToOnePoint->at(convexZeroToOneLine->at(0).LeftPoint) , convexZeroToOnePoint->at(convexZeroToOneLine->at(0).RightPoint));
	int dmaxZTOindex = 0;
	for(unsigned int i = 1 ; i < convexZeroToOneLine->size() ; i++)
	{
		double dis = Get3dDis(convexZeroToOnePoint->at(convexZeroToOneLine->at(i).LeftPoint) , convexZeroToOnePoint->at(convexZeroToOneLine->at(i).RightPoint));
		if(dis < dmaxZTO)
		{
			dmaxZTO = dis;
			dmaxZTOindex = i;
		}
	}
	//convexZeroToOneLine->at(dmaxZTOindex).ifHave = false;
	convexZeroToOneLine->at(cornerPoint0[2] - cornerPoint0[1]).ifHave = false;



	/************************
	子块2与3之间缝隙边界点的顺序为2子块右下角开始向上，顺时针方向存储
	**************************/
	convexTwoToThreePoint->assign(convexTemp1->begin() , convexTemp1->end());
	//convexTwoToThreePoint->insert(convexTwoToThreePoint->end() , convexTemp3->begin() , convexTemp3->end());
	for(unsigned int i = 0 ; i < convexTwoToThreePoint->size() - 1 ; i++)
	{
		convexTwoToThreeLine->push_back(AddLinePoint(i , i + 1 , convexTwoToThreePoint));
		convexTwoToThreeLine->at(i).ifHave = true;
	}
	convexTwoToThreeLine->push_back(AddLinePoint(convexTwoToThreePoint->size() - 1 , 0 , convexTwoToThreePoint));
	convexTwoToThreeLine->at(convexTwoToThreePoint->size() - 1).ifHave = true;

	double dmaxTTT = Get3dDis(convexTwoToThreePoint->at(convexTwoToThreeLine->at(0).LeftPoint) , convexTwoToThreePoint->at(convexTwoToThreeLine->at(0).RightPoint));
	int dmaxTTTindex = 0;
	for(unsigned int i = 1 ; i < convexTwoToThreeLine->size() ; i++)
	{
		double dis = Get3dDis(convexTwoToThreePoint->at(convexTwoToThreeLine->at(i).LeftPoint) , convexTwoToThreePoint->at(convexTwoToThreeLine->at(i).RightPoint));
		if(dis < dmaxTTT)
		{
			dmaxTTT = dis;
			dmaxTTTindex = i;
		}
	}
	//convexTwoToThreeLine->at(dmaxTTTindex).ifHave = false;
	convexTwoToThreeLine->at(convexTwoToThreePoint->size() - 1).ifHave = false;

	/***********************
	子块0、1和子块2、3的线文件存储，由子块0左下角开始顺时针进行存储
	**********************/
	convexTopToButtomPoint->assign(convexTemp4->begin() , convexTemp4->end());
	for(unsigned int i = 0 ; i <convexTopToButtomPoint->size() - 1 ; i++)
	{
		convexTopToButtomLine->push_back(AddLinePoint(i , i + 1 , convexTopToButtomPoint));
		convexTopToButtomLine->at(i).ifHave = true;
	}
	convexTopToButtomLine->push_back(AddLinePoint(convexTopToButtomPoint->size() - 1 , 0 , convexTopToButtomPoint));
	convexTopToButtomLine->at(convexTopToButtomPoint->size() - 1).ifHave = true;

	double dmaxTTB = Get3dDis(convexTopToButtomPoint->at(convexTopToButtomLine->at(0).LeftPoint) , convexTopToButtomPoint->at(convexTopToButtomLine->at(0).RightPoint));
	int dmaxTTBindex = 0;
	for(unsigned int i = 1 ; i < convexTopToButtomLine->size() ; i++)
	{
		double dis = Get3dDis(convexTopToButtomPoint->at(convexTopToButtomLine->at(i).LeftPoint) , convexTopToButtomPoint->at(convexTopToButtomLine->at(i).RightPoint));
		/*if(dis == 0)
		{
			int j = 0;
		}*/
		if(dis < dmaxTTB)
		{
			dmaxTTB = dis;
			dmaxTTBindex = i;
		}
	}
	//convexTopToButtomLine->at(dmaxTTBindex).ifHave = false;
	convexTopToButtomLine->at(convexTopToButtomPoint->size() - 1).ifHave = false;



	/*************************
	子块0和子块1之间的三角网的构建
	***********************/
	judge = new JudgeLine(convexZeroToOneLine->at(cornerPoint0[2] - cornerPoint0[1]) , convexZeroToOneLine->at(0));
	while(!judge->ifComplete)
	{
		if(judge->createL1.ifHave && judge->createL2.ifHave)
		{
			bool ifAll = true;//是否遍历所有lineSonPart0中的线段
			for(unsigned int i = 0 ; i < convexZeroToOneLine->size() ; i++)
			{
				if(!convexZeroToOneLine->at(i).ifHave)
				{
					ifAll = false;
					judge = DealConvexTriangle(convexZeroToOneLine->at(i) , convexZeroToOnePoint ,0);
					break;
				}
			}
			if(ifAll)
			{
				judge->ifComplete = true;
			}
		}
		else if (judge->createL1.ifHave && !judge->createL2.ifHave)
		{
			judge = DealConvexTriangle(judge->createL2 , convexZeroToOnePoint ,0);
		}
		else
		{
			judge = DealConvexTriangle(judge->createL1 , convexZeroToOnePoint , 0);
		}
	}

	delete judge;

	/***********************
	子块2和子块3之间的三角网构建
	***********************/
	judge1 = new JudgeLine(convexTwoToThreeLine->at(convexTwoToThreePoint->size() - 1) , convexTwoToThreeLine->at(0));
	while(!judge1->ifComplete)
	{
		if(judge1->createL1.ifHave && judge1->createL2.ifHave)
		{
			bool ifAll = true;//是否遍历所有lineSonPart0中的线段
			for(unsigned int i = 0 ; i < convexTwoToThreeLine->size() ; i++)
			{
				if(!convexTwoToThreeLine->at(i).ifHave)
				{
					ifAll = false;
					judge1 = DealConvexTriangle(convexTwoToThreeLine->at(i) , convexTwoToThreePoint ,1);
					break;
				}
			}
			if(ifAll)
			{
				judge1->ifComplete = true;
			}
		}
		else if (judge1->createL1.ifHave && !judge1->createL2.ifHave)
		{
			judge1 = DealConvexTriangle(judge1->createL2 , convexTwoToThreePoint ,1);
		}
		else
		{
			judge1 = DealConvexTriangle(judge1->createL1 , convexTwoToThreePoint , 1);
		}
	}

	delete judge1;

	/****************
	子块0、1和子块2、3之间的三角网构建
	****************/
	int indexNum = 0;
	judge2 = new JudgeLine(convexTopToButtomLine->at(convexTopToButtomPoint->size() - 1) , convexTopToButtomLine->at(0));
	while(!judge2->ifComplete)
	{
		/*indexNum++;
		if(indexNum == 27)
		{
			break;
		}*/
		if(judge2->createL1.ifHave && judge2->createL2.ifHave)
		{
			bool ifAll = true;//是否遍历所有lineSonPart0中的线段
			for(unsigned int i = 0 ; i < convexTopToButtomLine->size() ; i++)
			{
				if(!convexTopToButtomLine->at(i).ifHave)
				{
					ifAll = false;
					judge2 = DealConvexTriangle(convexTopToButtomLine->at(i) , convexTopToButtomPoint ,2);
					break;
				}
			}
			if(ifAll)
			{
				judge2->ifComplete = true;
			}
		}
		else if (judge2->createL1.ifHave && !judge2->createL2.ifHave)
		{
			judge2 = DealConvexTriangle(judge2->createL2 , convexTopToButtomPoint ,2);
		}
		else
		{
			judge2 = DealConvexTriangle(judge2->createL1 , convexTopToButtomPoint , 2);
		}
	}

	delete judge2;
	finish = clock();
	cout<<"构建缝隙三角网花费的时间："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;


}

void DealSonPartTin(VECTORMYPOINT* sonPointPart , VECTORINT* convexPoints , int index)//1.子块点集；2.边界点集；3.子块编号
{
	clock_t start , finish;
	JudgeLine* judge;
	start = clock();
	MyLine pMyLine;
	int minL = 0;
	double distance , distanceMin;
	unsigned convexPointsize = convexPoints->size() - 2;
	//unsigned int i = 0;

	switch(index)
	{
	case 0:
		{
			for(unsigned int i = 0; i < convexPointsize ; i++)
			{
				lineSonPart0->push_back(AddLinePoint(convexPoints->at(i) , convexPoints->at(i + 1) , sonPointPart));//先存入边界线到lineSonPart，等到生成三角形时在存入拓扑关系
				lineSonPart0->at(i).ifHave = true;//边界边可以作为三角形的边，但不作为生长的边
		
			}
			lineSonPart0->push_back(AddLinePoint(convexPoints->at(convexPointsize) , convexPoints->at(0) , sonPointPart));
			lineSonPart0->at(convexPointsize).ifHave = true;


			distanceMin = Get3dDis(sonPointPart->at(convexPoints->at(0)) , sonPointPart->at(convexPoints->at(1)));
			for(unsigned int i = 0 ; i < lineSonPart0->size() ; i++)
			{
				distance = Get3dDis(sonPointPart->at(lineSonPart0->at(i).LeftPoint) , sonPointPart->at(lineSonPart0->at(i).RightPoint));
				if(distanceMin > distance)
				{
					distanceMin = distance;
					minL = i;
				}
			}
			lineSonPart0->at(minL).ifHave = false;//起始边参与生长

			pMyLine = lineSonPart0->at(minL);

			judge = new JudgeLine(pMyLine , lineSonPart0->at(0));

			while(!judge->ifComplete)
			{
				if(judge->createL1.ifHave && judge->createL2.ifHave)
				{
					bool ifAll = true;//是否遍历所有lineSonPart0中的线段
					for(unsigned int i = 0 ; i < lineSonPart0->size() ; i++)
					{
						if(!lineSonPart0->at(i).ifHave)
						{
							ifAll = false;
							judge = DealSonPartTriangle1(lineSonPart0->at(i) , sonPointPart ,0);
							break;
						}
					}
					if(ifAll)
					{
						judge->ifComplete = true;
					}
				}
				else if (judge->createL1.ifHave && !judge->createL2.ifHave)
				{
					judge = DealSonPartTriangle1(judge->createL2 , sonPointPart ,0);
				}
				else
				{
					judge = DealSonPartTriangle1(judge->createL1 , sonPointPart , 0);
				}
			}

			delete judge;
	
			//DealSonPartTriangle(pMyLine , sonPointPart ,0);

	
			finish = clock();
			cout<<"构建子块"<<index<<"Delaunay三角网花费的时间："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;
			break;
		}
	case 1:
		{
			for(unsigned int i = 0; i < convexPointsize ; i++)
			{
				lineSonPart1->push_back(AddLinePoint(convexPoints->at(i) , convexPoints->at(i + 1) , sonPointPart));//先存入边界线到lineSonPart，等到生成三角形时在存入拓扑关系
				lineSonPart1->at(i).ifHave = true;//边界边可以作为三角形的边，但不作为生长的边
		
			}
			lineSonPart1->push_back(AddLinePoint(convexPoints->at(convexPointsize) , convexPoints->at(0) , sonPointPart));
			lineSonPart1->at(convexPointsize).ifHave = true;


			distanceMin = Get3dDis(sonPointPart->at(convexPoints->at(0)) , sonPointPart->at(convexPoints->at(1)));
			for(unsigned int i = 0 ; i < lineSonPart1->size() ; i++)
			{
				distance = Get3dDis(sonPointPart->at(lineSonPart1->at(i).LeftPoint) , sonPointPart->at(lineSonPart1->at(i).RightPoint));
				if(distanceMin > distance)
				{
					distanceMin = distance;
					minL = i;
				}
			}
			lineSonPart1->at(minL).ifHave = false;
			pMyLine = lineSonPart1->at(minL);
			judge = new JudgeLine(pMyLine , lineSonPart1->at(0));
			while(!judge->ifComplete)
			{
				if(judge->createL1.ifHave && judge->createL2.ifHave)
				{
					bool ifAll = true;//是否遍历所有lineSonPart0中的线段
					for(unsigned int i = 0 ; i < lineSonPart1->size() ; i++)
					{
						if(!lineSonPart1->at(i).ifHave)
						{
							ifAll = false;
							judge = DealSonPartTriangle1(lineSonPart1->at(i) , sonPointPart ,1);
							break;
						}
					}
					if(ifAll)
					{
						judge->ifComplete = true;
					}
				}
				else if (judge->createL1.ifHave && !judge->createL2.ifHave)
				{
					judge = DealSonPartTriangle1(judge->createL2 , sonPointPart ,1);
				}
				else
				{
					judge = DealSonPartTriangle1(judge->createL1 , sonPointPart , 1);
				}
			}

			delete judge;

			//DealSonPartTriangle(pMyLine , sonPointPart , 1);

	
			finish = clock();
			cout<<"构建子块"<<index<<"Delaunay三角网花费的时间："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;
			break;
		}
	case 2:
		{
			for(unsigned int i = 0; i < convexPointsize ; i++)
			{
				lineSonPart2->push_back(AddLinePoint(convexPoints->at(i) , convexPoints->at(i + 1) , sonPointPart));//先存入边界线到lineSonPart，等到生成三角形时在存入拓扑关系
				lineSonPart2->at(i).ifHave = true;//边界边可以作为三角形的边，但不作为生长的边
		
			}
			lineSonPart2->push_back(AddLinePoint(convexPoints->at(convexPointsize) , convexPoints->at(0) , sonPointPart));
			lineSonPart2->at(convexPointsize).ifHave = true;


			distanceMin = Get3dDis(sonPointPart->at(convexPoints->at(0)) , sonPointPart->at(convexPoints->at(1)));
			for(unsigned int i = 0 ; i < lineSonPart2->size() ; i++)
			{
				distance = Get3dDis(sonPointPart->at(lineSonPart2->at(i).LeftPoint) , sonPointPart->at(lineSonPart2->at(i).RightPoint));
				if(distanceMin > distance)
				{
					distanceMin = distance;
					minL = i;
				}
			}
			lineSonPart2->at(minL).ifHave = false;
			pMyLine = lineSonPart2->at(minL);
			judge = new JudgeLine(pMyLine , lineSonPart2->at(0));

			while(!judge->ifComplete)
			{
				if(judge->createL1.ifHave && judge->createL2.ifHave)
				{
					bool ifAll = true;//是否遍历所有lineSonPart0中的线段
					for(unsigned int i = 0 ; i < lineSonPart2->size() ; i++)
					{
						if(!lineSonPart2->at(i).ifHave)
						{
							ifAll = false;
							judge = DealSonPartTriangle1(lineSonPart2->at(i) , sonPointPart ,2);
							break;
						}
					}
					if(ifAll)
					{
						judge->ifComplete = true;
					}
				}
				else if (judge->createL1.ifHave && !judge->createL2.ifHave)
				{
					judge = DealSonPartTriangle1(judge->createL2 , sonPointPart ,2);
				}
				else
				{
					judge = DealSonPartTriangle1(judge->createL1 , sonPointPart , 2);
				}
			}

			delete judge;
			//DealSonPartTriangle(pMyLine , sonPointPart , 2);

	
			finish = clock();
			cout<<"构建子块"<<index<<"Delaunay三角网花费的时间："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;
			break;
		}
	case 3:
		{
			for(unsigned int i = 0; i < convexPointsize ; i++)
			{
				lineSonPart3->push_back(AddLinePoint(convexPoints->at(i) , convexPoints->at(i + 1) , sonPointPart));//先存入边界线到lineSonPart，等到生成三角形时在存入拓扑关系
				lineSonPart3->at(i).ifHave = true;//边界边可以作为三角形的边，但不作为生长的边
		
			}
			lineSonPart3->push_back(AddLinePoint(convexPoints->at(convexPointsize) , convexPoints->at(0) , sonPointPart));
			lineSonPart3->at(convexPointsize).ifHave = true;


			distanceMin = Get3dDis(sonPointPart->at(convexPoints->at(0)) , sonPointPart->at(convexPoints->at(1)));
			for(unsigned int i = 0 ; i < lineSonPart3->size() ; i++)
			{
				distance = Get3dDis(sonPointPart->at(lineSonPart3->at(i).LeftPoint) , sonPointPart->at(lineSonPart3->at(i).RightPoint));
				if(distanceMin > distance)
				{
					distanceMin = distance;
					minL = i;
				}
			}
			lineSonPart3->at(minL).ifHave = false;
			pMyLine = lineSonPart3->at(minL);
			judge = new JudgeLine(pMyLine , lineSonPart3->at(0));
	
			while(!judge->ifComplete)
			{
				if(judge->createL1.ifHave && judge->createL2.ifHave)
				{
					bool ifAll = true;//是否遍历所有lineSonPart0中的线段
					for(unsigned int i = 0 ; i < lineSonPart3->size() ; i++)
					{
						if(!lineSonPart3->at(i).ifHave)
						{
							ifAll = false;
							judge = DealSonPartTriangle1(lineSonPart3->at(i) , sonPointPart ,3);
							break;
						}
					}
					if(ifAll)
					{
						judge->ifComplete = true;
					}
				}
				else if (judge->createL1.ifHave && !judge->createL2.ifHave)
				{
					judge = DealSonPartTriangle1(judge->createL2 , sonPointPart ,3);
				}
				else
				{
					judge = DealSonPartTriangle1(judge->createL1 , sonPointPart , 3);
				}
			}

			delete judge;
			//DealSonPartTriangle(pMyLine , sonPointPart , 3);

	
			finish = clock();
			cout<<"构建子块"<<index<<"Delaunay三角网花费的时间："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;
			break;
			break;
		}
	default:
		cout<<"存入边界线段时出错，错误子块为"<<index<<endl;
		break;
	}
	
	//lineFile->at(i).ifHave = true;
	/*distanceMin = Get3dDis(sonPointPart->at(convexPoints->at(0)) , sonPointPart->at(convexPoints->at(1)));
	for(unsigned int i = 0 ; i < lineFile->size() ; i++)
	{
		distance = Get3dDis(rPoint->at(lineFile->at(i).LeftPoint) , rPoint->at(lineFile->at(i).RightPoint));
		if(distanceMin > distance)
		{
			distanceMin = distance;
			minL = i;
		}
	}
	pMyLine = lineFile->at(minL);
	
	DealTriangle(pMyLine , rPoint);

	
	finish = clock();
	cout<<"构建Delaunay三角网花费的时间："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;*/
	
	
     

}

void *BuildSonTin(void *threadid)//构建子块的三角网，包括子块凸包的确定和子块的构网
{
	struct unionMyPoint* tid = (struct unionMyPoint*)threadid;
	int partIndex = tid->partIndex;
	VECTORMYPOINT* sonPointPart = tid->sonPointPart;
	switch(partIndex)
	{
	case 0:
		getSonPointConvexIndex(sonPointPart , 0);
		DealSonPartTin(sonPointPart , convexSonPart0 , 0);

		break;
	case 1:
		getSonPointConvexIndex(sonPointPart , 1);
		DealSonPartTin(sonPointPart , convexSonPart1 , 1);
		break;
	case 2:
		getSonPointConvexIndex(sonPointPart , 2);
		DealSonPartTin(sonPointPart , convexSonPart2 , 2);
		break;
	case 3:
		getSonPointConvexIndex(sonPointPart , 3);
		DealSonPartTin(sonPointPart , convexSonPart3 , 3);
		break;
	default:
		cout<<"创建子块凸包出错，出错子块编号为："<<partIndex<<endl;
		break;
	}
	pthread_exit(NULL);
	return 0;
}


VECTORDOUBLE* getRageAngle(int p1 , int p2 , VECTORINT* pointRagei , VECTORMYPOINT* rPoint , int p3)//获得搜索范围内角度数组
{
	VECTORDOUBLE* rageAngle = new VECTORDOUBLE;
	unsigned int pointRageisize = pointRagei->size();
	int leftOrright;
	if(p3!=-1)
	{
		leftOrright = jugleLeftOrRight(rPoint->at(p1) , rPoint->at(p2) , rPoint->at(p3));//三角形中另外一个点对于直线的位置，搜索时取另外一边的范围
	}
	else
	{
		leftOrright = -2;//边界线
	}
	if(leftOrright!=-2)
	{
		for(unsigned int i = 0 ; i < pointRageisize ; i++)
		{
			if((jugleLeftOrRight(rPoint->at(p1) , rPoint->at(p2) , rPoint->at(pointRagei->at(i)))!=leftOrright)&&pointRagei->at(i)!=p1&&pointRagei->at(i)!=p2)//搜索相反位置且不为端点
			{
				rageAngle->push_back(getAngle(rPoint->at(p1) , rPoint->at(p2) , rPoint->at(pointRagei->at(i))));
			}
			else
			{
				continue;
			}
			
		}
	}
	else
	{
		for(unsigned int i = 0 ; i < pointRageisize ; i++)
		{

			if(pointRagei->at(i)!=p1&&pointRagei->at(i)!=p2)
			{
				rageAngle->push_back(getAngle(rPoint->at(p1) , rPoint->at(p2) , rPoint->at(pointRagei->at(i))));
			}
			else
			{
				continue;
			}
			
			
		}

	}
	return rageAngle;
}

VECTORDOUBLE* getRageAllAngle(int p1 , int p2 , VECTORINT* pointRagei , VECTORMYPOINT* rPoint)//获得该范围内所有点的角度，与pointRagei个数相对应，方便排序是记录位置索引
{
	VECTORDOUBLE* rageAngle = new VECTORDOUBLE;
	unsigned int pointRageisize = pointRagei->size();
	for(unsigned int i = 0 ; i < pointRageisize ; i++)
	{
		rageAngle->push_back(getAngle(rPoint->at(p1) , rPoint->at(p2) , rPoint->at(pointRagei->at(i))));
	}
	return rageAngle;
}

VECTORINT* GetAngleIndex(VECTORDOUBLE* Angle , VECTORDOUBLE* rageAngle , VECTORINT* pointRagei , VECTORMYPOINT* rPoint , int p1 , int p2 , int p3)//得到垂直角度索引的分布
{
    VECTORINT* result = new VECTORINT;
    VECTORDOUBLE* temp = new  VECTORDOUBLE;
	//VECTORDOUBLE* temp1 = new VECTORDOUBLE; 
	unsigned int Anglelength = Angle->size();
	int leftOrright;
	if(p3!=-1)
	{
		leftOrright = jugleLeftOrRight(rPoint->at(p1) , rPoint->at(p2) , rPoint->at(p3));//三角形中另外一个点对于直线的位置，搜索时取另外一边的范围
	}
	else
	{
		leftOrright = -2;//p3为-1即边界线没有对应形成三角形的点则搜索全部范围
	}
	for (unsigned int i = 0; i < Anglelength; i++)
    {
        //temp.Add(VerTi[i]); 
		temp->push_back(Angle->at(i));
		//temp1->push_back(Angle->at(i));
    }
	sort(temp->begin() , temp->end() , comp);
	unsigned int templength = temp->size();
	unsigned int rageAnglesize = rageAngle->size();
	for (unsigned int i = 0; i < templength; i++)
    {
		//if(temp->at(i)==-1)continue;
		for (unsigned int j = 0; j <rageAnglesize; j++)
        {
			if (temp->at(i) == rageAngle->at(j))
            {
				if(leftOrright!=-2)
				{
					if(jugleLeftOrRight(rPoint->at(p1) , rPoint->at(p2) , rPoint->at(pointRagei->at(j)))!=leftOrright)//搜索该线所组成的上一个三角形对应点方向相反的区域
					{
					
						if(!ifExistcos(pointRagei->at(j) , result)&&pointRagei->at(j)!=p1&&pointRagei->at(j)!=p2)//如果已经查找过该点则跳过该点查找下一个对应角度的点，且不能为线段端点
						{
							result->push_back(pointRagei->at(j));
							break;
						}
						else
						{
							continue;
						}
					}
					else
					{
						continue;
					}
				}
				else
				{
					if(!ifExistcos(pointRagei->at(j) , result)&&pointRagei->at(j)!=p1&&pointRagei->at(j)!=p2)//边界线段查找所有范围内
						{
							result->push_back(pointRagei->at(j));
							break;
						}
						else
						{
							continue;
						}
				}
            }
        
        
        }
    }
	delete temp;
    return result;
    
}

void DealTriangle(MyLine pMyLine , VECTORMYPOINT* rPoint)//构建三角网
{

	 MyLine CreaLine1;
     MyLine CreaLine2;
	 VECTORINT* pointRagei1 = new VECTORINT;//左点搜索范围
	 VECTORINT* pointRagei2 = new VECTORINT;//右点搜索范围
	 VECTORINT* pointRagei = new VECTORINT; //总的搜索范围
	 //VECTORDOUBLE* Angle = new VECTORDOUBLE;//每个点与直线所构成的角度数组
	 VECTORINT* AngleIndex;//Angle进行排序的垂直矩阵
	 VECTORDOUBLE* RageAngle;//搜索范围内对应方向的角度
	 VECTORDOUBLE* RageAllAngle;//搜索范围内所有角度
	 int LineIndex;
	 LineIndex = GetLineIndex(pMyLine);//在lineFlie中找到pMyLine索引
	 VECTORINT* RmvPoint;
	 /*if(pMyLine.LeftPoint == 5207)
	 {
		 cout<<getstring(TriFile->size())<<endl;
		 cout<<getstring(pointRage->at(pMyLine.LeftPoint).getRageSize())<<endl;
	 }*/
	 RmvPoint = GetRmvPoint(LineIndex);//LineIndex所对应三角形的另一个点的索引
	 bool createL1 = false;
	 bool createL2 = false;
	 bool Finish = false;
	 
	 pointRagei1->assign(pointRage->at(pMyLine.LeftPoint).getAllRageIndex()->begin() , pointRage->at(pMyLine.LeftPoint).getAllRageIndex()->end());
	 pointRagei2->assign(pointRage->at(pMyLine.RightPoint).getAllRageIndex()->begin() , pointRage->at(pMyLine.RightPoint).getAllRageIndex()->end());
	 pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
     pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());	//对应搜索范围的点
	 RageAllAngle = getRageAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , rPoint);//整个搜索范围内的点的角度，与pointRagei对应
	 if(RmvPoint->size() == 0)
	 {
		 RageAngle = getRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , rPoint , -1);//边界线对应搜索点的角度
		 AngleIndex = GetAngleIndex(RageAngle , RageAllAngle ,  pointRagei , rPoint , pMyLine.LeftPoint , pMyLine.RightPoint , -1);//排序
	 }
	 else
	 {
		 RageAngle = getRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , rPoint , RmvPoint->at(0));
		 AngleIndex = GetAngleIndex(RageAngle , RageAllAngle ,  pointRagei , rPoint , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
	 }
	 //AngleIndex = GetAngleIndex(RageAngle , RageAllAngle ,  pointRagei , rPoint , pMyLine.LeftPoint , pMyLine.RightPoint ,);

	 unsigned int AngleIndexsize = AngleIndex->size();
	 unsigned int i;
	 /*try{*/
	 for(i = 0 ; i < AngleIndexsize ; i++)
	 {
		 MyPoint Center;
		 Center = GetCircumCenter(rPoint->at(AngleIndex->at(i)), rPoint->at(pMyLine.LeftPoint), rPoint->at(pMyLine.RightPoint));
		 double r;   
		 r = GetR(Center, rPoint->at(AngleIndex->at(i)));
		 if (JudgeRule(r, Center, RmvPoint , rPoint , pointRagei , pMyLine , AngleIndex->at(i)))
		 {
			 CreaLine1 = AddLinePoint(AngleIndex->at(i), pMyLine.LeftPoint , rPoint);
			 CreaLine2 = AddLinePoint(AngleIndex->at(i), pMyLine.RightPoint ,rPoint);
			 if (ExistLine(lineFile, CreaLine1) && ExistLine(lineFile, CreaLine2))
             { 
			 	/*delete RmvPoint;
			 	delete AngleIndex;
				delete RageAngle;
				delete RageAllAngle;
			 	delete pointRagei;
				delete pointRagei1;
				delete pointRagei2;
			 	return; */
				 createL1 = true;//两条线都已存入lineFile,但有可能这两条线所组成三角形没存入
				 createL2 = true;
			 }
			 else if(ExistLine(lineFile, CreaLine1)&&!ExistLine(lineFile, CreaLine2))
			 {
				 createL1 = true;//CreaLine1存入
				 lineFile->push_back(CreaLine2);
			 }
			 else if(!ExistLine(lineFile, CreaLine1)&&ExistLine(lineFile, CreaLine2))
			 {
				 createL2 = true;//CreaLine2存入
				 lineFile->push_back(CreaLine1);  
			 }
			 else if(!ExistLine(lineFile, CreaLine1)&&!ExistLine(lineFile, CreaLine2))
			 {
				lineFile->push_back(CreaLine1);  //都没存入
				lineFile->push_back(CreaLine2);
			 }
			 else 
			 {
				 cout<<"存入线时出错"<<endl;
			 }
			 MyTri CreaTri;
			 CreaTri.Line1 = GetLineIndex(CreaLine1);
             CreaTri.Line2 = GetLineIndex(CreaLine2);
             CreaTri.Line3 = LineIndex;

			 CreaTri.p1 = CreaLine1.LeftPoint;
			 CreaTri.p2 = CreaLine1.RightPoint;
			 if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
			 {
				 CreaTri.p3 = CreaLine2.RightPoint;
			 }
			 else
			 {
				  CreaTri.p3 = CreaLine2.LeftPoint;
			 }
			 if (ExistTri(TriFile, CreaTri))
             {
				delete RmvPoint;//如果新三角形已存在则不存入
			 	delete AngleIndex;
				delete RageAngle;
				delete RageAllAngle;
			 	delete pointRagei;
				delete pointRagei1;
				delete pointRagei2;
			 	return;             
             
             }

			 
			/* if(CreaLine2.LeftPoint != CreaTri.p1)
			 {
				 CreaTri.p2 = CreaLine2.LeftPoint;
			 }
			 else
			 {
				 CreaTri.p2 = CreaLine2.RightPoint;
			 }*/

			 /*if(lineFile->at(LineIndex).LeftPoint != CreaTri.p1&&lineFile->at(LineIndex).LeftPoint != CreaTri.p2)
			 {
				 CreaTri.p3 = lineFile->at(LineIndex).LeftPoint;
			 }
			 else
			 {
				 CreaTri.p3 = lineFile->at(LineIndex).RightPoint;
			 }*/


			 TriFile->push_back(CreaTri);
			 TriFile1->push_back(CreaTri);
			 //AddTriRule(CreaTri, TriFile->size() - 1 , rPoint);//添加拓扑
			 
			 Finish = true;
			 CreaLine1 = lineFile->at(CreaTri.Line1);
			 CreaLine2 = lineFile->at(CreaTri.Line2);
             break;

		 }
		 else
         {
		 	
             continue;
         
         
         }


	 }
	 /*}
	 catch (std::out_of_range &exc) {  
        std::cerr << exc.what() << " Line:" << __LINE__ << " File:" << __FILE__ << endl;  
		cout<<getstring(TriFile->size())<<endl;
    } */



	 if (Finish && i < AngleIndexsize)
     {
		 
		 /*if(TriFile->size() == 1995)
		 {
			 cout<<getstring(TriFile->size())<<endl;
		 }*/
		//delete RmvPoint;
		delete RmvPoint;
		delete AngleIndex;
		delete RageAngle;
		delete RageAllAngle;
		delete pointRagei;
		delete pointRagei1;
		delete pointRagei2;
		/*if(CreaLine1.LeftPoint == 313||CreaLine2.LeftPoint == 313)
		{
			cout<<getstring(TriFile->size())<<endl;
		}*/
		if(createL1&&!createL2)
		{
			DealTriangle(CreaLine2 , rPoint);
		}
		if(createL2&&!createL1)
		{
			DealTriangle(CreaLine1 , rPoint);
		}
		if(!createL1&&!createL2)
		{
			DealTriangle(CreaLine1 , rPoint);
			DealTriangle(CreaLine2 , rPoint);
		}
		else
		{
			return;//两条线都已经存过lineFile，不必继续查找新点
		}
        



     }
    else
    {
		//delete RmvPoint;
		delete RmvPoint;
		delete AngleIndex;
		delete RageAngle;
		delete RageAllAngle;
		delete pointRagei;
		delete pointRagei1;
		delete pointRagei2;
        return;
    
    
    }

}

//JudgeLine* DelDelaunay(MyLine pMyLine , VECTORMYPOINT* rPoint)
//{
//	JudgeLine* judge;
//	MyLine CreaLine1;
//    MyLine CreaLine2;
//	VECTORINT* pointRagei1 = new VECTORINT;//左点搜索范围
//	VECTORINT* pointRagei2 = new VECTORINT;//右点搜索范围
//	VECTORINT* pointRagei = new VECTORINT; //总的搜索范围
//	VECTORINT* AngleIndex;//Angle进行排序的垂直矩阵
//	VECTORDOUBLE* RageAngle;//搜索范围内对应方向的角度
//	VECTORDOUBLE* RageAllAngle;//搜索范围内所有角度
//	int LineIndex;
//	LineIndex = GetLineIndex(pMyLine);//在lineFlie中找到pMyLine索引
//	VECTORINT* RmvPoint;
//	RmvPoint = GetRmvPoint(LineIndex);//LineIndex所对应三角形的另一个点的索引
//	bool Finish = false;
//	pointRagei1->assign(pointRage->at(pMyLine.LeftPoint).getAllRageIndex()->begin() , pointRage->at(pMyLine.LeftPoint).getAllRageIndex()->end());
//	pointRagei2->assign(pointRage->at(pMyLine.RightPoint).getAllRageIndex()->begin() , pointRage->at(pMyLine.RightPoint).getAllRageIndex()->end());
//	pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
//    pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());	//对应搜索范围的点
//	RageAllAngle = getRageAllAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , rPoint);//整个搜索范围内的点的角度，与pointRagei对应
//	if(RmvPoint->size() == 0)
//	{
//	 RageAngle = getRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , rPoint , -1);//边界线对应搜索点的角度
//	 AngleIndex = GetAngleIndex(RageAngle , RageAllAngle ,  pointRagei , rPoint , pMyLine.LeftPoint , pMyLine.RightPoint , -1);//排序
//	}
//	else
//	{
//	 RageAngle = getRageAngle(pMyLine.LeftPoint , pMyLine.RightPoint , pointRagei , rPoint , RmvPoint->at(0));
//	 AngleIndex = GetAngleIndex(RageAngle , RageAllAngle ,  pointRagei , rPoint , pMyLine.LeftPoint , pMyLine.RightPoint , RmvPoint->at(0));
//	}
//	unsigned int AngleIndexsize = AngleIndex->size();
//	unsigned int i;
//	for(i = 0 ; i < AngleIndexsize ; i++)
//	{
//	 MyPoint Center;
//	 Center = GetCircumCenter(rPoint->at(AngleIndex->at(i)), rPoint->at(pMyLine.LeftPoint), rPoint->at(pMyLine.RightPoint));
//	 double r;   
//	 r = GetR(Center, rPoint->at(AngleIndex->at(i)));
//	 if(JudgeRule(r, Center, RmvPoint , rPoint , pointRagei , pMyLine , AngleIndex->at(i)))
//	 {
//		 /*if(AngleIndex->at(i) == 296||AngleIndex->at(i) == 297)
//		 {
//			 cout<<"296"<<endl;
//		 }*/
//		 CreaLine1 = AddLinePoint(AngleIndex->at(i), pMyLine.LeftPoint , rPoint);
//		 CreaLine2 = AddLinePoint(AngleIndex->at(i), pMyLine.RightPoint ,rPoint);
//		 if (ExistLine(lineFile, CreaLine1) && ExistLine(lineFile, CreaLine2))
//         { 
//			 /*judge.setLine1True();
//			 judge.setLine2True();*/
//			 //createL1 = true;//两条线都已存入lineFile,但有可能这两条线所组成三角形没存入
//			 //createL2 = true;
//			 CreaLine1.ifHave = true;
//			 CreaLine2.ifHave = true;
//			 judge = new JudgeLine(CreaLine1 , CreaLine2);
//		 }
//		 else if(ExistLine(lineFile, CreaLine1)&&!ExistLine(lineFile, CreaLine2))
//		 {
//			 //judge.setLine1True();
//			 CreaLine1.ifHave = true;
//			 //createL1 = true;//CreaLine1存入
//			 lineFile->push_back(CreaLine2);
//			 judge = new JudgeLine(CreaLine1 , CreaLine2);
//		 }
//		 else if(!ExistLine(lineFile, CreaLine1)&&ExistLine(lineFile, CreaLine2))
//		 {
//			 //createL2 = true;//CreaLine2存入
//			 CreaLine2.ifHave = true;
//			 lineFile->push_back(CreaLine1); 
//			 judge = new JudgeLine(CreaLine1 , CreaLine2);
//		 }
//		 else if(!ExistLine(lineFile, CreaLine1)&&!ExistLine(lineFile, CreaLine2))
//		 {
//			lineFile->push_back(CreaLine1);  //都没存入
//			lineFile->push_back(CreaLine2);
//			judge = new JudgeLine(CreaLine1 , CreaLine2);
//		 }
//		 else 
//		 {
//			 cout<<"存入线时出错"<<endl;
//		 }
//		 MyTri CreaTri;
//		 CreaTri.Line1 = GetLineIndex(CreaLine1);
//         CreaTri.Line2 = GetLineIndex(CreaLine2);
//         CreaTri.Line3 = LineIndex;
//		 
//
//		 CreaTri.p1 = CreaLine1.LeftPoint;
//		 CreaTri.p2 = CreaLine1.RightPoint;
//		 if(CreaLine2.LeftPoint == CreaTri.p1||CreaLine2.LeftPoint == CreaTri.p2)
//		 {
//			 CreaTri.p3 = CreaLine2.RightPoint;
//		 }
//		 else
//		 {
//			  CreaTri.p3 = CreaLine2.LeftPoint;
//		 }
//		 if (ExistTri(TriFile, CreaTri))//如果新三角形已存在则不存入
//         {
//			delete RmvPoint;
//		 	delete AngleIndex;
//			delete RageAngle;
//			delete RageAllAngle;
//		 	delete pointRagei;
//			delete pointRagei1;
//			delete pointRagei2;
//			lineFile->at(LineIndex).ifHave = true;
//		 	return judge;             
//            
//         }
//
//		 
//		
//
//		 
//		 TriFile->push_back(CreaTri);
//		 TriFile1->push_back(CreaTri);
//		 //AddTriRule(CreaTri, TriFile->size() - 1 , rPoint);//添加拓扑
//		 
//		 Finish = true;
//		 CreaLine1 = lineFile->at(CreaTri.Line1);
//		 CreaLine2 = lineFile->at(CreaTri.Line2);
//         break;
//
//	 }
//	 else
//     {
//	 	
//            continue;
//        
//        
//     }
//
//
//	}
//	if(Finish && i < AngleIndexsize)
//	{
//		delete RmvPoint;
//		delete AngleIndex;
//		delete RageAngle;
//		delete RageAllAngle;
//		delete pointRagei;
//		delete pointRagei1;
//		delete pointRagei2;
//		return judge;
//	}
//	else
//	{
//		delete RmvPoint;
//		delete AngleIndex;
//		delete RageAngle;
//		delete RageAllAngle;
//		delete pointRagei;
//		delete pointRagei1;
//		delete pointRagei2;
//		//judge->ifComplete = true;
//		judge = new JudgeLine();
//		judge->setIfCom(true);
//		return judge;
//	}
//
//}

string getstring ( const double n )//double转string

{

	std::stringstream newstr;
	newstr<<n;
	return newstr.str();

}

//template <class double> //模板函数：将string类型变量转换为常用的数值类型
double stringToNum( const string str )  
{  
	std::istringstream iss(str);  
	double num;  
	iss >> num;  
	return num;      
} 

VECTORMYPOINT* LoadPoint(string file)//载入点数据
{
	clock_t start,finish;  
	start = clock();
	VECTORMYPOINT* rPoint = new VECTORMYPOINT;
	MyPoint point;
	ifstream infile; 
	infile.open(file.data());
	string a , b , c;
	while (!infile.eof())
    {
        infile >> a;
		infile >> b;
		infile >> c;
        //cout<<c<<endl;
		point.setX(stringToNum(a));
		point.setY(stringToNum(b));
		point.setZ(stringToNum(c));
		//point.setZ(stringToNum(c)*4);
		//point.index = rPoint->size();
		rPoint->push_back(point);

    }
	rPoint->pop_back();
    infile.close(); 
	finish = clock();
	cout<<"载入数据所花费时间为："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;
	return rPoint;
}

void GroupPoint()
{
	int maxY = getMaxYpoint(rPoint);
	int minY = getMinYpoint(rPoint);
	int maxX = getMaxXpoint(rPoint);
	int minX = getMinXpoint(rPoint);
	double middleXValue = (rPoint->at(maxX).getX() + rPoint->at(minX).getX())/2;
	double middleYValue = (rPoint->at(maxY).getY() + rPoint->at(minY).getY())/2;

	for(unsigned int i = 0 ; i < rPoint->size() ; i++)
	{
		MyPoint p = rPoint->at(i);
		if(p.getX() < middleXValue)
		{
			if(p.getY() > middleYValue)
			{
				pointPart0->push_back(p);
			}
			else
			{
				pointPart2->push_back(p);
			}
		}
		else
		{
			if(p.getY() > middleYValue)
			{
				pointPart1->push_back(p);
			}
			else
			{
				pointPart3->push_back(p);
			}
		}
	}
}

/**********************
获得每个点的法向量，sonPartPoint用于点数量的确定，真正添加法向量的对象为全局变量
**********************/

void getSonPartPointNormal(VECTORMYPOINT* sonPointPart , int index)//获得每个点的法向量
{
	unsigned int sonPointPartSize = sonPointPart->size();
	double nX;
	double nY;
	double nZ;
	SamePointTri* samePointTri;

	switch(index)
	{
	case 0:
		{
			for(unsigned int i = 0 ; i < sonPointPartSize ; i++)
			{
				nX = 0;
				nY = 0;
				nZ = 0;
				VECTORMYTRI* sameTri;
				samePointTri = new SamePointTri;
				sameTri = getSonPartSamePointTri(triSonPart0 , i);
				samePointTri->clear();
				samePointTri->setPointIndex(i);
				samePointTri->copy(sameTri);
				samePointAboutTri0->push_back(*samePointTri);
				unsigned int sameTriSize = sameTri->size();
				for(unsigned int j = 0 ; j < sameTriSize ; j++)
				{
					nX += sameTri->at(j).normal.getX();
					nY += sameTri->at(j).normal.getY();
					nZ += sameTri->at(j).normal.getZ();
				}
				pointPart0->at(i).normal.X = nX/sameTriSize;
				pointPart0->at(i).normal.Y = nY/sameTriSize;
				pointPart0->at(i).normal.Z = nZ/sameTriSize;
				delete sameTri;

			}
			break;
		}
	case 1:
		{
			for(unsigned int i = 0 ; i < sonPointPartSize ; i++)
			{
				nX = 0;
				nY = 0;
				nZ = 0;
				VECTORMYTRI* sameTri;
				samePointTri = new SamePointTri;
				sameTri = getSonPartSamePointTri(triSonPart1 , i);
				samePointTri->clear();
				samePointTri->setPointIndex(i);
				samePointTri->copy(sameTri);
				samePointAboutTri1->push_back(*samePointTri);
				unsigned int sameTriSize = sameTri->size();
				for(unsigned int j = 0 ; j < sameTriSize ; j++)
				{
					nX += sameTri->at(j).normal.getX();
					nY += sameTri->at(j).normal.getY();
					nZ += sameTri->at(j).normal.getZ();
				}
				pointPart1->at(i).normal.X = nX/sameTriSize;
				pointPart1->at(i).normal.Y = nY/sameTriSize;
				pointPart1->at(i).normal.Z = nZ/sameTriSize;
				delete sameTri;

			}
			break;
		}
	case 2:
		{
			for(unsigned int i = 0 ; i < sonPointPartSize ; i++)
			{
				nX = 0;
				nY = 0;
				nZ = 0;
				VECTORMYTRI* sameTri;
				samePointTri = new SamePointTri;
				sameTri = getSonPartSamePointTri(triSonPart2 , i);
				samePointTri->clear();
				samePointTri->setPointIndex(i);
				samePointTri->copy(sameTri);
				samePointAboutTri2->push_back(*samePointTri);
				unsigned int sameTriSize = sameTri->size();
				for(unsigned int j = 0 ; j < sameTriSize ; j++)
				{
					nX += sameTri->at(j).normal.getX();
					nY += sameTri->at(j).normal.getY();
					nZ += sameTri->at(j).normal.getZ();
				}
				pointPart2->at(i).normal.X = nX/sameTriSize;
				pointPart2->at(i).normal.Y = nY/sameTriSize;
				pointPart2->at(i).normal.Z = nZ/sameTriSize;
				delete sameTri;

			}
			break;
		}
	case 3:
		{
			for(unsigned int i = 0 ; i < sonPointPartSize ; i++)
			{
				nX = 0;
				nY = 0;
				nZ = 0;
				VECTORMYTRI* sameTri;
				samePointTri = new SamePointTri;
				sameTri = getSonPartSamePointTri(triSonPart3 , i);
				samePointTri->clear();
				samePointTri->setPointIndex(i);
				samePointTri->copy(sameTri);
				samePointAboutTri3->push_back(*samePointTri);
				unsigned int sameTriSize = sameTri->size();
				for(unsigned int j = 0 ; j < sameTriSize ; j++)
				{
					nX += sameTri->at(j).normal.getX();
					nY += sameTri->at(j).normal.getY();
					nZ += sameTri->at(j).normal.getZ();
				}
				pointPart3->at(i).normal.X = nX/sameTriSize;
				pointPart3->at(i).normal.Y = nY/sameTriSize;
				pointPart3->at(i).normal.Z = nZ/sameTriSize;
				delete sameTri;

			}
			break;
		}
	default:
		cout<<"进行点法向量求解时出错，错误子块为"<<index<<endl;
		break;
	}

}

//利用向量积可以求出和平面垂直的向量
//设三点坐标为A(x1,y1,z1),B(x2,y2,z2),C(x3,y3,z3)
//向量AB=(x2-x1,y2-y1,z2-z1),AC=(x3-x1,y3-y1,z3-z1)
//AB、AC所在平面的法向量即AB×AC=(a,b,c),其中：
//a=(y2-y1)(z3-z1)-(z2-z1)(y3-y1)
//b=(z2-z1)(x3-x1)-(z3-z1)(x2-x1)
//c=(x2-x1)(y3-y1)-(x3-x1)(y2-y1)
void getSonPartNormal(VECTORMYPOINT* sonPointPart , int index)//法向量
{
	//VECTORMYNORMAL* normalList = new VECTORMYNORMAL;
	//MyNormal normal;
	double a = 0;
	double b = 0;
	double c = 0;
	double X1 = 0;
	double Y1 = 0;
	double Z1 = 0;
	double X2 = 0;
	double Y2 = 0;
	double Z2 = 0;
	int ifleftorright;

	switch(index)
	{
	case 0:
		{
			for(unsigned int i = 0 ; i < triSonPart0->size() ; i++)
			{
				X1 = sonPointPart->at(triSonPart0->at(i).p2).getX() - sonPointPart->at(triSonPart0->at(i).p1).getX();
				Y1 = sonPointPart->at(triSonPart0->at(i).p2).getY() - sonPointPart->at(triSonPart0->at(i).p1).getY();
				Z1 = 4*sonPointPart->at(triSonPart0->at(i).p2).getZ() - 4*sonPointPart->at(triSonPart0->at(i).p1).getZ();

				X2 = sonPointPart->at(triSonPart0->at(i).p3).getX() - sonPointPart->at(triSonPart0->at(i).p1).getX();
				Y2 = sonPointPart->at(triSonPart0->at(i).p3).getY() - sonPointPart->at(triSonPart0->at(i).p1).getY();
				Z2 = 4*sonPointPart->at(triSonPart0->at(i).p3).getZ() - 4*sonPointPart->at(triSonPart0->at(i).p1).getZ();
				ifleftorright = jugleLeftOrRight(sonPointPart->at(lineSonPart0->at(triSonPart0->at(i).Line1).LeftPoint) , sonPointPart->at(lineSonPart0->at(triSonPart0->at(i).Line1).RightPoint) , sonPointPart->at(triSonPart0->at(i).ExistLine(lineSonPart0 , (triSonPart0->at(i).Line1))));

				if(ifleftorright == 1)
				{
					a = Y2 * Z1 - Z2 * Y1;
					b = Z2 * X1 - Z1 * X2;
					c = X2 * Y1 - X1 * Y2;
				}
				else
				{
					a = Y1 * Z2 - Z1 * Y2;
					b = Z1 * X2 - Z2 * X1;
					c = X1 * Y2 - X2 * Y1;
				}

				triSonPart0->at(i).normal.setX(a);
				triSonPart0->at(i).normal.setY(b);
				triSonPart0->at(i).normal.setZ(c);
				triSonPartTemp0->at(i).normal.setX(a);
				triSonPartTemp0->at(i).normal.setY(b);
				triSonPartTemp0->at(i).normal.setZ(c);
			}
			break;
		}
	case 1:
		{
			for(unsigned int j = 0 ; j < triSonPart1->size() ; j++)
			{
				X1 = sonPointPart->at(triSonPart1->at(j).p2).getX() - sonPointPart->at(triSonPart1->at(j).p1).getX();
				Y1 = sonPointPart->at(triSonPart1->at(j).p2).getY() - sonPointPart->at(triSonPart1->at(j).p1).getY();
				Z1 = 4*sonPointPart->at(triSonPart1->at(j).p2).getZ() - 4*sonPointPart->at(triSonPart1->at(j).p1).getZ();

				X2 = sonPointPart->at(triSonPart1->at(j).p3).getX() - sonPointPart->at(triSonPart1->at(j).p1).getX();
				Y2 = sonPointPart->at(triSonPart1->at(j).p3).getY() - sonPointPart->at(triSonPart1->at(j).p1).getY();
				Z2 = 4*sonPointPart->at(triSonPart1->at(j).p3).getZ() - 4*sonPointPart->at(triSonPart1->at(j).p1).getZ();
				ifleftorright = jugleLeftOrRight(sonPointPart->at(lineSonPart1->at(triSonPart1->at(j).Line1).LeftPoint) , sonPointPart->at(lineSonPart1->at(triSonPart1->at(j).Line1).RightPoint) , sonPointPart->at(triSonPart1->at(j).ExistLine(lineSonPart1 , (triSonPart1->at(j).Line1))));

				if(ifleftorright == 1)
				{
					a = Y2 * Z1 - Z2 * Y1;
					b = Z2 * X1 - Z1 * X2;
					c = X2 * Y1 - X1 * Y2;
				}
				else
				{
					a = Y1 * Z2 - Z1 * Y2;
					b = Z1 * X2 - Z2 * X1;
					c = X1 * Y2 - X2 * Y1;
				}

				triSonPart1->at(j).normal.setX(a);
				triSonPart1->at(j).normal.setY(b);
				triSonPart1->at(j).normal.setZ(c);
				triSonPartTemp1->at(j).normal.setX(a);
				triSonPartTemp1->at(j).normal.setY(b);
				triSonPartTemp1->at(j).normal.setZ(c);
			}
			break;
		}
	case 2:
		{
			for(unsigned int k = 0 ; k < triSonPart2->size() ; k++)
			{
				X1 = sonPointPart->at(triSonPart2->at(k).p2).getX() - sonPointPart->at(triSonPart2->at(k).p1).getX();
				Y1 = sonPointPart->at(triSonPart2->at(k).p2).getY() - sonPointPart->at(triSonPart2->at(k).p1).getY();
				Z1 = 4*sonPointPart->at(triSonPart2->at(k).p2).getZ() - 4*sonPointPart->at(triSonPart2->at(k).p1).getZ();

				X2 = sonPointPart->at(triSonPart2->at(k).p3).getX() - sonPointPart->at(triSonPart2->at(k).p1).getX();
				Y2 = sonPointPart->at(triSonPart2->at(k).p3).getY() - sonPointPart->at(triSonPart2->at(k).p1).getY();
				Z2 = 4*sonPointPart->at(triSonPart2->at(k).p3).getZ() - 4*sonPointPart->at(triSonPart2->at(k).p1).getZ();
				ifleftorright = jugleLeftOrRight(sonPointPart->at(lineSonPart2->at(triSonPart2->at(k).Line1).LeftPoint) , sonPointPart->at(lineSonPart2->at(triSonPart2->at(k).Line1).RightPoint) , sonPointPart->at(triSonPart2->at(k).ExistLine(lineSonPart2 , (triSonPart2->at(k).Line1))));

				if(ifleftorright == 1)
				{
					a = Y2 * Z1 - Z2 * Y1;
					b = Z2 * X1 - Z1 * X2;
					c = X2 * Y1 - X1 * Y2;
				}
				else
				{
					a = Y1 * Z2 - Z1 * Y2;
					b = Z1 * X2 - Z2 * X1;
					c = X1 * Y2 - X2 * Y1;
				}

				triSonPart2->at(k).normal.setX(a);
				triSonPart2->at(k).normal.setY(b);
				triSonPart2->at(k).normal.setZ(c);
				triSonPartTemp2->at(k).normal.setX(a);
				triSonPartTemp2->at(k).normal.setY(b);
				triSonPartTemp2->at(k).normal.setZ(c);
			}
			break;
		}
	case 3:
		{
			for(unsigned int l = 0 ; l < triSonPart3->size() ; l++)
			{
				X1 = sonPointPart->at(triSonPart3->at(l).p2).getX() - sonPointPart->at(triSonPart3->at(l).p1).getX();
				Y1 = sonPointPart->at(triSonPart3->at(l).p2).getY() - sonPointPart->at(triSonPart3->at(l).p1).getY();
				Z1 = 4*sonPointPart->at(triSonPart3->at(l).p2).getZ() - 4*sonPointPart->at(triSonPart3->at(l).p1).getZ();

				X2 = sonPointPart->at(triSonPart3->at(l).p3).getX() - sonPointPart->at(triSonPart3->at(l).p1).getX();
				Y2 = sonPointPart->at(triSonPart3->at(l).p3).getY() - sonPointPart->at(triSonPart3->at(l).p1).getY();
				Z2 = 4*sonPointPart->at(triSonPart3->at(l).p3).getZ() - 4*sonPointPart->at(triSonPart3->at(l).p1).getZ();
				ifleftorright = jugleLeftOrRight(sonPointPart->at(lineSonPart3->at(triSonPart3->at(l).Line1).LeftPoint) , sonPointPart->at(lineSonPart3->at(triSonPart3->at(l).Line1).RightPoint) , sonPointPart->at(triSonPart3->at(l).ExistLine(lineSonPart3 , (triSonPart3->at(l).Line1))));

				if(ifleftorright == 1)
				{
					a = Y2 * Z1 - Z2 * Y1;
					b = Z2 * X1 - Z1 * X2;
					c = X2 * Y1 - X1 * Y2;
				}
				else
				{
					a = Y1 * Z2 - Z1 * Y2;
					b = Z1 * X2 - Z2 * X1;
					c = X1 * Y2 - X2 * Y1;
				}

				triSonPart3->at(l).normal.setX(a);
				triSonPart3->at(l).normal.setY(b);
				triSonPart3->at(l).normal.setZ(c);
				triSonPartTemp3->at(l).normal.setX(a);
				triSonPartTemp3->at(l).normal.setY(b);
				triSonPartTemp3->at(l).normal.setZ(c);
			}
			break;
		}
	default:
		cout<<"在进行三角形面片法向量计算时出错，错误子块为"<<index<<endl;
		break;
	}

}

//利用向量积可以求出和平面垂直的向量
//设三点坐标为A(x1,y1,z1),B(x2,y2,z2),C(x3,y3,z3)
//向量AB=(x2-x1,y2-y1,z2-z1),AC=(x3-x1,y3-y1,z3-z1)
//AB、AC所在平面的法向量即AB×AC=(a,b,c),其中：
//a=(y2-y1)(z3-z1)-(z2-z1)(y3-y1)
//b=(z2-z1)(x3-x1)-(z3-z1)(x2-x1)
//c=(x2-x1)(y3-y1)-(x3-x1)(y2-y1)
void getNormal(VECTORMYPOINT* rPoint)//法向量
{
	//VECTORMYNORMAL* normalList = new VECTORMYNORMAL;
	//MyNormal normal;
	double a = 0;
	double b = 0;
	double c = 0;
	double X1 = 0;
	double Y1 = 0;
	double Z1 = 0;
	double X2 = 0;
	double Y2 = 0;
	double Z2 = 0;
	unsigned int TriFileSize = TriFile->size();
	unsigned i;
	int ifleftorright;
	for(i = 0 ; i < TriFileSize ; i++)
	{
		/*X1 = rPoint->at(lineFile->at(TriFile->at(i).Line1).RightPoint).getX() - rPoint->at(lineFile->at((TriFile->at(i).Line1)).LeftPoint).getX();
		Y1 = rPoint->at(lineFile->at(TriFile->at(i).Line1).RightPoint).getY() - rPoint->at(lineFile->at((TriFile->at(i).Line1)).LeftPoint).getY();
		Z1 = rPoint->at(lineFile->at(TriFile->at(i).Line1).RightPoint).getZ() - rPoint->at(lineFile->at((TriFile->at(i).Line1)).LeftPoint).getZ();

		X2 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getX() - rPoint->at(lineFile->at(TriFile->at(i).Line1).LeftPoint).getX();
		Y2 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getY() - rPoint->at(lineFile->at(TriFile->at(i).Line1).LeftPoint).getY();
		Z2 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getZ() - rPoint->at(lineFile->at(TriFile->at(i).Line1).LeftPoint).getZ();*/

		X1 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getX() - rPoint->at(lineFile->at((TriFile->at(i).Line1)).LeftPoint).getX();
		Y1 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getY() - rPoint->at(lineFile->at((TriFile->at(i).Line1)).LeftPoint).getY();
		Z1 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getZ() - rPoint->at(lineFile->at((TriFile->at(i).Line1)).LeftPoint).getZ();

		X2 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getX() - rPoint->at(lineFile->at(TriFile->at(i).Line1).RightPoint).getX();
		Y2 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getY() - rPoint->at(lineFile->at(TriFile->at(i).Line1).RightPoint).getY();
		Z2 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getZ() - rPoint->at(lineFile->at(TriFile->at(i).Line1).RightPoint).getZ();
		ifleftorright = jugleLeftOrRight(rPoint->at(lineFile->at(TriFile->at(i).Line1).LeftPoint) , rPoint->at(lineFile->at(TriFile->at(i).Line1).RightPoint) , rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))));

		/*X1 = rPoint->at(TriFile->at(i).p1).getX() - rPoint->at(TriFile->at(i).p2).getX();
		Y1 = rPoint->at(TriFile->at(i).p1).getY() - rPoint->at(TriFile->at(i).p2).getY();
		Z1 = rPoint->at(TriFile->at(i).p1).getZ() - rPoint->at(TriFile->at(i).p2).getZ();

		X2 = rPoint->at(TriFile->at(i).p1).getX() - rPoint->at(TriFile->at(i).p3).getX();
		Y2 = rPoint->at(TriFile->at(i).p1).getY() - rPoint->at(TriFile->at(i).p3).getY();
		Z2 = rPoint->at(TriFile->at(i).p1).getZ() - rPoint->at(TriFile->at(i).p3).getZ();
		if(rPoint->at(TriFile->at(i).p2).getX() <= rPoint->at(TriFile->at(i).p3).getX())
		{
			ifleftorright = jugleLeftOrRight(rPoint->at(TriFile->at(i).p2) , rPoint->at(TriFile->at(i).p3) , rPoint->at(TriFile->at(i).p1));
		}
		else
		{
			ifleftorright = jugleLeftOrRight(rPoint->at(TriFile->at(i).p3) , rPoint->at(TriFile->at(i).p2) , rPoint->at(TriFile->at(i).p1));
		}*/

		if(ifleftorright == -1)
		{
			a = Y1 * Z2 - Z1 * Y2;
			b = Z1 * X2 - Z2 * X1;
			c = X1 * Y2 - X2 * Y1;
		}
		else if(ifleftorright == 1)
		{
			a = Y2 * Z1 - Z2 * Y1;
			b = Z2 * X1 - Z1 * X2;
			c = X2 * Y1 - X1 * Y2;
		}
		else if(ifleftorright == 0)
		{
			cout<<"error 点在直线上"<<endl;
			a = Y1 * Z2 - Z1 * Y2;
			b = Z1 * X2 - Z2 * X1;
			c = X1 * Y2 - X2 * Y1;
		}
		else
		{
			cout<<"error"<<getstring(ifleftorright)<<endl;
		}


		/*normal.setX(a);
		normal.setY(b);
		normal.setZ(c);
		normalList->push_back(normal);*/
		TriFile->at(i).normal.setX(a);
		TriFile->at(i).normal.setY(b);
		TriFile->at(i).normal.setZ(c);
		TriFile1->at(i).normal.setX(a);
		TriFile1->at(i).normal.setY(b);
		TriFile1->at(i).normal.setZ(c);
	}
	//return normalList;
}

MyTri getTriNormal(MyTri tri , VECTORMYPOINT* rPoint)
{
	double a = 0;
	double b = 0;
	double c = 0;
	double X1 = 0;
	double Y1 = 0;
	double Z1 = 0;
	double X2 = 0;
	double Y2 = 0;
	double Z2 = 0;
	/*unsigned int TriFileSize = TriFile->size();
	unsigned i;*/
	int ifleftorright;

	/*X1 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getX() - rPoint->at(lineFile->at((TriFile->at(i).Line1)).LeftPoint).getX();
	Y1 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getY() - rPoint->at(lineFile->at((TriFile->at(i).Line1)).LeftPoint).getY();
	Z1 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getZ() - rPoint->at(lineFile->at((TriFile->at(i).Line1)).LeftPoint).getZ();

	X2 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getX() - rPoint->at(lineFile->at(TriFile->at(i).Line1).RightPoint).getX();
	Y2 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getY() - rPoint->at(lineFile->at(TriFile->at(i).Line1).RightPoint).getY();
	Z2 = rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))).getZ() - rPoint->at(lineFile->at(TriFile->at(i).Line1).RightPoint).getZ();
*/
	X1 = rPoint->at(tri.p1).getX() - rPoint->at(tri.p2).getX();
	Y1 = rPoint->at(tri.p1).getY() - rPoint->at(tri.p2).getY();
	Z1 = rPoint->at(tri.p1).getZ() - rPoint->at(tri.p2).getZ();

	X2 = rPoint->at(tri.p1).getX() - rPoint->at(tri.p3).getX();
	Y2 = rPoint->at(tri.p1).getY() - rPoint->at(tri.p3).getY();
	Z2 = rPoint->at(tri.p1).getZ() - rPoint->at(tri.p3).getZ();
	/*ifleftorright = jugleLeftOrRight(rPoint->at(lineFile->at(TriFile->at(i).Line1).LeftPoint) , rPoint->at(lineFile->at(TriFile->at(i).Line1).RightPoint) , rPoint->at(TriFile->at(i).ExistLine(lineFile , (TriFile->at(i).Line1))));
	*/
	ifleftorright = jugleLeftOrRight(rPoint->at(tri.p2) , rPoint->at(tri.p3) , rPoint->at(tri.p1));
	if(ifleftorright == -1)
	{
		a = Y1 * Z2 - Z1 * Y2;
		b = Z1 * X2 - Z2 * X1;
		c = X1 * Y2 - X2 * Y1;
	}
	else if(ifleftorright == 1)
	{
		a = Y2 * Z1 - Z2 * Y1;
		b = Z2 * X1 - Z1 * X2;
		c = X2 * Y1 - X1 * Y2;
	}
	else if(ifleftorright == 0)
	{
		cout<<"error 点在直线上"<<endl;
		cout<<getstring(tri.index)<<endl;
		a = Y1 * Z2 - Z1 * Y2;
		b = Z1 * X2 - Z2 * X1;
		c = X1 * Y2 - X2 * Y1;
	}
	else
	{
		cout<<"error"<<getstring(ifleftorright)<<endl;
	}

	/*TriFile->at(i).normal.setX(a);
	TriFile->at(i).normal.setY(b);
	TriFile->at(i).normal.setZ(c);*/
	tri.normal.setX(a);
	tri.normal.setY(b);
	tri.normal.setZ(c);
	return tri;
}

/******************************
获取对应子块内目标点相邻的所有三角形
*********************************/
VECTORMYTRI* getSonPartSamePointTri(VECTORMYTRI* triSonPart , int p)
{
	unsigned int triSonPartSize = triSonPart->size();
	VECTORMYTRI* sameTri = new VECTORMYTRI;
	bool ifHaveSameTri = false;
	for(unsigned int i = 0 ; i < triSonPartSize ; i++)
	{
		if(i == 5136)
		{
			int n = 0;
		}
		ifHaveSameTri = false;
		
		if(p == triSonPart->at(i).p1||p == triSonPart->at(i).p2||p == triSonPart->at(i).p3)
		{
			if(triSonPart->at(i).noActive)
			{
				continue;
			}
			else
			{
				for(unsigned int j = 0 ; j < sameTri->size() ; j++)
				{
					
					if(triSonPart->at(i).p1 == sameTri->at(j).p1||triSonPart->at(i).p2 == sameTri->at(j).p1||triSonPart->at(i).p3 == sameTri->at(j).p1)
					{
						if(triSonPart->at(i).p1 == sameTri->at(j).p2||triSonPart->at(i).p2 == sameTri->at(j).p2||triSonPart->at(i).p3 == sameTri->at(j).p2)
						{
							if(triSonPart->at(i).p1 == sameTri->at(j).p3||triSonPart->at(i).p2 == sameTri->at(j).p3||triSonPart->at(i).p3 == sameTri->at(j).p3)
							{
								ifHaveSameTri = true;
								break;
							}
						}
					}
					
				}
				if(!ifHaveSameTri)
				{
					sameTri->push_back(triSonPart->at(i));
				}
			}
		}
		
	}
	
	return sameTri;
}

VECTORMYTRI* getSamePointTri(int p)//获得点相邻接的所有三角形
{
	unsigned int TirFilesize = TriFile1->size();
	//VECTORINT* TriP = new VECTORINT;
	VECTORMYTRI* sameTri = new VECTORMYTRI;
	bool ifHaveSameTri = false;
	for(unsigned int i = 0 ; i < TirFilesize ; i++)
	{
		ifHaveSameTri = false;
		/*VECTORINT* TriP = new VECTORINT;
		TriP = TriFile->at(i).GetPointIndex(lineFile);
		if(p == TriP->at(0)||p == TriP->at(1)||p == TriP->at(2))
		{
			sameTri->push_back(TriFile->at(i));
		}
		else
		{
			continue;
		}*/
		if(p == TriFile1->at(i).p1||p == TriFile1->at(i).p2||p == TriFile1->at(i).p3)
		{
			if(TriFile1->at(i).noActive)
			{
				continue;
			}
			else
			{
				for(unsigned int j = 0 ; j < sameTri->size() ; j++)
				{
					
					if(TriFile1->at(i).p1 == sameTri->at(j).p1||TriFile1->at(i).p2 == sameTri->at(j).p1||TriFile1->at(i).p3 == sameTri->at(j).p1)
					{
						if(TriFile1->at(i).p1 == sameTri->at(j).p2||TriFile1->at(i).p2 == sameTri->at(j).p2||TriFile1->at(i).p3 == sameTri->at(j).p2)
						{
							if(TriFile1->at(i).p1 == sameTri->at(j).p3||TriFile1->at(i).p2 == sameTri->at(j).p3||TriFile1->at(i).p3 == sameTri->at(j).p3)
							{
								ifHaveSameTri = true;
								break;
							}
						}
					}
					
				}
				if(!ifHaveSameTri)
				{
					sameTri->push_back(TriFile1->at(i));
				}
			}
		}
		else
		{
			continue;
		}
		//delete TriP;
		
	}
	
	return sameTri;
}

void getPointNormal(VECTORMYPOINT* rPoint)//获得每个点的法向量
{
	unsigned int rPointsize = rPoint->size();
	double nX;
	double nY;
	double nZ;
	SamePointTri* samePointTri;
	for(unsigned int i = 0 ; i < rPointsize ; i++)
	{
		nX = 0;
		nY = 0;
		nZ = 0;
		VECTORMYTRI* sameTri;
		samePointTri = new SamePointTri;
		sameTri = getSamePointTri(i);
		samePointTri->clear();
		samePointTri->setPointIndex(i);
		samePointTri->copy(sameTri);
		samePointT->push_back(*samePointTri);

		unsigned int j;
		unsigned int sameTrisize = sameTri->size();
		for(j = 0 ; j < sameTrisize ; j++)
		{
			nX += sameTri->at(j).normal.getX();
			nY += sameTri->at(j).normal.getY();
			nZ += sameTri->at(j).normal.getZ();
		}
		rPoint->at(i).normal.X = nX/sameTrisize;
		rPoint->at(i).normal.Y = nY/sameTrisize;
		rPoint->at(i).normal.Z = nZ/sameTrisize;
		delete sameTri;
	}
}

VECTORDOUBLE* getPlaneData(MyTri tri , VECTORMYPOINT* pointPart)//平面表达式系数ABCD
{
	VECTORDOUBLE* abcd = new VECTORDOUBLE;
	double A , B , C , D , a , b , c , d , s , x1 , y1 , z1 , x2 , y2 , z2 , x3 , y3 , z3;
	x1 = pointPart->at(tri.p1).getX();
	y1 = pointPart->at(tri.p1).getY();
	z1 = pointPart->at(tri.p1).getZ();
	x2 = pointPart->at(tri.p2).getX();
	y2 = pointPart->at(tri.p2).getY();
	z2 = pointPart->at(tri.p2).getZ();
	x3 = pointPart->at(tri.p3).getX();
	y3 = pointPart->at(tri.p3).getY();
	z3 = pointPart->at(tri.p3).getZ();
	/*A = (y2 - y1)*(z3 - z1) - (y3 - y1)*(z2 - z1);
	B = (z2 - z1)*(x3 - x1) - (z3 - z1)*(x2 - x1);
	C = (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1);
	D = -A*x1 - B*y1 - C*z1;*/
	A = y1*(z2 - z3) + y2*(z3 - z1) + y3*(z1 - z2);
	B = z1*(x2 - x3) + z2*(x3 - x1) + z3*(x1 - x2);
	C = x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2);
	D = -x1*(y2*z3 - y3*z2) - x2*(y3*z1 - y1*z3) - x3*(y1*z2 - y2*z1);
	
	s = sqrt(A*A + B*B + C*C);
	a = A/s;
	b = B/s;
	c = C/s;
	d = D/s;
	abcd->push_back(a);
	abcd->push_back(b);
	abcd->push_back(c);
	abcd->push_back(d);
	return abcd;

	
}

void setQEM(VECTORMYPOINT* rPoint , int p)//p点二次误差
{
	rPoint->at(p).qemclear();
	//VECTORMYTRI* sameTri = new VECTORMYTRI;
	//sameTri = getSamePointTri(p);
	unsigned int sameTrisize = samePointT->at(p).getsameTriSize();
	VECTORDOUBLE* abcd;
	double a , b , c , d;
	for(unsigned int i = 0 ; i < sameTrisize ; i++)
	{
		//abcd->clear();
		abcd = getPlaneData(samePointT->at(p).getTri(i) , rPoint);
		a = abcd->at(0);
		b = abcd->at(1);
		c = abcd->at(2);
		d = abcd->at(3);
		rPoint->at(p).qem1[0][0] += a*a;
		rPoint->at(p).qem1[0][1] += a*b;
		rPoint->at(p).qem1[0][2] += a*c;
		rPoint->at(p).qem1[0][3] += a*d;
		rPoint->at(p).qem1[1][0] += a*b;
		rPoint->at(p).qem1[1][1] += b*b;
		rPoint->at(p).qem1[1][2] += b*c;
		rPoint->at(p).qem1[1][3] += b*d;
		rPoint->at(p).qem1[2][0] += a*c;
		rPoint->at(p).qem1[2][1] += b*c;
		rPoint->at(p).qem1[2][2] += c*c;
		rPoint->at(p).qem1[2][3] += c*d;
		rPoint->at(p).qem1[3][0] += a*d;
		rPoint->at(p).qem1[3][1] += b*d;
		rPoint->at(p).qem1[3][2] += c*d;
		rPoint->at(p).qem1[3][3] += d*d;
		delete abcd;
	}
	//delete sameTri;
	
	//return sameTri;


	
}

MyPoint setSonPartPtQem(MyPoint p , VECTORMYPOINT* pointPart , VECTORMYTRI* triSonPart)
{
	/*if(p.index == 110)
	{
		int n = 0;
	}*/
	p.qemclear();
	VECTORMYTRI* sameTri = new VECTORMYTRI;
	sameTri = getSonPartSamePointTri(triSonPart , p.index);
	VECTORDOUBLE* abcd = new VECTORDOUBLE;
	double a , b , c , d;
	for(unsigned int i = 0 ; i < sameTri->size() ; i++)
	{
		abcd = getPlaneData(sameTri->at(i) , pointPart);
		a = abcd->at(0);
		b = abcd->at(1);
		c = abcd->at(2);
		d = abcd->at(3);
		p.qem1[0][0] += a*a;
		p.qem1[0][1] += a*b;
		p.qem1[0][2] += a*c;
		p.qem1[0][3] += a*d;
		p.qem1[1][0] += a*b;
		p.qem1[1][1] += b*b;
		p.qem1[1][2] += b*c;
		p.qem1[1][3] += b*d;
		p.qem1[2][0] += a*c;
		p.qem1[2][1] += b*c;
		p.qem1[2][2] += c*c;
		p.qem1[2][3] += c*d;
		p.qem1[3][0] += a*d;
		p.qem1[3][1] += b*d;
		p.qem1[3][2] += c*d;
		p.qem1[3][3] += d*d;
	}
	delete sameTri;
	return p;

}

MyPoint setPtQEM(MyPoint p , VECTORMYPOINT* rPoint)//二次误差和矩阵
{
	p.qemclear();
	VECTORMYTRI* sameTri = new VECTORMYTRI;
	sameTri = getSamePointTri(p.index);
	unsigned int sameTrisize = sameTri->size();
	VECTORDOUBLE* abcd = new VECTORDOUBLE;
	double a , b , c , d;
	for(unsigned int i = 0 ; i < sameTrisize ; i++)
	{
		//abcd->clear();
		abcd = getPlaneData(sameTri->at(i) , rPoint);
		a = abcd->at(0);
		b = abcd->at(1);
		c = abcd->at(2);
		d = abcd->at(3);
		p.qem1[0][0] += a*a;
		p.qem1[0][1] += a*b;
		p.qem1[0][2] += a*c;
		p.qem1[0][3] += a*d;
		p.qem1[1][0] += a*b;
		p.qem1[1][1] += b*b;
		p.qem1[1][2] += b*c;
		p.qem1[1][3] += b*d;
		p.qem1[2][0] += a*c;
		p.qem1[2][1] += b*c;
		p.qem1[2][2] += c*c;
		p.qem1[2][3] += c*d;
		p.qem1[3][0] += a*d;
		p.qem1[3][1] += b*d;
		p.qem1[3][2] += c*d;
		p.qem1[3][3] += d*d;

	}
	return p;
}

VECTORINT* getPointLine(MyTri tri , int p)//p点对应三角形的两条边
{
	VECTORINT* sameline = new VECTORINT;
	if(lineFile->at(tri.Line1).LeftPoint == p||lineFile->at(tri.Line1).RightPoint == p)
	{
		sameline->push_back(tri.Line1);
	}
	if(lineFile->at(tri.Line2).LeftPoint == p||lineFile->at(tri.Line2).RightPoint == p)
	{
		sameline->push_back(tri.Line2);
	}
	if(lineFile->at(tri.Line3).LeftPoint == p||lineFile->at(tri.Line3).RightPoint == p)
	{
		sameline->push_back(tri.Line3);
	}
	/*else
	{
		cout<<"出错"<<endl;
	}*/
	return sameline;
}

VECTORINT* getSamePointLine(VECTORMYPOINT* rPoint , int p)//p点所有相邻三角形的邻接边
{
	VECTORMYTRI* samePTri = new VECTORMYTRI;
	VECTORINT* samePLine = new VECTORINT;
	for(int i = 0 ; i < samePointT->at(p).getsameTriSize() ; i++)
	{
		samePTri->push_back(samePointT->at(p).getTri(i));
	}
	for(unsigned int j = 0 ; j < samePTri->size() ; j++)
	{
		VECTORINT* sameline;
		sameline = getPointLine(samePTri->at(j) , p);
		samePLine->insert(samePLine->end() , sameline->begin() , sameline->end());
		delete sameline;
	}
	delete samePTri;
	return samePLine;
}

void *sideFolding(void *threadid)
{
	struct unionIndex* tid = (struct unionIndex*) threadid;
	int partIndex = tid->partIndex;
	switch(partIndex)
	{
	case 0:
		{
			setSonQemData(0);
			VECTORMYPOINT* pointPartQem;
			pointPartQem = qemSort(pointPart0);
			unsigned int j = 0;
			clock_t start , finish;
			start = clock();
			for(int i = 0 ; i < 4000 ; i++)
			{
				
				double sumV = 0;
				for(unsigned int f = 0 ; f < pointPartQem->size() ; f++)
				{
					sumV += pointPartQem->at(f).minqem1;
				}
				quadraticError0->push_back(sumV / pointPartQem->size());
				changeDataSonPartSave(pointPart0 , pointPartQem , 0);
				for(j ; j < changeSonPartPLV0->size() ; j++)
				{
					bool ifhave = false;
					bool ifhave1 = false;
					for(unsigned int k = 0 ; k < pointPartQem->size() ; k++)
					{
						if(pointPartQem->at(k).index == changeSonPartPLV0->at(j).index)
						{
							pointPartQem->erase(pointPartQem->begin() + k);
							ifhave = true;
							break;
						}
					}
					if(!ifhave)
					{
						cout<<"在子块"<<partIndex<<"没有进行pointPartQem对应点数据的删除"<<endl;
					}
					if(!changeSonPartPLV0->at(j).ifdelete)
					{
						for(unsigned int m = 0 ; m < pointPartQem->size() ; m++)
						{
							if(changeSonPartPLV0->at(j).minqem1 < pointPartQem->at(m).minqem1)
							{
								ifhave1 = true;
								pointPartQem->insert(pointPartQem->begin() + m , changeSonPartPLV0->at(j));
								break;
							}
						}
						if(!ifhave1)
						{
							pointPartQem->push_back(changeSonPartPLV0->at(j));
						}
					}
				}
				
			}
			finish = clock();
			cout<<"三角形子块"<<partIndex<<"边折叠花费的时间为："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;
			/*****************
			存储边折叠信息
			*****************/
			/*dataIndexSonPartChange(0);
			newSonPartData(0);*/
			saveSonPartError(0);
			saveSonPartChangeData(0);
			break;
		}

		case 1:
		{
			setSonQemData(1);
			VECTORMYPOINT* pointPartQem;
			pointPartQem = qemSort(pointPart1);
			unsigned int j = 0;
			clock_t start , finish;
			start = clock();
			for(int i = 0 ; i < 4000 ; i++)
			{
				/*if(i == 2472)
				{
					int n = 0;
				}*/
				double sumV = 0;
				for(unsigned int f = 0 ; f < pointPartQem->size() ; f++)
				{
					sumV += pointPartQem->at(f).minqem1;
				}
				quadraticError1->push_back(sumV / pointPartQem->size());
				changeDataSonPartSave(pointPart1 , pointPartQem , 1);
				for(j ; j < changeSonPartPLV1->size() ; j++)
				{
					bool ifhave = false;
					bool ifhave1 = false;
					for(unsigned int k = 0 ; k < pointPartQem->size() ; k++)
					{
						if(pointPartQem->at(k).index == changeSonPartPLV1->at(j).index)
						{
							pointPartQem->erase(pointPartQem->begin() + k);
							ifhave = true;
							break;
						}
					}
					if(!ifhave)
					{
						cout<<"在子块"<<partIndex<<"没有进行pointPartQem对应点数据的删除"<<endl;
					}
					if(!changeSonPartPLV1->at(j).ifdelete)
					{
						for(unsigned int m = 0 ; m < pointPartQem->size() ; m++)
						{
							if(changeSonPartPLV1->at(j).minqem1 < pointPartQem->at(m).minqem1)
							{
								ifhave1 = true;
								pointPartQem->insert(pointPartQem->begin() + m , changeSonPartPLV1->at(j));
								break;
							}
						}
						if(!ifhave1)
						{
							pointPartQem->push_back(changeSonPartPLV1->at(j));
						}
					}
				}
				
			}
			finish = clock();
			cout<<"三角形子块"<<partIndex<<"边折叠花费的时间为："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;

			/*dataIndexSonPartChange(1);
			newSonPartData(1);*/
			saveSonPartError(1);
			saveSonPartChangeData(1);
			break;
		}

		case 2:
		{
			setSonQemData(2);
			VECTORMYPOINT* pointPartQem;
			pointPartQem = qemSort(pointPart2);
			unsigned int j = 0;
			clock_t start , finish;
			start = clock();
			for(int i = 0 ; i < 4000 ; i++)
			{
				double sumV = 0;
				for(unsigned int f = 0 ; f < pointPartQem->size() ; f++)
				{
					sumV += pointPartQem->at(f).minqem1;
				}
				quadraticError2->push_back(sumV / pointPartQem->size());
				changeDataSonPartSave(pointPart2 , pointPartQem , 2);
				for(j ; j < changeSonPartPLV2->size() ; j++)
				{
					bool ifhave = false;
					bool ifhave1 = false;
					for(unsigned int k = 0 ; k < pointPartQem->size() ; k++)
					{
						if(pointPartQem->at(k).index == changeSonPartPLV2->at(j).index)
						{
							pointPartQem->erase(pointPartQem->begin() + k);
							ifhave = true;
							break;
						}
					}
					if(!ifhave)
					{
						cout<<"在子块"<<partIndex<<"没有进行pointPartQem对应点数据的删除"<<endl;
					}
					if(!changeSonPartPLV2->at(j).ifdelete)
					{
						for(unsigned int m = 0 ; m < pointPartQem->size() ; m++)
						{
							if(changeSonPartPLV2->at(j).minqem1 < pointPartQem->at(m).minqem1)
							{
								ifhave1 = true;
								pointPartQem->insert(pointPartQem->begin() + m , changeSonPartPLV2->at(j));
								break;
							}
						}
						if(!ifhave1)
						{
							pointPartQem->push_back(changeSonPartPLV2->at(j));
						}
					}
				}
				
			}
			finish = clock();
			cout<<"三角形子块"<<partIndex<<"边折叠花费的时间为："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;

			/*dataIndexSonPartChange(2);
			newSonPartData(2);*/
			saveSonPartError(2);
			saveSonPartChangeData(2);
			break;
		}

		case 3:
		{
			setSonQemData(3);
			VECTORMYPOINT* pointPartQem;
			pointPartQem = qemSort(pointPart3);
			unsigned int j = 0;
			clock_t start , finish;
			start = clock();
			for(int i = 0 ; i < 4000 ; i++)
			{
				double sumV = 0;
				for(unsigned int f = 0 ; f < pointPartQem->size() ; f++)
				{
					sumV += pointPartQem->at(f).minqem1;
				}
				quadraticError3->push_back(sumV / pointPartQem->size());
				changeDataSonPartSave(pointPart3 , pointPartQem , 3);
				for(j ; j < changeSonPartPLV3->size() ; j++)
				{
					bool ifhave = false;
					bool ifhave1 = false;
					for(unsigned int k = 0 ; k < pointPartQem->size() ; k++)
					{
						if(pointPartQem->at(k).index == changeSonPartPLV3->at(j).index)
						{
							pointPartQem->erase(pointPartQem->begin() + k);
							ifhave = true;
							break;
						}
					}
					if(!ifhave)
					{
						cout<<"在子块"<<partIndex<<"没有进行pointPartQem对应点数据的删除"<<endl;
					}
					if(!changeSonPartPLV3->at(j).ifdelete)
					{
						for(unsigned int m = 0 ; m < pointPartQem->size() ; m++)
						{
							if(changeSonPartPLV3->at(j).minqem1 < pointPartQem->at(m).minqem1)
							{
								ifhave1 = true;
								pointPartQem->insert(pointPartQem->begin() + m , changeSonPartPLV3->at(j));
								break;
							}
						}
						if(!ifhave1)
						{
							pointPartQem->push_back(changeSonPartPLV3->at(j));
						}
					}
				}
				
			}
			finish = clock();
			cout<<"三角形子块"<<partIndex<<"边折叠花费的时间为："<<finish - start<<"/"<<CLOCKS_PER_SEC<<"(s)"<<endl;

			/*dataIndexSonPartChange(3);
			newSonPartData(3);*/
			saveSonPartError(3);
			saveSonPartChangeData(3);
			break;
		}

		default:
			cout<<"在进行边折叠处理时出错！"<<endl;
			break;
			
	}
	pthread_exit(NULL);
	return 0;

}

/******************
求解p点的二次误差
****************/
void setSonPartQEM(int p , int index)
{
	switch(index)
	{
	case 0:
	{
		pointPart0->at(p).qemclear();
		VECTORMYTRI* sameTri = new VECTORMYTRI;
		sameTri = getSonPartSamePointTri(triSonPartTemp0 , p);
		unsigned int sameTriSize = sameTri->size();
		VECTORDOUBLE* abcd;
		double a , b , c , d;
		for(unsigned int i = 0 ; i < sameTriSize ; i++)
		{
			abcd = getPlaneData(sameTri->at(i) , pointPart0);
			a = abcd->at(0);
			b = abcd->at(1);
			c = abcd->at(2);
			d = abcd->at(3);
			pointPart0->at(p).qem1[0][0] += a*a;
			pointPart0->at(p).qem1[0][1] += a*b;
			pointPart0->at(p).qem1[0][2] += a*c;
			pointPart0->at(p).qem1[0][3] += a*d;
			pointPart0->at(p).qem1[1][0] += a*b;
			pointPart0->at(p).qem1[1][1] += b*b;
			pointPart0->at(p).qem1[1][2] += b*c;
			pointPart0->at(p).qem1[1][3] += b*d;
			pointPart0->at(p).qem1[2][0] += a*c;
			pointPart0->at(p).qem1[2][1] += b*c;
			pointPart0->at(p).qem1[2][2] += c*c;
			pointPart0->at(p).qem1[2][3] += c*d;
			pointPart0->at(p).qem1[3][0] += a*d;
			pointPart0->at(p).qem1[3][1] += b*d;
			pointPart0->at(p).qem1[3][2] += c*d;
			pointPart0->at(p).qem1[3][3] += d*d;
			delete abcd;
		}
		break;
	}

	case 1:
	{
		pointPart1->at(p).qemclear();
		VECTORMYTRI* sameTri = new VECTORMYTRI;
		sameTri = getSonPartSamePointTri(triSonPartTemp1 , p);
		unsigned int sameTriSize = sameTri->size();
		VECTORDOUBLE* abcd;
		double a , b , c , d;
		for(unsigned int i = 0 ; i < sameTriSize ; i++)
		{
			abcd = getPlaneData(sameTri->at(i) , pointPart1);
			a = abcd->at(0);
			b = abcd->at(1);
			c = abcd->at(2);
			d = abcd->at(3);
			pointPart1->at(p).qem1[0][0] += a*a;
			pointPart1->at(p).qem1[0][1] += a*b;
			pointPart1->at(p).qem1[0][2] += a*c;
			pointPart1->at(p).qem1[0][3] += a*d;
			pointPart1->at(p).qem1[1][0] += a*b;
			pointPart1->at(p).qem1[1][1] += b*b;
			pointPart1->at(p).qem1[1][2] += b*c;
			pointPart1->at(p).qem1[1][3] += b*d;
			pointPart1->at(p).qem1[2][0] += a*c;
			pointPart1->at(p).qem1[2][1] += b*c;
			pointPart1->at(p).qem1[2][2] += c*c;
			pointPart1->at(p).qem1[2][3] += c*d;
			pointPart1->at(p).qem1[3][0] += a*d;
			pointPart1->at(p).qem1[3][1] += b*d;
			pointPart1->at(p).qem1[3][2] += c*d;
			pointPart1->at(p).qem1[3][3] += d*d;
			delete abcd;
		}
		break;
	}

	case 2:
	{
		pointPart2->at(p).qemclear();
		VECTORMYTRI* sameTri = new VECTORMYTRI;
		sameTri = getSonPartSamePointTri(triSonPartTemp2 , p);
		unsigned int sameTriSize = sameTri->size();
		VECTORDOUBLE* abcd;
		double a , b , c , d;
		for(unsigned int i = 0 ; i < sameTriSize ; i++)
		{
			abcd = getPlaneData(sameTri->at(i) , pointPart2);
			a = abcd->at(0);
			b = abcd->at(1);
			c = abcd->at(2);
			d = abcd->at(3);
			pointPart2->at(p).qem1[0][0] += a*a;
			pointPart2->at(p).qem1[0][1] += a*b;
			pointPart2->at(p).qem1[0][2] += a*c;
			pointPart2->at(p).qem1[0][3] += a*d;
			pointPart2->at(p).qem1[1][0] += a*b;
			pointPart2->at(p).qem1[1][1] += b*b;
			pointPart2->at(p).qem1[1][2] += b*c;
			pointPart2->at(p).qem1[1][3] += b*d;
			pointPart2->at(p).qem1[2][0] += a*c;
			pointPart2->at(p).qem1[2][1] += b*c;
			pointPart2->at(p).qem1[2][2] += c*c;
			pointPart2->at(p).qem1[2][3] += c*d;
			pointPart2->at(p).qem1[3][0] += a*d;
			pointPart2->at(p).qem1[3][1] += b*d;
			pointPart2->at(p).qem1[3][2] += c*d;
			pointPart2->at(p).qem1[3][3] += d*d;
			delete abcd;
		}
		break;
	}

	case 3:
	{
		pointPart3->at(p).qemclear();
		VECTORMYTRI* sameTri = new VECTORMYTRI;
		sameTri = getSonPartSamePointTri(triSonPartTemp3 , p);
		unsigned int sameTriSize = sameTri->size();
		VECTORDOUBLE* abcd;
		double a , b , c , d;
		for(unsigned int i = 0 ; i < sameTriSize ; i++)
		{
			abcd = getPlaneData(sameTri->at(i) , pointPart3);
			a = abcd->at(0);
			b = abcd->at(1);
			c = abcd->at(2);
			d = abcd->at(3);
			pointPart3->at(p).qem1[0][0] += a*a;
			pointPart3->at(p).qem1[0][1] += a*b;
			pointPart3->at(p).qem1[0][2] += a*c;
			pointPart3->at(p).qem1[0][3] += a*d;
			pointPart3->at(p).qem1[1][0] += a*b;
			pointPart3->at(p).qem1[1][1] += b*b;
			pointPart3->at(p).qem1[1][2] += b*c;
			pointPart3->at(p).qem1[1][3] += b*d;
			pointPart3->at(p).qem1[2][0] += a*c;
			pointPart3->at(p).qem1[2][1] += b*c;
			pointPart3->at(p).qem1[2][2] += c*c;
			pointPart3->at(p).qem1[2][3] += c*d;
			pointPart3->at(p).qem1[3][0] += a*d;
			pointPart3->at(p).qem1[3][1] += b*d;
			pointPart3->at(p).qem1[3][2] += c*d;
			pointPart3->at(p).qem1[3][3] += d*d;
			delete abcd;
		}
		break;
	}
	}
}

/*******************
求解所有点的最小二次误差即边折叠系数
******************/
void setSonQemData(int index)
{
	/*unsigned int pointPartSize0 = pointPart0->size();
	unsigned int pointPartSize1 = pointPart1->size();
	unsigned int pointPartSize2 = pointPart2->size();
	unsigned int pointPartSize3 = pointPart3->size();*/
	switch(index)
	{
	case 0:
		{
			unsigned int pointPartSize0 = pointPart0->size();
			double qemL0[4];
			double qem0;
			for(unsigned int i = 0 ; i < pointPartSize0 ; i++)
			{
				setSonPartQEM(i , 0);
			}
			for(unsigned int j = 0 ; j < pointPartSize0 ; j++)
			{
				VECTORINT* relateP;
				relateP = getSonPartRelatePoint(j , 0);
				for(unsigned int k = 0 ; k < relateP->size() ; k++)
				{
					 if(relateP->at(k) != j)
					 {
						 qemL0[0] = (pointPart0->at(j).getX()) * (pointPart0->at(relateP->at(k)).qem1[0][0])
							 + (pointPart0->at(j).getY()) * (pointPart0->at(relateP->at(k)).qem1[1][0])
							 + (pointPart0->at(j).getZ()) * (pointPart0->at(relateP->at(k)).qem1[2][0])
							 + (pointPart0->at(relateP->at(k)).qem1[3][0]);

						 qemL0[1] = (pointPart0->at(j).getX()) * (pointPart0->at(relateP->at(k)).qem1[0][1])
							 + (pointPart0->at(j).getY()) * (pointPart0->at(relateP->at(k)).qem1[1][1])
							 + (pointPart0->at(j).getZ()) * (pointPart0->at(relateP->at(k)).qem1[2][1])
							 + (pointPart0->at(relateP->at(k)).qem1[3][1]);

						 qemL0[2] = (pointPart0->at(j).getX()) * (pointPart0->at(relateP->at(k)).qem1[0][2])
							 + (pointPart0->at(j).getY()) * (pointPart0->at(relateP->at(k)).qem1[1][2])
							 + (pointPart0->at(j).getZ()) * (pointPart0->at(relateP->at(k)).qem1[2][2])
							 + (pointPart0->at(relateP->at(k)).qem1[3][2]);

						 qemL0[3] = (pointPart0->at(j).getX()) * (pointPart0->at(relateP->at(k)).qem1[0][3])
							 + (pointPart0->at(j).getY()) * (pointPart0->at(relateP->at(k)).qem1[1][3])
							 + (pointPart0->at(j).getZ()) * (pointPart0->at(relateP->at(k)).qem1[2][3])
							 + (pointPart0->at(relateP->at(k)).qem1[3][3]);
						 qem0 = qemL0[0]*(pointPart0->at(j).getX()) + qemL0[1]*(pointPart0->at(j).getY()) + qemL0[2]*(pointPart0->at(j).getZ()) + qemL0[3];
						 if(pointPart0->at(j).minqemindex == -1)
						 {
							 pointPart0->at(j).minqem1 = qem0;
							 pointPart0->at(j).minqemindex = relateP->at(k);
						 }
						 else
						 {
							 if(pointPart0->at(j).minqem1 > qem0)
							 {
								 pointPart0->at(j).minqem1 = qem0;
								 pointPart0->at(j).minqemindex = relateP->at(k);
							 }
						 }
					 }
				}
				delete relateP;
			}
			break;

		}

		case 1:
		{
			unsigned int pointPartSize1 = pointPart1->size();
			double qemL1[4];
			double qem1;
			for(unsigned int i = 0 ; i < pointPartSize1 ; i++)
			{
				if(i == 5036)
				{
					int f = 0;
				}
				setSonPartQEM(i , 1);
			}
			for(unsigned int j = 0 ; j < pointPartSize1 ; j++)
			{
				VECTORINT* relateP;
				relateP = getSonPartRelatePoint(j , 1);
				for(unsigned int k = 0 ; k < relateP->size() ; k++)
				{
					 if(relateP->at(k) != j)
					 {
						 qemL1[0] = (pointPart1->at(j).getX()) * (pointPart1->at(relateP->at(k)).qem1[0][0])
							 + (pointPart1->at(j).getY()) * (pointPart1->at(relateP->at(k)).qem1[1][0])
							 + (pointPart1->at(j).getZ()) * (pointPart1->at(relateP->at(k)).qem1[2][0])
							 + (pointPart1->at(relateP->at(k)).qem1[3][0]);

						 qemL1[1] = (pointPart1->at(j).getX()) * (pointPart1->at(relateP->at(k)).qem1[0][1])
							 + (pointPart1->at(j).getY()) * (pointPart1->at(relateP->at(k)).qem1[1][1])
							 + (pointPart1->at(j).getZ()) * (pointPart1->at(relateP->at(k)).qem1[2][1])
							 + (pointPart1->at(relateP->at(k)).qem1[3][1]);

						 qemL1[2] = (pointPart1->at(j).getX()) * (pointPart1->at(relateP->at(k)).qem1[0][2])
							 + (pointPart1->at(j).getY()) * (pointPart1->at(relateP->at(k)).qem1[1][2])
							 + (pointPart1->at(j).getZ()) * (pointPart1->at(relateP->at(k)).qem1[2][2])
							 + (pointPart1->at(relateP->at(k)).qem1[3][2]);

						 qemL1[3] = (pointPart1->at(j).getX()) * (pointPart1->at(relateP->at(k)).qem1[0][3])
							 + (pointPart1->at(j).getY()) * (pointPart1->at(relateP->at(k)).qem1[1][3])
							 + (pointPart1->at(j).getZ()) * (pointPart1->at(relateP->at(k)).qem1[2][3])
							 + (pointPart1->at(relateP->at(k)).qem1[3][3]);
						 qem1 = qemL1[0]*(pointPart1->at(j).getX()) + qemL1[1]*(pointPart1->at(j).getY()) + qemL1[2]*(pointPart1->at(j).getZ()) + qemL1[3];
						 if(pointPart1->at(j).minqemindex == -1)
						 {
							 pointPart1->at(j).minqem1 = qem1;
							 pointPart1->at(j).minqemindex = relateP->at(k);
						 }
						 else
						 {
							 if(pointPart1->at(j).minqem1 > qem1)
							 {
								 pointPart1->at(j).minqem1 = qem1;
								 pointPart1->at(j).minqemindex = relateP->at(k);
							 }
						 }
					 }
				}
				delete relateP;
			}
			break;

		}

		case 2:
		{
			unsigned int pointPartSize2 = pointPart2->size();
			double qemL2[4];
			double qem2;
			for(unsigned int i = 0 ; i < pointPartSize2 ; i++)
			{
				setSonPartQEM(i , 2);
			}
			for(unsigned int j = 0 ; j < pointPartSize2 ; j++)
			{
				VECTORINT* relateP;
				relateP = getSonPartRelatePoint(j , 2);
				for(unsigned int k = 0 ; k < relateP->size() ; k++)
				{
					 if(relateP->at(k) != j)
					 {
						 qemL2[0] = (pointPart2->at(j).getX()) * (pointPart2->at(relateP->at(k)).qem1[0][0])
							 + (pointPart2->at(j).getY()) * (pointPart2->at(relateP->at(k)).qem1[1][0])
							 + (pointPart2->at(j).getZ()) * (pointPart2->at(relateP->at(k)).qem1[2][0])
							 + (pointPart2->at(relateP->at(k)).qem1[3][0]);

						 qemL2[1] = (pointPart2->at(j).getX()) * (pointPart2->at(relateP->at(k)).qem1[0][1])
							 + (pointPart2->at(j).getY()) * (pointPart2->at(relateP->at(k)).qem1[1][1])
							 + (pointPart2->at(j).getZ()) * (pointPart2->at(relateP->at(k)).qem1[2][1])
							 + (pointPart2->at(relateP->at(k)).qem1[3][1]);

						 qemL2[2] = (pointPart2->at(j).getX()) * (pointPart2->at(relateP->at(k)).qem1[0][2])
							 + (pointPart2->at(j).getY()) * (pointPart2->at(relateP->at(k)).qem1[1][2])
							 + (pointPart2->at(j).getZ()) * (pointPart2->at(relateP->at(k)).qem1[2][2])
							 + (pointPart2->at(relateP->at(k)).qem1[3][2]);

						 qemL2[3] = (pointPart2->at(j).getX()) * (pointPart2->at(relateP->at(k)).qem1[0][3])
							 + (pointPart2->at(j).getY()) * (pointPart2->at(relateP->at(k)).qem1[1][3])
							 + (pointPart2->at(j).getZ()) * (pointPart2->at(relateP->at(k)).qem1[2][3])
							 + (pointPart2->at(relateP->at(k)).qem1[3][3]);
						 qem2 = qemL2[0]*(pointPart2->at(j).getX()) + qemL2[1]*(pointPart2->at(j).getY()) + qemL2[2]*(pointPart2->at(j).getZ()) + qemL2[3];
						 if(pointPart2->at(j).minqemindex == -1)
						 {
							 pointPart2->at(j).minqem1 = qem2;
							 pointPart2->at(j).minqemindex = relateP->at(k);
						 }
						 else
						 {
							 if(pointPart2->at(j).minqem1 > qem2)
							 {
								 pointPart2->at(j).minqem1 = qem2;
								 pointPart2->at(j).minqemindex = relateP->at(k);
							 }
						 }
					 }
				}
				delete relateP;
			}
			break;

		}

		case 3:
		{
			unsigned int pointPartSize3 = pointPart3->size();
			double qemL3[4];
			double qem3;
			for(unsigned int i = 0 ; i < pointPartSize3 ; i++)
			{
				setSonPartQEM(i , 3);
			}
			for(unsigned int j = 0 ; j < pointPartSize3 ; j++)
			{
				VECTORINT* relateP;
				relateP = getSonPartRelatePoint(j , 3);
				for(unsigned int k = 0 ; k < relateP->size() ; k++)
				{
					 if(relateP->at(k) != j)
					 {
						 qemL3[0] = (pointPart3->at(j).getX()) * (pointPart3->at(relateP->at(k)).qem1[0][0])
							 + (pointPart3->at(j).getY()) * (pointPart3->at(relateP->at(k)).qem1[1][0])
							 + (pointPart3->at(j).getZ()) * (pointPart3->at(relateP->at(k)).qem1[2][0])
							 + (pointPart3->at(relateP->at(k)).qem1[3][0]);

						 qemL3[1] = (pointPart3->at(j).getX()) * (pointPart3->at(relateP->at(k)).qem1[0][1])
							 + (pointPart3->at(j).getY()) * (pointPart3->at(relateP->at(k)).qem1[1][1])
							 + (pointPart3->at(j).getZ()) * (pointPart3->at(relateP->at(k)).qem1[2][1])
							 + (pointPart3->at(relateP->at(k)).qem1[3][1]);

						 qemL3[2] = (pointPart3->at(j).getX()) * (pointPart3->at(relateP->at(k)).qem1[0][2])
							 + (pointPart3->at(j).getY()) * (pointPart3->at(relateP->at(k)).qem1[1][2])
							 + (pointPart3->at(j).getZ()) * (pointPart3->at(relateP->at(k)).qem1[2][2])
							 + (pointPart3->at(relateP->at(k)).qem1[3][2]);

						 qemL3[3] = (pointPart3->at(j).getX()) * (pointPart3->at(relateP->at(k)).qem1[0][3])
							 + (pointPart3->at(j).getY()) * (pointPart3->at(relateP->at(k)).qem1[1][3])
							 + (pointPart3->at(j).getZ()) * (pointPart3->at(relateP->at(k)).qem1[2][3])
							 + (pointPart3->at(relateP->at(k)).qem1[3][3]);
						 qem3 = qemL3[0]*(pointPart3->at(j).getX()) + qemL3[1]*(pointPart3->at(j).getY()) + qemL3[2]*(pointPart3->at(j).getZ()) + qemL3[3];
						 if(pointPart3->at(j).minqemindex == -1)
						 {
							 pointPart3->at(j).minqem1 = qem3;
							 pointPart3->at(j).minqemindex = relateP->at(k);
						 }
						 else
						 {
							 if(pointPart3->at(j).minqem1 > qem3)
							 {
								 pointPart3->at(j).minqem1 = qem3;
								 pointPart3->at(j).minqemindex = relateP->at(k);
							 }
						 }
					 }
				}
				delete relateP;
			}
			break;

		}
	}
}

void setQemData(VECTORMYPOINT* rPoint)//每个点相邻点到该点的最小折叠系数
{
	unsigned int rPointSize = rPoint->size();
	double qemL[4];
	double qem;
	for(unsigned int i = 0 ; i < rPointSize ; i++)
	{
		
		setQEM(rPoint , i);

	}
	for(unsigned int j = 0 ; j < rPointSize ; j++)
	{
		
			/*VECTORINT* samePLine;
			samePLine = getSamePointLine(rPoint , j);*/
			VECTORINT* relateP;
			relateP = getRelatedPoint(j);
			for(unsigned int k = 0 ; k < relateP->size() ; k++)
			{
				if(relateP->at(k) != j)
				{
					qemL[0] = (rPoint->at(j).getX())*(rPoint->at(relateP->at(k)).qem1[0][0])
						+(rPoint->at(j).getY())*(rPoint->at(relateP->at(k)).qem1[1][0])
						+(rPoint->at(j).getZ())*(rPoint->at(relateP->at(k)).qem1[2][0])
						+(rPoint->at(relateP->at(k)).qem1[3][0]);

					qemL[1] = (rPoint->at(j).getX())*(rPoint->at(relateP->at(k)).qem1[0][1])
						+(rPoint->at(j).getY())*(rPoint->at(relateP->at(k)).qem1[1][1])
						+(rPoint->at(j).getZ())*(rPoint->at(relateP->at(k)).qem1[2][1])
						+(rPoint->at(relateP->at(k)).qem1[3][1]);

					qemL[2] = (rPoint->at(j).getX())*(rPoint->at(relateP->at(k)).qem1[0][2])
						+(rPoint->at(j).getY())*(rPoint->at(relateP->at(k)).qem1[1][2])
						+(rPoint->at(j).getZ())*(rPoint->at(relateP->at(k)).qem1[2][2])
						+(rPoint->at(relateP->at(k)).qem1[3][2]);

					qemL[3] = (rPoint->at(j).getX())*(rPoint->at(relateP->at(k)).qem1[0][3])
						+(rPoint->at(j).getY())*(rPoint->at(relateP->at(k)).qem1[1][3])
						+(rPoint->at(j).getZ())*(rPoint->at(relateP->at(k)).qem1[2][3])
						+(rPoint->at(relateP->at(k)).qem1[3][3]);

					qem = qemL[0]*(rPoint->at(j).getX()) + qemL[1]*(rPoint->at(j).getY()) + qemL[2]*(rPoint->at(j).getZ()) + qemL[3];
					if(rPoint->at(j).minqemindex == -1)
					{
						//rPoint->at(j).minqem1 = qem/(relateP->size());
						rPoint->at(j).minqem1 = qem;
						rPoint->at(j).minqemindex = relateP->at(k);
					}
					else
					{
						if(rPoint->at(j).minqem1>qem)
						{
							//rPoint->at(j).minqem1 = qem/(relateP->size());
							rPoint->at(j).minqem1 = qem;
							rPoint->at(j).minqemindex = relateP->at(k);
						}
						else
						{
							continue;
						}

					}
				}

				//if(lineFile->at(samePLine->at(k)).LeftPoint!=j)
				//{
				//	qemL[0] = (rPoint->at(j).getX())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[0][0])
				//		+(rPoint->at(j).getY())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[1][0])
				//		+(rPoint->at(j).getZ())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[2][0])
				//		+(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[3][0]);

				//	qemL[1] = (rPoint->at(j).getX())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[0][1])
				//		+(rPoint->at(j).getY())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[1][1])
				//		+(rPoint->at(j).getZ())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[2][1])
				//		+(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[3][1]);

				//	qemL[2] = (rPoint->at(j).getX())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[0][2])
				//		+(rPoint->at(j).getY())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[1][2])
				//		+(rPoint->at(j).getZ())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[2][2])
				//		+(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[3][2]);

				//	qemL[3] = (rPoint->at(j).getX())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[0][3])
				//		+(rPoint->at(j).getY())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[1][3])
				//		+(rPoint->at(j).getZ())*(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[2][3])
				//		+(rPoint->at(lineFile->at(samePLine->at(k)).LeftPoint).qem1[3][3]);

				//	qem = qemL[0]*(rPoint->at(j).getX()) + qemL[1]*(rPoint->at(j).getY()) + qemL[2]*(rPoint->at(j).getZ()) + qemL[3];
				//	if(rPoint->at(j).minqemindex == -1)
				//	{
				//		//rPoint->at(j).minqem1 = qem/(samePLine->size());
				//		rPoint->at(j).minqem1 = qem;
				//		rPoint->at(j).minqemindex = lineFile->at(samePLine->at(k)).LeftPoint;
				//	}
				//	else
				//	{
				//		if(rPoint->at(j).minqem1>qem)
				//		{
				//			//rPoint->at(j).minqem1 = qem/(samePLine->size());
				//			rPoint->at(j).minqem1 = qem;
				//			rPoint->at(j).minqemindex = lineFile->at(samePLine->at(k)).LeftPoint;
				//		}
				//		else
				//		{
				//			continue;
				//		}

				//	}

				//}
				//else
				//{
				//	qemL[0] = (rPoint->at(j).getX())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[0][0])
				//		+(rPoint->at(j).getY())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[1][0])
				//		+(rPoint->at(j).getZ())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[2][0])
				//		+(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[3][0]);

				//	qemL[1] = (rPoint->at(j).getX())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[0][1])
				//		+(rPoint->at(j).getY())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[1][1])
				//		+(rPoint->at(j).getZ())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[2][1])
				//		+(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[3][1]);

				//	qemL[2] = (rPoint->at(j).getX())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[0][2])
				//		+(rPoint->at(j).getY())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[1][2])
				//		+(rPoint->at(j).getZ())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[2][2])
				//		+(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[3][2]);

				//	qemL[3] = (rPoint->at(j).getX())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[0][3])
				//		+(rPoint->at(j).getY())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[1][3])
				//		+(rPoint->at(j).getZ())*(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[2][3])
				//		+(rPoint->at(lineFile->at(samePLine->at(k)).RightPoint).qem1[3][3]);

				//	qem = qemL[0]*(rPoint->at(j).getX()) + qemL[1]*(rPoint->at(j).getY()) + qemL[2]*(rPoint->at(j).getZ()) + qemL[3];
				//	if(rPoint->at(j).minqemindex == -1)
				//	{
				//		//rPoint->at(j).minqem1 = qem/(samePLine->size());
				//		rPoint->at(j).minqem1 = qem;
				//		rPoint->at(j).minqemindex = lineFile->at(samePLine->at(k)).RightPoint;
				//	}
				//	else
				//	{
				//		if(rPoint->at(j).minqem1>qem)
				//		{
				//			//rPoint->at(j).minqem1 = qem/(samePLine->size());
				//			rPoint->at(j).minqem1 = qem;
				//			rPoint->at(j).minqemindex = lineFile->at(samePLine->at(k)).RightPoint;
				//		}
				//		else
				//		{
				//			continue;
				//		}

				//	}
				//}
			}

			//delete samePLine;
			delete relateP;
		

	}

}

/*********************
计算单个点的二次误差，返回的是点
**********************/
MyPoint setSonPtQemData(MyPoint p , VECTORMYPOINT* pointPart , int index , VECTORMYTRI* triSonPart)
{
	double qemL[4];
	double qem;
	VECTORINT* relatedP;
	relatedP = getSonPartRelatePoint(p.index , index);
	if(p.index == 661)
	{
		int n = 0;
	}
	for(unsigned int i = 0 ; i < relatedP->size() ; i++)
	{
		if(relatedP->at(i) != p.index)
		{
			MyPoint relatePoint;
			relatePoint = setSonPartPtQem(pointPart->at(relatedP->at(i)) , pointPart , triSonPart);
			qemL[0] = p.getX()*(relatePoint.qem1[0][0])+p.getY()*(relatePoint.qem1[1][0])+p.getZ()*(relatePoint.qem1[2][0])+relatePoint.qem1[3][0];
			qemL[1] = p.getX()*(relatePoint.qem1[0][1])+p.getY()*(relatePoint.qem1[1][1])+p.getZ()*(relatePoint.qem1[2][1])+relatePoint.qem1[3][1];
			qemL[2] = p.getX()*(relatePoint.qem1[0][2])+p.getY()*(relatePoint.qem1[1][2])+p.getZ()*(relatePoint.qem1[2][2])+relatePoint.qem1[3][2];
			qemL[3] = p.getX()*(relatePoint.qem1[0][3])+p.getY()*(relatePoint.qem1[1][3])+p.getZ()*(relatePoint.qem1[2][3])+relatePoint.qem1[3][3];
			qem = qemL[0]*p.getX() + qemL[1]*p.getY() + qemL[2]*p.getZ()+qemL[3];

			if(p.minqemindex == -1)
			{
				//p.minqem1 = qem/(relateP->size());
				p.minqem1 = qem;
				p.minqemindex = relatedP->at(i);
			}
			else
			{
				if(p.minqem1>qem)
				{
					//p.minqem1 = qem/(relateP->size());
					p.minqem1 = qem;
					p.minqemindex = relatedP->at(i);
				}
			}
			
		}
	}

	delete relatedP;
	
	return p;

}

MyPoint setPtQEMData(MyPoint p , VECTORMYPOINT* rPoint)//边折叠系数
{
	//unsigned int rPointSize = rPoint->size();
	double qemL[4];
	double qem;
	//double relatePqem;
	//MyPoint newP;
	VECTORINT* relateP;
	relateP = getRelatedPoint(p.index);
	//newP = setPtQEM(p , rPoint);
	for(unsigned int i = 0 ; i < relateP->size() ; i++)
	{
		if(relateP->at(i)!=p.index)
		{
			MyPoint relatePoint;
			relatePoint = setPtQEM(rPoint->at(relateP->at(i)) , rPoint);
			qemL[0] = p.getX()*(relatePoint.qem1[0][0])+p.getY()*(relatePoint.qem1[1][0])+p.getZ()*(relatePoint.qem1[2][0])+relatePoint.qem1[3][0];
			qemL[1] = p.getX()*(relatePoint.qem1[0][1])+p.getY()*(relatePoint.qem1[1][1])+p.getZ()*(relatePoint.qem1[2][1])+relatePoint.qem1[3][1];
			qemL[2] = p.getX()*(relatePoint.qem1[0][2])+p.getY()*(relatePoint.qem1[1][2])+p.getZ()*(relatePoint.qem1[2][2])+relatePoint.qem1[3][2];
			qemL[3] = p.getX()*(relatePoint.qem1[0][3])+p.getY()*(relatePoint.qem1[1][3])+p.getZ()*(relatePoint.qem1[2][3])+relatePoint.qem1[3][3];
			qem = qemL[0]*p.getX() + qemL[1]*p.getY() + qemL[2]*p.getZ()+qemL[3];

			if(p.minqemindex == -1)
			{
				//p.minqem1 = qem/(relateP->size());
				p.minqem1 = qem;
				p.minqemindex = relateP->at(i);
			}
			else
			{
				if(p.minqem1>qem)
				{
					//p.minqem1 = qem/(relateP->size());
					p.minqem1 = qem;
					p.minqemindex = relateP->at(i);
				}
				else
				{
					continue;
				}

			}
		}
		else
		{
			continue;
		}
	}
	

	return p;

		

	
}

bool SortPoint(const MyPoint p1 , const MyPoint p2)
{
	return p1.minqem1 < p2.minqem1;
}

VECTORMYPOINT* qemSort(VECTORMYPOINT* pointPart)//对qem进行排序
{
	VECTORMYPOINT* rPointS = new VECTORMYPOINT;
	rPointS->insert(rPointS->end() , pointPart->begin() , pointPart->end());
	sort(rPointS->begin() , rPointS->end() , SortPoint);
	
	
	
	return rPointS;
	
}

bool ifdelRule(int p1 , int p2 , int p3 , VECTORMYPOINT* rPoint)//del rule
{
	MyPoint center;
	center = GetCircumCenter(rPoint->at(p1), rPoint->at(p2), rPoint->at(p3));
	double r;
	r = GetR(center, rPoint->at(p1));
	VECTORINT* pointRagei1 = new VECTORINT;//p1搜索范围
	VECTORINT* pointRagei2 = new VECTORINT;//p2搜索范围
	VECTORINT* pointRagei3 = new VECTORINT;//p3
	VECTORINT* pointRagei = new VECTORINT; //总的搜索范围
	pointRagei1->assign(pointRage->at(p1).getAllRageIndex()->begin() , pointRage->at(p1).getAllRageIndex()->end());
	pointRagei2->assign(pointRage->at(p2).getAllRageIndex()->begin() , pointRage->at(p2).getAllRageIndex()->end());
	pointRagei3->assign(pointRage->at(p3).getAllRageIndex()->begin() , pointRage->at(p3).getAllRageIndex()->end());
	pointRagei->insert(pointRagei->end() , pointRagei1->begin() , pointRagei1->end());
	pointRagei->insert(pointRagei->end() , pointRagei2->begin() , pointRagei2->end());
	pointRagei->insert(pointRagei->end() , pointRagei3->begin() , pointRagei3->end());
	unsigned int pointRageisize = pointRagei->size();
	bool result = true;
	for(unsigned int i = 0 ; i < pointRageisize ; i++)
	{
		if(pointRagei->at(i)!=p1&&pointRagei->at(i)!=p2&&pointRagei->at(i)!=p3)
		{
			double d = Get3dDis(center, rPoint->at(pointRagei->at(i)));
			if (d < r)
            {
				if(ifExistcos(pointRagei->at(i) , deletePoint))
				{
					continue;
				}
				else
				{
					result = false;
					return result;
				}


            }
			else
			{
				continue;
			}

		}
		else
		{
			continue;
		}
	}

	return result;


}

/*******************
获取与p点相连的点集，包括p点
**********************/
VECTORINT* getSonPartRelatePoint(int p , int index)
{
	VECTORINT* relateP = new VECTORINT;
	switch(index)
	{
	case 0:
		{
			unsigned int triSonPartTempSize0 = triSonPartTemp0->size();
			//VECTORINT* relateP = new VECTORINT;
			for(unsigned int i = 0 ; i < triSonPartTempSize0 ; i++)
			{
				if(p == triSonPartTemp0->at(i).p1 || p == triSonPartTemp0->at(i).p2  || p == triSonPartTemp0->at(i).p3)
				{
					if(triSonPartTemp0->at(i).noActive)
					{
						continue;
					}
					else
					{
						if(!ifExistcos(triSonPartTemp0->at(i).p1 , relateP) && !ifExistcos(triSonPartTemp0->at(i).p1 , deleteSonPoint0))
						{
							relateP->push_back(triSonPartTemp0->at(i).p1);
						}
						if(!ifExistcos(triSonPartTemp0->at(i).p2 , relateP) && !ifExistcos(triSonPartTemp0->at(i).p2 , deleteSonPoint0))
						{
							relateP->push_back(triSonPartTemp0->at(i).p2);
						}
						if(!ifExistcos(triSonPartTemp0->at(i).p3 , relateP) && !ifExistcos(triSonPartTemp0->at(i).p3 , deleteSonPoint0))
						{
							relateP->push_back(triSonPartTemp0->at(i).p3);
						}
					}
				}
			}
			break;
			//return relateP;
		}

		case 1:
		{
			unsigned int triSonPartTempSize1 = triSonPartTemp1->size();
			//VECTORINT* relateP = new VECTORINT;
			for(unsigned int i = 0 ; i < triSonPartTempSize1 ; i++)
			{
				if(p == triSonPartTemp1->at(i).p1 || p == triSonPartTemp1->at(i).p2  || p == triSonPartTemp1->at(i).p3)
				{
					if(triSonPartTemp1->at(i).noActive)
					{
						continue;
					}
					else
					{
						if(!ifExistcos(triSonPartTemp1->at(i).p1 , relateP) && !ifExistcos(triSonPartTemp1->at(i).p1 , deleteSonPoint1))
						{
							relateP->push_back(triSonPartTemp1->at(i).p1);
						}
						if(!ifExistcos(triSonPartTemp1->at(i).p2 , relateP) && !ifExistcos(triSonPartTemp1->at(i).p2 , deleteSonPoint1))
						{
							relateP->push_back(triSonPartTemp1->at(i).p2);
						}
						if(!ifExistcos(triSonPartTemp1->at(i).p3 , relateP) && !ifExistcos(triSonPartTemp1->at(i).p3 , deleteSonPoint1))
						{
							relateP->push_back(triSonPartTemp1->at(i).p3);
						}
					}
				}
			}
			//return relateP;
			break;
		}

		case 2:
		{
			unsigned int triSonPartTempSize2 = triSonPartTemp2->size();
			//VECTORINT* relateP = new VECTORINT;
			for(unsigned int i = 0 ; i < triSonPartTempSize2 ; i++)
			{
				if(p == triSonPartTemp2->at(i).p1 || p == triSonPartTemp2->at(i).p2  || p == triSonPartTemp2->at(i).p3)
				{
					if(triSonPartTemp2->at(i).noActive)
					{
						continue;
					}
					else
					{
						if(!ifExistcos(triSonPartTemp2->at(i).p1 , relateP) && !ifExistcos(triSonPartTemp2->at(i).p1 , deleteSonPoint2))
						{
							relateP->push_back(triSonPartTemp2->at(i).p1);
						}
						if(!ifExistcos(triSonPartTemp2->at(i).p2 , relateP) && !ifExistcos(triSonPartTemp2->at(i).p2 , deleteSonPoint2))
						{
							relateP->push_back(triSonPartTemp2->at(i).p2);
						}
						if(!ifExistcos(triSonPartTemp2->at(i).p3 , relateP) && !ifExistcos(triSonPartTemp2->at(i).p3 , deleteSonPoint2))
						{
							relateP->push_back(triSonPartTemp2->at(i).p3);
						}
					}
				}
			}
			//return relateP;
			break;
		}

		case 3:
		{
			unsigned int triSonPartTempSize3 = triSonPartTemp3->size();
			//VECTORINT* relateP = new VECTORINT;
			for(unsigned int i = 0 ; i < triSonPartTempSize3 ; i++)
			{
				if(p == triSonPartTemp3->at(i).p1 || p == triSonPartTemp3->at(i).p2  || p == triSonPartTemp3->at(i).p3)
				{
					if(triSonPartTemp3->at(i).noActive)
					{
						continue;
					}
					else
					{
						if(!ifExistcos(triSonPartTemp3->at(i).p1 , relateP) && !ifExistcos(triSonPartTemp3->at(i).p1 , deleteSonPoint3))
						{
							relateP->push_back(triSonPartTemp3->at(i).p1);
						}
						if(!ifExistcos(triSonPartTemp3->at(i).p2 , relateP) && !ifExistcos(triSonPartTemp3->at(i).p2 , deleteSonPoint3))
						{
							relateP->push_back(triSonPartTemp3->at(i).p2);
						}
						if(!ifExistcos(triSonPartTemp3->at(i).p3 , relateP) && !ifExistcos(triSonPartTemp3->at(i).p3 , deleteSonPoint3))
						{
							relateP->push_back(triSonPartTemp3->at(i).p3);
						}
					}
				}
			}
			//return relateP;
			break;
		}
		default:
			cout<<"查找子块"<<index<<"的index为"<<index<<"的相关点时出错"<<endl;
			break;
	}
	return relateP;

}

VECTORINT* getRelatedPoint(int p)//获得与p点相连的点集包含p点
{
	unsigned int TriFilesize = TriFile1->size();
	VECTORINT* relatedP = new VECTORINT;
	for(unsigned int i = 0 ; i < TriFilesize ; i++)
	{
		if(p == TriFile1->at(i).p1||p == TriFile1->at(i).p2||p == TriFile1->at(i).p3)
		{
			if(TriFile1->at(i).noActive)
			{
				continue;
			}
			else
			{
				if(!ifExistcos(TriFile1->at(i).p1 , relatedP)&&!ifExistcos(TriFile1->at(i).p1 , deletePoint))
				{
					relatedP->push_back(TriFile1->at(i).p1);
				}
				//else
				//{
				//	//continue;
				//}
				if(!ifExistcos(TriFile1->at(i).p2 , relatedP)&&!ifExistcos(TriFile1->at(i).p2 , deletePoint))
				{
					relatedP->push_back(TriFile1->at(i).p2);
				}
				//else
				//{
				//	//continue;
				//}
				if(!ifExistcos(TriFile1->at(i).p3 , relatedP)&&!ifExistcos(TriFile1->at(i).p3 , deletePoint))
				{
					relatedP->push_back(TriFile1->at(i).p3);
				}
				//else
				//{
				//	//continue;
				//}
			}

		}

	}
	return relatedP;

}

bool ifConvexPolygon(VECTORMYTRI* sameTri , int oldP , int newP , VECTORMYPOINT* pointPart)//判断是否为凸多边形
{
	VECTORINT* clockwisePoint = new VECTORINT;
	bool ifConvex = true;
	bool ifAddClock = false;
	clockwisePoint->push_back(newP);
	/*try
	{*/
	for(unsigned int i = 0 ; i < sameTri->size()-1 ; i++)
	{
		ifAddClock = false;
		for(unsigned int j = 0 ; j < sameTri->size(); j++)
		{
			if(sameTri->at(j).p1 == oldP)
			{
				if(sameTri->at(j).p2 == clockwisePoint->at(i))
				{
					if(jugleLeftOrRight(pointPart->at(oldP) , pointPart->at(sameTri->at(j).p2) , pointPart->at(sameTri->at(j).p3)) == 1)
					{
						ifAddClock = true;
						clockwisePoint->push_back(sameTri->at(j).p3);
						break;
					}
					
					
				}
				else if(sameTri->at(j).p3 == clockwisePoint->at(i))
				{
					if(jugleLeftOrRight(pointPart->at(oldP) , pointPart->at(sameTri->at(j).p3) , pointPart->at(sameTri->at(j).p2)) == 1)
					{
						ifAddClock = true;
						clockwisePoint->push_back(sameTri->at(j).p2);
						break;
					}
				}
				/*else
				{
					cout<<"在求顺时针相关点数组时出错！"<<endl;
				}*/
			}
			else if(sameTri->at(j).p2 == oldP)
			{
				if(sameTri->at(j).p1 == clockwisePoint->at(i))
				{
					if(jugleLeftOrRight(pointPart->at(oldP) , pointPart->at(sameTri->at(j).p1) , pointPart->at(sameTri->at(j).p3)) == 1)
					{
						ifAddClock = true;
						clockwisePoint->push_back(sameTri->at(j).p3);
						break;
					}
				}
				else if(sameTri->at(j).p3 == clockwisePoint->at(i))
				{
					if(jugleLeftOrRight(pointPart->at(oldP) , pointPart->at(sameTri->at(j).p3) , pointPart->at(sameTri->at(j).p1)) == 1)
					{
						ifAddClock = true;
						clockwisePoint->push_back(sameTri->at(j).p1);
						break;
					}
				}
				/*else
				{
					cout<<"在求顺时针相关点数组时出错！"<<endl;
				}*/
			}
			else if(sameTri->at(j).p3 == oldP)
			{
				if(sameTri->at(j).p1 == clockwisePoint->at(i))
				{
					if(jugleLeftOrRight(pointPart->at(oldP) , pointPart->at(sameTri->at(j).p1) , pointPart->at(sameTri->at(j).p2)) == 1)
					{
						ifAddClock = true;
						clockwisePoint->push_back(sameTri->at(j).p2);
						break;
					}
				}
				else if(sameTri->at(j).p2 == clockwisePoint->at(i))
				{
					if(jugleLeftOrRight(pointPart->at(oldP) , pointPart->at(sameTri->at(j).p2) , pointPart->at(sameTri->at(j).p1)) == 1)
					{
						ifAddClock = true;
						clockwisePoint->push_back(sameTri->at(j).p1);
						break;
					}
				}
				/*else
				{
					cout<<"在求顺时针相关点数组时出错！"<<endl;
				}*/
			}
			else
			{
				cout<<"在进行判断是否有不符合的多边形是出错！"<<endl;
			}
		}
		if(!ifAddClock)
		{
			ifConvex = false;
			return ifConvex;
		}
		
	}
	/*}
	catch(exception& e)
	{
		cout<<e.what()<<endl;
		return false;
	}*/

	double anglemin = getAngle(pointPart->at(oldP) , pointPart->at(clockwisePoint->at(1)) , pointPart->at(newP));
	double angle , angle1;
	double cross;
	
	for(unsigned int n = 1 ; n < clockwisePoint->size() ; n++)
	{
		angle = getAngle(pointPart->at(oldP) , pointPart->at(clockwisePoint->at(n)) , pointPart->at(newP));
		cross = (pointPart->at(oldP).getX() - pointPart->at(newP).getX())*(pointPart->at(clockwisePoint->at(n)).getY() - pointPart->at(newP).getY())
			-(pointPart->at(clockwisePoint->at(n)).getX() - pointPart->at(newP).getX())*(pointPart->at(oldP).getY() - pointPart->at(newP).getY());
		if(cross<0)
		{
			angle1 = -angle;
			angle = angle1;
		}

		if(angle>anglemin)
		{
			ifConvex = false;
			break;
		}
		else
		{
			anglemin = angle;
		}
	}
	delete clockwisePoint;
	return ifConvex;

}

void ensureSonPartConvex()
{
	for(unsigned int i = 0 ; i < convexSonPart0->size() ; i++)
	{
		pointPart0->at(convexSonPart0->at(i)).ifCon = true;
	}
	for(unsigned int j = 0 ; j < convexSonPart1->size() ; j++)
	{
		pointPart1->at(convexSonPart1->at(j)).ifCon = true;
	}
	for(unsigned int k = 0 ; k < convexSonPart2->size() ; k++)
	{
		pointPart2->at(convexSonPart2->at(k)).ifCon = true;
	}
	for(unsigned int l = 0 ; l < convexSonPart3->size() ; l++)
	{
		pointPart3->at(convexSonPart3->at(l)).ifCon = true;
	}
}

void ensureCon()
{
	for(unsigned int i = 0 ; i < convexP1->size() ; i++)
	{
		rPoint->at(convexP1->at(i)).ifCon = true;
	}
	//return rPoint;
}
/************************
求解子块内四角点
pointPart为点集数据
pointIndexVector为边界点集的index数组
************************/

VECTORMYPOINT* getFourValue(VECTORMYPOINT* pointPart , VECTORINT* pointIndexVector)
{
	//MyPoint fourPoint[4];
	VECTORMYPOINT* fourPoint = new VECTORMYPOINT;
	double maxXValue = pointPart->at(pointIndexVector->at(0)).getX();
	double maxYValue = pointPart->at(pointIndexVector->at(0)).getY();
	double minXValue = pointPart->at(pointIndexVector->at(0)).getX();
	double minYValue = pointPart->at(pointIndexVector->at(0)).getY();
	double tempMinX;
	double tempMaxX;
	double tempMinY;
	double tempMaxY;

	unsigned int pointIndexVectorSize = pointIndexVector->size();
	for(unsigned int i = 0 ; i < pointIndexVectorSize ; i++)
	{
		double numX = pointPart->at(pointIndexVector->at(i)).getX();
		double numY = pointPart->at(pointIndexVector->at(i)).getY();
		tempMaxX = maxXValue;
		tempMinX = minXValue;
		tempMaxY = maxYValue;
		tempMinY = minYValue;
		if(numX > tempMaxX)
		{
			maxXValue = numX;
		}
		if(numY > tempMaxY)
		{
			maxYValue = numY;
		}
		if(numX < tempMinX)
		{
			minXValue = numX;
		}
		if(numY <tempMinY)
		{
			minYValue = numY;
		}

	}
	MyPoint* pointLeftButtom = new MyPoint(minXValue , minYValue , 0);
	fourPoint->push_back(*pointLeftButtom);
	MyPoint* pointRightButtom = new MyPoint(maxXValue , minYValue , 0);
	fourPoint->push_back(*pointRightButtom);
	MyPoint* pointRightTop = new MyPoint(maxXValue , maxYValue , 0);
	fourPoint->push_back(*pointRightTop);
	MyPoint* pointLeftTop = new MyPoint(minXValue , maxYValue , 0);
	fourPoint->push_back(*pointLeftTop);

	/*fourPoint[0].setX(minXValue);
	fourPoint[0].setY(minYValue);

	fourPoint[1].setX(maxXValue);
	fourPoint[1].setY(minYValue);

	fourPoint[2].setX(maxXValue);
	fourPoint[2].setY(maxYValue);

	fourPoint[3].setX(minXValue);
	fourPoint[3].setY(maxYValue);*/

	fourPointAll->push_back(*pointLeftButtom);
	fourPointAll->push_back(*pointRightButtom);
	fourPointAll->push_back(*pointRightTop);
	fourPointAll->push_back(*pointLeftTop);

	return fourPoint;


}

/*************************
确定每个子块点云文件的四个角点，用于实现两子块之间顶线底线连接
左右子块顶线为左子块右上角和右子块左上角
边界点由Y值最小处起始逆时针方向存储
子块左上角为0，右上角1，左下角2，右下角3
四个角点存储方向从左下角开始逆时针旋转
**********************/
void EnSureCornerPoint(int index)
{
	switch(index)
	{
	case 0:
		{
			VECTORMYPOINT* fourPoint;
			fourPoint = getFourValue(pointPart0 , convexSonPart0);
			int minRightTopIndex = 0;
			int minRightButtomIndex = 0;
			int minLeftTopIndex = 0;
			int minLeftButtomIndex = 0;

			double minRightTopDis = Get3dDis(fourPoint->at(2) , pointPart0->at(convexSonPart0->at(0)));
			double minRightButtomDis = Get3dDis(fourPoint->at(1) , pointPart0->at(convexSonPart0->at(0)));
			double minLeftTopDis = Get3dDis(fourPoint->at(3) , pointPart0->at(convexSonPart0->at(0)));;
			double minLeftButtomDis = Get3dDis(fourPoint->at(0) , pointPart0->at(convexSonPart0->at(0)));;

			for(unsigned int i = 0 ; i < convexSonPart0->size() ; i++)
			{
				double dis1 = Get3dDis(fourPoint->at(2) , pointPart0->at(convexSonPart0->at(i)));//右上角点与边界点集的距离，目的是找出其中的最近点作为边界点集的右上角点
				double dis2 = Get3dDis(fourPoint->at(1) , pointPart0->at(convexSonPart0->at(i)));
				double dis3 = Get3dDis(fourPoint->at(3) , pointPart0->at(convexSonPart0->at(i)));
				double dis4 = Get3dDis(fourPoint->at(0) , pointPart0->at(convexSonPart0->at(i)));

				if(dis1 < minRightTopDis)
				{
					minRightTopDis = dis1;
					minRightTopIndex = i;
				}
				if(dis2 < minRightButtomDis)
				{
					minRightButtomDis = dis2;
					minRightButtomIndex = i;
				}
				if(dis3 < minLeftTopDis)
				{
					minLeftTopDis = dis3;
					minLeftTopIndex = i;
				}
				if(dis4 < minLeftButtomDis)
				{
					minLeftButtomDis = dis4;
					minLeftButtomIndex = i;
				}
			}

			/*for(unsigned int i = 0 ; i < convexSonPart0->size() ; i++)
			{
				double dis1 = Get3dDis(fourPoint->at(2) , pointPart0->at(convexSonPart0->at(i)));
				if(dis1 < minRightTopDis)
				{
					minRightTopDis = dis1;
					minRightTopIndex = i;
				}
			}
			for(unsigned int i = 0 ; i < convexSonPart0->size() ; i++)
			{
				double dis2 = Get3dDis(fourPoint->at(1) , pointPart0->at(convexSonPart0->at(i)));
				if(dis2 <minRightButtomDis)
				{
					minRightButtomDis = dis2;
					minRightButtomIndex = i;
				}
			}
			for(unsigned int i = 0 ; i < convexSonPart0->size() ; i++)
			{
				double dis3 = Get3dDis(fourPoint->at(0) , pointPart0->at(convexSonPart0->at(i)));
				if(dis3 < minLeftButtomDis)
				{
					minLeftButtomDis = dis3;
					minLeftButtomIndex = i;
				}
			}
			for(unsigned int i = 0 ; i < convexSonPart0->size() ; i++)
			{
				double dis4 = Get3dDis(fourPoint->at(3) , pointPart0->at(convexSonPart0->at(i)));
				if(dis4 < minLeftTopDis)
				{
					minLeftTopDis = dis4;
					minLeftTopIndex = i;
				}
			}*/
			cornerPoint0[0] = minLeftButtomIndex;
			cornerPoint0[1] = minRightButtomIndex;
			cornerPoint0[2] = minRightTopIndex;
			cornerPoint0[3] = minLeftTopIndex;
			break;
			
		}
	case 1:
		{
			VECTORMYPOINT* fourPoint;
			fourPoint = getFourValue(pointPart1 , convexSonPart1);
			int minRightTopIndex = 0;
			int minRightButtomIndex = 0;
			int minLeftTopIndex = 0;
			int minLeftButtomIndex = 0;

			double minRightTopDis = Get3dDis(fourPoint->at(2) , pointPart1->at(convexSonPart1->at(0)));
			double minRightButtomDis = Get3dDis(fourPoint->at(1) , pointPart1->at(convexSonPart1->at(0)));
			double minLeftTopDis = Get3dDis(fourPoint->at(3) , pointPart1->at(convexSonPart1->at(0)));;
			double minLeftButtomDis = Get3dDis(fourPoint->at(0) , pointPart1->at(convexSonPart1->at(0)));;

			for(unsigned int i = 0 ; i < convexSonPart1->size() ; i++)
			{
				double dis1 = Get3dDis(fourPoint->at(2) , pointPart1->at(convexSonPart1->at(i)));//右上角点与边界点集的距离，目的是找出其中的最近点作为边界点集的右上角点
				double dis2 = Get3dDis(fourPoint->at(1) , pointPart1->at(convexSonPart1->at(i)));
				double dis3 = Get3dDis(fourPoint->at(3) , pointPart1->at(convexSonPart1->at(i)));
				double dis4 = Get3dDis(fourPoint->at(0) , pointPart1->at(convexSonPart1->at(i)));

				if(dis1 < minRightTopDis)
				{
					minRightTopDis = dis1;
					minRightTopIndex = i;
				}
				if(dis2 < minRightButtomDis)
				{
					minRightButtomDis = dis2;
					minRightButtomIndex = i;
				}
				if(dis3 < minLeftTopDis)
				{
					minLeftTopDis = dis3;
					minLeftTopIndex = i;
				}
				if(dis4 < minLeftButtomDis)
				{
					minLeftButtomDis = dis4;
					minLeftButtomIndex = i;
				}
			}
			cornerPoint1[0] = minLeftButtomIndex;
			cornerPoint1[1] = minRightButtomIndex;
			cornerPoint1[2] = minRightTopIndex;
			cornerPoint1[3] = minLeftTopIndex;
			break;
		}
	case 2:
		{
			VECTORMYPOINT* fourPoint;
			fourPoint = getFourValue(pointPart2 , convexSonPart2);
			int minRightTopIndex = 0;
			int minRightButtomIndex = 0;
			int minLeftTopIndex = 0;
			int minLeftButtomIndex = 0;

			double minRightTopDis = Get3dDis(fourPoint->at(2) , pointPart2->at(convexSonPart2->at(0)));
			double minRightButtomDis = Get3dDis(fourPoint->at(1) , pointPart2->at(convexSonPart2->at(0)));
			double minLeftTopDis = Get3dDis(fourPoint->at(3) , pointPart2->at(convexSonPart2->at(0)));;
			double minLeftButtomDis = Get3dDis(fourPoint->at(0) , pointPart2->at(convexSonPart2->at(0)));;

			for(unsigned int i = 0 ; i < convexSonPart2->size() ; i++)
			{
				double dis1 = Get3dDis(fourPoint->at(2) , pointPart2->at(convexSonPart2->at(i)));//右上角点与边界点集的距离，目的是找出其中的最近点作为边界点集的右上角点
				double dis2 = Get3dDis(fourPoint->at(1) , pointPart2->at(convexSonPart2->at(i)));
				double dis3 = Get3dDis(fourPoint->at(3) , pointPart2->at(convexSonPart2->at(i)));
				double dis4 = Get3dDis(fourPoint->at(0) , pointPart2->at(convexSonPart2->at(i)));

				if(dis1 < minRightTopDis)
				{
					minRightTopDis = dis1;
					minRightTopIndex = i;
				}
				if(dis2 < minRightButtomDis)
				{
					minRightButtomDis = dis2;
					minRightButtomIndex = i;
				}
				if(dis3 < minLeftTopDis)
				{
					minLeftTopDis = dis3;
					minLeftTopIndex = i;
				}
				if(dis4 < minLeftButtomDis)
				{
					minLeftButtomDis = dis4;
					minLeftButtomIndex = i;
				}
			}
			cornerPoint2[0] = minLeftButtomIndex;
			cornerPoint2[1] = minRightButtomIndex;
			cornerPoint2[2] = minRightTopIndex;
			cornerPoint2[3] = minLeftTopIndex;
			break;
		}
	case 3:
		{
			VECTORMYPOINT* fourPoint;
			fourPoint = getFourValue(pointPart3 , convexSonPart3);
			int minRightTopIndex = 0;
			int minRightButtomIndex = 0;
			int minLeftTopIndex = 0;
			int minLeftButtomIndex = 0;

			double minRightTopDis = Get3dDis(fourPoint->at(2) , pointPart3->at(convexSonPart3->at(0)));
			double minRightButtomDis = Get3dDis(fourPoint->at(1) , pointPart3->at(convexSonPart3->at(0)));
			double minLeftTopDis = Get3dDis(fourPoint->at(3) , pointPart3->at(convexSonPart3->at(0)));;
			double minLeftButtomDis = Get3dDis(fourPoint->at(0) , pointPart3->at(convexSonPart3->at(0)));;

			for(unsigned int i = 0 ; i < convexSonPart3->size() ; i++)
			{
				double dis1 = Get3dDis(fourPoint->at(2) , pointPart3->at(convexSonPart3->at(i)));//右上角点与边界点集的距离，目的是找出其中的最近点作为边界点集的右上角点
				double dis2 = Get3dDis(fourPoint->at(1) , pointPart3->at(convexSonPart3->at(i)));
				double dis3 = Get3dDis(fourPoint->at(3) , pointPart3->at(convexSonPart3->at(i)));
				double dis4 = Get3dDis(fourPoint->at(0) , pointPart3->at(convexSonPart3->at(i)));

				if(dis1 < minRightTopDis)
				{
					minRightTopDis = dis1;
					minRightTopIndex = i;
				}
				if(dis2 < minRightButtomDis)
				{
					minRightButtomDis = dis2;
					minRightButtomIndex = i;
				}
				if(dis3 < minLeftTopDis)
				{
					minLeftTopDis = dis3;
					minLeftTopIndex = i;
				}
				if(dis4 < minLeftButtomDis)
				{
					minLeftButtomDis = dis4;
					minLeftButtomIndex = i;
				}
			}
			cornerPoint3[0] = minLeftButtomIndex;
			cornerPoint3[1] = minRightButtomIndex;
			cornerPoint3[2] = minRightTopIndex;
			cornerPoint3[3] = minLeftTopIndex;
			break;
		}
	}
}


/*****************************
为边界点添加标注
**********************/

void EnSureAllConvex()
{
	for(unsigned int i = 0 ; i < convexSonPart0->size() ; i++)
	{
		pointPart0->at(convexSonPart0->at(i)).ifCon = true;
	}
	for(unsigned int j = 0 ; j < convexSonPart1->size() ; j++)
	{
		pointPart1->at(convexSonPart1->at(j)).ifCon = true;
	}
	for(unsigned int k = 0 ; k < convexSonPart2->size() ; k++)
	{
		pointPart2->at(convexSonPart2->at(k)).ifCon = true;
	}
	for(unsigned int l = 0 ; l < convexSonPart3->size() ; l++)
	{
		pointPart3->at(convexSonPart3->at(l)).ifCon = true;
	}
}

/**********************
边折叠信息的存储
************************/
void changeDataSonPartSave(VECTORMYPOINT* pointPart , VECTORMYPOINT* pointPartQem , int index)
{
	for(unsigned int i = 0 ; i < pointPartQem->size() ; i++)
	{
		
		if(pointPart->at(pointPartQem->at(i).minqemindex).ifCon)
		{
			continue;
		}
		else
		{
			VECTORMYTRI* sameTri;
			VECTORINT* relatedP;
			ChangeData* changeData = new ChangeData;
			switch(index)
			{
			case 0:
				{
					/*if(i == 201)
					{
						int n = 0;
					}*/
					sameTri = getSonPartSamePointTri(triSonPartTemp0 , pointPartQem->at(i).minqemindex);
					relatedP = getSonPartRelatePoint(pointPartQem->at(i).minqemindex , 0);
					bool ifcon = true;
					try
					{
						ifcon = ifConvexPolygon(sameTri , pointPartQem->at(i).minqemindex , pointPartQem->at(i).index , pointPart0);
					}
					catch(exception& e)
					{
						cout<<"出错子块为"<<index<<" "<<e.what()<<endl;
						continue;
					}
					if(!ifcon)
					{
						delete sameTri;
						delete relatedP;
						delete changeData;
						continue;
						
					}
					
					for(unsigned int j = 0 ; j < sameTri->size() ; j++)
					{
						if(sameTri->at(j).p1 == pointPartQem->at(i).index || sameTri->at(j).p2 == pointPartQem->at(i).index || sameTri->at(j).p3 == pointPartQem->at(i).index)
						{
							sameTri->at(j).noActive = true;
							triSonPartTemp0->at(sameTri->at(j).index).noActive = true;
							changeData->insertNoActive(sameTri->at(j));
						}
						else
						{
							if(sameTri->at(j).p1 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p1 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp0->at(sameTri->at(j).index).p1 = pointPartQem->at(i).index;
								triSonPartTemp0->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else if(sameTri->at(j).p2 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p2 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp0->at(sameTri->at(j).index).p2 = pointPartQem->at(i).index;
								triSonPartTemp0->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else if(sameTri->at(j).p3 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p3 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp0->at(sameTri->at(j).index).p3 = pointPartQem->at(i).index;
								triSonPartTemp0->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else
							{
								cout << "三角形边折叠记录存储时出错，index = " << index << endl;
							}

						}
					}
					if(changeData->noActiveTri->size() != 2)
					{
						delete sameTri;
						delete relatedP;
						delete[] changeData;
						continue;
					}
					changeSonPartData0->push_back(*changeData);
					for(unsigned int m = 0 ; m < relatedP->size() ; m++)
					{
						MyPoint newP;
						MyPoint newP1;
						if(relatedP->at(m) == pointPartQem->at(i).minqemindex)
						{
							newP.ifdelete = true;
							newP.index = relatedP->at(m);
							changeSonPartPLV0->push_back(newP);
							deleteSonPoint0->push_back(pointPartQem->at(i).minqemindex);
							continue;
						}

						VECTORMYTRI* rePtsameTri;
						rePtsameTri = getSonPartSamePointTri(triSonPartTemp0 , relatedP->at(m));
						newP.index = relatedP->at(m);
						newP.setX(pointPart0->at(relatedP->at(m)).getX());
						newP.setY(pointPart0->at(relatedP->at(m)).getY());
						newP.setZ(pointPart0->at(relatedP->at(m)).getZ());
						//可有可无，只是使该点的属性完整
						newP.minqem1 = pointPart0->at(relatedP->at(m)).minqem1;
						newP.minqemindex = -1;
						newP1 = setSonPtQemData(newP , pointPart , 0 , triSonPartTemp0);
						delete rePtsameTri;
						changeSonPartPLV0->push_back(newP1);

					}
					delete sameTri;
					delete relatedP;
					delete[] changeData;
					break;
				}
			case 1:
				{
					sameTri = getSonPartSamePointTri(triSonPartTemp1 , pointPartQem->at(i).minqemindex);
					relatedP = getSonPartRelatePoint(pointPartQem->at(i).minqemindex , 1);
					bool ifcon = true;
					try
					{
						ifcon = ifConvexPolygon(sameTri , pointPartQem->at(i).minqemindex , pointPartQem->at(i).index , pointPart1);
					}
					catch(exception& e)
					{
						cout<<"出错子块为"<<index<<" "<<e.what()<<endl;
						continue;
					}
					if(!ifcon)
					{
						delete sameTri;
						delete relatedP;
						delete changeData;
						continue;
						
					}
					
					for(unsigned int j = 0 ; j < sameTri->size() ; j++)
					{
						if(sameTri->at(j).p1 == pointPartQem->at(i).index || sameTri->at(j).p2 == pointPartQem->at(i).index || sameTri->at(j).p3 == pointPartQem->at(i).index)
						{
							sameTri->at(j).noActive = true;
							triSonPartTemp1->at(sameTri->at(j).index).noActive = true;
							changeData->insertNoActive(sameTri->at(j));
						}
						else
						{
							if(sameTri->at(j).p1 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p1 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp1->at(sameTri->at(j).index).p1 = pointPartQem->at(i).index;
								triSonPartTemp1->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else if(sameTri->at(j).p2 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p2 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp1->at(sameTri->at(j).index).p2 = pointPartQem->at(i).index;
								triSonPartTemp1->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else if(sameTri->at(j).p3 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p3 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp1->at(sameTri->at(j).index).p3 = pointPartQem->at(i).index;
								triSonPartTemp1->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else
							{
								cout << "三角形边折叠记录存储时出错，index = " << index << endl;
							}

						}
					}
					if(changeData->noActiveTri->size() != 2)
					{
						delete sameTri;
						delete relatedP;
						delete[] changeData;
						continue;
					}
					changeSonPartData1->push_back(*changeData);
					for(unsigned int m = 0 ; m < relatedP->size() ; m++)
					{
						MyPoint newP;
						MyPoint newP1;
						if(relatedP->at(m) == pointPartQem->at(i).minqemindex)
						{
							newP.ifdelete = true;
							newP.index = relatedP->at(m);
							changeSonPartPLV1->push_back(newP);
							deleteSonPoint1->push_back(pointPartQem->at(i).minqemindex);
							continue;
						}

						VECTORMYTRI* rePtsameTri;
						rePtsameTri = getSonPartSamePointTri(triSonPartTemp1 , relatedP->at(m));
						newP.index = relatedP->at(m);
						newP.setX(pointPart1->at(relatedP->at(m)).getX());
						newP.setY(pointPart1->at(relatedP->at(m)).getY());
						newP.setZ(pointPart1->at(relatedP->at(m)).getZ());
						//可有可无，只是使该点的属性完整
						newP.minqem1 = pointPart1->at(relatedP->at(m)).minqem1;
						newP.minqemindex = -1;
						newP1 = setSonPtQemData(newP , pointPart , 1 , triSonPartTemp1);
						delete rePtsameTri;
						changeSonPartPLV1->push_back(newP1);

					}
					delete sameTri;
					delete relatedP;
					delete[] changeData;
					break;
				}
			case 2:
				{
					sameTri = getSonPartSamePointTri(triSonPartTemp2 , pointPartQem->at(i).minqemindex);
					relatedP = getSonPartRelatePoint(pointPartQem->at(i).minqemindex , 2);
					bool ifcon = true;
					try
					{
						ifcon = ifConvexPolygon(sameTri , pointPartQem->at(i).minqemindex , pointPartQem->at(i).index , pointPart2);
					}
					catch(exception& e)
					{
						cout<<"出错子块为"<<index<<" "<<e.what()<<endl;
						continue;
					}
					if(!ifcon)
					{
						delete sameTri;
						delete relatedP;
						delete changeData;
						continue;
						
					}
					
					for(unsigned int j = 0 ; j < sameTri->size() ; j++)
					{
						if(sameTri->at(j).p1 == pointPartQem->at(i).index || sameTri->at(j).p2 == pointPartQem->at(i).index || sameTri->at(j).p3 == pointPartQem->at(i).index)
						{
							sameTri->at(j).noActive = true;
							triSonPartTemp2->at(sameTri->at(j).index).noActive = true;
							changeData->insertNoActive(sameTri->at(j));
						}
						else
						{
							if(sameTri->at(j).p1 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p1 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp2->at(sameTri->at(j).index).p1 = pointPartQem->at(i).index;
								triSonPartTemp2->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else if(sameTri->at(j).p2 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p2 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp2->at(sameTri->at(j).index).p2 = pointPartQem->at(i).index;
								triSonPartTemp2->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else if(sameTri->at(j).p3 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p3 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp2->at(sameTri->at(j).index).p3 = pointPartQem->at(i).index;
								triSonPartTemp2->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else
							{
								cout << "三角形边折叠记录存储时出错，index = " << index << endl;
							}

						}
					}
					if(changeData->noActiveTri->size() != 2)
					{
						delete sameTri;
						delete relatedP;
						delete[] changeData;
						continue;
					}
					changeSonPartData2->push_back(*changeData);
					for(unsigned int m = 0 ; m < relatedP->size() ; m++)
					{
						MyPoint newP;
						MyPoint newP1;
						if(relatedP->at(m) == pointPartQem->at(i).minqemindex)
						{
							newP.ifdelete = true;
							newP.index = relatedP->at(m);
							changeSonPartPLV2->push_back(newP);
							deleteSonPoint2->push_back(pointPartQem->at(i).minqemindex);
							continue;
						}

						VECTORMYTRI* rePtsameTri;
						rePtsameTri = getSonPartSamePointTri(triSonPartTemp2 , relatedP->at(m));
						newP.index = relatedP->at(m);
						newP.setX(pointPart2->at(relatedP->at(m)).getX());
						newP.setY(pointPart2->at(relatedP->at(m)).getY());
						newP.setZ(pointPart2->at(relatedP->at(m)).getZ());
						//可有可无，只是使该点的属性完整
						newP.minqem1 = pointPart2->at(relatedP->at(m)).minqem1;
						newP.minqemindex = -1;
						newP1 = setSonPtQemData(newP , pointPart , 2 , triSonPartTemp2);
						delete rePtsameTri;
						changeSonPartPLV2->push_back(newP1);

					}
					delete sameTri;
					delete relatedP;
					delete[] changeData;
					break;
				}
			case 3:
				{
					sameTri = getSonPartSamePointTri(triSonPartTemp3 , pointPartQem->at(i).minqemindex);
					relatedP = getSonPartRelatePoint(pointPartQem->at(i).minqemindex , 3);
					bool ifcon = true;
					try
					{
						ifcon = ifConvexPolygon(sameTri , pointPartQem->at(i).minqemindex , pointPartQem->at(i).index , pointPart3);
					}
					catch(exception& e)
					{
						cout<<"出错子块为"<<index<<" "<<e.what()<<endl;
						continue;
					}
					if(!ifcon)
					{
						delete sameTri;
						delete relatedP;
						delete changeData;
						continue;
						
					}
					
					for(unsigned int j = 0 ; j < sameTri->size() ; j++)
					{
						if(sameTri->at(j).p1 == pointPartQem->at(i).index || sameTri->at(j).p2 == pointPartQem->at(i).index || sameTri->at(j).p3 == pointPartQem->at(i).index)
						{
							sameTri->at(j).noActive = true;
							triSonPartTemp3->at(sameTri->at(j).index).noActive = true;
							changeData->insertNoActive(sameTri->at(j));
						}
						else
						{
							if(sameTri->at(j).p1 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p1 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp3->at(sameTri->at(j).index).p1 = pointPartQem->at(i).index;
								triSonPartTemp3->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else if(sameTri->at(j).p2 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p2 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp3->at(sameTri->at(j).index).p2 = pointPartQem->at(i).index;
								triSonPartTemp3->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else if(sameTri->at(j).p3 == pointPartQem->at(i).minqemindex)
							{
								changeData->insertOrigin(sameTri->at(j));
								sameTri->at(j).p3 = pointPartQem->at(i).index;
								sameTri->at(j).change = true;
								triSonPartTemp3->at(sameTri->at(j).index).p3 = pointPartQem->at(i).index;
								triSonPartTemp3->at(sameTri->at(j).index).change = true;
								changeData->insertChange(sameTri->at(j));
							}
							else
							{
								cout << "三角形边折叠记录存储时出错，index = " << index << endl;
							}

						}
					}
					if(changeData->noActiveTri->size() != 2)
					{
						delete sameTri;
						delete relatedP;
						delete[] changeData;
						continue;
					}
					changeSonPartData3->push_back(*changeData);
					for(unsigned int m = 0 ; m < relatedP->size() ; m++)
					{
						MyPoint newP;
						MyPoint newP1;
						if(relatedP->at(m) == pointPartQem->at(i).minqemindex)
						{
							newP.ifdelete = true;
							newP.index = relatedP->at(m);
							changeSonPartPLV3->push_back(newP);
							deleteSonPoint3->push_back(pointPartQem->at(i).minqemindex);
							continue;
						}

						VECTORMYTRI* rePtsameTri;
						rePtsameTri = getSonPartSamePointTri(triSonPartTemp3 , relatedP->at(m));
						newP.index = relatedP->at(m);
						newP.setX(pointPart3->at(relatedP->at(m)).getX());
						newP.setY(pointPart3->at(relatedP->at(m)).getY());
						newP.setZ(pointPart3->at(relatedP->at(m)).getZ());
						//可有可无，只是使该点的属性完整
						newP.minqem1 = pointPart3->at(relatedP->at(m)).minqem1;
						newP.minqemindex = -1;
						newP1 = setSonPtQemData(newP , pointPart , 3 , triSonPartTemp3);
						delete rePtsameTri;
						changeSonPartPLV3->push_back(newP1);

					}
					delete sameTri;
					delete relatedP;
					delete[] changeData;
					break;
				}
			}
			break;
		}
		
	}
	
}

/**********************
边折叠信息的存储
************************/
//void changeDataSonPartSave(VECTORMYPOINT* pointPart , VECTORMYPOINT* pointPartQem , int index)
//{
//	for(unsigned int i = 0 ; i < pointPartQem->size() ; i++)
//	{
//		if(pointPart->at(pointPartQem->at(i).minqemindex).ifCon)
//		{
//			continue;
//		}
//		else
//		{
//			VECTORMYTRI* sameTri;
//			VECTORINT* relatedP;
//			ChangeData* changeData = new ChangeData;
//			sameTri = getSonPartSamePointTri(triSonPartTemp0 , pointPartQem->at(i).minqemindex);
//
//			//bool ifdel = true;
//			relatedP = getSonPartRelatePoint(pointPartQem->at(i).minqemindex , 0);
//			bool ifCon = true;
//			try
//			{
//				switch(index)
//				{
//				case 0:
//					ifCon = ifConvexPolygon(sameTri , pointPartQem->at(i).minqemindex , pointPartQem->at(i).index , pointPart0);
//					break;
//				case 1:
//					ifCon = ifConvexPolygon(sameTri , pointPartQem->at(i).minqemindex , pointPartQem->at(i).index , pointPart1);
//					break;
//				case 2:
//					ifCon = ifConvexPolygon(sameTri , pointPartQem->at(i).minqemindex , pointPartQem->at(i).index , pointPart3);
//					break;
//				case 3:
//					ifCon = ifConvexPolygon(sameTri , pointPartQem->at(i).minqemindex , pointPartQem->at(i).index , pointPart3);
//					break;
//				default:
//					cout<<"边折叠时进行凸多边形判断出错，index = "<< index <<endl;
//					break;
//				}
//			}
//			catch(exception& e)
//			{
//				cout<<e.what()<<endl;
//			}
//			if(!ifCon)
//			{
//				continue;
//			}
//
//			for(unsigned int j = 0 ; j < sameTri->size() ; j++)
//			{
//				if(sameTri->at(j).p1 == pointPartQem->at(i).index || sameTri->at(j).p2 == pointPartQem->at(i).index || sameTri->at(j).p3 == pointPartQem->at(i).index)
//				{
//					sameTri->at(j).noActive = true;
//					triSonPartTemp0->at(sameTri->at(j).index).noActive = true;
//					changeData->insertNoActive(sameTri->at(j));
//				}
//				else
//				{
//					if(sameTri->at(j).p1 == pointPartQem->at(i).minqemindex)
//					{
//						changeData->insertOrigin(sameTri->at(j));
//						sameTri->at(j).p1 = pointPartQem->at(i).index;
//						sameTri->at(j).change = true;
//						triSonPartTemp0->at(sameTri->at(j).index).p1 = pointPartQem->at(i).index;
//						triSonPartTemp0->at(sameTri->at(j).index).change = true;
//						changeData->insertChange(sameTri->at(j));
//					}
//					else if(sameTri->at(j).p2 == pointPartQem->at(j).minqemindex)
//					{
//						changeData->insertOrigin(sameTri->at(j));
//						sameTri->at(j).p2 = pointPartQem->at(i).index;
//						sameTri->at(j).change = true;
//						triSonPartTemp0->at(sameTri->at(j).index).p2 = pointPartQem->at(i).index;
//						triSonPartTemp0->at(sameTri->at(j).index).change = true;
//						changeData->insertChange(sameTri->at(j));
//					}
//					else if(sameTri->at(j).p3 == pointPartQem->at(i).minqemindex)
//					{
//						changeData->insertOrigin(sameTri->at(j));
//						sameTri->at(j).p3 = pointPartQem->at(i).index;
//						sameTri->at(j).change = true;
//						triSonPartTemp0->at(sameTri->at(j).index).p3 = pointPartQem->at(i).index;
//					}
//					else
//					{
//						cout << "三角形边折叠记录存储时出错" << endl;
//					}
//
//				}
//			}
//
//			switch(index)
//			{
//			case 0:
//				{
//					changeSonPartData0->push_back(*changeData);
//					break;
//				}
//			case 1:
//				{
//					changeSonPartData1->push_back(*changeData);
//					break;
//				}
//			case 2:
//				{
//					changeSonPartData2->push_back(*changeData);
//					break;
//				}
//			case 3:
//				{
//					changeSonPartData3->push_back(*changeData);
//					break;
//				}
//			default:
//				cout<<"进行边折叠信息存入时出错，错误子块为"<<index<<endl;
//				break;
//			}
//			
//
//			for(unsigned int m = 0 ; m < relatedP->size() ; m++)
//			{
//				MyPoint newP;
//				MyPoint newP1;
//				if(relatedP->at(m) == pointPartQem->at(i).minqemindex)
//				{
//					newP.ifdelete = true;
//					newP.index = relatedP->at(m);
//					changeSonPartPLV0->push_back(newP);
//					deleteSonPoint0->push_back(pointPartQem->at(i).minqemindex);
//					continue;
//				}
//
//				VECTORMYTRI* rePtsameTri;
//				rePtsameTri = getSonPartSamePointTri(triSonPartTemp0 , relatedP->at(m));
//				newP.index = relatedP->at(m);
//				newP.setX(pointPart0->at(relatedP->at(m)).getX());
//				newP.setY(pointPart0->at(relatedP->at(m)).getY());
//				newP.setZ(pointPart0->at(relatedP->at(m)).getZ());
//				//可有可无，只是使该点的属性完整
//				newP.minqem1 = pointPart0->at(relatedP->at(m)).minqem1;
//				newP.minqemindex = -1;
//				switch(index)
//				{
//				case 0:
//					{
//						newP1 = setSonPtQemData(newP , pointPart , 0 , triSonPartTemp0);
//						delete rePtsameTri;
//						changeSonPartPLV0->push_back(newP1);
//						break;
//					}
//				case 1:
//					{
//						newP1 = setSonPtQemData(newP , pointPart , 1 , triSonPartTemp1);
//						delete rePtsameTri;
//						changeSonPartPLV1->push_back(newP1);
//						break;
//					}
//				case 2:
//					{
//						newP1 = setSonPtQemData(newP , pointPart , 2 , triSonPartTemp2);
//						delete rePtsameTri;
//						changeSonPartPLV2->push_back(newP1);
//						break;
//					}
//				case 3:
//					{
//						newP1 = setSonPtQemData(newP , pointPart , 3 , triSonPartTemp3);
//						delete rePtsameTri;
//						changeSonPartPLV3->push_back(newP1);
//						break;
//					}
//				default:
//					cout<<"计算"<<index<<"子块的newP二次误差时出错"<<endl;
//					break;
//				}
//				
//				
//
//
//			}
//			delete sameTri;
//			delete relatedP;
//			delete[] changeData;
//
//
//		}
//		break;
//	}
//}

void changDataSave(VECTORMYPOINT* rPoint ,VECTORMYPOINT* rPointS)//边折叠数据变化及记录的存储
{
	for(unsigned int i = 0 ; i < rPointS->size() ; i++)
	{
		//if(ifExistcos(rPointS->at(i).minqemindex , convexP1))//是否是边界上的点
		//{
		//	continue;
		//}

		if(rPoint->at(rPointS->at(i).minqemindex).ifCon)
		{
			continue;
		}
		else
		{
			
			VECTORMYTRI* sameTri;
			VECTORINT* relatedP;
			ChangeData* changeData = new ChangeData;
			//if(i == 122||i == 121||i ==123)
			//{
			//	cout<<"121"<<endl;
			//	//continue;
			//	
			//}
			/*if(rPointS->at(i).index == 744||rPointS->at(i).index == 185||rPointS->at(i).index == 190)
			{
				continue;
			}*/
			sameTri = getSamePointTri(rPointS->at(i).minqemindex);
			
			bool ifdel = true;
			/*for(unsigned int k = 0 ; k < sameTri->size() ; k++)
			{
				if(sameTri->at(k).p1 == rPointS->at(i).minqemindex)
				{
					if(sameTri->at(k).p2 == rPointS->at(i).index||sameTri->at(k).p3 == rPointS->at(i).index)
					{
						continue;
					}
					else
					{
						if(!ifdelRule(sameTri->at(k).p2 , sameTri->at(k).p3 , rPointS->at(i).index ,rPoint))
						{
							ifdel = false;
							break;
						}
						else
						{
							continue;
						}
					}

				}
				else if(sameTri->at(k).p2 == rPointS->at(i).minqemindex)
				{
					if(sameTri->at(k).p1 == rPointS->at(i).index || sameTri->at(k).p3 == rPointS->at(i).index)
					{
						continue;
					}
					else
					{
						if(!ifdelRule(sameTri->at(k).p1 , sameTri->at(k).p3 , rPointS->at(i).index ,rPoint))
						{
							ifdel = false;
							break;
						}
						else
						{
							continue;
						}
					}
				}
				else if(sameTri->at(k).p3 == rPointS->at(i).minqemindex)
				{
					if(sameTri->at(k).p1 == rPointS->at(i).index || sameTri->at(k).p2 == rPointS->at(i).index)
					{
						continue;
					}
					else
					{
						if(!ifdelRule(sameTri->at(k).p1 , sameTri->at(k).p2 , rPointS->at(i).index ,rPoint))
						{
							ifdel = false;
							break;
						}
						else
						{
							continue;
						}
					}
				}
				else
				{
					cout<<"出错了"<<endl;
					continue;
				}
			}*/
			/*if(rPointS->at(i).index == 1132)
			{
				cout<<"1132"<<endl;
			}*/
			relatedP = getRelatedPoint(rPointS->at(i).minqemindex);//获得与旧点相关联的点

			bool ifcon = true;
			try
			{
				ifcon = ifConvexPolygon(sameTri , rPointS->at(i).minqemindex , rPointS->at(i).index , rPoint);
			}
			catch(exception& e)
			{
				cout<<e.what()<<endl;
				continue;
			}
			if(!ifcon)
			{
				continue;
			}

			
			/*if(ifdel)
			{*/

				/*for(unsigned int k = 0 ; k < sameTri->size() ; k++)
				{
					c33hangeData->pushDelTri(sameTri->at(k));
				}*/

				for(unsigned int j = 0 ; j < sameTri->size() ; j++)
				{
					if(sameTri->at(j).p1 == rPointS->at(i).index||sameTri->at(j).p2 == rPointS->at(i).index||sameTri->at(j).p3 == rPointS->at(i).index)
					{
						sameTri->at(j).noActive = true;
						TriFile1->at(sameTri->at(j).index).noActive = true;
						//changeTriLV1->push_back(sameTri->at(j));
						changeData->insertNoActive(sameTri->at(j));
						
					}
					else
					{
						if(sameTri->at(j).p1 == rPointS->at(i).minqemindex)
						{
							changeData->insertOrigin(sameTri->at(j));
							sameTri->at(j).p1 = rPointS->at(i).index;
							sameTri->at(j).change = true;
							TriFile1->at(sameTri->at(j).index).p1 = rPointS->at(i).index;
							TriFile1->at(sameTri->at(j).index).change = true;
							//changeTriLV1->push_back(sameTri->at(j));
							changeData->insertChange(sameTri->at(j));
							//changeData->pushInsertTri(sameTri->at(j));
							
							
						}
						else if(sameTri->at(j).p2 == rPointS->at(i).minqemindex)
						{
							changeData->insertOrigin(sameTri->at(j));
							sameTri->at(j).p2 = rPointS->at(i).index;
							sameTri->at(j).change = true;
							TriFile1->at(sameTri->at(j).index).p2 = rPointS->at(i).index;
							TriFile1->at(sameTri->at(j).index).change = true;
							//changeTriLV1->push_back(sameTri->at(j));
							changeData->insertChange(sameTri->at(j));
							//changeData->pushInsertTri(sameTri->at(j));
						}
						else if(sameTri->at(j).p3 == rPointS->at(i).minqemindex)
						{
							changeData->insertOrigin(sameTri->at(j));
							sameTri->at(j).p3 = rPointS->at(i).index;
							sameTri->at(j).change = true;
							TriFile1->at(sameTri->at(j).index).p3 = rPointS->at(i).index;
							TriFile1->at(sameTri->at(j).index).change = true;
							//changeTriLV1->push_back(sameTri->at(j));
							changeData->insertChange(sameTri->at(j));
							//changeData->pushInsertTri(sameTri->at(j));
						}
						else
						{
							cout<<"新三角形存入出错"<<endl;
						}

					}
				}

				changeDataL->push_back(*changeData);

				for(unsigned int m = 0 ; m < relatedP->size() ; m++)
				{
					
						
						MyPoint newP;
						MyPoint newP1;
						if(relatedP->at(m) == rPointS->at(i).minqemindex)
						{
							newP.ifdelete = true;
							newP.index = relatedP->at(m);
							changePLV1->push_back(newP);
							deletePoint->push_back(rPointS->at(i).minqemindex);
							
							continue;
						}
						
						VECTORMYTRI* rePtsameTri;
						rePtsameTri = getSamePointTri(relatedP->at(m));
						newP.index = relatedP->at(m);
						newP.setX(rPoint->at(relatedP->at(m)).getX());
						newP.setY(rPoint->at(relatedP->at(m)).getY());
						newP.setZ(rPoint->at(relatedP->at(m)).getZ());
						newP.minqem1 = rPoint->at(relatedP->at(m)).minqem1;//可有可无
						double nX = 0;
						double nY = 0;
						double nZ = 0;
						/*unsigned int rePtsameTrisize = rePtsameTri->size();
						for(unsigned n = 0 ; n < rePtsameTrisize ; n++)
						{
							

							MyTri newTri;
							
							newTri = getTriNormal(rePtsameTri->at(n) , rPoint);
							nX += newTri.normal.getX();
							nY += newTri.normal.getY();
							nZ += newTri.normal.getZ();

						}
						

						newP.normal.X = nX/rePtsameTrisize;
						newP.normal.Y = nY/rePtsameTrisize;
						newP.normal.Z = nZ/rePtsameTrisize;*/
						newP.minqemindex = -1;
						newP1 = setPtQEMData(newP , rPoint);
						
						delete rePtsameTri;
						
						changePLV1->push_back(newP1);


				
				}
				
				
			/*}
			else
			{
				continue;
			}*/

			delete sameTri;
			delete relatedP;
			delete[] changeData;
		}

		break;
		
	}
	
}

osg::ref_ptr<osg::Group> createLight(osg::ref_ptr<osg::Node> node , VECTORMYPOINT* rPoint)
{
	osg::ref_ptr<osg::Group> lightRoot = new osg::Group();
	lightRoot->addChild(node);

	//开启光照
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	//statesetAll->setAttribute(stateset->getAttribute());
	stateset = lightRoot->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING , osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHT0 , osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHT1 , osg::StateAttribute::ON);
	//stateset->setMode(GL_LIGHT2 , osg::StateAttribute::ON);
	//stateset->setMode(GL_RESCALE_NORMAL,osg::StateAttribute::ON);
	//stateset->setMode(GL_NORMALIXE,osg::StateAttribute::ON);


	//计算包围盒
	/*osg::BoundingSphere bs;
	node->computeBound();
	bs = node->getBound();*/
	
	//创建一个Light对象
	osg::ref_ptr<osg::Light> light = new osg::Light();
	//osg::ref_ptr<osg::Light> light1 = new osg::Light();
	light->setLightNum(0);
	//light1->setLightNum(1);
	//设置方向
	//light->setDirection(osg::Vec3(0.0f , 0.0f , -1.0f));
	//设置位置
	light->setPosition(osg::Vec4(rPoint->at(minX).getX()-500.f , rPoint->at(minX).getY()-1200.f , rPoint->at(minX).getZ() + 700.f  , 1.0f));
	//light->setPosition(osg::Vec4(rPoint->at(minX).getX()-700.f , rPoint->at(minX).getY()-800.f , rPoint->at(minX).getZ() + 700.f  , 1.0f));
	//light->setPosition(osg::Vec4(rPoint->at(0).getX() , rPoint->at(0).getY() , rPoint->at(0).getZ() + 1000.f  , 1.0f));
	//light1->setPosition(osg::Vec4(rPoint->at(maxX).getX()+200.f , rPoint->at(maxX).getY() , rPoint->at(maxX).getZ() + 500.f , 1.0f));

	
	//设置光源颜色
	light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//light1->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//设置散射光的颜色
	light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//light1->setDiffuse(osg::Vec4(0.3f, 0.3f, 0.35f, 1.0f));

	light->setSpecular(osg::Vec4d( 1.0f, 1.0f, 1.0f, 1.0f ));
	//light1->setSpecular(osg::Vec4d( 0.3f, 0.3f, 0.3f, 1.0f ));

	


	//设置恒衰减指数
	light->setConstantAttenuation(1.f);
	//light1->setConstantAttenuation(1.f);
	//设置线性衰减指数
	light->setLinearAttenuation(0.00008f);
	//light1->setLinearAttenuation(0.0002f);
	//light->setLinearAttenuation(0.f);
	//设置二次方衰减指数
	light->setQuadraticAttenuation(0.00008f);
	//light1->setQuadraticAttenuation(0.0006f);
	//stateset->setAttribute(light,osg::StateAttribute::OFF);

	//light->setQuadraticAttenuation(0.f);

	//创建光源 
	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
	//osg::ref_ptr<osg::LightSource> lightSource1 = new osg::LightSource();
	lightSource->setLight(light.get());
	//lightSource1->setLight(light1.get());
	lightRoot->addChild(lightSource.get());
	//lightRoot->addChild(lightSource1.get());

	return lightRoot.get();

}

osg::ref_ptr<osg::Geode> CreateLiSphere(osg::Vec3 m_center,float m_radius)  
{  
    //光源物体  
    osg::ref_ptr<osg::Geode>geode=new osg::Geode;    
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);    
    osg::ref_ptr<osg::ShapeDrawable>sd=new osg::ShapeDrawable(new osg::Sphere(m_center,m_radius));    
    sd->setColor(osg::Vec4(1.f,0.f,0.f,1));    
    geode->addDrawable(sd);    
    return geode.get();  
}  

osg::ref_ptr<osgShadow::ShadowedScene> CreateShadowScene(osg::ref_ptr<osg::Node> m_scenemodel, VECTORMYPOINT* rPoint)  
{  
    //创建一个阴影节点，并标识接收对象和投影对象    
    osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = new osgShadow::ShadowedScene();    
    shadowedScene->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);    
    shadowedScene->setCastsShadowTraversalMask(CastsShadowTraversalMask);  
	
	osg::ref_ptr<osgShadow::SoftShadowMap > ssm = new osgShadow::SoftShadowMap;
	ssm->setAmbientBias(osg::Vec2(0.3f,0.3f));//0.5,0.5
	float bias = ssm->getBias();
	ssm->setBias(bias*1.0f);
  
  
    //创建阴影纹理，使用的是shadowTexture技法    
    osg::ref_ptr<osgShadow::ShadowTexture> st = new osgShadow::ShadowTexture;    
    osg::ref_ptr<osgShadow::ParallelSplitShadowMap> pss = new osgShadow::ParallelSplitShadowMap;    
    osg::ref_ptr<osgShadow::ShadowVolume> sv = new osgShadow::ShadowVolume;    
    osg::ref_ptr<osgShadow::MinimalShadowMap> ms = new osgShadow::MinimalShadowMap;    
    osg::ref_ptr<osgShadow::StandardShadowMap> ss = new osgShadow::StandardShadowMap;    
    osg::ref_ptr<osgShadow::SoftShadowMap> softS = new osgShadow::SoftShadowMap;    
    osg::ref_ptr<osgShadow::ViewDependentShadowTechnique> vds = new osgShadow::ViewDependentShadowTechnique;    
    //关联阴影纹理    
	shadowedScene->setShadowTechnique(ssm);
	//shadowedScene->setShadowTechnique(st)
    //shadowedScene->setShadowTechnique(softS);   
	/*shadowedScene->setShadowTechnique(pss);
	shadowedScene->setShadowTechnique(ms);
	shadowedScene->setShadowTechnique(ss);
	shadowedScene->setShadowTechnique(vds);
  */
    //添加场景数据并添加光源   
    shadowedScene->addChild(createLight(m_scenemodel.get() , rPoint));    
    //shadowedScene->addChild(m_scenemodel.get());   
  
    return shadowedScene;  
}  

VECTORMYTRI* copyTriFile(VECTORMYTRI* TriFileO)
{
	VECTORMYTRI* TriFileLV1 = new VECTORMYTRI;
	for(unsigned int i = 0 ; i < TriFile->size() ; i++)
	{
		TriFileLV1->push_back(TriFileO->at(i));
	}
	return TriFileLV1;
}

void setupProperties(osgText::Text& textObject,osgText::Font* font,float size,const osg::Vec3& pos )
{
    textObject.setFont(font);// 
 
    textObject.setCharacterSize(size);//字体大小
    textObject.setPosition(pos);
    textObject.setColor(osg::Vec4(0.0,1.0,0.0,1.0));
    textObject.setAlignment(osgText::Text::CENTER_BOTTOM);//文字显示方向
    //textObject.setAxisAlignment(osgText::Text::SCREEN);//获取文字对称成方式正对屏幕方向
    //textObject.setCharacterSizeMode(osgText::Text::SCREEN_COORDS);//跟随视角不断变化，离物体越远，文字越大
    textObject.setAutoRotateToScreen(false);//跟随视角不断变化，但离物体越远，文字越小，和现实当中像类似
    textObject.setBackdropType(osgText::Text::OUTLINE);//对文字进行描边
    textObject.setBackdropColor(osg::Vec4(1.0,1.0,0.0,1.0));//描边颜色
    textObject.setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);//添加文字边框       textObject.setAxisAlignment(osgText::Text::XZ_PLANE);//获取文字对称成方式
 
}

void createContent(osgText::Text& textObject,const char* string)
{
    int requiredSize=mbstowcs(NULL,string,0);//如果mbstowcs第一参数为NULL那么返回字符串的数目
    wchar_t* wText=new wchar_t[requiredSize+1];
    mbstowcs(wText,string,requiredSize+1);//由char转换成wchar类型
    textObject.setText(wText);
    delete wText;
}

/*****************
将所有子块的边折叠信息进行合并，顺序是每一次边折叠过程把四个子块的变化三角形以此排序
********************/
void dataAllIndexSonPartChange()
{
	ChangeTriIndex* change;
	unsigned int changeSonPartDataSize0 = changeSonPartData0->size();
	/*unsigned int changeSonPartDataSize1 = changeSonPartData1->size();
	unsigned int changeSonPartDataSize2 = changeSonPartData2->size();
	unsigned int changeSonPartDataSize3 = changeSonPartData3->size();
	unsigned int AllSize = changeSonPartDataSize0 + changeSonPartDataSize1 + changeSonPartDataSize2 + changeSonPartDataSize3;*/


	
	for(unsigned int i = 0 ; i < changeSonPartDataSize0 ; i++)
	{
		/***************
		子块1的第一次边折叠信息，以下以此类推
		****************/
		for(unsigned int j = 0 ; j < changeSonPartData0->at(i).changeTri->size() ; j++)
		{
			change = new ChangeTriIndex(changeSonPartData0->at(i).changeTri->at(j).index , false ,changeSonPartData0->at(i).changeTri->at(j).allIndex);
			changeSonPartTriIndexAll->push_back(*change);
		}
		change = new ChangeTriIndex(changeSonPartData0->at(i).noActiveTri->at(0).index , true , changeSonPartData0->at(i).noActiveTri->at(0).allIndex);
		changeSonPartTriIndexAll->push_back(*change);
		change = new ChangeTriIndex(changeSonPartData0->at(i).noActiveTri->at(1).index , true , changeSonPartData0->at(i).noActiveTri->at(1).allIndex);
		changeSonPartTriIndexAll->push_back(*change);

		for(unsigned int j = 0 ; j < changeSonPartData1->at(i).changeTri->size() ; j++)
		{
			change = new ChangeTriIndex(changeSonPartData1->at(i).changeTri->at(j).index , false , changeSonPartData1->at(i).changeTri->at(j).allIndex);
			changeSonPartTriIndexAll->push_back(*change);
		}
		change = new ChangeTriIndex(changeSonPartData1->at(i).noActiveTri->at(0).index , true , changeSonPartData1->at(i).noActiveTri->at(0).allIndex);
		changeSonPartTriIndexAll->push_back(*change);
		change = new ChangeTriIndex(changeSonPartData1->at(i).noActiveTri->at(1).index , true , changeSonPartData1->at(i).noActiveTri->at(1).allIndex);
		changeSonPartTriIndexAll->push_back(*change);

		for(unsigned int j = 0 ; j < changeSonPartData2->at(i).changeTri->size() ; j++)
		{
			change = new ChangeTriIndex(changeSonPartData2->at(i).changeTri->at(j).index , false , changeSonPartData2->at(i).changeTri->at(j).allIndex);
			changeSonPartTriIndexAll->push_back(*change);
		}
		change = new ChangeTriIndex(changeSonPartData2->at(i).noActiveTri->at(0).index , true , changeSonPartData2->at(i).noActiveTri->at(0).allIndex);
		changeSonPartTriIndexAll->push_back(*change);
		change = new ChangeTriIndex(changeSonPartData2->at(i).noActiveTri->at(1).index , true , changeSonPartData2->at(i).noActiveTri->at(1).allIndex);
		changeSonPartTriIndexAll->push_back(*change);

		for(unsigned int j = 0 ; j < changeSonPartData3->at(i).changeTri->size() ; j++)
		{
			change = new ChangeTriIndex(changeSonPartData3->at(i).changeTri->at(j).index , false , changeSonPartData3->at(i).changeTri->at(j).allIndex);
			changeSonPartTriIndexAll->push_back(*change);
		}
		change = new ChangeTriIndex(changeSonPartData3->at(i).noActiveTri->at(0).index , true , changeSonPartData3->at(i).noActiveTri->at(0).allIndex);
		changeSonPartTriIndexAll->push_back(*change);
		change = new ChangeTriIndex(changeSonPartData3->at(i).noActiveTri->at(1).index , true , changeSonPartData3->at(i).noActiveTri->at(1).allIndex);
		changeSonPartTriIndexAll->push_back(*change);
	}

	for(unsigned int k = 0 ; k < changeSonPartTriIndexAll->size() ; k++)
	{
		int n = 0;
		for(unsigned int m = 0 ; m < k ; m++)
		{
			if(changeSonPartTriIndexAll->at(m).ifdelete)
			{
				if(changeSonPartTriIndexAll->at(k).allIndex > changeSonPartTriIndexAll->at(m).allIndex)
				{
					n++;
				}
			}
		}
		changeSonPartTriIndexAll->at(k).changeIndex(changeSonPartTriIndexAll->at(k).allIndex - n);
	}
}

//void dataIndexSonPartChange(int index)
//{
//	switch(index)
//	{
//	case 0:
//		{
//			ChangeTriIndex* change;//用来存储变化的三角形信息，显存若干变化的，再存两个删除的，所有三角形信息都是连续的
//	
//			for(unsigned int i = 0 ; i < changeSonPartData0->size() ; i++)
//			{
//				for(unsigned int j = 0 ; j < changeSonPartData0->at(i).changeTri->size() ; j++)
//				{
//					change = new ChangeTriIndex(changeSonPartData0->at(i).changeTri->at(j).index , false);
//					changeSonPartTriIndex0->push_back(*change);
//				}
//
//				change = new ChangeTriIndex(changeSonPartData0->at(i).noActiveTri->at(0).index , true);
//				changeSonPartTriIndex0->push_back(*change);
//
//				change = new ChangeTriIndex(changeSonPartData0->at(i).noActiveTri->at(1).index , true);
//				changeSonPartTriIndex0->push_back(*change);
//				delete[] change;
//			}
//
//			for(unsigned int k = 0 ; k < changeSonPartTriIndex0->size() ; k++)
//			{
//				int n = 0;
//				for(unsigned int m = 0 ; m < k ; m++)
//				{
//					if(changeSonPartTriIndex0->at(m).ifdelete)
//					{
//						if(changeSonPartTriIndex0->at(k).index > changeSonPartTriIndex0->at(m).index)
//						{
//							n++;
//						}
//					}
//				}
//				changeSonPartTriIndex0->at(k).changeIndex(changeSonPartTriIndex0->at(k).index - n);
//			}
//			break;
//		}
//
//		case 1:
//		{
//			ChangeTriIndex* change;//用来存储变化的三角形信息，显存若干变化的，再存两个删除的，所有三角形信息都是连续的
//	
//			for(unsigned int i = 0 ; i < changeSonPartData1->size() ; i++)
//			{
//				for(unsigned int j = 0 ; j < changeSonPartData1->at(i).changeTri->size() ; j++)
//				{
//					change = new ChangeTriIndex(changeSonPartData1->at(i).changeTri->at(j).index , false);
//					changeSonPartTriIndex1->push_back(*change);
//				}
//
//				change = new ChangeTriIndex(changeSonPartData1->at(i).noActiveTri->at(0).index , true);
//				changeSonPartTriIndex1->push_back(*change);
//
//				change = new ChangeTriIndex(changeSonPartData1->at(i).noActiveTri->at(1).index , true);
//				changeSonPartTriIndex1->push_back(*change);
//				delete[] change;
//			}
//
//			for(unsigned int k = 0 ; k < changeSonPartTriIndex1->size() ; k++)
//			{
//				int n = 0;
//				for(unsigned int m = 0 ; m < k ; m++)
//				{
//					if(changeSonPartTriIndex1->at(m).ifdelete)
//					{
//						if(changeSonPartTriIndex1->at(k).index > changeSonPartTriIndex1->at(m).index)
//						{
//							n++;
//						}
//					}
//				}
//				changeSonPartTriIndex1->at(k).changeIndex(changeSonPartTriIndex1->at(k).index - n);
//			}
//			break;
//		}
//
//		case 2:
//		{
//			ChangeTriIndex* change;//用来存储变化的三角形信息，显存若干变化的，再存两个删除的，所有三角形信息都是连续的
//	
//			for(unsigned int i = 0 ; i < changeSonPartData2->size() ; i++)
//			{
//				for(unsigned int j = 0 ; j < changeSonPartData2->at(i).changeTri->size() ; j++)
//				{
//					change = new ChangeTriIndex(changeSonPartData2->at(i).changeTri->at(j).index , false);
//					changeSonPartTriIndex2->push_back(*change);
//				}
//
//				change = new ChangeTriIndex(changeSonPartData2->at(i).noActiveTri->at(0).index , true);
//				changeSonPartTriIndex2->push_back(*change);
//
//				change = new ChangeTriIndex(changeSonPartData2->at(i).noActiveTri->at(1).index , true);
//				changeSonPartTriIndex2->push_back(*change);
//				delete[] change;
//			}
//
//			for(unsigned int k = 0 ; k < changeSonPartTriIndex2->size() ; k++)
//			{
//				int n = 0;
//				for(unsigned int m = 0 ; m < k ; m++)
//				{
//					if(changeSonPartTriIndex2->at(m).ifdelete)
//					{
//						if(changeSonPartTriIndex2->at(k).index > changeSonPartTriIndex2->at(m).index)
//						{
//							n++;
//						}
//					}
//				}
//				changeSonPartTriIndex2->at(k).changeIndex(changeSonPartTriIndex2->at(k).index - n);
//			}
//			break;
//		}
//
//		case 3:
//		{
//			ChangeTriIndex* change;//用来存储变化的三角形信息，显存若干变化的，再存两个删除的，所有三角形信息都是连续的
//	
//			for(unsigned int i = 0 ; i < changeSonPartData3->size() ; i++)
//			{
//				for(unsigned int j = 0 ; j < changeSonPartData3->at(i).changeTri->size() ; j++)
//				{
//					change = new ChangeTriIndex(changeSonPartData3->at(i).changeTri->at(j).index , false);
//					changeSonPartTriIndex3->push_back(*change);
//				}
//
//				change = new ChangeTriIndex(changeSonPartData3->at(i).noActiveTri->at(0).index , true);
//				changeSonPartTriIndex3->push_back(*change);
//
//				change = new ChangeTriIndex(changeSonPartData3->at(i).noActiveTri->at(1).index , true);
//				changeSonPartTriIndex3->push_back(*change);
//				delete[] change;
//			}
//
//			for(unsigned int k = 0 ; k < changeSonPartTriIndex3->size() ; k++)
//			{
//				int n = 0;
//				for(unsigned int m = 0 ; m < k ; m++)
//				{
//					if(changeSonPartTriIndex3->at(m).ifdelete)
//					{
//						if(changeSonPartTriIndex3->at(k).index > changeSonPartTriIndex3->at(m).index)
//						{
//							n++;
//						}
//					}
//				}
//				changeSonPartTriIndex3->at(k).changeIndex(changeSonPartTriIndex3->at(k).index - n);
//			}
//			break;
//		}
//
//		default:
//			cout<<"进行边折叠三角形重新排序时出错，错误子块为"<<index<<endl;
//			break;
//	}
//
//
//}

void dataIndexChange()//进行相关三角形的重新排序
{
	//VECTORINT* deleteAllTri = new VECTORINT;//所有删除的三角形编号
	//VECTORINT* changeAllTri = new VECTORINT;//所有变化的三角形编号
	//VECTORINT* dcAllTri = new VECTORINT;//所有相关三角形的编号
	//VECTORCHANGETRII* changeTriIndex;//所有三角形边折叠后新的编号
	ChangeTriIndex* change;
	//ChangeNewData* newData;

	
	
	for(unsigned int i = 0 ; i < changeDataL->size() ; i++)
	{
		for(unsigned int j = 0 ; j < changeDataL->at(i).changeTri->size() ; j++)
		{
			/*changeAllTri->push_back(changeDataL->at(i).changeTri->at(j).index);
			dcAllTri->push_back(changeDataL->at(i).changeTri->at(j).index);*/
			//ChangeTriIndex* change = new ChangeTriIndex(changeDataL->at(i).changeTri->at(j).index , false) ;
			change = new ChangeTriIndex(changeDataL->at(i).changeTri->at(j).index , false , changeDataL->at(i).noActiveTri->at(j).allIndex);
			//changeTriIndex->push_back(new ChangeTriIndex(changeDataL->at(i).changeTri->at(j).index , false));
			//changeTriIndex = new VECTORCHANGETRII;
			changeTriIndex->push_back(*change);

			
		}


		/*deleteAllTri->push_back(changeDataL->at(i).noActiveTri->at(0).index);
		deleteAllTri->push_back(changeDataL->at(i).noActiveTri->at(1).index);
		dcAllTri->push_back(changeDataL->at(i).noActiveTri->at(0).index);
		dcAllTri->push_back(changeDataL->at(i).noActiveTri->at(1).index);*/
		change = new ChangeTriIndex(changeDataL->at(i).noActiveTri->at(0).index , true , changeDataL->at(i).noActiveTri->at(0).allIndex);
		//changeTriIndex->push_back(new ChangeTriIndex(changeDataL->at(i).noActiveTri->at(0).index , true));
		changeTriIndex->push_back(*change);
		change = new ChangeTriIndex(changeDataL->at(i).noActiveTri->at(1).index , true , changeDataL->at(i).noActiveTri->at(1).allIndex);
		//changeTriIndex->push_back(new ChangeTriIndex(changeDataL->at(i).noActiveTri->at(1).index , true));
		changeTriIndex->push_back(*change);
		/*newData->copy(changeTriIndex);
		changeNewData->push_back(*newData);
		delete change;
		delete changeTriIndex;*/
		delete[] change;
	}

	for(unsigned int k = 0 ; k < changeTriIndex->size() ; k++)
	{
		int n = 0;
		for(unsigned int m = 0 ; m < k ; m++)
		{
			
			if(changeTriIndex->at(m).ifdelete)
			{
				if(changeTriIndex->at(k).index > changeTriIndex->at(m).index)
				{
					n++;
				}
			}                                                                                          

		}
		//changeTriIndex->at(k).newIndex(changeTriIndex->at(k).index - n);
		changeTriIndex->at(k).changeIndex(changeTriIndex->at(k).index - n);
	}
}

/***************
对边折叠信息进行重新整理
***************/
void newAllSonPartData()
{
	ChangeNewData* changeNewD;
	int n = 0;
	//unsigned int changeSonPartDataAllSize = changeSonPartData0->size() + changeSonPartData1->size() + changeSonPartData2->size() + changeSonPartData3->size();
	for(unsigned int i = 0 ; i < changeSonPartData0->size() ; i++)
	{
		changeNewD = new ChangeNewData;
		for(unsigned int j = 0 ; j < changeSonPartData0->at(i).changeTri->size() ; j++)
		{
			changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
			n++;
		}
		changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
		n++;
		changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
		n++;
		changeNewDataAll->push_back(*changeNewD);
		delete[] changeNewD;

		changeNewD = new ChangeNewData;
		for(unsigned int j = 0 ; j < changeSonPartData1->at(i).changeTri->size() ; j++)
		{
			changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
			n++;
		}
		changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
		n++;
		changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
		n++;
		changeNewDataAll->push_back(*changeNewD);
		delete[] changeNewD;

		changeNewD = new ChangeNewData;
		for(unsigned int j = 0 ; j < changeSonPartData2->at(i).changeTri->size() ; j++)
		{
			changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
			n++;
		}
		changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
		n++;
		changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
		n++;
		changeNewDataAll->push_back(*changeNewD);
		delete[] changeNewD;

		changeNewD = new ChangeNewData;
		for(unsigned int j = 0 ; j <changeSonPartData3->at(i).changeTri->size() ; j++)
		{
			changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
			n++;
		}
		changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
		n++;
		changeNewD->insertChangeTrii(changeSonPartTriIndexAll->at(n));
		n++;
		changeNewDataAll->push_back(*changeNewD);
		delete[] changeNewD;
	}
}

//void newSonPartData(int index)
//{
//	switch(index)
//	{
//	case 0:
//		{
//			ChangeNewData* changeNewD;
//			int n = 0;
//			for(unsigned int i = 0 ; i < changeSonPartData0->size() ; i++)
//			{
//				changeNewD->insertChangeTrii(changeSonPartTriIndex0->at(n));
//				n++;
//			}
//			changeNewD->insertChangeTrii(changeSonPartTriIndex0->at(n));
//			n++;
//			changeNewD->insertChangeTrii(changeSonPartTriIndex0->at(n));
//			n++;
//			changeSonPartNewData0->push_back(*changeNewD);
//			delete[] changeNewD;
//			break;
//		}
//
//	case 1:
//		{
//			ChangeNewData* changeNewD;
//			int n = 0;
//			for(unsigned int i = 0 ; i < changeSonPartData1->size() ; i++)
//			{
//				changeNewD->insertChangeTrii(changeSonPartTriIndex1->at(n));
//				n++;
//			}
//			changeNewD->insertChangeTrii(changeSonPartTriIndex1->at(n));
//			n++;
//			changeNewD->insertChangeTrii(changeSonPartTriIndex1->at(n));
//			n++;
//			changeSonPartNewData1->push_back(*changeNewD);
//			delete[] changeNewD;
//			break;
//		}
//
//	case 2:
//		{
//			ChangeNewData* changeNewD;
//			int n = 0;
//			for(unsigned int i = 0 ; i < changeSonPartData2->size() ; i++)
//			{
//				changeNewD->insertChangeTrii(changeSonPartTriIndex2->at(n));
//				n++;
//			}
//			changeNewD->insertChangeTrii(changeSonPartTriIndex2->at(n));
//			n++;
//			changeNewD->insertChangeTrii(changeSonPartTriIndex2->at(n));
//			n++;
//			changeSonPartNewData2->push_back(*changeNewD);
//			delete[] changeNewD;
//			break;
//		}
//
//	case 3:
//		{
//			ChangeNewData* changeNewD;
//			int n = 0;
//			for(unsigned int i = 0 ; i < changeSonPartData3->size() ; i++)
//			{
//				changeNewD->insertChangeTrii(changeSonPartTriIndex3->at(n));
//				n++;
//			}
//			changeNewD->insertChangeTrii(changeSonPartTriIndex3->at(n));
//			n++;
//			changeNewD->insertChangeTrii(changeSonPartTriIndex3->at(n));
//			n++;
//			changeSonPartNewData3->push_back(*changeNewD);
//			delete[] changeNewD;
//			break;
//		}
//	default:
//		cout<<"进行边折叠信息的重新分配时出错，错误子块为"<<index<<endl;
//		break;
//	}
//}

void newData()//对重新排序后的三角形索引加工，与changeDataL格式相对应
{
	ChangeNewData* changeNewD;
	int n = 0;
	for(unsigned int i = 0 ; i < changeDataL->size() ; i++)
	{
		changeNewD = new ChangeNewData;
		for(unsigned int j = 0 ; j < changeDataL->at(i).changeTri->size() ; j++)
		{
			changeNewD->insertChangeTrii(changeTriIndex->at(n));
			n++;
		}
		changeNewD->insertChangeTrii(changeTriIndex->at(n));
		n++;
		changeNewD->insertChangeTrii(changeTriIndex->at(n));
		n++;
		changeNewData->push_back(*changeNewD);
		delete[] changeNewD;
	}
}

MyPoint getCentrePoint(VECTORMYPOINT* rPoint)
{
	unsigned int convexSize = convexP1->size();
	MyPoint centerP;
	double x = 0;
	double y = 0;
	double z = 0;
	for(unsigned int i = 0 ; i < convexSize ; i++)
	{
		x += rPoint->at(convexP1->at(i)).getX();
		y += rPoint->at(convexP1->at(i)).getY();
		z += rPoint->at(convexP1->at(i)).getZ();
	}
	centerP.setX(x/convexSize);
	centerP.setY(y/convexSize);
	centerP.setZ(z/convexSize);

	return centerP;

}

Color hsvToRgb(float h, float s, float v)
{
            double h60 = h / 60.0;
            double h60f = floor(h60);
            int hi = (int)(h60f) % 6;
            //double f = h60 - h60f;
            double f = h60 - hi;

            double p = v * (1 - s);
            double q = v * (1 - f * s);
            double t = v * (1 - (1 - f) * s);

            double r = 0, g = 0, b = 0;
            if (hi == 0)
            {
                r = v;
                g = t;
                b = p;
            }
            else if (hi == 1)
            {
                r = q;
                g = v;
                b = p;
            }
            else if (hi == 2)
            {
                r = p;
                g = v;
                b = t;
            }
            else if (hi == 3)
            {
                r = p;
                g = q;
                b = v;
            }
            else if (hi == 4)
            {
                r = t;
                g = p;
                b = v;

            }
            else if (hi == 5)
            {
                r = v;
                g = p;
                b = q;

            }

            //Color RGB1 = new int[3] { (int)(r * 255), (int)(g * 255), (int)(b * 255) };
			Color TRGB((float)(r * 255) , (float)(g * 255) , (float)(b * 255));
			
			
            return TRGB;
}

Color getCellColor(float min, float max, float value )
{
            float maxVale = max;//平均值加上3倍的标准差
            float minValue = min;//平均值减去3倍的标准差
            float valueMean = (max + min) / 2;
            float interval = (max - min) / 2;
            //采用HLS颜色变换空间，在变化时L默认是1，S默认是0.5，只有H变化,H的变化范围从0到240,3倍标准差内H从10到230，小于minValue时H=0；大于MaxValue,H=240
            float S = 1;
            float V = 1.f;
            float H = 0;
            //float aveH = 120;
            //float haltH = 110;
            float aveH = valueMean;
            float haltH = interval;
            if (value < minValue)
            {
                // H = 0;
                H = 0;
            }
            else if (value > maxVale)
            {
                H = 240;
                // H = 240;
            }
            else
            {
                //非线性变化
                if (value > valueMean)
                {

                    //double factor = (value - valueMean) / (3 * deviation);
                    //H = aveH + (float)Math.Sqrt(factor) * haltH;

                    double factor1 = (value - valueMean) / interval;
                    H = aveH + (float)sqrt(factor1) * haltH;

                }
                else
                {
                    double factor = (valueMean - value) / interval;
                    // H = aveH - (float)Math.Sqrt(factor) * haltH;
                    H = aveH - (float)sqrt(factor) * haltH;

                }
            }
            //将HSV色彩空间转换成RGB
            Color color = hsvToRgb(H, S, V);
            //int[] color = hsvToRgb(10, 1, 1);
            //Color valueColor = Color.FromArgb(color[0], color[1], color[2]);
            return color;
}

double getMinZ(VECTORMYPOINT* rPoint)
{
	double minZ = rPoint->at(0).getZ();
	for(unsigned int i = 0 ; i < rPoint->size() ; i++)
	{
		if(rPoint->at(i).getZ() < minZ)
		{
			minZ = rPoint->at(i).getZ();
		}
	}
	return minZ;
}

double getMaxZ(VECTORMYPOINT* rPoint)
{
	double maxZ = rPoint->at(0).getZ();
	for(unsigned int i = 0 ; i < rPoint->size() ; i++)
	{
		if(rPoint->at(i).getZ() > maxZ)
		{
			maxZ = rPoint->at(i).getZ();
		}
	}
	return maxZ;
}

/***************************
保存三角形，边和边界点数据
************************/

void saveSonPartTriAndLine(int index)
{
	ofstream outfile;
	ofstream outfileL;
	ofstream outfileC;

	switch(index)
	{
	case 0:
		{
			outfile.open("E:\\Learn\\Tin11-6-9-10-20\\sonPartTri0.txt");
			outfileL.open("E:\\Learn\\Tin11-6-9-10-20\\sonPartLine0.txt");
			outfileC.open("E:\\Learn\\Tin11-6-9-10-20\\convex0.txt");

			unsigned int triSonPartSize0 = triSonPart0->size();
			unsigned int lineSonPartSize0 = lineSonPart0->size();
			unsigned int convexSonPartSize0 = convexSonPart0->size();

			for(unsigned int i = 0 ; i < triSonPartSize0 ; i++)
			{
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart0->at(i).p1<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart0->at(i).p2<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart0->at(i).p3<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart0->at(i).Line1<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart0->at(i).Line2<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart0->at(i).Line3<<endl;

			}

			for(unsigned int j = 0 ; j < lineSonPartSize0 ; j++)
			{
				outfileL<<setiosflags(ios::fixed)<<setprecision(8)<<lineSonPart0->at(j).LeftPoint<<" ";
				outfileL<<setiosflags(ios::fixed)<<setprecision(8)<<lineSonPart0->at(j).RightPoint<<endl;
			}

			for(unsigned int k = 0 ; k < convexSonPartSize0 ; k++)
			{
				outfileC<<setiosflags(ios::fixed)<<setprecision(8)<<convexSonPart0->at(k)<<endl;
			}

			outfile.close();
			outfileL.close();
			outfileC.close();
			break;
		}
	case 1:
		{
			outfile.open("E:\\Learn\\Tin11-6-9-10-20\\sonPartTri1.txt");
			outfileL.open("E:\\Learn\\Tin11-6-9-10-20\\sonPartLine1.txt");
			outfileC.open("E:\\Learn\\Tin11-6-9-10-20\\convex1.txt");

			unsigned int triSonPartSize1 = triSonPart1->size();
			unsigned int lineSonPartSize1 = lineSonPart1->size();
			unsigned int convexSonPartSize1 = convexSonPart1->size();

			for(unsigned int i = 0 ; i < triSonPartSize1 ; i++)
			{
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart1->at(i).p1<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart1->at(i).p2<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart1->at(i).p3<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart1->at(i).Line1<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart1->at(i).Line2<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart1->at(i).Line3<<endl;

			}

			for(unsigned int j = 0 ; j < lineSonPartSize1 ; j++)
			{
				outfileL<<setiosflags(ios::fixed)<<setprecision(8)<<lineSonPart1->at(j).LeftPoint<<" ";
				outfileL<<setiosflags(ios::fixed)<<setprecision(8)<<lineSonPart1->at(j).RightPoint<<endl;
			}

			for(unsigned int k = 0 ; k < convexSonPartSize1 ; k++)
			{
				outfileC<<setiosflags(ios::fixed)<<setprecision(8)<<convexSonPart1->at(k)<<endl;
			}

			outfile.close();
			outfileL.close();
			outfileC.close();
			break;
		}
	case 2:
		{
			outfile.open("E:\\Learn\\Tin11-6-9-10-20\\sonPartTri2.txt");
			outfileL.open("E:\\Learn\\Tin11-6-9-10-20\\sonPartLine2.txt");
			outfileC.open("E:\\Learn\\Tin11-6-9-10-20\\convex2.txt");

			unsigned int triSonPartSize2 = triSonPart2->size();
			unsigned int lineSonPartSize2 = lineSonPart2->size();
			unsigned int convexSonPartSize2 = convexSonPart2->size();

			for(unsigned int i = 0 ; i < triSonPartSize2 ; i++)
			{
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart2->at(i).p1<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart2->at(i).p2<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart2->at(i).p3<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart2->at(i).Line1<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart2->at(i).Line2<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart2->at(i).Line3<<endl;

			}

			for(unsigned int j = 0 ; j < lineSonPartSize2 ; j++)
			{
				outfileL<<setiosflags(ios::fixed)<<setprecision(8)<<lineSonPart2->at(j).LeftPoint<<" ";
				outfileL<<setiosflags(ios::fixed)<<setprecision(8)<<lineSonPart2->at(j).RightPoint<<endl;
			}

			for(unsigned int k = 0 ; k < convexSonPartSize2 ; k++)
			{
				outfileC<<setiosflags(ios::fixed)<<setprecision(8)<<convexSonPart2->at(k)<<endl;
			}

			outfile.close();
			outfileL.close();
			outfileC.close();
			break;
		}
	case 3:
		{
			outfile.open("E:\\Learn\\Tin11-6-9-10-20\\sonPartTri3.txt");
			outfileL.open("E:\\Learn\\Tin11-6-9-10-20\\sonPartLine3.txt");
			outfileC.open("E:\\Learn\\Tin11-6-9-10-20\\convex3.txt");

			unsigned int triSonPartSize3 = triSonPart3->size();
			unsigned int lineSonPartSize3 = lineSonPart3->size();
			unsigned int convexSonPartSize3 = convexSonPart3->size();

			for(unsigned int i = 0 ; i < triSonPartSize3 ; i++)
			{
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart3->at(i).p1<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart3->at(i).p2<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart3->at(i).p3<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart3->at(i).Line1<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart3->at(i).Line2<<" ";
				outfile<<setiosflags(ios::fixed)<<setprecision(8)<<triSonPart3->at(i).Line3<<endl;

			}

			for(unsigned int j = 0 ; j < lineSonPartSize3 ; j++)
			{
				outfileL<<setiosflags(ios::fixed)<<setprecision(8)<<lineSonPart3->at(j).LeftPoint<<" ";
				outfileL<<setiosflags(ios::fixed)<<setprecision(8)<<lineSonPart3->at(j).RightPoint<<endl;
			}

			for(unsigned int k = 0 ; k < convexSonPartSize3 ; k++)
			{
				outfileC<<setiosflags(ios::fixed)<<setprecision(8)<<convexSonPart3->at(k)<<endl;
			}

			outfile.close();
			outfileL.close();
			outfileC.close();
			break;
		}
	default:
		cout<<"保存三角形和线文件时出错！"<<endl;
		break;

	}
	
}

void saveTritext()
{
	ofstream outfile;
	ofstream outfileL;
	//ifstream infile; 
	outfile.open("C:\\Users\\jaggerjack330\\Documents\\Visual Studio 2010\\Projects\\Tin11\\Tri.txt");
	outfileL.open("C:\\Users\\jaggerjack330\\Documents\\Visual Studio 2010\\Projects\\Tin11\\Line.txt");
	//infile.open("C:\\Users\\jaggerjack330\\Documents\\Visual Studio 2010\\Projects\\Tin11\\Tri.txt");
	
	unsigned int TriFileSize = TriFile->size();
	unsigned int lineFileSize = lineFile->size();
	/*if(outfile.eof())
	{*/
		for(unsigned int i = 0 ; i < TriFileSize ; i++)
		{
			
			outfile<<setiosflags(ios::fixed)<<setprecision(8)<<TriFile->at(i).p1<<" ";
			outfile<<setiosflags(ios::fixed)<<setprecision(8)<<TriFile->at(i).p2<<" ";
			outfile<<setiosflags(ios::fixed)<<setprecision(8)<<TriFile->at(i).p3<<" ";
			outfile<<setiosflags(ios::fixed)<<setprecision(8)<<TriFile->at(i).Line1<<" ";
			outfile<<setiosflags(ios::fixed)<<setprecision(8)<<TriFile->at(i).Line2<<" ";
			outfile<<setiosflags(ios::fixed)<<setprecision(8)<<TriFile->at(i).Line3<<endl;


		}

		for(unsigned int j = 0 ; j < lineFileSize ; j++)
		{
			outfileL<<setiosflags(ios::fixed)<<setprecision(8)<<lineFile->at(j).LeftPoint<<" ";
			outfileL<<setiosflags(ios::fixed)<<setprecision(8)<<lineFile->at(j).RightPoint<<endl;
		}
	//}

	/*for(unsigned int i = 0 ; i < rPoint->size() ; i++)
	{
		outfile<<getstring(rPoint->at(i).X)<<" "<<getstring(rPoint->at(i).Y)<<" "<<getstring(rPoint->at(i).Z)<<" ";
		
	}*/
	/*for(unsigned int j = 0 ; j < rPoint->size() ; j++)
	{
		outfile<<setiosflags(ios::fixed)<<setprecision(8)<<rPoint->at(j).X<<" ";
		outfile<<setiosflags(ios::fixed)<<setprecision(8)<<rPoint->at(j).Y<<" ";
		outfile<<setiosflags(ios::fixed)<<setprecision(8)<<rPoint->at(j).Z<<endl;
	}*/
	//infile.close();
	outfile.close();
	outfileL.close();

}

////void saveChangeSonPartData(string path , int index)
////{
////	ofstream outfileD;
////	outfileD.open(path);
////	switch(index)
////	{
////	case 0:
////		{
////			unsigned int changeSonPartDataSize0 = changeSonPartData0->size();
////			for(unsigned int i = 0 ; i < changeSonPartDataSize0 ; i++)
////			{
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).changeTri->size()<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(0).index<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(0).p1<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(0).p2<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(0).p3<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(1).index<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(1).p1<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(1).p2<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(1).p3<<" ";
////				for(unsigned int j = 0 ; j < changeSonPartData0->at(i).changeTri->size() ; j++)
////				{
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).changeTri->at(j).index<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).changeTri->at(j).p1<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).changeTri->at(j).p2<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).changeTri->at(j).p3<<" ";
////				}
////				for(unsigned int k = 0 ; k < changeSonPartData0->at(i).originTri->size() ; k++)
////				{
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).originTri->at(k).index<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).originTri->at(k).p1<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).originTri->at(k).p2<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).originTri->at(k).p3<<" ";
////				}
////				outfileD<<endl;
////			}
////			outfileD.close();
////			break;
////		}
////
////		case 1:
////		{
////			unsigned int changeSonPartDataSize1 = changeSonPartData1->size();
////			for(unsigned int i = 0 ; i < changeSonPartDataSize1 ; i++)
////			{
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).changeTri->size()<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(0).index<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(0).p1<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(0).p2<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(0).p3<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(1).index<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(1).p1<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(1).p2<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(1).p3<<" ";
////				for(unsigned int j = 0 ; j < changeSonPartData1->at(i).changeTri->size() ; j++)
////				{
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).changeTri->at(j).index<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).changeTri->at(j).p1<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).changeTri->at(j).p2<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).changeTri->at(j).p3<<" ";
////				}
////				for(unsigned int k = 0 ; k < changeSonPartData1->at(i).originTri->size() ; k++)
////				{
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).originTri->at(k).index<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).originTri->at(k).p1<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).originTri->at(k).p2<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).originTri->at(k).p3<<" ";
////				}
////				outfileD<<endl;
////			}
////			outfileD.close();
////			break;
////		}
////
////		case 2:
////		{
////			unsigned int changeSonPartDataSize2 = changeSonPartData2->size();
////			for(unsigned int i = 0 ; i < changeSonPartDataSize2 ; i++)
////			{
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).changeTri->size()<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(0).index<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(0).p1<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(0).p2<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(0).p3<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(1).index<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(1).p1<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(1).p2<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(1).p3<<" ";
////				for(unsigned int j = 0 ; j < changeSonPartData2->at(i).changeTri->size() ; j++)
////				{
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).changeTri->at(j).index<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).changeTri->at(j).p1<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).changeTri->at(j).p2<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).changeTri->at(j).p3<<" ";
////				}
////				for(unsigned int k = 0 ; k < changeSonPartData2->at(i).originTri->size() ; k++)
////				{
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).originTri->at(k).index<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).originTri->at(k).p1<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).originTri->at(k).p2<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).originTri->at(k).p3<<" ";
////				}
////				outfileD<<endl;
////			}
////			outfileD.close();
////			break;
////		}
////
////		case 3:
////		{
////			unsigned int changeSonPartDataSize3 = changeSonPartData3->size();
////			for(unsigned int i = 0 ; i < changeSonPartDataSize3 ; i++)
////			{
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).changeTri->size()<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(0).index<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(0).p1<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(0).p2<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(0).p3<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(1).index<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(1).p1<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(1).p2<<" ";
////				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(1).p3<<" ";
////				for(unsigned int j = 0 ; j < changeSonPartData3->at(i).changeTri->size() ; j++)
////				{
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).changeTri->at(j).index<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).changeTri->at(j).p1<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).changeTri->at(j).p2<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).changeTri->at(j).p3<<" ";
////				}
////				for(unsigned int k = 0 ; k < changeSonPartData3->at(i).originTri->size() ; k++)
////				{
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).originTri->at(k).index<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).originTri->at(k).p1<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).originTri->at(k).p2<<" ";
////					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).originTri->at(k).p3<<" ";
////				}
////				outfileD<<endl;
////			}
////			outfileD.close();
////			break;
////		}
////		default:
////			cout<<"子块"<<index<<"进行存储时出错"<<endl;
////			break;
////	}
////
////}


void saveSonPartError(int index)
{
	switch(index)
	{
	case 0:
		{
			ofstream outfileD;
			outfileD.open("E:\\Learn\\Tin11-6-9-10-20\\errorSonPart0.txt");
			unsigned int errorSonPartSize0 = quadraticError0->size();
			for(unsigned int i = 0 ; i < errorSonPartSize0 ; i++)
			{
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<quadraticError0->at(i)<<endl;
			}
			outfileD.close();
			break;
		}
	case 1:
		{
			ofstream outfileD;
			outfileD.open("E:\\Learn\\Tin11-6-9-10-20\\errorSonPart1.txt");
			unsigned int errorSonPartSize1 = quadraticError1->size();
			for(unsigned int i = 0 ; i < errorSonPartSize1 ; i++)
			{
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<quadraticError1->at(i)<<endl;
			}
			outfileD.close();
			break;
		}
	case 2:
		{
			ofstream outfileD;
			outfileD.open("E:\\Learn\\Tin11-6-9-10-20\\errorSonPart2.txt");
			unsigned int errorSonPartSize2 = quadraticError2->size();
			for(unsigned int i = 0 ; i < errorSonPartSize2 ; i++)
			{
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<quadraticError2->at(i)<<endl;
			}
			outfileD.close();
			break;
		}
	case 3:
		{
			ofstream outfileD;
			outfileD.open("E:\\Learn\\Tin11-6-9-10-20\\errorSonPart3.txt");
			unsigned int errorSonPartSize3 = quadraticError3->size();
			for(unsigned int i = 0 ; i < errorSonPartSize3 ; i++)
			{
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<quadraticError3->at(i)<<endl;
			}
			outfileD.close();
			break;
		}
	default:
		cout<<"存储二次误差时出错，错误子块为："<<index<<endl;
		break;
	}
}

void saveSonPartChangeData(int index)
{
	switch(index)
	{
	case 0:
		{
			ofstream outfileD;
			outfileD.open("E:\\Learn\\Tin11-6-9-10-20\\changeSonPartData0.txt");
			unsigned int changeSonPartDataSize0 = changeSonPartData0->size();
			for(unsigned int i = 0 ; i < changeSonPartDataSize0 ; i++)
			{
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).changeTri->size()<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(0).index<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(0).allIndex<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(0).p1<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(0).p2<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(0).p3<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(1).index<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(1).allIndex<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(1).p1<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(1).p2<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).noActiveTri->at(1).p3<<" ";
				for(unsigned int j = 0 ; j < changeSonPartData0->at(i).changeTri->size() ; j++)
				{
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).changeTri->at(j).index<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).changeTri->at(j).allIndex<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).changeTri->at(j).p1<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).changeTri->at(j).p2<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).changeTri->at(j).p3<<" ";
				}
				for(unsigned int k = 0 ; k < changeSonPartData0->at(i).originTri->size() ; k++)
				{
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).originTri->at(k).index<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).originTri->at(k).allIndex<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).originTri->at(k).p1<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).originTri->at(k).p2<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData0->at(i).originTri->at(k).p3<<" ";
				}
				outfileD<<endl;
			}
			outfileD.close();
			break;
		}

	case 1:
		{
			ofstream outfileD;
			outfileD.open("E:\\Learn\\Tin11-6-9-10-20\\changeSonPartData1.txt");
			unsigned int changeSonPartDataSize1 = changeSonPartData1->size();
			for(unsigned int i = 0 ; i < changeSonPartDataSize1 ; i++)
			{
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).changeTri->size()<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(0).index<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(0).allIndex<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(0).p1<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(0).p2<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(0).p3<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(1).index<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(1).allIndex<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(1).p1<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(1).p2<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).noActiveTri->at(1).p3<<" ";
				for(unsigned int j = 0 ; j < changeSonPartData1->at(i).changeTri->size() ; j++)
				{
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).changeTri->at(j).index<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).changeTri->at(j).allIndex<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).changeTri->at(j).p1<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).changeTri->at(j).p2<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).changeTri->at(j).p3<<" ";
				}
				for(unsigned int k = 0 ; k < changeSonPartData1->at(i).originTri->size() ; k++)
				{
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).originTri->at(k).index<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).originTri->at(k).allIndex<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).originTri->at(k).p1<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).originTri->at(k).p2<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData1->at(i).originTri->at(k).p3<<" ";
				}
				outfileD<<endl;
			}
			outfileD.close();
			break;
		}

	case 2:
		{
			ofstream outfileD;
			outfileD.open("E:\\Learn\\Tin11-6-9-10-20\\changeSonPartData2.txt");
			unsigned int changeSonPartDataSize2 = changeSonPartData2->size();
			for(unsigned int i = 0 ; i < changeSonPartDataSize2 ; i++)
			{
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).changeTri->size()<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(0).index<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(0).allIndex<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(0).p1<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(0).p2<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(0).p3<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(1).index<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(1).allIndex<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(1).p1<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(1).p2<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).noActiveTri->at(1).p3<<" ";
				for(unsigned int j = 0 ; j < changeSonPartData2->at(i).changeTri->size() ; j++)
				{
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).changeTri->at(j).index<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).changeTri->at(j).allIndex<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).changeTri->at(j).p1<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).changeTri->at(j).p2<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).changeTri->at(j).p3<<" ";
				}
				for(unsigned int k = 0 ; k < changeSonPartData2->at(i).originTri->size() ; k++)
				{
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).originTri->at(k).index<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).originTri->at(k).allIndex<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).originTri->at(k).p1<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).originTri->at(k).p2<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData2->at(i).originTri->at(k).p3<<" ";
				}
				outfileD<<endl;
			}
			outfileD.close();
			break;
		}

	case 3:
		{
			ofstream outfileD;
			outfileD.open("E:\\Learn\\Tin11-6-9-10-20\\changeSonPartData3.txt");
			unsigned int changeSonPartDataSize3 = changeSonPartData3->size();
			for(unsigned int i = 0 ; i < changeSonPartDataSize3 ; i++)
			{
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).changeTri->size()<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(0).index<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(0).allIndex<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(0).p1<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(0).p2<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(0).p3<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(1).index<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(1).allIndex<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(1).p1<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(1).p2<<" ";
				outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).noActiveTri->at(1).p3<<" ";
				for(unsigned int j = 0 ; j < changeSonPartData3->at(i).changeTri->size() ; j++)
				{
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).changeTri->at(j).index<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).changeTri->at(j).allIndex<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).changeTri->at(j).p1<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).changeTri->at(j).p2<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).changeTri->at(j).p3<<" ";
				}
				for(unsigned int k = 0 ; k < changeSonPartData3->at(i).originTri->size() ; k++)
				{
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).originTri->at(k).index<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).originTri->at(k).allIndex<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).originTri->at(k).p1<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).originTri->at(k).p2<<" ";
					outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeSonPartData3->at(i).originTri->at(k).p3<<" ";
				}
				outfileD<<endl;
			}
			outfileD.close();
			break;
		}

	default:
		cout<<"边折叠信息存至文本时出错，错误子块为"<<index<<endl;
		break;
	}
}

void savechangeData()//三角形变化的信息存储顺序 1.发生变化的三角形数量 2.删除的两个三角形的三点编号 3.变化的三角形的三点编号 4.变化的三角形的原始三点编号
{
	ofstream outfileD;
	outfileD.open("C:\\Users\\jaggerjack330\\Documents\\Visual Studio 2010\\Projects\\Tin11\\changeDataL.txt");
	unsigned int changeDataLSize = changeDataL->size();
	for(unsigned int i = 0 ; i < changeDataLSize ; i++)
	{
		outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).changeTri->size()<<" ";
		outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).noActiveTri->at(0).index<<" ";
		outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).noActiveTri->at(0).p1<<" ";
		outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).noActiveTri->at(0).p2<<" ";
		outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).noActiveTri->at(0).p3<<" ";
		outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).noActiveTri->at(1).index<<" ";
		outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).noActiveTri->at(1).p1<<" ";
		outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).noActiveTri->at(1).p2<<" ";
		outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).noActiveTri->at(1).p3<<" ";
		for(unsigned int j = 0 ; j < changeDataL->at(i).changeTri->size() ; j++)
		{
			outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).changeTri->at(j).index<<" ";
			outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).changeTri->at(j).p1<<" ";
			outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).changeTri->at(j).p2<<" ";
			outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).changeTri->at(j).p3<<" ";
		}
		for(unsigned int k = 0 ; k < changeDataL->at(i).originTri->size() ; k++)
		{
			outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).originTri->at(k).index<<" ";
			outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).originTri->at(k).p1<<" ";
			outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).originTri->at(k).p2<<" ";
			outfileD<<setiosflags(ios::fixed)<<setprecision(8)<<changeDataL->at(i).originTri->at(k).p3<<" ";
		}
		outfileD<<endl;
	}
	outfileD.close();
}

//世界坐标转屏幕坐标
osg::Vec3 worldToScreen(osg::Vec3 world , osg::ref_ptr<osgViewer::Viewer> viewer)
{
	//osg::Camera *camera = new osg::Camera;
	osg::ref_ptr<osg::Camera> camera = new osg::Camera();
	camera = viewer->getCamera();
	osg::Matrix VPW = camera->getViewMatrix() * camera->getProjectionMatrix() *
		camera->getViewport()->computeWindowMatrix();
	osg::Vec3 window = world * VPW;
	return window;

}

//屏幕坐标转世界坐标
osg::Vec3 screenToWorld(osg::Vec3 screen , osg::ref_ptr<osgViewer::Viewer> viewer)
{
	osg::ref_ptr<osg::Camera> camera = new osg::Camera();
	camera = viewer->getCamera();
	osg::Matrix VPW = camera->getViewMatrix()*
                      camera->getProjectionMatrix()*
                      camera->getViewport()->computeWindowMatrix();
	osg::Matrix inverseVPW =osg::Matrix::inverse(VPW);
	osg::Vec3 world = screen * inverseVPW;
	return world;
}

//void RefreshTimeText(string textString)
//{
//    setlocale(LC_ALL,".936");//　配置地域化信息
//    createContent(*titleTime,textString.data());
//}

//VECTORMYTRI* dealData()
//{
//	//VECTORMYLINE* sameLine;
//	VECTORMYTRI* sameTri;
//	for(unsigned int i = 0 ; i < TriFile->size() ; i++)
//	{
//		sameTri = new VECTORMYTRI;
//		for(unsigned int j = 0 ; j < TriFile->size() ; j++)
//		{
//			if( i != j )
//			{
//				/*if(lineFile->at(i).LeftPoint == lineFile->at(j).LeftPoint&&lineFile->at(i).RightPoint == lineFile->at(j).RightPoint)
//				{
//					
//					sameLine->push_back(lineFile->at(i));
//				}*/
//				if(TriFile->at(i).p1 == TriFile->at(j).p1 || TriFile->at(i).p1 == TriFile->at(j).p2 || TriFile->at(i).p1 == TriFile->at(j).p3)
//				{
//					if(TriFile->at(i).p2 == TriFile->at(j).p1 || TriFile->at(i).p2 == TriFile->at(j).p2 || TriFile->at(i).p2 == TriFile->at(j).p3)
//					{
//						if(TriFile->at(i).p3 == TriFile->at(j).p1 || TriFile->at(i).p3 == TriFile->at(j).p2 || TriFile->at(i).p3 == TriFile->at(j).p3)
//						{
//							sameTri->push_back(TriFile->at(i));
//						}
//					}
//				}
//			}
//			if(sameTri->size() == 2)
//			{
//				return sameTri;
//			}
//		}
//		delete sameTri;
//	}
//	return sameTri;
//}

//struct unionDouble
//{
//	VECTORDOUBLE* indexes1;
//};


class SysncOperation:public osg::GraphicsOperation
{
 
public:
    SysncOperation():osg::GraphicsOperation("SysncOperation",false)
    {
 
    }
    virtual void operator() (osg::GraphicsContext* context)
    {
        if(context)
        {
            context->makeCurrent();
            if(osgViewer::GraphicsWindow* window = dynamic_cast<osgViewer::GraphicsWindow*>(context))
            {
                window->setSyncToVBlank(false); // 关闭显卡垂直同步
            }
        }
    }
};


int main( int argc, char **argv )
{
	string str = "E:\\Learn\\Tin11-6-9-10-20\\461611-12new.txt";//Data file
	string strNew = "E:\\Learn\\Tin11-6-9-10-20\\20000.txt";
    //osg::ref_ptr<deprecated_osg::Geometry> geom2 = new deprecated_osg::Geometry;
	osg::ref_ptr<osg::Geode> geode1 = new osg::Geode();
	
	/*osg::ref_ptr<osg::Geode> geode2 = new osg::Geode();

	osg::ref_ptr<osg::Geode> geode3 = new osg::Geode();
	
	osg::ref_ptr<osg::Geode> geode4 = new osg::Geode();

	osg::ref_ptr<osg::Geode> geode5 = new osg::Geode();

	osg::ref_ptr<osg::Geode> geode6 = new osg::Geode();
	
	osg::ref_ptr<osg::Geode> geode7 = new osg::Geode();

	osg::ref_ptr<osg::Geode> geode8 = new osg::Geode();
	
	osg::ref_ptr<osg::Geode> geode9 = new osg::Geode();

	osg::ref_ptr<osg::Geode> geode10 = new osg::Geode();

	osg::ref_ptr<osg::Geode> geode11 = new osg::Geode();*/
	
	osg::ref_ptr<osg::Switch> root = new osg::Switch();

	osg::ref_ptr<osg::Vec2Array> texcoords= new osg::Vec2Array; //纹理坐标1
	//osg::ref_ptr<osg::Vec2Array> texcoords1= new osg::Vec2Array; //纹理坐标2
	//osg::ref_ptr<osg::Vec2Array> texcoords2= new osg::Vec2Array; //纹理坐标3
	//osg::ref_ptr<osg::Vec2Array> texcoords3= new osg::Vec2Array; //纹理坐标4
	//osg::ref_ptr<osg::Vec2Array> texcoords4= new osg::Vec2Array; //纹理坐标4
	//osg::ref_ptr<osg::Vec2Array> texcoords5= new osg::Vec2Array; //纹理坐标5
	//osg::ref_ptr<osg::Vec2Array> texcoords6= new osg::Vec2Array; //纹理坐标6
	//osg::ref_ptr<osg::Vec2Array> texcoords7= new osg::Vec2Array; //纹理坐标7
	//osg::ref_ptr<osg::Vec2Array> texcoords8= new osg::Vec2Array; //纹理坐标8
	//osg::ref_ptr<osg::Vec2Array> texcoords9= new osg::Vec2Array; //纹理坐标9
	//osg::ref_ptr<osg::Vec2Array> texcoords10 = new osg::Vec2Array;

	
	//texture->setImage(osgDB::readImageFile("ganquandaonew1.JPG"));
	texture->setImage(osgDB::readImageFile("Yanzhi.JPG"));
	osg::ref_ptr<HUD> hud = new HUD();


	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	//方便查看在多边形之间切换，以查看三角网
	//viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
	

	VECTORMYTRI* TriFileLV1 = new VECTORMYTRI;
	/*VECTORMYTRI* TriFileLV2 = new VECTORMYTRI;
	VECTORMYTRI* TriFileLV3 = new VECTORMYTRI;
	VECTORMYTRI* TriFileLV4 = new VECTORMYTRI;
	VECTORMYTRI* TriFileLV5 = new VECTORMYTRI;
	VECTORMYTRI* TriFileLV6 = new VECTORMYTRI;
	VECTORMYTRI* TriFileLV7 = new VECTORMYTRI;
	VECTORMYTRI* TriFileLV8 = new VECTORMYTRI;
	VECTORMYTRI* TriFileLV9 = new VECTORMYTRI;
	VECTORMYTRI* TriFileLV10 = new VECTORMYTRI;*/
	
	/*geode1->setNodeMask(CastsShadowTraversalMask);
	geode1->setNodeMask(ReceivesShadowTraversalMask);*/
	root->setDataVariance(osg::Object::DYNAMIC);
	geode1->setDataVariance(osg::Object::DYNAMIC);
	/*geode2->setDataVariance(osg::Object::DYNAMIC);
	geode3->setDataVariance(osg::Object::DYNAMIC);
	geode4->setDataVariance(osg::Object::DYNAMIC);
	geode5->setDataVariance(osg::Object::DYNAMIC);
	geode6->setDataVariance(osg::Object::DYNAMIC);
	geode7->setDataVariance(osg::Object::DYNAMIC);
	geode8->setDataVariance(osg::Object::DYNAMIC);
	geode9->setDataVariance(osg::Object::DYNAMIC);
	geode10->setDataVariance(osg::Object::DYNAMIC);
	geode11->setDataVariance(osg::Object::DYNAMIC);*/

	geom1->setDataVariance(osg::Object::DYNAMIC);
	/*geom2->setDataVariance(osg::Object::DYNAMIC);
	geom3->setDataVariance(osg::Object::DYNAMIC);
	geom4->setDataVariance(osg::Object::DYNAMIC);
	geom5->setDataVariance(osg::Object::DYNAMIC);
	geom6->setDataVariance(osg::Object::DYNAMIC);
	geom7->setDataVariance(osg::Object::DYNAMIC);
	geom8->setDataVariance(osg::Object::DYNAMIC);
	geom9->setDataVariance(osg::Object::DYNAMIC);
	geom10->setDataVariance(osg::Object::DYNAMIC);
	geom11->setDataVariance(osg::Object::DYNAMIC);*/
	

	
	

	//VECTORMYPOINT* rPoint = new VECTORMYPOINT;
	//VECTORMYNORMAL* normalList = new VECTORMYNORMAL;

	

	rPoint = LoadPoint(strNew);
//	bool ifc = check(rPoint);

	GroupPoint();

	//0号子块
	struct unionMyPoint pointPa0;
	pointPa0.sonPointPart = pointPart0;
	pointPa0.partIndex = 0;
	//1号子块
	struct unionMyPoint pointPa1;
	pointPa1.sonPointPart = pointPart1;
	pointPa1.partIndex = 1;
	//2号子块
	struct unionMyPoint pointPa2;
	pointPa2.sonPointPart = pointPart2;
	pointPa2.partIndex = 2;
	//3号子块
	struct unionMyPoint pointPa3;
	pointPa3.sonPointPart = pointPart3;
	pointPa3.partIndex = 3;

	/*********
	0号子块对应编号
	************/
	struct unionIndex partID0;
	partID0.partIndex = 0;
	struct unionIndex partID1;
	partID1.partIndex = 1;
	struct unionIndex partID2;
	partID2.partIndex = 2;
	struct unionIndex partID3;
	partID3.partIndex = 3;

	
	


	ifstream infileT;
	ifstream infileL;
	ifstream infileT1;
	ifstream infileL1;
	ifstream infileT2;
	ifstream infileL2;
	ifstream infileT3;
	ifstream infileL3;

	ifstream infileC;
	ifstream infileC1;
	ifstream infileC2;
	ifstream infileC3;



	string TriTxt = "E:\\Learn\\Tin11-6-9-10-20\\sonPartTri0.txt";//Tri存储的文件路径
	string LineTxt = "E:\\Learn\\Tin11-6-9-10-20\\sonPartLine0.txt";//Line存储的文件路径
	string TriTxt1 = "E:\\Learn\\Tin11-6-9-10-20\\sonPartTri1.txt";
	string LineTxt1 = "E:\\Learn\\Tin11-6-9-10-20\\sonPartLine1.txt";
	string TriTxt2 = "E:\\Learn\\Tin11-6-9-10-20\\sonPartTri2.txt";
	string LineTxt2 = "E:\\Learn\\Tin11-6-9-10-20\\sonPartLine2.txt";
	string TriTxt3 = "E:\\Learn\\Tin11-6-9-10-20\\sonPartTri3.txt";
	string LineTxt3 = "E:\\Learn\\Tin11-6-9-10-20\\sonPartLine3.txt";
	string ConvexTxt = "E:\\Learn\\Tin11-6-9-10-20\\convex0.txt";
	string ConvexTxt1 = "E:\\Learn\\Tin11-6-9-10-20\\convex1.txt";
	string ConvexTxt2 = "E:\\Learn\\Tin11-6-9-10-20\\convex2.txt";
	string ConvexTxt3 = "E:\\Learn\\Tin11-6-9-10-20\\convex3.txt";

	infileT.open(TriTxt , ios::in);
	infileL.open(LineTxt.data());

	infileT1.open(TriTxt1 , ios::in);
	infileL1.open(LineTxt1.data());

	infileT2.open(TriTxt2 , ios::in);
	infileL2.open(LineTxt2.data());

	infileT3.open(TriTxt3 , ios::in);
	infileL3.open(LineTxt3.data());

	infileC.open(ConvexTxt.data());
	infileC1.open(ConvexTxt1.data());
	infileC2.open(ConvexTxt2.data());
	infileC3.open(ConvexTxt3.data());

	string iftxt;
	infileT >> iftxt;

	if(iftxt == "")
	{
		pthread_t thread[4];
		int rc1 , rc2 , rc3 , rc4 , rc;
		void *status;
		/*pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);*/
		pthread_attr_t attr;

		// 初始化并设置线程为可连接的（joinable）
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		/*rc1 = pthread_create(&thread[0] , &attr , BuildSonTin , (void *)&(pointPa0));
		rc2 = pthread_create(&thread[1] , &attr , BuildSonTin , (void *)&(pointPa1));
		rc3 = pthread_create(&thread[2] , &attr , BuildSonTin , (void *)&(pointPa2));
		rc4 = pthread_create(&thread[3] , &attr , BuildSonTin , (void *)&(pointPa3));*/

		rc1 = pthread_create(&thread[0] , NULL , BuildSonTin , (void *)&(pointPa0));
		rc2 = pthread_create(&thread[1] , NULL , BuildSonTin , (void *)&(pointPa1));
		rc3 = pthread_create(&thread[2] , NULL , BuildSonTin , (void *)&(pointPa2));
		rc4 = pthread_create(&thread[3] , NULL , BuildSonTin , (void *)&(pointPa3));

		//rc = pthread_join(thread[0], &status);

		for(int i=0; i < 4; i++ )
		{
		  rc = pthread_join(thread[i], &status);
		  if (rc)
		  {
			 cout << "Error:unable to join," << rc << endl;
			 exit(-1);
		  }
		  //cout << "Main: completed thread id :" << i ;
		  //cout << "  exiting with status :" << status << endl;
	    }
		saveSonPartTriAndLine(0);
		saveSonPartTriAndLine(1);
		saveSonPartTriAndLine(2);
		saveSonPartTriAndLine(3);

	
	}
	else
	{
		string pA , pB , pC , lA , lB , lC , plA , plB , c;
		MyTri myTri;
		MyLine myLine;
		infileT >> pB;
		infileT >> pC;
		infileT >> lA;
		infileT >> lB;
		infileT >> lC;
		myTri.p1 = stringToNum(iftxt);
		myTri.p2 = stringToNum(pB);
		myTri.p3 = stringToNum(pC);
		myTri.Line1 = stringToNum(lA);
		myTri.Line2 = stringToNum(lB);
		myTri.Line3 = stringToNum(lC);
		triSonPart0->push_back(myTri);
		triSonPartTemp0->push_back(myTri);

		while(!infileT.eof())
		{
			infileT >> pA;
			infileT >> pB;
			infileT >> pC;
			infileT >> lA;
			infileT >> lB;
			infileT >> lC;
			myTri.p1 = stringToNum(pA);
			myTri.p2 = stringToNum(pB);
			myTri.p3 = stringToNum(pC);
			myTri.Line1 = stringToNum(lA);
			myTri.Line2 = stringToNum(lB);
			myTri.Line3 = stringToNum(lC);
			triSonPart0->push_back(myTri);
			triSonPartTemp0->push_back(myTri);
		}
		triSonPart0->pop_back();
		triSonPartTemp0->pop_back();
		infileT.close();

		while(!infileT1.eof())
		{
			infileT1 >> pA;
			infileT1 >> pB;
			infileT1 >> pC;
			infileT1 >> lA;
			infileT1 >> lB;
			infileT1 >> lC;
			myTri.p1 = stringToNum(pA);
			myTri.p2 = stringToNum(pB);
			myTri.p3 = stringToNum(pC);
			myTri.Line1 = stringToNum(lA);
			myTri.Line2 = stringToNum(lB);
			myTri.Line3 = stringToNum(lC);
			triSonPart1->push_back(myTri);
			triSonPartTemp1->push_back(myTri);
		}
		triSonPart1->pop_back();
		triSonPartTemp1->pop_back();
		infileT1.close();

		while(!infileT2.eof())
		{
			infileT2 >> pA;
			infileT2 >> pB;
			infileT2 >> pC;
			infileT2 >> lA;
			infileT2 >> lB;
			infileT2 >> lC;
			myTri.p1 = stringToNum(pA);
			myTri.p2 = stringToNum(pB);
			myTri.p3 = stringToNum(pC);
			myTri.Line1 = stringToNum(lA);
			myTri.Line2 = stringToNum(lB);
			myTri.Line3 = stringToNum(lC);
			triSonPart2->push_back(myTri);
			triSonPartTemp2->push_back(myTri);
		}
		triSonPart2->pop_back();
		triSonPartTemp2->pop_back();
		infileT2.close();

		while(!infileT3.eof())
		{
			infileT3 >> pA;
			infileT3 >> pB;
			infileT3 >> pC;
			infileT3 >> lA;
			infileT3 >> lB;
			infileT3 >> lC;
			myTri.p1 = stringToNum(pA);
			myTri.p2 = stringToNum(pB);
			myTri.p3 = stringToNum(pC);
			myTri.Line1 = stringToNum(lA);
			myTri.Line2 = stringToNum(lB);
			myTri.Line3 = stringToNum(lC);
			triSonPart3->push_back(myTri);
			triSonPartTemp3->push_back(myTri);
		}
		triSonPart3->pop_back();
		triSonPartTemp3->pop_back();
		infileT3.close();

		while(!infileL.eof())
		{
			infileL>>plA;
			infileL>>plB;
			myLine.LeftPoint = stringToNum(plA);
			myLine.RightPoint = stringToNum(plB);
			lineSonPart0->push_back(myLine);
		}
		lineSonPart0->pop_back();
		infileL.close();

		while(!infileL1.eof())
		{
			infileL1>>plA;
			infileL1>>plB;
			myLine.LeftPoint = stringToNum(plA);
			myLine.RightPoint = stringToNum(plB);
			lineSonPart1->push_back(myLine);
		}
		lineSonPart1->pop_back();
		infileL1.close();

		while(!infileL2.eof())
		{
			infileL2>>plA;
			infileL2>>plB;
			myLine.LeftPoint = stringToNum(plA);
			myLine.RightPoint = stringToNum(plB);
			lineSonPart2->push_back(myLine);
		}
		lineSonPart2->pop_back();
		infileL2.close();

		while(!infileL3.eof())
		{
			infileL3>>plA;
			infileL3>>plB;
			myLine.LeftPoint = stringToNum(plA);
			myLine.RightPoint = stringToNum(plB);
			lineSonPart3->push_back(myLine);
		}
		lineSonPart3->pop_back();
		infileL3.close();

		while(!infileC.eof())
		{
			infileC>>c;
			convexSonPart0->push_back(stringToNum(c));
		}
		convexSonPart0->pop_back();
		infileC.close();

		while(!infileC1.eof())
		{
			infileC1>>c;
			convexSonPart1->push_back(stringToNum(c));
		}
		convexSonPart1->pop_back();
		infileC1.close();

		while(!infileC2.eof())
		{
			infileC2>>c;
			convexSonPart2->push_back(stringToNum(c));
		}
		convexSonPart2->pop_back();
		infileC2.close();

		while(!infileC3.eof())
		{
			infileC3>>c;
			convexSonPart3->push_back(stringToNum(c));
		}
		convexSonPart3->pop_back();
		infileC3.close();

	}
	EnSureAllConvex();

	/****************
	为每个点添加index，对应自己的子块
	***********************/
	for(unsigned int i = 0 ; i < pointPart0->size() ; i++)
	{
		pointPart0->at(i).index = i;
	}
	for(unsigned int j = 0 ; j < pointPart1->size() ; j++)
	{
		pointPart1->at(j).index = j;
	}
	for(unsigned int k = 0 ; k < pointPart2->size() ; k++)
	{
		pointPart2->at(k).index = k;
	}
	for(unsigned int l = 0 ; l < pointPart3->size() ; l++)
	{
		pointPart3->at(l).index = l;
	}

	/*************************************
	为三角形数组各个对象添加index编号，各个编号只针对各自数组
	**********************************/
	for(unsigned int i = 0 ; i < triSonPart0->size() ; i++)
	{
		triSonPart0->at(i).index = i;
		triSonPartTemp0->at(i).index = i;
	}
	for(unsigned int j = 0 ; j < triSonPart1->size() ; j++)
	{
		triSonPart1->at(j).index = j;
		triSonPartTemp1->at(j).index = j;
	}
	for(unsigned int k = 0 ; k < triSonPart2->size() ; k++)
	{
		triSonPart2->at(k).index = k;
		triSonPartTemp2->at(k).index = k;
	}
	for(unsigned int l = 0 ; l < triSonPart3->size() ; l++)
	{
		triSonPart3->at(l).index = l;
		triSonPartTemp3->at(l).index = l;
	}

	/************************
	计算法向量，先算面，再算点
	*********************/
	getSonPartNormal(pointPart0 , 0);
	getSonPartNormal(pointPart1 , 1);
	getSonPartNormal(pointPart2 , 2);
	getSonPartNormal(pointPart3 , 3);

	/****************
	点的法线计算
	******************/
	getSonPartPointNormal(pointPart0 , 0);
	getSonPartPointNormal(pointPart1 , 1);
	getSonPartPointNormal(pointPart2 , 2);
	getSonPartPointNormal(pointPart3 , 3);

	/********************
	计算子块四个角点
	**********************/
	EnSureCornerPoint(0);
	EnSureCornerPoint(1);
	EnSureCornerPoint(2);
	EnSureCornerPoint(3);
	DealConvexTin();
	ensureSonPartConvex();

	/**************
	给三角形数组添加index
	**************/
	for(unsigned int i = 0 ; i < triSonPart0->size() ; i++)
	{
		//triSonPart0->at(i).index = i;
		triSonPart0->at(i).allIndex = i;
		//triSonPartTemp0->at(i).index = i;
		triSonPartTemp0->at(i).allIndex = i;
	}
	for(unsigned int j = 0 ; j < triSonPart1->size() ; j++)
	{
		//triSonPart1->at(j).index = j;
		triSonPart1->at(j).allIndex = j + triSonPart0->size();
		//triSonPartTemp1->at(j).index = j;
		triSonPartTemp1->at(j).allIndex = j + triSonPartTemp0->size();;
	}
	for(unsigned int k = 0 ; k < triSonPart2->size() ; k++)
	{
		//triSonPart2->at(k).index = k;
		triSonPart2->at(k).allIndex = k + triSonPart0->size() + triSonPart1->size();
		//triSonPartTemp2->at(k).index = k;
		triSonPartTemp2->at(k).allIndex = k + triSonPartTemp0->size() + triSonPartTemp1->size();
	}
	for(unsigned int l = 0 ; l < triSonPart3->size() ; l++)
	{
		//triSonPart3->at(l).index = l;
		triSonPart3->at(l).allIndex = l +  triSonPart0->size() + triSonPart1->size() + triSonPart2->size();
		//triSonPartTemp3->at(l).index = l;
		triSonPartTemp3->at(l).allIndex = l + triSonPartTemp0->size() + triSonPartTemp1->size() + triSonPartTemp2->size();
	}



	string changeTxt0 = "E:\\Learn\\Tin11-6-9-10-20\\changeSonPartData0.txt";
	string changeTxt1 = "E:\\Learn\\Tin11-6-9-10-20\\changeSonPartData1.txt";
	string changeTxt2 = "E:\\Learn\\Tin11-6-9-10-20\\changeSonPartData2.txt";
	string changeTxt3 = "E:\\Learn\\Tin11-6-9-10-20\\changeSonPartData3.txt";

	ifstream infileSonPartData;
	ifstream infileSonPartData1;
	ifstream infileSonPartData2;
	ifstream infileSonPartData3;

	infileSonPartData.open(changeTxt0 , ios::in);
	infileSonPartData1.open(changeTxt1.data());
	infileSonPartData2.open(changeTxt2.data());
	infileSonPartData3.open(changeTxt3.data());
	string iftxtD;
	infileSonPartData >> iftxtD;
	if(iftxtD == "")
	{
		/********************
		进行边折叠初始化
		***************/
		pthread_t threadFold[4];
		int rcF1 , rcF2 , rcF3 , rcF4 , rcF;
		void *statusF;

		pthread_attr_t attrF;

		// 初始化并设置线程为可连接的（joinable）
		pthread_attr_init(&attrF);
		pthread_attr_setdetachstate(&attrF, PTHREAD_CREATE_JOINABLE);

		

		rcF1 = pthread_create(&threadFold[0] , NULL , sideFolding , (void *)&(partID0));
		rcF2 = pthread_create(&threadFold[1] , NULL , sideFolding , (void *)&(partID1));
		rcF3 = pthread_create(&threadFold[2] , NULL , sideFolding , (void *)&(partID2));
		rcF4 = pthread_create(&threadFold[3] , NULL , sideFolding , (void *)&(partID3));

		//rc = pthread_join(thread[0], &status);

		for(int i=0; i < 4; i++ )
		{
		  rcF = pthread_join(threadFold[i], &statusF);
		  if (rcF)
		  {
			 cout << "Error:unable to join," << rcF << endl;
			 exit(-1);
		  }
		  //cout << "Main: completed thread id :" << i ;
		  //cout << "  exiting with status :" << status << endl;
	    }
	}
	else
	{
		string dSize , indexD , indexAllD , pDA , pDB , pDC;
		MyTri myTriD;
		ChangeData *changeDataD = new ChangeData;
		/*****************
		第一个被删除的三角形
		******************/
		infileSonPartData >> indexD;
		infileSonPartData >> indexAllD;
		infileSonPartData >> pDA;
		infileSonPartData >> pDB;
		infileSonPartData >> pDC;
		myTriD.index = stringToNum(indexD);
		myTriD.allIndex = stringToNum(indexAllD);
		myTriD.p1 = stringToNum(pDA);
		myTriD.p2 = stringToNum(pDB);
		myTriD.p3 = stringToNum(pDC);
		changeDataD->insertNoActive(myTriD);

		/*********************
		第二个被删除的三角形
		*********************/
		infileSonPartData >> indexD;
		infileSonPartData >> indexAllD;
		infileSonPartData >> pDA;
		infileSonPartData >> pDB;
		infileSonPartData >> pDC;
		myTriD.index = stringToNum(indexD);
		myTriD.allIndex = stringToNum(indexAllD);
		myTriD.p1 = stringToNum(pDA);
		myTriD.p2 = stringToNum(pDB);
		myTriD.p3 = stringToNum(pDC);
		changeDataD->insertNoActive(myTriD);
		/********************
		变化的三角形信息
		*********************/
		for(unsigned int i = 0 ; i < stringToNum(iftxtD) ; i++)
		{
			infileSonPartData >> indexD;
			infileSonPartData >> indexAllD;
			infileSonPartData >> pDA;
			infileSonPartData >> pDB;
			infileSonPartData >> pDC;
			myTriD.index = stringToNum(indexD);
			myTriD.allIndex = stringToNum(indexAllD);
			myTriD.p1 = stringToNum(pDA);
			myTriD.p2 = stringToNum(pDB);
			myTriD.p3 = stringToNum(pDC);
			changeDataD->insertChange(myTriD);
		}
		/********************
		原始的三角形信息
		******************/
		for(unsigned int j = 0 ; j < stringToNum(iftxtD) ; j++)
		{
			infileSonPartData >> indexD;
			infileSonPartData >> indexAllD;
			infileSonPartData >> pDA;
			infileSonPartData >> pDB;
			infileSonPartData >> pDC;
			myTriD.index = stringToNum(indexD);
			myTriD.allIndex = stringToNum(indexAllD);
			myTriD.p1 = stringToNum(pDA);
			myTriD.p2 = stringToNum(pDB);
			myTriD.p3 = stringToNum(pDC);
			changeDataD->insertOrigin(myTriD);
		}
		changeSonPartData0->push_back(*changeDataD);
		delete changeDataD;

		/*********************
		开始循环提取边折叠信息
		******************/
		while(!infileSonPartData.eof())
		{
			ChangeData* changeDataD = new ChangeData;
			infileSonPartData >> dSize;
			infileSonPartData >> indexD;
			infileSonPartData >> indexAllD;
			infileSonPartData >> pDA; //first deletedTri
			infileSonPartData >> pDB;
			infileSonPartData >> pDC;
			myTriD.index = stringToNum(indexD);
			myTriD.allIndex = stringToNum(indexAllD);
			myTriD.p1 = stringToNum(pDA);
			myTriD.p2 = stringToNum(pDB);
			myTriD.p3 = stringToNum(pDC);
			changeDataD->insertNoActive(myTriD);
			infileSonPartData >> indexD;
			infileSonPartData >> indexAllD;
			infileSonPartData >> pDA;// second deletedTri
			infileSonPartData >> pDB;
			infileSonPartData >> pDC;
			myTriD.index = stringToNum(indexD);
			myTriD.allIndex = stringToNum(indexAllD);
			myTriD.p1 = stringToNum(pDA);
			myTriD.p2 = stringToNum(pDB);
			myTriD.p3 = stringToNum(pDC);
			changeDataD->insertNoActive(myTriD);

			for(unsigned int i = 0 ; i < stringToNum(dSize) ; i++)
			{
				infileSonPartData >> indexD;
				infileSonPartData >> indexAllD;
				infileSonPartData >> pDA;// save changeTri
				infileSonPartData >> pDB;
				infileSonPartData >> pDC;
				myTriD.index = stringToNum(indexD);
				myTriD.allIndex = stringToNum(indexAllD);
				myTriD.p1 = stringToNum(pDA);
				myTriD.p2 = stringToNum(pDB);
				myTriD.p3 = stringToNum(pDC);
				changeDataD->insertChange(myTriD);
			}

			for(unsigned int j = 0 ; j < stringToNum(dSize) ; j++)
			{
				infileSonPartData >> indexD;
				infileSonPartData >> indexAllD;
				infileSonPartData >> pDA;// save origin
				infileSonPartData >> pDB;
				infileSonPartData >> pDC;
				myTriD.index = stringToNum(indexD);
				myTriD.allIndex = stringToNum(indexAllD);
				myTriD.p1 = stringToNum(pDA);
				myTriD.p2 = stringToNum(pDB);
				myTriD.p3 = stringToNum(pDC);
				changeDataD->insertOrigin(myTriD);
			}
			changeSonPartData0->push_back(*changeDataD);
			delete changeDataD;
		}
		changeSonPartData0->pop_back();
		infileSonPartData.close();

		while(!infileSonPartData1.eof())
		{
			ChangeData* changeDataD = new ChangeData;
			infileSonPartData1 >> dSize;
			infileSonPartData1 >> indexD;
			infileSonPartData1 >> indexAllD;
			infileSonPartData1 >> pDA; //first deletedTri
			infileSonPartData1 >> pDB;
			infileSonPartData1 >> pDC;
			myTriD.index = stringToNum(indexD);
			myTriD.allIndex = stringToNum(indexAllD);
			myTriD.p1 = stringToNum(pDA);
			myTriD.p2 = stringToNum(pDB);
			myTriD.p3 = stringToNum(pDC);
			changeDataD->insertNoActive(myTriD);
			infileSonPartData1 >> indexD;
			infileSonPartData1 >> indexAllD;
			infileSonPartData1 >> pDA;// second deletedTri
			infileSonPartData1 >> pDB;
			infileSonPartData1 >> pDC;
			myTriD.index = stringToNum(indexD);
			myTriD.allIndex = stringToNum(indexAllD);
			myTriD.p1 = stringToNum(pDA);
			myTriD.p2 = stringToNum(pDB);
			myTriD.p3 = stringToNum(pDC);
			changeDataD->insertNoActive(myTriD);

			for(unsigned int i = 0 ; i < stringToNum(dSize) ; i++)
			{
				infileSonPartData1 >> indexD;
				infileSonPartData1 >> indexAllD;
				infileSonPartData1 >> pDA;// save changeTri
				infileSonPartData1 >> pDB;
				infileSonPartData1 >> pDC;
				myTriD.index = stringToNum(indexD);
				myTriD.allIndex = stringToNum(indexAllD);
				myTriD.p1 = stringToNum(pDA);
				myTriD.p2 = stringToNum(pDB);
				myTriD.p3 = stringToNum(pDC);
				changeDataD->insertChange(myTriD);
			}

			for(unsigned int j = 0 ; j < stringToNum(dSize) ; j++)
			{
				infileSonPartData1 >> indexD;
				infileSonPartData1 >> indexAllD;
				infileSonPartData1 >> pDA;// save origin
				infileSonPartData1 >> pDB;
				infileSonPartData1 >> pDC;
				myTriD.index = stringToNum(indexD);
				myTriD.allIndex = stringToNum(indexAllD);
				myTriD.p1 = stringToNum(pDA);
				myTriD.p2 = stringToNum(pDB);
				myTriD.p3 = stringToNum(pDC);
				changeDataD->insertOrigin(myTriD);
			}
			changeSonPartData1->push_back(*changeDataD);
			delete changeDataD;
		}
		changeSonPartData1->pop_back();
		infileSonPartData1.close();

		while(!infileSonPartData2.eof())
		{
			ChangeData* changeDataD = new ChangeData;
			infileSonPartData2 >> dSize;
			infileSonPartData2 >> indexD;
			infileSonPartData2 >> indexAllD;
			infileSonPartData2 >> pDA; //first deletedTri
			infileSonPartData2 >> pDB;
			infileSonPartData2 >> pDC;
			myTriD.index = stringToNum(indexD);
			myTriD.allIndex = stringToNum(indexAllD);
			myTriD.p1 = stringToNum(pDA);
			myTriD.p2 = stringToNum(pDB);
			myTriD.p3 = stringToNum(pDC);
			changeDataD->insertNoActive(myTriD);
			infileSonPartData2 >> indexD;
			infileSonPartData2 >> indexAllD;
			infileSonPartData2 >> pDA;// second deletedTri
			infileSonPartData2>> pDB;
			infileSonPartData2 >> pDC;
			myTriD.index = stringToNum(indexD);
			myTriD.allIndex = stringToNum(indexAllD);
			myTriD.p1 = stringToNum(pDA);
			myTriD.p2 = stringToNum(pDB);
			myTriD.p3 = stringToNum(pDC);
			changeDataD->insertNoActive(myTriD);

			for(unsigned int i = 0 ; i < stringToNum(dSize) ; i++)
			{
				infileSonPartData2 >> indexD;
				infileSonPartData2 >> indexAllD;
				infileSonPartData2 >> pDA;// save changeTri
				infileSonPartData2 >> pDB;
				infileSonPartData2 >> pDC;
				myTriD.index = stringToNum(indexD);
				myTriD.allIndex = stringToNum(indexAllD);
				myTriD.p1 = stringToNum(pDA);
				myTriD.p2 = stringToNum(pDB);
				myTriD.p3 = stringToNum(pDC);
				changeDataD->insertChange(myTriD);
			}

			for(unsigned int j = 0 ; j < stringToNum(dSize) ; j++)
			{
				infileSonPartData2 >> indexD;
				infileSonPartData2 >> indexAllD;
				infileSonPartData2 >> pDA;// save origin
				infileSonPartData2 >> pDB;
				infileSonPartData2 >> pDC;
				myTriD.index = stringToNum(indexD);
				myTriD.allIndex = stringToNum(indexAllD);
				myTriD.p1 = stringToNum(pDA);
				myTriD.p2 = stringToNum(pDB);
				myTriD.p3 = stringToNum(pDC);
				changeDataD->insertOrigin(myTriD);
			}
			changeSonPartData2->push_back(*changeDataD);
			delete changeDataD;
		}
		changeSonPartData2->pop_back();
		infileSonPartData2.close();

		while(!infileSonPartData3.eof())
		{
			ChangeData* changeDataD = new ChangeData;
			infileSonPartData3 >> dSize;
			infileSonPartData3 >> indexD;
			infileSonPartData3 >> indexAllD;
			infileSonPartData3 >> pDA; //first deletedTri
			infileSonPartData3 >> pDB;
			infileSonPartData3 >> pDC;
			myTriD.index = stringToNum(indexD);
			myTriD.allIndex = stringToNum(indexAllD);
			myTriD.p1 = stringToNum(pDA);
			myTriD.p2 = stringToNum(pDB);
			myTriD.p3 = stringToNum(pDC);
			changeDataD->insertNoActive(myTriD);
			infileSonPartData3 >> indexD;
			infileSonPartData3 >> indexAllD;
			infileSonPartData3 >> pDA;// second deletedTri
			infileSonPartData3>> pDB;
			infileSonPartData3 >> pDC;
			myTriD.index = stringToNum(indexD);
			myTriD.allIndex = stringToNum(indexAllD);
			myTriD.p1 = stringToNum(pDA);
			myTriD.p2 = stringToNum(pDB);
			myTriD.p3 = stringToNum(pDC);
			changeDataD->insertNoActive(myTriD);

			for(unsigned int i = 0 ; i < stringToNum(dSize) ; i++)
			{
				infileSonPartData3 >> indexD;
				infileSonPartData3 >> indexAllD;
				infileSonPartData3 >> pDA;// save changeTri
				infileSonPartData3 >> pDB;
				infileSonPartData3 >> pDC;
				myTriD.index = stringToNum(indexD);
				myTriD.allIndex = stringToNum(indexAllD);
				myTriD.p1 = stringToNum(pDA);
				myTriD.p2 = stringToNum(pDB);
				myTriD.p3 = stringToNum(pDC);
				changeDataD->insertChange(myTriD);
			}

			for(unsigned int j = 0 ; j < stringToNum(dSize) ; j++)
			{
				infileSonPartData3 >> indexD;
				infileSonPartData3 >> indexAllD;
				infileSonPartData3 >> pDA;// save origin
				infileSonPartData3 >> pDB;
				infileSonPartData3 >> pDC;
				myTriD.index = stringToNum(indexD);
				myTriD.allIndex = stringToNum(indexAllD);
				myTriD.p1 = stringToNum(pDA);
				myTriD.p2 = stringToNum(pDB);
				myTriD.p3 = stringToNum(pDC);
				changeDataD->insertOrigin(myTriD);
			}
			changeSonPartData3->push_back(*changeDataD);
			delete changeDataD;
		}
		changeSonPartData3->pop_back();
		infileSonPartData3.close();

		/*newSonPartData(0);
		saveSonPartChangeData(0);
		newSonPartData(1);
		saveSonPartChangeData(1);
		newSonPartData(2);
		saveSonPartChangeData(2);
		newSonPartData(3);
		saveSonPartChangeData(3);*/


	}

	dataAllIndexSonPartChange();
	newAllSonPartData();
	/*******************
	创建顶点数组
	*************/
	osg::ref_ptr<osg::Vec3Array> vec = new osg::Vec3Array();
	osg::ref_ptr<osg::Vec3Array> vec1 = new osg::Vec3Array();
	for(unsigned int i = 0 ; i < triSonPart0->size() ; i++)
	{
		vec->push_back(osg::Vec3(pointPart0->at(triSonPart0->at(i).p1).getX() , pointPart0->at(triSonPart0->at(i).p1).getY() , 4*pointPart0->at(triSonPart0->at(i).p1).getZ()));
		vec->push_back(osg::Vec3(pointPart0->at(triSonPart0->at(i).p2).getX() , pointPart0->at(triSonPart0->at(i).p2).getY() , 4*pointPart0->at(triSonPart0->at(i).p2).getZ()));
		vec->push_back(osg::Vec3(pointPart0->at(triSonPart0->at(i).p3).getX() , pointPart0->at(triSonPart0->at(i).p3).getY() , 4*pointPart0->at(triSonPart0->at(i).p3).getZ()));
	}
	for(unsigned int j = 0 ; j < triSonPart1->size() ; j++)
	{
		vec->push_back(osg::Vec3(pointPart1->at(triSonPart1->at(j).p1).getX() , pointPart1->at(triSonPart1->at(j).p1).getY() , 4*pointPart1->at(triSonPart1->at(j).p1).getZ()));
		vec->push_back(osg::Vec3(pointPart1->at(triSonPart1->at(j).p2).getX() , pointPart1->at(triSonPart1->at(j).p2).getY() , 4*pointPart1->at(triSonPart1->at(j).p2).getZ()));
		vec->push_back(osg::Vec3(pointPart1->at(triSonPart1->at(j).p3).getX() , pointPart1->at(triSonPart1->at(j).p3).getY() , 4*pointPart1->at(triSonPart1->at(j).p3).getZ()));
	}
	for(unsigned int k = 0 ; k < triSonPart2->size() ; k++)
	{
		vec->push_back(osg::Vec3(pointPart2->at(triSonPart2->at(k).p1).getX() , pointPart2->at(triSonPart2->at(k).p1).getY() , 4*pointPart2->at(triSonPart2->at(k).p1).getZ()));
		vec->push_back(osg::Vec3(pointPart2->at(triSonPart2->at(k).p2).getX() , pointPart2->at(triSonPart2->at(k).p2).getY() , 4*pointPart2->at(triSonPart2->at(k).p2).getZ()));
		vec->push_back(osg::Vec3(pointPart2->at(triSonPart2->at(k).p3).getX() , pointPart2->at(triSonPart2->at(k).p3).getY() , 4*pointPart2->at(triSonPart2->at(k).p3).getZ()));
	}
	for(unsigned int l = 0 ; l < triSonPart3->size() ; l++)
	{
		vec->push_back(osg::Vec3(pointPart3->at(triSonPart3->at(l).p1).getX() , pointPart3->at(triSonPart3->at(l).p1).getY() , 4*pointPart3->at(triSonPart3->at(l).p1).getZ()));
		vec->push_back(osg::Vec3(pointPart3->at(triSonPart3->at(l).p2).getX() , pointPart3->at(triSonPart3->at(l).p2).getY() , 4*pointPart3->at(triSonPart3->at(l).p2).getZ()));
		vec->push_back(osg::Vec3(pointPart3->at(triSonPart3->at(l).p3).getX() , pointPart3->at(triSonPart3->at(l).p3).getY() , 4*pointPart3->at(triSonPart3->at(l).p3).getZ()));
	}
	for(unsigned int i = 0 ; i < convexZeroToOneTri->size() ; i++)
	{
		vec->push_back(osg::Vec3(convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p1).getX() , convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p1).getY() , 4*convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p1).getZ()));
		vec->push_back(osg::Vec3(convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p2).getX() , convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p2).getY() , 4*convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p2).getZ()));
		vec->push_back(osg::Vec3(convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p3).getX() , convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p3).getY() , 4*convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p3).getZ()));
	}
	
	for(unsigned int i = 0 ; i < convexTwoToThreeTri->size() ; i++)
	{
		vec->push_back(osg::Vec3(convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p1).getX() , convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p1).getY() , 4*convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p1).getZ()));
		vec->push_back(osg::Vec3(convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p2).getX() , convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p2).getY() , 4*convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p2).getZ()));
		vec->push_back(osg::Vec3(convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p3).getX() , convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p3).getY() , 4*convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p3).getZ()));
	}
	for(unsigned int i = 0 ; i < convexTopToButtomTri->size() ; i++)
	{
		vec->push_back(osg::Vec3(convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p1).getX() , convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p1).getY() , 4*convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p1).getZ()));
		vec->push_back(osg::Vec3(convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p2).getX() , convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p2).getY() , 4*convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p2).getZ()));
		vec->push_back(osg::Vec3(convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p3).getX() , convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p3).getY() , 4*convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p3).getZ()));
	}

	

	geom1->setVertexArray(vec.get());
	



	osg::TessellationHints* hints = new osg::TessellationHints;
	hints->setDetailRatio(0.5f);
	

	int num = (triSonPart0->size() + triSonPart1->size() + triSonPart2->size() + triSonPart3->size() + convexZeroToOneTri->size() + convexTwoToThreeTri->size() + convexTopToButtomTri->size());
	//int num = 3*(triSonPart0->size() + triSonPart1->size() + triSonPart2->size() + triSonPart3->size());
		//int num = 3*(convexZeroToOneTri->size() + convexTwoToThreeTri->size() + convexTopToButtomTri->size());
		//int num = 3*(triSonPart2->size() + triSonPart3->size());
		//int num = 3*(convexTopToButtomTri->size());
	geom1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES , 0 , 3*num));
	triNum = num;
	//geom1->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP , 0 , convexTopToButtomPoint->size()));
	geom1->setUseVertexBufferObjects(true);
	geom1->setUseDisplayList(false);
	geode1->addDrawable(geom1.get());
	//root->addChild(geode1.get());
	osg::ref_ptr<osg::StateSet> stateset=geode1->getOrCreateStateSet();  
	//osg::ref_ptr<osg::PolygonMode> polyMode=new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);  
	osg::ref_ptr<osg::PolygonMode> polyMode=new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);  
	stateset->setAttribute(polyMode);
	

	/*******************
	创建法线数组
	*******************/
	osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array();

	for(unsigned int i = 0 ; i < triSonPart0->size() ; i++)
	{
		nc->push_back(osg::Vec3(pointPart0->at(triSonPart0->at(i).p1).normal.X , pointPart0->at(triSonPart0->at(i).p1).normal.Y , pointPart0->at(triSonPart0->at(i).p1).normal.Z));
		nc->push_back(osg::Vec3(pointPart0->at(triSonPart0->at(i).p2).normal.X , pointPart0->at(triSonPart0->at(i).p2).normal.Y , pointPart0->at(triSonPart0->at(i).p2).normal.Z));
		nc->push_back(osg::Vec3(pointPart0->at(triSonPart0->at(i).p3).normal.X , pointPart0->at(triSonPart0->at(i).p3).normal.Y , pointPart0->at(triSonPart0->at(i).p3).normal.Z));
	}

	for(unsigned int j = 0 ; j < triSonPart1->size() ; j++)
	{
		nc->push_back(osg::Vec3(pointPart1->at(triSonPart1->at(j).p1).normal.X , pointPart1->at(triSonPart1->at(j).p1).normal.Y , pointPart1->at(triSonPart1->at(j).p1).normal.Z));
		nc->push_back(osg::Vec3(pointPart1->at(triSonPart1->at(j).p2).normal.X , pointPart1->at(triSonPart1->at(j).p2).normal.Y , pointPart1->at(triSonPart1->at(j).p2).normal.Z));
		nc->push_back(osg::Vec3(pointPart1->at(triSonPart1->at(j).p3).normal.X , pointPart1->at(triSonPart1->at(j).p3).normal.Y , pointPart1->at(triSonPart1->at(j).p3).normal.Z));
	}

	for(unsigned int k = 0 ; k < triSonPart2->size() ; k++)
	{
		nc->push_back(osg::Vec3(pointPart2->at(triSonPart2->at(k).p1).normal.X , pointPart2->at(triSonPart2->at(k).p1).normal.Y , pointPart2->at(triSonPart2->at(k).p1).normal.Z));
		nc->push_back(osg::Vec3(pointPart2->at(triSonPart2->at(k).p2).normal.X , pointPart2->at(triSonPart2->at(k).p2).normal.Y , pointPart2->at(triSonPart2->at(k).p2).normal.Z));
		nc->push_back(osg::Vec3(pointPart2->at(triSonPart2->at(k).p3).normal.X , pointPart2->at(triSonPart2->at(k).p3).normal.Y , pointPart2->at(triSonPart2->at(k).p3).normal.Z));
	}

	for(unsigned int l = 0 ; l < triSonPart3->size() ; l++)
	{
		nc->push_back(osg::Vec3(pointPart3->at(triSonPart3->at(l).p1).normal.X , pointPart3->at(triSonPart3->at(l).p1).normal.Y , pointPart3->at(triSonPart3->at(l).p1).normal.Z));
		nc->push_back(osg::Vec3(pointPart3->at(triSonPart3->at(l).p2).normal.X , pointPart3->at(triSonPart3->at(l).p2).normal.Y , pointPart3->at(triSonPart3->at(l).p2).normal.Z));
		nc->push_back(osg::Vec3(pointPart3->at(triSonPart3->at(l).p3).normal.X , pointPart3->at(triSonPart3->at(l).p3).normal.Y , pointPart3->at(triSonPart3->at(l).p3).normal.Z));
	}

	for(unsigned int i = 0 ; i < convexZeroToOneTri->size() ; i++)
	{
		nc->push_back(osg::Vec3(convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p1).normal.X , convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p1).normal.Y , convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p1).normal.Z));
		nc->push_back(osg::Vec3(convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p2).normal.X , convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p2).normal.Y , convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p2).normal.Z));
		nc->push_back(osg::Vec3(convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p3).normal.X , convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p3).normal.Y , convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p3).normal.Z));
	}

	for(unsigned int i = 0 ; i < convexTwoToThreeTri->size() ; i++)
	{
		nc->push_back(osg::Vec3(convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p1).normal.X , convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p1).normal.Y , convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p1).normal.Z));
		nc->push_back(osg::Vec3(convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p2).normal.X , convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p2).normal.Y , convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p2).normal.Z));
		nc->push_back(osg::Vec3(convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p3).normal.X , convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p3).normal.Y , convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p3).normal.Z));
	}

	for(unsigned int i = 0 ; i < convexTopToButtomTri->size() ; i++)
	{
		nc->push_back(osg::Vec3(convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p1).normal.X , convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p1).normal.Y , convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p1).normal.Z));
		nc->push_back(osg::Vec3(convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p2).normal.X , convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p2).normal.Y , convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p2).normal.Z));
		nc->push_back(osg::Vec3(convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p3).normal.X , convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p3).normal.Y , convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p3).normal.Z));
	}

	geom1->setNormalArray(nc.get());
	geom1->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	int sonPartPoint0MinX;
	int sonPartPoint0MaxX;
	int sonPartPoint0MinY;
	int sonPartPoint0MaxY;
	
	int sonPartPoint1MinX;
	int sonPartPoint1MaxX;
	int sonPartPoint1MinY;
	int sonPartPoint1MaxY;
	
	int sonPartPoint2MinX;
	int sonPartPoint2MaxX;
	int sonPartPoint2MinY;
	int sonPartPoint2MaxY;

	int sonPartPoint3MinX;
	int sonPartPoint3MaxX;
	int sonPartPoint3MinY;
	int sonPartPoint3MaxY;



	sonPartPoint0MinX = getMinXpoint(pointPart0);
	sonPartPoint0MaxX = getMaxXpoint(pointPart0);
	sonPartPoint0MinY = getMinYpoint(pointPart0);
	sonPartPoint0MaxY = getMaxYpoint(pointPart0);

	sonPartPoint1MinX = getMinXpoint(pointPart1);
	sonPartPoint1MaxX = getMaxXpoint(pointPart1);
	sonPartPoint1MinY = getMinYpoint(pointPart1);
	sonPartPoint1MaxY = getMaxYpoint(pointPart1);

	sonPartPoint2MinX = getMinXpoint(pointPart2);
	sonPartPoint2MaxX = getMaxXpoint(pointPart2);
	sonPartPoint2MinY = getMinYpoint(pointPart2);
	sonPartPoint2MaxY = getMaxYpoint(pointPart2);

	sonPartPoint3MinX = getMinXpoint(pointPart3);
	sonPartPoint3MaxX = getMaxXpoint(pointPart3);
	sonPartPoint3MinY = getMinYpoint(pointPart3);
	sonPartPoint3MaxY = getMaxYpoint(pointPart3);


	//unsigned int triSizeAll = triSonPart0->size() + triSonPart1->size() + triSonPart2->size() + triSonPart3->size();
	minXAll = min(pointPart0->at(sonPartPoint0MinX).getX() , pointPart2->at(sonPartPoint2MinX).getX());
	maxXAll = max(pointPart1->at(sonPartPoint1MaxX).getX() , pointPart3->at(sonPartPoint3MaxX).getX());
	minYAll = min(pointPart2->at(sonPartPoint2MinY).getY() , pointPart3->at(sonPartPoint3MinY).getY());
	maxYAll = max(pointPart0->at(sonPartPoint0MaxY).getY() , pointPart1->at(sonPartPoint1MaxY).getY());

	XSizeAll = maxXAll - minXAll;
	YSizeAll = maxYAll - minYAll;



	for(unsigned int i = 0 ; i < triSonPart0->size() ; i++)
	{
		double tx1 = (pointPart0->at(triSonPart0->at(i).p1).getX() - minXAll)/(XSizeAll);
		double ty1 = (pointPart0->at(triSonPart0->at(i).p1).getY() - minYAll)/(YSizeAll);
		double tx2 = (pointPart0->at(triSonPart0->at(i).p2).getX() - minXAll)/(XSizeAll);
		double ty2 = (pointPart0->at(triSonPart0->at(i).p2).getY() - minYAll)/(YSizeAll);
		double tx3 = (pointPart0->at(triSonPart0->at(i).p3).getX() - minXAll)/(XSizeAll);
		double ty3 = (pointPart0->at(triSonPart0->at(i).p3).getY() - minYAll)/(YSizeAll);
		texcoords->push_back(osg::Vec2(tx1 , ty1));										
		texcoords->push_back(osg::Vec2(tx2 , ty2));
		texcoords->push_back(osg::Vec2(tx3 , ty3));
	}

	for(unsigned int i = 0 ; i < triSonPart1->size() ; i++)
	{
		double tx1 = (pointPart1->at(triSonPart1->at(i).p1).getX() - minXAll)/(XSizeAll);
		double ty1 = (pointPart1->at(triSonPart1->at(i).p1).getY() - minYAll)/(YSizeAll);
		double tx2 = (pointPart1->at(triSonPart1->at(i).p2).getX() - minXAll)/(XSizeAll);
		double ty2 = (pointPart1->at(triSonPart1->at(i).p2).getY() - minYAll)/(YSizeAll);
		double tx3 = (pointPart1->at(triSonPart1->at(i).p3).getX() - minXAll)/(XSizeAll);
		double ty3 = (pointPart1->at(triSonPart1->at(i).p3).getY() - minYAll)/(YSizeAll);
		texcoords->push_back(osg::Vec2(tx1 , ty1));										
		texcoords->push_back(osg::Vec2(tx2 , ty2));
		texcoords->push_back(osg::Vec2(tx3 , ty3));
	}

	for(unsigned int i = 0 ; i < triSonPart2->size() ; i++)
	{
		double tx1 = (pointPart2->at(triSonPart2->at(i).p1).getX() - minXAll)/(XSizeAll);
		double ty1 = (pointPart2->at(triSonPart2->at(i).p1).getY() - minYAll)/(YSizeAll);
		double tx2 = (pointPart2->at(triSonPart2->at(i).p2).getX() - minXAll)/(XSizeAll);
		double ty2 = (pointPart2->at(triSonPart2->at(i).p2).getY() - minYAll)/(YSizeAll);
		double tx3 = (pointPart2->at(triSonPart2->at(i).p3).getX() - minXAll)/(XSizeAll);
		double ty3 = (pointPart2->at(triSonPart2->at(i).p3).getY() - minYAll)/(YSizeAll);
		texcoords->push_back(osg::Vec2(tx1 , ty1));										
		texcoords->push_back(osg::Vec2(tx2 , ty2));
		texcoords->push_back(osg::Vec2(tx3 , ty3));
	}

	for(unsigned int i = 0 ; i < triSonPart3->size() ; i++)
	{
		double tx1 = (pointPart3->at(triSonPart3->at(i).p1).getX() - minXAll)/(XSizeAll);
		double ty1 = (pointPart3->at(triSonPart3->at(i).p1).getY() - minYAll)/(YSizeAll);
		double tx2 = (pointPart3->at(triSonPart3->at(i).p2).getX() - minXAll)/(XSizeAll);
		double ty2 = (pointPart3->at(triSonPart3->at(i).p2).getY() - minYAll)/(YSizeAll);
		double tx3 = (pointPart3->at(triSonPart3->at(i).p3).getX() - minXAll)/(XSizeAll);
		double ty3 = (pointPart3->at(triSonPart3->at(i).p3).getY() - minYAll)/(YSizeAll);
		texcoords->push_back(osg::Vec2(tx1 , ty1));										
		texcoords->push_back(osg::Vec2(tx2 , ty2));
		texcoords->push_back(osg::Vec2(tx3 , ty3));
	}

	for(unsigned int i = 0 ; i < convexZeroToOneTri->size() ; i++)
	{
		double tx1 = (convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p1).getX() - minXAll)/(XSizeAll);
		double ty1 = (convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p1).getY() - minYAll)/(YSizeAll);
		double tx2 = (convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p2).getX() - minXAll)/(XSizeAll);
		double ty2 = (convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p2).getY() - minYAll)/(YSizeAll);
		double tx3 = (convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p3).getX() - minXAll)/(XSizeAll);
		double ty3 = (convexZeroToOnePoint->at(convexZeroToOneTri->at(i).p3).getY() - minYAll)/(YSizeAll);
		texcoords->push_back(osg::Vec2(tx1 , ty1));										
		texcoords->push_back(osg::Vec2(tx2 , ty2));
		texcoords->push_back(osg::Vec2(tx3 , ty3));
	}

	for(unsigned int i = 0 ; i < convexTwoToThreeTri->size() ; i++)
	{
		double tx1 = (convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p1).getX() - minXAll)/(XSizeAll);
		double ty1 = (convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p1).getY() - minYAll)/(YSizeAll);
		double tx2 = (convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p2).getX() - minXAll)/(XSizeAll);
		double ty2 = (convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p2).getY() - minYAll)/(YSizeAll);
		double tx3 = (convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p3).getX() - minXAll)/(XSizeAll);
		double ty3 = (convexTwoToThreePoint->at(convexTwoToThreeTri->at(i).p3).getY() - minYAll)/(YSizeAll);
		texcoords->push_back(osg::Vec2(tx1 , ty1));										
		texcoords->push_back(osg::Vec2(tx2 , ty2));
		texcoords->push_back(osg::Vec2(tx3 , ty3));
	}

	for(unsigned int i = 0 ; i < convexTopToButtomTri->size() ; i++)
	{
		double tx1 = (convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p1).getX() - minXAll)/(XSizeAll);
		double ty1 = (convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p1).getY() - minYAll)/(YSizeAll);
		double tx2 = (convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p2).getX() - minXAll)/(XSizeAll);
		double ty2 = (convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p2).getY() - minYAll)/(YSizeAll);
		double tx3 = (convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p3).getX() - minXAll)/(XSizeAll);
		double ty3 = (convexTopToButtomPoint->at(convexTopToButtomTri->at(i).p3).getY() - minYAll)/(YSizeAll);
		texcoords->push_back(osg::Vec2(tx1 , ty1));										
		texcoords->push_back(osg::Vec2(tx2 , ty2));
		texcoords->push_back(osg::Vec2(tx3 , ty3));
	}



	geom1->setTexCoordArray(0 , texcoords.get());
	geom1->getOrCreateStateSet()->setTextureAttributeAndModes(0 , texture , osg::StateAttribute::ON);

	/*******************
	材质选项渲染
	*****************/
	osg::ref_ptr<osg::Material> material = new osg::Material;
	//osg::Vec4 redColor(0.3f , 0.3f , 0.3f , 1.f);
	material->setAmbient(osg::Material::FRONT_AND_BACK , osg::Vec4(1.0f , 1.0f , 1.0f , 1.0f));//环境光
	material->setDiffuse(osg::Material::FRONT_AND_BACK , osg::Vec4(1.0f , 1.0f , 1.0f , 1.0f));//漫反射
	material->setSpecular(osg::Material::FRONT_AND_BACK , osg::Vec4(1.0f , 1.0f , 1.0f , 1.0f));//镜面反射
	material->setShininess(osg::Material::FRONT_AND_BACK ,0.6f);//光泽度
	material->setEmission(osg::Material::FRONT_AND_BACK , osg::Vec4(0.5f , 0.5f , 0.5f , 1.0f));//辐射光

	material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
	geom1->getOrCreateStateSet()->setAttributeAndModes(material.get() , osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	root->addChild(createLight(geode1.get() , rPoint) , true);
	


	
	
	


	
	root->addChild(hud->getHudCamera() , true);
	viewer->getCamera()->setClearColor(osg::Vec4f(0.184f, 0.310f , 0.310f , 1.0f)); 
	viewer->setSceneData(root.get());

	/***********************************************
	***********************************************/
	viewer->setRealizeOperation(new SysncOperation());
	viewer->addEventHandler(new KeyboradHandler(hud));
	viewer->addEventHandler(new ZoomManipulator(root));


	//viewer->addEventHandler(new osgViewer::HelpHandler);
	//viewer->addEventHandler(new ZoomManipulator(root));



	viewer->addEventHandler(new osgViewer::StatsHandler);
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
	
	viewer->realize();
	viewer->run();
	return 0;
}
