#include "mainwindow.h"
#include "out_of_range.h"
#include <QApplication>
#include <string>
#include <exception>
#include <string>
#include <exception>
#include <string>

class range_error : public std::exception {
private:
    std::string errorType;
    int minValue;
    int maxValue;

public:
    range_error(int minValue, int maxValue)
            : minValue(minValue), maxValue(maxValue) {}

    const char* what() const noexcept override {
        std::string message = "Out of range error: " + errorType + ". Range: [" + std::to_string(minValue) + " - " + std::to_string(maxValue) + "]";
        return message.c_str();
    }
};

class Person {
    std::string name;
    int id;
public:
    explicit Person(const std::string &name, int id) : name(name), id(id) {}
    const std::string &getName() const {
        return name;
    }
    void setName(const std::string &name) {
        Person::name = name;
    }
    int getId() const {
        return id;
    }
    void setId(int id) {
        Person::id = id;
    }
};

class Classroom {
    int number;
    bool projector;
    int capacity;
public:
    Classroom(int number, int capacity = 40 , bool projector = true) {
        try {
            if (number < 1 || number > 40) {
                throw range_error(1, 40);
            }
            this->number = number;
        } catch (const out_of_range& e) {
            throw;
        }
        try {
            if (capacity < 1 || capacity > 40) {
                throw range_error(1, 40);
            }
            this->capacity = capacity;
        } catch (const out_of_range& e) {
            throw;
        }
        this->projector = projector;
    }
    int getNumber() const {
        return number;
    }
    void setNumber(int number) {
        Classroom::number = number;
    }
    bool isProjector() const {
        return projector;
    }
    void setProjector(bool projector) {
        Classroom::projector = projector;
    }
    int getCapacity() const {
        return Capacity;
    }
    void setCapacity(int capacity) {
        Capacity = capacity;
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
