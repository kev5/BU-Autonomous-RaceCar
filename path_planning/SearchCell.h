#pragma once 
#include <math.h>

#define WORLD_SIZE 64
struct SearchCell
{
public:
    int m_xcoord, m_ycoord;
    int m_id;
    SearchCell *parent;
    float G;
    float H;

    SearchCell() : parent(0) {}
    SearchCell(int x, int y, SearchCell *_parent = 0) : m_xcoord(x), m_ycoord(y), parent(_parent), m_id(z * WORLD_SIZE + z), G(0), H(0) {};

    float getF() { return G+H; }
    float ManHattanDistance(SearchCell *nodeEnd)
    {
        float x = (float) (fabs(this-> m_xcoord - nodeEnd->m_xcoord));
        float y = (float) (fabs(this-> m_ycoord - nodeEnd->m_ycoord));

        return x + y;
};


