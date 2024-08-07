
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

			 

		double rand1 = getRandomValue(pai.x_min-1, pai.x_max+1);
		double rand2 = getRandomValue(pai.y_min-1, pai.y_max+1);
			

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


