﻿#include "stdafx.h"
#include "Resolution.h"

#include <cassert>
// ##include <pthread.h>
#include <thread>
#include <map>

using namespace std;

Resolution::Resolution(const Field &_field, const Parameters &_params) :
    field(_field), params(_params)
{
}

Resolution::~Resolution()
{
}

/// #########################
///      Getters
/// #########################
//@{

//@}
/// #########################
///      Setters
/// #########################
//@{

void Resolution::set_params(const Parameters &_params)
{
    params = _params;
}

//@}
/// #########################
///     Calculs de données
/// #########################
//@{

void Resolution::initCoordNeighbourhoodManhattan(const Coordinates &coord)
{
    if (field[coord] == is_usable) {
        cout << "Premier point : " << coord << endl;
        // On calcule et additionne le ratio pour aller vers chacun des successeurs
        Coordinates coord2(coord);
        // On commence à la coordonnée suivante de celle courante
        while (field.nextCoordinates(&coord2)) {
            // On calcule la distance que si elle n'a pas déjà été calculée
            if (field[coord2] == is_usable) {
		if (getRoadDistance(coord, coord2) == 0) {
                    road_distances[coord.row][coord.col][coord2.row][coord2.col] = calcRoadDistance(coord, coord2);
                }
            }
        }
    }
}


void Resolution::initNeighbourhoodManhattan()
{
    Coordinates coord1 = Field::first();

    // Initialisation, selon la taille, de toutes les cellules à 0
    road_distances.resize(field.get_height());
    // Initialisation de première colonne
    for (unsigned x1 = 0; x1 < field.get_height(); ++x1) {
        road_distances[x1].resize(field.get_width());

        // Initialisation de la deuxieme ligne
        for (unsigned y1 = 0; y1 < field.get_width(); ++y1) {
            road_distances[x1][y1].resize(field.get_height());

            // Initialisation de la deuxième colonne, à 0
            for (unsigned x2 = 0; x2 < field.get_height(); ++x2) {
                road_distances[x1][y1][x2].resize(field.get_width(), 0.0);
            }
        }
    }

    // Calculs des ratios de distances
    coord1 = Field::first();
    do {
        initCoordNeighbourhoodManhattan(coord1);
    } while (field.nextCoordinates(&coord1));
}


//@}
/// #########################
///   Fonctions auxiliaires
/// #########################
//@{

unsigned Resolution::calcRoadDistance(const Coordinates &coord1, const Coordinates &coord2)
{
    if (coord1 == coord2) {
#if DEBUG_ROADS_DIST
        clog << "les cases de départ et d'arrivée sont identiques" << endl;
#endif
        return 0;
    }

    unsigned min_dist = UNSIGNED_INFINITY;
#if DEBUG_ROADS_DIST
    Coordinates min_dist_neighbour_c1(0, 0);
    Coordinates min_dist_neighbour_c2(0, 0);
#endif

    list<Coordinates> *serving_roads_c1 = field.getServingRoads(coord1, params.get_serve_distance());

//    cout << "\tRatio entre : "<< coord1<< " et "<< coord2<< endl;
    list<Coordinates> *serving_roads_c2 = field.getServingRoads(coord2, params.get_serve_distance());
    for (Coordinates road_c1 : *serving_roads_c1) {
        for (Coordinates road_c2 : *serving_roads_c2) {
            list<Coordinates> *empty_visited = new list<Coordinates>;
            unsigned int dist = recCalcRoadDistance(road_c1, road_c2, empty_visited, UNSIGNED_INFINITY);
            delete empty_visited;

            if (dist < min_dist) {
                min_dist = dist + 1;
#if DEBUG_ROADS_DIST
                min_dist_neighbour_c1 = road_c1;
                min_dist_neighbour_c2 = road_c2;
#endif
            }
        }
    }

    delete serving_roads_c1;
    delete serving_roads_c2;

    if (min_dist) {
#if DEBUG_ROADS_DIST
        cout << "Les plus court chemin de " << coord1 << " à " << coord2 << " est de longueur " << min_dist << endl;
#endif
    } else {
        cerr << "Impossible de relier les deux parcelle par les routes"
             "(au moins une des 2 parcelle n'a pas de route à proximité (distance < " << params.get_serve_distance() << "))" << endl;
    }

    return min_dist;
}

unsigned Resolution::recCalcRoadDistance(const Coordinates &coord1, const Coordinates &coord2, list< Coordinates > *visited, unsigned int dist_max)
{
    #if DEBUG_ROADS_DIST
    cout << "Calcul de la distance entre " << coord1 << " et " << coord2 << endl;
    #endif
    assert(field[coord1] == is_road);
    
    if (coord1 == coord2) {
	return 0;
    } else {
	if (visited->size() >= dist_max) {
	    return UNSIGNED_INFINITY;
	} else {
	    unsigned min_dist = UNSIGNED_INFINITY;
	    //         Coordinates& min_dist_neighbour;
	    
	    list<Coordinates> *neighbour_roads = field.getNeighbourRoads(coord1);
	    for (Coordinates new_coord : *neighbour_roads) {
		#if DEBUG_ROADS_DIST
		cout << "Taille de la liste des visitées : " << visited->size() << endl;
		#endif
		//  recherche de cet élément dans la liste des visités
		if (find(visited->begin(), visited->end(), new_coord) == visited->end()) {
		    
		    // On applique la recursivité avec la coordonnée envisagée courante
		    unsigned dist;
		    float current_ratio = getRoadDistance(coord1, coord2);
		    if (current_ratio != 0) {
			dist = current_ratio;
		    } else {
			visited->push_back(coord1);
			dist= recCalcRoadDistance(new_coord,  coord2, visited, min_dist);
			visited->pop_back();
			road_distances[coord1.row][coord1.col][coord2.row][coord2.col] = dist;
		    }
		    
		    // On compare le résultat obtenu avec le minimum courant
		    if (dist < min_dist) {
			min_dist = dist;
		    }
		} else {
		    #if DEBUG_ROADS_DIST
		    cout << "La parcelle a déjà été visitée" << endl;
		    #endif
		}
	    }
	    
	    delete neighbour_roads;
	    
	    #if DEBUG_ROADS_DIST
	    cout << "Pour aller de " << coord1 << " à " << coord2 << ", il y a un trajet de longueur " << min_dist << endl;
	    #endif
	    return min_dist +1;
	}
    }
}


//@}
/// #########################
///      Evaluations
/// #########################
//@{

unsigned int Resolution::evaluateTotalUsable() const
{
    unsigned nb_usables = 0;
#if DEBUG_EVALUATION
    int x = 0, y = 0;
#endif
    for (vector<State> row_parcel_state : field) {
        for (State parcel_state : row_parcel_state) {
#if DEBUG_EVALUATION
            cout << "state(" << x << "," << y << ")= " << parcel_state << "; ";
            if (x == 0) cout << endl;
            ++x;
#endif
            assert(parcel_state >= -1 &&  parcel_state <= is_unusable);
            if (parcel_state == is_usable) {
                ++nb_usables;
            }
        }
#if DEBUG_EVALUATION
        ++y;
        x = 0;
#endif
    }

#if DEBUG_EVALUATION
    cout << "Total number of usables parcels : " << nb_usables << endl;
#endif
    assert(nb_usables >= 0 && nb_usables < field.getNbParcels() && "nombre d'exploitables incohérent");

    return nb_usables;
}

float Resolution::manhattanRatioBetween2Parcels(const Coordinates &p1, const Coordinates &p2) const
{
    unsigned road_distance = getRoadDistance(p1, p2);
    unsigned manhattan_distance = p1.manhattanDistance(p2);
    float ratio = road_distance / (float) manhattan_distance;

    return ratio;
}

float Resolution::evaluateRatio() const
{
    float total_ratio = 0.0;

    // Calculs des distances
    Coordinates coord1 = Field::first();
    do {
        if (field[coord1] == is_usable) {
            cout << "Premier point : " << coord1 << endl;
            // On calcule et additionne le ratio pour aller vers chacun des successeurs
            Coordinates coord2(coord1);
            // On commence à la coordonnée suivante de celle courante
            while (field.nextCoordinates(&coord2)) {
                if (field[coord2] == is_usable) {
		    float ratio_c1_goto_c2 = manhattanRatioBetween2Parcels(coord1, coord2);
                    total_ratio += 2.0 * ratio_c1_goto_c2; // @see on pourrait faire un décalage de bit
                }
            }
        }
    } while (field.nextCoordinates(&coord1));

    return total_ratio;
}

float Resolution::threadsEvaluateRatio() const
{
    // Initialisation des threads
    vector<pair<thread *, float *>> threads;

    // Calculs des distances
    Coordinates coord1 = Field::first();
    do {
        if (field[coord1] == is_usable) {
            // On calcule et additionne le ratio pour aller vers chacun des successeurs
            float *ratio = new float;
            thread *my_thread = new thread(TParcelRatios, coord1, ratio, this);
            threads.push_back(make_pair(my_thread, ratio));
        }
    } while (field.nextCoordinates(&coord1));

    float total_ratio = 0.0;
    for (pair<thread *, float *> thread_result : threads) {
        thread_result.first->join();
        total_ratio += (*thread_result.second);
#if DEBUG_EVALUATION
        cout << "On ajoute " << (*thread_result.second) << ", le ratio total est " << total_ratio << endl;
#endif
        delete thread_result.second;
    }

//    for(vector<pair<thread*, float*>>::iterator it(threads.end()); it != threads.begin(); --it ){
//        pair<thread*, float*>  thread_result;
//        thread_result.first->join();
//        total_ratio += (*thread_result.second);
//#if DEBUG_EVALUATION
//        cout << "On ajoute "<< (*thread_result.second)<< ", le ratio total est "<< total_ratio<< endl;
//#endif
//        delete thread_result.second;
//    }

    return total_ratio;
}

//@}
/// #########################
/// 	Threads
/// #########################
//@{

void TParcelRatios(Coordinates coord, float *ratio, const Resolution *res)
{
    (*ratio) = 0.0;
    cout << "Le ratio de " << coord << " au début est " << (*ratio) << endl;
    Coordinates coord2(coord);
    // On commence à la coordonnée suivante de celle courante
    while (res->field.nextCoordinates(&coord2)) {
        if (res->field[coord2] == is_usable) {
            float ratio_c1_goto_c2 = res->manhattanRatioBetween2Parcels(coord, coord2);
            (*ratio) += 2.0 * ratio_c1_goto_c2; // @see on pourrait faire un décalage de bit
        }
    }
    cout << "Le ratio de " << coord << " à la fin est " << (*ratio) << endl;
}

//@}

/// #########################
/// Autres méthodes utiles
/// #########################
//@{

void Resolution::createExample()
{
    // Surface de l'exemple :
    Field &example = field;
    example.set_width(20);
    example.set_height(20);
    example.resizeWithDimensions();

    example.add_in_out(9, 0);
    // Colonne Milieu
    example.add_road(9, 1);
    example.add_road(9, 2);
    example.add_road(9, 3);
    example.add_road(9, 4);
    example.add_road(9, 5);
    example.add_road(9, 6);
    example.add_road(9, 7);
    example.add_road(9, 8);
    example.add_road(9, 9);
    example.add_road(9, 10);
    example.add_road(9, 11);
    example.add_road(9, 12);
    example.add_road(9, 13);
    example.add_road(9, 14);
    example.add_road(9, 15);
    example.add_road(9, 16);
    example.add_road(9, 17);
    example.add_road(9, 18);
    // Première Ligne
    example.add_road(0, 2);
    example.add_road(1, 2);
    example.add_road(2, 2);
    example.add_road(3, 2);
    example.add_road(4, 2);
    example.add_road(5, 2);
    example.add_road(6, 2);
    example.add_road(7, 2);
    example.add_road(8, 2);
    // example.add_road(9, 2); déjà fait dans colonne milieu
    example.add_road(10, 2);
    example.add_road(11, 2);
    example.add_road(12, 2);
    example.add_road(13, 2);
    example.add_road(14, 2);
    example.add_road(15, 2);
    example.add_road(16, 2);
    example.add_road(17, 2);
    example.add_road(18, 2);
    example.add_road(19, 2);
    // Deuxième Ligne
    example.add_road(0, 7);
    example.add_road(1, 7);
    example.add_road(2, 7);
    example.add_road(3, 7);
    example.add_road(4, 7);
    example.add_road(5, 7);
    example.add_road(6, 7);
    example.add_road(7, 7);
    example.add_road(8, 7);
    // example.add_road(9, 7); déjà fait dans colonne milieu
    example.add_road(10, 7);
    example.add_road(11, 7);
    example.add_road(12, 7);
    example.add_road(13, 7);
    example.add_road(14, 7);
    example.add_road(15, 7);
    example.add_road(16, 7);
    example.add_road(17, 7);
    example.add_road(18, 7);
    example.add_road(19, 7);
    // Troisième Ligne
    example.add_road(0, 12);
    example.add_road(1, 12);
    example.add_road(2, 12);
    example.add_road(3, 12);
    example.add_road(4, 12);
    example.add_road(5, 12);
    example.add_road(6, 12);
    example.add_road(7, 12);
    example.add_road(8, 12);
    // example.add_road(9, 12); déjà fait dans colonne milieu
    example.add_road(10, 12);
    example.add_road(11, 12);
    example.add_road(12, 12);
    example.add_road(13, 12);
    example.add_road(14, 12);
    example.add_road(15, 12);
    example.add_road(16, 12);
    example.add_road(17, 12);
    example.add_road(18, 12);
    example.add_road(19, 12);
    // Quatrième Ligne
    example.add_road(0, 17);
    example.add_road(1, 17);
    example.add_road(2, 17);
    example.add_road(3, 17);
    example.add_road(4, 17);
    example.add_road(5, 17);
    example.add_road(6, 17);
    example.add_road(7, 17);
    example.add_road(8, 17);
    // example.add_road(9, 17); déjà fait dans colonne milieu
    example.add_road(10, 17);
    example.add_road(11, 17);
    example.add_road(12, 17);
    example.add_road(13, 17);
    example.add_road(14, 17);
    example.add_road(15, 17);
    example.add_road(16, 17);
    example.add_road(17, 17);
    example.add_road(18, 17);
    example.add_road(19, 17);

    example.add_in_out(9, 19);

    evaluateTotalUsable();

    // Paramètres
    params.set_road_width(1);
    params.set_serve_distance(2);

    // Définition des états des parcelles
    field.defineUsables(params.get_serve_distance());
}

//@}

