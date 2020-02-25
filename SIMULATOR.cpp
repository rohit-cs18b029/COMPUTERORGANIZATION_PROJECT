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

int pc=0;

class Instructions{
public:
string r[4];
Instructions(string r1[]){
for(int i=0;i<4;i++){
r[i]=r1[i];
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

string remove_spaces(string line){
int n;
while(true){
n=line.find(' ');
if(n!=-1){
line=line.replace(n,1,"");
}
else{
break;
}
}
return line;
}

string remove_commas(string line){
int n;
while(true){
n=line.find(',');
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
line=remove_spaces(line);
line=remove_commas(line);
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
line=remove_spaces(line);
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
line=remove_spaces(line);
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
line=remove_spaces(line);
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
//cout<<"type till end\n";
ifstream read;
read.open("bb.s");
while(true){
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
for(int i=0;i<35;i++){
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
s[1]=remove_spaces(flag);
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

void display_labels(){
map<string,int>::iterator it;
for(it=labels.begin();it!=labels.end();it++){
cout<<it->first<<" "<<it->second;
cout<<endl;
}
cout<<"end of labels\n";
}

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

void display_mem(){
for(int i=0;i<32;i++){
cout<<mem[i]<<" ";
}
cout<<endl;
}

void execution(vector<Instructions> loi){
string op,r1,r2,r3;
int r11,r22,r33;
vector<Instructions>::iterator it;
for(it=loi.begin();it!=loi.end();it++){
op=it->r[0];
r1=it->r[1];
r2=it->r[2];
r3=it->r[3];
if(op=="add"){
r11=res[r1];
r22=res[r2];
r33=res[r3];
r11=r22+r33;
res[r1]=r11;
}
else if(op==".word"){
r11=stoi(r1);
mem[temp]=r11;
temp++;
}
else if(op=="addi"){
r11=res[r1];
r22=res[r2];
r11=r22+stoi(r3);
res[r1]=r11;
}
else if(op=="sub"){
r11=res[r1];
r22=res[r2];
r33=res[r3];
r11=r22-r33;
res[r1]=r11;
}
else if(op=="slt"){
r22=res[r2];
r33=res[r3];
if(r22<r33){
res[r1]=1;
}
else{
res[r1]=0;
}
}
else if(op=="sll"){
r22=res[r2];
r33=stoi(r3);
res[r1]=r22*pow(2,r33);
}
else if(op=="beq"){
r11=res[r1];
r22=res[r2];
if(r11==r22){
if(pc<labels[r3]){
it=it+(labels[r3]-pc);
pc=pc+(labels[r3]-pc);
}
else{
it=it-(pc-labels[r3]);
pc=pc-(pc-labels[r3]);
}
}
}
else if(op=="bne"){
r11=res[r1];
r22=res[r2];
if(r11!=r22){
if(pc<labels[r3]){
it=it+(labels[r3]-pc);
pc=pc+(labels[r3]-pc);
}
else{
it=it-(pc-labels[r3]);
pc=pc-(pc-labels[r3]);
}
}
}

else if(op=="li"){
res[r1]=stoi(r2);
}

else if(op=="lw"){
r22=res[r2];
r33=stoi(r3);
res[r1]=mem[(r22+r33-10010000)/4];
}
else if(op=="sw"){
r11=res[r1];
r22=res[r2];
r33=stoi(r3);
mem[(r22+r33-10010000)/4]=r11;
}
else if(op=="j"){
if(pc<labels[r1]){
it=it+(labels[r1]-pc);
pc=pc+(labels[r1]-pc);
}
else{
it=it-(pc-labels[r1]);
pc=pc-(pc-labels[r1]);
}
}
pc++;
display_res();
}
}

int main(){
string str[36];
vector<Instructions> loi;
get_instruction(str);
loi=get_blocks(str);
//display_loi(loi);
//display_labels();
res_init();
execution(loi);
display_mem();
}
