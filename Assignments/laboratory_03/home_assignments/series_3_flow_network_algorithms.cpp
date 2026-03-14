#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <climits>

using namespace std;

typedef vector<vector<int>> CapacityMatrix;
typedef vector<vector<int>> AdjacencyList;

void read_edges(size_t num_edges, vector<int>& edges_from, vector<int>& edges_to, vector<int>& edges_capacities) {
    cout << "Enter edges (from to capacity):" << endl;

    for (size_t i = 0; i < num_edges; ++i) {
        cout << "  Edge " << (i + 1) << ": ";
        cin >> edges_from[i] >> edges_to[i] >> edges_capacities[i];
    }

    cout << endl;
}

void read_source_and_sink(int& source_vertex, int& sink_vertex) {
    cout << "Source vertex (s): ";
    cin >> source_vertex;
    --source_vertex;

    cout << "Sink vertex (t): ";
    cin >> sink_vertex;
    --sink_vertex;
}

CapacityMatrix build_capacity_matrix(size_t num_vertices, const vector<int>& edges_from, const vector<int>& edges_to, const vector<int>& edges_capacities) {
    CapacityMatrix capacity_matrix(num_vertices, vector<int>(num_vertices, 0));

    for (size_t i = 0; i < edges_from.size(); ++i) {
        capacity_matrix[edges_from[i] - 1][edges_to[i] - 1] += edges_capacities[i];
    }

    cout << endl << "Flow network (capacity matrix):" << endl;

    cout << "  V |";
    for (size_t i = 0; i < num_vertices; ++i) {
        cout << setw(3) << (i + 1);
    }
    cout << endl;

    cout << "  --+";
    for (size_t i = 0; i < num_vertices; ++i) {
        cout << "---";
    }
    cout << endl;

    for (size_t i = 0; i < num_vertices; ++i) {
        cout << setw(3) << (i + 1) << " |";

        for (int capacity : capacity_matrix[i]) {
            cout << setw(3) << capacity;
        }

        cout << endl;
    }

    return capacity_matrix;
}

AdjacencyList build_adjacency_list(size_t num_vertices, const vector<int>& edges_from, const vector<int>& edges_to) {
    AdjacencyList adjacency_list(num_vertices);

    for (size_t i = 0; i < edges_from.size(); ++i) {
        adjacency_list[edges_from[i] - 1].push_back(edges_to[i] - 1);
        adjacency_list[edges_to[i] - 1].push_back(edges_from[i] - 1);
    }

    cout << endl << "Flow network (adjacency list):" << endl;
    cout << "  Edges (to):" << endl;

    for (size_t i = 0; i < num_vertices; ++i) {
        cout << "    Node " << (i + 1) << " -> [";

        if (adjacency_list[i].empty()) {
            cout << "None";
        } else {
            bool is_first = true;

            for (int adjacent_vertex : adjacency_list[i]) {
                if (!is_first) {
                    cout << ", ";
                }

                cout << (adjacent_vertex + 1);
                is_first = false;
            }
        }

        cout << "]" << endl;
    }

    return adjacency_list;
}

bool has_path_dfs(const AdjacencyList& adjacency_list, const CapacityMatrix& capacity_matrix, int source_vertex, int sink_vertex, vector<int>& parent) {
    stack<int> vertex_stack;
    vertex_stack.push(source_vertex);

    while (!vertex_stack.empty()) {
        int current_vertex = vertex_stack.top();
        vertex_stack.pop();

        if (current_vertex == sink_vertex)
            return true;

        for (int adjacent_vertex : adjacency_list[current_vertex]) {
            if (parent[adjacent_vertex] == -1 && capacity_matrix[current_vertex][adjacent_vertex] > 0) {
                parent[adjacent_vertex] = current_vertex;
                vertex_stack.push(adjacent_vertex);
            }
        }
    }

    return false;
}

bool has_path_bfs(const AdjacencyList& adjacency_list, const CapacityMatrix& capacity_matrix, int source_vertex, int sink_vertex, vector<int>& parent) {
    fill(parent.begin(), parent.end(), -1);

    queue<int> vertex_queue;
    vector<bool> visited(adjacency_list.size(), false);

    vertex_queue.push(source_vertex);
    visited[source_vertex] = true;

    while (!vertex_queue.empty()) {
        int current_vertex = vertex_queue.front();
        vertex_queue.pop();

        for (int adjacent_vertex : adjacency_list[current_vertex]) {
            if (!visited[adjacent_vertex] && capacity_matrix[current_vertex][adjacent_vertex] > 0) {
                parent[adjacent_vertex] = current_vertex;
                visited[adjacent_vertex] = true;

                if (adjacent_vertex == sink_vertex) {
                    return true;
                }

                vertex_queue.push(adjacent_vertex);
            }
        }
    }

    return false;
}

int ford_fulkerson(size_t num_vertices, CapacityMatrix capacity_matrix, const AdjacencyList& adjacency_list, int source_vertex, int sink_vertex) {
    int max_flow = 0;

    while (true) {
        int path_flow = INT_MAX;
        vector<int> parent(num_vertices, -1);
        parent[source_vertex] = source_vertex;

        if (!has_path_dfs(adjacency_list, capacity_matrix, source_vertex, sink_vertex, parent)) {
            break;
        }

        for (int v = sink_vertex; v != source_vertex; v = parent[v]) {
            path_flow = min(path_flow, capacity_matrix[parent[v]][v]);
        }

        for (int v = sink_vertex; v != source_vertex; v = parent[v]) {
            int previous_vertex = parent[v];

            capacity_matrix[previous_vertex][v] -= path_flow;
            capacity_matrix[v][previous_vertex] += path_flow;
        }

        max_flow += path_flow;
    }

    return max_flow;
}

int edmonds_karp(size_t num_vertices, CapacityMatrix capacity_matrix, const AdjacencyList& adjacency_list, int source_vertex, int sink_vertex) {
    int max_flow = 0;

    vector<int> parent(num_vertices, -1);

    while (has_path_bfs(adjacency_list, capacity_matrix, source_vertex, sink_vertex, parent)) {
        int path_flow = INT_MAX;
        int current_vertex = sink_vertex;

        while (current_vertex != source_vertex) {
            int previous_vertex = parent[current_vertex];
            path_flow = min(path_flow, capacity_matrix[previous_vertex][current_vertex]);
            current_vertex = previous_vertex;
        }

        current_vertex = sink_vertex;

        while (current_vertex != source_vertex) {
            int previous_vertex = parent[current_vertex];

            capacity_matrix[previous_vertex][current_vertex] -= path_flow;
            capacity_matrix[current_vertex][previous_vertex] += path_flow;

            current_vertex = previous_vertex;
        }

        max_flow += path_flow;
    }

    return max_flow;
}

void run_max_flow_algorithms(size_t num_vertices, const CapacityMatrix& capacity_matrix, const AdjacencyList& adjacency_list, int source_vertex, int sink_vertex) {
    cout << endl << endl
         << "Maximum Flow (Ford-Fulkerson): "
         << ford_fulkerson(num_vertices, capacity_matrix, adjacency_list, source_vertex, sink_vertex)
         << endl;

    cout << "Maximum Flow (Edmonds-Karp): "
         << edmonds_karp(num_vertices, capacity_matrix, adjacency_list, source_vertex, sink_vertex)
         << endl;
}

void initialize_network() {
    size_t num_vertices, num_edges;
    int source_vertex, sink_vertex;

    cout << "Enter number of vertices and edges (n m): ";
    cin >> num_vertices >> num_edges;

    vector<int> edges_from(num_edges);
    vector<int> edges_to(num_edges);
    vector<int> edges_capacities(num_edges);

    read_edges(num_edges, edges_from, edges_to, edges_capacities);
    read_source_and_sink(source_vertex, sink_vertex);

    run_max_flow_algorithms(
        num_vertices,
        build_capacity_matrix(num_vertices, edges_from, edges_to, edges_capacities),
        build_adjacency_list(num_vertices, edges_from, edges_to),
        source_vertex,
        sink_vertex
    );
}

int main() {
    initialize_network();
    return 0;
}
