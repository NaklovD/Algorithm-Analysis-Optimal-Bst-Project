/* ------------------------------------------------------------
   Optimal Binary Search Tree - Dynamic Programming
   Design and Analysis of Algorithms - Term Project

   Derleme : g++ -std=c++11 -O2 -o obst obst.cpp
   Calistirma : ./obst input.txt
   ------------------------------------------------------------ */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <cstdlib>

using namespace std;
using namespace std::chrono;

const int MAX = 205;        // en fazla 200 anahtar
const int REPEAT = 1000;    // sure olcumu icin tekrar sayisi

/* ---------- global veriler ---------- */

int n;                      // anahtar sayisi
long long keys[MAX];        // sirali anahtarlar, 1..n
double prob[MAX];           // arama olasiliklari, 1..n

double cost[MAX][MAX];      // C[i][j] : i..j araligi icin en kucuk maliyet
int root[MAX][MAX];         // R[i][j] : secilen kokun indisi
double pre[MAX];            // onek toplami, aralik toplamini O(1) verir

struct Node {
    long long key;
    double prob;
    Node* left;
    Node* right;
};

/* ---------- sure olcumu ---------- */

/* Verilen baslangic anindan bu yana gecen sureyi milisaniye olarak verir. */
double elapsedMs(high_resolution_clock::time_point start) {
    high_resolution_clock::time_point stop = high_resolution_clock::now();
    return duration<double, milli>(stop - start).count();
}

/* ---------- 1. girdi okuma ---------- */

/* Dosyadan okuma */
bool readFromFile(const char* fileName) {
    ifstream in(fileName);
    if (!in) {
        cout << "Input file not found: " << fileName << endl;
        return false;
    }
    if (!(in >> n)) {
        cout << "Could not read the number of keys." << endl;
        return false;
    }
    if (n < 1 || n > MAX - 5) {
        cout << "n must be between 1 and " << MAX - 5 << "." << endl;
        return false;
    }
    for (int i = 1; i <= n; i++) {
        if (!(in >> keys[i] >> prob[i])) {
            cout << "Missing key/probability pair at line " << i << "." << endl;
            return false;
        }
    }
    return true;
}

/* Klavyeden okuma */
bool readFromKeyboard() {
    cout << "Number of keys (n): ";
    if (!(cin >> n)) {
        cout << "Invalid number." << endl;
        return false;
    }
    if (n < 1 || n > MAX - 5) {
        cout << "n must be between 1 and " << MAX - 5 << "." << endl;
        return false;
    }
    cout << "Enter each key and its probability, separated by a space." << endl;
    cout << "The keys must be given in increasing order." << endl;
    for (int i = 1; i <= n; i++) {
        cout << "  Key " << i << " : ";
        if (!(cin >> keys[i] >> prob[i])) {
            cout << "Invalid value." << endl;
            return false;
        }
    }
    return true;
}

/* Kullaniciya kaynak sectirir */
bool readInput() {
    cout << "Select the input source:" << endl;
    cout << "  1 - read from a file" << endl;
    cout << "  2 - enter the values from the keyboard" << endl;
    cout << "Your choice: ";

    int choice;
    if (!(cin >> choice)) {
        cout << "Invalid choice." << endl;
        return false;
    }
    if (choice == 1) {
        char fileName[200];
        cout << "File name: ";
        cin >> fileName;
        return readFromFile(fileName);
    }
    if (choice == 2) {
        return readFromKeyboard();
    }
    cout << "Choice must be 1 or 2." << endl;
    return false;
}

/* Anahtarlar sirali mi, olasiliklar gecerli mi? */
bool checkInput() {
    for (int i = 2; i <= n; i++) {
        if (keys[i] <= keys[i - 1]) {
            cout << "Keys must be sorted in increasing order. Error at position "
                 << i << "." << endl;
            return false;
        }
    }
    double sum = 0.0;
    for (int i = 1; i <= n; i++) {
        if (prob[i] < 0.0) {
            cout << "Probability cannot be negative. Error at position "
                 << i << "." << endl;
            return false;
        }
        sum += prob[i];
    }
    if (fabs(sum - 1.0) > 1e-4) {
        cout << "Probabilities must sum to 1. Current sum = "
             << fixed << setprecision(6) << sum << endl;
        return false;
    }
    return true;
}

/* ---------- 2. dinamik programlama ---------- */

/* p[i] + ... + p[j] toplamini onek dizisinden okur */
double weight(int i, int j) {
    return pre[j] - pre[i - 1];
}

void computeOBST() {
    /* onek toplamlari */
    pre[0] = 0.0;
    for (int i = 1; i <= n; i++)
        pre[i] = pre[i - 1] + prob[i];

    /* tabloyu temizle: bos aralik maliyeti 0 olsun */
    for (int i = 0; i <= n + 1; i++)
        for (int j = 0; j <= n + 1; j++)
            cost[i][j] = 0.0;

    /* taban durum: tek anahtar */
    for (int i = 1; i <= n; i++) {
        cost[i][i] = prob[i];
        root[i][i] = i;
    }

    /* aralik uzunlugu artarak ilerle */
    for (int len = 2; len <= n; len++) {
        for (int i = 1; i + len - 1 <= n; i++) {
            int j = i + len - 1;
            double best = 1e18;
            int bestRoot = i;

            /* aralikta her anahtari kok olarak dene */
            for (int r = i; r <= j; r++) {
                double value = cost[i][r - 1] + cost[r + 1][j];
                if (value < best) {
                    best = value;
                    bestRoot = r;
                }
            }
            /* aralik bir seviye asagi indi, toplam olasilik kadar ek maliyet */
            cost[i][j] = best + weight(i, j);
            root[i][j] = bestRoot;
        }
    }
}

/* ---------- 3. agac islemleri ---------- */

Node* newNode(int index) {
    Node* node = new Node;
    node->key = keys[index];
    node->prob = prob[index];
    node->left = NULL;
    node->right = NULL;
    return node;
}

/* Kok tablosunu kullanarak optimal agaci geri kurar */
Node* buildOptimalTree(int i, int j) {
    if (i > j) return NULL;
    int r = root[i][j];
    Node* node = newNode(r);
    node->left = buildOptimalTree(i, r - 1);
    node->right = buildOptimalTree(r + 1, j);
    return node;
}

/* Normal BST: anahtarlar verildikleri sirada tek tek eklenir */
Node* insertBST(Node* node, int index) {
    if (node == NULL) return newNode(index);
    if (keys[index] < node->key)
        node->left = insertBST(node->left, index);
    else
        node->right = insertBST(node->right, index);
    return node;
}

Node* buildConventionalTree() {
    Node* r = NULL;
    for (int i = 1; i <= n; i++)
        r = insertBST(r, i);
    return r;
}

/* Beklenen maliyet: her anahtar icin olasilik * karsilastirma sayisi */
double treeCost(Node* node, int level) {
    if (node == NULL) return 0.0;
    return node->prob * (level + 1)
         + treeCost(node->left, level + 1)
         + treeCost(node->right, level + 1);
}

/* Ortalama arama derinligi icin seviyelerin toplami.
   Kokun seviyesi 0 kabul edilir, 5. bolumdeki level sutunu ile ayni. */
double depthSum(Node* node, int level) {
    if (node == NULL) return 0.0;
    return level
         + depthSum(node->left, level + 1)
         + depthSum(node->right, level + 1);
}

int treeHeight(Node* node) {
    if (node == NULL) return -1;
    int h1 = treeHeight(node->left);
    int h2 = treeHeight(node->right);
    if (h1 > h2) return h1 + 1;
    return h2 + 1;
}

void freeTree(Node* node) {
    if (node == NULL) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

/* ---------- 4. yazdirma ---------- */

void printTree(Node* node, int level, const char* side) {
    if (node == NULL) return;
    for (int i = 0; i < level; i++) cout << "    ";
    cout << side << " " << node->key
         << "  (p = " << fixed << setprecision(4) << node->prob << ")" << endl;
    printTree(node->left, level + 1, "L:");
    printTree(node->right, level + 1, "R:");
}

/* Anahtarlar kucukten buyuge cikacak sekilde in-order gezinti */
void printLevels(Node* node, int level) {
    if (node == NULL) return;
    printLevels(node->left, level + 1);
    cout << "  " << setw(10) << node->key
         << setw(14) << fixed << setprecision(4) << node->prob
         << setw(10) << level
         << setw(14) << level + 1 << endl;
    printLevels(node->right, level + 1);
}

void printTables() {
    cout << endl << "Cost table C[i][j]:" << endl;
    cout << "        ";
    for (int j = 1; j <= n; j++) cout << setw(9) << j;
    cout << endl;
    for (int i = 1; i <= n; i++) {
        cout << "  i = " << setw(2) << i << " ";
        for (int j = 1; j <= n; j++) {
            if (j < i) cout << setw(9) << "-";
            else cout << setw(9) << fixed << setprecision(4) << cost[i][j];
        }
        cout << endl;
    }

    cout << endl << "Root table R[i][j] (key at the root):" << endl;
    cout << "        ";
    for (int j = 1; j <= n; j++) cout << setw(9) << j;
    cout << endl;
    for (int i = 1; i <= n; i++) {
        cout << "  i = " << setw(2) << i << " ";
        for (int j = 1; j <= n; j++) {
            if (j < i) cout << setw(9) << "-";
            else cout << setw(9) << keys[root[i][j]];
        }
        cout << endl;
    }
}

void line() {
    cout << "------------------------------------------------------------" << endl;
}

/* ---------- 5. tek bir girdinin incelenmesi ---------- */

void analyse() {
    cout << endl;
    line();
    cout << "1. INPUT KEYS AND PROBABILITIES" << endl;
    line();
    cout << "  n = " << n << endl << endl;
    cout << "  " << setw(8) << "index" << setw(14) << "key"
         << setw(14) << "probability" << endl;
    double sum = 0.0;
    for (int i = 1; i <= n; i++) {
        cout << "  " << setw(8) << i << setw(14) << keys[i]
             << setw(14) << fixed << setprecision(4) << prob[i] << endl;
        sum += prob[i];
    }
    cout << "  " << setw(8) << " " << setw(14) << "total"
         << setw(14) << fixed << setprecision(4) << sum << endl;

    /* DP tablosu kurulur ve suresi olculur */
    high_resolution_clock::time_point start = high_resolution_clock::now();
    for (int t = 0; t < REPEAT; t++) computeOBST();
    double dpTime = elapsedMs(start) / REPEAT;

    cout << endl;
    line();
    cout << "2. DYNAMIC PROGRAMMING TABLES" << endl;
    line();
    printTables();

    cout << endl;
    line();
    cout << "3. MINIMUM EXPECTED SEARCH COST" << endl;
    line();
    cout << "  C[1][" << n << "] = " << fixed << setprecision(6)
         << cost[1][n] << endl;

    Node* optimal = buildOptimalTree(1, n);

    cout << endl;
    line();
    cout << "4. STRUCTURE OF THE OPTIMAL BST" << endl;
    line();
    printTree(optimal, 0, "Root:");

    cout << endl;
    line();
    cout << "5. LEVEL OF EACH KEY" << endl;
    line();
    cout << "  " << setw(10) << "key" << setw(14) << "probability"
         << setw(10) << "level" << setw(14) << "comparisons" << endl;
    printLevels(optimal, 0);

    cout << endl;
    line();
    cout << "6. VERIFICATION" << endl;
    line();
    double fromTree = treeCost(optimal, 0);
    cout << "  Cost from the DP table    : " << fixed << setprecision(6)
         << cost[1][n] << endl;
    cout << "  Cost from the built tree  : " << fixed << setprecision(6)
         << fromTree << endl;
    cout << "  Difference                : " << scientific << setprecision(3)
         << fabs(fromTree - cost[1][n]) << endl;
    if (fabs(fromTree - cost[1][n]) < 1e-9)
        cout << "  Result                    : MATCH" << endl;
    else
        cout << "  Result                    : MISMATCH" << endl;

    /* Normal BST kurulur ve suresi olculur.
       Agaci silme islemi olcumun disinda tutulur, cunku burada sadece
       kurma suresi karsilastiriliyor. */
    double bstTime = 0.0;
    for (int t = 0; t < REPEAT; t++) {
        start = high_resolution_clock::now();
        Node* temp = buildConventionalTree();
        bstTime += elapsedMs(start);
        freeTree(temp);
    }
    bstTime = bstTime / REPEAT;

    Node* conventional = buildConventionalTree();

    cout << endl;
    line();
    cout << "7. COMPARISON WITH A CONVENTIONAL BST" << endl;
    line();
    cout << "  The conventional tree is built by inserting the keys one by one"
         << endl;
    cout << "  in the given order." << endl << endl;

    double costOpt = treeCost(optimal, 0);
    double costCon = treeCost(conventional, 0);
    double avgOpt = depthSum(optimal, 0) / n;
    double avgCon = depthSum(conventional, 0) / n;

    cout << "  " << setw(28) << " " << setw(16) << "Optimal BST"
         << setw(18) << "Conventional BST" << endl;
    cout << "  " << setw(28) << "Expected search cost"
         << setw(16) << fixed << setprecision(4) << costOpt
         << setw(18) << fixed << setprecision(4) << costCon << endl;
    cout << "  " << setw(28) << "Average level (root = 0)"
         << setw(16) << fixed << setprecision(4) << avgOpt
         << setw(18) << fixed << setprecision(4) << avgCon << endl;
    cout << "  " << setw(28) << "Tree height"
         << setw(16) << treeHeight(optimal)
         << setw(18) << treeHeight(conventional) << endl;
    cout << "  " << setw(28) << "Construction time (ms)"
         << setw(16) << fixed << setprecision(6) << dpTime
         << setw(18) << fixed << setprecision(6) << bstTime << endl;

    if (costCon > 0.0) {
        cout << endl << "  The optimal tree is "
             << fixed << setprecision(2)
             << (1.0 - costOpt / costCon) * 100.0
             << " % cheaper." << endl;
    }

    cout << endl << "  Structure of the conventional BST:" << endl;
    printTree(conventional, 0, "Root:");

    freeTree(optimal);
    freeTree(conventional);
}

/* ---------- 6. deneysel analiz ---------- */

/* n anahtarlik rastgele bir girdi uretir, olasiliklari 1 e normalize eder */
void makeRandomInput(int size) {
    n = size;
    double total = 0.0;
    for (int i = 1; i <= n; i++) {
        keys[i] = i * 10;
        prob[i] = (rand() % 1000 + 1) / 1000.0;
        total += prob[i];
    }
    for (int i = 1; i <= n; i++)
        prob[i] = prob[i] / total;
}

void runExperiments() {
    int sizes[5] = {5, 10, 20, 50, 100};

    cout << endl;
    line();
    cout << "8. EXPERIMENTAL ANALYSIS" << endl;
    line();
    cout << "  Each size is executed " << REPEAT
         << " times and the average is reported." << endl << endl;

    cout << "  " << setw(8) << "n" << setw(20) << "Execution time (ms)"
         << setw(22) << "Min expected cost" << setw(16) << "Time ratio" << endl;

    double previous = 0.0;
    srand(42);      // ayni sonuclar tekrar uretilebilsin diye sabit tohum

    for (int s = 0; s < 5; s++) {
        makeRandomInput(sizes[s]);

        high_resolution_clock::time_point start = high_resolution_clock::now();
        for (int t = 0; t < REPEAT; t++) computeOBST();
        double ms = elapsedMs(start) / REPEAT;

        cout << "  " << setw(8) << sizes[s]
             << setw(20) << fixed << setprecision(6) << ms
             << setw(22) << fixed << setprecision(6) << cost[1][n];
        if (previous > 0.0)
            cout << setw(15) << fixed << setprecision(2) << ms / previous << "x";
        else
            cout << setw(16) << "-";
        cout << endl;
        previous = ms;
    }

    cout << endl;
    cout << "  Theoretical complexity: time O(n^3), space O(n^2)." << endl;
    cout << "  When n is doubled the running time grows by about 8 times." << endl;
}

/* ---------- 7. ana program ---------- */

int main(int argc, char** argv) {
    bool ok;

    if (argc > 1) {
        /* Dosya adi komut satirindan verilmisse dogrudan okunur */
        ok = readFromFile(argv[1]);
        if (ok) cout << "Input file: " << argv[1] << endl;
    } else {
        /* Argüman yoksa kullaniciya sorulur */
        ok = readInput();
    }

    if (!ok) return 1;
    if (!checkInput()) return 1;

    analyse();
    runExperiments();

    return 0;
}
