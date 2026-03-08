from collections import deque

INF = float("inf")


def build_adjacency_list(num_vertices: int, edges_from: list[int], edges_to: list[int], edge_weights: list[int]) -> list[list[tuple[int, int]]]:
    adjacency_list = [[] for _ in range(num_vertices)]

    for source_vertex, target_vertex, weight in zip(edges_from, edges_to, edge_weights):
        adjacency_list[source_vertex - 1].append((target_vertex - 1, weight))

    return adjacency_list


def dijkstra(num_vertices: int, adjacency_list: list[list[tuple[int, int]]], start_vertex: int, end_vertex: int) -> tuple[list[float], list[int]]:
    distances = [INF] * num_vertices
    visited = [False] * num_vertices
    predecessors = [-1] * num_vertices

    start_index = start_vertex - 1
    end_index = end_vertex - 1

    distances[start_index] = 0
    current_vertex = start_index

    while not visited[end_index]:
        visited[current_vertex] = True

        for neighbor, weight in adjacency_list[current_vertex]:
            if not visited[neighbor]:
                if distances[neighbor] > distances[current_vertex] + weight:
                    distances[neighbor] = distances[current_vertex] + weight
                    predecessors[neighbor] = current_vertex

        best_distance = INF
        next_vertex = -1

        for i in range(num_vertices):
            if not visited[i] and distances[i] < best_distance:
                best_distance = distances[i]
                next_vertex = i

        if next_vertex == -1:
            break

        current_vertex = next_vertex

    return distances, predecessors


def moore_bellman(num_vertices: int, adjacency_list: list[list[tuple[int, int]]], start_vertex: int) -> tuple[list[float], list[int]] | tuple[None, None]:
    distances = [INF] * num_vertices
    predecessors = [-1] * num_vertices
    in_queue = [False] * num_vertices
    processed = [False] * num_vertices

    start_index = start_vertex - 1
    distances[start_index] = 0

    dequeue: deque[int] = deque([start_index])
    in_queue[start_index] = True

    relax_count = [0] * num_vertices

    while dequeue:
        current_vertex = dequeue.popleft()
        in_queue[current_vertex] = False
        processed[current_vertex] = True

        for neighbor, weight in adjacency_list[current_vertex]:
            if distances[neighbor] > distances[current_vertex] + weight:
                
                distances[neighbor] = distances[current_vertex] + weight
                predecessors[neighbor] = current_vertex

                relax_count[neighbor] += 1
                if relax_count[neighbor] > num_vertices:
                    print("  Negative cycle detected!")
                    return None, None

                if not in_queue[neighbor]:
                    if processed[neighbor]:
                        dequeue.appendleft(neighbor)
                    else:
                        dequeue.append(neighbor)

                    in_queue[neighbor] = True

    return distances, predecessors


def print_path_result(distances: list[float] | None, predecessors: list[int] | None, start_vertex: int, end_vertex: int) -> None:
    end_index = end_vertex - 1

    if distances is None or predecessors is None:
        return

    if distances[end_index] == INF:
        print(f"  No path exists from {start_vertex} to {end_vertex}.")
        return

    path: list[int] = []
    current_vertex = end_index

    while current_vertex != -1:
        path.append(current_vertex + 1)
        current_vertex = predecessors[current_vertex]

    path.reverse()

    print(f"  Distance from {start_vertex} to {end_vertex} = {distances[end_index]}")
    print("  Path:", " -> ".join(map(str, path)))


def dijkstra_results(num_vertices: int, adjacency_list: list[list[tuple[int, int]]], start_vertex: int, end_vertex: int) -> None:
    print("\n\nDijkstra's algorithm:")
    distances, predecessors = dijkstra(num_vertices, adjacency_list, start_vertex, end_vertex)
    print_path_result(distances, predecessors, start_vertex, end_vertex)


def moore_bellman_results(num_vertices: int, adjacency_list: list[list[tuple[int, int]]], start_vertex: int, end_vertex: int) -> None:
    print("\nMoore-Bellman algorithm:")
    distances, predecessors = moore_bellman(num_vertices, adjacency_list, start_vertex)
    print_path_result(distances, predecessors, start_vertex, end_vertex)


def read_graph() -> tuple[int, list[int], list[int], list[int]]:
    print("Enter number of vertices and edges (n m): ", end="")
    num_vertices, num_edges = map(int, input().strip().split())

    edges_from: list[int] = []
    edges_to: list[int] = []
    edge_weights: list[int] = []

    print("Enter edges (from to weight):")
    for i in range(num_edges):
        print(f"  Edge {i + 1}: ", end="")
        source_vertex, target_vertex, weight = map(int, input().strip().split())

        edges_from.append(source_vertex)
        edges_to.append(target_vertex)
        edge_weights.append(weight)

    return num_vertices, edges_from, edges_to, edge_weights


def read_queries() -> tuple[int, int]:
    print("Enter source and target vertices (v u): ", end="")
    start_vertex, end_vertex = map(int, input().strip().split())

    return start_vertex, end_vertex


def has_negative_weight(edge_weights: list[int]) -> bool:
    return any(weight < 0 for weight in edge_weights)


def initialize_graph() -> None:
    num_vertices, edges_from, edges_to, edge_weights = read_graph()
    start_vertex, end_vertex = read_queries()

    adjacency_list = build_adjacency_list(num_vertices, edges_from, edges_to, edge_weights)

    if has_negative_weight(edge_weights):
        print("The graph contains negative weights. Dijkstra's algorithm was not applied.", end="\n\n")
    else:
        dijkstra_results(num_vertices, adjacency_list, start_vertex, end_vertex)

    moore_bellman_results(num_vertices, adjacency_list, start_vertex, end_vertex)


def main() -> None:
    initialize_graph()


if __name__ == "__main__":
    main()