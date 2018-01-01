#ifndef _ROS_seven_dof_arm_test_GoalStatus_h
#define _ROS_seven_dof_arm_test_GoalStatus_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace seven_dof_arm_test
{

  class GoalStatus : public ros::Msg
  {
    public:
      uint8_t status;
      enum { PENDING =  0    };
      enum { ACTIVE =  1    };
      enum { PREEMPTED =  2    };
      enum { SUCCEEDED =  3    };
      enum { ABORTED =  4    };
      enum { REJECTED =  5    };
      enum { PREEMPTING =  6    };
      enum { RECALLING =  7    };
      enum { RECALLED =  8    };
      enum { LOST =  9    };

    GoalStatus():
      status(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->status >> (8 * 0)) & 0xFF;
      offset += sizeof(this->status);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
      this->status =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->status);
     return offset;
    }

    const char * getType(){ return "seven_dof_arm_test/GoalStatus"; };
    const char * getMD5(){ return "5854dd05dd2f507fe3114940c707bb59"; };

  };

}
#endif