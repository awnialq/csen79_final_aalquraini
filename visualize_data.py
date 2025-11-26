#!/usr/bin/env python3
"""
3D Data Visualization Script for Decision Tree Analysis

This script reads data files containing 3D coordinates with class labels
and creates an interactive 3D visualization to help assess decision tree accuracy.

Data Format Expected:
    x, y, z, Label
    (e.g., "29, 81, 11, Blue")

Usage:
    python visualize_data.py <datafile.txt>           # Interactive mode
    python visualize_data.py <datafile.txt> --save    # Save images to files
    python visualize_data.py                          # defaults to data1.txt
"""

import sys
import os
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np


def parse_data_file(filename):
    """
    Parse a data file and extract coordinates and labels.
    
    Args:
        filename: Path to the data file
        
    Returns:
        Tuple of (coordinates dict by label, all unique labels)
    """
    data_by_label = {}
    
    try:
        with open(filename, 'r') as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                if not line:
                    continue
                    
                try:
                    parts = [p.strip() for p in line.split(',')]
                    if len(parts) != 4:
                        print(f"Warning: Line {line_num} has {len(parts)} fields, expected 4. Skipping.")
                        continue
                    
                    x = float(parts[0])
                    y = float(parts[1])
                    z = float(parts[2])
                    label = parts[3]
                    
                    if label not in data_by_label:
                        data_by_label[label] = {'x': [], 'y': [], 'z': []}
                    
                    data_by_label[label]['x'].append(x)
                    data_by_label[label]['y'].append(y)
                    data_by_label[label]['z'].append(z)
                    
                except ValueError as e:
                    print(f"Warning: Could not parse line {line_num}: '{line}'. Error: {e}")
                    continue
                    
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
        sys.exit(1)
    except Exception as e:
        print(f"Error reading file: {e}")
        sys.exit(1)
    
    return data_by_label


def get_color_for_label(label):
    """
    Get a color for a given label.
    
    Args:
        label: The class label string
        
    Returns:
        A color string or RGB tuple
    """
    # Define colors for common labels
    color_map = {
        'Red': 'red',
        'Blue': 'blue',
        'Green': 'green',
        'Yellow': 'yellow',
        'Purple': 'purple',
        'Orange': 'orange',
        'Cyan': 'cyan',
        'Magenta': 'magenta',
    }
    
    # Return the mapped color or a default
    return color_map.get(label, 'gray')


def visualize_3d(data_by_label, title="3D Data Visualization", save_path=None):
    """
    Create an interactive 3D scatter plot of the data.
    
    Args:
        data_by_label: Dictionary mapping labels to coordinate lists
        title: Title for the plot
        save_path: If provided, save to this path instead of showing
    """
    fig = plt.figure(figsize=(12, 9))
    ax = fig.add_subplot(111, projection='3d')
    
    # Plot each class with different colors
    for label, coords in data_by_label.items():
        x = np.array(coords['x'])
        y = np.array(coords['y'])
        z = np.array(coords['z'])
        
        color = get_color_for_label(label)
        ax.scatter(x, y, z, 
                   c=color, 
                   label=f'{label} (n={len(x)})',
                   alpha=0.7,
                   s=50,
                   edgecolors='black',
                   linewidths=0.5)
    
    # Labels and title
    ax.set_xlabel('X', fontsize=12, fontweight='bold')
    ax.set_ylabel('Y', fontsize=12, fontweight='bold')
    ax.set_zlabel('Z', fontsize=12, fontweight='bold')
    ax.set_title(title, fontsize=14, fontweight='bold')
    
    # Add legend
    ax.legend(loc='upper left', fontsize=10)
    
    # Add grid
    ax.grid(True, alpha=0.3)
    
    # Print statistics
    print("\n" + "="*50)
    print("DATA STATISTICS")
    print("="*50)
    total_points = 0
    for label, coords in data_by_label.items():
        n = len(coords['x'])
        total_points += n
        x_arr = np.array(coords['x'])
        y_arr = np.array(coords['y'])
        z_arr = np.array(coords['z'])
        print(f"\n{label}:")
        print(f"  Count: {n}")
        print(f"  X range: [{x_arr.min():.1f}, {x_arr.max():.1f}], mean: {x_arr.mean():.1f}")
        print(f"  Y range: [{y_arr.min():.1f}, {y_arr.max():.1f}], mean: {y_arr.mean():.1f}")
        print(f"  Z range: [{z_arr.min():.1f}, {z_arr.max():.1f}], mean: {z_arr.mean():.1f}")
    
    print(f"\nTotal data points: {total_points}")
    print("="*50)
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        print(f"\n3D visualization saved to: {save_path}")
    else:
        plt.show()
    
    plt.close()


def visualize_2d_projections(data_by_label, title_prefix="", save_path=None):
    """
    Create 2D projection plots (XY, XZ, YZ planes) for additional analysis.
    
    Args:
        data_by_label: Dictionary mapping labels to coordinate lists
        title_prefix: Prefix for subplot titles
        save_path: If provided, save to this path instead of showing
    """
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    
    projections = [
        ('X', 'Y', 'x', 'y', axes[0]),
        ('X', 'Z', 'x', 'z', axes[1]),
        ('Y', 'Z', 'y', 'z', axes[2])
    ]
    
    for xlabel, ylabel, xkey, ykey, ax in projections:
        for label, coords in data_by_label.items():
            color = get_color_for_label(label)
            ax.scatter(coords[xkey], coords[ykey],
                       c=color,
                       label=label,
                       alpha=0.6,
                       s=30,
                       edgecolors='black',
                       linewidths=0.3)
        
        ax.set_xlabel(xlabel, fontsize=11, fontweight='bold')
        ax.set_ylabel(ylabel, fontsize=11, fontweight='bold')
        ax.set_title(f'{title_prefix}{xlabel}-{ylabel} Projection', fontsize=12)
        ax.legend(loc='best', fontsize=9)
        ax.grid(True, alpha=0.3)
    
    plt.suptitle(f'{title_prefix}2D Projections', fontsize=14, fontweight='bold')
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        print(f"2D projections saved to: {save_path}")
    else:
        plt.show()
    
    plt.close()


def visualize_3d_multiple_angles(data_by_label, title="3D Data", save_path=None):
    """
    Create 3D scatter plot from multiple viewing angles for better analysis.
    
    Args:
        data_by_label: Dictionary mapping labels to coordinate lists
        title: Title prefix for the plots
        save_path: If provided, save to this path instead of showing
    """
    fig = plt.figure(figsize=(16, 12))
    
    # Different viewing angles: (elevation, azimuth)
    angles = [
        (30, 45, "View 1 (default)"),
        (30, 135, "View 2 (rotated 90°)"),
        (60, 45, "View 3 (top-down)"),
        (0, 0, "View 4 (front)"),
    ]
    
    for i, (elev, azim, angle_title) in enumerate(angles, 1):
        ax = fig.add_subplot(2, 2, i, projection='3d')
        
        for label, coords in data_by_label.items():
            x = np.array(coords['x'])
            y = np.array(coords['y'])
            z = np.array(coords['z'])
            
            color = get_color_for_label(label)
            ax.scatter(x, y, z, 
                       c=color, 
                       label=f'{label}',
                       alpha=0.6,
                       s=30,
                       edgecolors='black',
                       linewidths=0.3)
        
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_zlabel('Z')
        ax.set_title(f'{title} - {angle_title}', fontsize=10)
        ax.legend(loc='upper left', fontsize=8)
        ax.view_init(elev=elev, azim=azim)
    
    plt.suptitle(f'{title} - Multiple Viewing Angles', fontsize=14, fontweight='bold')
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        print(f"Multi-angle visualization saved to: {save_path}")
    else:
        plt.show()
    
    plt.close()


def print_usage():
    """Print usage information."""
    print("""
Usage: python visualize_data.py [options] <datafile.txt>

Options:
    --save          Save visualizations as PNG files instead of displaying
    --help, -h      Show this help message

Examples:
    python visualize_data.py data1.txt              # Display interactive 3D plot
    python visualize_data.py data1.txt --save       # Save plots as PNG files
    python visualize_data.py                        # Use default file (data1.txt)
    
Data Format:
    Each line should contain: x, y, z, Label
    Example: 29, 81, 11, Blue
    """)


def main():
    """Main function to run the visualization."""
    # Parse command line arguments
    args = sys.argv[1:]
    
    if '--help' in args or '-h' in args:
        print_usage()
        sys.exit(0)
    
    save_mode = '--save' in args
    if save_mode:
        args.remove('--save')
    
    # Determine which file to visualize
    if args:
        filename = args[0]
    else:
        filename = "data1.txt"
        print(f"No file specified, using default: {filename}")
    
    print(f"\nLoading data from: {filename}")
    
    # Parse the data file
    data_by_label = parse_data_file(filename)
    
    if not data_by_label:
        print("Error: No valid data found in the file.")
        sys.exit(1)
    
    print(f"Found {len(data_by_label)} classes: {', '.join(data_by_label.keys())}")
    
    # Get base name for saving files
    base_name = os.path.splitext(os.path.basename(filename))[0]
    
    if save_mode:
        # Save all visualizations
        visualize_3d(data_by_label, 
                     title=f"3D Visualization: {filename}",
                     save_path=f"{base_name}_3d.png")
        
        visualize_3d_multiple_angles(data_by_label,
                                     title=filename,
                                     save_path=f"{base_name}_3d_angles.png")
        
        visualize_2d_projections(data_by_label, 
                                 title_prefix=f"{filename} - ",
                                 save_path=f"{base_name}_2d_projections.png")
        
        print(f"\nAll visualizations saved!")
    else:
        # Interactive mode
        visualize_3d(data_by_label, title=f"3D Visualization: {filename}")
        
        # Ask user if they want more visualizations
        try:
            response = input("\nWould you like to see multiple viewing angles? (y/n): ").strip().lower()
            if response == 'y':
                visualize_3d_multiple_angles(data_by_label, title=filename)
            
            response = input("\nWould you like to see 2D projections? (y/n): ").strip().lower()
            if response == 'y':
                visualize_2d_projections(data_by_label, title_prefix=f"{filename} - ")
        except (EOFError, KeyboardInterrupt):
            print("\n")


if __name__ == "__main__":
    main()
