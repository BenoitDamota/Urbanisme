﻿#include "stdafx.h"
#include "Resolution.h"

#include <cassert>
#include <map>

using namespace std;

Resolution::Resolution ( const Field& _field, const Parameters& _params ) :
    field ( _field ), params ( _params )
{
}

Resolution::~Resolution()
{
}

/// #########################
///      Getters
/// #########################

//@{
/// #########################
///      Setters
/// #########################
//@}

void Resolution::set_params ( const Parameters& _params )
{
    params= _params;
}

void Resolution::increment_road_distance ( const Coordinates& coord, unsigned int value )
{
    road_distances[coord.row][coord.col]+= value;
}

//@}
/// #########################
///     Calculs de données
/// #########################
//@{

// void Resolution::parcours_cellules()
// {
//     // Initialisation de la matrice selon la taille
//     road_distances.resize(field.get_height());
//     for (unsigned i= 0; i < field.get_height(); ++i) {
// 	vector<unsigned> row= road_distances[i];
// 	road_distances[i].resize(field.get_width(), 0); // Initialisation
//     }
//     // Calculs des distances
//     Coordinates coord1(0,0);
//     while(nextCoordinates(&coord1)){
// 	// On calcule et additionne la distance pour aller vers chacun des successeurs
// 	Coordinates coord2(coord1);
// 	while(nextCoordinates(&coord2)){
// // 	    unsigned dist= coord1.manhattanDistance(coord2);
// 	}
//     }
//
//     road_distances_are_initiated= true;
// }

//@}
/// ###############################
///	Fonctions auxiliaires
/// ###############################
//@{
std::list<Coordinates>* Resolution::getServingRoads ( const Coordinates& coord ) const
{
    list<Coordinates>* serving_roads= new list<Coordinates>;

    int serve_dist= params.get_serve_distance(); // on pourrait utiliser unsigned

    // On vérifie si les routes entre (x +dist;y +dist) et (x -dist;y -dist)
    // @SEE on vérifie serve_dist² parcelles,  alors qu'on pourrait en vérifier ?? (moins)
    for ( int i= coord.row + serve_dist; i >= coord.row -serve_dist; --i ) {
        for ( int j= coord.col + serve_dist; j >= coord.col -serve_dist; --j ) {

            // On vérifie que la parcelle n'est pas en dehors de la matrice et qu'elle n'est pas la coordonnée courante
            Coordinates neighbour ( j,  i );
            if ( field.contains ( neighbour ) && ! ( neighbour == coord )
                    && coord.manhattanDistance ( neighbour ) <= 2 // TODO changer, ne pas utiliser manhattanDistance,  peu performant ?
                    && field[neighbour] == is_road ) {
                // Ajout dans les routes voisines de la parcelle
                Coordinates& road_coord= * ( new Coordinates ( j, i ) );
                serving_roads->push_back ( road_coord );
#if DEBUG_ROADS_DIST
                cout << "parcelle en "<< j<< " ; "<< i<< " est une route voisine de la parcelle en "
                     << coord.col<< " ; "<< coord.row<< endl;
#endif
            }
        }
    }

    return serving_roads;
}

std::list<Coordinates>* Resolution::getNeighbourRoads ( const Coordinates& coord ) const
{
#if DEBUG_ROADS_DIST
    cout << "Recherche des voisins de la parcelle en "<< coord.col<< " ; "<< coord.row<< endl;
#endif
    list<Coordinates>* neighbour_roads= new list<Coordinates>;

    Coordinates& west= * ( new Coordinates ( coord.col -1, coord.row ) );
    Coordinates& east= * ( new Coordinates ( coord.col +1, coord.row ) );
    Coordinates& north= * ( new Coordinates ( coord.col, coord.row -1 ) );
    Coordinates& south= * ( new Coordinates ( coord.col, coord.row +1 ) );
// On vérifie que chaque voisin n'est pas en dehors de la matrice

    if ( field.contains ( west ) && field[west] == is_road ) {
        // Ajout dans les routes voisines de la parcelle
        neighbour_roads->push_back ( west );
#if DEBUG_ROADS_DIST
        cout << "\tparcelle "<< west<< endl;
#endif
    }
    if ( field.contains ( east ) && field[east] == is_road ) {
        // Ajout dans les routes voisines de la parcelle
        neighbour_roads->push_back ( east );
#if DEBUG_ROADS_DIST
        cout << "\tparcelle "<< east<< endl;
#endif
    }
    if ( field.contains ( north ) && field[north] == is_road ) {
        // Ajout dans les routes voisines de la parcelle
        neighbour_roads->push_back ( north );
#if DEBUG_ROADS_DIST
        cout << "\tparcelle "<< north<< endl;
#endif
    }
    if ( field.contains ( south ) && field[south] == is_road ) {
        // Ajout dans les routes voisines de la parcelle
        neighbour_roads->push_back ( south );
#if DEBUG_ROADS_DIST
        cout << "\tparcelle "<< south<< endl;
#endif
    }

    return neighbour_roads;
}

unsigned Resolution::calcRoadDistance ( const Coordinates &coord1, const Coordinates &coord2 ) const
{
    if ( coord1 == coord2 ) {
#if DEBUG_ROADS_DIST
        clog<< "les cases de départ et d'arrivée sont identiques"<< endl;
#endif
        return 0;
    }

    unsigned min_dist= UNSIGNED_INFINITY;
#if DEBUG_ROADS_DIST
    Coordinates min_dist_neighbour_c1 ( 0,0 );
    Coordinates min_dist_neighbour_c2 ( 0,0 );
#endif

    cout << "Premier point : "<< coord1<< endl;
    list<Coordinates>* serving_roads_c1 = getServingRoads ( coord1 );

    cout << endl<< "Deuxième point : "<< coord2<< endl;
    list<Coordinates>* serving_roads_c2 = getServingRoads ( coord2 );
    for ( Coordinates road_c1 : *serving_roads_c1 ) {
        for ( Coordinates road_c2 : *serving_roads_c2 ) {
            unsigned int dist = recCalcRoadDistance ( road_c1, road_c2, new list<Coordinates> );
            if ( dist < min_dist ) {
                min_dist= dist +1;
#if DEBUG_ROADS_DIST
                min_dist_neighbour_c1= road_c1;
                min_dist_neighbour_c2= road_c2;
#endif
            }
        }
    }

    if ( min_dist ) {
#if DEBUG_ROADS_DIST
        cout << "Les plus court chemin de "<< coord1<< " à "<< coord2<< " est de longueur "<< min_dist<<
// 		" et passe par "<< min_dist_neighbour_c1<< " et "<<
// 		min_dist_neighbour_c2<<
             endl;
#endif
    } else {
        cerr<< "Impossible de relier les deux parcelle par les routes"
            "(au moins une des 2 parcelle n'a pas de route à proximité (distance < "<<params.get_serve_distance() << "))"<< endl;
    }

    return min_dist;
}
unsigned Resolution::recCalcRoadDistance ( const Coordinates& coord1, const Coordinates& coord2, list<Coordinates>* visited ) const
{
#if DEBUG_ROADS_DIST
    cout << "Calcul de la distance entre "<< coord1<< " et "<< coord2<< endl;
#endif
    assert ( field[coord1] == is_road );

    if ( coord1 == coord2 ) {
        return 0;
    } else {
        unsigned min_dist= UNSIGNED_INFINITY;
//         Coordinates& min_dist_neighbour;

        list<Coordinates>* neighbour_roads = getNeighbourRoads ( coord1 );
        for ( Coordinates new_coord : *neighbour_roads ) {
#if DEBUG_ROADS_DIST
            cout << "Taille de la liste des visitées : "<< visited->size() << endl;
#endif
            //  recherche de cet élément dans la liste des visités
            if ( find ( visited->begin(), visited->end(), new_coord ) == visited->end() ) {
                // On applique la recursivité avec la coordonnée envisagée courante
                visited->push_back ( coord1 );
                unsigned dist = recCalcRoadDistance ( new_coord,  coord2, visited );
                visited->pop_back();

                // On compare le résultat obtenu avec le minimum courant
                if ( dist < min_dist ) {
                    min_dist= dist +1;
//                     min_dist_neighbour= new_coord;
                }
            } else {
#if DEBUG_ROADS_DIST
                cout << "La parcelle a déjà été visitée" << endl;
#endif
            }
        }

#if DEBUG_ROADS_DIST
//         if (min_dist_neighbour == NULL) {
//             clog << "Impossible d'aller de "<< coord1<< " à "<< coord2<< endl;
//             assert(min_dist == UNSIGNED_INFINITY);
//         }
//         else {
//             clog << "On passe par "<< min_dist_neighbour<< "pour aller de "<< coord1<< " à "<< coord2<< endl;
//         }

        cout << "Pour aller de "<< coord1<< " à "<< coord2<< ", il y a un trajet de longueur "<< min_dist
// 		", on est passé par "<< (*min_dist_neighbour)
             << endl;

#endif

        return min_dist;
    }
}


//@}
/// #########################
///      Evaluations
/// #########################
//@{

unsigned int Resolution::evaluateTotalUsable() const
{
    unsigned nb_usables= 0;
    for ( vector<State> row_parcel_state : field ) {
        for ( State parcel_state : row_parcel_state ) {
            cout << "parcel_state = "<< parcel_state<<endl;
            assert ( parcel_state >= -1 &&  parcel_state<= is_unusable );
            if ( parcel_state == is_usable ) {
                ++nb_usables;
            }
        }
    }

#if DEBUG_EVALUATION
    cout << "Total number of usables parcels : "<< nb_usables<< endl;
#endif
    assert ( nb_usables >=0 && nb_usables < field.getNbParcels() && "nombre d'exploitables incohérent" );

    return nb_usables;
}

float Resolution::manhattanRatioBetween2Parcels ( const Coordinates& p1, const Coordinates& p2 ) const
{
    unsigned road_distance = calcRoadDistance ( p1, p2 );
    unsigned manhattan_distance = p1.manhattanDistance ( p2 );
    float ratio = road_distance/ ( float ) manhattan_distance;

    return ratio;
}

float Resolution::evaluateRatio() const
{
    float total_ratio= 0.0;
    return total_ratio;
}

//@}
/// #########################
/// Autres méthodes utiles
/// #########################
//@{

bool Resolution::nextCoordinates ( Coordinates* coord )
{
    // On vérifie que la coordonnée actuelle est dans la surface
    //  (vérification sur les colonnes puis sur les lignes)
    if ( ( coord->col < 0 || coord->col >= ( int ) ( field.get_width() ) ) ||
            ( coord->row < 0 || coord->row >= ( int ) ( field.get_height() ) ) ) {
#if DEBUG_PARCOURS_COORDS
        cout << "Passage à la ligne suivante" <<endl;
#endif
        return false;
    } else {
        // Si on est à la fin de la ligne, on passe à la ligne suivante
        if ( ( unsigned ) ( coord->col ) +1 == field.get_width() ) {
            // seulement si on n'est pas à la fin de la surface
            if ( ( unsigned ) ( coord->row ) +1 < field.get_width() ) {
                coord->col= 0;
                coord->row+= 1;
#if DEBUG_PARCOURS_COORDS
                cout << "Passage àla ligne suivante" <<endl;
#endif
                return true;
            } else {
                // dernière case de la surface
#if DEBUG_PARCOURS_COORDS
                cout << "Dernière case de la matrice" <<endl;
#endif
                return false;
            }
        }// Sinon, on se décale sur la ligne
        else {
            coord->col+= 1;
#if DEBUG_PARCOURS_COORDS
            cout << "Passage à la case suivante (à droite)" <<endl;
#endif
            return true;
        }
    }
}

//@}
