#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;
const double EPSILON = 1e-6; 

string ReadLine() {
    string s;
    getline(std::cin, s);
    return s;
}

int ReadLineWithNumber() {
    int number;
    cin >> number;
    cin >> ws;
    return number;
}

enum class DocumentStatus
{
    ACTUAL, // актуальный
    IRRELEVANT, // устаревший
    BANNED, // отклонённый
    REMOVED // удалённый
};

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
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

    return words;
}

struct Document {
    int id;
    double relevance;
    int rating;
    DocumentStatus status;
};

struct Query {
    set<string> plus_words;
    set<string> minus_words;
};

class SearchServer {
public:
    int GetDocumentCount() const {
        return document_count_;
    }

    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document, DocumentStatus temp_status, const vector<int>& temp_ratings) {
        const vector<string>& words = SplitIntoWordsNoStop(document);
        const double& td = 1. / static_cast<double>(words.size());
        for (const string& word : words) {
            documents_[word][document_id] += td;
        }
        ratings.emplace(document_id, ComputeAverageRating(temp_ratings));
        status.emplace(document_id, temp_status);
        ++document_count_;
    }

    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus temp_status = DocumentStatus::ACTUAL) const {
        const Query query_words = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query_words); // id, relevance, rating, status

        sort(matched_documents.begin(), matched_documents.end(), [](const Document& lhs, const Document& rhs) {
            if (lhs.relevance != rhs.relevance) return lhs.relevance > rhs.relevance;
            else if (std::abs(lhs.relevance - rhs.relevance) < EPSILON) return lhs.rating > rhs.rating;
            else return false;
            });

        vector<Document> filtered_documents;
        copy_if(matched_documents.begin(), matched_documents.end(), back_inserter(filtered_documents),
            [temp_status](const Document& doc) {
                return doc.status == temp_status;
            });

        if (filtered_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            filtered_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }

        return filtered_documents;
    }



    //tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
    //    vector<string> matched_words;
    //    Query query = ParseQuery(raw_query);

    //    for (const string& word : query.plus_words) { // проходимся по плюс-словам
    //        if (documents_.at(word).count(document_id)) {
    //            matched_words.push_back(word);
    //        }
    //    }

    //    for (const auto& minus_word : query.minus_words) { // пробегаемся по минус словам:
    //        if (documents_.count(minus_word)) { // если в словаре всех слов есть минус-слово,
    //            matched_words.clear();
    //            return { matched_words, status.at(document_id) }; // то возвращаем пустой вектор
    //        }
    //    }

    //    return { matched_words, status.at(document_id) };
    //}

    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query, int document_id) const {
        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) { // проходимся по плюс-словам
            if (documents_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
            if (documents_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
        return { matched_words, status.at(document_id) };
    }

private:
    map<int, int> ratings; // <document_id, rating>
    map<int, DocumentStatus> status; // <document_id, status>
    map<string, map<int, double>> documents_; // <word, map<id_document, relevance>>
    int document_count_{};
    set<string> stop_words_;

    static int ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.empty()) return 0;
        else {
            return std::accumulate(ratings.begin(), ratings.end(), 0) / static_cast<int>(ratings.size());
        }
    }

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    Query ParseQuery(const string& text) const { // поиск по запросу
        Query query_words;
        for (const string& word : SplitIntoWordsNoStop(text)) {
            if (word.at(0) != '-') {
                query_words.plus_words.insert(word);
            }
            else {
                query_words.minus_words.insert(word.substr(1));
            }
        }
        return query_words;
    }

    double ComputeIDF(const string& plus_word) const {
        return log(static_cast<double>(document_count_) / documents_.at(plus_word).size());
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        map<int, double> document_to_relevance; // <document_id, relevance>

        for (const string& plus_word : query_words.plus_words) {
            if (documents_.count(plus_word)) {
                for (const auto& [document_id, tf] : documents_.at(plus_word)) {
                    document_to_relevance[document_id] += ComputeIDF(plus_word) * tf;
                }
            }
        }

        for (const string& minus_word : query_words.minus_words) {
            if (documents_.count(minus_word)) {
                for (const auto& [document_id, relevance] : documents_.at(minus_word)) {
                    document_to_relevance.erase(document_id);
                }
            }
        }

        vector<Document> matched_documents;
        for (auto& [document_id, relevance] : document_to_relevance) {
            matched_documents.push_back({ document_id, relevance, ratings.at(document_id), status.at(document_id) });
        }
        return matched_documents;
    }
};

void PrintDocument(const Document& document) {
    cout << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating
        << " }"s << endl;
}

int main() {
    setlocale(LC_ALL, "RUS");
    SearchServer search_server;
    search_server.SetStopWords("и в на"s);

    search_server.AddDocument(0, "белый кот и модный ошейник"s, DocumentStatus::ACTUAL, { 8, -3 });
    search_server.AddDocument(1, "пушистый кот пушистый хвост"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, { 5, -12, 2, 1 });

    for (const Document& document : search_server.FindTopDocuments("ухоженный кот"s)) {
        PrintDocument(document);
    }
}