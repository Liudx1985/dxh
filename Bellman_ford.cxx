#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <set>
#include <queue>
#include <unordered_map>
#include <Windows.h>

//
namespace std
{
template<class P1, class P2>
struct less<std::pair<P1, P2> >
    : public binary_function<std::pair<P1, P2>, std::pair<P1, P2>, bool>
{
    // functor for operator<
    typedef std::pair<P1, P2> _Ty;
    bool operator()(const _Ty& _Left, const _Ty& _Right) const
    {
        // apply operator< to operands
        return (_Left.first < _Right.first);
    }
};

}

using namespace std;
class topo_graph // 链式前向星-邻接表
{
    /*
    head[idx|val]  | edge-list[idx|{u,v,w,nxt}]
    0| ]    0|{..}->{}->{x, v, w, -1};
    ...   |  ...
    i|x] /->x|{i,j,1,y}; //!!!!recode all edge from node 2, the head save the first edge index in edge-list, the edge list save list by next segment.
    ...   | ...
    ?|?]    y|{i,k, w, -1}; // x pos
    ...   | ...
    */
    enum {INF = 0xFFFFFFF};
    // edge
    typedef struct topo_edge
    {
        int u, // start
            v, // end
            w; // weight
        int next; // next edge
        topo_edge(int u_, int v_, int w_ = 1, int next_ = -1)
            : u(u_), v(v_), w(w_), next(next_)
        {
        }

    } edge;

    std::vector<int> _head;
    std::vector<edge*> _edge_list;
public:
    virtual ~topo_graph()
    {
        for (std::vector<edge*>::iterator itV = _edge_list.begin();
                itV != _edge_list.end(); ++itV)
        {
            delete *itV;
        }
    }
    // add a edge.
    int add_edge(int u, int v, int w = 1)
    {
        if (u < 0 || v < 0 || u == v || w < 0)
        {
            return -1;
        }
        int z = _head.size();
        edge *p = new edge(u, v, w);
        if (u >= z || v >= z)
        {
            // resize  to offset
            _head.resize(max(u, v) + 1, -1);
        }
        _edge_list.push_back(p);
        if (_head[u] >= 0) // replace. & insert.
        {
            _edge_list.back()->next = _head[u];
        }
        return _head[u] = _edge_list.size() - 1;
    }

    edge *get_edge(int e) const
    {
        if (e < 0 || e >= _edge_list.size())
        {
            return 0;
        }
        return _edge_list[e];
    }

    // get min cost
    int cost(int u, int v) const
    {
        if (u < 0 || u >= _edge_list.size())
        {
            return INF;
        }
        // remove
        int c = INF;
        for (int e = _head[u]; e >= 0; e = _edge_list[e]->next)
        {
            int w = _edge_list[e]->w;
            if (_edge_list[e]->v == v)   // found
            {
                c = min(c, w);
            }
        }
        return c;
    }

    void remove_node(int n)
    {
        // un-implemented.
    }

    void remove_edge(int e)
    {
        edge *pe = get_edge(e);
        if (0 == pe)
        {
            return;
        }
        // get the head of edge-list from u;
        edge *prev = _edge_list[_head[pe->u]];
        if (prev == pe) // remove head.
        {
            _head[pe->u] = prev->next;
        }
        else // loop to tail.
        {
            while (true)
            {
                edge *cur = _edge_list[prev->next];
                if (cur == pe)
                {
                    prev->next = cur->next;
                    break;
                }
                prev = cur;
            }
        }
        // keep the pos.because we need to keep the index
        delete _edge_list[e];
        _edge_list[e] = 0;
    }

    // dijkstra heap algo.
    void dijkstra(int s, std::vector<int> &d)
    {
        std::priority_queue<std::pair<int, int> > q;
        d.resize(_head.size(), INF);
        q.push(std::make_pair(d[s] = 0, s));
        while (!q.empty())
        {
            int u = q.top().second;
            q.pop();
            for (int e = _head[u]; e >= 0; e = _edge_list[e]->next)
            {
                int v = _edge_list[e]->v;
                int w = _edge_list[e]->w;
                if(d[u] + w < d[v])
                {
                    d[v] = d[u] + w;
                    //path[v] = u;
                    q.push(std::make_pair(d[v], v));
                }
            }
        }
    }

    // Bellman Ford algo.
    bool bellman_ford(int s, std::vector<int> &d)
    {
        d.resize(_head.size(), INF);
        d[s] = 0;

        for (int k = 0; k < _head.size(); ++k) // for each node
        {
            if (_head[k] < 0)
            {
                continue;
            }
            // For each edge u → v of cost c: dv = min (dv, du + c)
            for (int e = _head[k]; e >= 0; e = _edge_list[e]->next)
            {
                int u = _edge_list[e]->u;
                int v = _edge_list[e]->v;
                int w = _edge_list[e]->w;
                if (d[u] + w <  d[v]) // notice the intger overflow{MAX_INT + 1 < 0 !!}
                {
                    d[v] = d[u] + w;
                }
            }
        }
        // test negative circle.
        // for-each u->v if d[v] > d[u] + cost(u->v)
        return true;

        for (int k = 1; k < _head.size(); ++k) // for each node
        {
            // For each edge u → v of cost c: dv = min (dv, du + c)
            for (int e = _head[k]; e >= 0; e = _edge_list[e]->next)
            {
                int u = _edge_list[e]->v;
                int v = _edge_list[e]->v;
                int w = _edge_list[e]->w;
                if (d[v] > d[u] + w)
                {
                    return false;
                }
            }
        }
        return true;
    }

    /* SPFA(Shortest Path Faster Algorithm)是Bellman-Ford算法的一种队列实现
    */
    bool spfa(int s, std::vector<int> &d)
    {
        int n = _head.size();
        std::priority_queue<std::pair<int, int> > q; // first is d[v], second is v.
        std::vector<bool> in_queue; // in_queue[i]表示结点i是否在队列中，初始时只有s在队列中；
        std::vector<int> visitCount;

        d.resize(n, INF);
        in_queue.resize(n, 0);
        visitCount.resize(n, 0);

        q.push(std::make_pair(d[s] = 0, s));
        in_queue[s] = true;

        while (!q.empty())
        {
            int u = q.top().second;
            q.pop();
            in_queue[u] = false;
            if (visitCount[u] ++ > n)
            {
                return false; // 存在负权圈
            }

            for (int e = _head[u]; e >= 0; e = _edge_list[e]->next)
            {
                int v = _edge_list[e]->v;
                int w = _edge_list[e]->w;
                if(d[u] + w < d[v])   // relax op
                {
                    d[v] = d[u] + w;
                    if (!in_queue[v])
                    {
                        in_queue[v] = true;
                        q.push(std::make_pair(d[v], v));
                    }
                }
            }
        }
        return true;
    }

    void get_a_path(const std::vector<int> &d, int src, int dst, std::vector<int> &route)
    {
        int n = _head.size();
        if (src < 0 || dst < 0 || src >= n || dst >= n)
        {
            return;
        }
        route.push_back(dst);
        while (src != route.front())
        {
            for (int i = 0; i < _edge_list.size(); ++i)
            {
                int u = _edge_list[i]->u;
                int v = _edge_list[i]->v;
                int w = _edge_list[i]->w;
                if (v == route.front() && d[u] + w == d[v])
                {
                    route.insert(route.begin(), u);
                    break;
                }
            }
        }
    }

    // call f when find the final route.
    void walk_all(const std::vector<int> &t, int src, int dst,
                  std::list<std::vector<int> >  &routes)
    {
        int n = _head.size();
        if (src < 0 || dst < 0 || src >= n || dst >= n)
        {
            return;
        }
        routes.clear();
        std::vector<int> route;
        route.push_back(dst);

        routes.push_back(route);

        // GET all shortest path backtrace.
        for (std::list<std::vector<int> >::iterator it = routes.begin();
                it != routes.end();)
        {
            std::vector<int> &route = *it;
            int head = route.front();
            if (head == src)
            {
                ++it;
                continue;
            }

            for (int i = 0; i < n; ++i)
            {
                //GET u| d(u) + w(u, v) = d(v)
                if (t[i] + cost(i, head) == t[head])
                {
                    std::vector<int> nx = route; // copy & inset. spawn
                    nx.insert(nx.begin(), i);
                    routes.push_back(nx);
                }
            }
            it = routes.erase(it);
        }
    }

};

int main()
{
    topo_graph g;
    /*
    0  1  2  3  4
    5  6  7  8  9
    10 11 12 13 14
    15 16 17 18 19
    20 21 22 23 24
    */
    const int n = 1000;
    for (int r = 0; r < n; ++r)
    {
        for (int c = 0; c < n; ++c)
        {
            if (c < n - 1)
            {
                g.add_edge(n * r + c, n * r + c + 1);
            }
            if (r < n - 1)
            {
                g.add_edge(n * r + c, n * (r + 1) + c);
            }
        }
    }
    std::vector<int> d;
    __int64 begin = GetTickCount();
    //g.dijkstra(0, d);
    //g.bellman_ford(0, d);
    g.spfa(0, d);
    cout << (GetTickCount() - begin) << "ms\n";

// 	for (int r = 0; r < n; ++r) {
// 		for (int c = 0; c < n; ++c) {
// 			cout << d[n * r + c] << "\t";
// 		}
// 		cout << "\n";
// 	}

    //--- walk-- ALL
// 	std::list<std::vector<int> >  routes;
// 	g.walk_all(d, 0, 25, routes);
// 	for (std::list<std::vector<int> >::iterator i = routes.begin();
// 		i != routes.end(); ++ i) {
// 			for (std::vector<int>::iterator r = (*i).begin();
// 				r != (*i).end(); ++r)
// 			{
// 				cout << *r << "\t";
// 			}
// 			cout << "\n";
// 	}
    std::vector<int> route;

    begin = GetTickCount();
    g.get_a_path(d, 0, 1017, route);
    cout << (GetTickCount() - begin) << "ms\n";
    for (std::vector<int>::iterator r = (route).begin();
            r != (route).end(); ++r)
    {
        cout << *r << "\t";
    }
    cout << "\n";
    return 0;
}
