// SmallestEnclosingCircle.hpp

#ifndef SMALLEST_ENCLOSING_CIRCLE_HPP
#define SMALLEST_ENCLOSING_CIRCLE_HPP

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Min_circle_2.h>
#include <CGAL/Min_circle_2_traits_2.h>
#include <vector>

typedef CGAL::Simple_cartesian<long double> K;
typedef K::Point_2 Point;
typedef CGAL::Min_circle_2_traits_2<K> Traits;
typedef CGAL::Min_circle_2<Traits> Min_circle;

struct Circle {
    Point c;   // Centro
    double r;  // Raio
	Point pos;
};



struct Ponto {
    CGAL::Simple_cartesian<long double>::Point_2 point;


    
    unsigned long int indice;

    // Constructor
    Ponto(long double x = 0, long double y = 0, unsigned long int indice = 0)
        : point(x, y),
          indice(indice) {}


              operator CGAL::Simple_cartesian<long double>::Point_2() const {
        return point;
    }

          
};



Circle makeSmallestEnclosingCircle(const std::vector<Ponto>& points);

#endif // SMALLEST_ENCLOSING_CIRCLE_HPP
