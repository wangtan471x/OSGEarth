// Lesson2SimpleWorld.cpp : �������̨Ӧ�ó������ڵ㡣
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

