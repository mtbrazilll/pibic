
#include "construct.hpp"

class Quadrante {
public:

	std::vector<Ponto> points;
    double x_max;
    double y_max;
    double x_min;
    double y_min;

    Quadrante(const std::vector<Ponto>& points, double x_max, double y_max, double x_min, double y_min)
        : points(points), x_max(x_max), y_max(y_max), x_min(x_min), y_min(y_min) {}
};

double getRandomValue(double minVal, double maxVal) {
	
    double randomValue = distr2(gen); // Número entre 0 e 1
    return minVal + randomValue * (maxVal - minVal); // Mapeia para o intervalo [minVal, maxVal]
}
 
int generate_solution(vector<Ponto> const &points, double x_max, double y_max, double x_min, double y_min, int qnt_disco) {
	

	queue<Quadrante> fila;
	Quadrante pai(points, x_max, y_max, x_min, y_min);
	fila.push(pai);

	int solution = 0;
	//std::cout << "Iniciando processamento com " << points.size() << " pontos." << std::endl;

	while (!fila.empty()){

		
		auto pai = fila.front();
        fila.pop();
		

		//std::cout << "Processando Quadranteo com " << pai.points.size() << " pontos." << std::endl;
		//std::cout << pai.x_min << " " << pai.x_max <<  " " << pai.x_max - pai.x_min <<std::endl;
		//std::cout << pai.y_min << " " << pai.y_max << " " << pai.y_max - pai.y_min <<std::endl;


		
		


		if(pai.points.size()==1){
			
			//	std::cout << "Adicionando componente com um único ponto: " << pai.points[0].point << std::endl;

				Component aux(raio,pai.points,pai.points[0].point);
				manager.addComponent(aux);	
				solution++;
				continue;		

		}
			
		if(fabs(pai.x_max - pai.x_min) <= raio*4 && fabs(pai.y_max - pai.y_min) <= raio*4){
		//if (true){

            Min_circle mc(pai.points.begin(), pai.points.end(), true);
            Traits_circle::Circle c = mc.circle();

	
			if (raio >= std::sqrt(c.squared_radius()) * (EPSILON)) { 
				//std::cout << "oi /n";

				Component aux(raio,pai.points,c.center());
				manager.addComponent(aux);
				//std::cout << "Adicionando componente com círculo menor ou igual ao raio." << std::endl;
				solution++;
				continue;

			}

			
		}

			 

		double rand1 = getRandomValue(pai.x_min, pai.x_max);
		double rand2 = getRandomValue(pai.y_min, pai.y_max);
			

		double XminQ1 = rand1, 	   YminQ1 = rand2,     XmaxQ1 = pai.x_max, YmaxQ1 = pai.y_max;
		double XminQ2 = pai.x_min, YminQ2 = rand2,     XmaxQ2 = rand1,     YmaxQ2 = pai.y_max;
		double XminQ3 = pai.x_min, YminQ3 = pai.y_min, XmaxQ3 = rand1,     YmaxQ3 = rand2;
		double XminQ4 = rand1, 	   YminQ4 = pai.y_min, XmaxQ4 = pai.x_max, YmaxQ4 = rand2;
			
		std::vector<Ponto> quad1, quad2, quad3, quad4;

		double Xminq1 = std::numeric_limits<K::FT>::max(), 	   Yminq1 = std::numeric_limits<K::FT>::max(),     Xmaxq1 = std::numeric_limits<K::FT>::lowest(), 	   Ymaxq1 = std::numeric_limits<K::FT>::lowest();
		double Xminq2 = std::numeric_limits<K::FT>::max(), 	   Yminq2 = std::numeric_limits<K::FT>::max(),     Xmaxq2 = std::numeric_limits<K::FT>::lowest(),     Ymaxq2 = std::numeric_limits<K::FT>::lowest();
		double Xminq3 = std::numeric_limits<K::FT>::max(),     Yminq3 = std::numeric_limits<K::FT>::max(), 	   Xmaxq3 = std::numeric_limits<K::FT>::lowest(),     Ymaxq3 = std::numeric_limits<K::FT>::lowest();
		double Xminq4 = std::numeric_limits<K::FT>::max(), 	   Yminq4 = std::numeric_limits<K::FT>::max(),     Xmaxq4 = std::numeric_limits<K::FT>::lowest(), 	   Ymaxq4 = std::numeric_limits<K::FT>::lowest();

		for (int i = 0; i < pai.points.size(); i++) {

			//std::cout << pai.points[i].indice << "\n";


			if ((pai.points[i].point.x() >= XminQ1 && pai.points[i].point.x() <= XmaxQ1) && (pai.points[i].point.y() >= YminQ1 && pai.points[i].point.y() <= YmaxQ1)) {

				quad1.push_back(pai.points[i]);

			}
			else if ((pai.points[i].point.x() >= XminQ2 && pai.points[i].point.x() <= XmaxQ2) && (pai.points[i].point.y() >= YminQ2 && pai.points[i].point.y() <= YmaxQ2)) {

				quad2.push_back(pai.points[i]);

			}
			else if ((pai.points[i].point.x() >= XminQ3 && pai.points[i].point.x() <= XmaxQ3) && (pai.points[i].point.y() >= YminQ3 && pai.points[i].point.y() <= YmaxQ3)) {

				quad3.push_back(pai.points[i]);

			}
			else if ((pai.points[i].point.x() >= XminQ4 && pai.points[i].point.x() <= XmaxQ4) && (pai.points[i].point.y() >= YminQ4 && pai.points[i].point.y() <= YmaxQ4)) {

				quad4.push_back(pai.points[i]);

			}

		}

		if (!quad1.empty()) fila.push(Quadrante(quad1, XmaxQ1, YmaxQ1, XminQ1, YminQ1));
		if (!quad2.empty()) fila.push(Quadrante(quad2, XmaxQ2, YmaxQ2, XminQ2, YminQ2));
		if (!quad3.empty()) fila.push(Quadrante(quad3, XmaxQ3, YmaxQ3, XminQ3, YminQ3));
		if (!quad4.empty()) fila.push(Quadrante(quad4, XmaxQ4, YmaxQ4, XminQ4, YminQ4));

	}


	return solution;

}



double distance(const Ponto& a, const Ponto& b) {
    double dx = CGAL::to_double(a.point.x() - b.point.x());
    double dy = CGAL::to_double(a.point.y() - b.point.y());
    return sqrt(dx * dx + dy * dy);
}


struct SubQuadrant {
    std::vector<Ponto> points;

    double x_min;
    double x_max;
    double y_min;
    double y_max;

    SubQuadrant()
        : x_min(std::numeric_limits<K::FT>::max()),
          x_max(std::numeric_limits<K::FT>::lowest()),
          y_min(std::numeric_limits<K::FT>::max()),
          y_max(std::numeric_limits<K::FT>::lowest()) {}

    CGAL::Point_2<K> getCenter() const {
        double center_x = (static_cast<double>(x_min) + static_cast<double>(x_max)) / 2.0;
        double center_y = (static_cast<double>(y_min) + static_cast<double>(y_max)) / 2.0;
        return CGAL::Point_2<K>(center_x, center_y);
    }
};



int generate_solution_cgal(const std::vector<Ponto>& points, double x_max, double y_max, double x_min, double y_min) {
    std::queue<Quadrante> fila;

    
    Quadrante pai(points, x_max, y_max, x_min, y_min);
    fila.push(pai);

    int solution = 0;

    while (!fila.empty()) {
        auto pai = fila.front();
        fila.pop();



        double rand1 = getRandomValue(pai.x_min, pai.x_max);
        double rand2 = getRandomValue(pai.y_min, pai.y_max);

        double XminQ1 = rand1,     YminQ1 = rand2,     XmaxQ1 = pai.x_max, YmaxQ1 = pai.y_max;
        double XminQ2 = pai.x_min, YminQ2 = rand2,     XmaxQ2 = rand1,     YmaxQ2 = pai.y_max;
        double XminQ3 = pai.x_min, YminQ3 = pai.y_min, XmaxQ3 = rand1,     YmaxQ3 = rand2;
        double XminQ4 = rand1,     YminQ4 = pai.y_min, XmaxQ4 = pai.x_max, YmaxQ4 = rand2;

        // Definir os subquadrantes
        SubQuadrant quad1, quad2, quad3, quad4;

        // Distribuir os pontos e atualizar os extremos
        for (const Ponto& p : pai.points) {
            if ((p.point.x() >= XminQ1 && p.point.x() <= XmaxQ1) &&
                (p.point.y() >= YminQ1 && p.point.y() <= YmaxQ1)) {
                quad1.points.push_back(p);
                // Atualizar extremos
                if (p.point.x() < quad1.x_min) {
                    quad1.x_min = p.point.x();
                    
                }
                if (p.point.x() > quad1.x_max) {
                    quad1.x_max = p.point.x();
                   
                }
                if (p.point.y() < quad1.y_min) {
                    quad1.y_min = p.point.y();
                    
                }
                if (p.point.y() > quad1.y_max) {
                    quad1.y_max = p.point.y();
                 
                }
            }
            else if ((p.point.x() >= XminQ2 && p.point.x() <= XmaxQ2) &&
                     (p.point.y() >= YminQ2 && p.point.y() <= YmaxQ2)) {
                quad2.points.push_back(p);
                // Atualizar extremos
                if (p.point.x() < quad2.x_min) {
                    quad2.x_min = p.point.x();
              
                }
                if (p.point.x() > quad2.x_max) {
                    quad2.x_max = p.point.x();
                   
                }
                if (p.point.y() < quad2.y_min) {
                    quad2.y_min = p.point.y();
                   
                }
                if (p.point.y() > quad2.y_max) {
                    quad2.y_max = p.point.y();
                   
                }
            }
            else if ((p.point.x() >= XminQ3 && p.point.x() <= XmaxQ3) &&
                     (p.point.y() >= YminQ3 && p.point.y() <= YmaxQ3)) {
                quad3.points.push_back(p);
                // Atualizar extremos
                if (p.point.x() < quad3.x_min) {
                    quad3.x_min = p.point.x();
                  
                }
                if (p.point.x() > quad3.x_max) {
                    quad3.x_max = p.point.x();
                    
                }
                if (p.point.y() < quad3.y_min) {
                    quad3.y_min = p.point.y();
                   
                }
                if (p.point.y() > quad3.y_max) {
                    quad3.y_max = p.point.y();
                 
                }
            }
            else if ((p.point.x() >= XminQ4 && p.point.x() <= XmaxQ4) &&
                     (p.point.y() >= YminQ4 && p.point.y() <= YmaxQ4)) {
                quad4.points.push_back(p);
                // Atualizar extremos
                if (p.point.x() < quad4.x_min) {
                    quad4.x_min = p.point.x();
                    
                }
                if (p.point.x() > quad4.x_max) {
                    quad4.x_max = p.point.x();
                   
                }
                if (p.point.y() < quad4.y_min) {
                    quad4.y_min = p.point.y();
           
                }
                if (p.point.y() > quad4.y_max) {
                    quad4.y_max = p.point.y();
               
                }
            }
        }

        // Função lambda para processar subquadrantes
        auto processSubQuadrant = [&](SubQuadrant& quad, double Xmax, double Ymax, double Xmin, double Ymin) {
            if (quad.points.size() == 1) {


                Point centro = quad.getCenter();
                Fuzzy_sphere sphere(centro, raio);

                std::vector<Ponto> neighbors;
                tree.search(std::back_inserter(neighbors), sphere);
                Component aux(raio, neighbors, centro);
                manager.addComponent(aux);
                solution++;
            }
            else {
                // Calcular distâncias entre pontos extremos
                
                
                double d1 = CGAL::squared_distance(Point(quad.x_max, quad.y_max), Point(quad.x_min, quad.y_min));

                if (d1 <= 4.0 * raio*raio) {

                    Point centro = quad.getCenter();
                    Fuzzy_sphere sphere(centro, raio);

                    std::vector<Ponto> neighbors;
                    tree.search(std::back_inserter(neighbors), sphere);
                    Component aux(raio, neighbors, centro);
                    manager.addComponent(aux);
                    solution++;

                }
                else {
                    fila.push(Quadrante(quad.points, Xmax, Ymax, Xmin, Ymin));
                } 
            }
        };

        // Processar quadrantes não vazios
        if (!quad1.points.empty())
            processSubQuadrant(quad1, XmaxQ1, YmaxQ1, XminQ1, YminQ1);
        if (!quad2.points.empty())
            processSubQuadrant(quad2, XmaxQ2, YmaxQ2, XminQ2, YminQ2);
        if (!quad3.points.empty())
            processSubQuadrant(quad3, XmaxQ3, YmaxQ3, XminQ3, YminQ3);
        if (!quad4.points.empty())
            processSubQuadrant(quad4, XmaxQ4, YmaxQ4, XminQ4, YminQ4);
    }

    return solution;
}