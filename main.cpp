#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

set<string> ParseStopWords(const string& text) {
    set<string> stop_words;
    for (const string& word : SplitIntoWords(text)) {
        stop_words.insert(word);
    }
    return stop_words;
}

vector<string> SplitIntoWordsNoStop(const string& text, const set<string>& stop_words) {
    vector<string> words;
    for (const string& word : SplitIntoWords(text)) {
        if (stop_words.count(word) == 0) {
            words.push_back(word);
        }
    }
    return words;
}

void AddDocument(vector<pair<int, vector<string>>>& documents,
                 const set<string>& stop_words,
                 int document_id,
                 const string& document) {
    pair<int, vector<string>> words = {document_id, SplitIntoWordsNoStop(document, stop_words)};
    documents.push_back(words);
}

set<string> ParseQuery(const string& text, const set<string>& stop_words) {
    set<string> query_words;
    for (const string& word : SplitIntoWordsNoStop(text, stop_words)) {
        query_words.insert(word);
    }
    return query_words;
}

int MatchDocument(const pair<int, vector<string>>& content,
                  const set<string>& query_words) {
    int relevance = 0;
    const vector<string>& copy_string = content.second;
    for (const auto& words : copy_string) {
        if (query_words.count(words) != 0) {
            ++relevance;
        }
    }
    return relevance;
}

// Релевантность возвращется с идентификатором документа как пару из двух целых чисел {document_id, relevance}
vector<pair<int, int>> FindDocuments(const vector<pair<int, vector<string>>>& documents,
                                     const set<string>& stop_words,
                                     const string& query) {
    const set<string> query_words = ParseQuery(query, stop_words);
    vector<pair<int, int>> matched_documents;
    int document_id = 0;

    for (const auto& document : documents) {
        int relevance = MatchDocument(document, query_words);
        if (relevance != 0) {
            matched_documents.push_back({document_id, relevance});
        }
        ++document_id;
    }
    return matched_documents;
}


// Для каждого документа возвращает его релевантность и id
vector<pair<int, int>> FindAllDocuments(const vector<pair<int, vector<string>>>& documents,
    const set<string>& query_words)
{
    vector<pair<int, int>> matched_documents;
    int document_id = 0;

    for (const auto& document : documents) {
        int relevance = MatchDocument(document, query_words);
        if (relevance != 0) {
            matched_documents.push_back({relevance, document_id});
        }
        ++document_id;
    }
    return matched_documents;
    // Превратите функцию FindDocuments в FindAllDocuments
    // Первым элементом возвращаемых пар идёт релевантность документа, а вторым - его id
}



// Возвращает топ-5 самых релевантных документов в виде пар: {id, релевантность}
vector<pair<int, int>> FindTopDocuments(const vector<pair<int, vector<string>>>& documents,
                                        const set<string>& stop_words, const string& raw_query) {
    vector<pair<int, int>> matched_documents = FindAllDocuments(documents, ParseQuery(raw_query, stop_words));
    vector<pair<int, int>> result;
    sort(matched_documents.begin(), matched_documents.end());
    reverse(matched_documents.begin(), matched_documents.end());
    int flag = 0;

    for (const auto& [rel, id] : matched_documents) {
        if (rel > 0 && flag < 5) {
            result.push_back({id, rel});
            ++flag;
        }
    }

    return result;
}


int main() {
    const string stop_words_joined = ReadLine();
    const set<string> stop_words = ParseStopWords(stop_words_joined);

    // Считываем документы
    vector<pair<int, vector<string>>> documents;
    const int document_count = ReadLineWithNumber();
    for (int document_id = 0; document_id < document_count; ++document_id) {
        AddDocument(documents, stop_words, document_id, ReadLine());
    }

    const string query = ReadLine();
    // Выводим результаты поиска по запросу query
    for (auto [document_id, relevance] : FindTopDocuments(documents, stop_words, query)) {
        cout << "{ document_id = "s << document_id << ", relevance = "s << relevance << " }"s
             << endl;
    }
}
