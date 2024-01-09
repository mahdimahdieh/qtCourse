#include "mainwindow.h"
#include <QApplication>
#include <string>
#include <exception>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "tinyxml2.h"

#define MIN_START_HOUR 7
#define MAX_START_HOUR 20
#define MAX_SESSION_DURATION_MINUTES 240

class range_error : public std::exception {
private:
    std::string message; // Change to std::string
    int minValue;
    int maxValue;
public:
    range_error(int minValue, int maxValue)
            : minValue(minValue), maxValue(maxValue) {
        message = "Out of range error: Range: [" + std::to_string(minValue) + " - " + std::to_string(maxValue) + "]";
    }
    const char* what() const noexcept override {
        return message.c_str();
    }
    int getMinValue() const {
        return minValue;
    }
    int getMaxValue() const {
        return maxValue;
    }
};

template<typename T>
int binarySearch(const std::vector<T>& vec, int (T::*method)() const, int target) {

    int low = 0;
    int high = vec.size() - 1;

    while(low <= high) {
        int mid = low + (high - low) / 2;
        int midVal = (vec[mid].*method)();

        if(midVal == target) {
            return mid;
        }
        else if(midVal < target) {
            low = mid + 1;
        }
        else {
            high = mid - 1;
        }
    }

    return -1;
}

template<typename T>
void quickSort(std::vector<T>& vec, int (T::*comparator)() const) {
    if (vec.size() <= 1) {
        return;
    }
    int pivotIndex = vec.size() / 2;
    T pivot = vec[pivotIndex];
    int i = 0;
    int j = vec.size() - 1;
    while (i <= j) {
        while((vec[i].*comparator)() < (pivot.*comparator)()) {
            i++;
        }
        while((vec[j].*comparator)() > (pivot.*comparator)()) {
            j--;
        }
        if (i <= j) {
            std::swap(vec[i], vec[j]);
            i++;
            j--;
        }
    }
    if (pivotIndex < j) {
        quickSort(vec, comparator, 0, j);
    }
    if (i < pivotIndex) {
        quickSort(vec, comparator, i, pivotIndex);
    }
    if (i < vec.size()) {
        quickSort(vec, comparator, i, vec.size()-1);
    }
}

class Time {
    int min;
    int hour;
public:
    explicit Time(int min, int hour) {
        try {
            if (min > 59 || min < 0)
                throw range_error(0, 59);
            else
                this->min = min;
            if (hour > MAX_START_HOUR - 1 || hour < MIN_START_HOUR)
                throw range_error(MIN_START_HOUR, MAX_START_HOUR - 1);
            else
                this->hour = hour;
        }
        catch (range_error& e) {
            throw;
        }
    }
    Time after(int durationMin) const{
        Time temp = *this;
        if (durationMin < 0 || durationMin > 240)
            throw range_error(0, 240);
        while (temp.getMin() + durationMin > 59) {
            durationMin -= 60;
            temp.setHour(getHour() + 1);
        }
        temp.setMin(temp.getMin() + durationMin);
        return temp;
    }
    int getMin() const {
        return min;
    }
    void setMin(int min) {
        Time::min = min % 60;
    }
    int getHour() const {
        return hour;
    }
    void setHour(int hour) {
        Time::hour = hour % 24;
    }
    bool operator>=(const Time& other) const {
        if (hour > other.hour)
            return true;
        else if (hour == other.hour && min >= other.min)
            return true;
        else
            return false;
    }
    bool operator<=(const Time& other) const {
        if (hour < other.hour)
            return true;
        else if (hour == other.hour && min <= other.min)
            return true;
        else
            return false;
    }
    bool operator<(const Time& other) const {
        if (hour < other.hour)
            return true;
        else if (hour == other.hour && min < other.min)
            return true;
        else
            return false;
    }
    bool operator>(const Time& other) const {
        if (hour > other.hour)
            return true;
        else if (hour == other.hour && min > other.min)
            return true;
        else
            return false;
    }
    bool operator==(const Time& other) const {
        return (hour == other.hour && min == other.min);
    }
};

enum class Day {
    sat,
    sun,
    mon,
    tue,
    wed,
    thu,
    fri
};
Day operator++(Day& day) {
    day = static_cast<Day>((static_cast<int>(day) + 1) % 7);
    return day;
}
std::ostream& operator<<(std::ostream& os, const Day& day) {
    switch (day) {
        case Day::sat:
            os << "Saturday";
            break;
        case Day::sun:
            os << "Sunday";
            break;
        case Day::mon:
            os << "Monday";
            break;
        case Day::tue:
            os << "Tuesday";
            break;
        case Day::wed:
            os << "Wednesday";
            break;
        case Day::thu:
            os << "Thursday";
            break;
        case Day::fri:
            os << "Friday";
            break;
    }
    return os;
}

class WeekTime {
    Day day;
    Time time;
public:
    WeekTime(Day day, const Time &time) : day(day), time(time) {}
    WeekTime endTime(int durationMinutes) const {
        return WeekTime(day, time.after(durationMinutes - 1));
    }
    bool operator>=(const WeekTime& other) const {
        if (day > other.day)
            return true;
        else if (day == other.day && time >= other.time)
            return true;
        else
            return false;
    }
    bool operator<=(const WeekTime& other) const {
        if (day < other.day)
            return true;
        else if (day == other.day && time <= other.time)
            return true;
        else
            return false;
    }
    bool operator<(const WeekTime& other) const {
        if (day < other.day)
            return true;
        else if (day == other.day && time < other.time)
            return true;
        else
            return false;
    }
    bool operator>(const WeekTime& other) const {
        if (day > other.day)
            return true;
        else if (day == other.day && time > other.time)
            return true;
        else
            return false;
    }
    bool operator==(const WeekTime& other) const {
        return (day == other.day && time == other.time);
    }
};

class Person {
    std::string name;
    int id;
    bool is_teacher;
public:
    explicit Person(const std::string &name, int id, bool teacher = false) : name(name), id(id), is_teacher(teacher) {}
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
    bool isTeacher() const {
        return is_teacher;
    }
};

class PersonList {
    std::vector<Person> personList;
    std::vector<Person> teacherList;
public:
    void addPerson (const Person& new_person) {
        for (auto person: personList)
            if (person.getId() == new_person.getId())
                throw "Duplicate ID";
        personList.push_back(new_person);
        if (new_person.isTeacher())
            teacherList.push_back(new_person);
        quickSort(personList, &Person::getId);
    }
    int findPerson(int personID) {
        return binarySearch(personList, &Person::getId, personID);
    }
};

class Classroom {
    int number;
    bool projector;
    int capacity;
public:
    explicit Classroom(int number, int capacity = 40 , bool projector = true) {
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
            this->capacity = capacity;
        } catch (const range_error& e) {
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
        return capacity;
    }
    void setCapacity(int capacity) {
        capacity = capacity;
    }
};

class ClassroomList {
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

class Lesson {
    int id;
    std::string name;
    std::map<WeekTime, int> session;
    int teacherID = 0;
    std::vector<int> studentIDList;

public:
    bool conflictSessionTime(const WeekTime& new_wt, int durationMin) const {
        for (auto this_session : session)
            if (!(new_wt.endTime(durationMin) <= this_session.first || new_wt >= this_session.first.endTime(this_session.second)))
                return true;
        return false;
    }
    bool addSession (const WeekTime& new_wt, int durationMin) {
        if (teacherID != 0 || studentIDList.size() != 0)
            throw "You are not allowed to add session after assigning Teacher or Student to this Lesson!";
        if (durationMin > MAX_SESSION_DURATION_MINUTES || durationMin < 1)
            throw range_error(1, MAX_SESSION_DURATION_MINUTES);
        if (conflictSessionTime(new_wt, durationMin))
            return false;
    }
    int getId() const {
        return id;
    }
    void setId(int id) {
        Lesson::id = id;
    }
    const std::string &getName() const {
        return name;
    }
    void setName(const std::string &name) {
        Lesson::name = name;
    }
};

class LessonList {
    std::map<Lesson, Classroom> lessonList;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

/*
 * Documentation
 * 1. You are not allow to add session after assigning student or teacher
 *
 *
 */