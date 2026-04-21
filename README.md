# COMPUTATIONAL-DISCRETE-SYSTEM
A Computational Discrete System is a menu-driven program that implements core concepts of discrete mathematics such as sets, logic, graphs, and relations. It allows users to perform operations like set manipulation, predicate evaluation, graph algorithms (Kruskal, Dijkstra), and function/relation analysis in an interactive way.
Computational Discrete System
Technical Documentation and User Guide
Overview
Computational Discrete System is a C++ console application implementing fundamental concepts from discrete mathematics and graph theory. The system provides an interactive, menu-driven interface organized into eight functional modules, each addressing a distinct area of discrete structures. Users may interact with any module by entering data manually, or drive the entire analysis from a structured configuration file.
The codebase applies three core object-oriented principles throughout: encapsulation, where all data members are private with access controlled through public methods; abstraction, where implementation details such as binary search, dynamic resizing, union-find, and path reconstruction are hidden behind clean interfaces; and templates, where the generic classes DiscreteSet<T> and PredicateEngine<T> support any comparable type without code duplication.
Modules
Module 1: Discrete Set Operations
Implements a generic sorted set using a dynamic array with binary search for O(log n) membership testing and insertion positioning. The template class DiscreteSet<T> maintains elements in sorted order and resizes its internal buffer automatically when capacity is reached.
Supported operations:
•	Union, intersection, difference, and symmetric difference of two sets
•	Membership testing for individual elements
•	Element insertion and deletion with maintained sort order
Module 2: Predicate Logic Engine
Uses the PredicateEngine<T> class to evaluate logical quantifiers over a finite discrete universe. Built-in predicates include isEven, isPositive, and greaterThan5.
Supported evaluations:
•	Universal quantification: for all x, P(x)
•	Existential quantification: there exists x such that P(x)
•	Unique existential quantification: there exists exactly one x such that P(x)
•	Nested quantifier: for all x, there exists y such that R(x, y)
•	De Morgan's law verification for quantifiers
Module 3: Graph Constraint Builder
Uses DiscreteSet2<int> to model network planning constraints. The GraphConstraintBuilder class computes which nodes in a network are mandatory based on the intersection of critical and high-traffic node sets with the redundancy node set. It can also compute forbidden nodes and verify that a collection of subsets fully covers a given universe.
Module 4: Network Graph Properties
The NetworkGraph class stores an undirected weighted graph using both an adjacency matrix and an adjacency list simultaneously. It performs structural analysis and produces a property report.
Reported properties:
•	Number of vertices and edges
•	Connectivity, verified via depth-first search
•	Completeness
•	Existence of an Euler path or Euler circuit based on vertex degree parity
Module 5: Kruskal MST Algorithm
The KruskalMST class builds a minimum spanning tree using Kruskal's algorithm with a union-find data structure for efficient cycle detection. After construction, the algorithm verifies that all user-specified mandatory vertices are present in the resulting tree and reports success or the first missing vertex.
Module 6: Dijkstra Shortest Path
The DijkstraGraph class implements Dijkstra's shortest path algorithm on a directed weighted graph using an adjacency list with per-edge weights. Given a source and destination vertex, it outputs the shortest total distance and reconstructs the full path by tracing parent pointers.
Module 7: Binary Relation Analyzer
The BinaryRelation1 class stores a relation over a base set as a dynamic collection of ordered pairs and tests its algebraic properties.
Tested properties:
•	Reflexivity, symmetry, antisymmetry, and transitivity
•	Classification as an equivalence relation
•	Classification as a partial order or total order
Module 8: Function Properties Analyzer
The DiscreteFunction class maps a finite domain to a codomain and classifies the function based on its mapping properties.
Tested classifications:
•	Total: every domain element has a defined mapping
•	Injective (one-to-one): no two domain elements map to the same output
•	Surjective (onto): every codomain element is reached by some mapping
•	Bijective: both injective and surjective
Usage
After launching, the main menu presents 12 options. Enter a number and follow the prompts. All inputs are entered via standard input.
Menu options:
•	1. Discrete Set Operations
•	2. Predicate Logic Engine
•	3. Graph Constraint Builder
•	4. Network Graph Properties
•	5. Kruskal MST Algorithm
•	6. Dijkstra Shortest Path
•	7. Binary Relation Analyzer
•	8. Function Properties Analyzer
•	9. Save Results to File
•	10. Load Data from File
•	11. Load Configuration File and Run Full Analysis
•	12. Exit
Configuration File Format
Option 11 accepts a plain-text configuration file that drives all modules without manual input. The first line specifies the execution mode. A value of RUN_ALL runs every module listed in the file automatically. A value of ASK_USER prompts the user before executing each module.
Lines beginning with the # character are treated as comments and skipped. Each module block begins with a keyword on its own line, followed immediately by the data that module requires.
SET_OPERATIONS
Two lines follow the keyword. Each line begins with a count n, then n space-separated integers representing the set elements.
PREDICATE_LOGIC
One line follows the keyword: a count n followed by n integers defining the universe.
GRAPH_CONSTRAINT
Three lines follow: one for critical infrastructure node IDs, one for high-traffic node IDs, and one for redundancy node IDs.
NETWORK_GRAPH
First line: number of vertices V. Second line: number of edges E. Then E lines each containing three integers: source vertex u, destination vertex v, and edge weight w.
KRUSKAL_MST
First line: V. Second line: E. Then E edge lines (u v w). Then a line with the count m of mandatory vertices, followed by m vertex IDs on the next line. Final line: the root vertex.
DIJKSTRA
First line: V. Second line: E. Then E directed edge lines (u v w). Final line: two integers representing the source and destination vertices.
BINARY_RELATION
First line: count n followed by n base set elements. Second line: count p of relation pairs. Then p lines each with two integers x and y representing the pair (x, y).
FUNCTION_ANALYZER
First line: count followed by domain elements. Second line: count followed by codomain elements. Then one line per domain element: the input value and its mapped output value.
File Input and Output
Option 9 writes accumulated output content to a specified text file. Option 10 reads and displays the full contents of any specified file to the console. Option 11 parses a structured configuration file and runs all listed modules in sequence.
