#include <bits/stdc++.h>
using namespace std;
vector<vector<pair<int, char> > > NFAgraph,DFAgraph,minDFAgraph;  // NFA状态关系图，邻接表
class NFA {
    string name;    // NFA name
    string re;      // 正则表达式
    set<char>alphaset;//字符集
    string re_;     // 带连接符的正则表达式
    string pe;      // 正则表达式的后缀形式
    int NFAstateNum,DFAstateNum,minDFANum;   // 状态数
    pair<int, int> se;      // 起点和终点状态编号
    set<int>DFAend,minDFAend;
    set<int>startset;//起始点闭包
    set<int>episiclo;//计算闭包辅助集合
    map<set<int>,int>DFAstate;//DFA状态,状态号

public:
    NFA(string name, string re);//构造函数，初始化NFA的名称和正则表达式。
    void initalphaset();//初始化字符集
    void insertContact();//在正则表达式中的适当位置插入连接符.，以明确隐式连接。
    int priority(char c);//确定正则表达式操作符的优先级，以便在将正则表达式转换为后缀形式时使用。
    void re2Pe();//将正则表达式转换为后缀形式
    int newState();//创建一个新的NFA状态，并返回其编号
    void pe2NFA();//将后缀形式的正则表达式转换为NFA
    void printNFA();//将NFA输出
    void printDFA();
    void genarateNFAdot(string path);
    void genarateDFAdot(string path);
    void genarateminDFAdot(string path);
    void DFA();
    void minDFA();
    void episiClosure(int start);
    // set<int>move(set<int> state,char a);//一个状态集合经a转移到另一个集合
    set<int> closure(set<int>state,char a);//state经a能到达的状态集合闭包
};
void NFA:: initalphaset()
{
    for(int i=0; i<re.size(); i++)
    {
        if(re[i]!='|'&&re[i]!='*'&&re[i]!='('&&re[i]!=')')
            alphaset.insert(re[i]);
    }
}
int vis[10000];
set<int> NFA:: closure(set<int>state,char a)
{
    episiclo.clear();
    // set<int>tmp;
    // startset.insert(se.first);
    for(auto s:state)
        for(auto edge:NFAgraph[s])
        {
            if(edge.second==a)
            {
                episiclo.insert(edge.first);
                // for(auto edg:NFAgraph[edge.first])
                // {
                //     if(edg.second=='#')tmp.insert(edg.first);
                // }
                memset(vis,0,sizeof(vis));
                episiClosure(edge.first);
            }
        }
    return episiclo;
}

void NFA::episiClosure(int start)
{
    episiclo.insert(start);
    vis[start]=1;
    for(auto edge:NFAgraph[start])
    {
        if(edge.second=='#'&&!vis[edge.first])episiClosure(edge.first);
    }
}
void NFA::DFA()
{
    episiClosure(se.first);
    startset=episiclo;
    set<int>start;
    DFAstateNum=0;
    if(startset.count(se.second))DFAend.insert(DFAstateNum);
    // start.insert(se.first);
    DFAstate[startset]=DFAstateNum;//状态，状态号
    queue<set<int>>q;
    q.push(startset);
    
    while(!q.empty())
    {
        set<int>state=q.front();
        q.pop();
        int start_index=DFAstate[state];
        DFAgraph.push_back(vector<pair<int, char> >());
        for(auto alpha:alphaset)
        {
            set<int>tmp=closure(state,alpha);//state经alpha能到达的所有状态
            if(!DFAstate.count(tmp)&&!tmp.empty()) {//状态集中没有tmp
                // DFAgraph.push_back(vector<pair<int, char> >());
                DFAstateNum++;
                DFAstate[tmp]=DFAstateNum;
                if(tmp.count(se.second))DFAend.insert(DFAstateNum);
                DFAgraph[start_index].push_back({DFAstateNum,alpha});
                q.push(tmp);
            }
            else if(!tmp.empty()) {
                // DFAgraph.push_back(vector<pair<int, char> >());
                DFAgraph[start_index].push_back({DFAstate[tmp],alpha});
            }

        }
    }
}
void NFA::minDFA() {
    map<int, set<int>> partition; // 用于分区的映射，每个分区由状态的集合表示
    queue<set<int>> partitionQueue; // 存储分区的队列，用于迭代处理
    map<int, int> stateToPartition; // 状态到其分区的映射

    // 初始分区：接受状态和非接受状态
    set<int> acceptStates, nonAcceptStates;
    for (int i = 0; i <= DFAstateNum; ++i) {
        if (DFAend.count(i))
            acceptStates.insert(i);
        else
            nonAcceptStates.insert(i);
    }

    int partitionNum = 0;
    // 如果有非接受状态，添加到分区和队列中
    if (!nonAcceptStates.empty()) {
        partition[partitionNum] = nonAcceptStates;
        partitionQueue.push(nonAcceptStates);
        for (int state : nonAcceptStates) stateToPartition[state] = partitionNum;partitionNum++;
    }

    // 如果有接受状态，添加到分区和队列中
    if (!acceptStates.empty()) {
        partition[partitionNum] = acceptStates;
        partitionQueue.push(acceptStates);
        for (int state : acceptStates) stateToPartition[state] = partitionNum;partitionNum++;
    }

    // 开始分区过程
    // while (!partitionQueue.empty()) {
    //     set<int> currentPartition = partitionQueue.front();
    //     partitionQueue.pop();

        // 对于每个字符a，查看它是否可以区分当前分区中的状态
        for (char a : alphaset) {

            int size=partitionQueue.size();
    for(int i=0;i<size;i++) {
        set<int> currentPartition = partitionQueue.front();
        partitionQueue.pop();

            map<int, set<int>> moved;
            for (int state : currentPartition) {
                for (auto& edge : DFAgraph[state]) {
                    if (edge.second == a) {
                        moved[stateToPartition[edge.first]].insert(state);//state转移到哪个分区
                        break;
                    }
                }
            }

            // 如果移动导致当前分区被分割
            for (auto& m : moved) {
                if (!m.second.empty() && m.second.size() < currentPartition.size()) {
                    set<int> newPartition = m.second;

                    for (int np : newPartition) {
                        currentPartition.erase(np);
                        stateToPartition[np] = partitionNum ;
                    }

                    if (!currentPartition.empty()) {
                        partitionQueue.push(currentPartition);
                        partition[stateToPartition[*currentPartition.begin()]] = currentPartition;
                    }

                    partitionQueue.push(newPartition);
                    partition[partitionNum++] = newPartition;

                    break;
                }else if(!m.second.empty()&&*(alphaset.rbegin())!=a){partitionQueue.push(m.second);}
            }
        }
    }

    // 构建一个临时的转移列表来保存所有必要的转移信息
    vector<vector<pair<int, char>>> tempDFAgraph(partitionNum);
    for (int state = 0; state <= DFAgraph.size(); ++state) {
        for (auto& edge : DFAgraph[state]) {
            int fromPartition = stateToPartition[state];
            if(DFAend.count(state))minDFAend.insert(fromPartition);
            int toPartition = stateToPartition[edge.first];
            if(DFAend.count(edge.first))minDFAend.insert(toPartition);
            char symbol = edge.second;
            tempDFAgraph[fromPartition].push_back(make_pair(toPartition, symbol));
        }
    }
    for(auto &g:tempDFAgraph)
    {
        sort(g.begin(),g.end());
        g.erase(unique(g.begin(),g.end()),g.end());
    }

    // 清除原始DFAgraph，并根据分区数重新初始化
    // DFAgraph.clear();
    // DFAstateNum = partition.size();
    minDFANum=partitionNum;
    minDFAgraph.resize(partitionNum);
    minDFAgraph=tempDFAgraph;
    // 使用记录下来的转移信息重建DFAgraph
    // for (int i = 0; i <= DFAstateNum; ++i) {
    //     sort(tempDFAgraph[i].begin(),tempDFAgraph[i].end());
    //     tempDFAgraph[i].erase(unique(tempDFAgraph[i].begin(),tempDFAgraph[i].end()),tempDFAgraph[i].end());
    //     for (auto& edge : tempDFAgraph[i]) {
    //         DFAgraph[i].push_back(edge);
    //     }
    // }

    // 最小化的DFA构建完毕
}



NFA::NFA(string name, string re) {
    this->name = name;
    this->re = re;
    re_ = pe = "";
    NFAstateNum = 0;
    NFAgraph.push_back(vector<pair<int, char> >());
}
void NFA::genarateNFAdot(string path) {
    // ofstream dotFile("compilers/NFA.dot");
    ofstream dotFile(path);

    if (dotFile.is_open()) {
        dotFile << "digraph NFA {\n";
        dotFile << "  rankdir=LR;  // 横向布局\n\n";
        dotFile << " node [shape = circle];   // 初始状态\n\n";
        // dotFile << dfaStates.back().stateName << "[shape = doublecircle];\n";
        dotFile << se.second << "[shape = doublecircle];\n";
        for(int i=1; i<=NFAstateNum; i++)
        {
            if(i!=se.second)dotFile<<i<<";\n";
        }
        // 添加NFA状态
        for(int i=1; i<=NFAstateNum; i++)
        {
            for(auto state:NFAgraph[i])
            {
                if(state.second=='#')
                    dotFile<<i<<"->"<<state.first<<"[label=\""<<"ε"<<"\"];\n";
                else
                    dotFile<<i<<"->"<<state.first<<"[label=\""<<state.second<<"\"];\n";
            }
        }
        dotFile << "}\n";

        dotFile.close();
        cout << "DOT file generated successfully.\n";
    }
    else {
        cout << "Unable to open DOT file.\n";
    }
}
void NFA::genarateminDFAdot(string path) {
    // ofstream dotFile("compilers/NFA.dot");
    ofstream dotFile(path);

    if (dotFile.is_open()) {
        dotFile << "digraph minDFA{\n";
        dotFile << "  rankdir=LR;  // 横向布局\n\n";
        dotFile << " node [shape = circle];   // 初始状态\n\n";
        // dotFile << dfaStates.back().stateName << "[shape = doublecircle];\n";
        // for(auto end:DFAend)
        // dotFile << end << "[shape = doublecircle];\n";
        for(int i=0; i<minDFANum; i++)
        {
            if(!minDFAend.count(i))dotFile<<i<<";\n";
            else dotFile << i << "[shape = doublecircle];\n";
        }
        
        for(int i=0; i<minDFANum; i++)
        {
            for(auto state:minDFAgraph[i])
            {
                if(state.second=='#')
                    // dotFile<<i<<"->"<<state.first<<"[label=\""<<"ε"<<"\"];\n";
                    continue;
                else
                    dotFile<<i<<"->"<<state.first<<"[label=\""<<state.second<<"\"];\n";
            }
        }
        dotFile << "}\n";

        dotFile.close();
        cout << "DOT file generated successfully.\n";
    }
    else {
        cout << "Unable to open DOT file.\n";
    }
}
void NFA::genarateDFAdot(string path) {
    // ofstream dotFile("compilers/NFA.dot");
    ofstream dotFile(path);

    if (dotFile.is_open()) {
        dotFile << "digraph DFA{\n";
        dotFile << "  rankdir=LR;  // 横向布局\n\n";
        dotFile << " node [shape = circle];   // 初始状态\n\n";
        // dotFile << dfaStates.back().stateName << "[shape = doublecircle];\n";
        // for(auto end:DFAend)
        // dotFile << end << "[shape = doublecircle];\n";
        for(int i=0; i<=DFAstateNum; i++)
        {
            if(!DFAend.count(i))dotFile<<i<<";\n";
            else dotFile << i << "[shape = doublecircle];\n";
        }
        // 添加NFA状态
        for(int i=0; i<=DFAstateNum; i++)
        {
            for(auto state:DFAgraph[i])
            {
                if(state.second=='#')
                    // dotFile<<i<<"->"<<state.first<<"[label=\""<<"ε"<<"\"];\n";
                    continue;
                else
                    dotFile<<i<<"->"<<state.first<<"[label=\""<<state.second<<"\"];\n";
            }
        }
        dotFile << "}\n";

        dotFile.close();
        cout << "DOT file generated successfully.\n";
    }
    else {
        cout << "Unable to open DOT file.\n";
    }
}
// 插入连接符 .
void NFA::insertContact() {//a*b(c) *.|
    for(int i = 0; i < re.size() - 1; i++) {
        re_ += re[i];
        if(re[i] != '(' && re[i + 1] != ')' && re[i] != '|' && re[i + 1] != '|'
                && re[i + 1] != '*') re_ += '.';
    }
    re_ += re.back();
}
// 运算符优先级
int NFA::priority(char c) {
    if(c == '*') return 3;
    else if(c == '.') return 2;
    else if(c == '|') return 1;
    else return 0;
}
// 正则表达式转换为后缀形式
void NFA::re2Pe() {
    stack<char> op;
    for(auto c : re_) {
        if(c == '(') op.push(c);
        else if(c == ')') {
            while(op.top() != '(') {
                pe += op.top();
                op.pop();
            }
            op.pop();
        }
        else if(c == '*' || c == '.' || c == '|') {
            while(op.size()) {
                if(priority(c) <= priority(op.top())) {
                    pe += op.top();
                    op.pop();
                }
                else break;
            }
            op.push(c);
        }
        else pe += c;//表达式字符
    }
    while(op.size()) {
        pe += op.top();
        op.pop();
    }
}
// 生成新状态
int NFA::newState() {
    NFAgraph.push_back(vector<pair<int, char> >());    // 生成新状态的边集
    return ++NFAstateNum;
}
// 后缀转换为NFA
void NFA::pe2NFA() {//a.b*   ab*.
    stack<pair<int, int> > states;      // 状态栈
    int s, e;       // 状态边起点和终点状态编号
    for(auto c : pe) {
        if(c != '*' && c != '.' && c != '|') {
            s = newState();//开始状态号
            e = newState();//结束状态号
            states.push(make_pair(s, e));
            NFAgraph[s].push_back(make_pair(e, c));
            continue;
        }
        switch (c)
        {
        case '*': {
            pair<int, int> origin = states.top();
            states.pop();
            s = newState();
            e = newState();
            states.push(make_pair(s, e));
            NFAgraph[s].push_back(make_pair(origin.first, '#'));
            NFAgraph[s].push_back(make_pair(e, '#'));
            NFAgraph[origin.second].push_back(make_pair(e, '#'));
            NFAgraph[origin.second].push_back(make_pair(origin.first, '#'));
            break;
        }
        case '.': {
            s=newState();
            e=newState();
            pair<int, int> right = states.top();
            states.pop();
            pair<int, int> left = states.top();
            states.pop();
            NFAgraph[s].push_back({left.first,'#'});
            NFAgraph[left.second].push_back({right.first,'#'});
            NFAgraph[right.second].push_back({e,'#'});
            states.push({s,e});
            // states.push(make_pair(left.first, right.second));
            // NFAgraph[left.second].push_back(make_pair(right.first, '#'));
            break;
        }
        case '|': {
            pair<int, int> down = states.top();
            states.pop();
            pair<int, int> up = states.top();
            states.pop();
            s = newState();
            e = newState();
            states.push(make_pair(s, e));
            NFAgraph[s].push_back(make_pair(up.first, '#'));
            NFAgraph[s].push_back(make_pair(down.first, '#'));
            NFAgraph[up.second].push_back(make_pair(e, '#'));
            NFAgraph[down.second].push_back(make_pair(e, '#'));
            break;
        }
        default:
            break;
        }
    }
    se = make_pair(states.top().first, states.top().second);
}
// 输出NFA
void NFA::printNFA() {
    ofstream out("compilers/output.txt");
    out << "name: " << name << "\n"
        << "re: " << re << "\n"
        << "pe: " << pe << "\n"
        << "NFAstateNum: " << NFAstateNum << "\n"
        << "start: " << se.first << "\n"
        << "end: " << se.second << endl;
    for(int i = 1; i <= NFAstateNum; i++) {
        for(auto edge : NFAgraph[i]) {
            out << i << "----" << edge.second << "---->" << edge.first << "\n";
        }
    }
    cout << endl;
}
void NFA::printDFA() {
    ofstream out("compilers/output.txt",ios::app);

    for(int i = 0; i <= DFAstateNum; i++) {
        for(auto edge : DFAgraph[i]) {
            out << i << "----" << edge.second << "---->" << edge.first << "\n";
        }
    }
    out << endl;
}

int main(void)
{
    ifstream fin("compilers/input.txt");
    string name, re;
    while(fin >> name >> re) {
        NFA nfa(name, re);
        nfa.initalphaset();
        nfa.insertContact();
        nfa.re2Pe();
        nfa.pe2NFA();
        nfa.printNFA();
        nfa.genarateNFAdot("C:/Users/DELL/Desktop/NFA.dot");
        nfa.DFA();
        nfa.printDFA();
        nfa.genarateDFAdot("C:/Users/DELL/Desktop/DFA.dot");
        nfa.minDFA();
        nfa.genarateminDFAdot("C:/Users/DELL/Desktop/minDFA.dot");
    }
    return 0;
}