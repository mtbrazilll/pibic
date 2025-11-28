#ifndef STRUCT_H 
#define STRUCT_H 


#include <CGAL/Simple_cartesian.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/Kd_tree.h>
#include <CGAL/property_map.h>
#include <CGAL/Search_traits_2.h>
#include <CGAL/Fuzzy_sphere.h>  
#include <CGAL/Min_circle_2.h>
#include <CGAL/Min_circle_2_traits_2.h>
#include <CGAL/squared_distance_2.h>


// Definições de tipos
typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_2 Point;


typedef CGAL::Min_circle_2_traits_2<K> Traits_circle;
typedef CGAL::Min_circle_2<Traits_circle> Min_circle;


struct Ponto {

    

    std::vector<unsigned int> neighbors; // To store indices of neighbors

    unsigned int indice;
    Point point;

    // Constructor
    Ponto( double x = 0,  double y = 0, unsigned int indice = 0)
        : point(x, y), indice(indice) {}


    operator CGAL::Simple_cartesian<double>::Point_2() const {
        return point;
    }

          
};   

struct Bola {

    Point centro;
    
    //Point borda_1;
    //Point borda_2;
    //Point borda_3;
    std::vector<Point> bordas;
    std::vector<int> pontos_indices; 
    Min_circle *mc = nullptr;
    double raio;
   
    // Constructor
    
    Bola() : centro(0, 0), raio(0) {}

    Bola(Point centro,  std::vector<int> &pontos_indices, double& raio) : centro(centro), pontos_indices(pontos_indices), raio(raio){}

    operator CGAL::Simple_cartesian<double>::Point_2() const {
        return centro;
    }

    void adicionarPonto(int idx) {
        pontos_indices.push_back(idx);
    }

          
}; 

// Mapa de propriedade para acessar o ponto de Ponto
struct Ponto_to_Point_map {
    typedef Ponto key_type;
    typedef Point value_type;
    typedef const value_type& reference;
    typedef boost::readable_property_map_tag category;

    friend inline reference get(const Ponto_to_Point_map&, const key_type& k) {
        return k.point;
    }
};

struct Bola_to_Point_map {
    typedef Bola key_type;
    typedef Point value_type;
    typedef const value_type& reference;
    typedef boost::readable_property_map_tag category;

    friend inline reference get(const Bola_to_Point_map&, const key_type& b) {
        return b.centro;
    }
};


// Mapa de propriedade para acessar o ponto a partir de um índice int
struct Index_to_Point_map {
    typedef int key_type;
    typedef Point value_type;
    typedef const value_type& reference;
    typedef boost::readable_property_map_tag category;

    friend inline reference get(const Index_to_Point_map&, const key_type& idx);
};

typedef CGAL::Search_traits_2<K> Base_traits;
// Traits para Ponto

typedef CGAL::Search_traits_adapter<Ponto, Ponto_to_Point_map, Base_traits> PontoTraits;
typedef CGAL::Kd_tree<PontoTraits> PontoTree;
typedef CGAL::Fuzzy_sphere<PontoTraits> Fuzzy_sphere_Ponto;

// Traits para Bola
typedef CGAL::Search_traits_adapter<Bola, Bola_to_Point_map, Base_traits> BolaTraits;
typedef CGAL::Kd_tree<BolaTraits> BolaTree;
typedef CGAL::Fuzzy_sphere<BolaTraits> Fuzzy_sphere_Bola;

// Traits para Index (int)
typedef CGAL::Search_traits_adapter<int, Index_to_Point_map, Base_traits> IndexTraits;
typedef CGAL::Kd_tree<IndexTraits> IndexTree;
typedef CGAL::Fuzzy_sphere<IndexTraits> Fuzzy_sphere_Index;

extern std::vector<Ponto> pontos;

inline Index_to_Point_map::reference get(const Index_to_Point_map&, const Index_to_Point_map::key_type& idx) {
    return pontos[idx].point;
}

#endif