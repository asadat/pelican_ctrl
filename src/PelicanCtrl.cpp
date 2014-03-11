
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <ros/ros.h>
#include <asctec_hl_comm/mav_ctrl.h>
#include <asctec_hl_comm/mav_ctrl_motors.h>

void usage()
{
  std::string text("usage: \n\n");
  text = "ctrl_test motors [0 | 1]\n";
  text += "ctrl_test ctrl [acc | vel | pos] x y z yaw\n";
  std::cout << text << std::endl;
}

int main(int argc, char ** argv)
{

  ros::init(argc, argv, "interfacetest");
  ros::NodeHandle nh;

  ros::Publisher pub;

  if (argc == 1)
  {
    ROS_ERROR("Wrong number of arguments!!!");
    usage();
    return -1;
  }

  std::string command = std::string(argv[1]);

  if (command == "motors")
  {
    if (argc != 3)
    {
      ROS_ERROR("Wrong number of arguments!!!");
      usage();
      return -1;
    }

    asctec_hl_comm::mav_ctrl_motors::Request req;
    asctec_hl_comm::mav_ctrl_motors::Response res;
    req.startMotors = atoi(argv[2]);
    ros::service::call("fcu/motor_control", req, res);
    std::cout << "motors running: " << (int)res.motorsRunning << std::endl;
  }
  else if (command == "ctrl")
  {
    if (argc != 7)
    {
      ROS_ERROR("Wrong number of arguments!");
      usage();
      return -1;
    }
    asctec_hl_comm::mav_ctrl msg;
    msg.x = atof(argv[3]);
    msg.y = atof(argv[4]);
    msg.z = atof(argv[5]);
    msg.yaw = atof(argv[6]);
    msg.v_max_xy = -1; // use max velocity from config
    msg.v_max_z = -1;

    std::string type(argv[2]);
    if (type == "acc")
      msg.type = asctec_hl_comm::mav_ctrl::acceleration;
    else if (type == "vel")
      msg.type = asctec_hl_comm::mav_ctrl::velocity;
    else if (type == "pos")
      msg.type = asctec_hl_comm::mav_ctrl::position;
    else
    {
      ROS_ERROR("Command type not recognized");
      usage();
      return -1;
    }

    pub = nh.advertise<asctec_hl_comm::mav_ctrl> ("fcu/control", 1);
    ros::Rate r(15); // ~15 Hz

    for (int i = 0; i < 15; i++)
    {
      pub.publish(msg);
      if (!ros::ok())
        return 0;
      r.sleep();
    }

    // reset
    if (type != "pos")
    {
      msg.x = 0;
      msg.y = 0;
      msg.z = 0;
      msg.yaw = 0;
    }

    for(int i=0; i<5; i++){
      pub.publish(msg);
      r.sleep();
    }

    ros::spinOnce();
  }

  return 0;
}
