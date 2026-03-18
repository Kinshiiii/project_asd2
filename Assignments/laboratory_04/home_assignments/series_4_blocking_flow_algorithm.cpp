#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
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

bool build_level_graph(const CapacityMatrix& residual_capacity, const AdjacencyList& adjacency_list, int source_vertex, int sink_vertex, vector<int>& level_graph) {
    fill(level_graph.begin(), level_graph.end(), -1);

    queue<int> vertex_queue;
    vertex_queue.push(source_vertex);
    level_graph[source_vertex] = 0;

    while (!vertex_queue.empty()) {
        int current_vertex = vertex_queue.front();
        vertex_queue.pop();

        for (int neighbor : adjacency_list[current_vertex]) {
            if (level_graph[neighbor] < 0 && residual_capacity[current_vertex][neighbor] > 0) {
                level_graph[neighbor] = level_graph[current_vertex] + 1;
                vertex_queue.push(neighbor);
            }
        }
    }

    return level_graph[sink_vertex] >= 0;
}

int send_blocking_flow(CapacityMatrix& residual_capacity, const AdjacencyList& adjacency_list, int current_vertex, int sink_vertex, int flow_to_push, vector<int>& level_graph, vector<size_t>& edge_index) {
    if (current_vertex == sink_vertex) {
        return flow_to_push;
    }

    for (size_t& edge_ptr = edge_index[current_vertex]; edge_ptr < adjacency_list[current_vertex].size(); ++edge_ptr) {
        int neighbor = adjacency_list[current_vertex][edge_ptr];

        if (level_graph[neighbor] == level_graph[current_vertex] + 1 &&
            residual_capacity[current_vertex][neighbor] > 0) {
            int pushed_flow = send_blocking_flow(
                residual_capacity,
                adjacency_list,
                neighbor,
                sink_vertex,
                min(flow_to_push, residual_capacity[current_vertex][neighbor]),
                level_graph,
                edge_index
            );

            if (pushed_flow > 0) {
                residual_capacity[current_vertex][neighbor] -= pushed_flow;
                residual_capacity[neighbor][current_vertex] += pushed_flow;
                return pushed_flow;
            }
        }
    }

    return 0;
}

int dinic_max_flow(size_t num_vertices, CapacityMatrix residual_capacity, const AdjacencyList& adjacency_list, int source_vertex, int sink_vertex) {
    int max_flow = 0;
    vector<int> level_graph(num_vertices);

    while (build_level_graph(residual_capacity, adjacency_list, source_vertex, sink_vertex, level_graph)) {
        vector<size_t> edge_index(num_vertices, 0);

        while (true) {
            int flow_to_push = send_blocking_flow(
                residual_capacity,
                adjacency_list,
                source_vertex,
                sink_vertex,
                INT_MAX,
                level_graph,
                edge_index
            );

            if (flow_to_push == 0) {
                break;
            }

            max_flow += flow_to_push;
        }
    }

    return max_flow;
}

void compute_levels(const CapacityMatrix& residual_capacity, const AdjacencyList& adjacency_list, int source_vertex, vector<int>& level_graph) {
    fill(level_graph.begin(), level_graph.end(), -1);

    queue<int> vertex_queue;
    vertex_queue.push(source_vertex);
    level_graph[source_vertex] = 0;

    while (!vertex_queue.empty()) {
        int current_vertex = vertex_queue.front();
        vertex_queue.pop();

        for (int neighbor : adjacency_list[current_vertex]) {
            if (level_graph[neighbor] < 0 && residual_capacity[current_vertex][neighbor] > 0) {
                level_graph[neighbor] = level_graph[current_vertex] + 1;
                vertex_queue.push(neighbor);
            }
        }
    }
}

void update_potentials(size_t num_vertices, const CapacityMatrix& residual_capacity, const AdjacencyList& adjacency_list, int source_vertex, int sink_vertex, vector<int>& level_graph, vector<int>& p_in, vector<int>& p_out, vector<size_t>& edge_forward, vector<size_t>& edge_backward) {
    compute_levels(residual_capacity, adjacency_list, source_vertex, level_graph);

    fill(p_in.begin(), p_in.end(), 0);
    fill(p_out.begin(), p_out.end(), 0);

    for (size_t vertex = 0; vertex < num_vertices; ++vertex) {
        if (level_graph[vertex] < 0) {
            continue;
        }

        for (int neighbor : adjacency_list[vertex]) {
            if (level_graph[neighbor] == level_graph[vertex] + 1 &&
                residual_capacity[vertex][neighbor] > 0) {
                p_out[vertex] += residual_capacity[vertex][neighbor];
                p_in[neighbor] += residual_capacity[vertex][neighbor];
            }
        }
    }

    p_in[source_vertex] = INT_MAX;
    p_out[sink_vertex] = INT_MAX;

    fill(edge_forward.begin(), edge_forward.end(), 0);
    fill(edge_backward.begin(), edge_backward.end(), 0);
}

void push_flow(size_t num_vertices, CapacityMatrix& residual_capacity, const AdjacencyList& adjacency_list, int start, int target, int initial_flow, bool is_backward, vector<int>& level_graph, vector<int>& p_in, vector<int>& p_out, vector<size_t>& edge_forward, vector<size_t>& edge_backward) {
    vector<int> excess_flow(num_vertices, 0);
    queue<int> active_vertices;

    excess_flow[start] = initial_flow;
    active_vertices.push(start);

    while (!active_vertices.empty()) {
        int current_vertex = active_vertices.front();
        active_vertices.pop();

        if (current_vertex == target) {
            excess_flow[current_vertex] = 0;
            continue;
        }

        size_t& edge_index = is_backward
            ? edge_backward[current_vertex]
            : edge_forward[current_vertex];

        while (excess_flow[current_vertex] > 0 && edge_index < adjacency_list[current_vertex].size()) {
            int next_vertex = adjacency_list[current_vertex][edge_index];

            if (level_graph[next_vertex] != level_graph[current_vertex] + (is_backward ? -1 : 1)) {
                ++edge_index;
                continue;
            }

            int& residual_cap = is_backward
                ? residual_capacity[next_vertex][current_vertex]
                : residual_capacity[current_vertex][next_vertex];

            int pushed_flow = min(excess_flow[current_vertex], residual_cap);

            if (pushed_flow == 0) {
                ++edge_index;
                continue;
            }

            if (excess_flow[next_vertex] == 0) {
                active_vertices.push(next_vertex);
            }

            excess_flow[current_vertex] -= pushed_flow;
            excess_flow[next_vertex] += pushed_flow;

            residual_cap -= pushed_flow;

            if (is_backward) {
                residual_capacity[current_vertex][next_vertex] += pushed_flow;
            } else {
                residual_capacity[next_vertex][current_vertex] += pushed_flow;
            }

            if (is_backward) {
                p_out[next_vertex] -= pushed_flow;
                p_in[current_vertex] -= pushed_flow;
            } else {
                p_in[next_vertex] -= pushed_flow;
                p_out[current_vertex] -= pushed_flow;
            }
        }
    }
}

int mkm_max_flow(size_t num_vertices, CapacityMatrix residual_capacity, const AdjacencyList& adjacency_list, int source_vertex, int sink_vertex) {
    vector<int> level_graph(num_vertices), p_in(num_vertices), p_out(num_vertices);
    vector<size_t> edge_forward(num_vertices), edge_backward(num_vertices);

    int max_flow = 0;

    update_potentials(
        num_vertices,
        residual_capacity, adjacency_list,
        source_vertex, sink_vertex,
        level_graph, p_in, p_out,
        edge_forward, edge_backward
    );

    while (level_graph[sink_vertex] >= 0) {
        while (true) {
            int current_vertex = -1;
            int best_pot = INT_MAX;

            for (size_t i = 0; i < num_vertices; ++i) {
                if (level_graph[i] < 0) {
                    continue;
                }

                int pot_i = min(p_in[i], p_out[i]);

                if (pot_i < best_pot) {
                    best_pot = pot_i;
                    current_vertex = i;
                }
            }

            if (current_vertex == -1) {
                break;
            }

            int pot = min(p_in[current_vertex], p_out[current_vertex]);

            if (pot > 0) {
                push_flow(
                    num_vertices, residual_capacity, adjacency_list,
                    current_vertex, sink_vertex, pot, false,
                    level_graph, p_in, p_out,
                    edge_forward, edge_backward
                );

                push_flow(
                    num_vertices, residual_capacity, adjacency_list,
                    current_vertex, source_vertex, pot, true,
                    level_graph, p_in, p_out,
                    edge_forward, edge_backward
                );

                max_flow += pot;
            } else {
                for (int next_vertex : adjacency_list[current_vertex]) {
                    if (level_graph[next_vertex] == level_graph[current_vertex] + 1) {
                        p_in[next_vertex] -= residual_capacity[current_vertex][next_vertex];
                    } else if (level_graph[next_vertex] == level_graph[current_vertex] - 1) {
                        p_out[next_vertex] -= residual_capacity[next_vertex][current_vertex];
                    }
                }

                level_graph[current_vertex] = -1;
            }
        }

        update_potentials(
            num_vertices,
            residual_capacity, adjacency_list,
            source_vertex, sink_vertex,
            level_graph, p_in, p_out,
            edge_forward, edge_backward
        );
    }

    return max_flow;
}

void run_max_flow_algorithm(size_t num_vertices, const CapacityMatrix& capacity_matrix, const AdjacencyList& adjacency_list, int source_vertex, int sink_vertex) {
    cout << endl << endl
         << "Maximum Flow (Dinic): "
         << dinic_max_flow(num_vertices, capacity_matrix, adjacency_list, source_vertex, sink_vertex)
         << endl;

    cout << "Maximum Flow (MKM algorithm): "
         << mkm_max_flow(num_vertices, capacity_matrix, adjacency_list, source_vertex, sink_vertex)
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

    run_max_flow_algorithm(
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
