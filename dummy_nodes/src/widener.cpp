//
// Created by rfeldhans on 22.08.19.
//

// esiaf include
#include <esiaf_ros/esiaf_ros.h>

// ros includes
#include "ros/ros.h"
#include "esiaf_ros/GenderInfo.h"


boost::function<void(const std::vector<int8_t> &, const esiaf_ros::RecordingTimeStamps &)> simple_esiaf_callback;
void esiaf_handler(const std::vector<int8_t> &signal, const esiaf_ros::RecordingTimeStamps & timeStamps){ simple_esiaf_callback(signal, timeStamps); };

int main(int argc, char *argv[]) {

    // ros initialization
    ros::init(argc, argv, "widener");
    ros::NodeHandle n;


    //////////////////////////////////////////////////////
    // esiaf start
    //////////////////////////////////////////////////////

    // initialise esiaf
    ROS_INFO("starting esiaf initialisation...");
    esiaf_ros::Esiaf_Handler handler(&n, esiaf_ros::NodeDesignation::Other);

    //create format for input topic
    esiaf_ros::EsiafAudioTopicInfo inputTopicInfo;

    esiaf_ros::EsiafAudioFormat allowedFormat;
    allowedFormat.rate = esiaf_ros::Rate::RATE_16000;
    allowedFormat.channels = 1;
    allowedFormat.bitrate = esiaf_ros::Bitrate::BIT_INT_16_SIGNED;
    allowedFormat.endian = esiaf_ros::Endian::LittleEndian;

    inputTopicInfo.allowedFormat = allowedFormat;
    inputTopicInfo.topic = "/esiaf_ros/segmented";

    ros::Publisher gender_pub = n.advertise<esiaf_ros::GenderInfo>("widener/gender", 1, true);


    // notify esiaf about the input topic

    // here we will segment the audio we acquire from esiaf
    simple_esiaf_callback = [&](const std::vector<int8_t> &signal,
                                const esiaf_ros::RecordingTimeStamps &timeStamps){

        esiaf_ros::GenderInfo g_info;
        g_info.gender = "unknown";
        g_info.duration = timeStamps;
        g_info.probability = 1.0;

        gender_pub.publish(g_info);
    };

    // add input topic
    handler.add_input_topic(inputTopicInfo, esiaf_handler);

    // start esiaf
    ROS_INFO("starting esiaf...");

    handler.start_esiaf();

    //////////////////////////////////////////////////////
    // esiaf initialisation finished
    //////////////////////////////////////////////////////


    ROS_INFO("Node ready and rockin'");

    while (ros::ok()) {
        ros::spinOnce();
    }

    handler.quit_esiaf();
    exit(0);
}
