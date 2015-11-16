
/***************************************************************************
 *   Copyright (C) 2014, 2015 Jan Fostier (jan.fostier@intec.ugent.be)     *
 *   Copyright (C) 2014, 2015 Mahdi Heydari (mahdi.heydari@intec.ugent.be) *
 *   This file is part of Brownie                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "string.h"
#include <iostream>
#include "graph.h"
#include <list>
using namespace std;

class Dijkstra {
private:
        typedef pair< double,NodeID > element ;
        struct comparator {
                bool operator()(const element& f, const element& s)
                {
                        return f.second >= s.second;
                }
        };
        vector<element> checkedList;
        set<NodeID> visited;
        DBGraph dbg;
        double getDistance(NodeID id );
        void updateChecklist(NodeID nodeID, double newDistance);
public:

        Dijkstra(const DBGraph &g);
        double shortestPath(SSNode start, SSNode end);
};










