
#include "construct.hpp"

double getRandomValue(double minVal, double maxVal)
{

    double randomValue = distr2(gen);                // Número entre 0 e 1
    return minVal + randomValue * (maxVal - minVal); // Mapeia para o intervalo [minVal, maxVal]
}

class Quadrant
{
public:
    std::vector<Ponto> points;
    double x_min, x_max;
    double y_min, y_max;

    Quadrant(const std::vector<Ponto> &pts, double Xmax, double Ymax, double Xmin, double Ymin)
        : points(pts), x_min(Xmin), x_max(Xmax), y_min(Ymin), y_max(Ymax) {}

    // Construtor vazio para criar quadrantes vazios
    Quadrant()
        : x_min(std::numeric_limits<double>::max()),
          x_max(std::numeric_limits<double>::lowest()),
          y_min(std::numeric_limits<double>::max()),
          y_max(std::numeric_limits<double>::lowest()) {}

    // Atualiza os limites ao adicionar um ponto
    void addPoint(const Ponto &p)
    {
        points.push_back(p);
        x_min = std::min(x_min, p.point.x());
        x_max = std::max(x_max, p.point.x());
        y_min = std::min(y_min, p.point.y());
        y_max = std::max(y_max, p.point.y());
    }

    // Calcula o centro do quadrante
    Point getCenter() const
    {
        double cx = (x_min + x_max) / 2.0;
        double cy = (y_min + y_max) / 2.0;
        return Point(cx, cy);
    }
};

// Função auxiliar para distribuir pontos em novos quadrantes
void distributePoints(const std::vector<Ponto> &pts,
                      Quadrant &q1, double XminQ1, double XmaxQ1, double YminQ1, double YmaxQ1,
                      Quadrant &q2, double XminQ2, double XmaxQ2, double YminQ2, double YmaxQ2,
                      Quadrant &q3, double XminQ3, double XmaxQ3, double YminQ3, double YmaxQ3,
                      Quadrant &q4, double XminQ4, double XmaxQ4, double YminQ4, double YmaxQ4)
{
    for (const auto &p : pts)
    {
        double px = p.point.x();
        double py = p.point.y();

        if (px >= XminQ1 && px <= XmaxQ1 && py >= YminQ1 && py <= YmaxQ1)
        {
            q1.addPoint(p);
        }
        else if (px >= XminQ2 && px <= XmaxQ2 && py >= YminQ2 && py <= YmaxQ2)
        {
            q2.addPoint(p);
        }
        else if (px >= XminQ3 && px <= XmaxQ3 && py >= YminQ3 && py <= YmaxQ3)
        {
            q3.addPoint(p);
        }
        else if (px >= XminQ4 && px <= XmaxQ4 && py >= YminQ4 && py <= YmaxQ4)
        {
            q4.addPoint(p);
        }
    }
}

int generate_solution_cgal(const std::vector<Ponto> &points,
                           double x_max, double y_max,
                           double x_min, double y_min)
{
    std::queue<Quadrant> fila;

    int k_random = int_distr(gen);
    fila.push(Quadrant(points, x_max, y_max, x_min, y_min));

    int solution = 0;

    while (!fila.empty())
    {
        Quadrant current = fila.front();
        fila.pop();

        // Escolhe limites aleatórios para subdividir
        double rand1 = getRandomValue(current.x_min, current.x_max);
        double rand2 = getRandomValue(current.y_min, current.y_max);

        // Definir novos limites de subquadrantes
        double XminQ1 = rand1, YminQ1 = rand2, XmaxQ1 = current.x_max, YmaxQ1 = current.y_max;
        double XminQ2 = current.x_min, YminQ2 = rand2, XmaxQ2 = rand1, YmaxQ2 = current.y_max;
        double XminQ3 = current.x_min, YminQ3 = current.y_min, XmaxQ3 = rand1, YmaxQ3 = rand2;
        double XminQ4 = rand1, YminQ4 = current.y_min, XmaxQ4 = current.x_max, YmaxQ4 = rand2;

        // Cria quadrantes vazios
        Quadrant q1, q2, q3, q4;

        // Distribuir pontos
        distributePoints(current.points,
                         q1, XminQ1, XmaxQ1, YminQ1, YmaxQ1,
                         q2, XminQ2, XmaxQ2, YminQ2, YmaxQ2,
                         q3, XminQ3, XmaxQ3, YminQ3, YmaxQ3,
                         q4, XminQ4, XmaxQ4, YminQ4, YmaxQ4);

        // Lambda para processar um quadrante e decidir se subdivide ou gera componente
        auto processQuadrant = [&](Quadrant &q, double mx, double my, double mnx, double mny)
        {
            if (q.points.size() == 1)
            {
                // Já temos um ponto único
                Point centro = q.getCenter();
                Fuzzy_sphere_Ponto sphere(centro, raio);
                std::vector<Ponto> neighbors;
                tree.search(std::back_inserter(neighbors), sphere);

                Component aux(raio, neighbors, centro);
                manager.addComponent(aux);
                solution++;
            }
            else
            {
                // Distância entre cantos opostos
                double d = CGAL::squared_distance(
                    Point(q.x_max, q.y_max),
                    Point(q.x_min, q.y_min));
                // Usa d para decidir se gera componente

                if (d <= int_distr(gen) * raio2x4)
                {
                    Point centro = q.getCenter();
                    Fuzzy_sphere_Ponto sphere(centro, raio);
                    std::vector<Ponto> neighbors;
                    tree.search(std::back_inserter(neighbors), sphere);

                    Component aux(raio, neighbors, centro);
                    manager.addComponent(aux);
                    solution++;
                }
                else
                {
                    // Se não couber, subdivide mais
                    fila.push(Quadrant(q.points, mx, my, mnx, mny));
                }
            }
        };

        // Processa cada subquadrante se não estiver vazio
        if (!q1.points.empty())
            processQuadrant(q1, XmaxQ1, YmaxQ1, XminQ1, YminQ1);
        if (!q2.points.empty())
            processQuadrant(q2, XmaxQ2, YmaxQ2, XminQ2, YminQ2);
        if (!q3.points.empty())
            processQuadrant(q3, XmaxQ3, YmaxQ3, XminQ3, YminQ3);
        if (!q4.points.empty())
            processQuadrant(q4, XmaxQ4, YmaxQ4, XminQ4, YminQ4);
    }

    return solution;
}

int generate_solution_cgal_1(const std::vector<Ponto> &points,
                             double x_max, double y_max,
                             double x_min, double y_min)
{
    std::queue<Quadrant> fila;

    fila.push(Quadrant(points, x_max, y_max, x_min, y_min));

    int solution = 0;

    while (!fila.empty())
    {
        Quadrant current = fila.front();
        fila.pop();

        // Escolhe limites aleatórios para subdividir
        double rand1 = getRandomValue(current.x_min, current.x_max);
        double rand2 = getRandomValue(current.y_min, current.y_max);

        // Definir novos limites de subquadrantes
        double XminQ1 = rand1, YminQ1 = rand2, XmaxQ1 = current.x_max, YmaxQ1 = current.y_max;
        double XminQ2 = current.x_min, YminQ2 = rand2, XmaxQ2 = rand1, YmaxQ2 = current.y_max;
        double XminQ3 = current.x_min, YminQ3 = current.y_min, XmaxQ3 = rand1, YmaxQ3 = rand2;
        double XminQ4 = rand1, YminQ4 = current.y_min, XmaxQ4 = current.x_max, YmaxQ4 = rand2;

        // Cria quadrantes vazios
        Quadrant q1, q2, q3, q4;

        // Distribuir pontos
        distributePoints(current.points,
                         q1, XminQ1, XmaxQ1, YminQ1, YmaxQ1,
                         q2, XminQ2, XmaxQ2, YminQ2, YmaxQ2,
                         q3, XminQ3, XmaxQ3, YminQ3, YmaxQ3,
                         q4, XminQ4, XmaxQ4, YminQ4, YmaxQ4);

        // Lambda para processar um quadrante e decidir se subdivide ou gera componente
        auto processQuadrant = [&](Quadrant &q, double mx, double my, double mnx, double mny)
        {
            if (q.points.size() == 1)
            {
                // Já temos um ponto único
                Point centro = q.getCenter();
                Fuzzy_sphere_Ponto sphere(centro, raio);
                std::vector<Ponto> neighbors;
                tree.search(std::back_inserter(neighbors), sphere);

                Component aux(raio, neighbors, centro);
                manager.addComponent(aux);
                solution++;
            }
            else
            {
                // Distância entre cantos opostos
                double d = CGAL::squared_distance(
                    Point(q.x_max, q.y_max),
                    Point(q.x_min, q.y_min));
                // Usa d para decidir se gera componente

                if (d <= raio2x4)
                {
                    Point centro = q.getCenter();
                    Fuzzy_sphere_Ponto sphere(centro, raio);
                    std::vector<Ponto> neighbors;
                    tree.search(std::back_inserter(neighbors), sphere);

                    Component aux(raio, neighbors, centro);
                    manager.addComponent(aux);
                    solution++;
                }
                else
                {
                    // Se não couber, subdivide mais
                    fila.push(Quadrant(q.points, mx, my, mnx, mny));
                }
            }
        };

        // Processa cada subquadrante se não estiver vazio
        if (!q1.points.empty())
            processQuadrant(q1, XmaxQ1, YmaxQ1, XminQ1, YminQ1);
        if (!q2.points.empty())
            processQuadrant(q2, XmaxQ2, YmaxQ2, XminQ2, YminQ2);
        if (!q3.points.empty())
            processQuadrant(q3, XmaxQ3, YmaxQ3, XminQ3, YminQ3);
        if (!q4.points.empty())
            processQuadrant(q4, XmaxQ4, YmaxQ4, XminQ4, YminQ4);
    }

    return solution;
}

int construtivo_brkg(const std::vector<Ponto> &points)
{

    BolaTree *conjunto_bolas = new BolaTree();

    std::shuffle(indices.begin(), indices.end(), gen);

    for (int idx : indices)
    {

        bool flag_ponto_achou_bola = false;
        Ponto ponto_atual = pontos[idx];
        Fuzzy_sphere_Bola sphere(ponto_atual.point, 2.5 * raio);
        std::vector<Bola> neighbors;
        conjunto_bolas->search(std::back_inserter(neighbors), sphere);

        // Ordena vizinhos por distância crescente (prioriza bolas mais próximas)
        std::sort(neighbors.begin(), neighbors.end(), [&ponto_atual](const Bola &b1, const Bola &b2)
                  { return CGAL::squared_distance(ponto_atual.point, b1.centro) <
                           CGAL::squared_distance(ponto_atual.point, b2.centro); });
        if (neighbors.size() != 0)
        {
            for (auto &bola : neighbors)
            {
                bola.pontos_da_bola.push_back(pontos[idx]);
                Min_circle mc(bola.pontos_da_bola.begin(), bola.pontos_da_bola.end(), true);
                Traits_circle::Circle c = mc.circle();

                if (c.squared_radius() <= raio * raio)
                {
                    conjunto_bolas->remove(bola);
                    bola.centro = c.center();
                    flag_ponto_achou_bola = true;
                    conjunto_bolas->insert(bola);
                    break;
                }
                bola.pontos_da_bola.pop_back();
            }
        }
        if (flag_ponto_achou_bola == false)
        {
            std::vector<Ponto> pontos_da_bola = {pontos[idx]};
            Bola nova_bola(ponto_atual.point, pontos_da_bola, raio);
            conjunto_bolas->insert(nova_bola);
        }
    }

    // Modo atual - percorre na ordem natural da árvore
    for (auto &bola : *conjunto_bolas)
    {
        // Trabalhe com cada bola aqui
        Component aux(bola.raio, bola.pontos_da_bola, bola.centro);
        manager.addComponent(aux);
    }

    return conjunto_bolas->size();
}

int generate_solution_dr(const std::vector<Ponto> &points,
                         double x_max, double y_max,
                         double x_min, double y_min)
{
    // Função interna para calcular distância entre dois pontos
    auto distance = [](const Point &p1, const Point &p2) -> double
    {
        return CGAL::squared_distance(p1, p2);
    };

    // Função para inicializar centros usando farthest-first traversal
    auto init_centers = [&](const std::vector<Ponto> &pts) -> std::vector<Point>
    {
        std::vector<Point> centers;
        if (pts.empty())
            return centers;

        // Escolhe um ponto aleatório como primeiro centro
        std::uniform_int_distribution<> distr(0, pts.size() - 1);
        centers.push_back(pts[distr(gen)].point);

        // Cria lista de pontos restantes e distâncias mínimas
        std::vector<Ponto> remaining_points = pts;
        std::vector<double> min_distances(remaining_points.size(), std::numeric_limits<double>::max());

        while (true)
        {
            // Atualiza as distâncias mínimas para todos os pontos restantes
            double max_dist = -1;
            int max_idx = -1;

            for (size_t i = 0; i < remaining_points.size(); ++i)
            {
                double dist_to_new_center = distance(remaining_points[i].point, centers.back());
                min_distances[i] = std::min(min_distances[i], dist_to_new_center);

                if (min_distances[i] > max_dist)
                {
                    max_dist = min_distances[i];
                    max_idx = i;
                }
            }

            // Se a maior distância for menor ou igual ao quadrado do raio, todos os pontos estão cobertos
            if (max_dist <= raio * raio || max_idx == -1)
            {
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
    for (const auto &center : centers)
    {
        // Encontra todos os pontos dentro do raio
        Fuzzy_sphere_Ponto sphere(center, raio);
        std::vector<Ponto> neighbors;
        tree.search(std::back_inserter(neighbors), sphere);

        // Cria e adiciona o componente
        Component aux(raio, neighbors, center);
        manager.addComponent(aux);
        current_solution++;
    }

    return current_solution;
}

// Inicialização dos centros para o algoritmo DR melhorado
std::vector<Point> init_centers_enhanced(const std::vector<Ponto> &demand_points)
{
    std::vector<Point> centers;

    if (demand_points.empty())
        return centers;

    // Escolhe um ponto de demanda aleatório como o primeiro centro
    std::uniform_int_distribution<> distr(0, demand_points.size() - 1);
    centers.push_back(demand_points[distr(gen)].point);

    // Filtra os pontos restantes
    std::vector<Ponto> remaining_points;
    for (const auto &p : demand_points)
    {
        if (p.point != centers[0])
        {
            remaining_points.push_back(p);
        }
    }

    while (!remaining_points.empty())
    {
        // Encontra o ponto com a maior distância até o centro mais próximo
        double max_squared_dist = -1;
        Point max_point;
        int max_idx = -1;

        for (size_t i = 0; i < remaining_points.size(); ++i)
        {
            const Point &p = remaining_points[i].point;
            double min_squared_dist = std::numeric_limits<double>::max();

            for (const Point &c : centers)
            {
                double squared_dist = CGAL::squared_distance(p, c);
                min_squared_dist = std::min(min_squared_dist, squared_dist);
            }

            if (min_squared_dist > max_squared_dist)
            {
                max_squared_dist = min_squared_dist;
                max_point = p;
                max_idx = i;
            }
        }

        // Se a maior distância for menor ou igual ao raio², todos os pontos estão cobertos
        if (max_squared_dist <= raio * raio)
        {
            break;
        }

        // Adiciona o ponto como novo centro
        centers.push_back(max_point);

        // Remove o ponto dos pontos restantes
        if (max_idx >= 0 && max_idx < remaining_points.size())
        {
            remaining_points[max_idx] = remaining_points.back();
            remaining_points.pop_back();
        }
    }

    return centers;
}

// Atribui cada ponto de demanda ao seu centro mais próximo
std::vector<std::vector<Ponto>> assign_to_clusters_enhanced(
    const std::vector<Ponto> &demand_points,
    const std::vector<Point> &centers)
{

    std::vector<std::vector<Ponto>> clusters(centers.size());

    for (const auto &p : demand_points)
    {
        // Encontra o centro mais próximo
        double min_squared_dist = std::numeric_limits<double>::max();
        size_t min_idx = 0;

        for (size_t i = 0; i < centers.size(); ++i)
        {
            double squared_dist = CGAL::squared_distance(p.point, centers[i]);
            if (squared_dist < min_squared_dist)
            {
                min_squared_dist = squared_dist;
                min_idx = i;
            }
        }

        // Adiciona o ponto ao cluster correspondente
        clusters[min_idx].push_back(p);
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

//         // Passo 3: Determinar os centros dos clusters usando Min_circle do CGAL

//         std::vector<double> cluster_radii;
//         centers.clear();
//         for (const auto& cluster : clusters) {
//             if (cluster.empty()) {
//                 continue;
//             } else {
//                 //std::vector<Point> cluster_points = convertPontosToPoints(cluster);

//                 Min_circle mc(cluster.begin(), cluster.end(), true);

//                 centers.push_back(mc.circle().center());
//                 //std::cout << "Centro do cluster: " << mc.circle().center() << std::endl;
//                 cluster_radii.push_back(std::sqrt(mc.circle().squared_radius()));
//             }
//         }

//         // Verifica se os centros mudaram significativamente
//         centers_changed = false;

//         if (centers.size() != previous_centers.size()) {
//             centers_changed = true;
//         } else {

//             for(int j = 0; j< centers.size(); j++) {
//                 double distance_squared = CGAL::squared_distance(centers[j], previous_centers[j]);
//                 if (distance_squared > tolerance * tolerance) {
//                     centers_changed = true;
//                     break;
//                 }
//             }
//             // for (const auto& c_new : centers) {
//             //     double min_squared_dist = std::numeric_limits<double>::max();
//             //     for (const auto& c_old : previous_centers) {
//             //         double squared_dist = CGAL::squared_distance(c_new, c_old);
//             //         min_squared_dist = std::min(min_squared_dist, squared_dist);
//             //     }
//             //     if (min_squared_dist > tolerance * tolerance) {
//             //         centers_changed = true;
//             //         break;
//             //     }
//             // }
//         }

//         // Passo 4: Calcular rmax(C)
//         if (!cluster_radii.empty()) {
//             double rmax = *std::max_element(cluster_radii.begin(), cluster_radii.end());

//             // Se rmax ≤ r, registrar a solução atual e tentar remover um centro
//             if (rmax <= raio) {
//                 best_k = k;
//                 best_centers = centers;

//                 // Estratégia de remoção - remove o menor círculo
//                 if (k > 1) {
//                     auto min_it = std::min_element(cluster_radii.begin(), cluster_radii.end());
//                     size_t idx_to_remove = std::distance(cluster_radii.begin(), min_it);

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

//                 // Perturbação: substitui o centro do maior cluster por um ponto aleatório
//                 if (!cluster_radii.empty() && !clusters.empty()) {
//                     auto max_it = std::max_element(cluster_radii.begin(), cluster_radii.end());
//                     size_t largest_idx = std::distance(cluster_radii.begin(), max_it);

//                     if (largest_idx < clusters.size() && !clusters[largest_idx].empty()) {
//                         std::uniform_int_distribution<> distr(0, clusters[largest_idx].size() - 1);
//                         int random_idx = distr(gen);
//                         centers[largest_idx] = clusters[largest_idx][random_idx].point;
//                         centers_changed = true;
//                     }
//                 }
//             } else {
//                 if (best_k != last_best_k || best_centers != last_best_centers) {
//                     last_best_k = best_k;
//                     last_best_centers = best_centers;

//                     // Nova perturbação
//                     if (!cluster_radii.empty() && !clusters.empty()) {
//                         auto max_it = std::max_element(cluster_radii.begin(), cluster_radii.end());
//                         size_t largest_idx = std::distance(cluster_radii.begin(), max_it);

//                         if (largest_idx < clusters.size() && !clusters[largest_idx].empty()) {
//                             std::uniform_int_distribution<> distr(0, clusters[largest_idx].size() - 1);
//                             int random_idx = distr(gen);
//                             centers[largest_idx] = clusters[largest_idx][random_idx].point;
//                             centers_changed = true;
//                         }
//                     }
//                 } else {
//                     break;
//                 }
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

//     //std::cout << "manager.size()" << manager.components.size() << std::endl;

//     return best_centers.size();
// }

// Inicialização otimizada usando índices

// Versão otimizada combinando o melhor de ambas:
std::vector<Point> init_centers_enhanced_optimized(const std::vector<Ponto> &demand_points)
{
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

    for (size_t i = 0; i < demand_points.size(); ++i)
    {
        if (i != center_idx)
        {
            remaining_indices.push_back(i);
        }
    }

    while (!remaining_indices.empty())
    {
        double max_squared_dist = -1;
        size_t best_idx = 0;
        int best_pos = -1;

        for (size_t pos = 0; pos < remaining_indices.size(); ++pos)
        {
            size_t i = remaining_indices[pos];
            double min_squared_dist = std::numeric_limits<double>::max();

            for (size_t atual_idx : centers_idx)
            {
                // Use cache SE for mais rápido, senão cálculo direto
                double squared_dist;
                if (0)
                { // Assumindo método para verificar se cache é válido
                    squared_dist = cache.getSquaredDistance(i, atual_idx);
                }
                else
                {
                    squared_dist = CGAL::squared_distance(demand_points[i].point, centers[atual_idx]);
                }
                min_squared_dist = std::min(min_squared_dist, squared_dist);
            }

            if (min_squared_dist > max_squared_dist)
            {
                max_squared_dist = min_squared_dist;
                best_idx = i;
                best_pos = pos;
            }
        }

        if (max_squared_dist <= raio2)
        {
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

int generate_solution_dr_enhanced(const std::vector<Ponto> &points)
{
#include <chrono>

    auto start_total = std::chrono::high_resolution_clock::now();

    // Medição do Passo 1
    auto start_init = std::chrono::high_resolution_clock::now();
    //FASTCOVER_PP fastcover(points); 200 ms
    //auto centers = fastcover.execute_dr();
    auto centers = init_centers_enhanced(points);
    auto end_init = std::chrono::high_resolution_clock::now();
    auto time_init = std::chrono::duration_cast<std::chrono::microseconds>(end_init - start_init).count();

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

    // Variáveis para medição de tempo
    long long total_time_assignment = 0;
    long long total_time_min_circle = 0;
    long long total_time_convergence = 0;
    long long total_time_perturbation = 0;
    int iterations = 0;

    while (true)
    {
        iterations++;

        // Salva os centros atuais para comparação
        previous_centers = centers;

        // Passo 2: Atribuir pontos de demanda aos centros mais próximos
        auto start_assignment = std::chrono::high_resolution_clock::now();
        auto clusters = assign_to_clusters_enhanced(points, centers);
        auto end_assignment = std::chrono::high_resolution_clock::now();
        total_time_assignment += std::chrono::duration_cast<std::chrono::microseconds>(end_assignment - start_assignment).count();

        // Passo 3: Determinar os centros dos clusters usando Min_circle do CGAL
        auto start_min_circle = std::chrono::high_resolution_clock::now();
        std::vector<double> cluster_radii;
        centers.clear();
        for (const auto &cluster : clusters)
        {
            if (cluster.empty())
            {
                continue;
            }
            else
            {
                Min_circle mc(cluster.begin(), cluster.end(), true);
                centers.push_back(mc.circle().center());
                cluster_radii.push_back(std::sqrt(mc.circle().squared_radius()));
            }
        }
        auto end_min_circle = std::chrono::high_resolution_clock::now();
        total_time_min_circle += std::chrono::duration_cast<std::chrono::microseconds>(end_min_circle - start_min_circle).count();

        // Verifica se os centros mudaram significativamente
        auto start_convergence = std::chrono::high_resolution_clock::now();
        centers_changed = false;

        if (centers.size() != previous_centers.size())
        {
            centers_changed = true;
        }
        else
        {
            for (int j = 0; j < centers.size(); j++)
            {
                double distance_squared = CGAL::squared_distance(centers[j], previous_centers[j]);
                if (distance_squared > tolerance * tolerance)
                {
                    centers_changed = true;
                    break;
                }
            }
        }

        // Passo 4: Calcular rmax(C)
        if (!cluster_radii.empty())
        {
            double rmax = *std::max_element(cluster_radii.begin(), cluster_radii.end());

            // Se rmax ≤ r, registrar a solução atual e tentar remover um centro
            if (rmax <= raio)
            {
                best_k = k;
                best_centers = centers;

                // Estratégia de remoção - remove o menor círculo
                if (k > 1)
                {
                    auto min_it = std::min_element(cluster_radii.begin(), cluster_radii.end());
                    size_t idx_to_remove = std::distance(cluster_radii.begin(), min_it);

                    centers.erase(centers.begin() + idx_to_remove);
                    k--;
                    centers_changed = true;
                }
            }
        }
        auto end_convergence = std::chrono::high_resolution_clock::now();
        total_time_convergence += std::chrono::duration_cast<std::chrono::microseconds>(end_convergence - start_convergence).count();

        // Passo 5: Verificar se os centros pararam de mudar
        auto start_perturbation = std::chrono::high_resolution_clock::now();
        if (!centers_changed)
        {
            if (first_no_change)
            {
                first_no_change = false;
                last_best_k = best_k;
                last_best_centers = best_centers;

                // Perturbação: substitui o centro do maior cluster por um ponto aleatório
                if (!cluster_radii.empty() && !clusters.empty())
                {
                    auto max_it = std::max_element(cluster_radii.begin(), cluster_radii.end());
                    size_t largest_idx = std::distance(cluster_radii.begin(), max_it);

                    if (largest_idx < clusters.size() && !clusters[largest_idx].empty())
                    {
                        std::uniform_int_distribution<> distr(0, clusters[largest_idx].size() - 1);
                        int random_idx = distr(gen);
                        centers[largest_idx] = clusters[largest_idx][random_idx].point;
                        centers_changed = true;
                    }
                }
            }
            else
            {
                if (best_k != last_best_k || best_centers != last_best_centers)
                {
                    last_best_k = best_k;
                    last_best_centers = best_centers;

                    // Nova perturbação
                    if (!cluster_radii.empty() && !clusters.empty())
                    {
                        auto max_it = std::max_element(cluster_radii.begin(), cluster_radii.end());
                        size_t largest_idx = std::distance(cluster_radii.begin(), max_it);

                        if (largest_idx < clusters.size() && !clusters[largest_idx].empty())
                        {
                            std::uniform_int_distribution<> distr(0, clusters[largest_idx].size() - 1);
                            int random_idx = distr(gen);
                            centers[largest_idx] = clusters[largest_idx][random_idx].point;
                            centers_changed = true;
                        }
                    }
                }
                else
                {
                    break;
                }
            }
        }
        auto end_perturbation = std::chrono::high_resolution_clock::now();
        total_time_perturbation += std::chrono::duration_cast<std::chrono::microseconds>(end_perturbation - start_perturbation).count();

        if (!centers_changed)
        {
            break;
        }
    }

    // Criação dos componentes finais
    auto start_final = std::chrono::high_resolution_clock::now();
    auto clusters = assign_to_clusters_enhanced(points, best_centers);
    for (int i = 0; i < best_centers.size(); i++)
    {
        Component aux(raio, clusters[i], best_centers[i]);
        manager.addComponent(aux);
    }
    auto end_final = std::chrono::high_resolution_clock::now();
    auto time_final = std::chrono::duration_cast<std::chrono::microseconds>(end_final - start_final).count();

    auto end_total = std::chrono::high_resolution_clock::now();
    auto time_total = std::chrono::duration_cast<std::chrono::microseconds>(end_total - start_total).count();

    // Imprimir estatísticas de tempo
    if (0)
    {
        std::cout << "=== Estatísticas de Tempo DR Enhanced ===" << std::endl;
        std::cout << "Tempo total: " << time_total << " μs" << std::endl;
        std::cout << "Inicialização: " << time_init << " μs (" << (time_init * 100.0 / time_total) << "%)" << std::endl;
        std::cout << "Atribuição de clusters (total): " << total_time_assignment << " μs (" << (total_time_assignment * 100.0 / time_total) << "%)" << std::endl;
        std::cout << "Min_circle (total): " << total_time_min_circle << " μs (" << (total_time_min_circle * 100.0 / time_total) << "%)" << std::endl;
        std::cout << "Verificação convergência (total): " << total_time_convergence << " μs (" << (total_time_convergence * 100.0 / time_total) << "%)" << std::endl;
        std::cout << "Perturbações (total): " << total_time_perturbation << " μs (" << (total_time_perturbation * 100.0 / time_total) << "%)" << std::endl;
        std::cout << "Criação final: " << time_final << " μs (" << (time_final * 100.0 / time_total) << "%)" << std::endl;
        std::cout << "Número de iterações: " << iterations << std::endl;
        std::cout << "Tempo médio por iteração: " << (time_total / iterations) << " μs" << std::endl;
        std::cout << "========================================" << std::endl;
    }
    return best_centers.size();
}

int generate_solution_dr_enhanced_leve(const std::vector<Ponto> &points)
{
    // Passo 1: Escolher k centros iniciais
    auto centers = init_centers_enhanced(points);
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

    while (true)
    {
        // Salva os centros atuais para comparação
        previous_centers = centers;

        // Passo 2: Atribuir pontos de demanda aos centros mais próximos
        auto clusters = assign_to_clusters_enhanced(points, centers);

        // Passo 3: Determinar os centros dos clusters usando Min_circle do CGAL

        std::vector<double> cluster_radii;
        centers.clear();
        for (const auto &cluster : clusters)
        {
            if (cluster.empty())
            {
                continue;
            }
            else
            {
                // std::vector<Point> cluster_points = convertPontosToPoints(cluster);

                Min_circle mc(cluster.begin(), cluster.end(), true);

                centers.push_back(mc.circle().center());
                // std::cout << "Centro do cluster: " << mc.circle().center() << std::endl;
                cluster_radii.push_back(std::sqrt(mc.circle().squared_radius()));
            }
        }

        // Verifica se os centros mudaram significativamente
        centers_changed = false;

        if (centers.size() != previous_centers.size())
        {
            centers_changed = true;
        }
        else
        {

            for (int j = 0; j < centers.size(); j++)
            {
                double distance_squared = CGAL::squared_distance(centers[j], previous_centers[j]);
                if (distance_squared > tolerance * tolerance)
                {
                    centers_changed = true;
                    break;
                }
            }
        }

        // Passo 4: Calcular rmax(C)
        if (!cluster_radii.empty())
        {
            double rmax = *std::max_element(cluster_radii.begin(), cluster_radii.end());

            // Se rmax ≤ r, registrar a solução atual e tentar remover um centro
            if (rmax <= raio)
            {
                best_k = k;
                best_centers = centers;

                // Estratégia de remoção - remove o menor círculo
                if (k > 1)
                {
                    auto min_it = std::min_element(cluster_radii.begin(), cluster_radii.end());
                    size_t idx_to_remove = std::distance(cluster_radii.begin(), min_it);

                    centers.erase(centers.begin() + idx_to_remove);
                    k--;
                    centers_changed = true;
                }
            }
        }

        // Passo 5: Verificar se os centros pararam de mudar
        if (!centers_changed)
        {
            if (first_no_change)
            {
                first_no_change = false;
                last_best_k = best_k;
                last_best_centers = best_centers;

                // Perturbação: substitui o centro do maior cluster por um ponto aleatório
                if (!cluster_radii.empty() && !clusters.empty())
                {
                    auto max_it = std::max_element(cluster_radii.begin(), cluster_radii.end());
                    size_t largest_idx = std::distance(cluster_radii.begin(), max_it);

                    if (largest_idx < clusters.size() && !clusters[largest_idx].empty())
                    {
                        std::uniform_int_distribution<> distr(0, clusters[largest_idx].size() - 1);
                        int random_idx = distr(gen);
                        centers[largest_idx] = clusters[largest_idx][random_idx].point;
                        centers_changed = true;
                    }
                }
            }
            else
            {
                if (best_k != last_best_k || best_centers != last_best_centers)
                {
                    last_best_k = best_k;
                    last_best_centers = best_centers;

                    // Nova perturbação
                    if (!cluster_radii.empty() && !clusters.empty())
                    {
                        auto max_it = std::max_element(cluster_radii.begin(), cluster_radii.end());
                        size_t largest_idx = std::distance(cluster_radii.begin(), max_it);

                        if (largest_idx < clusters.size() && !clusters[largest_idx].empty())
                        {
                            std::uniform_int_distribution<> distr(0, clusters[largest_idx].size() - 1);
                            int random_idx = distr(gen);
                            centers[largest_idx] = clusters[largest_idx][random_idx].point;
                            centers_changed = true;
                        }
                    }
                }
                else
                {
                    break;
                }
            }
        }

        if (!centers_changed)
        {
            break;
        }
    }

    // Cria componentes para a melhor solução encontrada
    auto clusters = assign_to_clusters_enhanced(points, best_centers);
    // identify_uncovered_points(points, best_centers, raio);
    for (int i = 0; i < best_centers.size(); i++)
    {
        Component aux(raio, clusters[i], best_centers[i]);
        manager.addComponent(aux);
    }

    // std::cout << "manager.size()" << manager.components.size() << std::endl;

    return best_centers.size();
}

int generate_divise_dr(const std::vector<Ponto> &points,
                       double x_max, double y_max,
                       double x_min, double y_min)
{
    std::queue<Quadrant> fila;

    fila.push(Quadrant(points, x_max, y_max, x_min, y_min));

    int solution = 0;

    while (!fila.empty())
    {
        Quadrant current = fila.front();
        fila.pop();

        // Escolhe limites aleatórios para subdividir
        double rand1 = getRandomValue(current.x_min, current.x_max);
        double rand2 = getRandomValue(current.y_min, current.y_max);

        // Definir novos limites de subquadrantes
        double XminQ1 = rand1, YminQ1 = rand2, XmaxQ1 = current.x_max, YmaxQ1 = current.y_max;
        double XminQ2 = current.x_min, YminQ2 = rand2, XmaxQ2 = rand1, YmaxQ2 = current.y_max;
        double XminQ3 = current.x_min, YminQ3 = current.y_min, XmaxQ3 = rand1, YmaxQ3 = rand2;
        double XminQ4 = rand1, YminQ4 = current.y_min, XmaxQ4 = current.x_max, YmaxQ4 = rand2;

        // Cria quadrantes vazios
        Quadrant q1, q2, q3, q4;

        // Distribuir pontos
        distributePoints(current.points,
                         q1, XminQ1, XmaxQ1, YminQ1, YmaxQ1,
                         q2, XminQ2, XmaxQ2, YminQ2, YmaxQ2,
                         q3, XminQ3, XmaxQ3, YminQ3, YmaxQ3,
                         q4, XminQ4, XmaxQ4, YminQ4, YmaxQ4);

        // Lambda para processar um quadrante e decidir se subdivide ou gera componente
        auto processQuadrant = [&](Quadrant &q, double mx, double my, double mnx, double mny)
        {
            if (q.points.size() == 1)
            {
                // Já temos um ponto único
                Point centro = q.getCenter();
                Fuzzy_sphere_Ponto sphere(centro, raio);
                std::vector<Ponto> neighbors;
                tree.search(std::back_inserter(neighbors), sphere);

                Component aux(raio, neighbors, centro);
                manager.addComponent(aux);
                solution++;
            }
            else
            {
                // Distância entre cantos opostos
                double d = CGAL::squared_distance(
                    Point(q.x_max, q.y_max),
                    Point(q.x_min, q.y_min));
                // Usa d para decidir se gera componente

                if (d <= 4 * raio2)
                {

                    Point centro = q.getCenter();
                    Fuzzy_sphere_Ponto sphere(centro, 2 * raio);
                    std::vector<Ponto> neighbors;
                    tree.search(std::back_inserter(neighbors), sphere);

                    generate_solution_dr_enhanced(neighbors);
                    solution++;
                }
                else
                {
                    // Se não couber, subdivide mais
                    fila.push(Quadrant(q.points, mx, my, mnx, mny));
                }
            }
        };

        // Processa cada subquadrante se não estiver vazio
        if (!q1.points.empty())
            processQuadrant(q1, XmaxQ1, YmaxQ1, XminQ1, YminQ1);
        if (!q2.points.empty())
            processQuadrant(q2, XmaxQ2, YmaxQ2, XminQ2, YminQ2);
        if (!q3.points.empty())
            processQuadrant(q3, XmaxQ3, YmaxQ3, XminQ3, YminQ3);
        if (!q4.points.empty())
            processQuadrant(q4, XmaxQ4, YmaxQ4, XminQ4, YminQ4);
    }

    return solution;
}

int generate_divise_dr_profundidade(const std::vector<Ponto> &points,
                       double x_max, double y_max,
                       double x_min, double y_min,
                       int max_depth)  // Parâmetro para profundidade máxima
{
    // Estrutura para armazenar quadrante com sua profundidade
    struct QuadrantWithDepth {
        Quadrant quad;
        int depth;
        
        QuadrantWithDepth(const Quadrant& q, int d) : quad(q), depth(d) {}
        QuadrantWithDepth(const std::vector<Ponto>& pts, double x_max, double y_max, 
                         double x_min, double y_min, int d) 
            : quad(pts, x_max, y_max, x_min, y_min), depth(d) {}
    };

    std::queue<QuadrantWithDepth> fila;

    // Inicia com profundidade 0
    fila.push(QuadrantWithDepth(points, x_max, y_max, x_min, y_min, 0));

    int solution = 0;

    while (!fila.empty())
    {
        QuadrantWithDepth current = fila.front();
        fila.pop();

        // CONDIÇÃO DE PARADA: Se atingiu a profundidade máxima
        if (current.depth >= max_depth)
        {
            // Processa o quadrante atual como solução final
            //Point centro = current.quad.getCenter();
            //Fuzzy_sphere_Ponto sphere(centro, 2 * raio);
            //std::vector<Ponto> neighbors;
            //tree.search(std::back_inserter(neighbors), sphere);

            Point centro = current.quad.getCenter();
            double aux = max(current.quad.x_max - current.quad.x_min,
                             current.quad.y_max - current.quad.y_min);
            double raio_final = aux  * sqrt(2.0);
            Fuzzy_sphere_Ponto sphere(centro, raio_final);
            std::vector<Ponto> neighbors;
            tree.search(std::back_inserter(neighbors), sphere);

            solution = generate_solution_dr_enhanced(neighbors) + solution;
            
            continue; // Pula para o próximo item da fila
        }

        // Escolhe limites aleatórios para subdividir
        double rand1 = getRandomValue(current.quad.x_min, current.quad.x_max);
        double rand2 = getRandomValue(current.quad.y_min, current.quad.y_max);

        // Definir novos limites de subquadrantes
        double XminQ1 = rand1, YminQ1 = rand2, XmaxQ1 = current.quad.x_max, YmaxQ1 = current.quad.y_max;
        double XminQ2 = current.quad.x_min, YminQ2 = rand2, XmaxQ2 = rand1, YmaxQ2 = current.quad.y_max;
        double XminQ3 = current.quad.x_min, YminQ3 = current.quad.y_min, XmaxQ3 = rand1, YmaxQ3 = rand2;
        double XminQ4 = rand1, YminQ4 = current.quad.y_min, XmaxQ4 = current.quad.x_max, YmaxQ4 = rand2;

        // Cria quadrantes vazios
        Quadrant q1, q2, q3, q4;

        // Distribuir pontos
        distributePoints(current.quad.points,
                         q1, XminQ1, XmaxQ1, YminQ1, YmaxQ1,
                         q2, XminQ2, XmaxQ2, YminQ2, YmaxQ2,
                         q3, XminQ3, XmaxQ3, YminQ3, YmaxQ3,
                         q4, XminQ4, XmaxQ4, YminQ4, YmaxQ4);

        // Lambda para processar um quadrante
        auto processQuadrant = [&](Quadrant &q, double mx, double my, double mnx, double mny)
        {
            if (q.points.size() == 1)
            {
                // Já temos um ponto único - gera componente
                Point centro = q.getCenter();
                Fuzzy_sphere_Ponto sphere(centro, raio);
                std::vector<Ponto> neighbors;
                tree.search(std::back_inserter(neighbors), sphere);

                Component aux(raio, neighbors, centro);
                manager.addComponent(aux);
                solution++;
            }
            else
            {
                // Adiciona à fila com profundidade incrementada
                fila.push(QuadrantWithDepth(q.points, mx, my, mnx, mny, current.depth + 1));
            }
        };

        // Processa cada subquadrante se não estiver vazio
        if (!q1.points.empty())
            processQuadrant(q1, XmaxQ1, YmaxQ1, XminQ1, YminQ1);
        if (!q2.points.empty())
            processQuadrant(q2, XmaxQ2, YmaxQ2, XminQ2, YminQ2);
        if (!q3.points.empty())
            processQuadrant(q3, XmaxQ3, YmaxQ3, XminQ3, YminQ3);
        if (!q4.points.empty())
            processQuadrant(q4, XmaxQ4, YmaxQ4, XminQ4, YminQ4);
    }

    return solution;
}