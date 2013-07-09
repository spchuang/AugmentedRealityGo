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

	//marker configuration
	marker.markerLength = configTree.get<double>("MarkerConfiguration.markerLength");
	i = configTree.get_child("MarkerConfiguration.markerID");
	j=0;
	BOOST_FOREACH(boost::property_tree::ptree::value_type& v,i){
		marker.boardMarkerID[j] = boost::lexical_cast<float>(v.second.data());
		j++;
	}

}