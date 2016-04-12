#pragma once
#include "Engine/Field.h"
#include "Engine/Parameters.h"
#include "Engine/coordinates.h"

class Resolution
{
private:
    // Données sur lesquelles on travaille
	Field field;
	Parameters params;

    // Données calculées
	std::vector<std::vector<unsigned>> road_distances;
	
    // FLAGS
	bool road_distances_are_initiated= false;

public:
	Resolution(const Field & _field, const Parameters & _params);
	~Resolution();
	
/* Getters */
	
/* Setters */
	/**
	 * @brief set_params
	 */
	void set_params(const Parameters& _params);
	/**
	 * @brief set_road_distance
	 * @deprecated TODO
	 */
	void increment_road_distance(const Coordinates& coord, unsigned value);
    
/* Calculs de données */
	/**
	 * Initialise la matrice de disance entre les points selon leur distance avec les routes
	 *  avec la distance Manhattan
	 */
	void initManhattanDistancesMatrice();
	
/* Evaluations */
	/**
	 * Compte le nombre total de parcelles exploitables sur la surface
	 * @return nombre de parcelles exploitables
	 */
	unsigned evaluateTotalUsable() const;
	
	/**
	 * Calcule le ratio entre distance par la route et distance Manhattan entre deux positions
	 * @param p1 premières coordonnées
	 * @param p2 secondes coordonnées
	 * @return le ratio, sous forme de flottant
	 */
	float manhattanRatioBetween2Parcels(const Coordinates& p1, const Coordinates& p2) const;
	/**
	 * Evalue le ratio de toutes les solutions
	 * @return l'évaluation en flottant
	 */
	float evaluateRatio() const;
	
/* Autres méthodes utiles */
	/**
	 * Modifie la coordonnée en entrée pour obtenir celle qu'elle précède
	 * Agit comme une itération sur l'ensemble des coordonnées de la surface
	 * @param coord Coordonnée qui va être modifiée, non modifiée si il n'y a pas de suivante
	 * @return vrai si il y a une coordonnées, faux si on est à la fin ou en dehors de la surface
	 */
	bool nextCoordinates(Coordinates* coord);

	
};
	

