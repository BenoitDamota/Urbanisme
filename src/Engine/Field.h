#pragma once
#ifndef FIELD_H
#define FIELD_H

#include <list>
#include <vector>

#include "coordinates.h"

enum State{is_undefined= -1, is_road, is_usable, is_unusable};

class Field
{

private:
    unsigned nb_cols, nb_rows;
    std::list<Coordinates> ins_outs;

    std::vector<std::vector<State>> parcels;

public:
    Field(unsigned width, unsigned height/*, std::list< Coordinates >& inputs_and_ouputs = *(new std::list<Coordinates>()) */);
    ~Field();

/*=== Getters ===*/
    unsigned get_width() const	{ return nb_cols; }
    unsigned get_height() const	{ return nb_rows; }
    
    unsigned getNbParcels() const	{ return nb_cols * nb_rows; }
    
/*=== Setters ===*/
private:
    void resizeWithDimensions();
public:
    /**
	* Ajoute une entr�e/sortie � partir de coordonn�es
	* @param coord Coordonn�es de l'E/S
	*/
    void add_in_out(const Coordinates& coord)
    { ins_outs.push_back(coord); }
    /**
	* Ajoute une entr�e/sortie � partir de coordonn�es
	* @param col Colonne de l'E/S
	* @param row Ligne de l'E/S
	*/
    void add_in_out(unsigned col, unsigned row)
    { ins_outs.push_back(Coordinates(col, row)); }

    /**
	* Ajoute une route � partir de coordonn�es
	* @param col Colonne de la route
	* @param row Ligne de la route
	*/
    void add_road(unsigned col, unsigned row)
    { parcels[col][row]= is_road; }
    
/**=== Operators ===**/
public:
    State operator[](const Coordinates& pos) const { return parcels[pos.row][pos.col]; }

    std::vector<std::vector<State>>::const_iterator begin() const { return parcels.cbegin(); }
    std::vector<std::vector<State>>::const_iterator end() const { return parcels.cend(); }
    
    std::vector<std::vector<State>>::iterator begin() { return parcels.begin(); }
    std::vector<std::vector<State>>::iterator end() { return parcels.end(); }
    
    friend std::vector<std::vector<State>>::const_iterator begin(const Field& f) { return f.begin(); }
    friend std::vector<std::vector<State>>::const_iterator end(const Field& f) { return f.end(); }
    friend std::vector<State>::const_iterator& std::vector<State>::const_iterator::operator++();
    
public:
/* Affichage */
    void show_ins_and_outs() const;

/**=== Fonctions sur les coordonn�es ===**/
    /**
	* Effectue un test d'appartenance des coordonn�es � la matrice
	* @param x abscisse de la coordonn�e
	* @param y ordonn�e de la coordonn�e
	* @return true si la coordonn�e appartient � la matrice
	*/
    bool contains(int x, int y) const ;
    /**
	* Effectue un test d'appartenance des coordonn�es � la matrice
	* @param coord Coordonn�es de la position
	* @return true si la coordonn�e appartient � la matrice
	*/
    bool contains(const Coordinates& coord) const ;
    
/**=== M�thodes g�n�rales	===**/
    /** Cr�e al�atoirement des entr�es et sorties
	* @param nb nombre d'entr�es et sorties � g�n�rer
	*/
    void generateInsAndOuts(unsigned nb);
};

#endif //FIELD_H