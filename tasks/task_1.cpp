#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector> 

using namespace std;

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses,
};

struct Query {
    QueryType type;
    string bus;
    string stop;
    vector<string> stops;
};

istream& operator>>(istream& is, Query& q) {
    string operation_code;
    cin >> operation_code;

    if (operation_code == "NEW_BUS"s) { // NEW_BUS bus stop_count stop1 stop2
        q.type = QueryType::NewBus;
        q.stops.clear(); // ������� ����������� ������ ��������� ��� �������� ��������
        q.bus.clear(); // ������� ������� �������
        is >> q.bus;
        int stop_count{};
        is >> stop_count;
        for (size_t i = 0; i < stop_count; i++) {
            string stop;
            is >> stop;
            q.stops.push_back(stop);
        }
    }
    else if (operation_code == "BUSES_FOR_STOP"s) {
        q.type = QueryType::BusesForStop;
        is >> q.stop;
    }
    else if (operation_code == "STOPS_FOR_BUS"s) {
        q.type = QueryType::StopsForBus;
        is >> q.bus;
    }
    else if (operation_code == "ALL_BUSES"s) {
        q.type = QueryType::AllBuses;
    }
    return is;
}

struct BusesForStopResponse {
    string stop; // �������� ������� ���������
    map<string, vector<string>> stop_and_buses; // <���������, ��������>
};

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {
    if (r.stop_and_buses.count(r.stop) == 0) { // �� ����, ����� ��������� ������������
        os << "No stop"s;
        return os;
    }
    bool isFirst = true;
    for (const auto& bus : r.stop_and_buses.at(r.stop)) {
        if (isFirst) {
            os << bus;
            isFirst = false;
        }
        else os << " "s << bus;
    }
    return os;
}

struct StopsForBusResponse {
    string bus; // �������� �������� 
    map<string, vector<string>> bus_and_stops; // <�������, ���������>
    map<string, vector<string>> stop_and_buses; // <���������, ��������>
};

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {
    if (r.bus_and_stops.count(r.bus) == 0) { // �� ����, ����� ��������� (���������) ������������
        os << "No bus"s;
        return os;
    }
    size_t count{}; // ������� ���-�� ��������
    for (const auto& stop : r.bus_and_stops.at(r.bus)) {
        ++count;
        os << "Stop "s << stop << ": "s;
        if (r.stop_and_buses.at(stop).size() == 1 and std::count(r.stop_and_buses.at(stop).begin(), r.stop_and_buses.at(stop).end(), r.bus)) {
            os << "no interchange"s;
        }
        bool isFirst = true;
        for (const auto& bus : r.stop_and_buses.at(stop)) {
            if (bus == r.bus) {
                continue;
            }
            if (isFirst) {
                os << bus;
                isFirst = false;
            }
            else os << " "s << bus;
        }
        if (count != r.bus_and_stops.at(r.bus).size()) os << endl;
    }
    return os;
}

struct AllBusesResponse {
    map<string, vector<string>> bus_and_stops; // <�������, ���������>
};

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
    if (r.bus_and_stops.empty()) {
        os << "No buses"s;
        return os;
    }
    size_t count{};
    for (const auto& [bus, stops] : r.bus_and_stops) {
        ++count;
        os << "Bus "s << bus << ": "s;
        bool isFirst = true;
        for (const auto& stop : r.bus_and_stops.at(bus)) {
            if (isFirst) {
                os << stop;
                isFirst = false;
            }
            else os << " "s << stop;
        }

        if (count != r.bus_and_stops.size()) os << endl;
    }
    return os;
}

class BusManager {
public:
    void AddBus(const string& bus, const vector<string>& stops) {
        // ���������� ���� �����
        bus_and_stops[bus] = stops;
        for (const auto& stop : stops) {
            stop_and_buses[stop].push_back(bus);
        }
    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        BusesForStopResponse buses_for_stop_response;
        buses_for_stop_response.stop = stop;
        buses_for_stop_response.stop_and_buses = stop_and_buses;
        return buses_for_stop_response;
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {
        StopsForBusResponse stops_for_bus_response;
        stops_for_bus_response.bus = bus;
        stops_for_bus_response.bus_and_stops = bus_and_stops;
        stops_for_bus_response.stop_and_buses = stop_and_buses;
        return stops_for_bus_response;
    }

    AllBusesResponse GetAllBuses() const {
        AllBusesResponse all_buses_response;
        all_buses_response.bus_and_stops = bus_and_stops;
        return all_buses_response;
    }

private:
    map<string, vector<string>> bus_and_stops; // <������� (bus), ��������� (stops)>
    map<string, vector<string>> stop_and_buses; // <��������� (stop), �������� (buses)> 
};

int main() {
    int query_count;
    Query q;

    cin >> query_count;

    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
        case QueryType::NewBus:
            bm.AddBus(q.bus, q.stops);
            break;
        case QueryType::BusesForStop:
            cout << bm.GetBusesForStop(q.stop) << endl;
            break;
        case QueryType::StopsForBus:
            cout << bm.GetStopsForBus(q.bus) << endl;
            break;
        case QueryType::AllBuses:
            cout << bm.GetAllBuses() << endl;
            break;
        }
    }
}
/*
NEW_BUS bus stop_count stop1 stop2 ... � �������� ������� �������� � ����������� bus, stop_count � stop1, stop2, ... 
��� ��������� ������� NEW_BUS ������ �������� �������� (bus), ���������� ��������� (stop_count) � �� ��������. ��������: NEW_BUS golden_ring 4 sergiev_posad rostov ivanovo vladimir
����� ������� � ��������� golden_ring � �������� �����������: sergiev_posad, rostov, ivanovo, vladimir.
BUSES_FOR_STOP stop � ������� �������� ���� ��������� ��������, ���������� ����� ��������� stop.
STOPS_FOR_BUS bus � ������� �������� ���� ��������� �������� bus �� ������� ���������, ���� ����� ��������� �� ������ �� ���������.
ALL_BUSES � ������� ������ ���� ��������� � �����������.

������ �����
� ������ ������ ����� ���������� ���������� �������� Q, ����� � Q ������� ������� �������� ��������.
�������������, ��� ��� �������� ��������� � ��������� ������� ���� �� ��������� ����, ���� � ������ �������������.
��� ������� ������� NEW_BUS bus stop_count stop1 stop2 ... �������������, ��� ������� bus �����������, ���������� ��������� ������ ����, 
� ����� ����� stop_count ������� ������ ����� ���������� �������� ���������, ������ ��� �������� � ������ ������ ��������.

������ ������
��� ������� ������� ����� NEW_BUS �������� ��������������� ����� �� ����:
�� ������ BUSES_FOR_STOP stop �������� ����� ������ ������ ���������, ����������� ����� ��� ���������, � ��� �������, � ������� ��� ����������� ��������� NEW_BUS. 
���� ��������� stop �� ����������, �������� No stop.

�� ������ STOPS_FOR_BUS bus �������� �������� ��������� �������� bus � ��������� ������� � ��� �������, � ������� ��� ���� ������ � ��������������� ������� NEW_BUS. 
�������� ������ ��������� stop ������ ����� ��� Stop stop: bus1 bus2 ..., ��� bus1 bus2 ... � ������ ���������, ����������� ����� ��������� stop. 
������ ������ ���� � ��� �������, � ����� �������� ����������� ��������� NEW_BUS, �� ����������� ��������� �������� bus. 
���� ����� ��������� stop �� ��������� �� ���� ������� ����� bus, ������ ������ ��������� ��� �� �������� "no interchange". ���� ������� bus �� ����������, �������� No bus.

�� ������ ALL_BUSES �������� �������� ���� ��������� � ���������� �������. 
�������� ������� �������� bus ������ ����� ��� Bus bus: stop1 stop2 ..., ��� stop1 stop2 ... 
� ������ ��������� �������� bus � ��� �������, � ����� ��� ���� ������ � ��������������� ������� NEW_BUS. 
���� �������� �����������, �������� No buses.

INPUT + OUTPUT:
10
ALL_BUSES
No buses
BUSES_FOR_STOP Marushkino
No stop
STOPS_FOR_BUS 32K
No bus
NEW_BUS 32 3 Tolstopaltsevo Marushkino Vnukovo
NEW_BUS 32K 6 Tolstopaltsevo Marushkino Vnukovo Peredelkino Solntsevo Skolkovo
BUSES_FOR_STOP Vnukovo
32 32K
NEW_BUS 950 6 Kokoshkino Marushkino Vnukovo Peredelkino Solntsevo Troparyovo
NEW_BUS 272 4 Vnukovo Moskovsky Rumyantsevo Troparyovo
STOPS_FOR_BUS 272
Stop Vnukovo: 32 32K 950
Stop Moskovsky: no interchange
Stop Rumyantsevo: no interchange
Stop Troparyovo: 950
ALL_BUSES
Bus 272: Vnukovo Moskovsky Rumyantsevo Troparyovo
Bus 32: Tolstopaltsevo Marushkino Vnukovo
Bus 32K: Tolstopaltsevo Marushkino Vnukovo Peredelkino Solntsevo Skolkovo
Bus 950: Kokoshkino Marushkino Vnukovo Peredelkino Solntsevo Troparyovo
*/
