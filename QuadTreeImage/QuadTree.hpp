#pragma once
#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;

class QuadTree
{
public:
	QuadTree(cv::Mat src, int area, float threshold);
	~QuadTree();

	cv::Mat getResultMat();
	int i;
	int num;
	int cnt;
private:
	cv::Mat img;
	cv::Mat dst;

	uchar* sImage;
	uchar* dImage;
	int minArea;
	float thr;



public:
	struct Node {
		int x, y, width, height;
		Node* Children;
	};

	Node root;
	Node getTree();
	int getNum();
	int getCnt();

private:
	cv::Point3f average(int x, int y, int width, int height);
	float measureDetail(int x, int y, int width, int height);
	void createQuadTree(Node node);
	void divideNode(Node& node);
	void coloring(cv::Point3f avg, int x, int y, int width, int height);
	void TestShowImage(int x, int y, int width, int height);
	

};