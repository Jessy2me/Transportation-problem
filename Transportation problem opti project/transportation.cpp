#include <bits/stdc++.h>
using namespace std;
#define int long long

vector<int> supply, demand; // Supply and demand vector

vector<vector<double>> c; // this is a cost matrix
// i named it c so that we can see c[i][j] better.

class Goods
{
public:
    double quantity, cost;
    // here quantity is the amount x[i][j] as per formulation
    // here cost is per unit cost for this good to be transported
    int row, col;
    Goods()
    {
        // default constructor
        quantity = 0;
        cost = 0;
        row = -1;
        col = -1;
    }
    Goods(double quantity, double cost, int row, int col)
    {
        // constructor with given values
        this->quantity = quantity;
        this->cost = cost;
        this->row = row;
        this->col = col;
    }

    // operator overloading
    friend bool operator==(const Goods &a, const Goods &b)
    {
        return a.quantity == b.quantity && a.cost == b.cost && a.row == b.row && a.col == b.col;
    }
    friend bool operator!=(const Goods &a, const Goods &b)
    {
        return !(a == b);
    }
    static Goods ZERO;
};

Goods Goods::ZERO = {}; // unallocated goods cell i.e., x[i][j]=0;

vector<vector<Goods>> x;
// this is the goods matrix that we are transporting
// i named it x so that we can see x[i][j] better.

void NWCR(); // function declaration

void printAnswer()
{
    cout << "Final Optimal answer \n\n";
    double ans = 0.0;
    for (int i = 0; i < supply.size(); i++)
    {
        for (int j = 0; j < demand.size(); j++)
        {
            auto gd = x[i][j];
            if (gd != Goods::ZERO && gd.row == i && gd.col == j)
            {
                cout << ' ' << setw(3) << gd.quantity << ' ';
                ans += gd.quantity * gd.cost;
            }
            else
            {
                cout << "  -  "; // unallocated cells
            }
        }
        cout << '\n';
    }
    cout << "\nTotal transportation cost: " << ans << "\n\n";
}

void steppingStone(); // function declaration

int32_t main()
{
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    int n; // num of sources.
    int m; // num of destinations.
    cin >> n >> m;

    // Taking input of supply and demand
    for (int i = 0; i < n; i++)
    {
        int val;
        cin >> val;
        supply.push_back(val);
    }
    for (int i = 0; i < m; i++)
    {
        int val;
        cin >> val;
        demand.push_back(val);
    }

    // Check for imbalance
    int sumSupply = 0, sumDemand = 0;
    for (int i = 0; i < n; i++)
    {
        sumSupply += supply[i];
    }
    for (int i = 0; i < m; i++)
    {
        sumDemand += demand[i];
    }

    // fixing imbalance if any
    if (sumSupply > sumDemand)
    {
        demand.push_back(sumSupply - sumDemand);
    }
    else if (sumDemand > sumSupply)
    {
        supply.push_back(sumDemand - sumSupply);
    }

    c.clear();
    x.clear();

    // taking input of cost matrix
    for (int i = 0; i < n; i++)
    {
        int sz = max(n, (int)demand.size());
        vector<double> temp(sz);
        vector<Goods> mt(sz);
        for (int j = 0; j < m; j++)
        {
            cin >> temp[j];
        }
        c.push_back(temp);
        x.push_back(mt); // pushing cells with value 0;
    }
    for (int i = n; i < supply.size(); i++)
    {
        // introducing dummy row/col
        int sz = max(m, (int)demand.size());
        vector<double> temp(sz);
        vector<Goods> mt(sz);
        c.push_back(temp);
        x.push_back(mt);
    }

    NWCR();          // NorthWest corner Rule
    steppingStone(); // Stepping Stone method
    printAnswer();   // Printing final answer
    return 0;
}

void NWCR()
{
    int cur = 0; // currently working cell
    for (int i = 0; i < supply.size(); i++)
    {
        for (int j = 0; j < demand.size(); j++)
        {
            int amount = min(supply[i], demand[j]);
            if (amount > 0)
            {
                x[i][j] = Goods(amount, c[i][j], i, j); // allocating goods value
                supply[i] -= amount;
                demand[j] -= amount;
                if (supply[i] == 0)
                {
                    // when supply is over
                    cur = j;
                    break;
                }
            }
        }
    }
}

vector<Goods> allGoods() // returns vector of all allocated cells
{
    vector<Goods> res;
    for (auto &line : x)
    {
        for (auto &g : line)
        {
            if (g != Goods::ZERO) // adding all non zero cells
            {
                res.push_back(g);
            }
        }
    }
    return res;
}

vector<Goods> getnbrs(const Goods &gd, const vector<Goods> &cells) // returns vertical and horizontal neighbors
{
    vector<Goods> nbrs(2);
    for (auto &it : cells)
    {
        if (it != gd)
        {
            if (it.row == gd.row && nbrs[0] == Goods::ZERO) // horizontal neighbor
            {
                nbrs[0] = it;
            }
            else if (it.col == gd.col && nbrs[1] == Goods::ZERO) // vertical neighbor
            {
                nbrs[1] = it;
            }
            if (nbrs[0] != Goods::ZERO && nbrs[1] != Goods::ZERO)
            {
                break;
            }
        }
    }
    return nbrs;
}

vector<Goods> getLoop(const Goods &g)
{
    vector<Goods> allCells = allGoods();  // all allocated cells
    allCells.insert(allCells.begin(), g); // introducing this new cell

    // now we keep removing elements that do not have a vertical and horizontal neighbor.

    int iSz; // allAllocated cells size initially
    do
    {
        iSz = allCells.size();
        // removing procedure
        allCells.erase(remove_if(allCells.begin(), allCells.end(), [&allCells](Goods &gd)
                                 {
                    auto nbrs = getnbrs(gd, allCells);
                    return nbrs[0] == Goods::ZERO || nbrs[1] == Goods::ZERO; }),
                       allCells.end());
    } while (iSz != allCells.size());

    // place the remaining elements in order of loop
    vector<Goods> stones(allCells.size());
    fill(stones.begin(), stones.end(), Goods::ZERO);
    auto prev = g;
    for (size_t i = 0; i < stones.size(); i++)
    {
        stones[i] = prev;
        prev = getnbrs(prev, allCells)[i % 2];
    }
    return stones; // vector containing cells in loop order
}

void handleDegeneracy()
{
    double eps = DBL_MIN; // adding a very small number
    if (supply.size() + demand.size() - 1 != allGoods().size())
    {
        for (int i = 0; i < supply.size(); i++)
        {
            for (int j = 0; j < demand.size(); j++)
            {
                if (x[i][j] == Goods::ZERO)
                {
                    Goods temp(eps, c[i][j], i, j);
                    // if no loop can be generated from that cell we'll include it
                    if (getLoop(temp).empty())
                    {
                        x[i][j] = temp;
                        return;
                    }
                }
            }
        }
    }
}

// int counts = 1;

void steppingStone()
{
    vector<Goods> loop; // vector in which we'll make +/- changes
    Goods leave;        // leaving cell
    bool mark = false;  // whether to do any optimization or not
    double maxChange = 0;

    // first check for degeneracy
    handleDegeneracy();

    for (int i = 0; i < supply.size(); i++)
    {
        for (int j = 0; j < demand.size(); j++)
        {
            if (x[i][j] != Goods::ZERO)
            {
                continue;
            }
            Goods myCell(0, c[i][j], i, j);
            vector<Goods> Lp = getLoop(myCell);
            double change = 0; // how much change will it make by introducing myCell
            double mini = INT_MAX;
            Goods leavingCell;

            bool flag = true; // for alternate plus minus
            for (auto &gd : Lp)
            {
                if (flag)
                {
                    change += gd.cost;
                    flag = false;
                }
                else
                {
                    change -= gd.cost;
                    if (gd.quantity < mini)
                    {
                        leavingCell = gd;
                        mini = gd.quantity;
                    }
                    flag = true;
                }
            }
            if (change < maxChange) // optimize the most negative first
            {
                mark = true;
                loop = Lp;
                leave = leavingCell;
                maxChange = change;
            }
        }
    }
    if (mark) // making changes in loop
    {
        double amount = leave.quantity;
        bool flag = true; // for alternate plus minus
        for (auto &gd : loop)
        {
            gd.quantity += flag ? amount : -amount;
            x[gd.row][gd.col] = gd.quantity == 0 ? Goods::ZERO : gd;
            flag = !flag;
        }
        // cout << "interation number is " << counts << " :" << endl;
        // printAnswer();
        // counts++;
        steppingStone(); // call it recursively to optimize again
    }
}