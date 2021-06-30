#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>


ros::ServiceClient client;



void drive_bot(float lin_x, float ang_z)
{

  ball_chaser::DriveToTarget srv;
  srv.request.linear_x = lin_x;
  srv.request.angular_z = ang_z;
  client.call(srv);
}


void process_image_callback(const sensor_msgs::Image img)
{

  int white_pixel = 255;
  int start_size = img.data.size() / 3;
  int end_size = img.data.size() * 2 / 3;
  int NumOfWhitePixel = 0;
  int posX_sum = 0;


  for (int i=start_size; (i+2) < end_size; i+=3) 
  {

    int red   = img.data[i];
    int green = img.data[i+1];
    int blue  = img.data[i+2];

    if (red == white_pixel && green == white_pixel && blue == white_pixel)
    {
      int posX = (i % (img.width * 3)) / 3;
      posX_sum += posX;
      NumOfWhitePixel += 1;
    }
  }

  if (NumOfWhitePixel == 0)
  {
    // Stop(Nothing)
    drive_bot(0.0, 0.0);
  }
  else
  {
    int PosX_mean = posX_sum / NumOfWhitePixel;
    if (PosX_mean < img.width / 3)
    {
      // Left
      drive_bot(0.5, 0.5);
    }
    else if (PosX_mean > img.width * 2 / 3)
    {
      // Forward
      drive_bot(0.5, -0.5);
    }
    else
    {
      // Right
      drive_bot(0.5, 0.0);
    }
  }
}

int main(int argc, char** argv)
{
  // Initialize the process_image node and create a handle to it
  ros::init(argc, argv, "process_image");
  ros::NodeHandle n;

  // Define a client service capable of requesting services from command_robot
  client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

  // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
  ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

  // Handle ROS communication events
  ros::spin();

  return 0;
}
