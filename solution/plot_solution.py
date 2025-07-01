import matplotlib.pyplot as plt
import numpy as np

def plot_solution(points_file, discs_file):
    # Ler discos
    discs_data = np.loadtxt(discs_file)
    discs_x = discs_data[:, 0]
    discs_y = discs_data[:, 1]
    discs_r = discs_data[:, 2]
    discs_id = discs_data[:, 3].astype(int)

    # Ler pontos
    points_data = np.loadtxt(points_file)
    points_x = points_data[:, 0]
    points_y = points_data[:, 1]
    points_indices = points_data[:, 2].astype(int)
    points_disc_id = points_data[:, 3].astype(int)

    fig, ax = plt.subplots()
    ax.set_aspect('equal', adjustable='box')

    # Plotar discos
    for x, y, r, disc_id in zip(discs_x, discs_y, discs_r, discs_id):
        circle = plt.Circle((x, y), r, color='green', fill=False, label=f'Disco {disc_id+1}')
        ax.add_patch(circle)

    # Plotar pontos cobertos, diferenciando por disco
    scatter = ax.scatter(points_x, points_y, c=points_disc_id, cmap='tab20', s=20, label='Pontos Cobertos')
    plt.xticks([])  # Remove números do eixo x
    plt.yticks([])  # Remove números do eixo y
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('Solução CMSA- Bairros de Maceió')
    plt.legend(handles=[circle], loc='upper right')
    plt.show()

if __name__ == "__main__":
    plot_solution('points_output.txt', 'discs_output.txt')
