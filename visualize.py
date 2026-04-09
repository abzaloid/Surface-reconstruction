"""
Surface reconstruction from 3D point cloud using Delaunay triangulation.
Reproduces the original C++/OpenGL project as static rendered images.
"""
import numpy as np
from scipy.spatial import Delaunay
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import sys

def load_points(filename):
    with open(filename) as f:
        n = int(f.readline().strip())
        points = []
        for line in f:
            parts = line.strip().split()
            if len(parts) >= 3:
                points.append([float(parts[0]), float(parts[1]), float(parts[2])])
        points = np.array(points[:n])
    print(f"Loaded {len(points)} points from {filename}")
    print(f"  X range: [{points[:,0].min():.1f}, {points[:,0].max():.1f}]")
    print(f"  Y range: [{points[:,1].min():.1f}, {points[:,1].max():.1f}]")
    print(f"  Z range: [{points[:,2].min():.1f}, {points[:,2].max():.1f}]")
    return points

def max_edge_length(pts, simplex):
    """Return the maximum edge length of a triangle."""
    p0, p1, p2 = pts[simplex[0]], pts[simplex[1]], pts[simplex[2]]
    d01 = np.linalg.norm(p0 - p1)
    d02 = np.linalg.norm(p0 - p2)
    d12 = np.linalg.norm(p1 - p2)
    return max(d01, d02, d12)

def triangulate_by_layers(points):
    """
    Mimics the original C++ approach: group points by Z-layer,
    triangulate adjacent layers together to form the surface.
    Filter out overly long triangles.
    """
    z_vals = np.round(points[:, 2], 1)
    unique_z = np.sort(np.unique(z_vals))
    print(f"Found {len(unique_z)} Z-layers: {unique_z}")

    all_triangles = []

    # Estimate a good max edge threshold from the point density
    # Use median nearest-neighbor distance within a layer as reference
    sample_z = unique_z[len(unique_z)//2]
    mask = z_vals == sample_z
    sample_pts = points[mask][:, :2]
    if len(sample_pts) > 5:
        from scipy.spatial import cKDTree
        tree = cKDTree(sample_pts)
        dists, _ = tree.query(sample_pts, k=2)
        median_nn = np.median(dists[:, 1])
        max_edge = median_nn * 12  # allow edges up to 12x median neighbor distance
    else:
        max_edge = np.inf
    print(f"  Median nearest-neighbor distance: {median_nn:.1f}, max edge threshold: {max_edge:.1f}")

    # For each pair of adjacent Z-layers, do 2D Delaunay on XY
    for i in range(len(unique_z) - 1):
        mask = (z_vals == unique_z[i]) | (z_vals == unique_z[i + 1])
        layer_indices = np.where(mask)[0]
        layer_pts = points[layer_indices]

        if len(layer_pts) < 3:
            continue

        tri = Delaunay(layer_pts[:, :2])
        for simplex in tri.simplices:
            global_simplex = layer_indices[simplex]
            if max_edge_length(points, global_simplex) < max_edge:
                all_triangles.append(global_simplex)

    print(f"Generated {len(all_triangles)} triangles (after filtering)")
    return np.array(all_triangles)

def render_surface(points, triangles, output_prefix, title="Surface Reconstruction"):
    """Render the triangulated surface from multiple angles."""

    views = [
        (30, -60, f"{output_prefix}_perspective.png", "Perspective view"),
        (60, -45, f"{output_prefix}_top_angle.png", "Top-angled view"),
        (5, -80, f"{output_prefix}_side.png", "Side view"),
        (85, -90, f"{output_prefix}_topdown.png", "Top-down view"),
    ]

    z_centers = np.array([
        np.mean([points[t[0], 2], points[t[1], 2], points[t[2], 2]])
        for t in triangles
    ])
    z_norm = (z_centers - z_centers.min()) / (z_centers.max() - z_centers.min() + 1e-10)

    for elev, azim, fname, subtitle in views:
        fig = plt.figure(figsize=(14, 11))
        ax = fig.add_subplot(111, projection='3d')

        polys = [[points[t[0]], points[t[1]], points[t[2]]] for t in triangles]

        colors = plt.cm.terrain(z_norm)
        colors[:, 3] = 0.9

        mesh = Poly3DCollection(polys, facecolors=colors, edgecolors='gray',
                                linewidths=0.05)
        ax.add_collection3d(mesh)

        ax.set_xlim(points[:, 0].min(), points[:, 0].max())
        ax.set_ylim(points[:, 1].min(), points[:, 1].max())
        ax.set_zlim(points[:, 2].min(), points[:, 2].max())

        ax.set_xlabel('X', fontsize=11)
        ax.set_ylabel('Y', fontsize=11)
        ax.set_zlabel('Z (elevation)', fontsize=11)
        ax.set_title(f"{title}\n{subtitle}  |  {len(points)} points, {len(triangles)} triangles",
                      fontsize=13)
        ax.view_init(elev=elev, azim=azim)

        plt.tight_layout()
        plt.savefig(fname, dpi=150, bbox_inches='tight')
        plt.close()
        print(f"  Saved: {fname}")

    # Point cloud + wireframe view
    fig = plt.figure(figsize=(14, 11))
    ax = fig.add_subplot(111, projection='3d')
    step = max(1, len(triangles) // 3000)
    for tri in triangles[::step]:
        pts = points[tri]
        pts = np.vstack([pts, pts[0]])
        ax.plot(pts[:, 0], pts[:, 1], pts[:, 2], 'b-', alpha=0.12, linewidth=0.3)
    sc = ax.scatter(points[:, 0], points[:, 1], points[:, 2],
                    c=points[:, 2], cmap='terrain', s=2, alpha=0.9)
    plt.colorbar(sc, label='Z (elevation)', shrink=0.6)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title(f"{title}\nPoint Cloud + Wireframe  |  {len(points)} points", fontsize=13)
    ax.view_init(elev=30, azim=-60)
    fname = f"{output_prefix}_pointcloud.png"
    plt.tight_layout()
    plt.savefig(fname, dpi=150, bbox_inches='tight')
    plt.close()
    print(f"  Saved: {fname}")

if __name__ == "__main__":
    input_file = sys.argv[1] if len(sys.argv) > 1 else "large_input.txt"
    output_prefix = sys.argv[2] if len(sys.argv) > 2 else "surface"

    print(f"=== Surface Reconstruction ===")
    points = load_points(input_file)
    triangles = triangulate_by_layers(points)
    print(f"\nRendering views...")
    render_surface(points, triangles, output_prefix)
    print(f"\nDone! Generated images with prefix '{output_prefix}_'")
