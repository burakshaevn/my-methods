#pragma once
#include <cassert> 
#include <fstream>
#include <iostream>
#include <queue>  
#include <regex>
#include <sstream>
#include <vector>  

using namespace std::string_literals;

struct Time {
    Time()
        : hours(0)
        , minutes(0)
    {}
    explicit Time(const int hour, const int minute)
        : hours(hour)
        , minutes(minute)
    {}
    int hours;
    int minutes;

    std::string ToString() const;

    bool operator<(const Time& other) const;

    bool operator>(const Time& other) const;
};

Time ParseTime(const std::string& time_str);

struct Client {
    Client()
        : name(""s)
        , is_inside(false)
        , assigned_table(-1)
        , total_time(Time())
        , current_start_table(Time())
    {}
    explicit Client(const std::string& client_name)
        : name(client_name)
        , is_inside(false)
        , assigned_table(-1)
        , total_time(Time())
        , current_start_table(Time())
    {}
    std::string name;
    bool is_inside;
    int assigned_table;
    Time total_time;
    Time current_start_table;
};

struct Table {
    Table()
        : number(0)
        , revenue(0)
        , total_time(Time())
        , is_occupied(false)
        , client(""s)
    {}
    explicit Table(const int num)
        : number(num)
        , revenue(0)
        , total_time(Time())
        , is_occupied(false)
        , client(""s)
    {}
    int number;
    int revenue;
    Time total_time;
    bool is_occupied;
    std::string client;
};

struct Event {
    Event()
        : time(Time())
        , id(0)
        , client(Client())
        , table(Table())
    {}
    Time time;
    int id;
    Client client;
    Table table;
};

void Print(const Time& time, const int id, const Client& client);

void Print(const Time& time, const int id, const std::string& client, const int& table);

void Print(const Time& time, const int id, const std::string& hint);

bool TimeIsCorrect(const std::string& time);

bool IsNaturalNum(const std::string& str);

bool NameIsCorrect(const std::string& str);

std::pair<std::string, std::string> SplitIntoWordsTime(const std::string& text);

Event SplitIntoWordsView(std::string_view str);

class ComputerClub {
public:
    void SetPrice(const int value);

    int GetPrice() const;

    void SetOpenTime(Time&& time);

    Time& GetOpenTime();

    void SetCloseTime(Time&& time);

    Time& GetCloseTime();

    void SetTables(const size_t num);

    std::vector<Table>& GetTables();

    void PushQueue(const Client& client);

    std::queue<Client>& GetQueue();

    void PushClient(Client& client);

    std::vector<Client>& GetClients();

    bool ClientIsExists(const Client& found_client) const;

    Client* FindClient(const Client& client_);

    void RemoveClient(const Client& client_);

    bool IsClientInside(const std::string& client_name) const;

    bool IsClientSitting(const std::string& client_name) const;

    bool IsTableOccupied(int table_number) const;

    bool IsClubOpen(const Time& time) const;

    void ChangeTable(const Time& time, const Client& client, int new_table_number);

    Time ComputeTotalTime(const Client& client, const Time& end_time);

    Time ComputeTotalTimeForTable(const Client& client, const Time& end_time);

    bool HasFreeTables() const;

    void HandleClientLeave(const Time& time, const Client client);

    void ReleaseTable(const Time& time, const Client client);

    void AssignTable(Client& client);

    void Event11(const Time& time, const Client& cl);

private:
    Time open;
    Time close;
    int price_of_hour;
    std::queue<Client> queue_;
    std::vector<Table> tables_;
    std::vector<Client> clients_;
};

void Test();