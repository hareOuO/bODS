// #define GOOGLE_GLOG_DLL_DECL
#include "src/client.hpp"
// #include "glog/logging.h"
#include <vector>
#include <iostream>
using namespace std;

// #define NDEBUG 0

// void logSet()
// {
// 	FLAGS_logtostderr = false;
// 	FLAGS_log_prefix = true;
// 	FLAGS_logbufsecs = 0;
// 	FLAGS_max_log_size = 10;
// 	FLAGS_stop_logging_if_full_disk = true;
// 	FLAGS_stderrthreshold = 2;
// 	FLAGS_minloglevel = 0;
// }

int main()
{
	// logSet();
	// #if NDEBUG
	// 	std::cout << "Build-RELASE" << std::endl;
	// 	FLAGS_alsologtostderr = false;
	// #else
	// 	std::cout << "Build-DEBUG" << std::endl;
	// 	//设置日志消息除日志文件之外同时输出到标准输出
	// 	FLAGS_alsologtostderr = false;
	// 	//设置记录到标准输出的颜色消息（如果终端支持）
	// 	// FLAGS_colorlogtostderr = true;
	// #endif
	// FLAGS_log_dir = "log";

	// google::InitGoogleLogging("client");                // 全局初始化glog，argv[0]是程序名
    // google::SetStderrLogging(google::GLOG_INFO);       // 设置glog的输出级别，这里的含义是输出INFO级别以上的信息
    // // 设置INFO级别以上的信息log文件的路径和前缀名
    // google::SetLogDestination(google::GLOG_INFO, "log/info_");
 
    // // 设置WARNING级别以上的信息log文件的路径和前缀名
    // google::SetLogDestination(google::GLOG_WARNING, "log/warn_");
 
    // // 设置ERROR级别以上的信息log文件的路径和前缀名
    // google::SetLogDestination(google::GLOG_ERROR, "log/errr_");
	const int L=4;
	vector<bytes<Key>> key1(L-3);
	for(int i = 0;i < L-3;i++)
	{
		bytes<Key> tempkey{(uint8_t)i};
		key1[i] = tempkey;
	}
	bytes<Key> key2{1};
    bytes<Key> key3{2};
	// LOG(INFO) << "[client]init begin";
	std::cout<<"init begin"<<std::endl;

	client* my_client = new client(10000, key1, key2, key3);//vector
	my_client->init();
	// LOG(INFO) << "[client]init finished";
	std::cout<<"init finished!"<<std::endl;

	// read csv file
	ifstream inFile("../key_value.csv", ios::in);
    if (!inFile)
    {
        cout << "打开文件失败！" << endl;
        exit(1);
    }
    int i = 0;
    string line;
    while (getline(inFile, line)) //按行读取CSV文件中的数据
    {
        string key, value;
        istringstream sin(line);

        getline(sin, key, ',');
        getline(sin, value, '\r');
        // cout << key << " " << value << endl;
		if(i > 0)
		{
			// std::cout << "begin insert (" << key << "," << value << ")";
			my_client->insert_map(key, stoi(value));
		}
		// my_map->printTree();
		
		
		// finddd
		// Bid findkey("key");
		// string ret = my_map->find(findkey);
		// LOG(INFO) << "find \"key\" and the value is:" << ret;
        i++;
		if(i == 10000) break;
    }
	int ret = my_client->search_map("Manson-1");
	//std::cout << "find Manson-1 and the value is:" << ret;
	my_client->sendend();

	int stash_size = my_client->leafcache.size()*sizeof(leafNode) + my_client->mid1cache.size() * sizeof(midNode1) + my_client->mid2cache.size() * sizeof(midNode2);
	//std::cout << "stash大小:" << stash_size * 8;

	// google::ShutdownGoogleLogging();  
	return 0;
}
