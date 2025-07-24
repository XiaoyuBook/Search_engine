#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

using namespace std;

// TLV编码函数
string encode_tlv(uint16_t type, const string &value) {
    uint32_t len = value.size();
    string packet;
    packet.append(reinterpret_cast<const char *>(&type), sizeof(type));
    packet.append(reinterpret_cast<const char *>(&len), sizeof(len));
    packet.append(value);
    return packet;
}

// TLV解码函数
pair<uint16_t, string> decode_tlv(int sockfd) {
    uint16_t type;
    uint32_t length;

    ssize_t n = read(sockfd, &type, sizeof(type));
    if (n <= 0) {
        throw runtime_error("Failed to read type");
    }

    n = read(sockfd, &length, sizeof(length));
    if (n <= 0) {
        throw runtime_error("Failed to read length");
    }

    string value(length, '\0');
    size_t total = 0;
    while (total < length) {
        ssize_t count = read(sockfd, &value[total], length - total);
        if (count <= 0) {
            throw runtime_error("Failed to read value");
        }
        total += count;
    }

    return {type, value};
}

int main() {
    const char *server_ip = "127.0.0.1";
    const int server_port = 8888;      

    // 创建socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return 1;
    }

    // 连接服务器
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(sockfd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return 1;
    }

    while (true) {
        cout << "请选择服务：1 关键字推荐，2 网页搜索（输入1或2进行选择）：" << endl;
        string choice;
        getline(cin, choice);

        uint16_t type = 0;
        string input;

        if (choice == "1") {
            type = 0x0002;
            cout << "关键字推荐: ";
            getline(cin, input);
        } else if (choice == "2") {
            type = 0x0001;
            cout << "网页搜索: ";
            getline(cin, input);
        } else {
            cout << "无效输入，请重新输入。" << endl;
            continue;
        }

        string packet = encode_tlv(type, input);
        if (write(sockfd, packet.data(), packet.size()) != (ssize_t)packet.size()) {
            cerr << "发送失败" << endl;
            break;
        }

        try {
            auto [resp_type, value] = decode_tlv(sockfd);
            cout << "\n--- 服务端响应（type=" << hex << resp_type << dec << "） ---" << endl;
            cout << value << endl;
        } catch (const exception &e) {
            cerr << "接收失败: " << e.what() << endl;
            break;
        }

        cout << "\n----------------------------------\n" << endl;
    }

    close(sockfd);
    return 0;
}
