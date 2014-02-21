// Lesson3-earthAPI.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions>
#include <osgEarthDrivers/feature_ogr/OGRFeatureOptions>
#include <osgEarthDrivers/cache_filesystem/FileSystemCache>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/GeodeticGraticule>
#include <osgEarthUtil/LatLongFormatter>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/MouseCoordsTool>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osg/PositionAttitudeTransform>
#include <osg/Group>
#include <osg/Node>
#include <osgDB/ReadFile>

using namespace osgEarth;
using namespace osgEarth::Drivers;


//初始化影像、地形
void initImageAndElevation(osgEarth::Map* map)
{
	//影像
	osgEarth::Drivers::TMSOptions imgOption;
	imgOption.url()="D:/CacheData/OUTDATA/JiLinImage/tms.xml";
	//imgOption.url()="http://readymap.org/readymap/tiles/1.0.0/7/";
	map->addImageLayer( new osgEarth::ImageLayer( "image", imgOption ) );
	//高程
	osgEarth::Drivers::TMSOptions elvOption;
	elvOption.url()="D:/CacheData/OUTDATA/JiLinElevation/tms.xml";
	//elvOption.url()="http://readymap.org/readymap/tiles/1.0.0/9/";
	map->addElevationLayer( new osgEarth::ElevationLayer( "elevation", elvOption ) );
}
//初始化省界国界
void initBoundaries(osgEarth::Map* map)
{
	osgEarth::Drivers::FeatureGeomModelOptions worldBoundaries;
	osgEarth::Drivers::OGRFeatureOptions ogrOptions;
	ogrOptions.url()="world.shp";
	worldBoundaries.compilerOptions()=ogrOptions;
	//worldBoundaries.styles().
	map->addModelLayer(new osgEarth::ModelLayer("world", worldBoundaries));
}
//初始化事件处理器
void initEventHandler(osgViewer::Viewer* viewer,osgEarth::MapNode* mapNode)
{
	//鼠标位置信息显示
	osgEarth::Util::Formatter* formatter = new osgEarth::Util::LatLongFormatter();
	osgEarth::Util::LabelControl* readout = new osgEarth::Util::LabelControl();
	osgEarth::Util::ControlCanvas::get( viewer, true )->addControl( readout );
	osgEarth::Util::MouseCoordsTool* tool = new osgEarth::Util::MouseCoordsTool( mapNode );
	tool->addCallback( new osgEarth::Util::MouseCoordsLabelCallback(readout, formatter) );
	viewer->addEventHandler( tool );
	// add the state manipulator
	viewer->addEventHandler( new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()) );
	// add the thread model handler
	viewer->addEventHandler(new osgViewer::ThreadingHandler);
	// add the window size toggle handler
	viewer->addEventHandler(new osgViewer::WindowSizeHandler);
	// add the stats handler
	viewer->addEventHandler(new osgViewer::StatsHandler);
	// add the record camera path handler
	viewer->addEventHandler(new osgViewer::RecordCameraPathHandler);
	// add the LOD Scale handler
	viewer->addEventHandler(new osgViewer::LODScaleHandler);
	// add the screen capture handler
	viewer->addEventHandler(new osgViewer::ScreenCaptureHandler);
}
//初始化操纵器
void initManipulator(osgViewer::Viewer* viewer,osgEarth::Map* map)
{
	//操纵器
	osgEarth::Util::EarthManipulator* em=new osgEarth::Util::EarthManipulator();
	//赤道半径
	double equatorRadius=map->getSRS()->getEllipsoid()->getRadiusEquator();//6378137.0
	//初始视点正对中国北京
	em->setHomeViewpoint(osgEarth::Util::Viewpoint(116.3,39.9,0,0,-90,equatorRadius*4));
	viewer->setCameraManipulator(em);
	//定位吉林
	em->setViewpoint(osgEarth::Util::Viewpoint(126,43,0,0,-90,5e4), 5);//5s
}
//初始化其他杂项
void initOther(osgViewer::Viewer* viewer,osgEarth::MapNode* mapNode)
{
	//反锯齿
	osg::DisplaySettings::instance()->setNumMultiSamples( 4 );
	//设置最大PagedLOD节点数目
	//viewer->getDatabasePager()->setTargetMaximumNumberOfPageLOD(10);
	//近地面自动裁剪AutoClipPlaneCullCallback
	viewer->getCamera()->addCullCallback( new osgEarth::Util::AutoClipPlaneCullCallback(mapNode) );
	//绘制经纬度网格
	//osgEarth::Util::GeodeticGraticule* gr = new osgEarth::Util::GeodeticGraticule( mapNode );
	//root->addChild(gr);
}


int _tmain(int argc, _TCHAR* argv[])
{
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer;
	//osgViewer::Viewer* viewer = new osgViewer::Viewer;
	osg::Group* root = new osg::Group();
	osg::Node* tankNode = NULL;
	osg::Vec3 tankPosit; 
	osg::PositionAttitudeTransform* tankXform;
	//缓存
	osgEarth::Drivers::FileSystemCacheOptions cacheOptions;
	cacheOptions.rootPath() = "./cache";
	osgEarth::MapOptions mapOptions;
	mapOptions.cache() = cacheOptions;
	//osgEarth::Map* map = new osgEarth::Map(mapOptions);
	//osgEarth::MapNode* mapNode = new osgEarth::MapNode( map );
	osg::Node* configEarth = osgDB::readNodeFile("config.earth");
	osgEarth::MapNode* mapNode = osgEarth::MapNode::findMapNode( configEarth );
	osgEarth::Map* map = mapNode->getMap();
	//关闭光照
	mapNode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
	viewer->setSceneData( root );
	root->addChild(mapNode);
	//读取模型
	tankNode = osgDB::readNodeFile("Models/t72-tank/t72-tank_des.flt");
	tankXform = new osg::PositionAttitudeTransform();
	root->addChild(tankXform);
	tankXform->addChild(tankNode);
	tankPosit.set(5,0,0);
	tankXform->setPosition( tankPosit ); 
	viewer->setCameraManipulator(new osgGA::TrackballManipulator());

	//地形影像
	//initImageAndElevation(map);

	//矢量
	//initBoundaries(map);

	//操纵器
	initManipulator(viewer, map);

	//事件处理（键盘鼠标）
	initEventHandler(viewer, mapNode);

	//其他
	initOther(viewer, mapNode);

	viewer->realize();

	while( !viewer->done() )
	{
		viewer->frame();
	} 

	return 0;
}

