
#include <iostream>
#include <vector>
#include "simulator/Simulator.h"

using namespace std;

int main() {
    int capacity, n;

    // Read cache capacity and number of requests
    if (!(cin >> capacity >> n)) {
        cerr << "Invalid input";
        return 1;
    }

    if (capacity <= 0 || n < 0) {
        cerr << "Invalid capacity or request count";
        return 1;
    }

    // Read the request sequence
    vector<int> requests(n);

    for (int i = 0; i < n; i++) {
        if (!(cin >> requests[i]) || requests[i] < 0) {
            cerr << "Invalid request";
            return 1;
        }
    }

    // Run all four cache replacement algorithms
    Simulator sim;
    vector<PolicyResult> results = sim.compare(capacity, requests);

    // Return results in JSON format
    cout << "{\"results\":[";

    for (int i = 0; i < results.size(); i++) {
        const PolicyResult& r = results[i];

        if (i > 0) cout << ",";

        cout << "{\"name\":\"" << r.name
             << "\",\"hits\":" << r.hits
             << ",\"misses\":" << r.misses
             << ",\"removals\":" << r.removals
             << ",\"hitRatio\":" << r.hitRatio
             << ",\"missRatio\":" << r.missRatio
             << "}";
    }

    cout << "]}" << endl;

    return 0;
}