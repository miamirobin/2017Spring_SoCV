#include <iostream>
#include <vector>
#include "sat.h"
#include <string>
#include <fstream>
#include "Solver.h"
using namespace std;

class Gate
{
public:
    Gate(unsigned i = 0): _gid(i) {}
    ~Gate() {}
    
    Var getVar() const { return _var; }
    void setVar(const Var& v) { _var = v; }
    
private:
    unsigned   _gid;  // for debugging purpose...
    Var        _var;
};

//
//[0] PI  1 (a)
//[1] PI  2 (b)
//[2] AIG 4 1 2
//[3] PI  3 (c)
//[4] AIG 5 1 3
//[5] AIG 6 !4 !5
//[6] PO  9 !6
//[7] AIG 7 !2 !3
//[8] AIG 8 !7 1
//[9] PO  10 8
//
vector<Gate *> gates;

void
initCircuit(int n)
{
    for (int i=1; i<=n*n+1;i++){
        
       gates.push_back(new Gate(i));  // gates[0]
    }
}

void
genProofModel(SatSolver& s, int n)
{
    
    // Allocate and record variables; No Var ID for POs
    for (size_t i = 0, n = gates.size(); i < n; ++i) {
        Var v = s.newVar();
        gates[i]->setVar(v);
    }
       vector <Var> ron;
        for (int i=0;i<n;i++){
           
         // cout<<n*i<<" "<<i*n+1<<" ";
        for (int j=0;j<n;j++){
            ron.push_back(gates[j+n*i]->getVar());
          
            //cout<<j+n*i<<endl;
        }
            s.addornCNF(gates[n*n]->getVar(),ron, false);
            ron.clear();
        for (int j=0;j<n-1;j++){
            for (int k=j+1;k<n;k++){
                s.addorCNF(gates[n*n]->getVar(), gates[i*n+j]->getVar(), true,
                           gates[i*n+k]->getVar(), true);
                //cout<<i*n+j<<" "<<i*n+k<<endl;
                
                 }
        }
    
          }
    for (int i=0;i<n;i++){
        
        //cout<<n*i<<" "<<i*n+1<<" ";
        for (int j=0;j<n;j++){
            ron.push_back(gates[i+n*j]->getVar());
            
            //cout<<i+n*j<<endl;
        }
        s.addornCNF(gates[n*n]->getVar(),ron, false);
        ron.clear();
        for (int j=0;j<n-1;j++){
            for (int k=j+1;k<n;k++){
                s.addorCNF(gates[n*n]->getVar(), gates[j*n+i]->getVar(), true,
                           gates[k*n+i]->getVar(), true);
                //cout<<j*n+i<<" "<<k*n+i<<endl;
                
            }
        }
        
    }

/*    for (int i=0;i<n;i++){
        s.addorCNF(gates[n*n+3]->getVar(), gates[i]->getVar(), false,
                   gates[n+i]->getVar(), false);
        for (int j=2;j<n;j++){
            s.addorCNF(gates[n*n+3]->getVar(), gates[n*n+3]->getVar(), false,
                       gates[j*n+i]->getVar(), false);
        }
        for (int j=0;j<n-1;j++){
            for (int k=j+1;k<n;k++){
                s.addorCNF(gates[n*n+4]->getVar(), gates[j*n+i]->getVar(), true,
                           gates[k*n+i]->getVar(), true);
                cout<<j*n+i<<" "<<k*n+i<<endl;

                s.addAigCNF(gates[n*n+3]->getVar(), gates[n*n+3]->getVar(), false,
                            gates[n*n+4]->getVar(), false);
            }
        }
        
        s.addAigCNF(gates[n*n+5]->getVar(), gates[n*n+5]->getVar(), false,
                    gates[n*n+3]->getVar(), false);
    }*/
/*    // Hard code the model construction here...
    // [2] AIG 4 1 2 ==> [2] = [0] & [1]
    s.addAigCNF(gates[2]->getVar(), gates[0]->getVar(), false,
                gates[1]->getVar(), false);
    // [4] AIG 5 1 3 ==> [4] = [0] & [3]
    s.addAigCNF(gates[4]->getVar(), gates[0]->getVar(), false,
                gates[3]->getVar(), false);
    // [5] AIG 6 !4 !5 ==> [5] = ![2] & ![4]
    s.addAigCNF(gates[5]->getVar(), gates[2]->getVar(), true,
                gates[4]->getVar(), true);
    // [7] AIG 7 !2 !3 ==> [7] = ![1] & ![3]
    s.addAigCNF(gates[7]->getVar(), gates[1]->getVar(), true,
                gates[3]->getVar(), true);
    // [8] AIG 8 !7 1 ==> [8] = ![7] & [0]
    s.addAigCNF(gates[8]->getVar(), gates[7]->getVar(), true,
                gates[0]->getVar(), false);       */
}

void reportResult(const SatSolver& solver, bool result,int n)
{
    //solver.printStats();
    cout << (result? "Satisfiable assignment:" : "No satisfiable assignment can be found.") << endl;
    if (result) {
        for (int i=0;i<n;i++){
            for (int j=0;j<n;j++){
                if (solver.getValue(gates[i*n+j]->getVar())==1){
                    cout<<i<<"("<<j<<")";
                    if (i!=n-1){cout<<", ";}
                }
            }
        }
        cout<<" "<<endl;
        //for (size_t i = 0, n = gates.size(); i < n; ++i)
            //cout << i<<": "<<solver.getValue(gates[i]->getVar()) << endl;
    }
    }

int main( int argc, char** argv)
{    vector<string> a;
    fstream fin;
    fin.open(argv[1],ios::in);
    char line[1000];
    while(fin.getline(line,sizeof(line),'\n' )){
        //cout<<line<<endl;
        a.push_back(line);
    }
    fin.close();
    //for (int i=0;i<a.size();i++) {
      //    cout<<l<<endl;}
    int num=stoi(a[0]);
    int n=num;
    //cout<<num<<endl;
    initCircuit(n);
    SatSolver solver;
    solver.initialize();
    genProofModel(solver, n);
    bool result;
    //cout<<a.size()<<endl;
    for (int i=1;i<a.size();i++){
        //cout<<"哈"<<endl;
        if(a[i][1]=='e'){
            vector <Var> roy;
            string str=a[i];
            int y=str.find(",");
            //cout<<str.substr(9,y-9)<<" "<<str.substr(y+1,str.size()-1-(y+1));
            //cout<<a[i][9]<<" "<<a[i][12]<<endl;
            int p1=stoi(str.substr(y+1,str.size()-1-(y+1)));
            int p2=stoi(str.substr(9,y-9));
                 //    cout<<p1<<" "<<p2<<endl;
            solver.addAigCNF(gates[n*n]->getVar(), gates[0*num+p1]->getVar(), true,
                             gates[0*num+p1]->getVar(), true);
            solver.addAigCNF(gates[n*n]->getVar(), gates[(n-1)*num+p2]->getVar(), true,
                             gates[(n-1)*num+p2]->getVar(), true);
            for (int j=0;j<num;j++){
                
               for (int k=0;k<j;k++){
                   
                   roy.push_back(gates[k*num+p2]->getVar());
                                 }
                if(roy.size()>0){
                solver.nnaddornCNF(gates[j*num+p1]->getVar(), roy, false);
                    
                    roy.clear();}
            }  //cout<<"done"<<endl;
        }
        if(a[i][1]=='s'){
            if(a[i][6]=='N')
            {//cout<<a[i][10]<<" "<<a[i][13]<<endl;
                string str=a[i];
                int y=str.find(",");
                //cout<<str.substr(10,y-10)<<" "<<str.substr(y+1,str.size()-1-(y+1));
                int m=stoi(str.substr(y+1,str.size()-1-(y+1)));
                int o= stoi(str.substr(10,y-10));
                int p=m*num+o;
                // cout<<o<<" "<<m<<endl;
                solver.addAigCNF(gates[n*n]->getVar(), gates[p]->getVar(), true,
                                 gates[p]->getVar(), true);}
            else {//cout<<a[i][7]<<" "<<a[i][10]<<endl;
                string str=a[i];
                int y=str.find(",");
                //cout<<str.substr(7,y-7)<<" "<<str.substr(y+1,str.size()-1-(y+1));

                int m= stoi(str.substr(y+1,str.size()-1-(y+1)));
                int o= stoi(str.substr(7,y-7));
                int p=m*num+o;
                 // cout<<o<<" "<<m<<endl;
                solver.addAigCNF(gates[n*n]->getVar(), gates[p]->getVar(), false,
                                 gates[p]->getVar(), false);                           }
        }
        if(a[i][1]=='d'){
            if(a[i][8]=='N'){ //cout<<a[i][12]<<" "<<a[i][15]<<endl;
                string str=a[i];
                int y=str.find(",");
               // cout<<str.substr(12,y-12)<<" "<<str.substr(y+1,str.size()-1-(y+1));
                int q1=stoi(str.substr(12,y-12));
                int q2=stoi(str.substr(y+1,str.size()-1-(y+1)));
               //   cout<<q1<<" "<<q2<<endl;
                for (int j=0;j<num-1;j++){
                    solver.addorCNF(gates[n*n]->getVar(), gates[j*num+q1]->getVar(), true,
                                     gates[(j+1)*num+q2]->getVar(), true);
                    solver.addorCNF(gates[n*n]->getVar(), gates[j*num+q2]->getVar(), true,
                                    gates[(j+1)*num+q1]->getVar(), true);
                    }
            }
        else {   //cout<<a[i][9]<<" "<<a[i][12]<<endl;
            string str=a[i];
            int y=str.find(",");
           // cout<<str.substr(9,y-9)<<" "<<str.substr(y+1,str.size()-1-(y+1));
            int q1=stoi(str.substr(9,y-9));
            int q2=stoi(str.substr(y+1,str.size()-1-(y+1)));
            //cout<<q1<<" "<<q2<<endl;
            for (int j=0;j<num;j++){
                
                if (j==0){
                    //cout<<j*num+q1<<" "<<(j+1)*num+q2<<endl;
                    solver.addqorCNF(gates[j*num+q1]->getVar(), gates[(j+1)*num+q2]->getVar(), false,gates[(j+1)*num+q2]->getVar(), false);
                    
                    solver.addqorCNF( gates[j*num+q2]->getVar(),gates[(j+1)*num+q1]->getVar(), false, gates[(j+1)*num+q1]->getVar(), false);}
                    else if(j==num-1){solver.addqorCNF(gates[j*num+q1]->getVar(),gates[(j-1)*num+q2]->getVar(), false,gates[(j-1)*num+q2]->getVar(), false);
                        solver.addqorCNF(gates[j*num+q2]->getVar(),gates[(j-1)*num+q1]->getVar(), false,
                                        gates[(j-1)*num+q1]->getVar(), false);
                    }
                    else {solver.addqorCNF(gates[j*num+q2]->getVar(), gates[(j-1)*num+q1]->getVar(), false,gates[(j+1)*num+q1]->getVar(), false);
                        solver.addqorCNF(gates[j*num+q1]->getVar(),gates[(j-1)*num+q2]->getVar(), false,gates[(j+1)*num+q2]->getVar(), false);
                        
                    }
                
            }
                
        }
        }
    }
   
    
    
    
    
    
    
    
   //solver.addAigCNF(gates[n*n]->getVar(), gates[2]->getVar(), false,
           //gates[2]->getVar(), false);
   
    solver.assumeRelease();
    solver.assumeProperty(gates[n*n]->getVar(), true);
    result = solver.assumpSolve();
    reportResult(solver, result, n);
    
}
