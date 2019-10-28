#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/Twist.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::string imu_topic = "/mobile_base/sensors/imu_data";

using namespace ros;
using namespace std;

//======================================================================================================================
class CSVWriter
{
    std::string file_name;
    std::string delim;
    std::ofstream csv_file;

    int cur_row_index;

public:
    CSVWriter( const std::string fname , const std::string d=","): file_name(fname), delim(d), cur_row_index(0)
    {
        csv_file.open( file_name.c_str(), ios::out | ios::binary );
    }

    bool IsOpen() { return csv_file.is_open(); }

    template<typename T>
    void AppendItem( T item )
    {
        if ( cur_row_index++ > 0 )
        {
            csv_file << delim;
        }

        csv_file << item;
    }

    void EndEntry()
    {
        cur_row_index = 0;
        csv_file << "\n";
    }

    ~CSVWriter() { csv_file.close(); }
};

CSVWriter csv_writer("imu_data_output.csv");

//======================================================================================================================
void ImuCallback( const sensor_msgs::Imu::ConstPtr& imu_msg )// TODO: find the right message type
{
    if ( !csv_writer.IsOpen() )
    {
        ROS_FATAL("ERROR: writing to a file that is not open" );
    }

    csv_writer.AppendItem( imu_msg->header.stamp.toSec() );
    csv_writer.AppendItem( imu_msg->orientation.x );
    csv_writer.AppendItem( imu_msg->orientation.y );
    csv_writer.AppendItem( imu_msg->orientation.z );
    csv_writer.AppendItem( imu_msg->orientation.w );
    csv_writer.AppendItem( imu_msg->angular_velocity.x );
    csv_writer.AppendItem( imu_msg->angular_velocity.y );
    csv_writer.AppendItem( imu_msg->angular_velocity.z );
    csv_writer.AppendItem( imu_msg->linear_acceleration.x );
    csv_writer.AppendItem( imu_msg->linear_acceleration.y );
    csv_writer.AppendItem( imu_msg->linear_acceleration.z );
    csv_writer.EndEntry();
    //ROS_INFO("IMU DATA here"); // TODO: output to CSV file, not .txt file
}

//======================================================================================================================
void InsertHeader()
{
    csv_writer.AppendItem( "Time Stamp" );
    csv_writer.AppendItem( "Orientation (x)" );
    csv_writer.AppendItem( "Orientation (y)" );
    csv_writer.AppendItem( "Orientation (z)" );
    csv_writer.AppendItem( "Orientation (w)" );
    csv_writer.AppendItem( "Angular Velocity (x)" );
    csv_writer.AppendItem( "Angular Velocity (y)" );
    csv_writer.AppendItem( "Angular Velocity (z)" );
    csv_writer.AppendItem( "Linear Accel (x)" );
    csv_writer.AppendItem( "Linear Accel (y)" );
    csv_writer.AppendItem( "Linear Accel (z)" );
    csv_writer.EndEntry();
}

//======================================================================================================================
void WaitForClock()
{
    while (Time::now().toSec() <= 0)
    ;
}

//======================================================================================================================
int main ( int argc, char** argv )
{
    init( argc, argv, "record_imu_data" );

    InsertHeader();

    NodeHandle nh;
    Time::init();
    ros::Rate( 1 ); // runs at 1 Hz
    WaitForClock();

    Subscriber sub = nh.subscribe( imu_topic, 10, ImuCallback );
    if ( !sub )
    {
        ROS_FATAL("Subscriber failed. Exiting now...");
    }
    spin();

}
