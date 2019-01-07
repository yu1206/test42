#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include "fstream"
#include <direct.h>
#include <io.h>
#include <Windows.h>

using namespace std;
using namespace cv;

extern int readDir(char *dirName, vector<string> &filesName);

struct rect3
{
	int x1;
	int y1;
	int x2;
	int y2;
	int label;
};

struct imfo
{
	string name;
	int srcw;
	int srch;
	int srcow;
	int  srcoh;
	int stdw;
	int stdh;
	int stdow;
	int stdoh;
	float objpersrcw;
	float objpersrch;
};

//降序
bool GreaterSort(imfo a, imfo b)
{ 
	int sidea = min(a.stdow, a.stdoh);
	int sideb = min(b.stdow, b.stdoh);
	return (sidea>sideb);
}
//升序
bool LessSort(imfo a, imfo b)
{
	int sidea = max(a.stdow, a.stdoh);
	int sideb = max(b.stdow, b.stdoh);
	return (sidea<sideb);
}


int test42(int argc, char *argv[])
{

	//string imgpath = "G:/F/Traffic-signs/data/FullIJCNN2013/test";
	//string txtpath = "G:/F/Traffic-signs/data/FullIJCNN2013/test/0-per.txt";

	string imgpath = "G:/F/Traffic-signs/data/TrainIJCNN2013/image-gt";
	string txtpath = "G:/F/Traffic-signs/data/TrainIJCNN2013/image-gt/0-per.txt";

	fstream finWrite;
	finWrite.open(txtpath, ios::out);
	if (!finWrite.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	vector<string> v_img_; int num = 0;
	
	readDir((char*)imgpath.c_str(), v_img_);
	int short_size = 600; int max_long_size = 1000;

	int perminw = 10000; int perminh = 10000;
	int permaxw = 0; int permaxh = 0;
	vector<imfo>vfo;
	for (int i = 0; i<v_img_.size(); i++)
	{
		int npos = v_img_[i].find_last_of('/');
		int npos2 = v_img_[i].find_last_of('.');
		
		string name1 = v_img_[i].substr(npos + 1, npos2 - npos - 1);
		Mat img = imread(v_img_[i].c_str());
		if (img.data == NULL)
		{
			printf("img.data = NULL！\n");
			system("pause");
			continue;
		}

		
		int minside = min(img.cols, img.rows);
		int maxside = max(img.cols, img.rows);

		float scale_factor = static_cast<float>(short_size) / minside;

		if (scale_factor * maxside > max_long_size)
		{
			scale_factor = static_cast<float>(max_long_size) / maxside;
		}

		int stdw = scale_factor*img.cols;
		int stdh = scale_factor*img.rows;


		string str = imgpath + "/" + name1 + ".txt";
		fstream finRead1;
		finRead1.open(str, ios::in);
		if (!finRead1.is_open())
		{
			cout << "finRead 文件数据打开错误！" << endl;
			system("pause");
			return false;
		}

		string line; int num1 = 0;
		
		while (getline(finRead1, line))
		{

			if (line.empty())
			{
				break;
			}

			num1++;

			int label; int x1, y1, x2, y2;
			stringstream str_s(line);
			str_s >> label >> x1 >> y1 >> x2 >> y2;
			
			int objw = x2 - x1 + 1; int objh = y2 - y1 + 1;
			int stdow = objw*scale_factor;
			int stdoh = objh*scale_factor;
			float objperw = objw*1.0 / img.cols;
			float objperh = objh*1.0 / img.rows;

			imfo vi;
			vi.name = name1;
			vi.srcw = img.cols;
			vi.srch = img.rows;
			vi.srcow = objw;
			vi.srcoh = objh;
			vi.stdw = stdw;
			vi.stdh = stdh;
			vi.stdow = stdow;
			vi.stdoh = stdoh;
			vi.objpersrcw = objperw;
			vi.objpersrch = objperh;

			vfo.push_back(vi);


			if (stdow<perminw)
			{
				perminw = stdow;
			}
			if (stdoh < perminh)
			{
				perminh = stdoh;
			}

			if (stdow>permaxw)
			{
				permaxw = stdow;
			}
			if (stdoh > permaxh)
			{
				permaxh = stdoh;
			}

		}

		finRead1.close();

	}
	printf("perminw=%d,perminh=%d,permaxw=%d,permaxh=%d\n", perminw, perminh, permaxw , permaxh);

	sort(vfo.begin(), vfo.end(), LessSort);//升序排列

	for (int j = 0; j < vfo.size(); j++)
	{
		imfo vi = vfo[j];
		finWrite << vi.name  << " " << vi.srcw << " " <<vi.srch<< " " << vi.srcow << " " <<vi.srcoh<< " " <<
			vi.stdw << " " << vi.stdh << " " << vi.stdow << " " << vi.stdoh << " " <<vi.objpersrcw<<
			" " << vi.objpersrch << endl;
	}

	finWrite.close();

	return 0;
}


