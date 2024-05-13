#include "header.h" 

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "RUS");

    if (argc != 2) {
        std::cerr << "Используйте: " << argv[0] << " <input_file.txt>\n";
        return 1;
    }
    std::ifstream input_file(argv[1]);

    if (!input_file) {
        std::cerr << "Невозможно открыть файл.\n";
        return 1;
    }

    ComputerClub computer_club;

    {
        std::string num_tables;
        std::getline(input_file, num_tables);
        if (IsNaturalNum(num_tables) and std::stoi(num_tables) > 0) {
            computer_club.SetTables(static_cast<size_t>(std::stoi(num_tables)));
        }
        else {
            // Количество столов некорректное
            std::cout << num_tables << std::endl;
            return 1;
        }
    }

    {
        std::string time;
        std::getline(input_file, time);
        auto [open, close] = SplitIntoWordsTime(time);
        if (TimeIsCorrect(open) and TimeIsCorrect(close)) {
            computer_club.SetOpenTime(ParseTime(open));
            computer_club.SetCloseTime(ParseTime(close));
        }
        else {
            // Время некорректное
            std::cout << time << std::endl;
            return 1;
        }
    }

    {
        std::string price;
        input_file >> price;
        if (IsNaturalNum(price)) {
            computer_club.SetPrice(std::stoi(price));
        }
        else {
            // Цена некорректная
            std::cout << price << std::endl;
            return 1;
        }
    }

    input_file >> std::ws;

    std::cout << computer_club.GetOpenTime().ToString() << std::endl;

    // Когда поступает событие со временем >= время_закрытия,
    // day_is_end = true и вызывается событие ID11
    bool day_is_end{ false };

    std::string line;
    Event event;

    // Сюда будет сохранятся время из предыдущего события для сравнения со временем из нового события,
    // чтобы проверить корректность выполнения условия "Все события идут последовательно во времени. (время события N+1) ≥ (время события N)."
    Time prev_time;

    while (std::getline(input_file, line)) {
        try {
            event = SplitIntoWordsView(static_cast<std::string_view>(line));
            if (prev_time > event.time) {
                std::cout << line << std::endl;
                return 1;
            }
            prev_time = event.time;
        }
        catch (...) {
            return 1;
        }

        if (event.time.hours >= computer_club.GetCloseTime().hours and day_is_end != true) {
            day_is_end = true;
            computer_club.Event11(computer_club.GetCloseTime(), event.client);
        }

        // Если событие в нерабочие часы, тогда "NotOpenYet"
        if (!computer_club.IsClubOpen(event.time)) {
            Print(event.time, event.id, event.client);
            Print(event.time, 13, "NotOpenYet"s);
            continue;
        }

        // Если клиент не в клубе и он пытается выполнить команды, доступные только тем клиентам, которые находятся в клубе
        if (event.id != 1 and computer_club.ClientIsExists(event.client) == false) {
            Print(event.time, event.id, event.client);
            Print(event.time, 13, "ClientUnknown"s);
            continue;
        }

        switch (event.id) {

        case 1: { // Клиент пришёл
            // Если клиент уже в компьютерном клубе, генерируется ошибка "YouShallNotPass"
            if (computer_club.IsClientInside(event.client.name)) {
                Print(event.time, event.id, event.client);
                Print(event.time, 13, "YouShallNotPass"s);
                continue;
            }

            // Если это новый клиент (client.is_inside == false) и он пришёл в рабочие часы, 
            // добавляем его в клуб client.is_inside = true
            Client client(event.client);
            computer_club.PushClient(client);
            Print(event.time, event.id, event.client);
            continue;
        }

        case 2: { // Клиент сел за стол  

            // Проверяем, сидит ли уже клиент за столом
            if (computer_club.IsClientSitting(event.client.name)) {
                // Если клиент уже сидит за столом, то он может сменить стол.
                Print(event.time, event.id, event.client.name, event.table.number);
                computer_club.ChangeTable(event.time, event.client, event.table.number);
                continue;
            }

            // Если стол <номер стола> занят (в том числе, если клиент пытается пересесть за стол, за которым сам и сидит), генерируется ошибка "PlaceIsBusy".
            if (computer_club.IsTableOccupied(event.table.number)) {
                Print(event.time, event.id, event.client.name, event.table.number);
                Print(event.time, 13, "PlaceIsBusy"s);
                continue;
            }

            // Помечаем стол как занятый и привязываем клиента к этому столу
            computer_club.GetTables()[event.table.number - 1].number = event.table.number;
            computer_club.GetTables()[event.table.number - 1].is_occupied = true;
            computer_club.GetTables()[event.table.number - 1].client = event.client.name;

            const auto& fount_client = computer_club.FindClient(event.client);
            fount_client->current_start_table = event.time;
            fount_client->assigned_table = event.table.number;

            Print(event.time, 2, event.client.name, event.table.number);
            continue;
        }

        case 3: { // Клиент ожидает 
            Print(event.time, event.id, event.client.name);

            // Проверяем, есть ли свободные столы
            if (computer_club.HasFreeTables()) {
                Print(event.time, 13, "ICanWaitNoLonger!"s);
                continue;
            }

            // Если количество клиентов в очереди не превышает количество столов
            if (computer_club.GetQueue().size() < computer_club.GetTables().size()) {
                computer_club.PushQueue(event.client);
            }
            else {
                // Иначе генерируется событие ID11
                computer_club.Event11(event.time, event.client);
            }
            continue;
        }

        case 4: {// Клиент ушел  
            if (computer_club.FindClient(event.client)->is_inside == false) {
                Print(event.time, event.id, event.client);
                Print(event.time, 13, "ClientUnknown"s);
                continue;
            }
            // Обработка ухода клиента
            // Освобождаем стол и занимаем его первым клиентом из очереди ожидания (ID 12)
            // Найдем первого клиента из очереди ожидания и освободим его стол
            for (auto& table : computer_club.GetTables()) {
                if (table.is_occupied && table.client == event.client.name) {
                    auto total_time = computer_club.ComputeTotalTime(*computer_club.FindClient(event.client), event.time);
                    if (total_time.minutes > 59) {
                        int additional_hours = total_time.minutes / 60;
                        total_time.hours += additional_hours;
                        total_time.minutes %= 60;
                    }
                    computer_club.ReleaseTable(total_time, event.client);
                    computer_club.RemoveClient(event.client);
                    Print(event.time, 4, event.client);
                    break;
                }
            }
            // Если есть клиенты в очереди ожидания, займем стол первым в очереди
            if (!computer_club.GetQueue().empty()) {
                auto& next_client = computer_club.GetQueue().front();
                for (auto& table : computer_club.GetTables()) {
                    if (!table.is_occupied) {
                        table.client = next_client.name;
                        table.is_occupied = true;
                        const auto& found_client = computer_club.FindClient(next_client);
                        computer_club.FindClient(next_client)->assigned_table = table.number;
                        computer_club.FindClient(next_client)->current_start_table = event.time;
                        computer_club.GetQueue().pop();
                        Print(event.time, 12, table.client, table.number);
                        break;
                    }
                }
            }
            continue;
        }

        default:
            std::cerr << line << std::endl;
            return 1;
        }
    }

    // Поток входящих событий завершился и, если остались не ушедшие клиенты, вызываем для них событие ID11
    if (!day_is_end) {
        auto clients = computer_club.GetClients();
        auto it = clients.begin();

        while (it != clients.end()) {
            computer_club.Event11(computer_club.GetCloseTime(), *it);
            it = clients.erase(it);
        }
        clients = computer_club.GetClients();
    }

    std::cout << computer_club.GetCloseTime().ToString() << std::endl;

    for (const auto& table : computer_club.GetTables()) {
        if (table.total_time.minutes > 0) {
            std::cout << table.number << " "s << (table.total_time.hours + 1) * computer_club.GetPrice() << " "s << table.total_time.ToString() << std::endl;
        }
        else {
            std::cout << table.number << " "s << table.total_time.hours * computer_club.GetPrice() << " "s << table.total_time.ToString() << std::endl;
        }
    }

    return 0;
}