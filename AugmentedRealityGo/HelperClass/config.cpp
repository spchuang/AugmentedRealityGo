#include "config.h"
using namespace std;



Config::Config(std::string config_file)
{

	boost::property_tree::ptree configTree;

	std::wifstream ifs(config_file.c_str());
    
    if( !ifs ) {
        std::cerr << "cannot open the config file: " << config_file<< std::endl;
		exit(1);
    }


	boost::property_tree::json_parser::read_json(config_file, configTree);

	//retrieve config properties
	//camera configuration
	cam.height = configTree.get<int>("camera.height");
	cam.width = configTree.get<int>("camera.width");

	boost::property_tree::ptree& i = configTree.get_child("camera.intrinsics");
	int j=0;
	BOOST_FOREACH(boost::property_tree::ptree::value_type& v,i){
		cam.intrinsic_array[j/3][j%3] = boost::lexical_cast<float>(v.second.data());
		j++;
	}



	i = configTree.get_child("camera.distortionCoeffs");
	j=0;
	BOOST_FOREACH(boost::property_tree::ptree::value_type& v,i){
		cam.distCoeffs_array[j] = boost::lexical_cast<float>(v.second.data());
		j++;
	}

	//board configuration
	board.numPoint		= configTree.get<int>("boardConfiguration.numPoint");
	board.widthGap		= configTree.get<double>("boardConfiguration.widthGap");
	board.heightGap		= configTree.get<double>("boardConfiguration.heightGap");
	board.boardHeight	= configTree.get<double>("boardConfiguration.boardHeight");
	board.boardWidth	= configTree.get<double>("boardConfiguration.boardWidth");
	board.boardLength	= configTree.get<double>("boardConfiguration.boardLength");
	board.gap			= configTree.get<double>("boardConfiguration.gap");
	board.widthInterval = configTree.get<double>("boardConfiguration.widthInterval");
	board.heightInterval = configTree.get<double>("boardConfiguration.heightInterval");
	board.blockLength	= configTree.get<double>("boardConfiguration.blockLength");

	//stone configuration
	stone.a		= configTree.get<float>("stoneConfiguration.a");
	stone.b		= configTree.get<float>("stoneConfiguration.b");
	stone.c		= configTree.get<float>("stoneConfiguration.c");

	//marker configuration
	marker.markerLength = configTree.get<double>("markerConfiguration.markerLength");
	i = configTree.get_child("markerConfiguration.markerID");

	BOOST_FOREACH(boost::property_tree::ptree::value_type& v,i){
		marker.boardMarkerID.push_back(boost::lexical_cast<int>(v.second.data()));
	}
	marker.xMarkerNumber		= configTree.get<int>("markerConfiguration.xMarkerNumber");
	marker.yMarkerNumber		= configTree.get<int>("markerConfiguration.yMarkerNumber");
	marker.totalMarkerNumber = 2*marker.xMarkerNumber + 2*marker.yMarkerNumber -4;


	int s =  configTree.get<int>("markerConfiguration.showMarkers");
	if(s==0) marker.showMarkers = false;
	else marker.showMarkers = true;
	//stone detection configuration
	board.whiteStoneThresh		= configTree.get<int>("stoneDetection.whiteThrehold");
	board.blackStoneThresh		= configTree.get<int>("stoneDetection.blackThrehold");

	//joseki setting
	joseki.dbFile = configTree.get<string>("joseki.databaseFileName");
	joseki.sgfDirectory = configTree.get<string>("joseki.sgfFolderPath");

	//fuego setting
	fuego.memoryLimit = configTree.get<int>("fuego.memory_limit");
}