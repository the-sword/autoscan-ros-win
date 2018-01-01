#ifndef _ROS_custom_msg_Execute_Status_h
#define _ROS_custom_msg_Execute_Status_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace custom_msg
{

  class Execute_Status : public ros::Msg
  {
    public:
      uint8_t data;

    Execute_Status():
      data(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->data >> (8 * 0)) & 0xFF;
      offset += sizeof(this->data);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      this->data =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->data);
     return offset;
    }

    const char * getType(){ return "custom_msg/Execute_Status"; };
    const char * getMD5(){ return "7c8164229e7d2c17eb95e9231617fdee"; };

  };

}
#endif