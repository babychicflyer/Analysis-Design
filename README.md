The problem we were addressing in this project was the need to print a list of all the computer science courses in alphanumeric order. For each course, we needed to display its title along with its prerequisites. We utilized Big O notation to determine which data structure would be most suitable for organizing and displaying the list effectively. 

Additionally, this project allowed us to examine how our approach to designing software and developing programs has evolved. We also considered how our work has influenced the way we write code that is maintainable, readable, and adaptable.
During this course, we learned about Vectors, Linked Lists, Hash tables, and Binary searches. In order to find the best fit, I read about each one of these structures in order to know and utilize them. This was an important step because we needed to know how to store the .csv file.  

I treated the assignment as a miniature requirements-analysis exercise:

a. Data format → record structure → Course { number, title, prereqs }

b. Advisor queries → performance targets

  • “find one course” should be very fast (ideally O(1))
  
  • “print all courses A→Z” should not require O(n log²n) every time
  
c. Three candidate containers required by the syllabus—vector, hash table, BST—were prototyped in pseudocode. Only by writing those prototypes and the accompanying Big-O table did it become obvious that:

  • Hash table wins for many random look-ups.
  
  • BST in-order traversal wins for the single “print all” list.
  
  • Vector is simplest but scales worst for look-ups.

That comparison is exactly why data-structure literacy matters: the shape of the data and the pattern of expected operations dictate which container is the right choice, not personal habit.

The hurdles I overcame:

• “Circular dependency” in the CSV: a prerequisite that never appears as its own line.

→ Solved by a two-pass load: first build every Course object, then re-scan to verify that each prerequisite key exists.

• CLion built the executable in a sub-folder, so the program couldn’t find the CSV at run-time.

→ Fixed by adding a one-line debug print (std::filesystem::current_path()) and then adjusting the IDE’s Working directory property.

• Accidentally pasted the CSV file name into the #include list, causing cryptic compile errors.

→ Lesson: keep headers and assets in separate folders; read compiler error messages top-to-bottom.

Before this course I tended to think “get it working first, optimize later.” Writing three parallel versions forced me to quantify why one structure is better for a given workload before writing code. Now I sketch a quick Big-O table at the whiteboard as part of design, even for small tasks.

Impact on Code Quality (maintainability, readability, adaptability)

• Single-responsibility functions: loadFile, printCourse, printAll each do exactly one thing.

• Validation is isolated; the data structure is populated only after the file is declared clean—no half-valid state to debug later.

• Menu logic is independent of the storage choice; swapping vector ↔ hash ↔ BST required changing only the helper functions, not the UI loop.

• Consistent naming (Course, gTable, printAll) and liberal inline comments make the code approachable for the next developer (or for myself six months from now).

• By embedding small utility functions (trim, toUpper) at the top, I eliminated hidden dependencies on third-party libraries, making the program easier to port.
