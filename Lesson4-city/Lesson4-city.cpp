// Lesson4-city.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <osg/ArgumentParser>


int _tmain(int argc, _TCHAR* argv[])
{
	osg::ArgumentParser arguments(&argc,argv);

	// create the map.

	Map* map = new Map();




	// add a TMS imagery layer:

	TMSOptions imagery;

	imagery.url() = "http://readymap.org/readymap/tiles/1.0.0/22/";

	map->addImageLayer( new ImageLayer("ReadyMap imagery", imagery) );




	// create a feature source to load the building footprint shapefile.

	OGRFeatureOptions feature_opt;

	feature_opt.name() = "buildings";

	feature_opt.url() = "../data/boston_buildings_utm19.shp";

	feature_opt.buildSpatialIndex() = true;



	// a style for the building data:

	Style buildingStyle;

	buildingStyle.setName( "default" );




	ExtrusionSymbol* extrusion = buildingStyle.getOrCreate();

	extrusion->heightexpression_r() = Numericexpression_r( "3.5 * max( [story_ht_], 1 )" );

	extrusion->flatten() = true;

	extrusion->wallStyleName() = "building-wall";

	extrusion->roofStyleName() = "building-roof";




	// a style for the wall textures:

	Style wallStyle;

	wallStyle.setName( "building-wall" );

	SkinSymbol* wallSkin = wallStyle.getOrCreate();

	wallSkin->libraryName() = "us_resources";

	wallSkin->addTag( "building" );

	wallSkin->randomSeed() = 1;




	// a style for the rooftop textures:

	Style roofStyle;

	roofStyle.setName( "building-roof" );

	SkinSymbol* roofSkin = roofStyle.getOrCreate();

	roofSkin->libraryName() = "us_resources";

	roofSkin->addTag( "rooftop" );

	roofSkin->randomSeed() = 1;

	roofSkin->isTiled() = true;




	// assemble a stylesheet and add our styles to it:

	StyleSheet* styleSheet = new StyleSheet();

	styleSheet->addStyle( buildingStyle );

	styleSheet->addStyle( wallStyle );

	styleSheet->addStyle( roofStyle );



	// load a resource library that contains the building textures.

	ResourceLibrary* reslib = 

		new ResourceLibrary( "us_resources", "../data/resources/textures_us/catalog.xml" );

	styleSheet->addResourceLibrary( reslib );




	// set up a paging layout for incremental loading.

	FeatureDisplayLayout layout;

	layout.tileSizeFactor() = 45.0;

	layout.addLevel( FeatureLevel(0.0f, 20000.0f) );




	// create a model layer that will render the buildings according to our style sheet.

	FeatureGeomModelOptions fgm_opt;

	fgm_opt.featureOptions() = feature_opt;

	fgm_opt.styles() = styleSheet;

	fgm_opt.layout() = layout;




	map->addModelLayer( new ModelLayer( "buildings", fgm_opt ) );




	// initialize a viewer:

	osgViewer::Viewer viewer(arguments);

	EarthManipulator* manip = new EarthManipulator();

	viewer.setCameraManipulator( manip );




	// make the map scene graph:

	osg::Group* root = new osg::Group();

	viewer.setSceneData( root );




	MapNode* mapNode = new MapNode( map );

	root->addChild( mapNode );



	// Process cmdline args

	MapNodeHelper helper;

	helper.configureView( &viewer );

	//在configureView中加入了

	// add some stock OSG handlers:

	//view->addEventHandler(new osgViewer::StatsHandler());

	//view->addEventHandler(new osgViewer::WindowSizeHandler());

	//view->addEventHandler(new osgViewer::ThreadingHandler());

	//view->addEventHandler(new osgViewer::LODScaleHandler());

	//view->addEventHandler(new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet()));




	// osgEarth benefits from pre-compilation of GL objects in the pager. In newer versions of

	// OSG, this activates OSG's IncrementalCompileOpeartion in order to avoid frame breaks.

	//view->getDatabasePager()->setDoPreCompile( true );




	helper.parse(mapNode, arguments, &viewer, root, new LabelControl("City Demo"));

	//处理控制台命令

	// zoom to a good startup position

	manip->setViewpoint( Viewpoint(-71.0763, 42.34425, 0, 24.261, -21.6, 3450.0), 5.0 );




	viewer.getDatabasePager()->setDoPreCompile( true );

	viewer.getCamera()->addCullCallback( new AutoClipPlaneCullCallback(mapNode) );




	return viewer.run();
	return 0;
}

