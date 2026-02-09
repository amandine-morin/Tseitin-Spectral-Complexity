import argparse
import networkx as nx
import matplotlib.pyplot as plt

def build_graph(n, d, seed, mode):
    """
    Must reproduce EXACTLY the graph used in C++.
    """
    if mode == "config_model":
        G = nx.random_regular_graph(d, n, seed=seed)
    elif mode == "circulant":
        G = nx.circulant_graph(n, list(range(1, d//2 + 1)))
    else:
        raise ValueError(f"Unknown mode: {mode}")
    return G


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--mode", required=True)
    parser.add_argument("--n", type=int, required=True)
    parser.add_argument("--d", type=int, required=True)
    parser.add_argument("--seed", type=int, required=True)
    parser.add_argument("--out", default="graph.png")
    args = parser.parse_args()

    G = build_graph(args.n, args.d, args.seed, args.mode)

    pos = nx.spring_layout(G, seed=0, k=0.22, iterations=300)

    plt.figure(figsize=(10, 10))

    nx.draw_networkx_edges(G, pos, width=0.8)

    nx.draw_networkx_nodes(
        G, pos,
        node_color="#f08080",
        node_size=450,
        edgecolors="black",
        linewidths=0.5
    )

    # Optional: comment out labels for cleaner figures
    # nx.draw_networkx_labels(G, pos, font_size=6)

    plt.title(
        f"{args.mode} graph (n={args.n}, d={args.d}, seed={args.seed})",
        fontsize=14
    )

    plt.axis("off")
    plt.tight_layout()
    plt.savefig(args.out, bbox_inches="tight")
    plt.close()



if __name__ == "__main__":
    main()
