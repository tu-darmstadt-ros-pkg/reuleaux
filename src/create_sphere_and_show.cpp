#include<ros/ros.h>
#include "reuleaux/PointArray.h"
#include "geometry_msgs/Point.h"
#include<octomap/octomap.h>
#include<octomap/MapCollection.h>
#include <octomap/math/Utils.h>
#include <reuleaux/sphere_discretization.h>
using namespace octomap;
using namespace std;
using namespace octomath;
using namespace sphere_discretization;



int main(int argc, char **argv)
{
    ros::init(argc, argv, "rand_points");
    ros::NodeHandle n;
  
    ros::Publisher point_pub = n.advertise<reuleaux::PointArray>("points", 1000);

  //ros::Rate loop_rate(10);
  ros::Rate poll_rate(100);
while(point_pub.getNumSubscribers() == 0)
    poll_rate.sleep();
  int count = 0;
  while (ros::ok())
  {
      
     float HI=-1.5, LO=1.5;
     unsigned char maxDepth = 16;
     unsigned char minDepth = 0;
    

    

    SphereDiscretization sd;
    float r=1;
    float resolution =0.1;
    point3d origin=point3d(0,0,0);
    OcTree* tree=sd.generateBoxTree(origin, r, resolution);
    
    std::vector<point3d> newData;
    for (OcTree::leaf_iterator it=tree->begin_leafs(maxDepth), end=tree->end_leafs();it !=end;++it){
	
	newData.push_back(it.getCoordinate());
	
		
 	}
    
    ROS_INFO("The # of leaf nodes in first tree: [%zd]",tree->getNumLeafNodes());
    ROS_INFO("The # of data in the array: [%zd]",newData.size());
   
    
    float radius=resolution;
    std::vector<Pointcloud> cloud_coll;
    
    for (int i=0;i<newData.size();i++){
	Pointcloud cloud;
	cloud=sd.make_sphere_points(newData[i],radius);
	cloud_coll.push_back(cloud);

	}
    
    Pointcloud whole_cloud;
    for (int i=0;i<cloud_coll.size();i++){
		Pointcloud p=cloud_coll[i];
		for (int j=0;j<p.size();j++){
			whole_cloud.push_back(p[j]);
		}
	}
    
    
    reuleaux::PointArray po;
    
    for(int i=0;i<newData.size();++i){
      geometry_msgs::Point32 p;
      p.x=newData[i].x();
      p.y=newData[i].y();
      p.z=newData[i].z();
      po.points.push_back(p);
    }


    
    
    point_pub.publish(po);

    ros::spinOnce();
    sleep(10000);
    //loop_rate.sleep();
    ++count;
  }
  
  return 0;
}
