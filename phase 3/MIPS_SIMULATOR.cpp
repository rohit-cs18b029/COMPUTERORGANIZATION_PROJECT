#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<math.h>
using namespace std;
map<int,int> mem;
map<string,int> labels;
map<string,int> res;
int s1,s2,b1,b2,asso;
int temp=0;
int ins_count=0;
int pc=0;
float no_of_cycles=0;
float no_of_stalls=0;
float no_of_hits=0;
float no_of_miss=0;
//LRU////////////////////////////////////////////
class Node{
    public:
    int address;
    int data;
    Node *next;
    Node(int a,int d){
        address=a;
        data=d;
        next=NULL;
    }
};
class LRU{
    public:
    int capacity;
    int block_size;
    int *lastindex=new int[asso];
    Node **head=new Node*[asso];
    LRU(){
        for(int i=0;i<asso;i++){
            lastindex[i]=-1;
            head[i]=NULL;
        }
        capacity=0;
    }
    void listSize(int);
    void setItem(int,int);
    int getItem(int);
    bool isFull(int);
    void printItem();
    void updateList(int,int);
    void editItem(int,int);
};
void LRU::listSize(int c){
    capacity=c;
}
void LRU::setItem(int a,int d){
    int rem=a%asso;
    if(lastindex[rem]==capacity-1){
        cout<<"overflow"<<endl;
    }
    else{
        lastindex[rem]++;
        Node *temp=head[rem];
        if(temp==NULL){
            head[rem]=new Node(a,d);
        }
        else{
            while(temp->next!=NULL){
                temp=temp->next;
            }
            temp->next=new Node(a,d);
        }
    }
}
int LRU::getItem(int a){
    int rem=a%asso;
    Node *temp=head[rem];
    while(temp!=NULL){
        if(temp->address==a){
            int d=temp->data;
            while(temp->next!=NULL){
                temp->address=temp->next->address;
                temp->data=temp->next->data;
                temp=temp->next;
            }
            temp->address=a;
            temp->data=d;
            return d;
        }
        else{
            temp=temp->next;
        }
    }
    return 0;
}
void LRU::updateList(int a,int d){
    int rem=a%asso;
    Node *temp=head[rem];
    while(temp->next!=NULL){
        temp->address=temp->next->address;
        temp->data=temp->next->data;
        temp=temp->next;
    }
    temp->address=a;
    temp->data=d;
}

bool LRU::isFull(int rem){
    if((lastindex[rem]+1)==capacity){
        return true;
    }
    return false;
}
void LRU::printItem(){
    int i=0;
    while(i<asso){
        Node *temp=head[i];
        while(temp!=NULL){
            cout<<temp->data<<" ";
            temp=temp->next;
        }
        i++;
        cout<<endl;
    }
}
void LRU::editItem(int a,int d){
    int rem=a%asso;
    Node *temp=head[rem];
    while(temp!=NULL){
        if(temp->address==a){
            while(temp->next!=NULL){
                temp->address=temp->next->address;
                temp->data=temp->next->data;
                temp=temp->next;
            }
            temp->address=a;
            temp->data=d;
            break;
        }
        else{
            temp=temp->next;
        }
    }
}
/////end/////////////////////////////////////////////////
///cache//////////////////////////////////////////////
int cache(int i,LRU *L1,LRU *L2){
int d;
while(true){
        d=L1->getItem(i);
        if(d!=0){
            no_of_hits++;
            return d;
        }
        else{
            no_of_miss++;
            no_of_cycles++;
            no_of_stalls++;
            cout<<"stall from L1"<<endl;
            d=L2->getItem(i);
            if(d!=0){
                if(!L1->isFull(i%asso)){
                    for(int l=0;l<L1->block_size;l++){
                        L1->setItem(i+l,L2->getItem(i+l));
                    }                  
                }
                else{
                    for(int l=0;l<L1->block_size;l++){
                        L1->updateList(i+l,L2->getItem(i+l));
                    }                  
                }
            }
            else{
                cout<<"stall from L2"<<endl;
                no_of_cycles++;
                no_of_stalls++;
                if(!L2->isFull(i%asso)){
                    for(int l=0;l<L2->block_size;l++){
                        L2->setItem(i+l,mem[i+l]);
                    }                  
                }
                else{
                    for(int l=0;l<L2->block_size;l++){
                        L2->updateList(i+l,mem[i+l]);
                    }                  
                }
                if(!L1->isFull(i%asso)){
                    for(int l=0;l<L1->block_size;l++){
                        L1->setItem(i+l,mem[i+l]);
                    }  
                }
                else{
                    for(int l=0;l<L1->block_size;l++){
                        L1->updateList(i+l,mem[i+l]);
                    }  
                }
            }
        }
    }
}
/////end///////////////////////////////////////////
/////read input file////////////////////////////////////////////////
void get_input(){
    string flag,str;
    int n;
    ifstream read;
    read.open("input.text");
    while(true){
        getline(read,flag);
        n=flag.find(':');
        str=flag.substr(0,n);
        if(str=="size of L1"){
            s1=stoi(flag.substr(n+1,flag.length()));
        }
        if(str=="size of L2"){
            s2=stoi(flag.substr(n+1,flag.length()));
        }
        if(str=="block size of L1"){
            b1=stoi(flag.substr(n+1,flag.length()));
        }
        if(str=="block size of L2"){
            b2=stoi(flag.substr(n+1,flag.length()));
        }
        if(str=="associativity"){
            asso=stoi(flag.substr(n+1,flag.length()));
            break;
        }
    }
}
void set_input(LRU *L1,LRU *L2){
        L1->listSize(s1);
        L2->listSize(s2);
        L1->block_size=b1;
        L2->block_size=b2;
}
///end/////////////////////////////////////////////
class Instructions{
    public:
    string r[4];
    Instructions(string rs[]){
        for(int i=0;i<4;i++){
            r[i]=rs[i];
        }
    }
};

void res_init(){
    string r[32]={"r0","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","s8","ra"};
    for(int i=0;i<32;i++){
        res[r[i]]=0;
    }
    res["zero"]=0;
}

string remove_char(string line,char ch){
    int n;
    while(true){
        n=line.find(ch);
        if(n!=-1){
            line=line.replace(n,1,"");
        }
        else{
            break;
        }
    }
    return line;
}

string remove_haze(string line){
    int n=line.find('#');
    line=line.substr(0,n);
    return line;
}

void R_type(string line,string s[]){
    line=remove_char(line,' ');
    line=remove_char(line,',');
    int n;
    int start=0;
    for(int i=0;i<3;i++){
        n=line.find('$');
        s[i]=line.substr(start,n-start);
        start=n;
        line=line.replace(n,1,"");
    }
    s[3]=line.substr(n,line.size());
}

void I_type(string line,string s[]){
    line=remove_char(line,' ');
    int n;
    int start=0;
    n=line.find('$');
    s[0]=line.substr(start,n);
    start=n;
    line=line.replace(n,1,"");
    n=line.find(',');
    s[1]=line.substr(start,n-start);
    start=n;
    line=line.replace(n,1,"");
    n=line.find('(');
    s[3]=line.substr(start,n-start);
    start=n;
    line=line.replace(n,2,"");
    n=line.find(')');
    s[2]=line.substr(start,n-start);
}

void Bne_type(string line,string s[]){
    line=remove_char(line,' ');
    int n;
    int start=0;
    n=line.find('$');
    s[0]=line.substr(start,n);
    start=n;
    line=line.replace(n,1,"");
    n=line.find(',');
    s[1]=line.substr(start,n-start);
    start=n;
    line=line.replace(n,2,"");
    n=line.find(',');
    s[2]=line.substr(start,n-start);
    start=n;
    line=line.replace(n,1,"");
    n=line.size();
    s[3]=line.substr(start,n-start+1);
}

void J_type(string line,string s[]){
    line=remove_char(line,' ');
    s[0]=line.substr(0,1);
    line=line.replace(0,1,"");
    s[1]=line;
    s[2]="";
    s[3]="";
}

void L_type(string line,string s[],int i){
    int n=line.find(':');
    s[0]="level";
    line=line.substr(0,n);
    s[1]=line;
    s[2]="";
    s[3]="";
    labels[line]=i;
}

void get_instruction(string str[]){
    string flag;
    int n,index=0;
    ifstream read;
    read.open("bb.s");
    while(true){
        ins_count++;
        getline(read,flag);
        str[index]=flag;
        str[index]=remove_haze(str[index]);
        index++;
        if(flag=="jr $ra"){
            break;
        }
    }
}

vector<Instructions> get_blocks(string str[]){
    int n;
    string flag;
    string s[4];
    vector<Instructions> loi;
    Instructions *temp;
    for(int i=0;i<ins_count+3;i++){
        n=str[i].find(' ');
        flag=str[i].substr(0,n);
        if(flag=="add" || flag=="sub" || flag=="slt"){
            R_type(str[i],s);
        }
        else if(flag=="addi" || flag=="beq" || flag=="bne" || flag=="sll"){
            Bne_type(str[i],s);
        }
        else if(flag=="lw" || flag=="sw"){
            I_type(str[i],s);
        }
        else if(flag=="j"){
            J_type(str[i],s);
        }
        else if(flag.find(':')!=-1){
            L_type(str[i],s,i);
        }
        else if(flag==".data" || flag==".text" || flag==".globl"){
            s[0]=flag;
            s[1]="";
            s[2]="";
            s[3]="";
        }
        else if(flag==".word"){
            s[0]=flag;
            flag=str[i].replace(0,n+1,"");
            s[1]=remove_char(flag,' ');
            s[2]="";
            s[3]="";
        }
        else if(flag=="li"){
            s[0]=flag;
            flag=str[i].replace(0,n+2,"");
            n=flag.find(',');
            s[1]=flag.substr(0,n);
            flag=flag.replace(0,n+3,"");
            s[2]=flag;
            s[3]="";
        }
        else{
            s[0]="";
            s[1]="";
            s[2]="";
            s[3]="";
        }
        temp=new Instructions(s);
        loi.push_back(*temp);
    }
    return loi;
}
//list of instructions
void display_loi(vector<Instructions> loi){
    vector<Instructions>::iterator it;
    for(it=loi.begin();it!=loi.end();it++){
        for(int i=0;i<4;i++){
            cout<<it->r[i]<<" ";
        }
        cout<<endl;
    }
    cout<<"end of loi\n";
}
//labels
void display_labels(){
    map<string,int>::iterator it;
    for(it=labels.begin();it!=labels.end();it++){
        cout<<it->first<<" "<<it->second;
        cout<<endl;
    }
    cout<<"end of labels\n";
}
//registers
void display_res(){
    map<string,int>::iterator it;
    for(it=res.begin();it!=res.end();it++){
        cout<<it->first<<" ";
    }
    cout<<endl;
    for(it=res.begin();it!=res.end();it++){
        cout<<it->second<<"  ";
    }
    cout<<endl;
}
//memory
void display_mem(){
    for(int i=0;i<32;i++){
        cout<<mem[i]<<" ";
    }
    cout<<endl;
}


/////pipeline steps//////////////////////////////////////////////////////////
vector<Instructions>::iterator it1;
string r1,r2,r3,rs;
string sd,op;
int value,value1;
int f=0;
//IF
void IF(string ins[],vector<Instructions>::iterator it2){
    ins[0]=it2->r[0];
    ins[1]=it2->r[1];
    ins[2]=it2->r[2];
    ins[3]=it2->r[3];
}
//IDandRF
void IDandRF(int rv[],string ins[]){
    int pc1=pc-1;
    op=ins[0];
    r1=ins[1];
    r2=ins[2];
    r3=ins[3];
    if(op=="add"){
        if(r2==rs){
            rv[1]=res[r1];
            rv[2]=value;
            rv[3]=res[r3];
        }
        else if(r3==rs){
            rv[1]=res[r1];
            rv[2]=res[r2];
            rv[3]=value;
        }
        else{
            rv[1]=res[r1];
            rv[2]=res[r2];
            rv[3]=res[r3];
        }
    }
    else if(op==".word"){
        rv[1]=stoi(ins[1]);
    }
    else if(op=="addi"){
        if(r2==rs){
            rv[1]=res[r1];
            rv[2]=value;
            rv[3]=stoi(r3);
        }
        else{
            rv[1]=res[r1];
            rv[2]=res[r2];
            rv[3]=stoi(r3);
        }
    }
    else if(op=="sub"){
        if(r1==rs){
            rv[1]=value;
            rv[2]=res[r2];
            rv[3]=res[r3];
        }
        else if(r2==rs){
            rv[1]=res[r1];
            rv[2]=value;
            rv[3]=res[r3];
        }
        else if(r3==rs){
            rv[1]=res[r1];
            rv[2]=res[r2];
            rv[3]=value;
        }
        else{
            rv[1]=res[r1];
            rv[2]=res[r2];
            rv[3]=res[r3];
        }
    }
    else if(op=="slt"){
        if(r2==rs){
            rv[2]=value;
            rv[3]=res[r3];
        }
        else if(r3==rs){
            rv[2]=res[r2];
            rv[3]=value;
        }
        else{
            rv[2]=res[r2];
            rv[3]=res[r3];
        }
    }
    else if(op=="sll"){
        if(r2==rs){
            rv[2]=value;
        }
        else{
            rv[2]=res[r2];
        }
        rv[3]=stoi(r3);
    }
    else if(op=="beq"){
        if(r1==rs){
            rv[1]=value;
            rv[2]=res[r2];
        }
        else if(r2==rs){
            rv[1]=res[r1];
            rv[2]=value;
        }
        else{
            rv[1]=res[r1];
            rv[2]=res[r2];
        }
        rv[3]=labels[r3];
        if(rv[1]==rv[2]){
            f=1;
            ins[0]="";
            if(pc1<rv[3]){
                it1=it1+(rv[3]-pc1);
                pc=pc+(rv[3]-pc1);
            }
            else{
                it1=it1-(pc1-rv[3]);
                pc=pc-(pc1-rv[3]);
            }
        }
    }


    else if(op=="bne"){
        if(r1==rs){
            rv[1]=value;
            rv[2]=res[r2];
        }
        else if(r2==rs){
            rv[1]=res[r1];
            rv[2]=value;
        }
        else{
            rv[1]=res[r1];
            rv[2]=res[r2];
        }
        rv[3]=labels[r3];
        if(rv[1]!=rv[2]){
            f=1;
            ins[0]="";
            if(pc1<rv[3]){
                it1=it1+(rv[3]-pc1);
                pc=pc+(rv[3]-pc1);
            }
            else{
                it1=it1-(pc1-rv[3]);
                pc=pc-(pc1-rv[3]);
            }
        }
    }

    else if(op=="li"){
        rv[1]=res[r1];
        rv[2]=stoi(r2);
    }

    else if(op=="lw"){
        if(r2==rs){
            rv[2]=value;
        }
        else{
            rv[2]=res[r2];
        }
        rv[3]=stoi(r3);
    }
    else if(op=="sw"){
        rv[1]=res[r1];
        rv[2]=res[r2];
        rv[3]=stoi(r3);
    }
    else if(op=="j"){
        f=1;
        ins[0]="";
        rv[1]=labels[r1];
        if(pc1<rv[1]){
            it1=it1+(rv[1]-pc1);
            pc=pc+(rv[1]-pc1);
        }
        else{
            it1=it1-(pc1-rv[1]);
            pc=pc-(pc1-rv[1]);
        }
    }
    else if(op==".data" || op==".text" || op==".globl"){
    }
    rs=r1;
}
//EX
string EX(int rv[],LRU *L1,LRU *L2){
    if(op=="add"){
        value=rv[2]+rv[3];
        sd=r1;
        return op;
    }
    else if(op==".word"){
        value=rv[1];
        sd="NULL";
        return op;
    }
    else if(op=="addi"){
        value=rv[2]+rv[3];
        sd=r1;
        return op;
    }
    else if(op=="sub"){
        value=rv[2]-rv[3];
        sd=r1;
        return op;
    }
    else if(op=="slt"){
        if(rv[2]<rv[3]){
            sd=r1;
            value=1;
        }
        else{
            sd=r1;
            value=0;
        }
        return op;
    }
    else if(op=="sll"){
        sd=r1;
        value=rv[2]*pow(2,rv[3]);
        return op;
    }
    else if(op=="beq"){
        sd="NULL";
        value=0;
        return op;
    }
    else if(op=="bne"){
        sd="NULL";
        value=0;
        return op;
    }

    else if(op=="li"){
        sd=r1;
        value=rv[2];
        return op;
    }

    else if(op=="lw"){
        sd=r1;
        value=cache((rv[2]+rv[3]-10010000)/4,L1,L2);
        return op;
    }
    else if(op=="sw"){
        sd="NULL";
        value=rv[1];
        value1=(rv[2]+rv[3]-10010000)/4;
        return op;
    }
    else if(op=="j"){
        sd="NULL";
        value=0;
        return op;
    }
    else if(op==".data" || op==".text" || op==".globl"){
        sd="NULL";
        value=0;
        return op;
    }
}
//mem
string MEMO(string op1,LRU *L1,LRU *L2){
    if(op1=="lw"){
        res[sd]=value;
    }
    else if(op1=="sw"){
        mem[value1]=value;
        L1->editItem(value1,value);
        L2->editItem(value1,value);
    }
    else if(op1==".word"){
        mem[temp]=value;
        temp++;
    }
    else{
    }
    return op1;
}

//WB
void WR(string op1){
    if(op1=="add"){
        res[sd]=value;
    }
    else if(op1=="addi"){
        res[sd]=value;
    }
    else if(op1=="sub"){
        res[sd]=value;
    }
    else if(op1=="slt"){
        res[sd]=value;
    }
    else if(op1=="sll"){
        res[sd]=value;
    }
    else if(op1=="beq"){
    }
    else if(op1=="bne"){
    }

    else if(op1=="li"){
        res[sd]=value;
    }
    else if(op1=="j"){
    }
    else if(op1==".data" || op1==".word" || op1==".globl" || op1=="lw" || op1=="sw"){
    }
}
////end//////////////////////////////////////////////////////////////
//pipeline process////////////////////////////////////////////////
void pipe_line(vector<Instructions> loi,LRU *L1,LRU *L2){
    string ins[4];
    string op1;
    int rv[4];
    for(it1=loi.begin();it1!=loi.end();it1++){
        no_of_cycles++;
        if(pc==0){
            pc++;
            goto l0;
        }
       if(pc==1){
            pc++;
            goto l1;
        }
        else if(pc==2){
            pc++;
            goto l2;
        }
        else if(pc==3){
            pc++;
            goto l3;
        }
        else{
            pc++;
            goto l4;
        }

        l0:
        IF(ins,it1);
        continue;

        l1:
        IDandRF(rv,ins);
        if(f==1){
            no_of_stalls++;
            f=0;
            goto end;
        }
        goto l0;

        l2:
        op1=EX(rv,L1,L2);
        goto l1;

        l3:
        MEMO(op1,L1,L2);
        goto l2;
        
        l4:
        WR(op1);
        display_res();
        cout<<endl;
        display_mem();
        cout<<endl;
        goto l3;

        end:
        cout<<endl;
    }
}
////end/////////////////////////////////////////////////
//main function
int main(){
    get_input();
    LRU *L1=new LRU();//L1 cache;
    LRU *L2=new LRU();//L2 cache;
    set_input(L1,L2);
    float IPC=0.0;
    float miss_rate=0.0;
    string str[200];
    vector<Instructions> loi;
    get_instruction(str);
    loi=get_blocks(str);
    res_init();
    pipe_line(loi,L1,L2);

    IPC=(no_of_cycles-no_of_stalls)/no_of_cycles;
    miss_rate=no_of_miss/(no_of_hits+no_of_miss);
    cout<<endl<<"no_of_stalls: "<<no_of_stalls<<endl;
    cout<<"IPC: "<<IPC<<endl;
    cout<<"cache miss rate: "<<miss_rate<<endl;
    cout<<"cache memory of L1:"<<endl;
    L1->printItem();
    cout<<"cache memory of L2:"<<endl;
    L2->printItem();
}
