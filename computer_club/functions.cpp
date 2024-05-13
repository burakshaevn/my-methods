#include "header.h"

std::string Time::ToString() const {
    std::string hour_str = std::to_string(hours);
    std::string minute_str = std::to_string(minutes);
    size_t num_hours_digits = std::count_if(hour_str.begin(), hour_str.end(), [](char c) {
        return std::isdigit(c);
        });

    size_t num_minutes_digits = std::count_if(minute_str.begin(), minute_str.end(), [](char c) {
        return std::isdigit(c);
        });

    if (num_hours_digits == 1 and num_minutes_digits == 1) {
        return "0"s + hour_str + ":"s + "0"s + minute_str;
    }
    else if (num_hours_digits == 1 and num_minutes_digits == 2) {
        return "0"s + hour_str + ":"s + minute_str;
    }
    else if (num_hours_digits == 2 and num_minutes_digits == 1) {
        return hour_str + ":"s + "0"s + minute_str;
    }
    else if (num_hours_digits == 2 and num_minutes_digits == 2) {
        return hour_str + ":"s + minute_str;
    }

    return "";
}

bool Time::operator<(const Time& other) const {
    if (hours != other.hours) {
        return hours < other.hours;
    }
    return minutes < other.minutes;
}

bool Time::operator>(const Time& other) const {
    if (hours != other.hours) {
        return hours > other.hours;
    }
    return minutes > other.minutes;
}

bool TimeIsCorrect(const std::string& time) {
    std::regex time_regex("([01][0-9]|2[0-3]):[0-5][0-9]");
    std::smatch match;
    return std::regex_match(time, match, time_regex);
}

Time ParseTime(const std::string& time_str) {
    auto it = time_str.find(":"s);
    std::string hours(time_str.begin(), time_str.begin() + it);
    std::string minutes(time_str.begin() + it + 1, time_str.end());
    return Time(std::stoi(hours), std::stoi(minutes));
}

bool IsNaturalNum(const std::string& str) {
    static std::regex num_reg("^[0-9]+$");
    return regex_match(str, num_reg);
}

bool NameIsCorrect(const std::string& str) {
    static std::regex name_literal("^[a-z0-9_-]*$");
    return regex_match(str, name_literal);
}

std::pair<std::string, std::string> SplitIntoWordsTime(const std::string& text) {
    std::vector<std::string> words;
    std::string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        }
        else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    if (words.size() != 2) {
        return { ""s, ""s };
    }
    else {
        return { words.at(0), words.at(1) };
    }
}

Event SplitIntoWordsView(std::string_view str) {
    std::vector<std::string_view> result;
    auto pos = str.find_first_not_of(" ");
    const auto pos_end = str.npos;
    while (pos != pos_end) {
        auto space = str.find(' ', pos);
        result.push_back(space == pos_end ? str.substr(pos) : str.substr(pos, space - pos));
        pos = str.find_first_not_of(" ", space);
    }

    Event event;
    if (TimeIsCorrect(std::string(result[0]))) {
        event.time = ParseTime(std::string(result[0]));
    }
    else {
        // Время некорректное
        std::cout << str << std::endl;
        throw std::exception();
    }
    if (IsNaturalNum(std::string(result[1]))) {
        event.id = std::stoi(static_cast<std::string>(result[1]));
    }
    else {
        // id некорректный
        std::cout << str << std::endl;
        throw std::exception();
    }
    if (NameIsCorrect(std::string(result[2]))) {
        event.client.name = std::string(result[2]);
    }
    else {
        // client name uncorrect
        std::cout << str << std::endl;
        throw std::exception();
    }

    // Объявить стол занятым клиентом event.client.name
    if (result.size() == 4) {
        if (IsNaturalNum(std::string(result[3]))) {
            event.table.number = std::stoi(std::string(result[3]));
            event.table.client = event.client.name;
            event.table.is_occupied = true;
        }
    }
    return event;
}

void Print(const Time& time, const int id, const Client& client) {
    std::cout << time.ToString() << " "s << id << " "s << client.name << std::endl;
}

void Print(const Time& time, const int id, const std::string& client, const int& table) {
    std::cout << time.ToString() << " "s << id << " "s << client << " "s << table << std::endl;
}

void Print(const Time& time, const int id, const std::string& hint) {
    std::cout << time.ToString() << " "s << id << " "s << hint << std::endl;
}

void ComputerClub::SetPrice(const int value) {
    price_of_hour = value;
}

int ComputerClub::GetPrice() const {
    return price_of_hour;
}

void ComputerClub::SetOpenTime(Time&& time) {
    open = std::move(time);
}

Time& ComputerClub::GetOpenTime() {
    return open;
}

void ComputerClub::SetCloseTime(Time&& time) {
    close = std::move(time);
}

Time& ComputerClub::GetCloseTime() {
    return close;
}

void ComputerClub::SetTables(const size_t num) {
    for (int i = 1; i <= num; ++i) {
        tables_.push_back(Table(i));
    }
}

std::vector<Table>& ComputerClub::GetTables() {
    return tables_;
}

void ComputerClub::PushQueue(const Client& client) {
    queue_.push(client);
}

std::queue<Client>& ComputerClub::GetQueue() {
    return queue_;
}

void ComputerClub::PushClient(Client& client) {
    client.is_inside = true;
    clients_.push_back(client);
}

std::vector<Client>& ComputerClub::GetClients() {
    return clients_;
}

bool ComputerClub::ClientIsExists(const Client& found_client) const {
    for (auto& client : clients_) {
        if (client.name == found_client.name) {
            return true;
        }
    }
    return false;
}

Client* ComputerClub::FindClient(const Client& client_) {
    for (auto& client : clients_) {
        if (client.name == client_.name) {
            return &client;
        }
    }
    return nullptr;
}

void ComputerClub::RemoveClient(const Client& client_) {
    clients_.erase(std::remove_if(clients_.begin(), clients_.end(), [&client_](const Client& client) {
        return client.name == client_.name;
        }), clients_.end());
}

bool ComputerClub::IsClientInside(const std::string& client_name) const {
    for (const auto& client : clients_) {
        if (client.name == client_name) {
            return true;
        }
    }
    return false;
}

bool ComputerClub::IsClientSitting(const std::string& client_name) const {
    for (const auto& table : tables_) {
        if (table.is_occupied && table.client == client_name) {
            return true;
        }
    }
    return false;
}

bool ComputerClub::IsTableOccupied(int table_number) const {
    return tables_[table_number - 1].is_occupied;
}

bool ComputerClub::IsClubOpen(const Time& time) const {
    if (time.hours >= open.hours and time.hours < close.hours) {
        return true;
    }
    else {
        return false;
    }
}

void ComputerClub::ChangeTable(const Time& time, const Client& client, int new_table_number) {
    for (auto& table : tables_) {
        if (table.is_occupied && table.client == client.name) {
            if (!tables_[new_table_number - 1].is_occupied) {
                // Освобождаем стол от клиента
                table.client = ""s;
                table.is_occupied = false;

                // Перекидываем клиента за другой стол
                tables_[new_table_number - 1].is_occupied = true;
                tables_[new_table_number - 1].client = client.name;

                // При замене стола мы к total_time добавляем столько времени, сколько он сидел за current_time (за текущим столом)
                auto total_time = ComputeTotalTime(*FindClient(client), time);
                if (total_time.minutes > 59) {
                    int additional_hours = total_time.minutes / 60;
                    total_time.hours += additional_hours;
                    total_time.minutes %= 60;
                }
                const auto& fount_client = FindClient(client);
                fount_client->total_time.hours += total_time.hours;
                fount_client->total_time.minutes += total_time.minutes;
                fount_client->current_start_table = time;
                table.total_time.hours += total_time.hours;
                table.total_time.minutes += total_time.minutes;
                return;
            }
            else {
                Print(time, 13, "PlaceIsBusy"s);
                return;
            }
        }
    }
}

Time ComputerClub::ComputeTotalTime(const Client& client, const Time& end_time) {
    if (client.total_time.hours != 0 || client.total_time.minutes != 0) {
        int total_hours = client.total_time.hours;
        int total_minutes = client.total_time.minutes;
        total_hours += end_time.hours - client.current_start_table.hours;
        total_minutes += end_time.minutes - client.current_start_table.minutes;
        if (total_minutes >= 60) {
            total_hours += total_minutes / 60;
            total_minutes %= 60;
        }

        return Time{ total_hours, total_minutes };
    }
    else {
        // Если клиент сидит за столом в первый раз, выполняем расчет как обычно
        int hours_difference = end_time.hours - client.current_start_table.hours;
        int minutes_difference = end_time.minutes - client.current_start_table.minutes;
        if (minutes_difference < 0) {
            hours_difference -= 1;
            minutes_difference += 60;
        }
        return Time{ hours_difference, minutes_difference };
    }
}

Time ComputerClub::ComputeTotalTimeForTable(const Client& client, const Time& end_time) {
    int hours_difference = end_time.hours - client.current_start_table.hours;
    int minutes_difference = end_time.minutes - client.current_start_table.minutes;
    if (minutes_difference < 0) {
        hours_difference -= 1;
        minutes_difference += 60;
    }
    return Time{ hours_difference, minutes_difference };
}

bool ComputerClub::HasFreeTables() const {
    for (const auto& table : tables_) {
        if (!table.is_occupied) {
            return true;
        }
    }
    return false;
}

void ComputerClub::HandleClientLeave(const Time& time, const Client client) {
    // Проверяем, есть ли клиент в клубе
    if (!IsClientInside(client.name)) {
        Print(time, 4, client.name);
        Print(time, 13, "ClientUnknown"s);
        return;
    }
    ReleaseTable(time, client);

    // Если в очереди ожидания есть клиенты, помещаем первого из них за освободившийся стол и генерируем событие ID 12
    if (!queue_.empty()) {
        Client next_client = queue_.front();
        queue_.pop();
        AssignTable(next_client);
        Print(time, 12, next_client.name, next_client.assigned_table);
    }

    // Удаляем клиента из клуба
    RemoveClient(client);
    Print(time, 4, client.name);
}

// Освободить стол
void ComputerClub::ReleaseTable(const Time& time, const Client client) {
    for (auto& table : tables_) {
        if (table.is_occupied && table.client == client.name) {
            table.client = ""s;
            table.is_occupied = false;
            table.total_time.hours += time.hours;
            table.total_time.minutes += time.minutes;
            if (table.total_time.minutes > 59) {
                table.total_time.hours += table.total_time.minutes / 60;
                table.total_time.minutes %= 60;
            }
            return;
        }
    }
}

void ComputerClub::AssignTable(Client& client) {
    for (auto& table : tables_) {
        if (!table.is_occupied) {
            table.is_occupied = true;
            table.client = client.name;
            client.assigned_table = &table - &tables_[0] + 1;
            return;
        }
    }
}

void ComputerClub::Event11(const Time& time, const Client& cl) {
    for (const auto& client : clients_) {
        if (client.name == cl.name and client.is_inside == true) {
            Print(time, 11, client.name);
            if (cl.total_time.hours != 0 || cl.total_time.minutes != 0) {
                ReleaseTable(ComputeTotalTimeForTable(client, time), client);
            }
            else {
                ReleaseTable(ComputeTotalTime(client, time), client);
            }
            RemoveClient(client);
            return;
        }
    }
}