#include <iostream>
#include <vector>
#include <algorithm>
#include <random> 
#include <chrono> 
#include <thread> 

using namespace std;
using namespace chrono;

//Definite the values of the deck
const vector<string> symbols = {"Heart", "Club", "Spade", "Diamond"};

struct card{
    string symbol;
    int number; 
};

const vector<string> symbol_order = {"Heart", "Club", "Spade", "Diamond", "Joker"};


//Function to create a base deck
vector <card> create_deck(){
    vector<card> base_deck;
    for (string symbol : symbols) {
        for (int i=1; i<=13; i++) {
            base_deck.push_back({symbol, i});
        }
    }
    return base_deck;
}

//Function to create a N cards deck
vector<card> create_set(int n) {
    vector<card> base_deck = create_deck(); 
    vector<card> deck;
    int num_decks = n / 52; 
    deck.reserve(n); 

    for (int i = 0; i < num_decks; i++) {
        deck.insert(deck.end(), base_deck.begin(), base_deck.end());
    }

    random_device rd;
    mt19937 g(rd());
    shuffle(deck.begin(), deck.end(), g);

    return deck;
}

//Function Merge Sort
void merge(vector<card>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<card> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int i = 0; i < n2; i++)
        R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].number <= R[j].number)
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];
}

void merge_sort(vector<card>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}


//Function sequential bucket sort 
vector<card> bucket_sort_sequential(vector<card>& deck){
    vector<vector<card>> buckets(4);
    vector<card> sorted_deck;

    for (const card& card : deck) {
        int index = find(symbol_order.begin(), symbol_order.end(), card.symbol) - symbol_order.begin();
        buckets[index].push_back(card);
    }

    for(auto& bucket:buckets){
        merge_sort(bucket, 0, bucket.size() - 1);
        sorted_deck.insert(sorted_deck.end(), bucket.begin(), bucket.end());
    }

    return sorted_deck;
}

//Function parallel bucket sort
vector<card> bucket_sort_parallel(vector<card>& deck){
    vector<vector<card>> buckets(4);
    vector<card> sorted_deck;
    vector<thread> threads;

    for (const card& card : deck) {
        int index = find(symbol_order.begin(), symbol_order.end(), card.symbol) - symbol_order.begin();
        buckets[index].push_back(card);
    }

    for(auto& bucket:buckets){
        threads.push_back(thread([&bucket](){
            merge_sort(bucket, 0, bucket.size() - 1);
        }));
        sorted_deck.insert(sorted_deck.end(), bucket.begin(), bucket.end());

    }

    for (auto& t : threads) {
        t.join();
    }

    return sorted_deck;
}

//Function Evaluation Data
void evaluation(float time_par, float time_seq){
    float speedup = time_seq/time_par;
    float overall_speedup = 1/((1-0.80)+0.80/speedup);
    float efficiency = 100*speedup/4;

    cout << "Speedup: " << speedup << endl;
    cout << "Overall Speedup: " << overall_speedup << endl;
    cout << "Efficiency: " << efficiency << "%"<< endl;

}

//Function Print
void print_sample(const vector<card>& deck) {
    for (int i = 0; i < 64; i++) {
        cout << deck[i].symbol << " " << deck[i].number << endl;
    }
    cout << "...\n";
}

int main(){
    int num_cards = 4000000;
    vector<card> set_cards = create_set(num_cards);

    //Sequential execution time
    auto start_seq = high_resolution_clock::now(); 
    vector<card> sorted_set_seq = bucket_sort_sequential(set_cards);
    auto end_seq = high_resolution_clock::now(); 

    auto duration_seq = duration_cast<milliseconds>(end_seq - start_seq);
    cout << "Tiempo para ordenar - Secuencial: " << duration_seq.count() << " ms\n";

    //Parallel execution time
    auto start_par = high_resolution_clock::now(); 
    vector<card> sorted_set_par = bucket_sort_parallel(set_cards);
    auto end_par = high_resolution_clock::now(); 

    auto duration_par = duration_cast<milliseconds>(end_par - start_par);
    cout << "Tiempo para ordenar - Paralelo: " << duration_par.count() << " ms\n";

    //Evaluation
    evaluation(static_cast<float>(duration_par.count()), static_cast<float>(duration_seq.count()));
    
    return 0;
}