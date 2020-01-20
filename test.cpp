

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, const char *argv[])
{
    vector<int> v;
    std::vector<int>::iterator it;

    v.push_back(1);
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(3);

    it = find(v.begin(), v.end(), 1);
    if (it != v.end()) {
        cout << "find: " << *it << endl;
    }

    //v.clear();

    for (auto iter = v.begin(); iter != v.end(); iter++)
    {
        cout << "iter:" << *iter << endl;
    }

    return 0;
}
