#include <iostream>
#include <string>

#include "ros/ros.h"
#include "my_server/calc_roots.h"

#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float32MultiArray.h"

ros::ServiceServer serv;
ros::Publisher pub;
ros::Subscriber sub;

bool createResponce(my_server::calc_roots::Request&  req,
                    my_server::calc_roots::Response& resp) 
  {
  resp.roots.layout.dim.push_back(std_msgs::MultiArrayDimension());
  resp.roots.layout.dim[0].label = "num_roots";
  resp.roots.layout.data_offset = 0;

  float d;
  d = req.b * req.b - 4 * req.c * req.a;
  if (req.a == 0.0) 
  {
    resp.roots.layout.dim[0].size = 1;
    resp.roots.layout.dim[0].stride = 1;
    resp.roots.data.push_back(-1 * req.c / req.b);
  } else if (d > 0.0) 
  {
    resp.roots.layout.dim[0].size = 2;
    resp.roots.layout.dim[0].stride = 2;
    resp.roots.data.push_back((-1 * req.b - sqrt(d)) / (2 * req.a));
    resp.roots.data.push_back((-1 * req.b + sqrt(d)) / (2 * req.a));
  } else if (d < -0.0) 
  {
    resp.roots.layout.dim[0].size = 0;
    resp.roots.layout.dim[0].stride = 0;
  } else 
  {
    resp.roots.layout.dim[0].size = 1;
    resp.roots.layout.dim[0].stride = 1;
    resp.roots.data.push_back((-1 * req.b) / (2 * req.a));

  }
  pub.publish(resp);
  return true;
}

void receive(const std_msgs::Float32MultiArray& arr) 
{
  std::string buf = "";
  for (int i = 0; i < arr.layout.dim[0].size; i++) 
  {
    buf += "x" + std::to_string(i + 1) + " = " + std::to_string(arr.data[i]) + ", ";
  }
  if (buf == "") 
  {
    buf = "No roots!";
  } 
  ROS_INFO_STREAM(buf);
}

int main(int argc, char** argv) 
{
  ros::init(argc, argv, "calc_roots");
  ros::init(argc, argv, "subscriber");
  ros::init(argc, argv, "publisher");
  ros::NodeHandle n;
  serv = n.advertiseService("calc_roots", createResponce);
  pub = n.advertise < std_msgs::Float32MultiArray > ("root_topic", 1000);
  ROS_INFO_STREAM("Ready to work");
  sub = n.subscribe("root_topic", 1000, receive);
  ros::spin();
  return 0;
}
