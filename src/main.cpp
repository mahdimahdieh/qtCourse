#include <string>
#include <exception>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <algorithm> // for std::find
#include "tinyxml2.h"
#include <string>


#define MIN_START_HOUR 7
#define MAX_START_HOUR 20
#define MAX_SESSION_DURATION_MINUTES 240

class range_error;
class conflict_error;
class Time;
enum class Day;
class Date;
class WeekTime;
class Person;
class PersonList;
class Classroom;
class ClassroomList;
class Lesson;
class LessonList;

class range_error : public std::exception {
    std::string message;
    int minValue;
    int maxValue;
    char* charStar;

public:
    range_error(int minValue, int maxValue)
            : minValue(minValue), maxValue(maxValue) {
        message = "Out of range error: Range: [" + std::to_string(minValue) + " - " + std::to_string(maxValue) + "]";
    }
    const char* what() {
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
    std::string message;

public:
    explicit conflict_error(std::string  at) : at(std::move(at)) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
    const std::string &getWith() const {
        return with;
    }
    void setWith(const std::string &w) {
        with = w;
        message = "Conflict Error with: " + with + " at " + at;
    }
    const std::string &getAt() const {
        return at;
    }
    void setAt(const std::string &a) {
        at = a;
        message = "Conflict Error with: " + with + " at " + at;
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
    explicit Time(int hour, int min) {
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
    void setMin(int minute) {
        min = minute % 60;
    }
    int getHour() const {
        return hour;
    }
    void setHour(int hr) {
        hour = hr % 24;
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
    return  str;
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
    std::string dateToString() const {
        return std::to_string(year) + "/" + std::to_string(month) + "/" + std::to_string(day);
    }
};

class WeekTime {
    Day day;
    Time time;

public:
    WeekTime(Day day, const Time &time) : day(day), time(time) {}
    WeekTime endTime(int durationMinutes) const {
        WeekTime wt(day, time.after(durationMinutes - 1));
        return wt;
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
        return day + " " + std::to_string(time.getHour()) + ":" + (time.getMin() < 10 ? "0" : "") + std::to_string(time.getMin() );
    }

};

class Person {
    std::string name;
    int id;
    bool is_teacher;

public:
    explicit Person(std::string name, int id, bool teacher = false) : name(std::move(name)), id(id), is_teacher(teacher) {}
    const std::string &getName() const {
        return name;
    }
    void setName(const std::string &string) {
        Person::name = string;
    }
    int getId() const {
        return id;
    }
    void setId(int i) {
        Person::id = i;
    }
    bool isTeacher() const {
        return is_teacher;
    }
    void printPerson() const {
        std::cout << "ID: " << id << "\t" << "Name: " << name << std::endl;
    }
};

class PersonList {
    std::vector<Person> personList;
    std::vector<Person> teacherList;

public:
    void addPerson (const Person& new_person) {
        try {
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
        catch (char const* s) {
            std::cout << s << std::endl;
        }
        catch (std::exception& e) {
            throw;
        }
    }
    Person getPersonInfo(int personID) const {
        if (int index = binarySearch(personList, &Person::getId, personID); index != -1)
            return personList.at(index);
        else
            throw "Couldn't Find Such a Person!";
    }
    Person getTeacherInfo(int teacherID) const {
        return teacherList.at(binarySearch(personList, &Person::getId, teacherID));
    }
    const std::vector<Person>& getPersonList() const {
        return personList;
    }
    const std::vector<Person>& getTeacherList() const {
        return teacherList;
    }
    bool emptyPerson() {
        if (personList.empty())
            return true;
        else
            return false;
    }
    bool emptyTeacher() {
        if (teacherList.empty())
            return true;
        else
            return false;
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
    void printClassroomSpecs() const {
        std::cout << "Classroom Number: " << number << std::endl;
        std::cout << "Has Projector: " << (projector ? "Yes" : "No") << std::endl;
        std::cout << "Capacity: " << capacity << " students" << std::endl;
    }
};

class ClassroomList {
    std::vector<Classroom> list;
    int readFile() {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile("class.xml") != tinyxml2::XML_SUCCESS) {
            std::cout << "Error loading XML file." << std::endl;
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
    explicit ClassroomList() {
        readFile();
    }
    ~ClassroomList() {
    }
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
        list = filtered;
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
    Classroom getClassroomInfo(int classroomNumber) const {
        return list.at(binarySearch(list, &Classroom::getNumber,classroomNumber));
    }
};


class Lesson {
protected:
    int id;
    std::string name;
    std::map<WeekTime, int> session;
    int lesson_max_capacity;
    bool projector;
public:
    Lesson(int id, std::string name, int capacity, bool projector = false) : id(id), name(std::move(name)), lesson_max_capacity(capacity), projector(projector) {}
    virtual void conflictSessionTime(const WeekTime& new_wt, int durationMin) const {
        for (auto this_session : session)
            if (!(new_wt.endTime(durationMin) <= this_session.first || new_wt >= this_session.first.endTime(this_session.second)))
                throw conflict_error(this_session.first.weekTimeToString());
    }
    void addSession (const WeekTime& new_wt, int durationMin) {
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
    void setId(int i) {
        id = i;
    }
    const std::string &getName() const {
        return name;
    }
    void setName(const std::string &string) {
        Lesson::name = string;
    }
    const std::map<WeekTime, int> &getSession() const {
        return session;
    }
    int getLessonMaxCapacity() const {
        return lesson_max_capacity;
    }
    bool getNeedProjector() const {
        return projector;
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
    virtual void printLesson() const {
        std::cout << "Lesson ID: " << id << std::endl;
        std::cout << "Lesson Name: " << name << std::endl;
        std::cout << "Max Capacity: " << lesson_max_capacity << std::endl;
        std::cout << "Requires Projector: " << (projector ? "Yes" : "No") << std::endl;
        std::cout << "Session Details:\n  ----------" << std::endl;
        for (const auto &sessionEntry: session) {
            const WeekTime &wt = sessionEntry.first;
            int duration = sessionEntry.second;
            std::cout << "  Start Time: " << wt.weekTimeToString() << std::endl;
            std::cout << "  Duration: " << duration << " minutes" << std::endl;
            std::cout << "  End Time: " << wt.endTime(duration).weekTimeToString() << std::endl;
            std::cout << "  ----------" << std:: endl;
        }
    }
};

class ExtraLesson : public Lesson {
    Date start, end;

public:
    ExtraLesson(int id, const std::string &name, int capacity, const Date &start, const Date &anEnd, bool projector = false) : Lesson(id, name, capacity, projector), start(start), end(anEnd) {}
    void conflictLessonTime(const Lesson& lesson) const override {
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
    void printLesson() const override  {
        Lesson::printLesson();
        std::cout << "  Start Date: " << start.dateToString() << std::endl;
        std::cout << "  End Date: " << end.dateToString() << std::endl;
        std::cout << "  ----------" << std::endl;
    }

    const Date &getStart() const {
        return start;
    }
    const Date &getEnd() const {
        return end;
    }
};



class LessonList {
    friend Lesson;
    std::vector<Lesson> lessonList;
    std::vector<ExtraLesson> extraLessonList;
    ClassroomList classroomList;
    PersonList personList;
    std::map<int, int> lessonListLocation;
    std::map<int, int> lessonListTeacher;
    std::map<int, std::vector<int>> lessonListStudentList;
    Date start, end;

public:
    LessonList(const Date &start, const Date &anEnd) : start(start), end(anEnd) {}
    const Lesson& getLesson(int lessonID) const {
        for (const auto& i : extraLessonList)
            if(i.getId() == lessonID)
                return i;
        return lessonList.at(binarySearch(lessonList, &Lesson::getId, lessonID));
    }
    const ClassroomList& getClassroomList() const {
        return classroomList;
    }
    const PersonList& getPersonList() const {
        return personList;
    }
    void conflictLesson (const int newLessonId, const int classroomNumber) {
        if (lessonListTeacher[newLessonId] != -1 || !lessonListStudentList[newLessonId].empty())
            throw "You are not allowed to add session after assigning Teacher or Student to this Lesson!";
        for (const auto& elem: lessonListLocation) {
            if (elem.second == classroomNumber) {
                try {
                    getLesson(elem.first).conflictLessonTime(getLesson(newLessonId));
                }
                catch (conflict_error& e) {
                    e.setWith(e.getWith() + " at the class number" + std::to_string(elem.second));
                    throw;
                }
            }
        }
    }
    void setClassroom(const int lessonId, const int classroomNumber) {
        try {
            conflictLesson(lessonId, classroomNumber);
        }
        catch (conflict_error& e) {
            throw;
        }
        if (lessonListLocation.find(lessonId) == lessonListLocation.end() && classroomList.getClassroomInfo(classroomNumber).getCapacity() >= getLesson(lessonId).getLessonMaxCapacity() &&
            (!getLesson(lessonId).getNeedProjector() || classroomList.getClassroomInfo(classroomNumber).isProjector()))
            lessonListLocation.insert(std::make_pair(lessonId, classroomNumber));
        else if (lessonListLocation.find(lessonId) != lessonListLocation.end() && classroomList.getClassroomInfo(classroomNumber).getCapacity() >= getLesson(lessonId).getLessonMaxCapacity() &&
                 (!getLesson(lessonId).getNeedProjector() ||
                  classroomList.getClassroomInfo(classroomNumber).isProjector())) {
            lessonListLocation[lessonId] = classroomNumber;
        }
    }
    int findEmptyClass(const int newLessonId) const {
        int minimumCapacity = getLesson(newLessonId).getLessonMaxCapacity();
        ClassroomList app = classroomList;
        app.removeUnderCapacity(minimumCapacity);
        while (!app.isEmpty()) {
            bool conflict = false;
            auto min = app.minCapacity();
            for (auto oldLesson: getPlannedLessonOnClassroom(min->getNumber())) {

                try {
                    getLesson(oldLesson).conflictLessonTime(getLesson(newLessonId));
                }
                catch (conflict_error&) {
                    conflict = true;
                    break;
                }

            }
            if (!conflict)
                return min->getNumber();
            else
                app.removeMinCapacityClassroom();
        }
        throw "There is no Empty Class";
    }
    std::vector<int> getPlannedLessonOnClassroom(int classroomNumber) const {
        std::vector<int> list;
        for (const auto& lesLoc: lessonListLocation) {
            if (lesLoc.second == classroomNumber){
                list.push_back(lesLoc.first);
            }
        }
        return list;
    }
    std::vector<int> getLessonListOfPerson(int i) const {
        std::vector<int> list;
        for (const auto& lesson: lessonListStudentList) {
            if (lesson.second.end() != std::find(lesson.second.begin(), lesson.second.end(), i))
                list.push_back(lesson.first);
        }
        for (const auto& lesson: lessonListTeacher) {
            if (lesson.second == i)
                list.push_back(lesson.first);
        }
        return list;
    }
    void conflictPersonLessonTime(const int lessonId, const int personID) const {
        try {
            for (auto oldLesson : getLessonListOfPerson(personID)) {
                getLesson(oldLesson).conflictLessonTime(getLesson(lessonId));
            }
        }
        catch (conflict_error& e){
            throw;
        }
    }
    void addStudent(int studentId, int lessonId) {
        try {
            if (lessonListLocation[lessonId] == -1)
                throw "set classroom!";
            conflictPersonLessonTime(lessonId, studentId);
            lessonListStudentList[lessonId].push_back(studentId);
        }
        catch (std::exception& e) {
            throw e;
        }
        catch (char const* s) {
            throw s;
        }
    }
    void setTeacher(int teacherId, int lessonId) {
        try {
            if (lessonListLocation[lessonId] == -1)
                throw "set classroom!";
            if (!personList.getPersonInfo(teacherId).isTeacher())
                throw "Person is not student!";
            conflictPersonLessonTime(lessonId, teacherId);
            lessonListTeacher[lessonId] = teacherId;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }
        catch (std::exception& e) {
            throw;
        }

    }
    void pushPersonList(const PersonList& newList) {
        std::vector<Person> newPersonList = newList.getPersonList();
        while (!newPersonList.empty()) {
            auto newPerson = newPersonList.back();
            personList.addPerson(newPerson);
            newPersonList.pop_back();
        }
    }
    void pushLesson(const Lesson& lesson) {
        try {
            if (lesson.getSession().empty())
                throw "Session is Empty!";
            std::vector<int> emptyStudentList;
            lessonList.push_back(lesson);
            lessonListLocation.insert(std::make_pair(lesson.getId(), -1));
            lessonListTeacher.insert(std::make_pair(lesson.getId(), -1));
            lessonListStudentList.insert(std::make_pair(lesson.getId(), emptyStudentList));
        }
        catch (char const* s) {
            throw;
        }
        catch (std::exception& e) {
            throw;
        }

    }
    void pushExtraLesson(const ExtraLesson& lesson) {
        try {
            if (lesson.getSession().empty())
                throw "Session is Empty!";
            std::vector<int> emptyStudentList;
            lessonList.push_back(lesson);
            extraLessonList.push_back(lesson);
            lessonListLocation.insert(std::make_pair(lesson.getId(), -1));
            lessonListTeacher.insert(std::make_pair(lesson.getId(), -1));
            lessonListStudentList.insert(std::make_pair(lesson.getId(), emptyStudentList));
        }
        catch (char const* s) {
            throw;
        }
        catch (std::exception& e) {
            throw;
        }

    }
    void printLessonSpecs(int lessonId) const {
        std::cout << "\n\n::::::::::::::::::: COURSE INFO :::::::::::::::::::\n";
        const auto& temp = getLesson(lessonId);
        temp.printLesson();
        int classroom = lessonListLocation.at(lessonId);
        classroomList.getClassroomInfo(classroom).printClassroomSpecs();
        int teacher = lessonListTeacher.at(lessonId);
        std::cout << "Teacher:  ";
        (personList.getPersonInfo(teacher)).printPerson();
        std::cout << "Students: " << std::endl;
        for (int student: lessonListStudentList.at(lessonId)) {
            std::cout << "  ";
            (personList.getPersonInfo(student)).printPerson();
        }
        std::cout << "::::::::::::::::: END COURSE INFO :::::::::::::::::\n";
    }
    void printTermSpecs() const {
        std::cout << ":::::::::::::::::::::::::::::::: TERM INFO ::::::::::::::::::::::::::::::::" << std::endl;
        std::cout << "Term Range: From " << start.dateToString() << " To " << end.dateToString();
        for(auto const& lesson : lessonList) {
            printLessonSpecs(lesson.getId());
        }
        std::cout << "\n:::::::::::::::::::::::::::::: END TERM INFO ::::::::::::::::::::::::::::::" << std::endl;
    }
};

int main()
{
    try {
        //define term
        LessonList term14021(Date(10, 9, 2023), Date(15, 2, 2024));
        //Read Classroom list from file by default

        // Define People. If they were teacher then pass true
        {
            PersonList people;
            try{
                people.addPerson(Person("Ali Ahmadi", 501, false));
                people.addPerson(Person("Hossein Mohammadi", 502, false));
                people.addPerson(Person("Javad Aziz", 503, false));
                people.addPerson(Person("Azita Sadeghi", 504, false));
                people.addPerson(Person("Kamand Esmaeili", 505, true));
                people.addPerson(Person("Jamal Kamali", 506, true));
                people.addPerson(Person("Duplicate Test", 501, true));
            }
                // check not uniqueness
            catch (char* s) {
                std::cout << s << std::endl;
            }
            catch (std::exception& e) {
                e.what();
                std::cout << std::endl;
            }
            term14021.pushPersonList(people);
        }

        //Define Course
        {
            Lesson ap(10, "Advanced Programming", 25, true);

            try {
                term14021.pushLesson(ap);
            }
            catch (char const* s) {
                std::cout << s << std::endl;
            }
            ap.addSession(WeekTime(Day::sun, Time(14, 00)), 120);
            ap.addSession(WeekTime(Day::tue, Time(13, 00)),60);

            Lesson dm(11, "Discrete Mathematics", 35);
            dm.addSession(WeekTime(Day::sat, Time(14, 00)), 120);
            dm.addSession(WeekTime(Day::wed, Time(8, 00)),60);

            //Extra Lesson
            ExtraLesson xpCon(12, "XPCon", 35, Date(2, 12, 2023), Date(3, 12, 2023), true);
            xpCon.addSession(WeekTime(Day::mon, Time(15, 00)), 50);
            xpCon.addSession(WeekTime(Day::wed, Time(8, 00)),60);

            term14021.pushLesson(ap);
            term14021.pushLesson(dm);
            term14021.pushExtraLesson(xpCon);
        }

        // find empty classroom
        //manual assign classroom
        try {
            term14021.setClassroom(10, 1);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }

        // Auto assign classroom (duplicate capacity in the same time)
        try {
            int emptyClass = term14021.findEmptyClass(11);
            term14021.setClassroom(11, emptyClass);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        try {
            int emptyClass = term14021.findEmptyClass(12);
            term14021.setClassroom(12, emptyClass);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }


        //set wrong teacher
        try {
            term14021.setTeacher(501, 10);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }

        //set teacher
        try {
            term14021.setTeacher(506, 11);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }
        try {
            term14021.setTeacher(505, 12);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }
        try {
            term14021.setTeacher(505, 10);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }


        // add student to lesson
        try {
            term14021.addStudent(505, 10);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }
        try {
            term14021.addStudent(501, 10);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }
        try {
            term14021.addStudent(501, 11);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }
        try {
            term14021.addStudent(502, 12);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }
        try {
            term14021.addStudent(503, 11);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }
        try {
            term14021.addStudent(503, 10);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }
        try {
            term14021.addStudent(503, 12);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }
        try {
            term14021.addStudent(504, 11);
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }


        //print term
        try {
            term14021.printTermSpecs();
        }
        catch (std::exception& e) {
            e.what();
            std::cout << std::endl;
        }
        catch (char const* s) {
            std::cout << s << std::endl;
        }
    }
    catch (std::exception& e) {
        e.what();
        std::cout << std::endl;
    }
    catch (char const* s) {
        std::cout << s << std::endl;
    }
}

/*
 * Documentation
 * 1. You are not allow to add session after assigning student or teacher
 * 2. GitHub Address: https://github.com/mahdimahdieh/qtCourse
 *
 */
