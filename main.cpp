#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

// Add these function declarations at the top after includes
void saveToFile(const string& filename, const string& content);
void loadFromFile(const string& filename);
void parseConfigurationFile(const char* filename);
void runFullAnalysisFromConfig(const string& filename);  // NEW FUNCTION

/* OOP Concepts Used:
 * 1. ENCAPSULATION - Private data members with public methods
 * 2. ABSTRACTION - Hidden implementation details
 * 3. TEMPLATES - Generic programming for type flexibility
 */

template<typename T>
class DiscreteSet {
private:
    T* arr;
    int capacity;
    int count;

    void resize() {
        capacity *= 2;
        T* newArr = new T[capacity];
        for (int i = 0; i < count; i++) {
            newArr[i] = arr[i];
        }
        delete[] arr;
        arr = newArr;
    }

    int binarySearch(T element) const {
        int left = 0, right = count - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid] == element) return mid;
            if (arr[mid] < element) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }

    int findInsertPos(T element) const {
        int left = 0, right = count - 1, pos = count;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid] > element) {
                pos = mid;
                right = mid - 1;
            }
            else {
                left = mid + 1;
            }
        }
        return pos;
    }

public:
    DiscreteSet() : capacity(10), count(0) {
        arr = new T[capacity];
    }

    DiscreteSet(const DiscreteSet& other) : capacity(other.capacity), count(other.count) {
        arr = new T[capacity];
        for (int i = 0; i < count; i++) {
            arr[i] = other.arr[i];
        }
    }

    ~DiscreteSet() {
        delete[] arr;
    }

    void add(T element) {
        if (member(element)) return;
        if (count == capacity) resize();
        int pos = findInsertPos(element);
        for (int i = count; i > pos; i--) {
            arr[i] = arr[i - 1];
        }
        arr[pos] = element;
        count++;
    }

    void delete_element(T element) {
        int pos = binarySearch(element);
        if (pos == -1) return;
        for (int i = pos; i < count - 1; i++) {
            arr[i] = arr[i + 1];
        }
        count--;
    }

    bool member(T element) const {
        return binarySearch(element) != -1;
    }

    int size() const {
        return count;
    }

    bool empty() const {
        return count == 0;
    }

    void clear_all() {
        count = 0;
    }

    T get(int index) const {
        if (index < 0 || index >= count) {
            throw "Index out of bounds";
        }
        return arr[index];
    }

    DiscreteSet<T> set_union(const DiscreteSet<T>& other) const {
        DiscreteSet<T> result = *this;
        for (int i = 0; i < other.count; i++) {
            result.add(other.arr[i]);
        }
        return result;
    }

    DiscreteSet<T> set_intersection(const DiscreteSet<T>& other) const {
        DiscreteSet<T> result;
        for (int i = 0; i < count; i++) {
            if (other.member(arr[i])) {
                result.add(arr[i]);
            }
        }
        return result;
    }

    DiscreteSet<T> set_difference(const DiscreteSet<T>& other) const {
        DiscreteSet<T> result;
        for (int i = 0; i < count; i++) {
            if (!other.member(arr[i])) {
                result.add(arr[i]);
            }
        }
        return result;
    }

    DiscreteSet<T> symmetric_diff(const DiscreteSet<T>& other) const {
        DiscreteSet<T> diff1 = set_difference(other);
        DiscreteSet<T> diff2 = other.set_difference(*this);
        return diff1.set_union(diff2);
    }

    friend ostream& operator<<(ostream& os, const DiscreteSet<T>& s) {
        os << "{";
        for (int i = 0; i < s.count; i++) {
            os << s.arr[i];
            if (i < s.count - 1) os << ",";
        }
        os << "}";
        return os;
    }

    friend istream& operator>>(istream& is, DiscreteSet<T>& s) {
        int n;
        is >> n;
        s.clear_all();
        for (int i = 0; i < n; i++) {
            T element;
            is >> element;
            s.add(element);
        }
        return is;
    }
};

/* MODULE 1.2 */
template<typename T>
class PredicateEngine {
public:
    template<typename Pred>
    static bool forall(const DiscreteSet<T>& universe, Pred P) {
        for (int i = 0; i < universe.size(); i++) {
            if (!P(universe.get(i))) return false;
        }
        return true;
    }

    template<typename Pred>
    static bool exists(const DiscreteSet<T>& universe, Pred P) {
        for (int i = 0; i < universe.size(); i++) {
            if (P(universe.get(i))) return true;
        }
        return false;
    }

    static bool exists_unique(const DiscreteSet<T>& universe, bool (*P)(const T&)) {
        int trueCount = 0;
        for (int i = 0; i < universe.size(); i++) {
            if (P(universe.get(i))) {
                trueCount++;
                if (trueCount > 1) return false;
            }
        }
        return trueCount == 1;
    }

    template<typename U>
    static bool forall_exists(
        const DiscreteSet<T>& set_A,
        const DiscreteSet<U>& set_B,
        bool (*relation_R)(const T&, const U&)
    ) {
        for (int i = 0; i < set_A.size(); i++) {
            bool foundY = false;
            for (int j = 0; j < set_B.size(); j++) {
                if (relation_R(set_A.get(i), set_B.get(j))) {
                    foundY = true;
                    break;
                }
            }
            if (!foundY) return false;
        }
        return true;
    }

    static bool verify_not_forall_equals_exists_not(
        const DiscreteSet<T>& universe,
        bool (*P)(const T&)
    ) {
        bool notForall = !forall(universe, P);
        struct NotPredicate {
            bool (*P)(const T&);
            bool operator()(const T& x) const { return !P(x); }
        };
        NotPredicate notP = { P };
        bool existsNot = exists(universe, notP);
        return notForall == existsNot;
    }

    static bool verify_not_exists_equals_forall_not(
        const DiscreteSet<T>& universe,
        bool (*P)(const T&)
    ) {
        bool notExists = !exists(universe, P);

        struct NotPredicate {
            bool (*P)(const T&);
            bool operator()(const T& x) const { return !P(x); }
        };
        NotPredicate notP = { P };
        bool forallNot = forall(universe, notP);
        return notExists == forallNot;
    }
};

// Predicates
bool isEven(const int& x) { return x % 2 == 0; }
bool isPositive(const int& x) { return x > 0; }
bool greaterThan5(const int& x) { return x > 5; }
bool isDivisible(const int& x, const int& y) { return y != 0 && x % y == 0; }

/* MODULE 1.3 */

template<typename T>
class DiscreteSet2 {
private:
    T* arr;
    int capacity;
    int count;

    void resize() {
        capacity *= 2;
        T* newArr = new T[capacity];
        for (int i = 0; i < count; i++) newArr[i] = arr[i];
        delete[] arr;
        arr = newArr;
    }

    int binarySearch(T element) const {
        int left = 0, right = count - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid] == element) return mid;
            if (arr[mid] < element) left = mid + 1;
            else right = mid - 1;
        }
        return -1;
    }

    int findInsertPos(T element) const {
        int left = 0, right = count - 1, pos = count;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (arr[mid] > element) {
                pos = mid;
                right = mid - 1;
            }
            else {
                left = mid + 1;
            }
        }
        return pos;
    }

public:
    DiscreteSet2() : capacity(10), count(0) {
        arr = new T[capacity];
    }

    DiscreteSet2(const DiscreteSet2& other) : capacity(other.capacity), count(other.count) {
        arr = new T[capacity];
        for (int i = 0; i < count; i++) arr[i] = other.arr[i];
    }

    ~DiscreteSet2() { delete[] arr; }

    void add(T element) {
        if (member(element)) return;
        if (count == capacity) resize();
        int pos = findInsertPos(element);
        for (int i = count; i > pos; i--) arr[i] = arr[i - 1];
        arr[pos] = element;
        count++;
    }

    void delete_element(T element) {
        int pos = binarySearch(element);
        if (pos == -1) return;
        for (int i = pos; i < count - 1; i++) arr[i] = arr[i + 1];
        count--;
    }

    bool member(T element) const { return binarySearch(element) != -1; }
    int size() const { return count; }
    bool empty() const { return count == 0; }
    void clear_all() { count = 0; }

    T get(int index) const {
        if (index < 0 || index >= count) throw "Index out of bounds";
        return arr[index];
    }

    DiscreteSet2<T> set_union(const DiscreteSet2<T>& other) const {
        DiscreteSet2<T> result;
        for (int i = 0; i < count; i++) result.add(arr[i]);
        for (int i = 0; i < other.count; i++) result.add(other.arr[i]);
        return result;
    }

    DiscreteSet2<T> set_intersection(const DiscreteSet2<T>& other) const {
        DiscreteSet2<T> result;
        for (int i = 0; i < count; i++)
            if (other.member(arr[i])) result.add(arr[i]);
        return result;
    }

    DiscreteSet2<T> set_difference(const DiscreteSet2<T>& other) const {
        DiscreteSet2<T> result;
        for (int i = 0; i < count; i++)
            if (!other.member(arr[i])) result.add(arr[i]);
        return result;
    }

    DiscreteSet2<T> symmetric_diff(const DiscreteSet2<T>& other) const {
        return set_difference(other).set_union(other.set_difference(*this));
    }

    bool is_subset(const DiscreteSet2<T>& other) const {
        for (int i = 0; i < count; i++)
            if (!other.member(arr[i])) return false;
        return true;
    }

    bool is_strict_subset(const DiscreteSet2<T>& other) const {
        return is_subset(other) && count < other.count;
    }


    DiscreteSet2<DiscreteSet2<T> > powerset() const {
        DiscreteSet2<DiscreteSet2<T> > result;
        int psize = 1 << count;
        for (int i = 0; i < psize; i++) {
            DiscreteSet2<T> subset;
            for (int j = 0; j < count; j++)
                if (i & (1 << j)) subset.add(arr[j]);
            result.add(subset);
        }
        return result;
    }


    struct Pair {
        T first;
        T second;
        Pair(T f, T s) : first(f), second(s) {}
        bool operator<(const Pair& other) const {
            if (first != other.first) return first < other.first;
            return second < other.second;
        }
    };


    DiscreteSet2<Pair> cartesian_prod(const DiscreteSet2<T>& other) const {
        DiscreteSet2<Pair> result;
        for (int i = 0; i < count; i++)
            for (int j = 0; j < other.count; j++)
                result.add(Pair(arr[i], other.arr[j]));
        return result;
    }

    bool operator==(const DiscreteSet2<T>& other) const {
        if (count != other.count) return false;
        for (int i = 0; i < count; i++)
            if (arr[i] != other.arr[i]) return false;
        return true;
    }

    bool operator<(const DiscreteSet2<T>& other) const {
        int minSize = count < other.count ? count : other.count;
        for (int i = 0; i < minSize; i++) {
            if (arr[i] < other.arr[i]) return true;
            if (arr[i] > other.arr[i]) return false;
        }
        return count < other.count;
    }

    friend ostream& operator<<(ostream& os, const DiscreteSet2<T>& s) {
        os << "{";
        for (int i = 0; i < s.count; i++) {
            os << s.arr[i];
            if (i < s.count - 1) os << ",";
        }
        os << "}";
        return os;
    }

    friend ostream& operator<<(ostream& os, const typename DiscreteSet2<T>::Pair& p) {
        os << "(" << p.first << "," << p.second << ")";
        return os;
    }

    friend istream& operator>>(istream& is, DiscreteSet2<T>& s) {
        int n;
        is >> n;
        s.clear_all();
        for (int i = 0; i < n; i++) {
            T element;
            is >> element;
            s.add(element);
        }
        return is;
    }
};

class GraphConstraintBuilder {
public:
    static DiscreteSet2<int> compute_mandatory_nodes(
        const DiscreteSet2<int>& critical_infrastructure,
        const DiscreteSet2<int>& high_traffic,
        const DiscreteSet2<int>& redundancy_nodes
    ) {
        return critical_infrastructure.set_union(high_traffic)
            .set_intersection(redundancy_nodes);
    }

    static DiscreteSet2<int> compute_forbidden_nodes(
        const DiscreteSet2<int>& all_nodes,
        const DiscreteSet2<int>& mandatory_nodes,
        const DiscreteSet2<int>& faulty_nodes
    ) {
        return faulty_nodes.set_union(all_nodes.set_difference(mandatory_nodes));
    }


    static bool verify_coverage(
        const DiscreteSet2<int>& total_universe,
        const DiscreteSet2<DiscreteSet2<int> >& coverage_sets
    ) {
        DiscreteSet2<int> union_coverage;
        for (int i = 0; i < coverage_sets.size(); i++)
            union_coverage = union_coverage.set_union(coverage_sets.get(i));

        return union_coverage == total_universe;
    }
};

const int MAX = 20;

class NetworkGraph {
private:
    int n;
    int adjMatrix[MAX][MAX];
    int adjList[MAX][MAX];
    int adjListCount[MAX];

public:
    NetworkGraph(int vertices) {
        n = vertices;

        for (int i = 0; i < n; i++) {
            adjListCount[i] = 0;
            for (int j = 0; j < n; j++)
                adjMatrix[i][j] = 0;
        }
    }

    void addEdge(int u, int v, int w) {
        adjMatrix[u][v] = w;
        adjMatrix[v][u] = w;

        adjList[u][adjListCount[u]++] = v;
        adjList[v][adjListCount[v]++] = u;
    }

    int countEdges() {
        int edges = 0;
        for (int i = 0; i < n; i++)
            for (int j = i; j < n; j++)
                if (adjMatrix[i][j] != 0)
                    edges++;
        return edges;
    }

    void dfs(int v, int visited[]) {
        visited[v] = 1;
        for (int i = 0; i < n; i++)
            if (adjMatrix[v][i] != 0 && !visited[i])
                dfs(i, visited);
    }

    bool isConnected() {
        int visited[MAX];
        for (int i = 0; i < n; i++) visited[i] = 0;

        dfs(0, visited);

        for (int i = 0; i < n; i++)
            if (!visited[i])
                return false;

        return true;
    }

    bool isComplete() {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (i != j && adjMatrix[i][j] == 0)
                    return false;

        return true;
    }

    int degree(int v) {
        int d = 0;
        for (int i = 0; i < n; i++)
            if (adjMatrix[v][i] != 0)
                d++;
        return d;
    }

    bool hasEulerPath() {
        if (!isConnected()) return false;

        int odd = 0;
        for (int i = 0; i < n; i++)
            if (degree(i) % 2 != 0)
                odd++;

        return (odd == 0 || odd == 2);
    }

    bool hasEulerCircuit() {
        if (!isConnected()) return false;

        for (int i = 0; i < n; i++)
            if (degree(i) % 2 != 0)
                return false;

        return true;
    }

    void printPropertyReport() {
        cout << "\n--------- GRAPH PROPERTY REPORT ---------\n";
        cout << "Graph Type          : Undirected\n";
        cout << "Number of Vertices  : " << n << endl;
        cout << "Number of Edges     : " << countEdges() << endl;

        cout << "Connected           : ";
        cout << (isConnected() ? "YES" : "NO") << endl;

        cout << "Complete            : ";
        cout << (isComplete() ? "YES" : "NO") << endl;

        cout << "Euler Path          : ";
        cout << (hasEulerPath() ? "YES" : "NO") << endl;

        cout << "Euler Circuit       : ";
        cout << (hasEulerCircuit() ? "YES" : "NO") << endl;

        cout << "------------------------------------------\n";
    }

    void generate_property_report() {
        printPropertyReport();
    }

    void printMatrix() {
        cout << "\nAdjacency Matrix:\n";
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++)
                cout << adjMatrix[i][j] << " ";
            cout << endl;
        }
    }

    void printList() {
        cout << "\nAdjacency List:\n";
        for (int i = 0; i < n; i++) {
            cout << i << " -> ";
            for (int j = 0; j < adjListCount[i]; j++)
                cout << adjList[i][j] << " ";
            cout << endl;
        }
    }
};

struct Edge {
    int u, v, w;
};

class KruskalMST {
private:
    int n;
    int adjMatrix[MAX][MAX];
    int parent[MAX];

public:
    KruskalMST(int vertices) {
        n = vertices;

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                adjMatrix[i][j] = 0;

        for (int i = 0; i < n; i++)
            parent[i] = i;
    }

    void addEdge(int u, int v, int w) {
        adjMatrix[u][v] = w;
        adjMatrix[v][u] = w;
    }

    int findSet(int x) {
        if (parent[x] == x) return x;
        return parent[x] = findSet(parent[x]);
    }

    void unionSet(int a, int b) {
        a = findSet(a);
        b = findSet(b);
        if (a != b) parent[b] = a;
    }

    bool kruskal(int mandatory[], int mandCount, int root_vertex) {
        Edge edges[MAX * MAX];
        int edgeCount = 0;

        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++)
                if (adjMatrix[i][j] != 0) {
                    edges[edgeCount].u = i;
                    edges[edgeCount].v = j;
                    edges[edgeCount].w = adjMatrix[i][j];
                    edgeCount++;
                }

        for (int i = 0; i < edgeCount; i++) {
            for (int j = 0; j < edgeCount - 1; j++) {
                if (edges[j].w > edges[j + 1].w) {
                    Edge temp = edges[j];
                    edges[j] = edges[j + 1];
                    edges[j + 1] = temp;
                }
            }
        }

        cout << "\nMST edges:\n";
        int included[MAX];
        for (int i = 0; i < n; i++) included[i] = 0;

        included[root_vertex] = 1;
        int mstEdges = 0;

        for (int i = 0; i < edgeCount; i++) {
            int u = edges[i].u;
            int v = edges[i].v;

            if (findSet(u) != findSet(v)) {
                unionSet(u, v);
                cout << u << " - " << v << " (" << edges[i].w << ")\n";
                included[u] = included[v] = 1;
                mstEdges++;
            }
        }

        for (int i = 0; i < mandCount; i++) {
            int v = mandatory[i];
            if (included[v] == 0) {
                cout << "\nMST FAILED: Mandatory vertex " << v << " missing.\n";
                return false;
            }
        }

        cout << "\nMST SUCCESS.\n";
        return true;
    }
};

const int INF = 99999;

class DijkstraGraph {
private:
    int n;
    int adjList[MAX][MAX];
    int weight[MAX][MAX];
    int count[MAX];

public:
    DijkstraGraph(int vertices) {
        n = vertices;

        for (int i = 0; i < n; i++) {
            count[i] = 0;
            for (int j = 0; j < n; j++) {
                adjList[i][j] = -1;
                weight[i][j] = 0;
            }
        }
    }

    void addEdge(int u, int v, int w) {
        adjList[u][count[u]] = v;
        weight[u][count[u]] = w;
        count[u]++;
    }

    void dijkstra(int src, int dest) {
        int dist[MAX];
        int visited[MAX];
        int parent[MAX];

        for (int i = 0; i < n; i++) {
            dist[i] = INF;
            visited[i] = 0;
            parent[i] = -1;
        }

        dist[src] = 0;

        for (int step = 0; step < n; step++) {
            int u = -1, minD = INF;

            for (int i = 0; i < n; i++) {
                if (!visited[i] && dist[i] < minD) {
                    minD = dist[i];
                    u = i;
                }
            }

            if (u == -1)
                break;

            visited[u] = 1;

            for (int i = 0; i < count[u]; i++) {
                int v = adjList[u][i];
                int w = weight[u][i];

                if (dist[v] > dist[u] + w) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                }
            }
        }

        if (dist[dest] == INF) {
            cout << "\nNO PATH EXISTS\n";
            return;
        }

        cout << "\nShortest Distance = " << dist[dest] << endl;

        int path[MAX];
        int path_len = 0;

        int temp = dest;
        while (temp != -1) {
            path[path_len++] = temp;
            temp = parent[temp];
        }

        cout << "Shortest Path = ";
        for (int i = path_len - 1; i >= 0; i--) {
            cout << path[i];
            if (i != 0) cout << " -> ";
        }
        cout << endl;
    }
};


struct PairSimple {
    int first;
    int second;
};

class DiscreteSetSingle {
private:
    int* elements;
    int size;
    int capacity;

public:
    DiscreteSetSingle() {
        capacity = 20;
        size = 0;
        elements = new int[capacity];
    }

    // Copy Constructor
    DiscreteSetSingle(const DiscreteSetSingle& other) {
        capacity = other.capacity;
        size = other.size;
        elements = new int[capacity];
        for (int i = 0; i < size; i++) elements[i] = other.elements[i];
    }

    // Assignment Operator
    DiscreteSetSingle& operator=(const DiscreteSetSingle& other) {
        if (this != &other) {
            delete[] elements;
            capacity = other.capacity;
            size = other.size;
            elements = new int[capacity];
            for (int i = 0; i < size; i++) elements[i] = other.elements[i];
        }
        return *this;
    }

    void add(int element) {
        if (!contains(element)) {
            if (size >= capacity) {
                capacity *= 2;
                int* newElements = new int[capacity];
                for (int i = 0; i < size; i++) newElements[i] = elements[i];
                delete[] elements;
                elements = newElements;
            }
            elements[size++] = element;
        }
    }

    bool contains(int element) const {
        for (int i = 0; i < size; i++) if (elements[i] == element) return true;
        return false;
    }

    int getSize() const { return size; }
    int getElement(int index) const { return elements[index]; }

    void display() const {
        cout << "{";
        for (int i = 0; i < size; i++) {
            cout << elements[i];
            if (i < size - 1) cout << ",";
        }
        cout << "}";
    }

    ~DiscreteSetSingle() {
        delete[] elements;
    }
};
class BinaryRelation1 {
private:
    DiscreteSetSingle base_set;
    PairSimple* relation_pairs;
    int pair_count;
    int pair_capacity;

public:
    BinaryRelation1(const DiscreteSetSingle& base) : base_set(base) {
        pair_capacity = 20;
        pair_count = 0;
        relation_pairs = new PairSimple[pair_capacity];
    }

    void insert_pair(int x, int y) {
        if (pair_count >= pair_capacity) {
            pair_capacity *= 2;
            PairSimple* newPairs = new PairSimple[pair_capacity];
            for (int i = 0; i < pair_count; i++) newPairs[i] = relation_pairs[i];
            delete[] relation_pairs;
            relation_pairs = newPairs;
        }
        relation_pairs[pair_count].first = x;
        relation_pairs[pair_count].second = y;
        pair_count++;
    }

    void remove_pair(int x, int y) {
        for (int i = 0; i < pair_count; i++) {
            if (relation_pairs[i].first == x && relation_pairs[i].second == y) {
                for (int j = i; j < pair_count - 1; j++) relation_pairs[j] = relation_pairs[j + 1];
                pair_count--;
                return;
            }
        }
    }

    bool has_pair(int x, int y) const {
        for (int i = 0; i < pair_count; i++)
            if (relation_pairs[i].first == x && relation_pairs[i].second == y) return true;
        return false;
    }


    bool test_reflexive() const {
        for (int i = 0; i < base_set.getSize(); i++) {
            int element = base_set.getElement(i);
            if (!has_pair(element, element)) return false;
        }
        return true;
    }

    bool test_symmetric() const {
        for (int i = 0; i < pair_count; i++) {
            int x = relation_pairs[i].first;
            int y = relation_pairs[i].second;
            if (!has_pair(y, x)) return false;
        }
        return true;
    }

    bool test_antisymmetric() const {
        for (int i = 0; i < pair_count; i++) {
            int x = relation_pairs[i].first;
            int y = relation_pairs[i].second;
            if (x != y && has_pair(y, x)) return false;
        }
        return true;
    }

    bool test_transitive() const {
        for (int i = 0; i < pair_count; i++) {
            for (int j = 0; j < pair_count; j++) {
                int a = relation_pairs[i].first;
                int b = relation_pairs[i].second;
                int c = relation_pairs[j].first;
                int d = relation_pairs[j].second;
                if (b == c) {
                    if (!has_pair(a, d)) return false;
                }
            }
        }
        return true;
    }


    bool is_equivalence() const {
        return test_reflexive() && test_symmetric() && test_transitive();
    }

    bool is_partial_order() const {
        return test_reflexive() && test_antisymmetric() && test_transitive();
    }

    bool is_total_order() const {
        if (!is_partial_order()) return false;
        for (int i = 0; i < base_set.getSize(); i++) {
            for (int j = 0; j < base_set.getSize(); j++) {
                int x = base_set.getElement(i);
                int y = base_set.getElement(j);
                if (!has_pair(x, y) && !has_pair(y, x)) return false;
            }
        }
        return true;
    }

    void display() const {
        cout << "Relation pairs: {";
        for (int i = 0; i < pair_count; i++) {
            cout << "(" << relation_pairs[i].first << "," << relation_pairs[i].second << ")";
            if (i < pair_count - 1) cout << ", ";
        }
        cout << "}\n";
    }

    ~BinaryRelation1() {
        delete[] relation_pairs;
    }
};
struct Mapping {
    int input;
    int output;
};

class DiscreteFunction {
private:
    DiscreteSetSingle input_set;
    DiscreteSetSingle output_set;
    Mapping* function_map;
    int map_count;
    int map_capacity;

public:
    DiscreteFunction(const DiscreteSetSingle& inputs, const DiscreteSetSingle& outputs)
        : input_set(inputs), output_set(outputs) {
        map_capacity = 20;
        map_count = 0;
        function_map = new Mapping[map_capacity];
    }

    void set_mapping(int x, int y) {

        for (int i = 0; i < map_count; i++) {
            if (function_map[i].input == x) {
                function_map[i].output = y;
                return;
            }
        }


        if (map_count >= map_capacity) {
            map_capacity *= 2;
            Mapping* newMap = new Mapping[map_capacity];
            for (int i = 0; i < map_count; i++) newMap[i] = function_map[i];
            delete[] function_map;
            function_map = newMap;
        }
        function_map[map_count].input = x;
        function_map[map_count].output = y;
        map_count++;
    }

    bool has_mapping(int x) const {
        for (int i = 0; i < map_count; i++) if (function_map[i].input == x) return true;
        return false;
    }

    int apply(int x) const {
        for (int i = 0; i < map_count; i++) if (function_map[i].input == x) return function_map[i].output;
        return -1;
    }


    bool is_total_function() const {

        for (int i = 0; i < input_set.getSize(); i++) {
            if (!has_mapping(input_set.getElement(i))) return false;
        }
        return true;
    }

    bool is_injection() const {

        for (int i = 0; i < map_count; i++) {
            for (int j = i + 1; j < map_count; j++) {
                if (function_map[i].output == function_map[j].output) return false;
            }
        }
        return true;
    }

    bool is_surjection() const {

        for (int i = 0; i < output_set.getSize(); i++) {
            int y = output_set.getElement(i);
            bool found = false;
            for (int j = 0; j < map_count; j++) {
                if (function_map[j].output == y) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
        return true;
    }

    bool is_bijection() const {
        return is_injection() && is_surjection();
    }

    void display() const {
        cout << "Function mappings:\n";
        for (int i = 0; i < map_count; i++) {
            cout << "  " << function_map[i].input << " -> " << function_map[i].output << "\n";
        }
    }

    void display_compact() const {
        cout << "maps: ";
        for (int i = 0; i < map_count; i++) {
            cout << function_map[i].input << "->" << function_map[i].output;
            if (i < map_count - 1) cout << ", ";
        }
        cout << "\n";
    }

    ~DiscreteFunction() {
        delete[] function_map;
    }
};

void saveToFile(const string& filename, const string& content) {
    ofstream file(filename.c_str());
    if (!file) {
        cout << "Error: Cannot open file " << filename << " for writing.\n";
        return;
    }
    file << content;
    file.close();
    cout << "Data saved to " << filename << " successfully.\n";
}

void loadFromFile(const string& filename) {
    ifstream file(filename.c_str());
    if (!file) {
        cout << "Error: Cannot open file " << filename << " for reading.\n";
        return;
    }

    string line;
    cout << "\n=== Content of " << filename << " ===\n";
    while (getline(file, line)) {
        cout << line << endl;
    }
    file.close();
    cout << "=== End of file ===\n";
}

void parseConfigurationFile(const char* filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open configuration file " << filename << endl;
        return;
    }

    cout << "\n=== Parsing Configuration File: " << filename << " ===\n";

    string line;
    int lineNum = 0;
    while (getline(file, line)) {
        lineNum++;
        if (line.empty() || line[0] == '#') continue;

        cout << "Line " << lineNum << ": " << line << endl;


        if (line.find("SET") != string::npos) {
            cout << "  Detected SET operation\n";
        }
        else if (line.find("GRAPH") != string::npos) {
            cout << "  Detected GRAPH operation\n";
        }
        else if (line.find("RELATION") != string::npos) {
            cout << "  Detected RELATION operation\n";
        }
    }

    file.close();
    cout << "=== Configuration parsing complete ===\n";
}

// Helper function to parse integers from a string
void parseIntsFromLine(const string& line, int* values, int& count) {
    count = 0;
    const char* str = line.c_str();
    int len = line.length();
    int pos = 0;

    while (pos < len) {
        // Skip spaces
        while (pos < len && (str[pos] == ' ' || str[pos] == '\t')) pos++;

        if (pos >= len) break;

        // Parse number
        int num = 0;
        bool isNegative = false;

        if (str[pos] == '-') {
            isNegative = true;
            pos++;
        }

        while (pos < len && str[pos] >= '0' && str[pos] <= '9') {
            num = num * 10 + (str[pos] - '0');
            pos++;
        }

        if (isNegative) num = -num;

        values[count++] = num;
    }
}

// NEW FUNCTION: Run full analysis from configuration file
void runFullAnalysisFromConfig(const string& filename) {
    ifstream file(filename.c_str());
    if (!file) {
        cout << "Error: Cannot open configuration file " << filename << endl;
        return;
    }

    cout << "\n=========================================\n";
    cout << "   RUNNING FULL ANALYSIS FROM CONFIG FILE\n";
    cout << "=========================================\n";

    string line;
    int analysisMode = 0; // 0=all, 1=ask user

    // First line: Analysis mode
    if (getline(file, line)) {
        if (line.find("ASK_USER") != string::npos) {
            analysisMode = 1;
            cout << "Mode: Ask user for each module\n";
        }
        else {
            analysisMode = 0;
            cout << "Mode: Run all modules automatically\n";
        }
    }

    // Process each module
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        // Check what module to run
        if (line.find("SET_OPERATIONS") != string::npos) {
            bool runThis = (analysisMode == 0);
            if (analysisMode == 1) {
                cout << "\nRun Discrete Set Operations? (y/n): ";
                char choice;
                cin >> choice;
                runThis = (choice == 'y' || choice == 'Y');
            }

            if (runThis) {
                cout << "\n=== MODULE 1: Discrete Set Operations ===\n";

                // Read set data
                DiscreteSet<int> setA, setB;

                // Read set A
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);

                    if (count > 0) {
                        int n = values[0];
                        setA.clear_all();
                        for (int i = 1; i <= n && i < count; i++) {
                            setA.add(values[i]);
                        }
                    }
                }

                // Read set B
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);

                    if (count > 0) {
                        int n = values[0];
                        setB.clear_all();
                        for (int i = 1; i <= n && i < count; i++) {
                            setB.add(values[i]);
                        }
                    }
                }

                cout << "Set A: " << setA << endl;
                cout << "Set B: " << setB << endl;
                cout << "Union (A ∪ B): " << setA.set_union(setB) << endl;
                cout << "Intersection (A ∩ B): " << setA.set_intersection(setB) << endl;
                cout << "Difference (A \\ B): " << setA.set_difference(setB) << endl;
                cout << "Symmetric Difference (A Δ B): " << setA.symmetric_diff(setB) << endl;

                // Check membership for a sample element
                if (!setA.empty()) {
                    int sample = setA.get(0);
                    cout << "Membership check for " << sample << ":" << endl;
                    cout << "  In Set A: " << (setA.member(sample) ? "Yes" : "No") << endl;
                    cout << "  In Set B: " << (setB.member(sample) ? "Yes" : "No") << endl;
                }
            }
        }

        else if (line.find("PREDICATE_LOGIC") != string::npos) {
            bool runThis = (analysisMode == 0);
            if (analysisMode == 1) {
                cout << "\nRun Predicate Logic Engine? (y/n): ";
                char choice;
                cin >> choice;
                runThis = (choice == 'y' || choice == 'Y');
            }

            if (runThis) {
                cout << "\n=== MODULE 2: Predicate Logic Engine ===\n";

                DiscreteSet<int> universe;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);

                    if (count > 0) {
                        int n = values[0];
                        universe.clear_all();
                        for (int i = 1; i <= n && i < count; i++) {
                            universe.add(values[i]);
                        }
                    }
                }

                cout << "Universe: " << universe << endl;
                cout << "∀x (x is even): " << (PredicateEngine<int>::forall(universe, isEven) ? "TRUE" : "FALSE") << endl;
                cout << "∃x (x > 5): " << (PredicateEngine<int>::exists(universe, greaterThan5) ? "TRUE" : "FALSE") << endl;
                cout << "∃!x (x is positive): " << (PredicateEngine<int>::exists_unique(universe, isPositive) ? "TRUE" : "FALSE") << endl;
            }
        }

        else if (line.find("GRAPH_CONSTRAINT") != string::npos) {
            bool runThis = (analysisMode == 0);
            if (analysisMode == 1) {
                cout << "\nRun Graph Constraint Builder? (y/n): ";
                char choice;
                cin >> choice;
                runThis = (choice == 'y' || choice == 'Y');
            }

            if (runThis) {
                cout << "\n=== MODULE 3: Graph Constraint Builder ===\n";

                DiscreteSet2<int> critical, traffic, redundancy;

                // Read critical infrastructure
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);

                    if (count > 0) {
                        critical.clear_all();
                        for (int i = 0; i < count; i++) {
                            critical.add(values[i]);
                        }
                    }
                }

                // Read high traffic
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);

                    if (count > 0) {
                        traffic.clear_all();
                        for (int i = 0; i < count; i++) {
                            traffic.add(values[i]);
                        }
                    }
                }

                // Read redundancy nodes
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);

                    if (count > 0) {
                        redundancy.clear_all();
                        for (int i = 0; i < count; i++) {
                            redundancy.add(values[i]);
                        }
                    }
                }

                cout << "Critical Infrastructure: " << critical << endl;
                cout << "High Traffic: " << traffic << endl;
                cout << "Redundancy Nodes: " << redundancy << endl;

                DiscreteSet2<int> mandatory = GraphConstraintBuilder::compute_mandatory_nodes(critical, traffic, redundancy);
                cout << "Mandatory Nodes: " << mandatory << endl;
            }
        }

        else if (line.find("NETWORK_GRAPH") != string::npos) {
            bool runThis = (analysisMode == 0);
            if (analysisMode == 1) {
                cout << "\nRun Network Graph Properties? (y/n): ";
                char choice;
                cin >> choice;
                runThis = (choice == 'y' || choice == 'Y');
            }

            if (runThis) {
                cout << "\n=== MODULE 4: Network Graph Properties ===\n";

                int V = 0;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);
                    if (count > 0) V = values[0];
                }

                if (V <= 0) {
                    cout << "Invalid number of vertices: " << V << endl;
                    continue;
                }

                NetworkGraph g(V);

                int E = 0;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);
                    if (count > 0) E = values[0];
                }

                for (int i = 0; i < E; i++) {
                    if (getline(file, line)) {
                        int values[100];
                        int count = 0;
                        parseIntsFromLine(line, values, count);

                        if (count >= 3) {
                            int u = values[0];
                            int v = values[1];
                            int w = values[2];
                            g.addEdge(u, v, w);
                        }
                    }
                }

                g.generate_property_report();
            }
        }

        else if (line.find("KRUSKAL_MST") != string::npos) {
            bool runThis = (analysisMode == 0);
            if (analysisMode == 1) {
                cout << "\nRun Kruskal MST Algorithm? (y/n): ";
                char choice;
                cin >> choice;
                runThis = (choice == 'y' || choice == 'Y');
            }

            if (runThis) {
                cout << "\n=== MODULE 5: Kruskal MST Algorithm ===\n";

                int V = 0;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);
                    if (count > 0) V = values[0];
                }

                if (V <= 0) {
                    cout << "Invalid number of vertices: " << V << endl;
                    continue;
                }

                KruskalMST g(V);

                int E = 0;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);
                    if (count > 0) E = values[0];
                }

                for (int i = 0; i < E; i++) {
                    if (getline(file, line)) {
                        int values[100];
                        int count = 0;
                        parseIntsFromLine(line, values, count);

                        if (count >= 3) {
                            int u = values[0];
                            int v = values[1];
                            int w = values[2];
                            g.addEdge(u, v, w);
                        }
                    }
                }

                int m = 0;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);
                    if (count > 0) m = values[0];
                }

                int* mandatory = new int[m];
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);

                    for (int i = 0; i < m && i < count; i++) {
                        mandatory[i] = values[i];
                    }
                }

                int root = 0;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);
                    if (count > 0) root = values[0];
                }

                g.kruskal(mandatory, m, root);
                delete[] mandatory;
            }
        }

        else if (line.find("DIJKSTRA") != string::npos) {
            bool runThis = (analysisMode == 0);
            if (analysisMode == 1) {
                cout << "\nRun Dijkstra Shortest Path? (y/n): ";
                char choice;
                cin >> choice;
                runThis = (choice == 'y' || choice == 'Y');
            }

            if (runThis) {
                cout << "\n=== MODULE 6: Dijkstra Shortest Path ===\n";

                int V = 0;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);
                    if (count > 0) V = values[0];
                }

                if (V <= 0) {
                    cout << "Invalid number of vertices: " << V << endl;
                    continue;
                }

                DijkstraGraph g(V);

                int E = 0;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);
                    if (count > 0) E = values[0];
                }

                for (int i = 0; i < E; i++) {
                    if (getline(file, line)) {
                        int values[100];
                        int count = 0;
                        parseIntsFromLine(line, values, count);

                        if (count >= 3) {
                            int u = values[0];
                            int v = values[1];
                            int w = values[2];
                            g.addEdge(u, v, w);
                        }
                    }
                }

                int src = 0, dest = 0;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);
                    if (count >= 2) {
                        src = values[0];
                        dest = values[1];
                    }
                }

                g.dijkstra(src, dest);
            }
        }

        else if (line.find("BINARY_RELATION") != string::npos) {
            bool runThis = (analysisMode == 0);
            if (analysisMode == 1) {
                cout << "\nRun Binary Relation Analyzer? (y/n): ";
                char choice;
                cin >> choice;
                runThis = (choice == 'y' || choice == 'Y');
            }

            if (runThis) {
                cout << "\n=== MODULE 7: Binary Relation Analyzer ===\n";

                DiscreteSetSingle baseSet;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);

                    if (count > 0) {
                        int n = values[0];
                        for (int i = 1; i <= n && i < count; i++) {
                            baseSet.add(values[i]);
                        }
                    }
                }

                BinaryRelation1 rel(baseSet);

                int p = 0;
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);
                    if (count > 0) p = values[0];
                }

                for (int i = 0; i < p; i++) {
                    if (getline(file, line)) {
                        int values[100];
                        int count = 0;
                        parseIntsFromLine(line, values, count);

                        if (count >= 2) {
                            int x = values[0];
                            int y = values[1];
                            rel.insert_pair(x, y);
                        }
                    }
                }

                rel.display();
                cout << "Reflexive: " << (rel.test_reflexive() ? "YES" : "NO") << endl;
                cout << "Symmetric: " << (rel.test_symmetric() ? "YES" : "NO") << endl;
                cout << "Antisymmetric: " << (rel.test_antisymmetric() ? "YES" : "NO") << endl;
                cout << "Transitive: " << (rel.test_transitive() ? "YES" : "NO") << endl;
                cout << "Equivalence Relation: " << (rel.is_equivalence() ? "YES" : "NO") << endl;
                cout << "Partial Order: " << (rel.is_partial_order() ? "YES" : "NO") << endl;
                cout << "Total Order: " << (rel.is_total_order() ? "YES" : "NO") << endl;
            }
        }

        else if (line.find("FUNCTION_ANALYZER") != string::npos) {
            bool runThis = (analysisMode == 0);
            if (analysisMode == 1) {
                cout << "\nRun Function Properties Analyzer? (y/n): ";
                char choice;
                cin >> choice;
                runThis = (choice == 'y' || choice == 'Y');
            }

            if (runThis) {
                cout << "\n=== MODULE 8: Function Properties Analyzer ===\n";

                DiscreteSetSingle domain, codomain;

                // Read domain
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);

                    if (count > 0) {
                        int nx = values[0];
                        for (int i = 1; i <= nx && i < count; i++) {
                            domain.add(values[i]);
                        }
                    }
                }

                // Read codomain
                if (getline(file, line)) {
                    int values[100];
                    int count = 0;
                    parseIntsFromLine(line, values, count);

                    if (count > 0) {
                        int ny = values[0];
                        for (int i = 1; i <= ny && i < count; i++) {
                            codomain.add(values[i]);
                        }
                    }
                }

                DiscreteFunction func(domain, codomain);

                // Read mappings
                for (int i = 0; i < domain.getSize(); i++) {
                    if (getline(file, line)) {
                        int values[100];
                        int count = 0;
                        parseIntsFromLine(line, values, count);

                        if (count >= 2) {
                            int x = values[0];
                            int y = values[1];
                            func.set_mapping(x, y);
                        }
                    }
                }

                func.display();
                cout << "Total Function: " << (func.is_total_function() ? "YES" : "NO") << endl;
                cout << "Injective (One-to-One): " << (func.is_injection() ? "YES" : "NO") << endl;
                cout << "Surjective (Onto): " << (func.is_surjection() ? "YES" : "NO") << endl;
                cout << "Bijective: " << (func.is_bijection() ? "YES" : "NO") << endl;
            }
        }
    }

    file.close();
    cout << "\n=========================================\n";
    cout << "   FULL ANALYSIS COMPLETE\n";
    cout << "=========================================\n";
}

void clearInputBuffer() {
    cin.clear();
    while (cin.get() != '\n') {

    }
}

void displayMainMenu() {
    cout << "\n=========================================\n";
    cout << "      COMPUTATIONAL DISCRETE SYSTEM\n";
    cout << "=========================================\n";
    cout << "MAIN MENU:\n";
    cout << "1. Discrete Set Operations\n";
    cout << "2. Predicate Logic Engine\n";
    cout << "3. Graph Constraint Builder\n";
    cout << "4. Network Graph Properties\n";
    cout << "5. Kruskal MST Algorithm\n";
    cout << "6. Dijkstra Shortest Path\n";
    cout << "7. Binary Relation Analyzer\n";
    cout << "8. Function Properties Analyzer\n";
    cout << "9. Save Results to File\n";
    cout << "10. Load Data from File\n";
    cout << "11. Load Configuration File and Run Full Analysis\n";
    cout << "12. Exit\n";
    cout << "Enter your choice (1-12): ";
}

int main() {
    int choice;
    string filename;
    string outputContent;

    do {
        displayMainMenu();
        cin >> choice;

        if (cin.fail()) {
            cout << "Invalid input! Please enter a number.\n";
            clearInputBuffer();
            continue;
        }

        switch (choice) {
        case 1: {
            int subChoice;
            DiscreteSet<int> setA, setB;

            do {
                cout << "\n=== Discrete Set Operations ===\n";
                cout << "1. Input Set A\n";
                cout << "2. Input Set B\n";
                cout << "3. Display Sets\n";
                cout << "4. Union (A ∪ B)\n";
                cout << "5. Intersection (A ∩ B)\n";
                cout << "6. Difference (A \\ B)\n";
                cout << "7. Symmetric Difference (A Δ B)\n";
                cout << "8. Check Membership\n";
                cout << "9. Back to Main Menu\n";
                cout << "Enter option: ";
                cin >> subChoice;

                switch (subChoice) {
                case 1: {
                    cout << "Enter number of elements for Set A: ";
                    int n;
                    cin >> n;
                    setA.clear_all();
                    cout << "Enter " << n << " elements: ";
                    for (int i = 0; i < n; i++) {
                        int val;
                        cin >> val;
                        setA.add(val);
                    }
                    break;
                }
                case 2: {
                    cout << "Enter number of elements for Set B: ";
                    int n;
                    cin >> n;
                    setB.clear_all();
                    cout << "Enter " << n << " elements: ";
                    for (int i = 0; i < n; i++) {
                        int val;
                        cin >> val;
                        setB.add(val);
                    }
                    break;
                }
                case 3: {
                    cout << "Set A: " << setA << endl;
                    cout << "Set B: " << setB << endl;
                    break;
                }
                case 4: {
                    cout << "A ∪ B = " << setA.set_union(setB) << endl;
                    break;
                }
                case 5: {
                    cout << "A ∩ B = " << setA.set_intersection(setB) << endl;
                    break;
                }
                case 6: {
                    cout << "A \\ B = " << setA.set_difference(setB) << endl;
                    break;
                }
                case 7: {
                    cout << "A Δ B = " << setA.symmetric_diff(setB) << endl;
                    break;
                }
                case 8: {
                    cout << "Enter element to check: ";
                    int val;
                    cin >> val;
                    cout << "In Set A: " << (setA.member(val) ? "Yes" : "No") << endl;
                    cout << "In Set B: " << (setB.member(val) ? "Yes" : "No") << endl;
                    break;
                }
                case 9:
                    break;
                default:
                    cout << "Invalid option!\n";
                }
            } while (subChoice != 9);
            break;
        }

        case 2: {
            DiscreteSet<int> universe;
            cout << "\n=== Predicate Logic Engine ===\n";
            cout << "Enter size of universe: ";
            int n;
            cin >> n;
            universe.clear_all();
            cout << "Enter " << n << " elements: ";
            for (int i = 0; i < n; i++) {
                int val;
                cin >> val;
                universe.add(val);
            }

            cout << "\nUniverse: " << universe << endl;
            cout << "∀x (x is even): " << (PredicateEngine<int>::forall(universe, isEven) ? "TRUE" : "FALSE") << endl;
            cout << "∃x (x > 5): " << (PredicateEngine<int>::exists(universe, greaterThan5) ? "TRUE" : "FALSE") << endl;
            cout << "∃!x (x is positive): " << (PredicateEngine<int>::exists_unique(universe, isPositive) ? "TRUE" : "FALSE") << endl;
            break;
        }

        case 3: {
            DiscreteSet2<int> critical, traffic, redundancy;
            cout << "\n=== Graph Constraint Builder ===\n";

            cout << "Enter critical infrastructure nodes (format: n v1 v2 ... vn): ";
            cin >> critical;

            cout << "Enter high traffic nodes (format: n v1 v2 ... vn): ";
            cin >> traffic;

            cout << "Enter redundancy nodes (format: n v1 v2 ... vn): ";
            cin >> redundancy;

            cout << "\nCritical: " << critical << endl;
            cout << "Traffic: " << traffic << endl;
            cout << "Redundancy: " << redundancy << endl;

            DiscreteSet2<int> mandatory = GraphConstraintBuilder::compute_mandatory_nodes(critical, traffic, redundancy);
            cout << "Mandatory Nodes: " << mandatory << endl;
            break;
        }

        case 4: {
            cout << "\n=== Network Graph Properties ===\n";
            cout << "Enter number of vertices: ";
            int V;
            cin >> V;
            NetworkGraph g(V);

            cout << "Enter number of edges: ";
            int E;
            cin >> E;

            cout << "Enter edges (u v weight):\n";
            for (int i = 0; i < E; i++) {
                int u, v, w;
                cin >> u >> v >> w;
                g.addEdge(u, v, w);
            }

            g.generate_property_report();
            break;
        }

        case 5: {
            cout << "\n=== Kruskal MST Algorithm ===\n";
            cout << "Enter number of vertices: ";
            int V;
            cin >> V;
            KruskalMST g(V);

            cout << "Enter number of edges: ";
            int E;
            cin >> E;

            cout << "Enter edges (u v weight):\n";
            for (int i = 0; i < E; i++) {
                int u, v, w;
                cin >> u >> v >> w;
                g.addEdge(u, v, w);
            }

            cout << "Enter number of mandatory vertices: ";
            int m;
            cin >> m;
            int* mandatory = new int[m];
            cout << "Enter mandatory vertices: ";
            for (int i = 0; i < m; i++) {
                cin >> mandatory[i];
            }

            cout << "Enter root vertex: ";
            int root;
            cin >> root;

            g.kruskal(mandatory, m, root);
            delete[] mandatory;
            break;
        }

        case 6: {
            cout << "\n=== Dijkstra Shortest Path ===\n";
            cout << "Enter number of vertices: ";
            int V;
            cin >> V;
            DijkstraGraph g(V);

            cout << "Enter number of edges: ";
            int E;
            cin >> E;

            cout << "Enter edges (u v weight):\n";
            for (int i = 0; i < E; i++) {
                int u, v, w;
                cin >> u >> v >> w;
                g.addEdge(u, v, w);
            }

            cout << "Enter source vertex: ";
            int src;
            cin >> src;

            cout << "Enter destination vertex: ";
            int dest;
            cin >> dest;

            g.dijkstra(src, dest);
            break;
        }

        case 7: {
            cout << "\n=== Binary Relation Analyzer ===\n";
            DiscreteSetSingle baseSet;

            cout << "Enter size of base set: ";
            int n;
            cin >> n;

            cout << "Enter " << n << " elements: ";
            for (int i = 0; i < n; i++) {
                int val;
                cin >> val;
                baseSet.add(val);
            }

            BinaryRelation1 rel(baseSet);

            cout << "Enter number of relation pairs: ";
            int p;
            cin >> p;

            cout << "Enter " << p << " pairs (x y):\n";
            for (int i = 0; i < p; i++) {
                int x, y;
                cin >> x >> y;
                rel.insert_pair(x, y);
            }

            rel.display();
            cout << "Reflexive: " << (rel.test_reflexive() ? "YES" : "NO") << endl;
            cout << "Symmetric: " << (rel.test_symmetric() ? "YES" : "NO") << endl;
            cout << "Antisymmetric: " << (rel.test_antisymmetric() ? "YES" : "NO") << endl;
            cout << "Transitive: " << (rel.test_transitive() ? "YES" : "NO") << endl;
            cout << "Equivalence Relation: " << (rel.is_equivalence() ? "YES" : "NO") << endl;
            cout << "Partial Order: " << (rel.is_partial_order() ? "YES" : "NO") << endl;
            cout << "Total Order: " << (rel.is_total_order() ? "YES" : "NO") << endl;
            break;
        }

        case 8: {
            cout << "\n=== Function Properties Analyzer ===\n";
            DiscreteSetSingle domain, codomain;

            cout << "Enter size of domain: ";
            int nx;
            cin >> nx;
            cout << "Enter " << nx << " domain elements: ";
            for (int i = 0; i < nx; i++) {
                int val;
                cin >> val;
                domain.add(val);
            }

            cout << "Enter size of codomain: ";
            int ny;
            cin >> ny;
            cout << "Enter " << ny << " codomain elements: ";
            for (int i = 0; i < ny; i++) {
                int val;
                cin >> val;
                codomain.add(val);
            }

            DiscreteFunction func(domain, codomain);

            cout << "Enter mappings (x y for each x in domain):\n";
            for (int i = 0; i < domain.getSize(); i++) {
                int x = domain.getElement(i);
                cout << x << " -> ";
                int y;
                cin >> y;
                func.set_mapping(x, y);
            }

            func.display();
            cout << "Total Function: " << (func.is_total_function() ? "YES" : "NO") << endl;
            cout << "Injective (One-to-One): " << (func.is_injection() ? "YES" : "NO") << endl;
            cout << "Surjective (Onto): " << (func.is_surjection() ? "YES" : "NO") << endl;
            cout << "Bijective: " << (func.is_bijection() ? "YES" : "NO") << endl;
            break;
        }

        case 9: {
            cout << "\nEnter filename to save results (e.g., results.txt): ";
            cin >> filename;
            saveToFile(filename, outputContent);
            break;
        }

        case 10: {
            cout << "\nEnter filename to load data from: ";
            cin >> filename;
            loadFromFile(filename);
            break;
        }

        case 11: {
            cout << "\nEnter configuration filename (e.g., config.txt): ";
            cin >> filename;

            // Simple approach - directly run analysis
            cout << "\nRunning full analysis from: " << filename << endl;
            runFullAnalysisFromConfig(filename);
            break;
        }

        case 12:
            cout << "\nExiting program. Goodbye!\n";
            break;

        default:
            cout << "\nInvalid choice! Please enter 1-12.\n";
        }

        if (choice != 12) {
            cout << "\nPress Enter to continue...";
            clearInputBuffer();
            cin.get();
        }

    } while (choice != 12);

    return 0;
}
