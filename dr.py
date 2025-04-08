import random
import math
import numpy as np
import matplotlib.pyplot as plt
import argparse
import sys
import time 

def read_points_from_file(filename):
    """Lê pontos de um arquivo de texto.
    Cada linha deve conter as coordenadas x e y separadas por espaço ou vírgula."""
    points = []
    try:
        with open(filename, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue  # Ignora linhas vazias ou comentários
                
                # Tenta diferentes separadores (espaço ou vírgula)
                if ',' in line:
                    parts = line.split(',')
                else:
                    parts = line.split()
                
                if len(parts) >= 2:
                    try:
                        x = float(parts[0])
                        y = float(parts[1])
                        points.append((x, y))
                    except ValueError:
                        print(f"Aviso: Não foi possível converter linha: {line}", file=sys.stderr)
        
        if not points:
            raise ValueError("Nenhum ponto válido encontrado no arquivo.")
            
        return points
    except Exception as e:
        print(f"Erro ao ler o arquivo: {e}", file=sys.stderr)
        sys.exit(1)

class Circle:
    def __init__(self, center_x=0, center_y=0, radius=0):
        self.center_x = center_x
        self.center_y = center_y
        self.radius = radius
    
    def __str__(self):
        return f"Circle(center=({self.center_x}, {self.center_y}), radius={self.radius})"

def distance(p1, p2):
    return math.sqrt((p1[0] - p2[0])**2 + (p1[1] - p2[1])**2)

def is_point_inside_circle(point, circle, epsilon=1e-10):
    """Verifica se um ponto está dentro de um círculo, com tolerância epsilon."""
    dist = distance((point[0], point[1]), (circle.center_x, circle.center_y))
    return dist <= circle.radius + epsilon


def gartner_min_circle(points):
    """
    Implementação do algoritmo de Gärtner para o menor círculo envolvente.
    Referência: Gärtner, B. (1999). "Fast and Robust Smallest Enclosing Balls"
    """
    if not points:
        return Circle()
    
    if len(points) == 1:
        return Circle(points[0][0], points[0][1], 0)
    
    # Inicializa com um círculo trivial contendo os dois primeiros pontos
    current_circle = trivial_circle(points[:2])
    
    # Processa o restante dos pontos
    for i in range(2, len(points)):
        if not is_point_inside_circle(points[i], current_circle):
            # Se o ponto não estiver no círculo atual, atualize o círculo
            current_circle = update_circle(points[:i], points[i])
    
    return current_circle

def trivial_circle(points):
    """Cria um círculo mínimo a partir de 1 ou 2 pontos."""
    if len(points) == 1:
        return Circle(points[0][0], points[0][1], 0)
    else:  # len(points) == 2
        center_x = (points[0][0] + points[1][0]) / 2
        center_y = (points[0][1] + points[1][1]) / 2
        radius = distance(points[0], points[1]) / 2
        return Circle(center_x, center_y, radius)

def update_circle(points, q):
    """
    Atualiza o círculo para incluir o ponto q e todos os pontos em points.
    Esta é a parte principal do algoritmo de Gärtner.
    """
    if not points:
        return Circle(q[0], q[1], 0)
    
    # Inicializa com um círculo contendo q e o primeiro ponto
    current_circle = trivial_circle([q, points[0]])
    
    # Support set - pontos na fronteira do círculo
    support_set = [q, points[0]]
    
    # Processa o restante dos pontos
    for i in range(1, len(points)):
        p = points[i]
        
        if not is_point_inside_circle(p, current_circle):
            # Se p não estiver no círculo atual
            
            # Se temos apenas 1 ponto no support set, adicione p
            if len(support_set) == 1:
                current_circle = trivial_circle([support_set[0], p])
                support_set = [support_set[0], p]
            
            # Se temos 2 pontos no support set
            elif len(support_set) == 2:
                # Tente criar um círculo com os 3 pontos
                circle_3 = min_circle_from_three_points(support_set[0], support_set[1], p)
                
                # Verifique se os pontos anteriores estão contidos no novo círculo
                all_contained = True
                for j in range(i):
                    if not is_point_inside_circle(points[j], circle_3):
                        all_contained = False
                        break
                
                if all_contained:
                    current_circle = circle_3
                    support_set = [support_set[0], support_set[1], p]
                else:
                    # Encontre a melhor combinação de 2 pontos
                    best_circle = None
                    best_support = []
                    
                    for a in range(len(support_set)):
                        for b in range(a+1, len(support_set)):
                            circle = trivial_circle([support_set[a], p])
                            if is_point_inside_circle(support_set[b], circle):
                                if best_circle is None or circle.radius < best_circle.radius:
                                    best_circle = circle
                                    best_support = [support_set[a], p]
                    
                    # Se não encontramos um bom círculo com 2 pontos
                    if best_circle is None:
                        current_circle = min_circle_from_three_points(support_set[0], support_set[1], p)
                        support_set = [support_set[0], support_set[1], p]
                    else:
                        current_circle = best_circle
                        support_set = best_support
            
            # Se temos 3 pontos no support set
            elif len(support_set) == 3:
                # Verifique qual ponto pode ser removido do support set
                for j in range(3):
                    other_points = [support_set[k] for k in range(3) if k != j]
                    circle = trivial_circle(other_points + [p])
                    
                    if is_point_inside_circle(support_set[j], circle):
                        current_circle = circle
                        support_set = other_points + [p]
                        break
                else:
                    # Se nenhum ponto puder ser removido, use todos os 4 pontos
                    # (isso é uma simplificação, o algoritmo completo de Gärtner tem mais detalhes)
                    current_circle = min_circle_from_four_points(support_set[0], support_set[1], support_set[2], p)
                    # Mantém apenas os pontos mais distantes do centro
                    distances = [distance((current_circle.center_x, current_circle.center_y), pt) for pt in support_set + [p]]
                    indices = sorted(range(4), key=lambda i: distances[i], reverse=True)[:3]
                    support_set = [support_set[i] if i < 3 else p for i in indices]
    
    return current_circle

def min_circle_from_three_points(p1, p2, p3):
    """Cria um círculo que passa por três pontos."""
    # Usando a fórmula do circuncentro de um triângulo
    
    # Compute determinant
    A = p1[0] * (p2[1] - p3[1]) - p1[1] * (p2[0] - p3[0]) + p2[0] * p3[1] - p3[0] * p2[1]
    
    # Se A é próximo de zero, os pontos podem ser colineares
    if abs(A) < 1e-10:
        # Encontra os dois pontos mais distantes e cria um círculo com eles como diâmetro
        d12 = distance(p1, p2)
        d13 = distance(p1, p3)
        d23 = distance(p2, p3)
        
        if d12 >= d13 and d12 >= d23:
            return trivial_circle([p1, p2])
        elif d13 >= d12 and d13 >= d23:
            return trivial_circle([p1, p3])
        else:
            return trivial_circle([p2, p3])
    
    # Caso normal, pontos não colineares
    B = (p1[0]**2 + p1[1]**2) * (p2[1] - p3[1]) + (p2[0]**2 + p2[1]**2) * (p3[1] - p1[1]) + (p3[0]**2 + p3[1]**2) * (p1[1] - p2[1])
    C = (p1[0]**2 + p1[1]**2) * (p3[0] - p2[0]) + (p2[0]**2 + p2[1]**2) * (p1[0] - p3[0]) + (p3[0]**2 + p3[1]**2) * (p2[0] - p1[0])
    
    center_x = B / (2 * A)
    center_y = C / (2 * A)
    
    radius = distance((center_x, center_y), p1)
    return Circle(center_x, center_y, radius)

def min_circle_from_four_points(p1, p2, p3, p4):
    """
    Encontra o menor círculo que contém quatro pontos.
    Simplificação: testar todas as combinações de 3 pontos e escolher o menor círculo.
    """
    circles = [
        min_circle_from_three_points(p1, p2, p3),
        min_circle_from_three_points(p1, p2, p4),
        min_circle_from_three_points(p1, p3, p4),
        min_circle_from_three_points(p2, p3, p4)
    ]
    
    valid_circles = []
    for circle in circles:
        if (is_point_inside_circle(p1, circle) and 
            is_point_inside_circle(p2, circle) and 
            is_point_inside_circle(p3, circle) and 
            is_point_inside_circle(p4, circle)):
            valid_circles.append(circle)
    
    if not valid_circles:
        # Caso de emergência, cria um círculo a partir dos dois pontos mais distantes
        all_points = [p1, p2, p3, p4]
        max_dist = -1
        max_pair = None
        
        for i in range(len(all_points)):
            for j in range(i+1, len(all_points)):
                dist = distance(all_points[i], all_points[j])
                if dist > max_dist:
                    max_dist = dist
                    max_pair = (i, j)
        
        circle = trivial_circle([all_points[max_pair[0]], all_points[max_pair[1]]])
        # Ajusta o raio para conter todos os pontos
        max_radius = 0
        for point in all_points:
            dist = distance((circle.center_x, circle.center_y), point)
            max_radius = max(max_radius, dist)
        
        return Circle(circle.center_x, circle.center_y, max_radius)
    
    # Retorna o círculo válido de menor raio
    return min(valid_circles, key=lambda c: c.radius)

# Implementação do algoritmo de Welzl para encontrar o menor círculo
def welzl_min_circle(points):
    """Encontra o menor círculo que contém todos os pontos usando o algoritmo de Welzl."""
    def _welzl(P, R, n):
        if n == 0 or len(R) == 3:
            return min_circle_from_points(R)
        
        # Escolhe um ponto aleatório e o remove do conjunto
        idx = random.randint(0, n-1)
        p = P[idx]
        P[idx], P[n-1] = P[n-1], P[idx]  # Troca com o último ponto
        
        # Encontra o menor círculo sem o ponto p
        circle = _welzl(P, R, n-1)
        
        # Se p está dentro do círculo, retorna o círculo
        if is_point_inside_circle(p, circle):
            return circle
        
        # Caso contrário, p deve estar na fronteira do novo círculo
        return _welzl(P[:n-1], R + [p], n-1)
    
    points_copy = points.copy()
    random.shuffle(points_copy)  # Permutação aleatória para melhor desempenho
    return _welzl(points_copy, [], len(points_copy))

def min_circle_from_points(points):
    """Cria o menor círculo a partir de pontos específicos (até 3)."""
    if len(points) == 0:
        return Circle()
    elif len(points) == 1:
        return Circle(points[0][0], points[0][1], 0)
    elif len(points) == 2:
        return circle_from_two_points(points[0], points[1])
    else:  # len(points) == 3
        return circle_from_three_points(points[0], points[1], points[2])

def circle_from_two_points(p1, p2):
    """Cria um círculo com dois pontos como diâmetro."""
    center_x = (p1[0] + p2[0]) / 2
    center_y = (p1[1] + p2[1]) / 2
    radius = distance(p1, p2) / 2
    return Circle(center_x, center_y, radius)

def circle_from_three_points(p1, p2, p3):
    """Cria um círculo que passa por três pontos."""
    # Usando a fórmula do circuncentro de um triângulo
    
    # Compute determinant
    A = p1[0] * (p2[1] - p3[1]) - p1[1] * (p2[0] - p3[0]) + p2[0] * p3[1] - p3[0] * p2[1]
    
    # Se A é próximo de zero, os pontos podem ser colineares
    if abs(A) < 1e-10:
        # Encontra os dois pontos mais distantes e cria um círculo com eles como diâmetro
        d12 = distance(p1, p2)
        d13 = distance(p1, p3)
        d23 = distance(p2, p3)
        
        if d12 >= d13 and d12 >= d23:
            return circle_from_two_points(p1, p2)
        elif d13 >= d12 and d13 >= d23:
            return circle_from_two_points(p1, p3)
        else:
            return circle_from_two_points(p2, p3)
    
    # Caso normal, pontos não colineares
    B = (p1[0]**2 + p1[1]**2) * (p2[1] - p3[1]) + (p2[0]**2 + p2[1]**2) * (p3[1] - p1[1]) + (p3[0]**2 + p3[1]**2) * (p1[1] - p2[1])
    C = (p1[0]**2 + p1[1]**2) * (p3[0] - p2[0]) + (p2[0]**2 + p2[1]**2) * (p1[0] - p3[0]) + (p3[0]**2 + p3[1]**2) * (p2[0] - p1[0])
    
    center_x = B / (2 * A)
    center_y = C / (2 * A)
    
    radius = distance((center_x, center_y), p1)
    return Circle(center_x, center_y, radius)

def init_centers(demand_points, r):
    """Inicialização dos centros para o algoritmo DR."""
    # Escolhe um ponto de demanda aleatório como o primeiro centro
    centers = [random.choice(demand_points)]
    remaining_points = [p for p in demand_points if not (p[0] == centers[0][0] and p[1] == centers[0][1])]
    
    while remaining_points:
        # Encontra o ponto com a maior distância até o centro mais próximo
        max_dist = -1
        max_point = None
        
        for p in remaining_points:
            min_dist = min(distance(p, c) for c in centers)
            if min_dist > max_dist:
                max_dist = min_dist
                max_point = p
        
        # Se a maior distância for menor ou igual ao raio, todos os pontos estão cobertos
        if max_dist <= r:
            break
        
        # Adiciona o ponto como novo centro
        centers.append(max_point)
        remaining_points = [p for p in remaining_points if not (p[0] == max_point[0] and p[1] == max_point[1])]
    
    return centers

def assign_to_clusters(demand_points, centers):
    """Atribui cada ponto de demanda ao seu centro mais próximo."""
    clusters = [[] for _ in range(len(centers))]
    
    for p in demand_points:
        # Encontra o centro mais próximo
        min_dist = float('inf')
        min_idx = -1
        
        for i, c in enumerate(centers):
            d = distance(p, c)
            if d < min_dist:
                min_dist = d
                min_idx = i
        
        # Adiciona o ponto ao cluster correspondente
        clusters[min_idx].append(p)
    
    return clusters

def dynamic_reduction(demand_points, r, tolerance=1e-6, remove_strategy='smallest'):
    """Implementação do algoritmo Dynamic Reduction (DR)."""
    
    # Passo 1: Escolher k centros iniciais
    start_time = time.time()  # Iniciar cronômetro
    centers = init_centers(demand_points, r)
    k = len(centers)
    k0 = len(centers)
    # Melhor solução encontrada até agora
    best_k = k
    best_centers = centers.copy()
    
    # Flag para verificar se os centros pararam de mudar
    centers_changed = True
    previous_centers = []
    
    while centers_changed and k > 0:
        # Salva os centros atuais para comparação
        previous_centers = centers.copy()
        
        # Passo 2: Atribuir pontos de demanda aos centros mais próximos
        clusters = assign_to_clusters(demand_points, centers)
        
        # Passo 3: Determinar os centros dos clusters usando o algoritmo Welzl
        min_circles = []
        for cluster in clusters:
            if not cluster:  # Se o cluster estiver vazio
                min_circles.append(Circle())
            else:
                min_circle = welzl_min_circle(cluster)
                #in_circle = gartner_min_circle(cluster)
                min_circles.append(min_circle)
        
        # Atualiza os centros
        centers = [(circle.center_x, circle.center_y) for circle in min_circles]
        
        # Verifica se os centros mudaram significativamente
        centers_changed = False
        if len(centers) == len(previous_centers):
            for i in range(len(centers)):
                if distance(centers[i], previous_centers[i]) > tolerance:
                    centers_changed = True
                    break
        else:
            centers_changed = True
        
        # Passo 4: Calcular rmax(C)
        if min_circles:
            rmax = max(circle.radius for circle in min_circles)
            
            # Se rmax ≤ r, registrar a solução atual e tentar remover um centro
            if rmax <= r:
                best_k = k
                best_centers = centers.copy()
                
                # Estratégia de remoção
                if k > 1:  # Garantir que pelo menos um disco permaneça
                    if remove_strategy == 'random':
                        idx_to_remove = random.randint(0, k-1)
                    elif remove_strategy == 'largest':
                        idx_to_remove = max(range(k), key=lambda i: min_circles[i].radius)
                    else:  # 'smallest' (padrão)
                        idx_to_remove = min(range(k), key=lambda i: min_circles[i].radius)
                    
                    # Remove o centro escolhido
                    centers.pop(idx_to_remove)
                    k -= 1
                    centers_changed = True  # Força outra iteração
    execution_time = time.time() - start_time  # Calcular tempo de execução
    return k0, best_k, best_centers, execution_time

def enhanced_dynamic_reduction(demand_points, r, tolerance=1e-6, remove_strategy='smallest'):
    """Implementação do algoritmo Dynamic Reduction Aprimorado (DR+)."""
    
    # Executa o DR padrão primeiro
    best_k, best_centers = dynamic_reduction(demand_points, r, tolerance, remove_strategy)
    
    # Flag para verificar se houve melhoria
    improvement = True
    
    while improvement:
        improvement = False
        
        # Atribui pontos de demanda aos centros mais próximos
        clusters = assign_to_clusters(demand_points, best_centers)
        
        # Determina os centros dos clusters usando Welzl
        min_circles = []
        for cluster in clusters:
            if not cluster:
                min_circles.append(Circle())
            else:
                min_circle = welzl_min_circle(cluster)
                min_circles.append(min_circle)
        
        # Encontra o maior círculo
        if min_circles:
            largest_idx = max(range(len(min_circles)), key=lambda i: min_circles[i].radius)
            
            # Substitui o centro do maior círculo por um ponto aleatório naquele cluster
            if clusters[largest_idx]:
                random_point = random.choice(clusters[largest_idx])
                new_centers = best_centers.copy()
                new_centers[largest_idx] = random_point
                
                # Executa o DR com os novos centros iniciais
                new_k, new_centers = dynamic_reduction(demand_points, r, tolerance, remove_strategy)
                
                # Verifica se houve melhoria
                if new_k < best_k:
                    best_k = new_k
                    best_centers = new_centers
                    improvement = True
    
    return best_k, best_centers

def check_coverage(demand_points, centers, r):
    """Verifica se todos os pontos de demanda estão cobertos pelos discos."""
    for p in demand_points:
        covered = False
        for c in centers:
            if distance(p, c) <= r:
                covered = True
                break
        if not covered:
            return False
    return True

def visualize_solution(demand_points, centers, r, title):
    """Visualiza os pontos de demanda e os discos da solução."""
    plt.figure(figsize=(10, 10))
    
    # Plota os pontos de demanda
    x_points = [p[0] for p in demand_points]
    y_points = [p[1] for p in demand_points]
    plt.scatter(x_points, y_points, color='blue', s=10, label='Pontos de Demanda')
    
    # Plota os discos
    for c in centers:
        circle = plt.Circle(c, r, fill=False, color='red')
        plt.gca().add_patch(circle)
        plt.scatter(c[0], c[1], color='red', s=50)
    
    plt.title(title)
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.axis('equal')
    plt.grid(True)
    plt.legend()
    plt.show()

def main():
    # Configurar parser de argumentos
    parser = argparse.ArgumentParser(description='Algoritmo Dynamic Reduction para o problema de cobertura com discos.')
    parser.add_argument('--file', '-f', required=True, help='Arquivo de pontos (cada linha com x y)')
    parser.add_argument('--radius', '-r', type=float, required=True, help='Raio dos discos')
    parser.add_argument('--seed', '-s', type=int, default=42, help='Semente aleatória (padrão: 42)')
    parser.add_argument('--enhanced', '-e', action='store_true', help='Usar versão aprimorada DR+')
    
    args = parser.parse_args()
    
    # Configurar semente aleatória
    random.seed(args.seed)
    
    # Ler pontos do arquivo
    demand_points = read_points_from_file(args.file)
    print(f"Carregados {len(demand_points)} pontos do arquivo {args.file}")
    
    # Usar o raio especificado
    r = args.radius
    print(f"Usando raio: {r}")
    
    # Executar o algoritmo
    print("Executando o algoritmo Dynamic Reduction (DR)...")
    k0,k, centers, dr_time = dynamic_reduction(demand_points, r)
    print(f"K0: {k0}")
    print(f"Número mínimo de discos (DR): {k}")
    print(f"Tempo de execução (DR): {dr_time:.4f} segundos")
    
    # Verificar cobertura
    covered = check_coverage(demand_points, centers, r)
    print(f"Todos os pontos estão cobertos: {covered}")
    
    # Se solicitado, executar a versão aprimorada
    if args.enhanced:
        print("\nExecutando o algoritmo Dynamic Reduction Aprimorado (DR+)...")
        k_enhanced, centers_enhanced = enhanced_dynamic_reduction(demand_points, r)
        print(f"Número mínimo de discos (DR+): {k_enhanced}")
        
        covered_enhanced = check_coverage(demand_points, centers_enhanced, r)
        print(f"Todos os pontos estão cobertos (DR+): {covered_enhanced}")
        
        # Visualizar resultados da versão aprimorada
        visualize_solution(demand_points, centers_enhanced, r, f"Dynamic Reduction Aprimorado (DR+): {k_enhanced} discos")
    
    # Visualizar resultados
    #visualize_solution(demand_points, centers, r, f"Dynamic Reduction (DR): {k} discos")

if __name__ == "__main__":
    main()