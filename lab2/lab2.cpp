#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>

const double PI = 3.14159265358979323846;

std::mutex mtx; 


class Developer {
public:
    Developer(double v, int n) : speed(v), changeDirectionTime(n) {
        x = 0;
        y = 0;
        direction = randomDirection();
        lastDirectionChange = 0;
    }

    void update(double time) {
        if (time - lastDirectionChange >= changeDirectionTime) {
            direction = randomDirection();
            lastDirectionChange = time;
        }

        x += speed * cos(direction) * time;
        y += speed * sin(direction) * time;
    }

    void printPosition() const {
        std::lock_guard<std::mutex> guard(mtx);
        std::cout << "Developer Position: (" << x << ", " << y << ")\n";
    }

private:
    double x, y;
    double speed;
    int changeDirectionTime;
    double direction;
    double lastDirectionChange;

    double randomDirection() {
        return ((double)rand() / RAND_MAX) * 2 * PI;
    }
};




class Manager {
public:
    Manager(double v, double r) : speed(v), radius(r) {
        angle = 0;
    }

    void update(double time) {
        angle += speed / radius * time;
        if (angle >= 2 * PI) {
            angle -= 2 * PI;
        }
    }

    void printPosition() const {
        double x = radius * cos(angle);
        double y = radius * sin(angle);
        std::lock_guard<std::mutex> guard(mtx);
        std::cout << "Manager Position: (" << x << ", " << y << ")\n";
    }

private:
    double speed;
    double radius;
    double angle;
};

void developerThread(Developer& developer, double timeStep, double totalTime) {
    for (double time = 0; time <= totalTime; time += timeStep) {
        developer.update(timeStep);
        developer.printPosition();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(timeStep * 1000)));
    }
}

void managerThread(Manager& manager, double timeStep, double totalTime) {
    for (double time = 0; time <= totalTime; time += timeStep) {
        manager.update(timeStep);
        manager.printPosition();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(timeStep * 1000)));
    }
}


int main() {
    srand(time(0));

    double developerSpeed = 1.0;
    int directionChangeInterval = 5;
    Developer developer(developerSpeed, directionChangeInterval);

    double managerSpeed = 1.0;
    double managerRadius = 10.0;
    Manager manager(managerSpeed, managerRadius);

    double timeStep = 1.0;
    double totalTime = 20.0;

    std::thread devThread(developerThread, std::ref(developer), timeStep, totalTime);
    std::thread mgrThread(managerThread, std::ref(manager), timeStep, totalTime);

    devThread.join();
    mgrThread.join();

    return 0;
}
