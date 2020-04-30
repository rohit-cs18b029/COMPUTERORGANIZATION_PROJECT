#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<math.h>
using namespace std;
map<string,int> labels;
map<string,int> res;
int mem[32]={0};
int temp=0;
int ins_count=0;
int pc=0;
float no_of_cycles=0;
float no_of_stalls=0;

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


///////////////////////////////////////////////////////////////
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
string EX(int rv[]){
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
        value=mem[(rv[2]+rv[3]-10010000)/4];
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
string MEMO(string op1){
    if(op1=="lw"){
        res[sd]=value;
    }
    else if(op1=="sw"){
        mem[value1]=value;
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
//pipeline
void pipe_line(vector<Instructions> loi){
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
        op1=EX(rv);
        goto l1;

        l3:
        MEMO(op1);
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

//main function
int main(){
    float IPC=0.0;
    string str[200];
    vector<Instructions> loi;
    get_instruction(str);
    loi=get_blocks(str);
    res_init();
    pipe_line(loi);

    IPC=(no_of_cycles-no_of_stalls)/no_of_cycles;
    cout<<endl<<"no_of_stalls: "<<no_of_stalls;
    cout<<endl<<"IPC: "<<IPC;
}
