# qtCourse

A course management system written in C++17 for my Advanced Programming course.
It models a semester with classrooms, teachers, students and lessons, checks for
scheduling conflicts, assigns classrooms and prints a term report.

The original code is kept as submitted. Only the folder layout and build files
were cleaned up.

## What it does

- Lessons with one or more weekly sessions, plus date-bounded extra lessons.
- Conflict checks for lesson/lesson, lesson/classroom and person/person overlaps.
- Classrooms loaded from `class.xml`, with capacity and projector requirements.
- Manual classroom assignment, or automatic assignment to the smallest suitable
  free room.
- People list with unique IDs, sorted lookups and a separate teacher list.
- Custom `range_error` and `conflict_error` exceptions.
- Hand-written `binarySearch` and bubble `sort` templates.
- A printed report showing each lesson, its sessions, room, teacher and students.

The Qt window is just the application shell; all logic lives in `src/main.cpp`
and the demo scenario writes its output to the console.

## Build

You need CMake 3.5+, a C++17 compiler and Qt 5 or Qt 6 with Widgets. `tinyxml2`
is bundled in `third_party/`, so there is nothing else to install.

```sh
cmake -S . -B build
cmake --build build
```

## Run

Run from the repository root, because `class.xml` is read from the working
directory:

```sh
./build/Course
```

On Windows use `build\Course.exe` from a console so you can see the output.

## Layout

```text
CMakeLists.txt
class.xml                  classroom data
src/main.cpp               domain model and demo scenario
src/mainwindow.*           Qt shell
third_party/tinyxml2/      XML parser
```

## Useful to know

- Times are limited to 07:00–19:59 and sessions to 240 minutes.
- Classroom numbers and capacities are 1–40.
- A session's printed end time is its last minute, so 14:00 + 120 min shows 15:59.
- Sessions cannot be added after a teacher or student is assigned to a lesson.
- The demo in `main()` is hard-coded; there is no interactive UI and nothing is
  saved between runs.

## License

MIT. See [LICENSE](LICENSE).

`tinyxml2` is by Lee Thomason and uses the zlib license; see its header.
