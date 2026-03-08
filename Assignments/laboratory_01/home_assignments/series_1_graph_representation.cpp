#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

void build_adjacency_matrix(size_t num_vertices, const vector<int>& edge_from, const vector<int>& edge_to, const vector<int>& edge_weight) {
    vector<vector<int>> matrix(num_vertices, vector<int>(num_vertices, 0));

    for (size_t i = 0; i < edge_from.size(); ++i) {
        matrix[edge_from[i]][edge_to[i]] = edge_weight[i];
    }

    cout << endl << endl << "Graph using adjacency matrix:" << endl;

    cout << "  A |";
    for (size_t i = 0; i < num_vertices; ++i) {
        cout << setw(3) << i;
    }
    cout << endl;

    cout << "  --+";
    for (size_t i = 0; i < num_vertices; ++i) {
        cout << "---";
    }
    cout << endl;

    for (size_t i = 0; i < num_vertices; ++i) {
        cout << setw(3) << i << " |";
        for (const auto& value : matrix[i]) {
            cout << setw(3) << value;
        }
        cout << endl;
    }
}

void build_adjacency_list(size_t num_vertices, const vector<int>& edge_from, const vector<int>& edge_to, const vector<int>& edge_weight) {
    vector<vector<pair<int, int>>> edges(num_vertices);

    for (size_t i = 0; i < edge_from.size(); ++i) {
        edges[edge_from[i]].push_back({edge_to[i], edge_weight[i]});
    }

    cout << endl << "Graph using adjacency list:" << endl;
    cout << "  " << "Edges (to, weight):" << endl;

    for (size_t i = 0; i < num_vertices; ++i) {
        cout << "    Node " << i << " -> [";

        if (edges[i].empty()) {
            cout << "None";
        } else {
            for (size_t j = 0; j < edges[i].size(); ++j) {
                const auto& e = edges[i][j];
                cout << "(" << e.first << ", " << e.second << ")";
                if (j + 1 < edges[i].size()) cout << ", ";
            }
        }

        cout << "]" << endl;
    }
}

void build_with_two_arrays(size_t num_vertices, const vector<int>& edge_from, const vector<int>& edge_to, const vector<int>& edge_weight) {
    vector<int> offset(num_vertices + 1, 0);
    vector<pair<int, int>> edges(edge_from.size());

    for (size_t i = 0; i < edge_from.size(); ++i) {
        offset[edge_from[i] + 1]++;
    }

    for (size_t i = 1; i <= num_vertices; ++i) {
        offset[i] += offset[i - 1];
    }

    for (size_t i = 0; i < edge_from.size(); ++i) {
        int from = edge_from[i];
        int pos = offset[from]++;
        edges[pos] = {edge_to[i], edge_weight[i]};
    }

    for (size_t i = num_vertices; i > 0; --i) {
        offset[i] = offset[i - 1];
    }
    offset[0] = 0;

    cout << endl << "Graph using two arrays (variation with array length in last offset slot):" << endl;

    cout << "  Offset array (start indices per vertex): [";
    for (size_t i = 0; i < offset.size(); ++i) {
        cout << offset[i];
        if (i + 1 < offset.size()) cout << ", ";
    }
    cout << "]" << endl;

    cout << "  Edges array (to, weight): [";
    for (size_t i = 0; i < edges.size(); ++i) {
        const auto& e = edges[i];
        cout << "(" << e.first << ", " << e.second << ")";
        if (i + 1 < edges.size()) cout << ", ";
    }
    cout << "]" << endl << endl;
}

void initialize_graph() {
    size_t num_vertices, num_edges;

    cout << "Enter number of vertices and edges (n m): ";
    cin >> num_vertices >> num_edges;

    vector<int> edge_from(num_edges), edge_to(num_edges), edge_weight(num_edges);

    cout << "Enter edges (from to weight):" << endl;
    for (size_t i = 0; i < num_edges; ++i) {
        cout << "  Edge " << (i + 1) << ": ";
        cin >> edge_from[i] >> edge_to[i] >> edge_weight[i];
    }

    build_adjacency_matrix(num_vertices, edge_from, edge_to, edge_weight);
    build_adjacency_list(num_vertices, edge_from, edge_to, edge_weight);
    build_with_two_arrays(num_vertices, edge_from, edge_to, edge_weight);
}

int main() {
    initialize_graph();
    return 0;
}
