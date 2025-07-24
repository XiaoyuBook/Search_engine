#include "../include/Search_server.h"
#include "../include/main_cache.h"
#include "../include/patch_register.h"
#include <chrono>
#include <thread>

using std::string;
using std::vector;


int main(int argc, char **argv) {
    Search_server server(4, 10, "127.0.0.1", 8888);
    start_candidate_patch_sync();
    start_web_patch_sync();
    server.start();




    return 0;
}

