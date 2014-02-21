// Lesson2SimpleWorld.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <osgViewer/Viewer>
#include <osg/Geode>
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>

int _tmain(int argc, _TCHAR* argv[])
{
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	osg::ref_ptr<osg::Group> root = new osg::Group();
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("mymap.earth");

	root->addChild(node.get());
	osgUtil::Optimizer optimizer;
	optimizer.optimize(root.get());
	viewer->setSceneData(root.get());
	viewer->realize();
	viewer->run();

	return 0;
}

