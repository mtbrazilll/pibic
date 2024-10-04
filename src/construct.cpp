
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
 
void mateus(vector<Ponto> const &points, double x_max, double y_max, double x_min, double y_min) {
	

	queue<Quadrante> fila;
	Quadrante pai(points, x_max, y_max, x_min, y_min);
	fila.push(pai);

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
				continue;		

		}
			
		//if(fabs(pai.x_max - pai.x_min) <= raio*10 && fabs(pai.y_max - pai.y_min) <= raio*10){
		if (true){
			Circle smallest = makeSmallestEnclosingCircle(pai.points);
		//	std::cout << "pos: " << smallest.pos << std::endl;
		//	std::cout << "raio: " << smallest.r << std::endl;
			//if (smallest.r >= 1) std::cout<<"miss"<<std::endl;
			if (raio >= smallest.r * (EPSILON)) { 
				//std::cout << "oi /n";

				Component aux(raio,pai.points,smallest.pos);
				manager.addComponent(aux);
				//std::cout << "Adicionando componente com círculo menor ou igual ao raio." << std::endl;
				continue;

			}

			
		}

			 

		double rand1 = getRandomValue(pai.x_min-1, pai.x_max+1);
		double rand2 = getRandomValue(pai.y_min-1, pai.y_max+1);

		//std::cout << "rand 1: " << rand1 << " rand 2: " << rand2 << std::endl;	

		double XminQ1 = rand1, 	   YminQ1 = rand2,     XmaxQ1 = pai.x_max+1, YmaxQ1 = pai.y_max+1;
		double XminQ2 = pai.x_min-1, YminQ2 = rand2,     XmaxQ2 = rand1,     YmaxQ2 = pai.y_max+1;
		double XminQ3 = pai.x_min-1, YminQ3 = pai.y_min-1, XmaxQ3 = rand1,     YmaxQ3 = rand2;
		double XminQ4 = rand1, 	   YminQ4 = pai.y_min-1, XmaxQ4 = pai.x_max+1, YmaxQ4 = rand2;
			
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


	return ;

}



void mateus2(const std::vector<Ponto>& points, double x_max, double y_max, double x_min, double y_min) {
    std::queue<Quadrante> fila;
    Quadrante pai(points, x_max, y_max, x_min, y_min);
    fila.push(pai);

    while (!fila.empty()) {
        auto pai = fila.front();
        fila.pop();

        // Base cases remain the same...
		if(pai.points.size()==1){
			
			//	std::cout << "Adicionando componente com um único ponto: " << pai.points[0].point << std::endl;

				Component aux(raio,pai.points,pai.points[0].point);
				manager.addComponent(aux);	
				continue;		

		}
			
		if(fabs(pai.x_max - pai.x_min) <= raio*4 && fabs(pai.y_max - pai.y_min) <= raio*4){

			Circle smallest = makeSmallestEnclosingCircle(pai.points);
		//	std::cout << "pos: " << smallest.pos << std::endl;
		//	std::cout << "raio: " << smallest.r << std::endl;
			//if (smallest.r >= 1) std::cout<<"miss"<<std::endl;
			if (raio >= smallest.r * (EPSILON)) { 
				//std::cout << "oi /n";

				Component aux(raio,pai.points,smallest.pos);
				manager.addComponent(aux);
				//std::cout << "Adicionando componente com círculo menor ou igual ao raio." << std::endl;
				continue;

			}

			
		}

        // Compute medians
        std::vector<double> x_coords, y_coords;
        for (const auto& p : pai.points) {
            x_coords.push_back(p.point.x());
            y_coords.push_back(p.point.y());
        }
        size_t mid_x = x_coords.size() / 2;
        size_t mid_y = y_coords.size() / 2;
        std::nth_element(x_coords.begin(), x_coords.begin() + mid_x, x_coords.end());
        std::nth_element(y_coords.begin(), y_coords.begin() + mid_y, y_coords.end());
        double median_x = x_coords[mid_x];
        double median_y = y_coords[mid_y];

        // Controlled randomness
        double x_range = pai.x_max - pai.x_min;
        double y_range = pai.y_max - pai.y_min;

        // Define the offset ratio (e.g., 0.1 for 10%)
        double offset_ratio = 0.1;
        std::uniform_real_distribution<> distr(-offset_ratio, offset_ratio);
        double x_offset = distr(gen) * x_range;
        double y_offset = distr(gen) * y_range;

        double adjusted_median_x = median_x + x_offset;
        double adjusted_median_y = median_y + y_offset;

        // Ensure adjusted medians are within bounds
        adjusted_median_x = std::max(pai.x_min, std::min(adjusted_median_x, pai.x_max));
        adjusted_median_y = std::max(pai.y_min, std::min(adjusted_median_y, pai.y_max));

        // Define sub-quadrants
        // Quadrant boundaries
        double XminQ1 = adjusted_median_x, YminQ1 = adjusted_median_y, XmaxQ1 = pai.x_max, YmaxQ1 = pai.y_max;
        double XminQ2 = pai.x_min,         YminQ2 = adjusted_median_y, XmaxQ2 = adjusted_median_x, YmaxQ2 = pai.y_max;
        double XminQ3 = pai.x_min,         YminQ3 = pai.y_min,         XmaxQ3 = adjusted_median_x, YmaxQ3 = adjusted_median_y;
        double XminQ4 = adjusted_median_x, YminQ4 = pai.y_min,         XmaxQ4 = pai.x_max,         YmaxQ4 = adjusted_median_y;

        // Assign points to sub-quadrants
        std::vector<Ponto> quad1, quad2, quad3, quad4;
        for (const auto& point : pai.points) {
            if (point.point.x() >= adjusted_median_x && point.point.y() >= adjusted_median_y) {
                quad1.push_back(point);
            } else if (point.point.x() < adjusted_median_x && point.point.y() >= adjusted_median_y) {
                quad2.push_back(point);
            } else if (point.point.x() < adjusted_median_x && point.point.y() < adjusted_median_y) {
                quad3.push_back(point);
            } else {
                quad4.push_back(point);
            }
        }

        // Enqueue non-empty sub-quadrants
        if (!quad1.empty()) fila.push(Quadrante(quad1, XmaxQ1, YmaxQ1, XminQ1, YminQ1));
        if (!quad2.empty()) fila.push(Quadrante(quad2, XmaxQ2, YmaxQ2, XminQ2, YminQ2));
        if (!quad3.empty()) fila.push(Quadrante(quad3, XmaxQ3, YmaxQ3, XminQ3, YminQ3));
        if (!quad4.empty()) fila.push(Quadrante(quad4, XmaxQ4, YmaxQ4, XminQ4, YminQ4));
    }
}

