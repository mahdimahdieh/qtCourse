#include "mainwindow.h"
#include <QApplication>
#include <string>
#include <exception>
#include <string>
#include <exception>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "tinyxml2.h"

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
            Classroom::number = number;
        } catch (const range_error& e) {
            throw;
        }
        try {
            if (capacity < 1 || capacity > 40) {
                throw range_error(1, 40);
            }
            Classroom::capacity = capacity;
        } catch (const range_error& e) {
            throw;
        }
        Classroom::projector = projector;
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
        return capacity;
    }
    void setCapacity(int capacity) {
        capacity = capacity;
    }
};

class TimeClass {
    std::vector<Classroom> list;
    const char* address = "class.xml";

    int readFile() {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(address) != tinyxml2::XML_SUCCESS) {
            std::cerr << "Error loading XML file." << std::endl;
            return 1;
        }
        tinyxml2::XMLElement* root = doc.FirstChildElement("classrooms");
        if (root) {
            for (tinyxml2::XMLElement* elem = root->FirstChildElement("classroom"); elem; elem = elem->NextSiblingElement("classroom")) {
                int num, cap;
                bool proj;
                elem->QueryIntAttribute("number", &num);
                elem->QueryIntAttribute("capacity", &cap);
                proj = elem->BoolAttribute("projector");
                list.emplace_back(num, cap, proj);
            }
        } else {
            std::cerr << "Error finding root element in XML file." << std::endl;
            return 1;
        }
        return 0;
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
