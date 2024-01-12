#include "mainwindow.h"
#include <QApplication>
#include <string>
#include <exception>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <algorithm> // for std::find
#include "tinyxml2.h"

#define MIN_START_HOUR 7
#define MAX_START_HOUR 20
#define MAX_SESSION_DURATION_MINUTES 240

class range_error : public std::exception {
    std::string message;
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

class conflict_error : public std::exception {
    std::string with;
    std::string at;

public:
    conflict_error(const std::string& at) : at(at) {}
    const char* what() const noexcept override {
        return ("Conflict Error with: " + with + " at " + at).c_str();
    }
    const std::string &getWith() const {
        return with;
    }
    void setWith(const std::string &with) {
        conflict_error::with = with;
    }
    const std::string &getAt() const {
        return at;
    }
    void setAt(const std::string &at) {
        conflict_error::at = at;
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

// Bubble Sort
template<typename T>
void sort(std::vector<T>& vec, int (T::*comparator)() const) {
    for (int i=0; i<vec.size()-1; i++)
        for (int j=0; j<vec.size()-i-1; j++)
            if ((vec[j].*comparator)() > (vec[j+1].*comparator)())
                std::swap(vec[j], vec[j+1]);
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
        case Day::sat: os << "Saturday"; break;
        case Day::sun: os << "Sunday"; break;
        case Day::mon: os << "Monday"; break;
        case Day::tue: os << "Tuesday"; break;
        case Day::wed: os << "Wednesday"; break;
        case Day::thu: os << "Thursday"; break;
        case Day::fri: os << "Friday"; break;
    }
    return os;
}
std::string dayToStr(const Day& day) {
    std::string str;
    switch(day) {
        case Day::sat: str = "Saturday"; break;
        case Day::sun: str = "Sunday"; break;
        case Day::mon: str = "Monday"; break;
        case Day::tue: str = "Tuesday"; break;
        case Day::wed: str = "Wednesday"; break;
        case Day::thu: str = "Thursday"; break;
        case Day::fri: str = "Friday"; break;
    }
}
std::string operator+(const std::string& str, const Day& day) {
    return str + dayToStr(day);
}
std::string operator+(const Day& day, const std::string& str) {
    return dayToStr(day) + str;
}

class Date {
private:
    int day, month, year;

public:
    Date(int d, int m, int y) : day(d), month(m), year(y) {}
    bool operator==(const Date& other) const {
        return (day == other.day) && (month == other.month) && (year == other.year);
    }
    bool operator!=(const Date& other) const {
        return !(*this == other);
    }
    bool operator<(const Date& other) const {
        if (year != other.year) {
            return year < other.year;
        }
        if (month != other.month) {
            return month < other.month;
        }
        return day < other.day;
    }
    bool operator<=(const Date& other) const {
        return (*this < other) || (*this == other);
    }
    bool operator>(const Date& other) const {
        return !(*this <= other);
    }
    bool operator>=(const Date& other) const {
        return !(*this < other);
    }
};

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
    std::string weekTimeToString() const {
        return day + " " + std::to_string(time.getHour()) + ":" + std::to_string(time.getMin());
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
        for (const auto& person: personList)
            if (person.getId() == new_person.getId())
                throw "Duplicate ID";
        personList.push_back(new_person);
        sort(personList, &Person::getId);
        if (new_person.isTeacher()) {
            teacherList.push_back(new_person);
            sort(teacherList, &Person::getId);
        }
    }
    Person getPersonInfo(int personID) {
        return personList.at(binarySearch(personList, &Person::getId, personID));
    }
    Person getTeacherInfo(int teacherID) {
        return teacherList.at(binarySearch(personList, &Person::getId, teacherID));
    }
    const std::vector<Person>& getPersonList() const {
        return personList;
    }
    const std::vector<Person>& getTeacherList() const {
        return teacherList;
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
    bool isProjector() const {
        return projector;
    }
    int getCapacity() const {
        return capacity;
    }
    bool operator==(const Classroom &rhs) const {
        return number == rhs.number;
    }
    bool operator!=(const Classroom &rhs) const {
        return number != rhs.number;
    }
};

class ClassroomList {
    explicit ClassroomList() {
        readFile();
    }
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

public:
    explicit ClassroomList(const std::vector<Classroom> &list) : list(list) {}
    const std::vector<Classroom> &getList() const {
        return list;
    }
    void removeUnderCapacity(int minCapacity) {
        std::vector<Classroom> filtered;
        for (const auto& classroom : list) {
            if (classroom.getCapacity() >= minCapacity) {
                filtered.push_back(classroom);
            }
        }
        for (const auto& classroom : filtered) {
            auto it = std::find(list.begin(), list.end(), classroom);
            if (it != list.end()) {
                list.erase(it);
            }
        }
    }
    bool isEmpty() const {
        return list.empty();
    }
    auto minCapacity() {
        auto it = list.begin();
        int min = list.front().getCapacity();
        for (auto classroom: list) {
            if (classroom.getCapacity() < min)
                it = std::find(list.begin(), list.end(), classroom);
        }
        return it;
    }
    void removeMinCapacityClassroom() {
        list.erase(minCapacity());
    }
};

class Lesson {
protected:
    int id;
    std::string name;
    std::map<WeekTime, int> session;
    int teacherID = 0;
    std::vector<int> studentIDList;
    int lesson_max_capacity;
public:
    Lesson(int id, const std::string &name) : id(id), name(name) {}

    virtual void conflictSessionTime(const WeekTime& new_wt, int durationMin) const {
        for (auto this_session : session)
            if (!(new_wt.endTime(durationMin) <= this_session.first || new_wt >= this_session.first.endTime(this_session.second)))
                throw conflict_error(this_session.first.weekTimeToString());
    }
    void addSession (const WeekTime& new_wt, int durationMin) {
        if (teacherID != 0 || studentIDList.size() != 0)
            throw "You are not allowed to add session after assigning Teacher or Student to this Lesson!";
        if (durationMin > MAX_SESSION_DURATION_MINUTES || durationMin < 1)
            throw range_error(1, MAX_SESSION_DURATION_MINUTES);
        try {
            conflictSessionTime(new_wt, durationMin);
        }
        catch (conflict_error& e) {
            e.setWith("self lesson");
            throw;
        }
        session.insert(std::pair(new_wt, durationMin));
    }

    virtual void conflictLessonTime(const Lesson& lesson) const {
        try {
            for (auto temp_session: lesson.getSession())
                conflictSessionTime(temp_session.first, temp_session.second);
        }
        catch (conflict_error& e) {
            e.setWith(std::to_string(lesson.getId()));
            throw;
        }
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
    const std::map<WeekTime, int> &getSession() const {
        return session;
    }
    int getLessonMaxCapacity() const {
        return lesson_max_capacity;
    }
    bool operator<(const Lesson& rhs) const {
        return id < rhs.id;
    }
    bool operator==(const Lesson& other) const {
        return id == other.id;
    }
    bool operator!=(const Lesson& other) const {
        return id != other.id;
    }
};

class ExtraLesson : public Lesson {
    Date start, end;

public:
    ExtraLesson(int id, const std::string &name, const Date &start, const Date &anEnd) : Lesson(id, name), start(start), end(anEnd) {}
    void conflictLessonTime(const Lesson& lesson) const {
        if (typeid(lesson) == typeid(ExtraLesson)) {
            const ExtraLesson& extraLesson = *dynamic_cast<const ExtraLesson*>(&lesson);
            if (start >= extraLesson.getEnd() || end <= extraLesson.getStart()) {
                return;
            }
        }
        try {
            for (auto temp_session: lesson.getSession())
                conflictSessionTime(temp_session.first, temp_session.second);
        }
        catch (conflict_error& e) {
            e.setWith(std::to_string(lesson.getId()));
            throw;
        }
    }
    const Date &getStart() const {
        return start;
    }
    const Date &getEnd() const {
        return end;
    }
};

class LessonList {
    std::map<Lesson, Classroom> lessonList;

public:
    void conflictLesson (const Lesson& lesson, const Classroom& classroom) {
        for (const auto& elem: lessonList) {
            if (elem.second == classroom) {
                try {
                    elem.first.conflictLessonTime(lesson);
                }
                catch (conflict_error& e) {
                    e.setWith(e.getWith() + " at the class number" + std::to_string(elem.second.getNumber()));
                    throw;
                }
            }
        }
    }
    void addLesson(const Lesson& lesson, const Classroom& classroom) {
        conflictLesson(lesson, classroom);
        lessonList.insert(std::make_pair(lesson, classroom));
    }
    int findEmptyClass(const Lesson& lesson, ClassroomList list) {
        bool conflict = false;
        list.removeUnderCapacity(lesson.getLessonMaxCapacity());
        while (!list.isEmpty()) {
            auto min = list.minCapacity();
            for(const auto& myLesson: lessonList){
                if(min->getNumber() == myLesson.second.getNumber()) {
                    try {
                        myLesson.first.conflictLessonTime(lesson);
                    }
                    catch (conflict_error) {
                        conflict = true;
                        break;
                    }
                }
            }
            if (conflict == false) {
                return min->getNumber();
            } else {
                list.removeMinCapacityClassroom();
            }
        }
        throw "There is no Empty Class";
    }
    Lesson getLessonInfo(int lessonID) const {
        for (const auto& lesson:  lessonList) {
            if (lesson.first.getId() == lessonID){
                return lesson.first;
            }
        }
    }
    Classroom getClassroomInfo(int lessonID) const {
        for (const auto& lesson:  lessonList) {
            if (lesson.first.getId() == lessonID){
                return lesson.second;
            }
        }
    }
    std::vector<Lesson> getPlannedLessonOnClassroomList(int classroomNumber) {
        std::vector<Lesson> list;
        for (const auto& lesson:  lessonList) {
            if (lesson.second.getNumber() == classroomNumber){
                list.push_back(lesson.first);
            }
        }
        return list;
    }

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
