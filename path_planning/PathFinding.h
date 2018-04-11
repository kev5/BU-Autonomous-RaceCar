#pragma once
#include <vector>

class PathFinding
{
public:
    PathFinding(void);
    ~PathFinding(void);

    void FindPath(Vector3 currentPos, Vector3 targetPos);
    Vector3 NextPathPos();
    void ClearOpenList() { m_openList.clear(); }
    void ClearVisitedList() {m_visitedList.clear(); }
    void ClearPathToGoal() { m_pathToGoal.clear(); }
    bool m_initializedStartGoal;
    bool m_foundGoal;

private:
    void SetStartAndGoal (SearchCell start, SearchCell goal);
    void PathOpened(int x, int y, float newCost, SearchCell *parent);
    SearchCell *GetNextCell();
    void ContinuePath();

    SearchCell *m_startCell;
    SearchCell *m_goalCell;
    vector<SearchCell*> m_openList;
    vector<SearchCell*> m_visitedList;
    vector<Vector3*> m_pathToGoal;

};



