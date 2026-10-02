#include <iostream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

class Movie {
public:
    string id;
    string name;
    string genre;
    double rating;

    Movie() : id(""), name(""), genre(""), rating(0.0) {}

    Movie(string i, string n, string g, double r) : id(i), name(n), genre(g), rating(r) {}
};

class MovieManager {
private:
    vector<Movie> movies;
    unordered_map<string, Movie> movieByID;
    unordered_map<string, vector<Movie>> moviesByGenre;
    stack<Movie> recentlyWatched;
    queue<Movie> watchHistory;
    unordered_map<string, vector<string>> recommendationGraph;
    const string filename = "movies.txt";

    void mergeByRating(vector<Movie>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        
        vector<Movie> L(n1), R(n2);
        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
        
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i].rating >= R[j].rating) { 
                arr[k++] = L[i++];
            } else {
                arr[k++] = R[j++];
            }
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    void mergeSortByRating(vector<Movie>& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSortByRating(arr, left, mid);
        mergeSortByRating(arr, mid + 1, right);
        mergeByRating(arr, left, mid, right);
    }
    
    void mergeByID(vector<Movie>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        vector<Movie> L(n1), R(n2);
        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
        
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i].id <= R[j].id) {
                arr[k++] = L[i++];
            } else {
                arr[k++] = R[j++];
            }
        }
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }
    
    void mergeSortByID(vector<Movie>& arr, int left, int right) {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSortByID(arr, left, mid);
        mergeSortByID(arr, mid + 1, right);
        mergeByID(arr, left, mid, right);
    }

    void buildGraph() {
        recommendationGraph.clear();
        for (const auto& m1 : movies) {
            for (const auto& m2 : movies) {
                if (m1.id != m2.id && m1.genre == m2.genre) {
                    recommendationGraph[m1.id].push_back(m2.id);
                }
            }
        }
    }

    void updateMaps() {
        movieByID.clear();
        moviesByGenre.clear();
        for (const auto& m : movies) {
            movieByID[m.id] = m;
            moviesByGenre[m.genre].push_back(m);
        }
    }

public:
    MovieManager() {
        checkAndCreateSampleData();
        loadFromFile();
    }

    void checkAndCreateSampleData() {
        ifstream file(filename);
        if (!file.good()) {
            ofstream newFile(filename);
            newFile << "M001,The Matrix,Sci-Fi,8.7\n";
            newFile << "M002,Inception,Sci-Fi,8.8\n";
            newFile << "M003,Interstellar,Sci-Fi,8.6\n";
            newFile << "M004,The Dark Knight,Action,9.0\n";
            newFile << "M005,Avengers: Endgame,Action,8.4\n";
            newFile << "M006,Gladiator,Action,8.5\n";
            newFile << "M007,Titanic,Romance,7.8\n";
            newFile << "M008,The Notebook,Romance,7.8\n";
            newFile << "M009,Pride & Prejudice,Romance,7.8\n";
            newFile << "M010,The Hangover,Comedy,7.7\n";
            newFile << "M011,Superbad,Comedy,7.6\n";
            newFile << "M012,Step Brothers,Comedy,6.9\n";
            newFile << "M013,The Conjuring,Horror,7.5\n";
            newFile << "M014,A Nightmare on Elm Street,Horror,7.5\n";
            newFile << "M015,The Exorcist,Horror,8.0\n";
            newFile << "M016,Spirited Away,Animation,8.6\n";
            newFile << "M017,Toy Story,Animation,8.3\n";
            newFile << "M018,Finding Nemo,Animation,8.1\n";
            newFile << "M019,The Godfather,Drama,9.2\n";
            newFile << "M020,The Shawshank Redemption,Drama,9.3\n";
            newFile << "M021,Forrest Gump,Drama,8.8\n";
            newFile << "M022,The Matrix Reloaded,Sci-Fi,7.2\n";
            newFile << "M023,The Lord of the Rings: The Return of the King,Fantasy,8.9\n";
            newFile << "M024,Pulp Fiction,Crime,8.9\n";
            newFile << "M025,Fight Club,Drama,8.8\n";
            newFile << "M026,Goodfellas,Crime,8.7\n";
            newFile << "M027,The Silence of the Lambs,Thriller,8.6\n";
            newFile << "M028,Se7en,Thriller,8.6\n";
            newFile << "M029,Star Wars: Episode IV - A New Hope,Sci-Fi,8.6\n";
            newFile << "M030,The Green Mile,Drama,8.6\n";
            newFile.close();
            cout << "Sample database created with 30 movies.\n";
        }
    }

    void loadFromFile() {
        movies.clear();
        ifstream file(filename);
        string line;
        
        while (getline(file, line)) {
            if(line.empty()) continue;
            
            stringstream ss(line);
            string id, name, genre, ratingStr;
            
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, genre, ',');
            getline(ss, ratingStr, ',');
            
            if(!id.empty() && !ratingStr.empty()) {
                double rating = stod(ratingStr);
                movies.push_back(Movie(id, name, genre, rating));
            }
        }
        file.close();
        
        updateMaps();
        buildGraph();
    }

    void saveToFile() {
        ofstream file(filename);
        for (const auto& m : movies) {
            file << m.id << "," << m.name << "," << m.genre << "," << m.rating << "\n";
        }
        file.close();
    }
    
    void displayMovie(const Movie& m) {
        cout << left << setw(6) << m.id 
             << setw(30) << m.name 
             << setw(15) << m.genre 
             << m.rating << endl;
    }

    void displayAllMovies() {
        cout << "\n--- All Movies ---\n";
        cout << left << setw(6) << "ID" << setw(30) << "Name" << setw(15) << "Genre" << "Rating\n";
        for (const auto& m : movies) {
            displayMovie(m);
        }
    }

    void searchMovieByID() {
        string id;
        cout << "Enter Movie ID to search: ";
        cin >> id;
        
        vector<Movie> sortedMovies = movies;
        if(!sortedMovies.empty()) {
            mergeSortByID(sortedMovies, 0, sortedMovies.size() - 1);
        }
        
        int left = 0, right = sortedMovies.size() - 1;
        bool found = false;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (sortedMovies[mid].id == id) {
                cout << "\nMovie found!\n";
                cout << left << setw(6) << "ID" << setw(30) << "Name" << setw(15) << "Genre" << "Rating\n";
                displayMovie(sortedMovies[mid]);
                found = true;
                break;
            }
            if (sortedMovies[mid].id < id) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        
        if (!found) {
            cout << "Movie with ID " << id << " not found.\n";
        }
    }

    void searchMovieByName() {
        string name;
        cout << "Enter Movie Name to search: ";
        cin.ignore();
        getline(cin, name);
        
        bool found = false;
        cout << "\nSearch Results:\n";
        cout << left << setw(6) << "ID" << setw(30) << "Name" << setw(15) << "Genre" << "Rating\n";
        for (const auto& m : movies) {
            if (m.name.find(name) != string::npos) {
                displayMovie(m);
                found = true;
            }
        }
        if (!found) cout << "No movies found matching the name.\n";
    }

    void searchMoviesByGenre() {
        string genre;
        cout << "Enter Genre: ";
        cin >> genre;
        
        if (moviesByGenre.find(genre) != moviesByGenre.end()) {
            cout << "\nMovies in genre '" << genre << "':\n";
            cout << left << setw(6) << "ID" << setw(30) << "Name" << setw(15) << "Genre" << "Rating\n";
            for (const auto& m : moviesByGenre[genre]) {
                displayMovie(m);
            }
        } else {
            cout << "No movies found for genre '" << genre << "'.\n";
        }
    }

    void addNewMovie() {
        string id, name, genre;
        double rating;
        
        cout << "Enter Movie ID (e.g., M021): ";
        cin >> id;
        
        if (movieByID.find(id) != movieByID.end()) {
            cout << "Error: Movie ID already exists!\n";
            return;
        }
        
        cout << "Enter Movie Name: ";
        cin.ignore();
        getline(cin, name);
        
        cout << "Enter Genre: ";
        cin >> genre;
        
        cout << "Enter Rating (0.0 - 10.0): ";
        cin >> rating;
        
        if (rating < 0.0 || rating > 10.0) {
            cout << "Invalid rating. Must be between 0 and 10.\n";
            return;
        }
        
        Movie newMovie(id, name, genre, rating);
        movies.push_back(newMovie);
        
        updateMaps();
        buildGraph();
        saveToFile();
        
        cout << "Movie added successfully!\n";
    }

    void deleteMovie() {
        string id;
        cout << "Enter Movie ID to delete: ";
        cin >> id;
        
        auto it = remove_if(movies.begin(), movies.end(), [&](const Movie& m){ return m.id == id; });
        
        if (it != movies.end()) {
            movies.erase(it, movies.end());
            
            updateMaps();
            buildGraph();
            saveToFile();
            
            cout << "Movie deleted successfully!\n";
        } else {
            cout << "Movie ID not found.\n";
        }
    }

    void updateMovie() {
        string id;
        cout << "Enter Movie ID to update: ";
        cin >> id;
        
        for (auto& m : movies) {
            if (m.id == id) {
                cout << "Current Name: " << m.name << "\nEnter new name: ";
                cin.ignore();
                getline(cin, m.name);
                
                cout << "Current Genre: " << m.genre << "\nEnter new genre: ";
                cin >> m.genre;
                
                cout << "Current Rating: " << m.rating << "\nEnter new rating: ";
                cin >> m.rating;
                
                updateMaps();
                buildGraph();
                saveToFile();
                
                cout << "Movie updated successfully!\n";
                return;
            }
        }
        cout << "Movie ID not found.\n";
    }

    void displayTopRatedMovies() {
        if(movies.empty()) {
            cout << "No movies available.\n";
            return;
        }
        
        vector<Movie> sortedMovies = movies;
        
        mergeSortByRating(sortedMovies, 0, sortedMovies.size() - 1);
        
        cout << "\n--- Top Rated Movies ---\n";
        cout << left << setw(6) << "ID" << setw(30) << "Name" << setw(15) << "Genre" << "Rating\n";
        
        for (size_t i = 0; i < sortedMovies.size() && i < 10; i++) {
            displayMovie(sortedMovies[i]);
        }
    }

    void watchMovie() {
        string id;
        cout << "Enter Movie ID to watch: ";
        cin >> id;
        
        if (movieByID.find(id) != movieByID.end()) {
            Movie m = movieByID[id];
            
            recentlyWatched.push(m);
            watchHistory.push(m);
            
            cout << "You are now watching: " << m.name << "\n";
            cout << "Grab some popcorn and enjoy!\n";
        } else {
            cout << "Movie ID not found.\n";
        }
    }

    void viewWatchHistory() {
        if (watchHistory.empty()) {
            cout << "Watch history is empty. Go watch some movies!\n";
            return;
        }
        
        cout << "\n--- Watch History (Oldest to Newest) ---\n";
        cout << left << setw(6) << "ID" << setw(30) << "Name" << setw(15) << "Genre" << "Rating\n";
        
        queue<Movie> temp = watchHistory;
        while (!temp.empty()) {
            displayMovie(temp.front());
            temp.pop();
        }
    }

    void viewRecentlyWatched() {
        if (recentlyWatched.empty()) {
            cout << "No recently watched movies.\n";
            return;
        }
        
        cout << "\n--- Recently Watched (Newest to Oldest) ---\n";
        cout << left << setw(6) << "ID" << setw(30) << "Name" << setw(15) << "Genre" << "Rating\n";
        
        stack<Movie> temp = recentlyWatched;
        while (!temp.empty()) {
            displayMovie(temp.top());
            temp.pop();
        }
    }

    void recommendMovies() {
        string id;
        cout << "Enter a Movie ID you liked to get recommendations: ";
        cin >> id;
        
        if (movieByID.find(id) == movieByID.end()) {
            cout << "Movie ID not found.\n";
            return;
        }
        
        if (recommendationGraph.find(id) != recommendationGraph.end() && !recommendationGraph[id].empty()) {
            cout << "\n--- Recommendations based on '" << movieByID[id].name << "' ---\n";
            cout << left << setw(6) << "ID" << setw(30) << "Name" << setw(15) << "Genre" << "Rating\n";
            
            vector<string>& recs = recommendationGraph[id];
            int count = 0;
            
            for (const string& recId : recs) {
                if (count >= 5) break;
                displayMovie(movieByID[recId]);
                count++;
            }
        } else {
            cout << "No specific recommendations found for this movie yet.\n";
        }
    }
};

int main() {
    MovieManager manager;
    int choice;
    
    do {
        cout << "\n==========================================\n";
        cout << "       MOVIE RECOMMENDATION SYSTEM        \n";
        cout << "==========================================\n";
        cout << "1. Display All Movies\n";
        cout << "2. Search Movie by ID (Binary Search)\n";
        cout << "3. Search Movie by Name\n";
        cout << "4. Search Movies by Genre\n";
        cout << "5. Add New Movie\n";
        cout << "6. Delete Movie\n";
        cout << "7. Update Movie Details\n";
        cout << "8. Display Top Rated Movies\n";
        cout << "9. Watch a Movie\n";
        cout << "10. View Watch History\n";
        cout << "11. View Recently Watched Movies\n";
        cout << "12. Recommend Movies\n";
        cout << "0. Exit\n";
        cout << "==========================================\n";
        cout << "Enter your choice: ";
        
        if (!(cin >> choice)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: manager.displayAllMovies(); break;
            case 2: manager.searchMovieByID(); break;
            case 3: manager.searchMovieByName(); break;
            case 4: manager.searchMoviesByGenre(); break;
            case 5: manager.addNewMovie(); break;
            case 6: manager.deleteMovie(); break;
            case 7: manager.updateMovie(); break;
            case 8: manager.displayTopRatedMovies(); break;
            case 9: manager.watchMovie(); break;
            case 10: manager.viewWatchHistory(); break;
            case 11: manager.viewRecentlyWatched(); break;
            case 12: manager.recommendMovies(); break;
            case 0: cout << "Exiting the system. Goodbye!\n"; break;
            default: cout << "Invalid choice! Please select a valid option from 0 to 12.\n";
        }
    } while (choice != 0);

    return 0;
}
