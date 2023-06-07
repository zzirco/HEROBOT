#include "next_goal.h"

point::point()
{
    ros::NodeHandle nh;

    //publish
    goal_pub = nh.advertise<geometry_msgs::PoseStamped>("/move_base_simple/goal",10);
    //subscibe
    goal_sub = nh.subscribe("/move_base_simple/goal",10,&point::goal_callback,this);
    current_sub = nh.subscribe("/amcl_pose",10,&point::amcl_callback,this);
    state_sub = nh.subscribe("/emergency_state",10,&point::state_callback,this);
    //point
    node[0]={0.497762962173, -0.308064600171, 1, 0};
    node[1]={2.45057215145, 0.0181094133518, 0.87, 0.49};
    node[2]={2.9506921622, 1.34924363663, 0.71, 0.70};
    node[3]={3.07424021866, 2.82286608539, 0.45, 0.89};
    node[4]={1.52560869177, 3.65735450706, 0, 1};
    node[5]={0.263203667222, 3.6882641403, 0, 1};
    node[6]={-1.11006552549, 3.4655583391, -0.47, 0.88};
    node[7]={-2.12430806264, 2.14179120734, 0.70, -0.71};
    node[8]={-2.08565683185, 0.780913984384, 0.92, -0.39};
    node[9]={-0.875144404987, -0.0622035756579, 1, 0};
}

void point::goal_callback(const geometry_msgs::PoseStamped& msg)
{
    goal = msg;
    xp1 = goal.pose.position.x;
    yp1 = goal.pose.position.y;
    state = true;
}

void point::amcl_callback(const geometry_msgs::PoseWithCovarianceStamped& msg)
{
    amcl = msg;
    xp2 = amcl.pose.pose.position.x;
    yp2 = amcl.pose.pose.position.y;
}

void point::state_callback(const std_msgs::String& msg)
{
    emer_state = msg.data;
}
float point::calcDistance()
{
    codist = (xp1-xp2)*(xp1-xp2)+(yp1-yp2)*(yp1-yp2);
    dist = sqrt(codist);
    std::cout<<"dist : "<<dist<<std::endl;
    return dist;
}

Node point::nextGoal()
{
    float min = 10000;
    
    if(first_state == false)
    {
        for(int i=0; i<10; i++)
        {
            codistance = (node[i].x-amcl.pose.pose.position.x)*(node[i].x-amcl.pose.pose.position.x)+(node[i].y-amcl.pose.pose.position.y)*(node[i].y-amcl.pose.pose.position.y);
            distance = sqrt(codistance);
            if(distance < min)
            {
                min = distance;
                min_index = i;
            }
        }
        first_state = true;
        std::cout<<"GOOD"<<std::endl;
        return node[min_index + 1];
    }
    else
    {
        std::cout<<"min_index : "<<min_index<<std::endl;
        min_index = min_index + 1;
        if(min_index > 9)
        {    
            min_index = 0;
            return node[min_index];
        }
        else
        {
           
            return node[min_index];
        }
    }
}

void point::pub_goal()
{

    Node nextnode;
 
    nextnode = nextGoal();

    next_goal.pose.position.x = nextnode.x;
    next_goal.pose.position.y = nextnode.y;

    next_goal.pose.orientation.w = nextnode.w;
    next_goal.pose.orientation.z = nextnode.z;
 
    next_goal.header.frame_id = "map";

    goal_pub.publish(next_goal);

}

void point::isPub()
{
    std::cout<<"min_index : "<<min_index<<std::endl;
    if(state == true)
    {
        float d;
        d=calcDistance();
        if(d<=1.0)
        {
            pub_goal();
        }
        else
        {
            ;
        }
    }
    else
    {
        ;
    }
}

string point::get_state()
{
    return emer_state;
}

int main(int argc, char * argv[])
{
    ros::init(argc, argv, "next_goal");
    ros::NodeHandle nh;
    ros::NodeHandle nh_priv("~");

    point p;

    ros::Rate  loop_rate(10);

    while(ros::ok())
    {
        p.isPub();
        loop_rate.sleep();
        ros::spinOnce();
    }

    ros::spin();
    return 0;
}
