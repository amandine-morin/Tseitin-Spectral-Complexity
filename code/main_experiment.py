import networkx as nx
import numpy as np
import time
import matplotlib.pyplot as plt
from pysat.formula import CNF
from pysat.solvers import Solver
import os 

# --- Step 1: Function to Calculate hat_deg(G) ---
def calculate_hat_deg(graph):
    """
    Calculates the predicted hardness metric (hat_deg) based on the formula: 
    hat_deg(G) = n / sqrt(1/d + 1/|lambda_2|)
    """
    n = graph.number_of_nodes()
    
    # Handle degree 'd'
    degrees = [d for node, d in graph.degree()]
    if not degrees:
        return 0, 0, 0
    
    # Assuming quasi-regularity, using the max degree as a proxy if not perfectly regular
    d = degrees[0] if all(d == degrees[0] for d in degrees) else max(degrees)
    
    if n <= 1 or d == 0:
        return 0, d, 0

    # Calculate eigenvalues of the Adjacency Matrix
    try:
        adj_matrix = nx.to_numpy_array(graph)
        eigenvalues = np.linalg.eigvalsh(adj_matrix)
        eigenvalues.sort()
        eigenvalues = eigenvalues[::-1] # Sort descending
    except Exception:
        return np.inf, d, 0

    # lambda_2 (second largest eigenvalue of the Adjacency Matrix)
    if len(eigenvalues) < 2:
        return 0, d, 0

    lambda2 = eigenvalues[1] 
    
    if d == 0 or np.abs(lambda2) < 1e-9: # Avoid division by zero
        return np.inf, d, lambda2

    term_d = 1.0 / d
    term_lambda2 = 1.0 / np.abs(lambda2) 
    
    denominator_term = term_d + term_lambda2
    
    if denominator_term <= 0:
        return np.inf, d, lambda2

    hat_deg = n / np.sqrt(denominator_term)
    
    return hat_deg, d, lambda2

# --- Step 2: Function to Build the Tseitin SAT Formula ---
def build_tseitin_formula(graph):
    """
    Constructs the Tseitin CNF formula for the parity problem (XOR = 0) on the graph.
    The Tseitin formula is UNSAT iff the sum of charges (implied here as 0) is odd. 
    Since we use charge 0 everywhere, the formula is UNSAT if the graph has no even cut (which is impossible for a cycle graph).
    """
    cnf = CNF()
    var_counter = 1
    
    # 1. Variables for edges
    edge_vars = {}
    edges = list(graph.edges())
    for u, v in edges:
        key = tuple(sorted((u, v)))
        edge_vars[key] = var_counter
        var_counter += 1

    # Auxiliary function for XOR CNF encoding (y = x1 XOR x2)
    def add_xor_clauses(x1, x2, y, cnf_obj):
        cnf_obj.append([-x1, -x2, -y])
        cnf_obj.append([x1, x2, -y])
        cnf_obj.append([-x1, x2, y])
        cnf_obj.append([x1, -x2, y])
    
    # 2. Building parity clauses (XOR = 0 at each vertex)
    for v in graph.nodes():
        incident_edges = list(graph.edges(v))
        
        # Get literals for incident edges
        literals = []
        for u1, u2 in incident_edges:
            edge_key = tuple(sorted((u1, u2)))
            literals.append(edge_vars[edge_key])
        
        k = len(literals)
        if k == 0:
            continue
        
        if k == 1:
            # XOR sum = 0 means x1 = 0 => clause (~x1)
            cnf.append([-literals[0]])
        elif k == 2:
            # x1 XOR x2 = 0 <=> x1 = x2 <=> (~x1 V x2) & (x1 V ~x2)
            cnf.append([-literals[0], literals[1]])
            cnf.append([literals[0], -literals[1]])
        else:
            # Chain of XORs: a_{k-1} = 0
            
            # Start: a_1 = l_1 XOR l_2
            aux_var = var_counter
            var_counter += 1
            add_xor_clauses(literals[0], literals[1], aux_var, cnf)
            
            # Chaining: a_i = a_{i-1} XOR l_{i+1}
            for i in range(2, k):
                prev_aux = aux_var
                aux_var = var_counter
                var_counter += 1
                add_xor_clauses(prev_aux, literals[i], aux_var, cnf)
                
            # The final auxiliary variable must be False (0)
            cnf.append([-aux_var])
            
    return cnf, var_counter

# --- Graph Generation Functions ---
def generate_cycle_graph(n_nodes):
    return nx.cycle_graph(n_nodes)

def generate_random_regular_graph(n_nodes, d_degree):
    # Ensure n*d is even for regular graph existence
    if (n_nodes * d_degree) % 2 != 0:
        d_degree += 1
    if d_degree >= n_nodes:
        return None
    try:
        # Using a fixed seed for replicability
        G = nx.random_regular_graph(d_degree, n_nodes, seed=42)
    except:
        # Fallback for small N or difficult combinations
        return nx.complete_graph(n_nodes) 
    return G
    
# --- Main Experiment ---
if __name__ == "__main__":
    
    # Experiment Parameters (N increases)
    N_NODES = 50 
    DEGREE_MID = 3 
    DEGREE_HARD = 6
    
    # Parameters for the exponential jump instance
    N_NODES_VHARD = 80 
    DEGREE_VHARD = 6
    
    results = []
    graph_data = []
    
    # --- Test Instances Definition ---
    
    # 1. Instance A (Easy): Cycle Graph (d=2, N=50)
    print(f"--- Instance A: Cycle Graph (Easy, N={N_NODES}) ---")
    G_easy = generate_cycle_graph(N_NODES)
    hat_deg, d, lambda2 = calculate_hat_deg(G_easy)
    
    cnf, num_vars = build_tseitin_formula(G_easy)
    start_time = time.time()
    with Solver(bootstrap_with=cnf.clauses) as solver: 
        is_sat = solver.solve()
    solve_time = time.time() - start_time
    
    print(f"Hat_deg={hat_deg:.2f}, d={d}, lambda_2={lambda2:.2f}")
    print(f"SAT Time={solve_time:.5f}s, Satisfiable={is_sat}")
    results.append({'label': 'Easy (Cycle)', 'hat_deg': hat_deg, 'time': solve_time, 'is_sat': is_sat})
    graph_data.append({'graph': G_easy, 'label': 'Easy (Cycle)', 'hat_deg': hat_deg})
    
    # 2. Instance B (Medium): Random Regular Graph (d=3, N=50)
    print(f"\n--- Instance B: {DEGREE_MID}-Regular Graph (Medium, N={N_NODES}) ---")
    G_mid = generate_random_regular_graph(N_NODES, DEGREE_MID)
    if G_mid:
        hat_deg, d, lambda2 = calculate_hat_deg(G_mid)
        cnf, num_vars = build_tseitin_formula(G_mid)
        start_time = time.time()
        with Solver(bootstrap_with=cnf.clauses) as solver: 
            is_sat = solver.solve()
        solve_time = time.time() - start_time
        
        print(f"Hat_deg={hat_deg:.2f}, d={d}, lambda_2={lambda2:.2f}")
        print(f"SAT Time={solve_time:.5f}s, Satisfiable={is_sat}")
        results.append({'label': f'Mid ({DEGREE_MID}-Reg)', 'hat_deg': hat_deg, 'time': solve_time, 'is_sat': is_sat})
        graph_data.append({'graph': G_mid, 'label': f'Mid ({DEGREE_MID}-Reg)', 'hat_deg': hat_deg})
    
    # 3. Instance C (Hard): Denser Regular Graph (d=6, N=50)
    print(f"\n--- Instance C: {DEGREE_HARD}-Regular Graph (Hard, N={N_NODES}) ---")
    G_hard = generate_random_regular_graph(N_NODES, DEGREE_HARD)
    if G_hard:
        hat_deg, d, lambda2 = calculate_hat_deg(G_hard)
        cnf, num_vars = build_tseitin_formula(G_hard)
        start_time = time.time()
        with Solver(bootstrap_with=cnf.clauses) as solver: 
            is_sat = solver.solve()
        solve_time = time.time() - start_time
        
        print(f"Hat_deg={hat_deg:.2f}, d={d}, lambda_2={lambda2:.2f}")
        print(f"SAT Time={solve_time:.5f}s, Satisfiable={is_sat}")
        results.append({'label': f'Hard ({DEGREE_HARD}-Reg)', 'hat_deg': hat_deg, 'time': solve_time, 'is_sat': is_sat})
        graph_data.append({'graph': G_hard, 'label': f'Hard ({DEGREE_HARD}-Reg)', 'hat_deg': hat_deg})
        
    # 4. Instance D (Very Hard): Large Size + Denser Regular Graph (d=6, N=80)
    print(f"\n--- Instance D: {DEGREE_VHARD}-Regular Graph (Very Hard, N={N_NODES_VHARD}) ---")
    G_vhard = generate_random_regular_graph(N_NODES_VHARD, DEGREE_VHARD)
    if G_vhard:
        hat_deg, d, lambda2 = calculate_hat_deg(G_vhard)
        cnf, num_vars = build_tseitin_formula(G_vhard)
        print(f"  --> Num Variables (SAT): {num_vars}, Num Clauses: {len(cnf.clauses)}")
        
        start_time = time.time()
        with Solver(bootstrap_with=cnf.clauses) as solver: 
            is_sat = solver.solve()
        solve_time = time.time() - start_time
        
        print(f"Hat_deg={hat_deg:.2f}, d={d}, lambda_2={lambda2:.2f}")
        print(f"SAT Time={solve_time:.5f}s, Satisfiable={is_sat}")
        results.append({'label': f'V. Hard (N={N_NODES_VHARD})', 'hat_deg': hat_deg, 'time': solve_time, 'is_sat': is_sat})
        graph_data.append({'graph': G_vhard, 'label': f'V. Hard (N={N_NODES_VHARD})', 'hat_deg': hat_deg})
    
    
    # --- Visualization and Saving ---
    print("\n--- Visualization and Saving (figures/tseitin_complexity_analysis.png) ---")
    if len(results) >= 2:
        
        # Ensure the 'figures' directory exists
        output_dir = "figures"
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
            print(f"Directory created: {output_dir}/")

        # Configure the plot
        fig, axs = plt.subplots(1, 2, figsize=(15, 6))

        # Plot 1: Hat_deg vs Solve Time
        hat_degs = [r['hat_deg'] for r in results]
        solve_times = [r['time'] for r in results]
        labels = [r['label'] for r in results]

        axs[0].scatter(hat_degs, solve_times, s=150)
        for i, label in enumerate(labels):
            axs[0].annotate(f"{label}\n({solve_times[i]:.4f}s)", (hat_degs[i], solve_times[i]), textcoords="offset points", xytext=(0,10), ha='center')
        
        axs[0].set_xlabel("Predicted Difficulty (hat_deg(G))")
        axs[0].set_ylabel("Resolution Time (seconds)")
        axs[0].set_title("Correlation between hat_deg and Solver Time")
        axs[0].grid(True)
        
        # Plot 2: Graph visualization
        G_display = graph_data[-1]['graph']
        hat_deg_display = graph_data[-1]['hat_deg']
        
        pos = nx.spring_layout(G_display)
        nx.draw_networkx(G_display, pos=pos, ax=axs[1], with_labels=True, node_color='lightcoral', node_size=250, font_size=7, font_weight='bold')
        axs[1].set_title(f"Graph of the Hardest Instance (N={N_NODES_VHARD})\n(hat_deg={hat_deg_display:.2f})")
        axs[1].axis('off')

        plt.tight_layout()
        
        # Save to the 'figures/' directory
        plt.savefig(os.path.join(output_dir, "tseitin_complexity_analysis.png")) 
        
        plt.show()

    else:
        print("Not enough data to generate a correlation plot.")