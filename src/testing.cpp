#include <stdio.h>
#include <cmath>
#include <ctime>
#include <string.h>
#include <string>
#include <chrono>
#include <iostream>
#include <thread>
#include <mutex>

int GetRandom(int max) {
    srand(time(NULL));
    return rand() % max;
}

std::string GetTime() {
    auto nowTime = std::chrono::system_clock::now();
    std::time_t sleepTime =
        std::chrono::system_clock::to_time_t(nowTime);
    return std::ctime(&sleepTime);
}

double acctBalance = 100;

std::mutex acctLock;

void GetMoney(int id, double withdrawal) {
    std::lock_guard<std::mutex> lock(acctLock);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << id <<
        " tries to withdraw $" <<
        withdrawal << " on " <<
        GetTime() << "\n";
    if ((acctBalance - withdrawal) >= 0) {
        acctBalance -= withdrawal;
        std::cout << "New Account Balance is $" <<
            acctBalance << "\n";
    }
    else {
        std::cout << "Not enough money in Accound\n";
        std::cout << "Current Balance is $ " << 
            acctBalance << "\n";
    }
}

int main(int argc, char** argv) {
    printf("%s was run with %d arguments\n", argv[0], argc - 1);

    for (int i = 1; i < argc; i++) {
        printf("[%d] %s {%d}\n", i, argv[i], (int)strlen(argv[i]));
    }
    printf("\n\n");

    std::thread threads[10];
    for (int i = 0; i < 10; ++i) {
        threads[i] = std::thread(GetMoney, i, 15);
    }

    for (int i = 0; i < 10; ++i) {
        threads[i].join();
    }

    system("pause");
    return(0);
}