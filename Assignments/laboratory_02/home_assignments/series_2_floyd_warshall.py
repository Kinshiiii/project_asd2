INF = float("inf")


def build_adjacency_matrix(num_vertices: int, edges_from: list[int], edges_to: list[int], edge_weights: list[int]) -> list[list[float]]:
    weight_matrix = [[INF] * num_vertices for _ in range(num_vertices)]

    for i in range(num_vertices):
        weight_matrix[i][i] = 0

    for source_vertex, target_vertex, weight in zip(edges_from, edges_to, edge_weights):
        weight_matrix[source_vertex - 1][target_vertex - 1] = weight

    return weight_matrix


def floyd_warshall(num_vertices: int, weight_matrix: list[list[float]]) -> tuple[list[list[float]], list[list[int]]]:
    distances = [row[:] for row in weight_matrix]
    next_vertex = [[-1] * num_vertices for _ in range(num_vertices)]

    for i in range(num_vertices):
        for j in range(num_vertices):
            if weight_matrix[i][j] != INF and i != j:
                next_vertex[i][j] = j

    for k in range(num_vertices):
        for i in range(num_vertices):
            if distances[i][k] != INF:
                for j in range(num_vertices):
                    if distances[k][j] != INF:
                        if distances[i][j] > distances[i][k] + distances[k][j]:
                            distances[i][j] = distances[i][k] + distances[k][j]
                            next_vertex[i][j] = next_vertex[i][k]

    for i, row in enumerate(distances):
        print(*[8 if i == j else value for j, value in enumerate(row)])

    return distances, next_vertex


def print_shortest_path(start_vertex: int, end_vertex: int, distances: list[list[float]], next_vertex: list[list[int]]) -> None:
    start_vertex -= 1
    end_vertex -= 1

    if next_vertex[start_vertex][end_vertex] == -1:
        print("No path found.")
    else:
        path: list[int] = []
        current_vertex = start_vertex
        while current_vertex != end_vertex:
            path.append(current_vertex + 1)
            current_vertex = next_vertex[current_vertex][end_vertex]

        path.append(end_vertex + 1)

        print(distances[start_vertex][end_vertex], *path)


def print_shortest_cycle(cycle_vertex: int, num_vertices: int, distances: list[list[float]], next_vertex: list[list[int]]) -> None:
    cycle_vertex -= 1
    best_cycle_length: float = INF
    best_cycle_path: list[int] | None = None

    for i in range(num_vertices):
        if i != cycle_vertex and distances[cycle_vertex][i] != INF and distances[i][cycle_vertex] != INF:
            cycle_length = distances[cycle_vertex][i] + distances[i][cycle_vertex]

            if cycle_length < best_cycle_length:
                best_cycle_length = cycle_length

                p1: list[int] = []
                current_vertex = cycle_vertex
                while current_vertex != i:
                    p1.append(current_vertex)
                    current_vertex = next_vertex[current_vertex][i]

                p1.append(i)

                p2: list[int] = []
                current_vertex = i
                while current_vertex != cycle_vertex:
                    current_vertex = next_vertex[current_vertex][cycle_vertex]
                    p2.append(current_vertex)

                best_cycle_path = p1 + p2

    if best_cycle_path is None:
        print("No cycle found.")
    else:
        print(best_cycle_length, *[x + 1 for x in best_cycle_path])
        

def read_graph() -> tuple[int, list[int], list[int], list[int]]:
    num_vertices, num_edges = map(int, input().strip().split())

    edges_from: list[int] = []
    edges_to: list[int] = []
    edge_weights: list[int] = []

    for _ in range(num_edges):
        source_vertex, target_vertex, weight = map(int, input().strip().split())

        edges_from.append(source_vertex)
        edges_to.append(target_vertex)
        edge_weights.append(weight)

    return num_vertices, edges_from, edges_to, edge_weights


def read_queries() -> tuple[int, int, int]:
    start_vertex, end_vertex = map(int, input().strip().split())
    cycle_vertex = int(input().strip())

    return start_vertex, end_vertex, cycle_vertex


def initialize_graph() -> None:
    num_vertices, edges_from, edges_to, edge_weights = read_graph()
    start_vertex, end_vertex, cycle_vertex = read_queries()

    weight_matrix = build_adjacency_matrix(num_vertices, edges_from, edges_to, edge_weights)

    distances, next_vertex = floyd_warshall(num_vertices, weight_matrix)

    print_shortest_path(start_vertex, end_vertex, distances, next_vertex)
    print_shortest_cycle(cycle_vertex, num_vertices, distances, next_vertex)


def main() -> None:
    initialize_graph()


if __name__ == "__main__":
    main()