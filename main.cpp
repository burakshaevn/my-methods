#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <span>

using namespace std;

template <typename Documents, typename Term>
vector<double> ComputeTfIdfs(const Documents& documents, const Term& term) {
    vector<double> tf_idfs; // вектор, содержащий TD-IDF

    /*
    TF - частота слова в конкретном документе 
    IDF - количество всех документов делят на количество тех, где встречается слово
    */
     
    int term_occurrences{}; // количество документов, где встречается term
    for (const auto& document : documents) {
        int сount_term_in_document = std::count(document.begin(), document.end(), term);
        tf_idfs.push_back(static_cast<double>(сount_term_in_document) / document.size());

        if (сount_term_in_document > 0) { // если терм в принципе встречается, 
            ++term_occurrences; // то +1 документ, где есть терм
        }
    }
    double idf = log(static_cast<double>(documents.size()) / term_occurrences);
    for (auto& td : tf_idfs) {
        td *= idf;
    }
    return tf_idfs;
}

int main() {
    const vector<vector<string>> documents = {
        {"белый"s, "кот"s, "и"s, "модный"s, "ошейник"s},
        {"пушистый"s, "кот"s, "пушистый"s, "хвост"s},
        {"ухоженный"s, "пёс"s, "выразительные"s, "глаза"s},
    };
    const auto& tf_idfs = ComputeTfIdfs(documents, "кот"s);
    for (const double tf_idf : tf_idfs) {
        cout << tf_idf << " "s;
    }
    cout << endl;
    return 0;
}

// Output >> 0.081093 0.101366 0