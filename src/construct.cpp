
#include "construct.hpp"


using namespace std;



bool outputQuadrants = false;  // Set this flag as needed
extern std::unordered_map<Point, std::vector<int>> cellToDisks;

class Element {
public:

	std::vector<Ponto> points;
    double x_max;
    double y_max;
    double x_min;
    double y_min;

    Element(const std::vector<Ponto>& points, double x_max, double y_max, double x_min, double y_min)
        : points(points), x_max(x_max), y_max(y_max), x_min(x_min), y_min(y_min) {}
};

double getRandomValue(double minVal, double maxVal) {
    double randomValue = static_cast<double>(rand()) / RAND_MAX; // Número entre 0 e 1
    return minVal + randomValue * (maxVal - minVal); // Mapeia para o intervalo [minVal, maxVal]
}
 
int mateus(vector<Ponto> const &points, double x_max, double y_max, double x_min, double y_min) {
	
	double raio = 1.0;
	queue<Element> fila;
	Element pai(points, x_max, y_max, x_min, y_min);
	fila.push(pai);

	while (!fila.empty()){

		
		auto pai = fila.front();
        fila.pop();
		 

		if (pai.x_min >= pai.x_max || pai.y_max <= pai.y_min) continue;
		
		if (fabs(pai.x_max - pai.x_min) > 0 && fabs(pai.y_max - pai.y_min) > 0) {


			if(pai.points.size()==1){
				
				std::vector<Ponto> pontos = pai.points;
				Component aux(id_aux_d,0,0,1.0,pontos,pai.points[0].point);

				manager.addComponent(aux);
				id_aux_d++;
				
				continue;

			}
			
			if(fabs(pai.x_max - pai.x_min) <= 10.0 && fabs(pai.y_max - pai.y_min) <= 10.0){

				Circle smallest = makeSmallestEnclosingCircle(pai.points);
					
				if (1.0 >= smallest.r * (EPSILON)) { 
					
					std::vector<Ponto> pontos = pai.points;
					Component aux(id_aux_d,0,0,1.0,pontos,smallest.pos);

					manager.addComponent(aux);
					id_aux_d++;
					continue;
				}
					
				
			}

			double rand1 = getRandomValue(pai.x_min, pai.x_max);
			double rand2 = getRandomValue(pai.y_min, pai.y_max);
				
			
			
			double XminQ1 = rand1, 	   YminQ1 = rand2,     XmaxQ1 = pai.x_max, YmaxQ1 = pai.y_max;
			double XminQ2 = pai.x_min, YminQ2 = rand2,     XmaxQ2 = rand1,     YmaxQ2 = pai.y_max;
			double XminQ3 = pai.x_min, YminQ3 = pai.y_min, XmaxQ3 = rand1,     YmaxQ3 = rand2;
			double XminQ4 = rand1, 	   YminQ4 = pai.y_min, XmaxQ4 = pai.x_max, YmaxQ4 = rand2;
				
			vector<Ponto> quad1, quad2, quad3, quad4;

			for (int i = 0; i < pai.points.size(); i++) {

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

			if (!quad1.empty()) fila.push(Element(quad1, XmaxQ1, YmaxQ1, XminQ1, YminQ1));
			if (!quad2.empty()) fila.push(Element(quad2, XmaxQ2, YmaxQ2, XminQ2, YminQ2));
			if (!quad3.empty()) fila.push(Element(quad3, XmaxQ3, YmaxQ3, XminQ3, YminQ3));
			if (!quad4.empty()) fila.push(Element(quad4, XmaxQ4, YmaxQ4, XminQ4, YminQ4));
	

		}		
		

	}


	return 0;

}


