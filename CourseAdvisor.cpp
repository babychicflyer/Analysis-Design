//  CourseAdvisor.cpp — final, clean version
//  Hash-table implementation of ABCU advising tool
// Author: Kelly Reinersman
// Version     : 1.0
//  ------------------------------------------------------------

#include <algorithm>       // std::sort, std::transform
#include <fstream>         // ifstream
#include <iostream>        // cout, cin, cerr
#include <limits>          // std::numeric_limits
#include <sstream>         // stringstream, getline with delimiter
#include <string>          // std::string
#include <unordered_map>   // std::unordered_map
#include <unordered_set>   // std::unordered_set
#include <vector>          // std::vector
const std::string kDefaultFile = "CS 300 ABCU_Advising_Program_Input.csv";
// Place that CSV in the SAME folder as the executable (cmake-build-debug).


using namespace std;
// -----------------------------------------------------------------
// Plain-old-data record for one course
// -----------------------------------------------------------------
struct Course
{
    std::string number;                 // “CSCI200”
    std::string title;                  // “Data Structures”
    std::vector<std::string> prereqs;   // may be empty
};

// -----------------------------------------------------------------
// Hash table that stores every Course object (key = course number)
// -----------------------------------------------------------------
std::unordered_map<std::string, Course> gTable;

// -----------------------------------------------------------------
// Little string helpers
// -----------------------------------------------------------------
static inline std::string ltrim(std::string s)
{
    auto pos = s.find_first_not_of(" \t\r\n");
    return (pos == std::string::npos) ? "" : s.substr(pos);
}

static inline std::string rtrim(std::string s)
{
    auto pos = s.find_last_not_of(" \t\r\n");
    return (pos == std::string::npos) ? "" : s.substr(0, pos + 1);
}

static inline std::string trim(std::string s)
{
    return rtrim(ltrim(std::move(s)));
}

// -----------------------------------------------------------------
// Forward declarations
// -----------------------------------------------------------------
bool loadFile(const std::string& fileName);
void printCourse(const std::string& id);
void printAll();

// =================================================================
// main – four-option console menu
// =================================================================
int main()
{
    std::cout << "ABCU Course Advisor — hash-table version\n";

    while (true)
    {
        std::cout << "\nMAIN MENU\n"
                  << "  1) Load data file\n"
                  << "  2) Print course list (A→Z)\n"
                  << "  3) Print one course & prerequisites\n"
                  << "  9) Exit\n"
                  << "Choice: ";

        int menu = 0;
        if (!(std::cin >> menu)) break;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (menu == 1)
        {
            std::cout << "Loading \"" << kDefaultFile << "\" ...\n";
            if (loadFile(kDefaultFile))
                std::cout << "File loaded successfully.\n";
            else
                std::cout << "Load failed.  Make sure \"" << kDefaultFile
                          << "\" is in the program folder.\n";
        }

        else if (menu == 2)
        {
            if (gTable.empty())
                std::cout << "No data loaded yet.\n";
            else
                printAll();
        }
        else if (menu == 3)
        {
            if (gTable.empty())
            {
                std::cout << "No data loaded yet.\n";
                continue;
            }
            std::cout << "Enter course number (e.g., CSCI200): ";
            std::string id;
            std::getline(std::cin, id);
            printCourse(trim(id));
        }
        else if (menu == 9)
        {
            std::cout << "Good-bye!\n";
            break;
        }
        else
        {
            std::cout << "Invalid choice.\n";
        }
    }
    return 0;
}

// =================================================================
// loadFile – parses CSV, validates, populates gTable
// =================================================================
bool loadFile(const std::string& fileName)
{
    std::ifstream in(fileName);
    if (!in)
    {
        std::cerr << "ERROR: cannot open \"" << fileName << "\"\n";
        return false;
    }

    std::unordered_map<std::string, Course> temp;
    std::unordered_set<std::string> idSet;
    std::vector<std::string> errors;

    std::string line;
    size_t lineNo = 0;

    // ---------- first pass ------------------------------------------------
    while (std::getline(in, line))
    {
        ++lineNo;
        line = trim(line);
        if (line.empty()) continue;

        std::vector<std::string> tok;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ','))
            tok.emplace_back(trim(cell));

        if (tok.size() < 2)                         // rule A
        {
            errors.emplace_back("Line " + std::to_string(lineNo) + ": < 2 fields");
            continue;
        }

        const std::string& id = tok[0];
        const std::string& title = tok[1];

        if (idSet.count(id))                        // duplicate id
        {
            errors.emplace_back("Line " + std::to_string(lineNo) +
                                ": duplicate course " + id);
            continue;
        }
        idSet.insert(id);

        std::vector<std::string> prereq;
        for (size_t i = 2; i < tok.size(); ++i)
            if (!tok[i].empty()) prereq.push_back(tok[i]);

        temp[id] = Course{id, title, prereq};
    }
    in.close();

    // ---------- second pass – rule B -------------------------------------
    for (const auto& [key, c] : temp)
        for (const std::string& p : c.prereqs)
            if (!idSet.count(p))
                errors.emplace_back("Course " + c.number +
                                    " lists unknown prerequisite " + p);

    // ---------- commit or reject -----------------------------------------
    if (!errors.empty())
    {
        std::cerr << "\nFILE FORMAT ERRORS:\n";
        for (const std::string& e : errors) std::cerr << "  " << e << '\n';
        std::cerr << "DATA NOT LOADED.\n";
        return false;
    }

    gTable.swap(temp);
    std::cout << "Loaded " << gTable.size() << " courses.\n";
    return true;
}

// =================================================================
// printCourse – single lookup
// =================================================================
void printCourse(const std::string& id)
{
    auto it = gTable.find(id);
    if (it == gTable.end())
    {
        std::cout << "Course " << id << " not found.\n";
        return;
    }
    const Course& c = it->second;
    std::cout << c.number << ": " << c.title << '\n';
    if (c.prereqs.empty())
    {
        std::cout << "   Prerequisites: none\n";
    }
    else
    {
        std::cout << "   Prerequisites:\n";
        for (const std::string& p : c.prereqs)
            std::cout << "      " << p << '\n';
    }
}

// =================================================================
// printAll – alphabetical list
// =================================================================
void printAll()
{
    std::vector<const Course*> list;
    list.reserve(gTable.size());
    for (const auto& [id, c] : gTable) list.push_back(&c);

    std::sort(list.begin(), list.end(),
              [](const Course* a, const Course* b)
              { return a->number < b->number; });

    std::cout << "\n---------- FULL COURSE LIST ----------\n";
    for (const Course* c : list)
        std::cout << c->number << " | " << c->title << '\n';
}