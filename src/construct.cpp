
#include "construct.hpp"



double getRandomValue(double minVal, double maxVal) {
	
    double randomValue = distr2(gen); // Número entre 0 e 1
    return minVal + randomValue * (maxVal - minVal); // Mapeia para o intervalo [minVal, maxVal]
}
 

class Quadrant {
    public:
        std::vector<Ponto> points;
        double x_min, x_max;
        double y_min, y_max;
    
        Quadrant(const std::vector<Ponto>& pts, double Xmax, double Ymax, double Xmin, double Ymin)
            : points(pts), x_min(Xmin), x_max(Xmax), y_min(Ymin), y_max(Ymax) {}
    
        // Construtor vazio para criar quadrantes vazios
        Quadrant()
            : x_min(std::numeric_limits<double>::max()),
              x_max(std::numeric_limits<double>::lowest()),
              y_min(std::numeric_limits<double>::max()),
              y_max(std::numeric_limits<double>::lowest()) {}
    
        // Atualiza os limites ao adicionar um ponto
        void addPoint(const Ponto& p) {
            points.push_back(p);
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
    };
    
    // Função auxiliar para distribuir pontos em novos quadrantes
    void distributePoints(const std::vector<Ponto>& pts,
                          Quadrant& q1, double XminQ1, double XmaxQ1, double YminQ1, double YmaxQ1,
                          Quadrant& q2, double XminQ2, double XmaxQ2, double YminQ2, double YmaxQ2,
                          Quadrant& q3, double XminQ3, double XmaxQ3, double YminQ3, double YmaxQ3,
                          Quadrant& q4, double XminQ4, double XmaxQ4, double YminQ4, double YmaxQ4)
    {
        for (const auto& p : pts) {
            double px = p.point.x();
            double py = p.point.y();
    
            if (px >= XminQ1 && px <= XmaxQ1 && py >= YminQ1 && py <= YmaxQ1) {
                q1.addPoint(p);
            } else if (px >= XminQ2 && px <= XmaxQ2 && py >= YminQ2 && py <= YmaxQ2) {
                q2.addPoint(p);
            } else if (px >= XminQ3 && px <= XmaxQ3 && py >= YminQ3 && py <= YmaxQ3) {
                q3.addPoint(p);
            } else if (px >= XminQ4 && px <= XmaxQ4 && py >= YminQ4 && py <= YmaxQ4) {
                q4.addPoint(p);
            }
        }
    }
    
    int generate_solution_cgal(const std::vector<Ponto>& points,
                               double x_max, double y_max,
                               double x_min, double y_min)
    {
        std::queue<Quadrant> fila;
        fila.push(Quadrant(points, x_max, y_max, x_min, y_min));
    
        int solution = 0;
    
        while (!fila.empty()) {
            Quadrant current = fila.front();
            fila.pop();
    
            // Escolhe limites aleatórios para subdividir
            double rand1 = getRandomValue(current.x_min, current.x_max);
            double rand2 = getRandomValue(current.y_min, current.y_max);
    
            // Definir novos limites de subquadrantes
            double XminQ1 = rand1,     YminQ1 = rand2,     XmaxQ1 = current.x_max, YmaxQ1 = current.y_max;
            double XminQ2 = current.x_min, YminQ2 = rand2,     XmaxQ2 = rand1,     YmaxQ2 = current.y_max;
            double XminQ3 = current.x_min, YminQ3 = current.y_min, XmaxQ3 = rand1,     YmaxQ3 = rand2;
            double XminQ4 = rand1,     YminQ4 = current.y_min, XmaxQ4 = current.x_max, YmaxQ4 = rand2;
    
            // Cria quadrantes vazios
            Quadrant q1, q2, q3, q4;
    

            // Distribuir pontos
            distributePoints(current.points, 
                             q1, XminQ1, XmaxQ1, YminQ1, YmaxQ1,
                             q2, XminQ2, XmaxQ2, YminQ2, YmaxQ2,
                             q3, XminQ3, XmaxQ3, YminQ3, YmaxQ3,
                             q4, XminQ4, XmaxQ4, YminQ4, YmaxQ4);
    
            // Lambda para processar um quadrante e decidir se subdivide ou gera componente
            auto processQuadrant = [&](Quadrant& q, double mx, double my, double mnx, double mny) {
                if (q.points.size() == 1) {
                    // Já temos um ponto único
                    Point centro = q.getCenter();
                    Fuzzy_sphere_Ponto sphere(centro, raio);
                    std::vector<Ponto> neighbors;
                    tree.search(std::back_inserter(neighbors), sphere);
    
                    Component aux(raio, neighbors, centro);
                    manager.addComponent(aux);
                    solution++;
                } else {
                    // Distância entre cantos opostos
                    double d = CGAL::squared_distance(
                        Point(q.x_max, q.y_max),
                        Point(q.x_min, q.y_min)
                    );
                    // Usa d para decidir se gera componente
                    if (d <= 4.0 * raio * raio) {
                        Point centro = q.getCenter();
                        Fuzzy_sphere_Ponto sphere(centro, raio);
                        std::vector<Ponto> neighbors;
                        tree.search(std::back_inserter(neighbors), sphere);
    
                        Component aux(raio, neighbors, centro);
                        manager.addComponent(aux);
                        solution++;
                    } else {
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


int construtivo_brkg(const std::vector<Ponto>& points){

    BolaTree* conjunto_bolas = new BolaTree();

    std::shuffle(indices.begin(), indices.end(), gen);

    for (int idx : indices) {
        
        bool flag_ponto_achou_bola = false;	
        Ponto ponto_atual = pontos[idx];
        Fuzzy_sphere_Bola sphere(ponto_atual.point, 2*raio);
        std::vector<Bola> neighbors;
        conjunto_bolas->search(std::back_inserter(neighbors), sphere);

        if(neighbors.size() != 0){
            for(auto& bola: neighbors){
                bola.pontos_da_bola.push_back(pontos[idx]);
                Min_circle mc(bola.pontos_da_bola.begin(), bola.pontos_da_bola.end(), true);
                Traits_circle::Circle c = mc.circle();

                if (c.squared_radius() <= raio * raio){
                    conjunto_bolas->remove(bola);
                    bola.centro = c.center();
                    flag_ponto_achou_bola = true;	
                    conjunto_bolas->insert(bola);
                    break;
                }
                bola.pontos_da_bola.pop_back();
            }
        }
        if (flag_ponto_achou_bola == false){
            std::vector<Ponto> pontos_da_bola =  {pontos[idx]};
            Bola nova_bola(ponto_atual.point,pontos_da_bola,raio);
            conjunto_bolas->insert(nova_bola);
        }
    }   

    
   // Modo atual - percorre na ordem natural da árvore
    for (auto &bola: *conjunto_bolas) {
        // Trabalhe com cada bola aqui
        Component aux(bola.raio, bola.pontos_da_bola, bola.centro);
        manager.addComponent(aux);
    }

    return conjunto_bolas->size();
}


int generate_solution_dr(const std::vector<Ponto>& points,
    double x_max, double y_max,
    double x_min, double y_min)
{
    // Função interna para calcular distância entre dois pontos
    auto distance = [](const Point& p1, const Point& p2) -> double {
    return CGAL::squared_distance(p1, p2);
    };

    // Função para inicializar centros usando farthest-first traversal
    auto init_centers = [&](const std::vector<Ponto>& pts) -> std::vector<Point> {
    std::vector<Point> centers;
    if (pts.empty()) return centers;

    // Escolhe um ponto aleatório como primeiro centro
    std::uniform_int_distribution<> distr(0, pts.size() - 1);
    centers.push_back(pts[distr(gen)].point);

    // Cria lista de pontos restantes e distâncias mínimas
    std::vector<Ponto> remaining_points = pts;
    std::vector<double> min_distances(remaining_points.size(), std::numeric_limits<double>::max());

    while (true) {
    // Atualiza as distâncias mínimas para todos os pontos restantes
    double max_dist = -1;
    int max_idx = -1;

    for (size_t i = 0; i < remaining_points.size(); ++i) {
    double dist_to_new_center = distance(remaining_points[i].point, centers.back());
    min_distances[i] = std::min(min_distances[i], dist_to_new_center);

    if (min_distances[i] > max_dist) {
    max_dist = min_distances[i];
    max_idx = i;
    }
    }

    // Se a maior distância for menor ou igual ao quadrado do raio, todos os pontos estão cobertos
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

    if (remaining_points.empty()) break;
    }

    return centers;
    };

    
   
   
        // Limpa o manager para esta iteração


    // Inicializa centros
    std::vector<Point> centers = init_centers(points);

    // Para cada centro, cria um componente
    int current_solution = 0;
    for (const auto& center : centers) {
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