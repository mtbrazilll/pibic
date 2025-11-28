
#include "construct.hpp"

double getRandomValue(double minVal, double maxVal) {

  double randomValue = distr2(gen); // Número entre 0 e 1
  return minVal +
         randomValue *
             (maxVal - minVal); // Mapeia para o intervalo [minVal, maxVal]
}

double gerarAleatorioPonderado(double p, double min, double max,
                               double &media) {

  double randomValue = distr2(gen);
  double r_uniform = min + randomValue * (max - min);

  double resultado = (1.0 - p) * media + p * r_uniform;

  return resultado;
}

double gerarAleatorioPorSelecao(double p, double min, double max,
                                double media) {
  double prob_escolha = distr2(gen);

  if (prob_escolha < p) {

    double randomValue_01 = distr2(gen);
    double r_uniform = min + randomValue_01 * (max - min);

    return r_uniform;
  } else {
    // NÃO: Manter o valor antigo
    return media;
  }
}

class QuadrantGuloso {
public:
  std::vector<int> point_indices;
  double x_min, x_max;
  double y_min, y_max;
  double x_media, y_media;

  QuadrantGuloso(const std::vector<int> &pts_idxs, double Xmax, double Ymax,
                 double Xmin, double Ymin)
      : point_indices(pts_idxs), x_min(Xmin), x_max(Xmax), y_min(Ymin),
        y_max(Ymax) {
    // Calcula a média inicial (centroide)
    size_t N = point_indices.size();
    if (N == 0) {
      x_media = 0.0;
      y_media = 0.0;
    } else {
      double sum_x = 0.0;
      double sum_y = 0.0;

      // Soma todos os pontos
      for (int idx : point_indices) {
        sum_x += pontos[idx].point.x();
        sum_y += pontos[idx].point.y();
      }

      // Divide pela quantidade para obter a média
      x_media = sum_x / N;
      y_media = sum_y / N;
    }
  }
  // --- FIM DA ALTERAÇÃO ---

  // Construtor vazio para criar quadrantes vazios
  QuadrantGuloso()
      : x_min(std::numeric_limits<double>::max()),
        x_max(std::numeric_limits<double>::lowest()),
        y_min(std::numeric_limits<double>::max()),
        y_max(std::numeric_limits<double>::lowest()), x_media(0.0),
        y_media(0.0) {}

  // Atualiza os limites ao adicionar um ponto
  void addPoint(int idx) {
    const Ponto &p = pontos[idx];
    size_t N = point_indices.size();

    // --- INÍCIO DA MODIFICAÇÃO ---
    // Atualiza a média (cálculo online/incremental)
    if (N == 0) {
      // Se este é o primeiro ponto, a média é o próprio ponto
      x_media = p.point.x();
      y_media = p.point.y();
    } else {
      // Calcula a nova média com base na média anterior e no novo valor
      // nova_media = (media_antiga * N + novo_valor) / (N + 1)
      x_media = (x_media * N + p.point.x()) / (N + 1.0);
      y_media = (y_media * N + p.point.y()) / (N + 1.0);
    }
    // --- FIM DA MODIFICAÇÃO ---

    // Adiciona o ponto ao vetor
    point_indices.push_back(idx);

    // Atualiza os limites (bounding box)
    x_min = std::min(x_min, p.point.x());
    x_max = std::max(x_max, p.point.x());
    y_min = std::min(y_min, p.point.y());
    y_max = std::max(y_max, p.point.y());
  }

  // Calcula o centro do quadrante
  Point getCenter() const {
    double cx = (x_min + x_max) / 2.0;
    double cy = (y_min + y_max) / 2.0;
    return Point(cx, cy);
  }

  // (Opcional) Função para obter o centro de massa (a média)
  Point getCentroid() const { return Point(x_media, y_media); }
};

// Optimized Quadrant for algo=0
struct SimpleQuadrant {
  std::vector<int> point_indices;
  double x_min, x_max;
  double y_min, y_max;

  SimpleQuadrant()
      : x_min(std::numeric_limits<double>::max()),
        x_max(std::numeric_limits<double>::lowest()),
        y_min(std::numeric_limits<double>::max()),
        y_max(std::numeric_limits<double>::lowest()) {}

  // Constructor with bounds
  SimpleQuadrant(const std::vector<int> &indices, double Xmax, double Ymax,
                 double Xmin, double Ymin)
      : point_indices(indices), x_min(Xmin), x_max(Xmax), y_min(Ymin),
        y_max(Ymax) {}

  void addPoint(int idx, const Point &p) {
    point_indices.push_back(idx);
    if (p.x() < x_min)
      x_min = p.x();
    if (p.x() > x_max)
      x_max = p.x();
    if (p.y() < y_min)
      y_min = p.y();
    if (p.y() > y_max)
      y_max = p.y();
  }

  Point getCenter() const {
    double cx = (x_min + x_max) / 2.0;
    double cy = (y_min + y_max) / 2.0;
    return Point(cx, cy);
  }
};

// --- Spatial Grid Implementation ---
class SpatialGrid {
public:
  SpatialGrid(double x_min, double x_max, double y_min, double y_max,
              double cell_size)
      : x_min(x_min), y_min(y_min), cell_size(cell_size) {
    cols = std::ceil((x_max - x_min) / cell_size);
    rows = std::ceil((y_max - y_min) / cell_size);
    grid.resize(cols * rows);
  }

  void insert(int point_idx, const Point &p) {
    int c = (p.x() - x_min) / cell_size;
    int r = (p.y() - y_min) / cell_size;

    // Clamp indices to be safe
    if (c < 0)
      c = 0;
    if (c >= cols)
      c = cols - 1;
    if (r < 0)
      r = 0;
    if (r >= rows)
      r = rows - 1;

    grid[r * cols + c].push_back(point_idx);
  }

  void query(const Point &center, double radius,
             std::vector<int> &result) const {
    double cx = center.x();
    double cy = center.y();
    double r2 = radius * radius;

    // Determine range of cells to check
    int c_min = (cx - radius - x_min) / cell_size;
    int c_max = (cx + radius - x_min) / cell_size;
    int r_min = (cy - radius - y_min) / cell_size;
    int r_max = (cy + radius - y_min) / cell_size;

    // Clamp
    if (c_min < 0)
      c_min = 0;
    if (c_max >= cols)
      c_max = cols - 1;
    if (r_min < 0)
      r_min = 0;
    if (r_max >= rows)
      r_max = rows - 1;

    for (int r = r_min; r <= r_max; ++r) {
      for (int c = c_min; c <= c_max; ++c) {
        const auto &cell_points = grid[r * cols + c];
        for (int idx : cell_points) {
          if (CGAL::squared_distance(center, pontos[idx].point) <= r2) {
            result.push_back(idx);
          }
        }
      }
    }
  }

private:
  double x_min, y_min;
  double cell_size;
  int cols, rows;
  std::vector<std::vector<int>> grid;
};

// Helper function for optimized CGAL solution using Spatial Grid
// Helper function for optimized CGAL solution using Spatial Grid
int solve_cgal_grid(bool use_randomness, double x_max, double y_max,
                    double x_min, double y_min,
                    const std::vector<bool> &tabu_list) {
  // 1. Initialize Grid (ONLY ONCE)
  static SpatialGrid *grid = nullptr;
  if (grid == nullptr) {
    grid = new SpatialGrid(x_min, x_max, y_min, y_max, raio);
    for (const auto &p : pontos) {
      grid->insert(p.indice, p.point);
    }
  }

  std::vector<SimpleQuadrant> pilha;
  pilha.reserve(100); // Pre-allocate some space

  // Filter indices based on tabu_list
  std::vector<int> initial_indices;
  initial_indices.reserve(pontos.size());
  for (size_t i = 0; i < pontos.size(); ++i) {
    if (!tabu_list[i]) {
      initial_indices.push_back(pontos[i].indice);
    }
  }

  // Use filtered indices
  pilha.push_back(SimpleQuadrant(initial_indices, x_max, y_max, x_min, y_min));

  int solution = 0;

  while (!pilha.empty()) {
    SimpleQuadrant current = std::move(pilha.back());
    pilha.pop_back();

    // Escolhe limites aleatórios para subdividir
    double rand1 = getRandomValue(current.x_min, current.x_max);
    double rand2 = getRandomValue(current.y_min, current.y_max);

    // Cria quadrantes vazios
    SimpleQuadrant q1, q2, q3, q4;
    size_t reserve_size = current.point_indices.size() / 4 + 1;
    q1.point_indices.reserve(reserve_size);
    q2.point_indices.reserve(reserve_size);
    q3.point_indices.reserve(reserve_size);
    q4.point_indices.reserve(reserve_size);

    for (int idx : current.point_indices) {
      const Ponto &p = pontos[idx];
      double px = p.point.x();
      double py = p.point.y();

      if (px >= rand1 && px <= current.x_max && py >= rand2 &&
          py <= current.y_max)
        q1.addPoint(idx, p.point);
      else if (px >= current.x_min && px <= rand1 && py >= rand2 &&
               py <= current.y_max)
        q2.addPoint(idx, p.point);
      else if (px >= current.x_min && px <= rand1 && py >= current.y_min &&
               py <= rand2)
        q3.addPoint(idx, p.point);
      else
        q4.addPoint(idx, p.point);
    }

    // Lambda para processar um quadrante
    auto processQuadrant = [&](SimpleQuadrant &q) {
      if (q.point_indices.size() == 1) {
        Point centro = q.getCenter();
        std::vector<int> neighbor_indices;
        grid->query(centro, raio, neighbor_indices);

        Component aux(raio, neighbor_indices, centro);
        manager.addComponent(aux);
        solution++;
      } else {
        double d = CGAL::squared_distance(Point(q.x_max, q.y_max),
                                          Point(q.x_min, q.y_min));

        bool create_component = false;
        if (use_randomness) {
          create_component = (d <= int_distr(gen) * raio2x4);
        } else {
          create_component = (d <= raio2x4);
        }

        if (create_component) {
          Point centro = q.getCenter();
          std::vector<int> neighbor_indices;
          grid->query(centro, raio, neighbor_indices);

          Component aux(raio, neighbor_indices, centro);
          manager.addComponent(aux);
          solution++;
        } else {
          pilha.push_back(std::move(q));
        }
      }
    };

    if (!q1.point_indices.empty())
      processQuadrant(q1);
    if (!q2.point_indices.empty())
      processQuadrant(q2);
    if (!q3.point_indices.empty())
      processQuadrant(q3);
    if (!q4.point_indices.empty())
      processQuadrant(q4);
  }

  return solution;
}

// Helper function for optimized CGAL solution
int solve_cgal_impl(bool use_randomness, double x_max, double y_max,
                    double x_min, double y_min,
                    const std::vector<bool> &tabu_list) {
  std::queue<SimpleQuadrant> fila;

  std::vector<int> initial_indices;
  initial_indices.reserve(pontos.size());
  for (size_t i = 0; i < pontos.size(); ++i) {
    if (!tabu_list[i]) {
      initial_indices.push_back(pontos[i].indice);
    }
  }

  fila.push(SimpleQuadrant(initial_indices, x_max, y_max, x_min, y_min));

  int solution = 0;

  while (!fila.empty()) {
    SimpleQuadrant current = std::move(fila.front());
    fila.pop();

    // Escolhe limites aleatórios para subdividir
    double rand1 = getRandomValue(current.x_min, current.x_max);
    double rand2 = getRandomValue(current.y_min, current.y_max);

    // Definir novos limites de subquadrantes (apenas para classificação)
    // Note: The actual bounds of sub-quadrants will be tight bounds of points
    // inside them. But we use these split lines to decide where points go.

    // Cria quadrantes vazios
    SimpleQuadrant q1, q2, q3, q4;
    // Reserve memory to avoid reallocations? We don't know size, but average is
    // N/4.
    size_t reserve_size = current.point_indices.size() / 4 + 1;
    q1.point_indices.reserve(reserve_size);
    q2.point_indices.reserve(reserve_size);
    q3.point_indices.reserve(reserve_size);
    q4.point_indices.reserve(reserve_size);

    for (int idx : current.point_indices) {
      const Ponto &p = pontos[idx];
      double px = p.point.x();
      double py = p.point.y();

      if (px >= rand1 && px <= current.x_max && py >= rand2 &&
          py <= current.y_max) {
        q1.addPoint(idx, p.point);
      } else if (px >= current.x_min && px <= rand1 && py >= rand2 &&
                 py <= current.y_max) {
        q2.addPoint(idx, p.point);
      } else if (px >= current.x_min && px <= rand1 && py >= current.y_min &&
                 py <= rand2) {
        q3.addPoint(idx, p.point);
      } else // if (px >= rand1 && px <= current.x_max && py >= current.y_min &&
             // py <= rand2)
      {
        q4.addPoint(idx, p.point);
      }
    }

    // Lambda para processar um quadrante e decidir se subdivide ou gera
    // componente Lambda para processar um quadrante e decidir se subdivide ou
    // gera componente Reutiliza o vetor de vizinhos para evitar realocações
    std::vector<int> neighbor_indices;
    neighbor_indices.reserve(100); // Reserva inicial razoável

    auto processQuadrant = [&](SimpleQuadrant &q) {
      if (q.point_indices.size() == 1) {
        // Já temos um ponto único
        Point centro = q.getCenter();
        Fuzzy_sphere_Index sphere(centro, raio);

        neighbor_indices.clear(); // Limpa o vetor reutilizado
        tree.search(std::back_inserter(neighbor_indices), sphere);

        // O tree.search já retorna os índices diretamente!
        // Não precisamos mais do loop de conversão.

        Component aux(raio, neighbor_indices, centro);
        manager.addComponent(aux);
        solution++;
      } else {
        // Distância entre cantos opostos (tight bounds)
        double d = CGAL::squared_distance(Point(q.x_max, q.y_max),
                                          Point(q.x_min, q.y_min));

        bool create_component = false;
        if (use_randomness) {
          create_component = (d <= int_distr(gen) * raio2x4);
        } else {
          create_component = (d <= raio2x4);
        }

        if (create_component) {
          Point centro = q.getCenter();
          Fuzzy_sphere_Index sphere(centro, raio);

          neighbor_indices.clear(); // Limpa o vetor reutilizado
          tree.search(std::back_inserter(neighbor_indices), sphere);

          // O tree.search já retorna os índices diretamente!

          Component aux(raio, neighbor_indices, centro);
          manager.addComponent(aux);
          solution++;
        } else {
          // Se não couber, subdivide mais
          fila.push(std::move(q));
        }
      }
    };

    // Processa cada subquadrante se não estiver vazio
    if (!q1.point_indices.empty())
      processQuadrant(q1);
    if (!q2.point_indices.empty())
      processQuadrant(q2);
    if (!q3.point_indices.empty())
      processQuadrant(q3);
    if (!q4.point_indices.empty())
      processQuadrant(q4);
  }

  return solution;
}

int generate_solution_cgal(const std::vector<Ponto> &points, double x_max,
                           double y_max, double x_min, double y_min,
                           const std::vector<bool> &tabu_list) {
  return solve_cgal_impl(true, x_max, y_max, x_min, y_min, tabu_list);
}

int generate_solution_cgal_1(const std::vector<Ponto> &points, double x_max,
                             double y_max, double x_min, double y_min,
                             const std::vector<bool> &tabu_list) {
  return solve_cgal_impl(false, x_max, y_max, x_min, y_min, tabu_list);
}

int generate_solution_guloso(const std::vector<Ponto> &points, double x_max,
                             double y_max, double x_min, double y_min, double p,
                             const std::vector<bool> &tabu_list) {
  std::queue<QuadrantGuloso> fila;

  std::vector<int> initial_indices;
  initial_indices.reserve(points.size());
  for (size_t i = 0; i < points.size(); ++i) {
    if (!tabu_list[i]) {
      initial_indices.push_back(points[i].indice);
    }
  }

  fila.push(QuadrantGuloso(initial_indices, x_max, y_max, x_min, y_min));

  int solution = 0;

  while (!fila.empty()) {
    QuadrantGuloso current = std::move(fila.front());
    fila.pop();

    // Escolhe limites aleatórios para subdividir

    double rand1 = gerarAleatorioPorSelecao(p, current.x_min, current.x_max,
                                            current.x_media);
    double rand2 = gerarAleatorioPorSelecao(p, current.y_min, current.y_max,
                                            current.y_media);

    // Definir novos limites de subquadrantes
    double XminQ1 = rand1, YminQ1 = rand2, XmaxQ1 = current.x_max,
           YmaxQ1 = current.y_max;
    double XminQ2 = current.x_min, YminQ2 = rand2, XmaxQ2 = rand1,
           YmaxQ2 = current.y_max;
    double XminQ3 = current.x_min, YminQ3 = current.y_min, XmaxQ3 = rand1,
           YmaxQ3 = rand2;
    double XminQ4 = rand1, YminQ4 = current.y_min, XmaxQ4 = current.x_max,
           YmaxQ4 = rand2;

    // Cria quadrantes vazios
    QuadrantGuloso q1, q2, q3, q4;
    size_t reserve_size = current.point_indices.size() / 4 + 1;
    q1.point_indices.reserve(reserve_size);
    q2.point_indices.reserve(reserve_size);
    q3.point_indices.reserve(reserve_size);
    q4.point_indices.reserve(reserve_size);

    for (int idx : current.point_indices) {
      const Ponto &p = pontos[idx];
      double px = p.point.x();
      double py = p.point.y();

      if (px >= XminQ1 && px <= XmaxQ1 && py >= YminQ1 && py <= YmaxQ1) {
        q1.addPoint(idx);
      } else if (px >= XminQ2 && px <= XmaxQ2 && py >= YminQ2 && py <= YmaxQ2) {
        q2.addPoint(idx);
      } else if (px >= XminQ3 && px <= XmaxQ3 && py >= YminQ3 && py <= YmaxQ3) {
        q3.addPoint(idx);
      } else if (px >= XminQ4 && px <= XmaxQ4 && py >= YminQ4 && py <= YmaxQ4) {
        q4.addPoint(idx);
      }
    }

    // Lambda para processar um quadrante e decidir se subdivide ou gera
    // componente Lambda para processar um quadrante e decidir se subdivide ou
    // gera componente
    std::vector<int> neighbor_indices;
    neighbor_indices.reserve(100);

    auto processQuadrant = [&](QuadrantGuloso &q, double mx, double my,
                               double mnx, double mny) {
      if (q.point_indices.size() == 1) {
        // Já temos um ponto único
        Point centro = q.getCenter();
        Fuzzy_sphere_Index sphere(centro, raio);

        neighbor_indices.clear();
        tree.search(std::back_inserter(neighbor_indices), sphere);

        Component aux(raio, neighbor_indices, centro);
        manager.addComponent(aux);
        solution++;
      } else {
        // Distância entre cantos opostos
        double d = CGAL::squared_distance(Point(q.x_max, q.y_max),
                                          Point(q.x_min, q.y_min));
        // Usa d para decidir se gera componente

        if (d <= raio2x4) {
          Point centro = q.getCenter();
          Fuzzy_sphere_Index sphere(centro, raio);

          neighbor_indices.clear();
          tree.search(std::back_inserter(neighbor_indices), sphere);

          Component aux(raio, neighbor_indices, centro);
          manager.addComponent(aux);
          solution++;
        } else {
          // Se não couber, subdivide mais
          fila.push(QuadrantGuloso(q.point_indices, mx, my, mnx, mny));
        }
      }
    };

    // Processa cada subquadrante se não estiver vazio
    if (!q1.point_indices.empty())
      processQuadrant(q1, XmaxQ1, YmaxQ1, XminQ1, YminQ1);
    if (!q2.point_indices.empty())
      processQuadrant(q2, XmaxQ2, YmaxQ2, XminQ2, YminQ2);
    if (!q3.point_indices.empty())
      processQuadrant(q3, XmaxQ3, YmaxQ3, XminQ3, YminQ3);
    if (!q4.point_indices.empty())
      processQuadrant(q4, XmaxQ4, YmaxQ4, XminQ4, YminQ4);
  }

  return solution;
}

int generate_solution_dr(const std::vector<Ponto> &points, double x_max,
                         double y_max, double x_min, double y_min) {
  // Função interna para calcular distância entre dois pontos
  auto distance = [](const Point &p1, const Point &p2) -> double {
    return CGAL::squared_distance(p1, p2);
  };

  // Função para inicializar centros usando farthest-first traversal
  auto init_centers = [&](const std::vector<Ponto> &pts) -> std::vector<Point> {
    std::vector<Point> centers;
    if (pts.empty())
      return centers;

    // Escolhe um ponto aleatório como primeiro centro
    std::uniform_int_distribution<> distr(0, pts.size() - 1);
    centers.push_back(pts[distr(gen)].point);

    // Cria lista de pontos restantes e distâncias mínimas
    std::vector<Ponto> remaining_points = pts;
    std::vector<double> min_distances(remaining_points.size(),
                                      std::numeric_limits<double>::max());

    while (true) {
      // Atualiza as distâncias mínimas para todos os pontos restantes
      double max_dist = -1;
      int max_idx = -1;

      for (size_t i = 0; i < remaining_points.size(); ++i) {
        double dist_to_new_center =
            distance(remaining_points[i].point, centers.back());
        min_distances[i] = std::min(min_distances[i], dist_to_new_center);

        if (min_distances[i] > max_dist) {
          max_dist = min_distances[i];
          max_idx = i;
        }
      }

      // Se a maior distância for menor ou igual ao quadrado do raio, todos os
      // pontos estão cobertos
      if (max_dist <= raio * raio || max_idx == -1) {
        break;
      }

      // Adiciona o ponto mais distante como novo centro
      centers.push_back(remaining_points[max_idx].point);

      // Remove o ponto adicionado
      std::swap(remaining_points[max_idx], remaining_points.back());
      std::swap(min_distances[max_idx], min_distances.back());
      remaining_points.pop_back();
      min_distances.pop_back();

      if (remaining_points.empty())
        break;
    }

    return centers;
  };

  // Inicializa centros
  std::vector<Point> centers = init_centers(points);

  // Para cada centro, cria um componente
  int current_solution = 0;
  for (const auto &center : centers) {
    // Encontra todos os pontos dentro do raio
    // Encontra todos os pontos dentro do raio
    Fuzzy_sphere_Index sphere(center, raio);
    std::vector<int> neighbor_indices;
    tree.search(std::back_inserter(neighbor_indices), sphere);

    // Cria e adiciona o componente
    Component aux(raio, neighbor_indices, center);
    manager.addComponent(aux);
    current_solution++;
  }

  return current_solution;
}

// Inicialização dos centros para o algoritmo DR melhorado
std::vector<Point>
init_centers_enhanced(const std::vector<int> &point_indices) {
  std::vector<Point> centers;

  if (point_indices.empty())
    return centers;

  // Escolhe um ponto de demanda aleatório como o primeiro centro
  std::uniform_int_distribution<> distr(0, point_indices.size() - 1);
  centers.push_back(pontos[point_indices[distr(gen)]].point);

  // Cria lista de pontos restantes e distâncias mínimas
  std::vector<int> remaining_indices;
  remaining_indices.reserve(point_indices.size() - 1);
  std::vector<double> min_dist_sq(point_indices.size(),
                                  std::numeric_limits<double>::max());

  // Inicializa remaining_indices e min_dist_sq
  for (size_t i = 0; i < point_indices.size(); ++i) {
    if (pontos[point_indices[i]].point !=
        centers[0]) // Evita adicionar o primeiro centro novamente
    {
      remaining_indices.push_back(point_indices[i]);
      min_dist_sq[i] =
          CGAL::squared_distance(pontos[point_indices[i]].point, centers[0]);
    }
  }

  while (!remaining_indices.empty()) {
    // Encontra o ponto com a maior distância até o centro mais próximo
    double max_squared_dist = -1;
    Point max_point;
    int max_idx_in_remaining = -1; // Index within remaining_indices

    for (size_t i = 0; i < remaining_indices.size(); ++i) {
      const Point &p = pontos[remaining_indices[i]].point;
      double current_min_squared_dist = std::numeric_limits<double>::max();

      for (const Point &c : centers) {
        double squared_dist = CGAL::squared_distance(p, c);
        current_min_squared_dist =
            std::min(current_min_squared_dist, squared_dist);
      }

      if (current_min_squared_dist > max_squared_dist) {
        max_squared_dist = current_min_squared_dist;
        max_point = p;
        max_idx_in_remaining = i;
      }
    }

    // Se a maior distância for menor ou igual ao raio², todos os pontos estão
    // cobertos
    if (max_squared_dist <= raio * raio) {
      break;
    }

    // Adiciona o ponto como novo centro
    centers.push_back(max_point);

    // Remove o ponto dos pontos restantes
    if (max_idx_in_remaining != -1) {
      // Troca com o último e remove (O(1))
      std::swap(remaining_indices[max_idx_in_remaining],
                remaining_indices.back());
      remaining_indices.pop_back();
    }
  }

  return centers;
}

// Atribui cada ponto de demanda ao seu centro mais próximo
std::vector<std::vector<int>>
assign_to_clusters_enhanced(const std::vector<int> &demand_indices,
                            const std::vector<Point> &centers) {

  std::vector<std::vector<int>> clusters(centers.size());

  for (int idx : demand_indices) {
    const Ponto &p = pontos[idx];
    // Encontra o centro mais próximo
    double min_squared_dist = std::numeric_limits<double>::max();
    size_t min_idx = 0;

    for (size_t i = 0; i < centers.size(); ++i) {
      double squared_dist = CGAL::squared_distance(p.point, centers[i]);
      if (squared_dist < min_squared_dist) {
        min_squared_dist = squared_dist;
        min_idx = i;
      }
    }

    // Adiciona o ponto ao cluster correspondente
    clusters[min_idx].push_back(idx);
  }

  return clusters;
}

// int generate_solution_dr_enhanced(const std::vector<Ponto>& points)
// {
//     // Passo 1: Escolher k centros iniciais
//     auto centers = init_centers_enhanced(points);
//     int k = centers.size();

//     // Melhor solução encontrada até agora
//     int best_k = k;
//     std::vector<Point> best_centers = centers;

//     // Flag para verificar se os centros pararam de mudar
//     bool centers_changed = true;
//     std::vector<Point> previous_centers;

//     // Variáveis para o método melhorado
//     bool first_no_change = true;
//     int last_best_k = best_k;
//     std::vector<Point> last_best_centers = best_centers;

//     double tolerance = 0.001;

//     while (true) {
//         // Salva os centros atuais para comparação
//         previous_centers = centers;

//         // Passo 2: Atribuir pontos de demanda aos centros mais próximos
//         auto clusters = assign_to_clusters_enhanced(points, centers);

//         // Passo 3: Determinar os centros dos clusters usando Min_circle do
//         CGAL

//         std::vector<double> cluster_radii;
//         centers.clear();
//         for (const auto& cluster : clusters) {
//             if (cluster.empty()) {
//                 continue;
//             } else {
//                 //std::vector<Point> cluster_points =
//                 convertPontosToPoints(cluster);

//                 Min_circle mc(cluster.begin(), cluster.end(), true);

//                 centers.push_back(mc.circle().center());
//                 //std::cout << "Centro do cluster: " << mc.circle().center()
//                 << std::endl;
//                 cluster_radii.push_back(std::sqrt(mc.circle().squared_radius()));
//             }
//         }

//         // Verifica se os centros mudaram significativamente
//         centers_changed = false;

//         if (centers.size() != previous_centers.size()) {
//             centers_changed = true;
//         } else {

//             for(int j = 0; j< centers.size(); j++) {
//                 double distance_squared = CGAL::squared_distance(centers[j],
//                 previous_centers[j]); if (distance_squared > tolerance *
//                 tolerance) {
//                     centers_changed = true;
//                     break;
//                 }
//             }
//             // for (const auto& c_new : centers) {
//             //     double min_squared_dist =
//             std::numeric_limits<double>::max();
//             //     for (const auto& c_old : previous_centers) {
//             //         double squared_dist = CGAL::squared_distance(c_new,
//             c_old);
//             //         min_squared_dist = std::min(min_squared_dist,
//             squared_dist);
//             //     }
//             //     if (min_squared_dist > tolerance * tolerance) {
//             //         centers_changed = true;
//             //         break;
//             //     }
//             // }
//         }

//         // Passo 4: Calcular rmax(C)
//         if (!cluster_radii.empty()) {
//             double rmax = *std::max_element(cluster_radii.begin(),
//             cluster_radii.end());

//             // Se rmax ≤ r, registrar a solução atual e tentar remover um
//             centro if (rmax <= raio) {
//                 best_k = k;
//                 best_centers = centers;

//                 // Estratégia de remoção - remove o menor círculo
//                 if (k > 1) {
//                     auto min_it = std::min_element(cluster_radii.begin(),
//                     cluster_radii.end()); size_t idx_to_remove =
//                     std::distance(cluster_radii.begin(), min_it);

//                     centers.erase(centers.begin() + idx_to_remove);
//                     k--;
//                     centers_changed = true;
//                 }
//             }
//         }

//         // Passo 5: Verificar se os centros pararam de mudar
//         if (!centers_changed) {
//             if (first_no_change) {
//                 first_no_change = false;
//                 last_best_k = best_k;
//                 last_best_centers = best_centers;

//                 // Perturbação: substitui o centro do maior cluster por um
//                 ponto aleatório if (!cluster_radii.empty() &&
//                 !clusters.empty()) {
//                     auto max_it = std::max_element(cluster_radii.begin(),
//                     cluster_radii.end()); size_t largest_idx =
//                     std::distance(cluster_radii.begin(), max_it);

//                     if (largest_idx < clusters.size() &&
//                     !clusters[largest_idx].empty()) {
//                         std::uniform_int_distribution<> distr(0,
//                         clusters[largest_idx].size() - 1); int random_idx =
//                         distr(gen); centers[largest_idx] =
//                         clusters[largest_idx][random_idx].point;
//                         centers_changed = true;
//                     }
//                 }
//             } else {
//                 if (best_k != last_best_k || best_centers !=
//                 last_best_centers) {
//                     last_best_k = best_k;
//                     last_best_centers = best_centers;

//                     // Nova perturbação
//                     if (!cluster_radii.empty() && !clusters.empty()) {
//                         auto max_it = std::max_element(cluster_radii.begin(),
//                         cluster_radii.end()); size_t largest_idx =
//                         std::distance(cluster_radii.begin(), max_it);

//                         if (largest_idx < clusters.size() &&
//                         !clusters[largest_idx].empty()) {
//                             std::uniform_int_distribution<> distr(0,
//                             clusters[largest_idx].size() - 1); int random_idx
//                             = distr(gen); centers[largest_idx] =
//                             clusters[largest_idx][random_idx].point;
//                             centers_changed = true;
//                     }
//                 }
//             } else {
//                 break;
//             }
//         }

//         if (!centers_changed) {
//             break;
//         }
//     }

//     // Cria componentes para a melhor solução encontrada
//     auto clusters = assign_to_clusters_enhanced(points, best_centers);
//     // identify_uncovered_points(points, best_centers, raio);
//     for (int i = 0; i < best_centers.size(); i++)
//     {
//         Component aux(raio, clusters[i], best_centers[i]);
//         manager.addComponent(aux);
//     }

//     //std::cout << "manager.size()" << manager.components.size() <<
//     std::endl;

//     return best_centers.size();
// }

// Inicialização otimizada usando índices

// Versão otimizada combinando o melhor de ambas:
std::vector<Point>
init_centers_enhanced_optimized(const std::vector<Ponto> &demand_points) {
  std::vector<Point> centers;
  if (demand_points.empty())
    return centers;

  // Primeiro centro aleatório
  std::uniform_int_distribution<> distr(0, demand_points.size() - 1);
  size_t center_idx = distr(gen);

  std::vector<size_t> centers_idx = {center_idx};
  centers.push_back(demand_points[center_idx].point);

  // Trabalha com índices para evitar cópias desnecessárias
  std::vector<size_t> remaining_indices;
  remaining_indices.reserve(demand_points.size() - 1);

  for (size_t i = 0; i < demand_points.size(); ++i) {
    if (i != center_idx) {
      remaining_indices.push_back(i);
    }
  }

  while (!remaining_indices.empty()) {
    double max_squared_dist = -1;
    size_t best_idx = 0;
    int best_pos = -1;

    for (size_t pos = 0; pos < remaining_indices.size(); ++pos) {
      size_t i = remaining_indices[pos];
      double min_squared_dist = std::numeric_limits<double>::max();

      for (size_t atual_idx : centers_idx) {
        // Use cache SE for mais rápido, senão cálculo direto
        double squared_dist;
        if (0) { // Assumindo método para verificar se cache é válido
                 // squared_dist = cache.getSquaredDistance(i, atual_idx);
        } else {
          squared_dist = CGAL::squared_distance(demand_points[i].point,
                                                centers[atual_idx]);
        }
        min_squared_dist = std::min(min_squared_dist, squared_dist);
      }

      if (min_squared_dist > max_squared_dist) {
        max_squared_dist = min_squared_dist;
        best_idx = i;
        best_pos = pos;
      }
    }

    if (max_squared_dist <= raio2) {
      break;
    }

    centers.push_back(demand_points[best_idx].point);
    centers_idx.push_back(best_idx);

    // Remove usando swap para O(1)
    std::swap(remaining_indices[best_pos], remaining_indices.back());
    remaining_indices.pop_back();
  }

  return centers;
}

int generate_solution_dr_enhanced(const std::vector<int> &point_indices) {
#include <chrono>

  auto start_total = std::chrono::high_resolution_clock::now();

  // Medição do Passo 1
  auto start_init = std::chrono::high_resolution_clock::now();
  // FASTCOVER_PP fastcover(points); 200 ms
  // auto centers = fastcover.execute_dr();
  auto centers = init_centers_enhanced(point_indices);
  auto end_init = std::chrono::high_resolution_clock::now();
  auto time_init = std::chrono::duration_cast<std::chrono::microseconds>(
                       end_init - start_init)
                       .count();

  int k = centers.size();

  // Melhor solução encontrada até agora
  int best_k = k;
  std::vector<Point> best_centers = centers;

  // Flag para verificar se os centros pararam de mudar
  bool centers_changed = true;
  std::vector<Point> previous_centers;

  // Variáveis para o método melhorado
  bool first_no_change = true;
  int last_best_k = best_k;
  std::vector<Point> last_best_centers = best_centers;

  double tolerance = 0.001;
  double raio2_tolerance =
      raio * raio + tolerance; // Para verificar cobertura com tolerância

  // Variáveis para medição de tempo
  long long total_time_assignment = 0;
  long long total_time_min_circle = 0;
  long long total_time_convergence = 0;
  long long total_time_perturbation = 0;
  int iterations = 0;

  while (true) {
    iterations++;

    // Salva os centros atuais para comparação
    previous_centers = centers;

    // Passo 2: Atribuir pontos de demanda aos centros mais próximos
    auto start_assignment = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<int>> clusters =
        assign_to_clusters_enhanced(point_indices, centers);
    auto end_assignment = std::chrono::high_resolution_clock::now();
    total_time_assignment +=
        std::chrono::duration_cast<std::chrono::microseconds>(end_assignment -
                                                              start_assignment)
            .count();

    // Passo 3: Atualizar a posição dos centros (Min-Circle Problem)
    auto start_min_circle = std::chrono::high_resolution_clock::now();

    std::vector<double> cluster_radii;
    centers.clear(); // Limpa para recalcular os centros
    for (const auto &cluster_indices : clusters) {
      if (cluster_indices.empty()) {
        continue;
      } else {
        std::vector<Point> cluster_points;
        cluster_points.reserve(cluster_indices.size());
        for (int idx : cluster_indices) {
          cluster_points.push_back(pontos[idx].point);
        }

        Min_circle mc(cluster_points.begin(), cluster_points.end(), true);
        centers.push_back(mc.circle().center());
        cluster_radii.push_back(std::sqrt(mc.circle().squared_radius()));
      }
    }
    k = centers
            .size(); // Atualiza k com o número real de centros após min_circle
    auto end_min_circle = std::chrono::high_resolution_clock::now();
    total_time_min_circle +=
        std::chrono::duration_cast<std::chrono::microseconds>(end_min_circle -
                                                              start_min_circle)
            .count();

    // Verifica se os centros mudaram significativamente
    auto start_convergence = std::chrono::high_resolution_clock::now();
    centers_changed = false;

    if (centers.size() != previous_centers.size()) {
      centers_changed = true;
    } else {
      for (int j = 0; j < centers.size(); j++) {
        double distance_squared =
            CGAL::squared_distance(centers[j], previous_centers[j]);
        if (distance_squared > tolerance * tolerance) {
          centers_changed = true;
          break;
        }
      }
    }

    // Verifica se todos os pontos estão cobertos
    bool all_covered = true;
    for (int idx : point_indices) {
      bool covered_by_any_center = false;
      for (const auto &c : centers) {
        if (CGAL::squared_distance(pontos[idx].point, c) <= raio2_tolerance) {
          covered_by_any_center = true;
          break;
        }
      }
      if (!covered_by_any_center) {
        all_covered = false;
        break;
      }
    }

    // Passo 4: Perturbação (se necessário)
    auto start_perturbation = std::chrono::high_resolution_clock::now();

    if (centers_changed) {
      // Se houve mudança, continua
    } else {
      // Se convergiu (não mudou centros)
      if (all_covered) {
        // Solução viável encontrada!
        if (k < best_k) {
          best_k = k;
          best_centers = centers;
          // Tenta reduzir k
          if (k > 1) { // Only remove if there's more than one center
            centers.pop_back();
            k--;
          } else { // If only one center and it's the best, we can't reduce
                   // further
            break;
          }
        } else {
          // Já temos k, tentamos k-1
          if (k > 1) {
            centers.pop_back();
            k--;
          } else {
            break; // Cannot reduce k further
          }
        }
      } else {
        // Convergiu mas não cobriu tudo (inviável)
        // Tenta perturbar ou reiniciar

        // Vamos re-inicializar
        centers = init_centers_enhanced(point_indices);
        k = centers.size();

        // Se k inicial for muito grande, podemos tentar reduzir
        if (k >= best_k && best_k < (int)point_indices.size()) {
          // Força k menor que o melhor encontrado
          while (centers.size() >= best_k)
            centers.pop_back();
          k = centers.size();
        }
      }
    }
    auto end_perturbation = std::chrono::high_resolution_clock::now();
    total_time_perturbation +=
        std::chrono::duration_cast<std::chrono::microseconds>(
            end_perturbation - start_perturbation)
            .count();

    if (!centers_changed && !all_covered &&
        k >= best_k) { // If converged, not all covered, and k is not improving,
                       // break
      break;
    }
    if (k == 0 && !point_indices.empty()) { // Avoid infinite loop if k becomes
                                            // 0 and points are not covered
      break;
    }
  }

  // Reconstrói a solução final (Componentes)
  auto start_final = std::chrono::high_resolution_clock::now();
  for (const auto &c : best_centers) {
    // Encontra pontos cobertos por este centro
    std::vector<int> covered_indices;
    for (int idx : point_indices) {
      if (CGAL::squared_distance(pontos[idx].point, c) <= raio2_tolerance) {
        covered_indices.push_back(idx);
      }
    }

    if (!covered_indices.empty()) {
      Component comp(raio, covered_indices, c);
      manager.addComponent(comp);
    }
  }
  auto end_final = std::chrono::high_resolution_clock::now();
  auto time_final = std::chrono::duration_cast<std::chrono::microseconds>(
                        end_final - start_final)
                        .count();

  auto end_total = std::chrono::high_resolution_clock::now();
  auto time_total = std::chrono::duration_cast<std::chrono::microseconds>(
                        end_total - start_total)
                        .count();

  // Imprimir estatísticas de tempo
  if (0) {
    std::cout << "=== Estatísticas de Tempo DR Enhanced ===" << std::endl;
    std::cout << "Tempo total: " << time_total << " μs" << std::endl;
    std::cout << "Inicialização: " << time_init << " μs ("
              << (time_init * 100.0 / time_total) << "%)" << std::endl;
    std::cout << "Atribuição de clusters (total): " << total_time_assignment
              << " μs (" << (total_time_assignment * 100.0 / time_total) << "%)"
              << std::endl;
    std::cout << "Min_circle (total): " << total_time_min_circle << " μs ("
              << (total_time_min_circle * 100.0 / time_total) << "%)"
              << std::endl;
    std::cout << "Verificação convergência (total): " << total_time_convergence
              << " μs (" << (total_time_convergence * 100.0 / time_total)
              << "%)" << std::endl;
    std::cout << "Perturbações (total): " << total_time_perturbation << " μs ("
              << (total_time_perturbation * 100.0 / time_total) << "%)"
              << std::endl;
    std::cout << "Criação final: " << time_final << " μs ("
              << (time_final * 100.0 / time_total) << "%)" << std::endl;
    std::cout << "Número de iterações: " << iterations << std::endl;
    std::cout << "Tempo médio por iteração: " << (time_total / iterations)
              << " μs" << std::endl;
    std::cout << "========================================" << std::endl;
  }
  return best_centers.size();
}

int generate_solution_dr_enhanced_leve(const std::vector<Ponto> &points) {
  // Passo 1: Escolher k centros iniciais
  std::vector<int> point_indices(points.size());
  for (size_t i = 0; i < points.size(); ++i)
    point_indices[i] = points[i].indice;

  auto centers = init_centers_enhanced(point_indices);
  int k = centers.size();

  // Melhor solução encontrada até agora
  int best_k = k;
  std::vector<Point> best_centers = centers;

  // Flag para verificar se os centros pararam de mudar
  bool centers_changed = true;
  std::vector<Point> previous_centers;

  // Variáveis para o método melhorado
  bool first_no_change = true;
  int last_best_k = best_k;
  std::vector<Point> last_best_centers = best_centers;

  double tolerance = 0.001;
  double raio2_tolerance = raio * raio + tolerance;

  while (true) {
    // Salva os centros atuais para comparação
    previous_centers = centers;

    // Passo 2: Atribuir pontos de demanda aos centros mais próximos
    auto clusters = assign_to_clusters_enhanced(point_indices, centers);

    // Passo 3: Determinar os centros dos clusters usando Min_circle do CGAL

    std::vector<double> cluster_radii;
    centers.clear();
    for (const auto &cluster_indices : clusters) {
      if (cluster_indices.empty()) {
        continue;
      } else {
        std::vector<Point> cluster_points;
        cluster_points.reserve(cluster_indices.size());
        for (int idx : cluster_indices) {
          cluster_points.push_back(pontos[idx].point);
        }

        Min_circle mc(cluster_points.begin(), cluster_points.end(), true);

        centers.push_back(mc.circle().center());
        // std::cout << "Centro do cluster: " << mc.circle().center() <<
        // std::endl;
        cluster_radii.push_back(std::sqrt(mc.circle().squared_radius()));
      }
    }

    // Verifica se os centros mudaram significativamente
    centers_changed = false;

    if (centers.size() != previous_centers.size()) {
      centers_changed = true;
    } else {

      for (int j = 0; j < centers.size(); j++) {
        double distance_squared =
            CGAL::squared_distance(centers[j], previous_centers[j]);
        if (distance_squared > tolerance * tolerance) {
          centers_changed = true;
          break;
        }
      }
    }

    // Passo 4: Calcular rmax(C)
    if (!cluster_radii.empty()) {
      double rmax =
          *std::max_element(cluster_radii.begin(), cluster_radii.end());

      // Se rmax ≤ r, registrar a solução atual e tentar remover um centro
      if (rmax <= raio) {
        best_k = k;
        best_centers = centers;

        // Estratégia de remoção - remove o menor círculo
        if (k > 1) {
          auto min_it =
              std::min_element(cluster_radii.begin(), cluster_radii.end());
          size_t idx_to_remove = std::distance(cluster_radii.begin(), min_it);

          centers.erase(centers.begin() + idx_to_remove);
          k--;
          centers_changed = true;
        }
      }
    }

    // Passo 5: Verificar se os centros pararam de mudar
    if (!centers_changed) {
      if (first_no_change) {
        first_no_change = false;
        last_best_k = best_k;
        last_best_centers = best_centers;

        // Perturbação: substitui o centro do maior cluster por um ponto
        // aleatório
        if (!cluster_radii.empty() && !clusters.empty()) {
          auto max_it =
              std::max_element(cluster_radii.begin(), cluster_radii.end());
          size_t largest_idx = std::distance(cluster_radii.begin(), max_it);

          if (largest_idx < clusters.size() && !clusters[largest_idx].empty()) {
            std::uniform_int_distribution<> distr(
                0, clusters[largest_idx].size() - 1);
            int random_idx = distr(gen);
            centers[largest_idx] =
                pontos[clusters[largest_idx][random_idx]].point;
            centers_changed = true;
          }
        }
      } else {
        if (best_k != last_best_k || best_centers != last_best_centers) {
          last_best_k = best_k;
          last_best_centers = best_centers;

          // Nova perturbação
          if (!cluster_radii.empty() && !clusters.empty()) {
            auto max_it =
                std::max_element(cluster_radii.begin(), cluster_radii.end());
            size_t largest_idx = std::distance(cluster_radii.begin(), max_it);

            if (largest_idx < clusters.size() &&
                !clusters[largest_idx].empty()) {
              std::uniform_int_distribution<> distr(
                  0, clusters[largest_idx].size() - 1);
              int random_idx = distr(gen);
              centers[largest_idx] =
                  pontos[clusters[largest_idx][random_idx]].point;
              centers_changed = true;
            }
          }
        } else {
          break;
        }
      }
    }

    if (!centers_changed) {
      break;
    }
  }

  // Cria componentes para a melhor solução encontrada
  auto clusters = assign_to_clusters_enhanced(point_indices, best_centers);
  // identify_uncovered_points(points, best_centers, raio);
  for (int i = 0; i < best_centers.size(); i++) {
    Component aux(raio, clusters[i], best_centers[i]);
    manager.addComponent(aux);
  }

  // std::cout << "manager.size()" << manager.components.size() << std::endl;

  return best_centers.size();
}

void distributePointsSimple(const std::vector<int> &pts_idxs,
                            SimpleQuadrant &q1, double XminQ1, double XmaxQ1,
                            double YminQ1, double YmaxQ1, SimpleQuadrant &q2,
                            double XminQ2, double XmaxQ2, double YminQ2,
                            double YmaxQ2, SimpleQuadrant &q3, double XminQ3,
                            double XmaxQ3, double YminQ3, double YmaxQ3,
                            SimpleQuadrant &q4, double XminQ4, double XmaxQ4,
                            double YminQ4, double YmaxQ4) {
  for (int idx : pts_idxs) {
    const auto &p = pontos[idx];
    double px = p.point.x();
    double py = p.point.y();

    if (px >= XminQ1 && px <= XmaxQ1 && py >= YminQ1 && py <= YmaxQ1) {
      q1.addPoint(idx, p.point);
    } else if (px >= XminQ2 && px <= XmaxQ2 && py >= YminQ2 && py <= YmaxQ2) {
      q2.addPoint(idx, p.point);
    } else if (px >= XminQ3 && px <= XmaxQ3 && py >= YminQ3 && py <= YmaxQ3) {
      q3.addPoint(idx, p.point);
    } else // if (px >= XminQ4 && px <= XmaxQ4 && py >= YminQ4 && py <= YmaxQ4)
    {
      q4.addPoint(idx, p.point);
    }
  }
}

int generate_divise_dr(const std::vector<Ponto> &points, double x_max,
                       double y_max, double x_min, double y_min) {
  std::vector<int> initial_indices;
  initial_indices.reserve(points.size());
  for (const auto &p : points) {
    initial_indices.push_back(p.indice);
  }

  std::queue<SimpleQuadrant> fila;
  fila.push(SimpleQuadrant(initial_indices, x_max, y_max, x_min, y_min));

  int solution = 0;

  while (!fila.empty()) {
    SimpleQuadrant current = std::move(fila.front());
    fila.pop();

    // Escolhe limites aleatórios para subdividir
    double rand1 = getRandomValue(current.x_min, current.x_max);
    double rand2 = getRandomValue(current.y_min, current.y_max);

    // Definir novos limites de subquadrantes
    double XminQ1 = rand1, YminQ1 = rand2, XmaxQ1 = current.x_max,
           YmaxQ1 = current.y_max;
    double XminQ2 = current.x_min, YminQ2 = rand2, XmaxQ2 = rand1,
           YmaxQ2 = current.y_max;
    double XminQ3 = current.x_min, YminQ3 = current.y_min, XmaxQ3 = rand1,
           YmaxQ3 = rand2;
    double XminQ4 = rand1, YminQ4 = current.y_min, XmaxQ4 = current.x_max,
           YmaxQ4 = rand2;

    // Cria quadrantes vazios
    SimpleQuadrant q1, q2, q3, q4;

    // Distribuir pontos
    distributePointsSimple(current.point_indices, q1, XminQ1, XmaxQ1, YminQ1,
                           YmaxQ1, q2, XminQ2, XmaxQ2, YminQ2, YmaxQ2, q3,
                           XminQ3, XmaxQ3, YminQ3, YmaxQ3, q4, XminQ4, XmaxQ4,
                           YminQ4, YmaxQ4);

    // Lambda para processar um quadrante e decidir se subdivide ou gera
    // componente
    auto processQuadrant = [&](SimpleQuadrant &q, double mx, double my,
                               double mnx, double mny) {
      if (q.point_indices.size() == 1) {
        // Já temos um ponto único
        Point centro = q.getCenter();
        Fuzzy_sphere_Ponto sphere(centro, raio);
        std::vector<Ponto> neighbors;
        tree.search(std::back_inserter(neighbors), sphere);

        std::vector<int> neighbor_indices;
        neighbor_indices.reserve(neighbors.size());
        for (const auto &p : neighbors)
          neighbor_indices.push_back(p.indice);

        Component aux(raio, neighbor_indices, centro);
        manager.addComponent(aux);
        solution++;
      } else {
        // Distância entre cantos opostos
        double d = CGAL::squared_distance(Point(q.x_max, q.y_max),
                                          Point(q.x_min, q.y_min));
        // Usa d para decidir se gera componente

        if (d <= 4 * raio2) {

          Point centro = q.getCenter();
          Fuzzy_sphere_Ponto sphere(centro, 2 * raio);
          std::vector<Ponto> neighbors;
          tree.search(std::back_inserter(neighbors), sphere);

          std::vector<int> neighbor_indices;
          neighbor_indices.reserve(neighbors.size());
          for (const auto &p : neighbors)
            neighbor_indices.push_back(p.indice);

          generate_solution_dr_enhanced(neighbor_indices);
          solution++;
        } else {
          // Se não couber, subdivide mais
          fila.push(std::move(q));
        }
      }
    };

    // Processa cada subquadrante se não estiver vazio
    if (!q1.point_indices.empty())
      processQuadrant(q1, XmaxQ1, YmaxQ1, XminQ1, YminQ1);
    if (!q2.point_indices.empty())
      processQuadrant(q2, XmaxQ2, YmaxQ2, XminQ2, YminQ2);
    if (!q3.point_indices.empty())
      processQuadrant(q3, XmaxQ3, YmaxQ3, XminQ3, YminQ3);
    if (!q4.point_indices.empty())
      processQuadrant(q4, XmaxQ4, YmaxQ4, XminQ4, YminQ4);
  }

  return solution;
}

int generate_divise_dr_profundidade(
    const std::vector<Ponto> &points, double x_max, double y_max, double x_min,
    double y_min,
    int max_depth) // Parâmetro para profundidade máxima
{
  // Estrutura para armazenar quadrante com sua profundidade
  struct QuadrantWithDepth {
    SimpleQuadrant quad;
    int depth;

    QuadrantWithDepth(const SimpleQuadrant &q, int d) : quad(q), depth(d) {}
    QuadrantWithDepth(const std::vector<int> &pts_idxs, double x_max,
                      double y_max, double x_min, double y_min, int d)
        : quad(pts_idxs, x_max, y_max, x_min, y_min), depth(d) {}
  };

  std::queue<QuadrantWithDepth> fila;

  std::vector<int> initial_indices;
  initial_indices.reserve(points.size());
  for (const auto &p : points) {
    initial_indices.push_back(p.indice);
  }

  // Inicia com profundidade 0
  fila.push(QuadrantWithDepth(initial_indices, x_max, y_max, x_min, y_min, 0));

  int solution = 0;

  while (!fila.empty()) {
    QuadrantWithDepth current = std::move(fila.front());
    fila.pop();

    // CONDIÇÃO DE PARADA: Se atingiu a profundidade máxima
    if (current.depth >= max_depth) {
      // Processa o quadrante atual como solução final
      // Point centro = current.quad.getCenter();
      // Fuzzy_sphere_Ponto sphere(centro, 2 * raio);
      // std::vector<Ponto> neighbors;
      // tree.search(std::back_inserter(neighbors), sphere);

      // Point centro = current.quad.getCenter();
      // double aux = max(current.quad.x_max - current.quad.x_min,
      // current.quad.y_max - current.quad.y_min);
      // double raio_final = aux  * sqrt(2.0);
      // Fuzzy_sphere_Ponto sphere(centro, raio_final);
      // std::vector<Ponto> neighbors;
      // tree.search(std::back_inserter(neighbors), sphere);

      solution =
          generate_solution_dr_enhanced(current.quad.point_indices) + solution;

      continue; // Pula para o próximo item da fila
    }

    // Escolhe limites aleatórios para subdividir
    double rand1 = getRandomValue(current.quad.x_min, current.quad.x_max);
    double rand2 = getRandomValue(current.quad.y_min, current.quad.y_max);

    // Definir novos limites de subquadrantes
    double XminQ1 = rand1, YminQ1 = rand2, XmaxQ1 = current.quad.x_max,
           YmaxQ1 = current.quad.y_max;
    double XminQ2 = current.quad.x_min, YminQ2 = rand2, XmaxQ2 = rand1,
           YmaxQ2 = current.quad.y_max;
    double XminQ3 = current.quad.x_min, YminQ3 = current.quad.y_min,
           XmaxQ3 = rand1, YmaxQ3 = rand2;
    double XminQ4 = rand1, YminQ4 = current.quad.y_min,
           XmaxQ4 = current.quad.x_max, YmaxQ4 = rand2;

    // Cria quadrantes vazios
    SimpleQuadrant q1, q2, q3, q4;

    // Distribuir pontos
    distributePointsSimple(current.quad.point_indices, q1, XminQ1, XmaxQ1,
                           YminQ1, YmaxQ1, q2, XminQ2, XmaxQ2, YminQ2, YmaxQ2,
                           q3, XminQ3, XmaxQ3, YminQ3, YmaxQ3, q4, XminQ4,
                           XmaxQ4, YminQ4, YmaxQ4);

    // Lambda para processar um quadrante
    auto processQuadrant = [&](SimpleQuadrant &q, double mx, double my,
                               double mnx, double mny) {
      if (q.point_indices.size() == 1) {
        // Já temos um ponto único - gera componente
        Point centro = q.getCenter();
        Fuzzy_sphere_Ponto sphere(centro, raio);
        std::vector<Ponto> neighbors;
        tree.search(std::back_inserter(neighbors), sphere);

        std::vector<int> neighbor_indices;
        neighbor_indices.reserve(neighbors.size());
        for (const auto &p : neighbors)
          neighbor_indices.push_back(p.indice);

        Component aux(raio, neighbor_indices, centro);
        manager.addComponent(aux);
        solution++;
      } else {
        // Adiciona à fila com profundidade incrementada
        fila.push(QuadrantWithDepth(q, current.depth + 1));
      }
    };

    // Processa cada subquadrante se não estiver vazio
    if (!q1.point_indices.empty())
      processQuadrant(q1, XmaxQ1, YmaxQ1, XminQ1, YminQ1);
    if (!q2.point_indices.empty())
      processQuadrant(q2, XmaxQ2, YmaxQ2, XminQ2, YminQ2);
    if (!q3.point_indices.empty())
      processQuadrant(q3, XmaxQ3, YmaxQ3, XminQ3, YminQ3);
    if (!q4.point_indices.empty())
      processQuadrant(q4, XmaxQ4, YmaxQ4, XminQ4, YminQ4);
  }

  return solution;
}

int construtivo_brkg(const std::vector<Ponto> &points) {
  // 1. Create a vector of indices and shuffle it
  std::vector<int> ranking(points.size());
  for (size_t i = 0; i < points.size(); ++i) {
    ranking[i] = points[i].indice;
  }
  std::shuffle(ranking.begin(), ranking.end(), gen);

  // 2. Data Structures
  std::list<Bola> balls_list; // Use list to keep pointers valid
  // Grid: key -> vector of pointers to balls
  std::unordered_map<long long, std::vector<Bola *>> grid;

  double cell_size = 2.0 * raio;
  auto get_grid_idx = [cell_size](const Point &p) -> std::pair<int, int> {
    return {(int)floor(p.x() / cell_size), (int)floor(p.y() / cell_size)};
  };
  auto get_hash = [](int x, int y) -> long long {
    return ((long long)x * 1000000007LL) + y; // Simple hash
  };

  // 3. Process points
  for (int idx : ranking) {
    const Ponto &ponto_atual = pontos[idx];
    bool flag_ponto_achou_bola = false;

    // Search neighbors in grid (3x3)
    std::pair<int, int> cell = get_grid_idx(ponto_atual.point);
    std::vector<Bola *> neighbors;

    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        long long key = get_hash(cell.first + dx, cell.second + dy);
        auto it = grid.find(key);
        if (it != grid.end()) {
          neighbors.insert(neighbors.end(), it->second.begin(),
                           it->second.end());
        }
      }
    }

    // Sort neighbors by distance
    std::sort(neighbors.begin(), neighbors.end(),
              [&ponto_atual](const Bola *b1, const Bola *b2) {
                return CGAL::squared_distance(ponto_atual.point, b1->centro) <
                       CGAL::squared_distance(ponto_atual.point, b2->centro);
              });

    // Try to fit in existing balls
    for (Bola *bola_ptr : neighbors) {
      Bola &bola = *bola_ptr;

      // 1. Ensure MC exists
      if (bola.mc == nullptr) {
        std::vector<Point> pts;
        for (int p_idx : bola.pontos_indices)
          pts.push_back(pontos[p_idx].point);
        bola.mc = new Min_circle(pts.begin(), pts.end(), true);
      }

      // 2. Check if point is inside current circle
      if (bola.mc->circle().has_on_bounded_side(ponto_atual.point)) {
        bola.pontos_indices.push_back(idx);
        flag_ponto_achou_bola = true;
        // Center didn't change, no grid update needed
        break;
      }

      // 3. Try incremental update
      // Strategy: Try to add point, build new MC, verify ALL points. If fail,
      // revert.

      // Optimization: First check if incremental MC (from support points) is
      // promising
      Min_circle mc_test;
      for (auto it = bola.mc->support_points_begin();
           it != bola.mc->support_points_end(); ++it) {
        mc_test.insert(*it);
      }
      mc_test.insert(ponto_atual.point);

      double epsilon = 1e-9;
      if (mc_test.circle().squared_radius() <= raio * raio - epsilon) {

        // Tentative commit
        bola.pontos_indices.push_back(idx);

        std::vector<Point> current_pts;
        for (int p_idx : bola.pontos_indices)
          current_pts.push_back(pontos[p_idx].point);

        Min_circle *new_mc =
            new Min_circle(current_pts.begin(), current_pts.end(), false);

        // Verify new_mc
        bool valid = true;
        double max_dist = 0;
        if (new_mc->circle().squared_radius() > raio * raio + epsilon) {
          valid = false;
        } else {
          // Check coverage of ALL points
          for (const auto &p_idx : bola.pontos_indices) {
            double d = CGAL::squared_distance(new_mc->circle().center(),
                                              pontos[p_idx].point);
            if (d > max_dist)
              max_dist = d;
            if (d > raio * raio + epsilon) {
              valid = false;
              break;
            }
          }
        }

        if (valid) {
          // Commit
          std::pair<int, int> old_cell =
              get_grid_idx(bola.centro); // Capture OLD center

          delete bola.mc;
          bola.mc = new_mc;
          bola.centro = bola.mc->circle().center(); // Update to NEW center

          std::pair<int, int> new_cell = get_grid_idx(bola.centro);

          if (old_cell != new_cell) {
            // Remove from old cell
            long long old_key = get_hash(old_cell.first, old_cell.second);
            auto &vec = grid[old_key];
            vec.erase(std::remove(vec.begin(), vec.end(), bola_ptr), vec.end());

            // Add to new cell
            long long new_key = get_hash(new_cell.first, new_cell.second);
            grid[new_key].push_back(bola_ptr);
          }

          flag_ponto_achou_bola = true;
          break;
        } else {
          // Revert
          delete new_mc;
          bola.pontos_indices.pop_back();
          continue; // Try next neighbor
        }
      }
    }

    if (!flag_ponto_achou_bola) {
      // Create new ball
      std::vector<int> pontos_da_bola = {idx};
      double r = raio;
      balls_list.emplace_back(ponto_atual.point, pontos_da_bola, r);
      Bola &nova_bola = balls_list.back();

      std::vector<Point> pts = {ponto_atual.point};
      nova_bola.mc = new Min_circle(pts.begin(), pts.end(), true);

      // Add to grid
      std::pair<int, int> new_cell = get_grid_idx(nova_bola.centro);
      long long key = get_hash(new_cell.first, new_cell.second);
      grid[key].push_back(&nova_bola);
    }
  }

  // 4. Build result
  int solution_size = 0;
  for (Bola &bola : balls_list) {
    Component aux(raio, bola.pontos_indices, bola.centro);
    manager.addComponent(aux);
    solution_size++;
    if (bola.mc)
      delete bola.mc;
  }

  return solution_size;
}
