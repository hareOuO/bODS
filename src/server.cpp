#include "server.hpp"
server::server()
{
	vector<bytes<Key>> key1(L-3);
	for(int i = 0;i < L-3;i++)
	{
		bytes<Key> tempkey{(uint8_t)i};
		key1[i] = tempkey;
	}
	bytes<Key> key2{1};
    bytes<Key> key3{2};
    
    for(int i = 0; i < L - 3; i++)
    {
        ORAM<midNode1> *oram_temp = new ORAM<midNode1>(P1*pow(P2,i), key1[i]);
        oram1.push_back(oram_temp);
    }
    // oram1 = new ORAM<midNode1>(P1, key1);
    oram2 = new ORAM<midNode2>(P1 * pow(P2,L-3), key2);
    oram3 = new ORAM<leafNode>(P1 * pow(P2,L-2), key3);
}

server::~server()
{
}

void server::init()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        std::cout << "ERROR socket";
        //std::cout << "Error: socket" << std::endl;
        return;
    }
    // bind
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cout << "ERROR bind";
        //std::cout << "Error: bind" << std::endl;
        return;
    }
    // listen
    if(listen(listenfd, 5) == -1) {
        std::cout << "ERROR listen";
        //std::cout << "Error: listen" << std::endl;
        return;
    }
    // accept
    char clientIP[INET_ADDRSTRLEN] = "";
    socklen_t clientAddrLen = sizeof(clientAddr);
    std::cout << "...listening...";
    //std::cout << "...listening" << std::endl;
    conn = accept(listenfd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (conn < 0) {
        std::cout << "ERROR accept";
        //std::cout << "Error: accept" << std::endl;
        return;
    }
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    std::cout << "...connect " << clientIP << ":" << ntohs(clientAddr.sin_port);
    //std::cout << "...connect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;

    for (int i = 0; i < L - 3; i++)
    {
        oram1[i]->conn = conn;
    }
    
    oram2->conn = conn;
    oram3->conn = conn;
    char buf[102400];

    vector<int> recv_count1(L - 3, 0);
    int recv_count2 = 0;
    int recv_count3 = 0;
    clock_t begin_time, end_time;
    begin_time = clock();
    while (true)
    {
        memset(buf, 0, sizeof(buf));
        int len = recv(conn, buf, sizeof(buf), 0);
	    if (len < 0) std::cout << "receive Failed";
        #if debug 
        //std::cout << "[server] receives " << len << " bits:";
        #endif
        //std::cout << "receive " << len << " bits" << std::endl;
        buf[len] = '\0';
        for(int i = 0; i < L - 3; i++)
        {
            if (recv_count1[i] % 2 == 0) oram1[i]->start();
        }       
        if (recv_count2 % 2 == 0) oram2->start();
        if (recv_count3 % 2 == 0) oram3->start();

        if (buf[len - 1] <= '9' && buf[len - 1] >= '0')
        {
            int num = atoi(buf);
            int leaf = num / 10;
            if(buf[len - 1] >= '2') 
            {
                recv_count1[buf[len - 1] - '2']++;
                // recv_count1++;
                #if debug
                std::cout << "Begin fetch oram1 path:" << leaf;
                #endif
                oram1[buf[len - 1] - '2']->FetchPath(leaf);
                #if debug
                std::cout << "Finish fetch oram1 path:" << leaf;
                #endif
                //std::cout << "Fetch Oram1 Path " << leaf << std::endl;
            }
            if(buf[len - 1] == '1') 
            {
                recv_count2++;
                #if debug
                std::cout << "Begin fetch oram2 path:" << leaf;
                #endif
                oram2->FetchPath(leaf);
                #if debug
                std::cout << "Finish fetch oram2 path:" << leaf;
                #endif
            }
            if(buf[len - 1] == '0') 
            {
                recv_count3++;
                #if debug
                std::cout << "Begin fetch leaforam path:" << leaf;
                #endif
                oram3->FetchPath(leaf);
                #if debug
                std::cout << "Finish fetch leaforam path:" << leaf;
                #endif
            }
        }
        if (buf[len - 1] <= 'j' && buf[len - 1] >= 'a')
        {
            int index_len = buf[len - 2];
            char *ind = new char(index_len + 1);
            memcpy(ind, buf + (len - index_len - 2), index_len);
            ind[index_len] = '\0';
            int index = atoi(ind);
            block ciphertext;
            ciphertext.insert(ciphertext.end(), buf, buf + (len - index_len - 2));

            if(buf[len - 1] >= 'c') 
            {
                #if debug
                std::cout << "Begin Write to oram1-RAMstore : index = " << index;
                #endif
                oram1[buf[len - 1] - 'c']->WriteBucket(index, ciphertext);
                #if debug
                std::cout << "Finish Write to oram1-RAMstore : index = " << index;
                #endif
            }
            if(buf[len - 1] == 'b') 
            {
                #if debug
                std::cout << "Begin Write to oram2-RAMstore : index = " << index;
                #endif
                oram2->WriteBucket(index, ciphertext);
                #if debug

                std::cout << "Finish Write to oram2-RAMstore : index = " << index;
                #endif

                //std::cout << "Write to Oram2 Path " << index << std::endl;
            }
            if(buf[len - 1] == 'a') 
            {
                #if debug
                std::cout << "Begin Write to leaforam-RAMstore : index = " << index;
                #endif
                oram3->WriteBucket(index, ciphertext);
                #if debug
                std::cout << "Finish Write to leaforam-RAMstore : index = " << index;
                #endif
                //std::cout << "Write to leafOram Path " << index << std::endl;
            }
            char send_buf[255];
            memset(send_buf, 0, sizeof(send_buf));
            strcpy(send_buf, "ACK");
            send(conn, send_buf, sizeof(send_buf), 0);
            #if debug
            std::cout << "Send ACK!";
            #endif
        }
        if (strcmp(buf, "exit") == 0) {
            std::cout << "...disconnect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
            break;
        }
       // std::cout << std::endl;
    }
    end_time = clock();
    std::cout << "时间开销:" << double(end_time - begin_time)/CLOCKS_PER_SEC << std::endl;
    close(conn);
}
