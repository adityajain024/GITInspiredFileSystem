#include<iostream>
#include<ctime>
#include<string>
#include<vector>
#include<sstream>

using namespace std;

class TreeNode{
    public:
    int version_id;
    string content;
    string message; // Empty if not a snapshot
    time_t created_timestamp;
    time_t snapshot_timestamp; // Null if not a snapshot
    TreeNode* parent;
    vector<TreeNode*> children;

    //functions
    //constructor
    TreeNode(){
        created_timestamp = time(nullptr);
        content = "";
        message = "";
        snapshot_timestamp = NULL;
        parent = NULL;
    }

    void SetVersionId(int n){
        version_id = n;
    }

    void SetContent(string c){
        content = c;
    }

    void SetMessage(string m){
        message = m;
    }

    string GetContent(){
        return content;
    }

    void SetParent(TreeNode* p){
        this -> parent = p;
        (p -> children).push_back(this);
    }

    bool isSnapshot(){
        if(snapshot_timestamp!=NULL){
            return true;
        }
        else{
            return false;
        }
    }

    ~TreeNode(){

    }


};

struct NodeInt {
    int key;
    TreeNode* value;
    NodeInt* next;

    NodeInt(int k, TreeNode* v) {
        key = k;
        value = v;
        next = nullptr;
    }
};

class VersionMap {
private:
    int capacity;
    vector<NodeInt*> table;

    int hashFunction(int key) {
        return key % capacity;  
    }

public:
    VersionMap(int size = 10001) {
        capacity = size;
        table = vector<NodeInt*>(capacity, nullptr); 
    }

    void insert(int key, TreeNode* value) {
        int index = hashFunction(key);
        NodeInt* head = table[index];

        for (NodeInt* cur = head; cur != nullptr; cur = cur->next) {
            if (cur->key == key) {
                cur->value = value;
                return;
            }
        }

        NodeInt* newNodeInt = new NodeInt(key, value);
        newNodeInt->next = head;
        table[index] = newNodeInt;
    }

    bool get(int key, TreeNode* &value) {
        int index = hashFunction(key);
        NodeInt* head = table[index];

        for (NodeInt* cur = head; cur != nullptr; cur = cur->next) {
            if (cur->key == key) {
                value = cur->value;
                return true;
            }
        }
        return false;
    }

    
};

 // File Structure
class File{
    public:
    TreeNode* root; // Your implementation of the tree
    TreeNode* active_version;
    //map<int, TreeNode*> version_map; // Your implementation of the HashMap
    VersionMap version_map;
    int total_versions;

    //modified timestamp 
    time_t modified_timestamp;
    //functions

    //index for heap of recent files
    int recent_index;

    //index for heap of biggest trees
    int biggest_trees_index;

    //name

    string name;
    
    //Create
    File(string filename){
        total_versions = 1;
        TreeNode* S = new TreeNode;
        root = S;
        S -> SetVersionId(0);
        S -> SetContent("");
        S -> SetMessage("Start");
        active_version = S;
        //version_map[0] = S;
        version_map.insert(0,S);
        root -> created_timestamp = time(nullptr);
        root -> snapshot_timestamp = time(nullptr);
        modified_timestamp = time(nullptr);

        name = filename;
    }

    //Read

    void Read(){
        cout << active_version -> GetContent() << endl;
    }

    //Insert

    void Insert(string contentt){
        if(active_version -> isSnapshot() == false){
            active_version -> content = active_version -> content + contentt;
        }
        else{
            TreeNode* temp = new TreeNode;
            temp -> content = active_version -> content + contentt;
            // temp -> SetParent(active_version);
            // temp -> parent = active_version;
            temp -> parent = active_version;
            (active_version -> children).push_back(temp);
            active_version = temp;
            total_versions++;
            //version_map[total_versions-1] = active_version;
            version_map.insert(total_versions-1, active_version);
            active_version -> version_id = total_versions -1;
        }
        modified_timestamp = time(nullptr);
    }

    //Update

    void Update(string contentt){
        if(active_version -> isSnapshot() == false){
            active_version -> content = contentt;
        }
        else{
            TreeNode* temp = new TreeNode;
            temp -> content = contentt;
            // temp -> SetParent(active_version);
            // temp -> parent = active_version;
            temp -> parent = active_version;
            (active_version -> children).push_back(temp);
            active_version = temp;
            total_versions++;
            //version_map[total_versions-1] = active_version;
            version_map.insert(total_versions-1, active_version);
            active_version -> version_id = total_versions -1;
        }
        modified_timestamp = time(nullptr);
    }

    //Snapshot

    void Snapshot(string m){
        if(active_version -> snapshot_timestamp!=NULL){
            cout << "It is already a snapshot." << endl;
            return;
        }
        active_version -> snapshot_timestamp = time(nullptr);
        active_version -> message = m;
        cout << "Snapshotted file: " << name << ", with message: " << m << endl;
    }

    //Rollback (with and without id)

    void Rollback(){
        if(active_version -> parent == NULL){
            cout << "Parent doesnt exist. " << endl;
            return;
        }
        active_version = active_version -> parent;
        cout << "Rollback to parent done." << endl;
    }

    void Rollback(int id){
        if(id>=total_versions or id<0){
            cout << "Error: Version ID given is not valid" << endl;
            return; 
        }
        //active_version = version_map[id];
        bool temp = version_map.get(id, active_version);
        cout << "Rollback to version id - " << id << " done." << endl;
    }

    //History

    void History(){
        vector<TreeNode*> h;
        TreeNode* curr = active_version;
        while(curr!=NULL){
            if(curr -> isSnapshot()){
                h.push_back(curr);
            }
            curr = curr -> parent;
        }
        //reversing
        for(int i = 0; i<h.size()/2; i++){
            swap(h[i], h[h.size() - 1 - i]);
        }

        for(int i = 0; i<h.size(); i++){
            cout << "Id: " << h[i]->version_id << ", ";
            cout << "Snapshot Timestamp: ";
            tm* SnapTime = localtime(&((h[i] -> snapshot_timestamp)));
            cout << asctime(SnapTime);
            cout << ", ";
            cout << "Message: " << h[i] -> message;
            cout << endl;
        }
    }
    
};

struct NodeStr {
    string key;

    File* value;
    NodeStr* next;

    NodeStr(string k, File* v) {
        key = k;
        value = v;
        next = nullptr;
    }
};

class Filemap {
public:
    int capacity;
    vector<NodeStr*> table;

    int hashFunction(string &key) {
        unsigned long hash = 0;
        int p = 31;
        unsigned long p_pow = 1;

        for (char c : key) {
            hash = (hash + (c - 'a' + 1) * p_pow) % capacity;
            p_pow = (p_pow * p) % capacity;
        }
        return hash % capacity;
    }


    Filemap(int size = 10001) {
        capacity = size;
        table = vector<NodeStr*>(capacity, nullptr);
    }

    void insert(string &key, File* value) {
        int index = hashFunction(key);
        NodeStr* head = table[index];


        for (NodeStr* cur = head; cur != nullptr; cur = cur->next) {
            if (cur->key == key) {
                cur->value = value;
                return;
            }
        }

        
        NodeStr* newNodeStr = new NodeStr(key, value);
        newNodeStr->next = head;
        table[index] = newNodeStr;
    }

    bool get(string &key, File* &value) {
        int index = hashFunction(key);
        NodeStr* head = table[index];

        for (NodeStr* cur = head; cur != nullptr; cur = cur->next) {
            if (cur->key == key) {
                value = cur->value;
                return true;
            }
        }
        return false;
    }

    
};



class RecentHeap{

    public: 

    RecentHeap(){

    }

    vector<File*> recentheap;
    

    int parent(int i){
        return (i-1)/2;
    }
    int left(int i){
        return (2*i+1);
    }
    int right(int i){
        return (2*i+2);
    }

    void HeapifyUp(int i){
        while(i>0 and recentheap[i] -> modified_timestamp > recentheap[parent(i)] -> modified_timestamp){
            swap(recentheap[i], recentheap[parent(i)]);
            recentheap[i] -> recent_index = (i);
            recentheap[parent(i)] -> recent_index = parent(i);
            i = parent(i);
        }
        
    }

    void push(File* a){
        recentheap.push_back(a);
        a -> recent_index = recentheap.size()-1;
        HeapifyUp(a -> recent_index);
    }

    void modifyUp(File* a){
        // a -> value = val;
        int i = a -> recent_index;
        // swap(recentheap[i], recentheap[0]);
        // recentheap[i] -> recent_index = i;
        // recentheap[0] -> recent_index = 0;

        HeapifyUp(i);
    }

    void HeapifyDown(int i){
        if(2*i + 1 >= recentheap.size()){
            return;
        }
        if(2*i+2 == recentheap.size() and recentheap[i] -> modified_timestamp < recentheap[left(i)] -> modified_timestamp){
            swap(recentheap[i], recentheap[left(i)]);
            recentheap[i] -> recent_index = i;
            recentheap[left(i)] -> recent_index = left(i);
            i = left(i);
            HeapifyDown(i);
            return;
        }
        else if(2*i+2 == recentheap.size() and recentheap[i] -> modified_timestamp >= recentheap[left(i)] -> modified_timestamp){
            return;
        }
        if(recentheap[i] -> modified_timestamp >= max(recentheap[left(i)] -> modified_timestamp,recentheap[right(i)] -> modified_timestamp)){
            return;
        }
        else{
            if(recentheap[left(i)]-> modified_timestamp > recentheap[right(i)] -> modified_timestamp){
                swap(recentheap[i], recentheap[left(i)]);
                recentheap[i] -> recent_index = i;
                recentheap[left(i)] -> recent_index = left(i);
                i = left(i);
                HeapifyDown(i);
                return; 
            }
            else{
                swap(recentheap[i], recentheap[right(i)]);
                recentheap[i] -> recent_index = i;
                recentheap[right(i)] -> recent_index = right(i);
                i = right(i);
                HeapifyDown(i);
                return;
            }
        }
    }

    void pop(){
        swap(recentheap[0], recentheap[recentheap.size() -1]);
        recentheap[0] -> recent_index = 0;
        recentheap[recentheap.size() - 1] -> recent_index = recentheap.size() -1;
        recentheap.pop_back();
        HeapifyDown(0);
    }

    File* top(){
        // if(empty()){
        //     return -1;
        // }
        return recentheap[0];
    }

    void print(){
        cout << recentheap.size() << endl;
        for(int i = 0; i<recentheap.size(); i++){
            cout << recentheap[i] -> name << " " << recentheap[i] -> modified_timestamp << endl;
        }
    }

    bool isEmpty(){
        if (recentheap.size() == 0){
            return true;
        }
        else{
            return false;

        }
    }
    
};

class BiggestHeap{
    public: 

    BiggestHeap(){

    }

    vector<File*> biggestTreesHeap;
    

    int parent(int i){
        return (i-1)/2;
    }
    int left(int i){
        return (2*i+1);
    }
    int right(int i){
        return (2*i+2);
    }

    void HeapifyUp(int i){
        while(i>0 and biggestTreesHeap[i] -> total_versions > biggestTreesHeap[parent(i)] -> total_versions){
            swap(biggestTreesHeap[i], biggestTreesHeap[parent(i)]);
            biggestTreesHeap[i] -> biggest_trees_index = (i);
            biggestTreesHeap[parent(i)] -> biggest_trees_index = parent(i);
            i = parent(i);
        }
        
    }

    void push(File* a){
        biggestTreesHeap.push_back(a);
        a -> biggest_trees_index = biggestTreesHeap.size()-1;
        HeapifyUp(a -> biggest_trees_index);
    }

    void modifyUp(File* a){
        int i = a -> biggest_trees_index;
        HeapifyUp(i);
    }

    void HeapifyDown(int i){
        if(2*i + 1 >= biggestTreesHeap.size()){
            return;
        }
        if(2*i+2 == biggestTreesHeap.size() and biggestTreesHeap[i] -> total_versions < biggestTreesHeap[left(i)] -> total_versions){
            swap(biggestTreesHeap[i], biggestTreesHeap[left(i)]);
            biggestTreesHeap[i] -> biggest_trees_index = i;
            biggestTreesHeap[left(i)] -> biggest_trees_index = left(i);
            i = left(i);
            HeapifyDown(i);
            return;
        }
        else if(2*i+2 == biggestTreesHeap.size() and biggestTreesHeap[i] -> total_versions >= biggestTreesHeap[left(i)] -> total_versions){
            return;
        }
        if(biggestTreesHeap[i] -> total_versions >= max(biggestTreesHeap[left(i)] -> total_versions,biggestTreesHeap[right(i)] -> total_versions)){
            return;
        }
        else{
            if(biggestTreesHeap[left(i)]-> total_versions > biggestTreesHeap[right(i)] -> total_versions){
                swap(biggestTreesHeap[i], biggestTreesHeap[left(i)]);
                biggestTreesHeap[i] -> biggest_trees_index = i;
                biggestTreesHeap[left(i)] -> biggest_trees_index = left(i);
                i = left(i);
                HeapifyDown(i);
                return; 
            }
            else{
                swap(biggestTreesHeap[i], biggestTreesHeap[right(i)]);
                biggestTreesHeap[i] -> biggest_trees_index = i;
                biggestTreesHeap[right(i)] -> biggest_trees_index = right(i);
                i = right(i);
                HeapifyDown(i);
                return;
            }
        }
    }

    void pop(){
        swap(biggestTreesHeap[0], biggestTreesHeap[biggestTreesHeap.size() -1]);
        biggestTreesHeap[0] -> biggest_trees_index = 0;
        biggestTreesHeap[biggestTreesHeap.size() - 1] -> biggest_trees_index = biggestTreesHeap.size() -1;
        biggestTreesHeap.pop_back();
        HeapifyDown(0);
    }

    File* top(){
        return biggestTreesHeap[0];
    }

    void print(){
        cout << biggestTreesHeap.size() << endl;
        for(int i = 0; i<biggestTreesHeap.size(); i++){
            cout << biggestTreesHeap[i] -> name << " " << biggestTreesHeap[i] -> total_versions << endl;
        }
    }

    bool isEmpty(){
        if (biggestTreesHeap.size() == 0){
            return true;
        }
        else{
            return false;
            
        }
    }
};

void Recent_Files(RecentHeap& recent_files_stack, int num){
    vector<File*> temp;
    for(int i = 1; i<=num; i++){
        if(recent_files_stack.isEmpty()){
            break;
        }
        else{
            cout << i << " - " << recent_files_stack.top() -> name << endl;
            temp.push_back(recent_files_stack.top());
            recent_files_stack.pop();
        }
    }
    for(int i = 0; i<temp.size(); i++){
        recent_files_stack.push(temp[i]);
    }
}

void Biggest_Trees(BiggestHeap& biggest_trees_stack, int num){
    vector<File*> temp;
    for(int i = 1; i<=num; i++){
        if(biggest_trees_stack.isEmpty()){
            break;
        }
        else{
            cout << i << " - " << biggest_trees_stack.top() -> name << endl;
            temp.push_back(biggest_trees_stack.top());
            biggest_trees_stack.pop();
        }
    }
    for(int i = 0; i<temp.size(); i++){
        biggest_trees_stack.push(temp[i]);
    }
}


int main(){
    RecentHeap recent_files_stack;
    RecentHeap* recent_files = &(recent_files_stack);

    BiggestHeap biggest_trees_stack;
    BiggestHeap* biggest_trees = &(biggest_trees_stack);

    //map<string, File*> filemap;
    Filemap filemap;

    // File* A = new File("A");
    // recent_files -> push(A);

    // File* B = new File("B");
    // recent_files -> push(B);

    // File* C = new File("C");
    // recent_files -> push(C);

    // recent_files -> print();

    // string s;
    // cin >> s;

    string sen;

    while(getline(cin, sen)){
        
        stringstream ss(sen);

        string command;
        ss >> command;

        // string filename;
        // ss >> filename;
        // string content;
        // getline(ss, content);

        //normal file functions
        //filemap[filename] 
        // File* tempF = NULL;
        // bool chec = filemap.get(filename, tempF);

        if(command == "CREATE"){
            string filename;
            ss >> filename;
            if(filename == ""){
                cout << "Error: No filename given." << endl;
            }
            else{
                File* tempF = NULL;
                if(filemap.get(filename, tempF)){
                    cout << "A file with this filename already exists." << endl;
                }
                else{
                    File* A = new File(filename);
                    recent_files -> push(A);
                    filemap.insert(filename, A);
                    // filemap[filename] = A;
                    biggest_trees -> push(A);
                    cout << "Created: " << filename << endl;
                }    
            }  
        }

        else if(command == "READ"){
            string filename;
            ss >> filename;
            if(filename == ""){
                cout << "Error: No filename given." << endl;
            }
            else{
                File* tempF = NULL;
                if(filemap.get(filename, tempF) == false){
                    cout << "No such file exists." << endl;
                }
                else{
                    cout << tempF -> active_version -> content << " - Content" << endl;
                }
            }
        }

        else if(command == "INSERT"){
            string filename;
            ss >> filename;
            if(filename == ""){
                cout << "Error: No filename given." << endl;
            }
            else{
                File* tempF = NULL;
                if(filemap.get(filename, tempF) == false){
                    cout << "No such file exists." << endl;
                }
                else{
                    string content;
                    getline(ss, content);
                    if(content == ""){
                        cout << "Error: No content given." << endl;
                    }
                    else{
                        content.erase(0,1);

                        int total = tempF -> total_versions;
                    
                        tempF -> Insert(content);

                        recent_files -> modifyUp(tempF);

                        if(tempF -> total_versions > total){
                            biggest_trees -> modifyUp(tempF);
                        }
                        cout << "Content inserted: *" << content << "*" << ", in file: " << filename << endl;
                    }
                }
            }
        }
        
        else if(command == "UPDATE"){
            string filename;
            ss >> filename;
            if(filename == ""){
                cout << "Error: No filename given." << endl;
            }
            else{
                File* tempF = NULL;
                if(filemap.get(filename, tempF) == false){
                    cout << "No such file exists." << endl;
                }
                else{
                    string content;
                    getline(ss, content);
                    if(content == ""){
                        cout << "Error: No content given." << endl;
                    }
                    else{
                        content.erase(0,1);

                        int total = tempF -> total_versions;
                    
                        tempF -> Update(content);
                        recent_files -> modifyUp(tempF);

                        if(tempF -> total_versions > total){
                            //cout << "Currently Updating biggest trees" << endl;
                            biggest_trees -> modifyUp(tempF);
                        }
                        cout << "Content updated: *" << content << "*" << ", in file: " << filename << endl;
                    }
                }
            }
        }

        else if(command == "SNAPSHOT"){
            string filename;
            ss >> filename;
            if(filename == ""){
                cout << "Error: No filename given." << endl;
            }
            else{
                File* tempF = NULL;
                if(filemap.get(filename, tempF) == false){
                    cout << "No such file exists." << endl;
                }
                else{
                    string message;
                    getline(ss, message);
                    if(message == ""){
                        cout << "Error: No message given." << endl;
                    }
                    else{
                        message.erase(0,1);
                        tempF -> Snapshot(message);
                    }
                }
            }
        }     
        
        else if(command == "ROLLBACK"){
            string filename;
            ss >> filename;
            if(filename == ""){
                cout << "Error: No filename given." << endl;
            }
            else{
                File* tempF = NULL;
                if(filemap.get(filename, tempF) == false){
                    cout << "No such file exists." << endl;
                }
                else{
                    string content;
                    getline(ss, content);
                    if(content == ""){
                        tempF -> Rollback();
                        
                    }
                    else{
                        content.erase(0,1);
                        stringstream ss(content);
                        int i;
                        ss >> i;               
                    
                        tempF -> Rollback(i);
                    }
                }
            }
        }

        else if(command == "HISTORY"){
            string filename;
            ss >> filename;
            if(filename == ""){
                cout << "Error: No filename given." << endl;
            }
            else{
                File* tempF = NULL;
                if(filemap.get(filename, tempF) == NULL){
                    cout << "No such file exists." << endl;
                }
                else{
                    tempF -> History();
                }
            }
        }

        //system wide analytics
        
        else if(command == "RECENT_FILES"){
            int i;
            ss >> i;
            //cout << "i is:" << i << endl;
            if(i<=0){
                cout << "Error: Number input needs to be positive." << endl;
                
            }
            else{
                Recent_Files(recent_files_stack, i);
            }
        }

        else if(command == "BIGGEST_TREES"){
            int i;
            ss >> i;
            //cout << "i is:" << i << endl;
            if(i<=0){
                cout << "Error: Number input needs to be positive." << endl;
            }
            else{
                Biggest_Trees(biggest_trees_stack, i);
            }
        }

        // else if(command == "TOTAL"){
        //     string filename;
        //     ss >> filename;

        //     cout << filemap[filename] -> total_versions << endl;
        // }
        else{
            cout << "Error: The command is invalid." << endl;
        }
    }
       

}