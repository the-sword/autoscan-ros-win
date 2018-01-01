#ifndef _ROS_seven_dof_arm_test_exe_status_h
#define _ROS_seven_dof_arm_test_exe_status_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Header.h"
#include "seven_dof_arm_test/GoalStatus.h"

namespace seven_dof_arm_test
{

  class exe_status : public ros::Msg
  {
    public:
      std_msgs::Header header;
      uint8_t status_list_length;
      seven_dof_arm_test::GoalStatus st_status_list;
      seven_dof_arm_test::GoalStatus * status_list;

    exe_status():
      header(),
      status_list_length(0), status_list(NULL)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      offset += this->header.serialize(outbuffer + offset);
      *(outbuffer + offset++) = status_list_length;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      *(outbuffer + offset++) = 0;
      for( uint8_t i = 0; i < status_list_length; i++){
      offset += this->status_list[i].serialize(outbuffer + offset);
      }
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      offset += this->header.deserialize(inbuffer + offset);
      uint8_t status_list_lengthT = *(inbuffer + offset++);
      if(status_list_lengthT > status_list_length)
        this->status_list = (seven_dof_arm_test::GoalStatus*)realloc(this->status_list, status_list_lengthT * sizeof(seven_dof_arm_test::GoalStatus));
      offset += 3;
      status_list_length = status_list_lengthT;
      for( uint8_t i = 0; i < status_list_length; i++){
      offset += this->st_status_list.deserialize(inbuffer + offset);
        memcpy( &(this->status_list[i]), &(this->st_status_list), sizeof(seven_dof_arm_test::GoalStatus));
      }
     return offset;
    }

    const char * getType(){ return "seven_dof_arm_test/exe_status"; };
    const char * getMD5(){ return "7920851daee309c8ae663029a7713c82"; };

  };

}
#endif