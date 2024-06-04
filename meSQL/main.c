#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TABLE_SIZE 2999;  // hash范围
//这个项目是我简单的实现了sql语句的解析，并且实现了简单的哈希表，但是没有实现数据库的连接，所以这个项目只能解析sql语句。
int String_equals(const char *str1, const char *str2);
//select id,xxx from stu where id=23 and name='xxx'
int PRIMARY_KEY;

int get_PRIMARY_KEY(){
    return PRIMARY_KEY++;
}

typedef struct stu{//行

    char * id;
    char * name;
    char * home;
    int primary_key;
}stu;

typedef struct stu_list_node{//表
    stu * this;
    struct stu_list_node * next;
}stu_list_node;


typedef stu_list_node * (FUNC)();

typedef struct stu_list_map{//库
    char * table_name;
    stu_list_node * table;
    struct stu_list_map *next;
}stu_list_map;

typedef struct sql_Node{//树节点
    char * body;
    char ** select;
    char num;
    struct sql_Node* left;//左
    struct sql_Node* right;//右
}sql_Node;

typedef struct hashMapNode{
    stu * body;
    struct hashMapNode * next;
}hashMapNode;

typedef struct priority_Node{
    char* key;
    int num;
    struct priority_Node * next;
}priority_Node;

typedef struct Stack_pc{
    char pc;
    stu_list_node ** list;
}Stack_pc;

typedef struct func{
    char* name;
    FUNC* Func;
    struct func* next;
}func;
// 哈希函数
int hashFunction(char *key) {
    int hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash += key[i];
    }
    return hash % TABLE_SIZE;
}


int hashFunction2(int key) {
    return key % 100;
}

int hashFunction3(char *key) {
    int hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash += key[i];
    }
    return hash % 100;
}
int hashFunction4(int key) {
    return key % 2999;
}
//开辟内存
stu *new_stu(int size){
    stu * ptr = (stu*)malloc(size * sizeof(stu));
    for (int i = 0; i < size; ++i) {//初始化
        ptr[i].primary_key=0;
    }
    return ptr;
}
sql_Node *new_sql_Node(int size) {
    sql_Node *ptr = (sql_Node*)malloc(size * sizeof(sql_Node));
    return ptr;
}

hashMapNode *new_hashMapNode(int size){
    hashMapNode * ptr = (hashMapNode*)malloc(size * sizeof(hashMapNode));
    return ptr;
}

priority_Node *new_priority_Node(int size){
    priority_Node * ptr = (priority_Node*)malloc(size * sizeof(priority_Node));
    for (int i = 0; i < size; ++i) {//初始化
        ptr[i].num=0;
    }
    return ptr;
}

stu_list_node* new_stu_list_node(int size){
    stu_list_node * ptr = (stu_list_node*)malloc(size * sizeof(stu_list_node));
    return ptr;
}

stu_list_map* new_stu_list_map(int size){
    stu_list_map * ptr = (stu_list_map*)malloc(size * sizeof(stu_list_map));
    return ptr;
}
stu_list_node** new_stack(int size){
    stu_list_node** list = (stu_list_node **)malloc( size* sizeof(stu_list_node*));
    return list;
}
func* new_func(int size){
    func* list = (func*)malloc( size* sizeof(func));
    return list;
}
int compareNumbers(char* num1, char* num2);


//创建hash表
hashMapNode * new_hashMap(){
    hashMapNode * list= new_hashMapNode(2999);
    return list;
}

priority_Node * new_priority_hashMap(){
    priority_Node *list = new_priority_Node(100);
    return list;
}
stu_list_map* new_stu_list_hashMap(){
    stu_list_map * list = new_stu_list_map(100);
    return list;
}
func* new_func_hashmap(){
    func* list= new_func(100);
    return list;
}

//属性地址表,后可改成hash表
char ** dizhi(stu * last,char * name){
    if(String_equals(name,"id")){
        return (char **)(last);
    } else if(String_equals(name,"name")){
        return (char **)(((char*)last)+8);
    }else if(String_equals(name,"home")){
        return (char **)(((char*)last)+8+8);
    } else{
        return (char **)last;
    }
}

//对应方法
func * func1;
//建库
stu_list_map * My_sql;



//hash表操作
//put
void func_hashmap_put(char* name,FUNC f){
    int num=hashFunction3(name);
    func *ptr=&func1[num];
    while (1){
        if(ptr->Func==NULL){
            ptr->name=name;
            ptr->Func=f;
            break;
        } else if(String_equals(ptr->name,name)){

        } else{
            ptr=ptr->next;
        }
    }
}
hashMapNode* stu_hashMap_put(hashMapNode * hashMap,stu *node){
    if(node!=NULL){
        int num=hashFunction4(node->primary_key);
        hashMapNode *ptr=&hashMap[num];
        while (1){
            if(ptr->body!=NULL&&node->primary_key==ptr->body->primary_key){//去重
                break;
            }
            if(ptr->body==NULL){
                ptr->body=node;
                break;
            } else if(ptr->next==NULL){
                ptr->next= new_hashMapNode(1);
                ptr=ptr->next;
            }else{
                ptr=ptr->next;
            }
        }
        return hashMap;
    }
    return NULL;
}

priority_Node * priority_hashMap_put(priority_Node * hashMap,char * key,int v){
    int num=hashFunction2(*key);
    priority_Node *ptr=&hashMap[num];
    while (1){
        if(ptr->key!=NULL&&String_equals(key,ptr->key)){//去重
            break;
        }
        if(ptr->num==0){
            ptr->key=key;
            ptr->num=v;
            break;
        } else if(ptr->next==NULL){
            ptr->next= new_priority_Node(1);
            ptr=ptr->next;
        }else{
            ptr=ptr->next;
        }
    }

    return hashMap;
}

void stu_list_hashMap_put(char * tablename,stu_list_node * node){
    if(node!=NULL&&tablename!=NULL){
        int a=hashFunction3(tablename);
        stu_list_map * ptr =&My_sql[a];
        while (1){
            if(ptr->table_name!=NULL&&String_equals(ptr->table_name,tablename)){
                stu_list_node * ptr2=ptr->table;
                while (1){
                    if(ptr2==NULL){
                        ptr2=node;
                        break;
                    } else{
                        ptr2=ptr2->next;
                    }
                }
                break;
            } else if(ptr->table_name==NULL){
                ptr->table_name=tablename;
                ptr->table=node;
                break;
            } else{
                ptr=ptr->next;
            }
        }
    } else{

    }
}
//get


FILE * func_hashmap_get(char* name){
    int num=hashFunction3(name);
    func *ptr=&func1[num];
    while (1){
        if(ptr->Func==NULL){
            return NULL;
        } else if(String_equals(ptr->name,name)){
            return ptr->Func;
        } else{
            ptr=ptr->next;
        }
    }
}
stu_list_node* stu_list_hashMapget(char * tablename){
    stu_list_node *ptr = NULL;
    int i=hashFunction3(tablename);
    stu_list_map * ptr2 =&My_sql[i];
    while (1){
        if(ptr2->table_name!=NULL&& String_equals(ptr2->table_name,tablename)){
            ptr=ptr2->table;
            break;
        }else if(ptr2->next==NULL){
            break;
        } else{  ptr2=ptr2->next;
        }
    }
    return ptr;
}

int stu_hashMap_get(hashMapNode * hashMap,stu *node){
    int num=hashFunction4(node->primary_key);
    hashMapNode *ptr=&hashMap[num];
    while (1){
        if(ptr->body==NULL||node==NULL){
            return 0;
        } else if(ptr->body->primary_key==node->primary_key){
            return 1;
        }else{
            ptr=ptr->next;
        }
    }
}

int priority_hashMap_get(priority_Node * hashMap,char * key) {
    int num = hashFunction2(*key);
    priority_Node *ptr = &hashMap[num];
    while (1) {
        if (ptr->num!=0&&String_equals(key, ptr->key)) {//找到
            return ptr->num;
        }
        if (ptr->num == 0) {
            return 0;
        } else if (ptr->next == NULL) {//无下一个
            return 0;
        } else {
            ptr = ptr->next;
        }

    }
}
int selectnamenum;
char ** selectname;

int selectnameput(char * xxx){
    selectname[selectnamenum]=xxx;
    selectnamenum++;
    return selectnamenum;
}

char * selectnameget(){
    if(selectnamenum>0){
        selectnamenum--;
        int unm2=selectnamenum;
        return selectname[unm2];
    } else{
        return NULL;
    }
}

sql_Node * createtree(char ** ptr,int size);
int houxu(sql_Node * ROOT);

priority_Node * priorityHashMap;//全局优先级表


int Char_stacknum;
char ** Char_stack;
char * Char_stackget(){
    if(Char_stacknum>0){
        Char_stacknum--;

        return Char_stack[Char_stacknum];
    } else{
        return NULL;
    }
}
int Char_stackput(char * xxx){
    Char_stack[Char_stacknum]=xxx;
        Char_stacknum++;
    return Char_stacknum;
}


//建栈
Stack_pc *stack_pc;

//入栈
void set_Stack_pc(stu_list_node * list){
    if(list!=NULL){
        stack_pc->list[stack_pc->pc]=list;
        stack_pc->pc++;
    }
}

//出栈
stu_list_node * pop_Stack_pc(){
    if(stack_pc->pc>0){
        stack_pc->pc--;
        char num=stack_pc->pc;
        return stack_pc->list[num];
    }
    return NULL;
}


stu_list_node * from_func(){
    char * tablename=  Char_stackget();
    if(tablename!=NULL){
        set_Stack_pc(stu_list_hashMapget(tablename));
        return stu_list_hashMapget(tablename);
    }
    return NULL;
}
stu_list_node * select_func(){
    stu_list_node *ptr1=pop_Stack_pc();
    char* xxx;
    while ((xxx=Char_stackget())!=NULL){
        selectnameput(xxx);
    }
    set_Stack_pc(ptr1);
    return ptr1;
}

stu_list_node * where_And_func(){

    stu_list_node *and_stu_list_node=NULL;
    stu_list_node *ptr1=pop_Stack_pc();
    stu_list_node *ptr2=pop_Stack_pc();
    if(ptr1==NULL||ptr2==NULL){
        set_Stack_pc(ptr1==NULL?ptr1:ptr2);
        return ptr1==NULL?ptr1:ptr2;
    } else{
        stu_list_node *and_stu_list_node_ptr1;
        hashMapNode* hashmao= new_hashMap();
        while (ptr1!=NULL){
            stu_hashMap_put(hashmao,ptr1->this);
            ptr1=ptr1->next;
        }
        while (ptr2!=NULL){
            if(stu_hashMap_get(hashmao,ptr2->this)){
               if(and_stu_list_node==NULL){
                   stu_list_node *ptr3= new_stu_list_node(1);
                   ptr3->this=ptr2->this;
                   and_stu_list_node=ptr3;
                   and_stu_list_node_ptr1=and_stu_list_node;

               } else{
                   stu_list_node *ptr3= new_stu_list_node(1);
                   ptr3->this=ptr2->this;
                   and_stu_list_node_ptr1->next=ptr3;
                   and_stu_list_node_ptr1=and_stu_list_node_ptr1->next;
               }
            };
            ptr2=ptr2->next;
        }
        set_Stack_pc(and_stu_list_node);
        return and_stu_list_node;
    }

}

stu_list_node * Denyu_func(){
    stu_list_node *and_stu_list_node=NULL;
    char * you=  Char_stackget();
    char * zuo=  Char_stackget();
    stu_list_node *ptr1=pop_Stack_pc();
    set_Stack_pc(ptr1);
    stu_list_node *and_stu_list_node_ptr1;
    while (ptr1!=NULL){
        if(String_equals( *dizhi(ptr1->this,zuo),you)){
            if(and_stu_list_node==NULL){
                stu_list_node *ptr3= new_stu_list_node(1);
                ptr3->this=ptr1->this;
                and_stu_list_node=ptr3;
                and_stu_list_node_ptr1=and_stu_list_node;
            } else{
                stu_list_node *ptr3= new_stu_list_node(1);
                ptr3->this=ptr1->this;
                and_stu_list_node_ptr1->next=ptr3;
                and_stu_list_node_ptr1=and_stu_list_node_ptr1->next;
            }
        }
        ptr1=ptr1->next;
    }
    set_Stack_pc(and_stu_list_node);
    return and_stu_list_node;
}
stu_list_node * Dayu_func(){
    stu_list_node *and_stu_list_node=NULL;
    char * you=  Char_stackget();
    char * zuo=  Char_stackget();
    stu_list_node *ptr1=pop_Stack_pc();
    set_Stack_pc(ptr1);
    stu_list_node *and_stu_list_node_ptr1;
    while (ptr1!=NULL){
        if(compareNumbers( *dizhi(ptr1->this,zuo),you)==1){
            if(and_stu_list_node==NULL){
                stu_list_node *ptr3= new_stu_list_node(1);
                ptr3->this=ptr1->this;
                and_stu_list_node=ptr3;
                and_stu_list_node_ptr1=and_stu_list_node;
            } else{
                stu_list_node *ptr3= new_stu_list_node(1);
                ptr3->this=ptr1->this;
                and_stu_list_node_ptr1->next=ptr3;
                and_stu_list_node_ptr1=and_stu_list_node_ptr1->next;
            }
        }
        ptr1=ptr1->next;
    }
    set_Stack_pc(and_stu_list_node);
    return and_stu_list_node;
}
stu_list_node * Xiaoyu_func(){
    stu_list_node *and_stu_list_node=NULL;
    char * you=  Char_stackget();
    char * zuo=  Char_stackget();
    stu_list_node *ptr1=pop_Stack_pc();
    set_Stack_pc(ptr1);
    stu_list_node *and_stu_list_node_ptr1;
    while (ptr1!=NULL){
        if(compareNumbers( *dizhi(ptr1->this,zuo),you)==-1){
            if(and_stu_list_node==NULL){
                stu_list_node *ptr3= new_stu_list_node(1);
                ptr3->this=ptr1->this;
                and_stu_list_node=ptr3;
                and_stu_list_node_ptr1=and_stu_list_node;
            } else{
                stu_list_node *ptr3= new_stu_list_node(1);
                ptr3->this=ptr1->this;
                and_stu_list_node_ptr1->next=ptr3;
                and_stu_list_node_ptr1=and_stu_list_node_ptr1->next;
            }
        }
        ptr1=ptr1->next;
    }
    set_Stack_pc(and_stu_list_node);
    return and_stu_list_node;
}
stu_list_node * NotDenyu_func(){
    stu_list_node *and_stu_list_node=NULL;
    char * you=  Char_stackget();
    char * zuo=  Char_stackget();
    stu_list_node *ptr1=pop_Stack_pc();
    set_Stack_pc(ptr1);
    stu_list_node *and_stu_list_node_ptr1;
    while (ptr1!=NULL){
        if(String_equals( *dizhi(ptr1->this,zuo),you)){
            if(and_stu_list_node==NULL){
                stu_list_node *ptr3= new_stu_list_node(1);
                ptr3->this=ptr1->this;
                and_stu_list_node=ptr3;
                and_stu_list_node_ptr1=and_stu_list_node;
            } else{
                stu_list_node *ptr3= new_stu_list_node(1);
                ptr3->this=ptr1->this;
                and_stu_list_node_ptr1->next=ptr3;
                and_stu_list_node_ptr1=and_stu_list_node_ptr1->next;
            }
        }
        ptr1=ptr1->next;
    }
    set_Stack_pc(and_stu_list_node);
    return and_stu_list_node;
}
int SQL(){
    stu_list_node* list= new_stu_list_node(4);
    stu_list_node* ptr=list;
    stu* people=new_stu(4);
    stu_list_node* list2= new_stu_list_node(4);
    stu_list_node* ptr2=list;
    stu* people2=new_stu(4);
    people[0].id="10086";
    people[0].name="黑子";
    people[0].home="山东菏泽";
    people[1].id="10010";
    people[1].name="xua";
    people[1].home="山东菏泽";
    people[2].id="956";
    people[2].home="山东菏泽";
    people[3].id="10";
    people[3].name="guo";
    people[3].home="湖南益阳";

    people2[0].id="96311";
    people2[0].name="txy";
    people2[0].home="湖南长沙";
    people2[1].id="1";
    people2[1].name="军志";
    people2[1].home="湖南益阳";
    people2[2].id="96";
    people2[2].name="ixix";
    people2[2].home="湖南益阳";
    people2[3].id="96877";
    people2[3].name="小明";
    people2[3].home="湖南刘阳";
    for (int i = 0; i < 4; ++i) {
        if(i!=3){list[i].next=&list[i+1];}
        people[i].primary_key=get_PRIMARY_KEY();//设置主健
       list[i].this=&people[i];
    }
    for (int i = 0; i < 4; ++i) {
        if(i!=3){list2[i].next=&list2[i+1];}
        people2[i].primary_key=get_PRIMARY_KEY();//设置主健
        list2[i].this=&people2[i];
    }
    stu_list_hashMap_put("stu",list);
    stu_list_hashMap_put("stu2",list2);

}
int init(){//准备内容
    selectnamenum=0;
    func1= new_func(100);
    Char_stacknum=0;
    selectname=(char **)malloc(100*sizeof(char*));
    Char_stack=(char **)malloc(100*sizeof(char*));
    //库
    My_sql=new_stu_list_hashMap();
    PRIMARY_KEY=0;

//栈
    stack_pc=(Stack_pc*)malloc(  sizeof(Stack_pc));
    stack_pc->list= new_stu_list_node(100);

    /*优先级后改为倒序
 * select  11
 *where    12
 *and      13
 *or       14
 * =       15
 * >       16
 * <       17
 * <>      18
 * from    19

 * */

    int xxx=100;
    priorityHashMap=new_priority_hashMap();//
    priority_hashMap_put(priorityHashMap,  "select", xxx--);//100
    func_hashmap_put("select",select_func);

    priority_hashMap_put(priorityHashMap,  "where", xxx--);//99
    func_hashmap_put("where",where_And_func);

    priority_hashMap_put(priorityHashMap,  "and", xxx--);//98
    func_hashmap_put("and",where_And_func);

    priority_hashMap_put(priorityHashMap,  "or", xxx--);//97

    priority_hashMap_put(priorityHashMap,  "=", xxx--);//96
    func_hashmap_put("=",Denyu_func);
    priority_hashMap_put(priorityHashMap,  ">", xxx--);//95
    func_hashmap_put("<",Xiaoyu_func);
    priority_hashMap_put(priorityHashMap,  "<", xxx--);//94
    func_hashmap_put(">",Dayu_func);
    priority_hashMap_put(priorityHashMap,  "<>", xxx--);//93
    func_hashmap_put("<>",NotDenyu_func);
    priority_hashMap_put(priorityHashMap,  "from", xxx--);//92
    func_hashmap_put("from",from_func);

    return 1;
}
void splitStringBySpace(char* input, char** output, int* count) {
    char* token;
    token = strtok(input, " ");
    int index = 0;
    while (token != NULL) {
        output[index] = token;
        index++;
        token = strtok(NULL, " ");
    }
    *count = index;
}

sql_Node * zuoyezi(sql_Node * ROOT);
sql_Node * yidong(sql_Node * ROOT);
int main() {
    init();
    SQL();
    char input[]="select  name id home from stu ";
//    char input[]="select name id home from stu2 ";
//    char input[]="select id name home from stu where id > 10 ";
//    char input[]="select id name  from stu2 where name = 军志 and id = 1 ";
//    char input[]="select id name  from stu2 where name = 军志 and id = 2 ";
//        char input[]="select id name home from stu home = 山东菏泽  ";
        printf("请输入字符串：\n");
        printf("\n");
        char* output[100]; // 设置一个字符串数组来存储分割后的字符串
        int count; // 用于存储分割后的字符串数量

        splitStringBySpace(input, output, &count);

        printf("分割后的字符串数量: %d\n", count);
        printf("分割后的字符串分别是: ");

        for (int i = 0; i < count; i++) {
            printf("%s ", output[i]);
        }
        printf("\n");

        sql_Node * root=createtree(output,count);
        if(String_equals(root->body,"select")){
            root= yidong(root);
        }
        houxu(root);
        stu_list_node * list=pop_Stack_pc();
        stu_list_node * ptr=list;
        char* in;
        for (int i = selectnamenum-1; i >=0; --i) {
            printf("%s\t\t\t",selectname[i]);
        }
        printf("\n");
        while (ptr!=NULL&&ptr->this!=NULL){
            for (int i = selectnamenum-1; i >=0; --i) {
                printf("%s\t\t\t",*dizhi(ptr->this,selectname[i]));
            }
            printf("\n");
            ptr=ptr->next;
        }



    return 0;
}


int String_equals(const char *str1, const char *str2) {//
    if (str1 == NULL || str2 == NULL) {
        return 0;
    }
    return !strcmp(str1, str2);
}

int Stu_equals(stu* stu1, stu* stu2) {//
    int num=0;
    if(stu1->primary_key==stu2->primary_key){
        num=1;
    }
    return num;
}

//建树
sql_Node * createtree(char ** ptr, int size) {
    sql_Node* this = new_sql_Node(1);
    int pc = -1;
    int max = 0;
    for (int i = 0; i < size; i++) {
        if (priority_hashMap_get(priorityHashMap, ptr[i]) > max) {
            pc = i;
            max = priority_hashMap_get(priorityHashMap, ptr[i]);
        }
    }

    if (pc != -1) {
        this->body = ptr[pc];
        if (pc - 0 >= 2) {
            this->left = createtree(ptr, pc);
        } else if(pc-0==1){
            this->left= new_sql_Node(1);
            this->left->body=ptr[pc-1];
        }
        if (size - 1 - pc >= 2) {
            this->right = createtree(&ptr[pc + 1], size - pc - 1);
        } else if(size - 1 - pc ==1){
            this->right= new_sql_Node(1);
            this->right->body=ptr[pc+1];
        }
    } else {
        this->select = ptr; // 用来存储输出
        this->num = (char)size;
    }

    return this;
}

int houxu(sql_Node * ROOT){
    if(ROOT->left!=NULL&& houxu(ROOT->left)){}
    if(ROOT->right!=NULL&& houxu(ROOT->right)){}
    if(ROOT->body!=NULL){
//        printf("%s\n",ROOT->body);
        int num=priority_hashMap_get(priorityHashMap,ROOT->body);
       if(!num){
           Char_stackput(ROOT->body);
       } else{
         FUNC * xxx=func_hashmap_get(ROOT->body);
         xxx();
       }
    }
    if(ROOT->select!=NULL){
        for (int i = 0; i <ROOT->num ; ++i) {
            Char_stackput(ROOT->select[i]);
        }
    }
}
sql_Node * yidong(sql_Node * ROOT){
if(ROOT->right!=NULL){
    ROOT->left=zuoyezi(ROOT->right);
}
    return ROOT;
}
sql_Node * zuoyezi(sql_Node * ROOT){
    if(ROOT->left!=NULL&&ROOT->left->left!=NULL){
        return zuoyezi(ROOT->left);
    }else{

        sql_Node *ptr=ROOT->left;
        ROOT->left=NULL;
        return ptr;
    }
}

int compareNumbers(char* num1, char* num2) {//比较大小
    int intNum1 = atoi(num1);
    int intNum2 = atoi(num2);

    if (intNum1 > intNum2) {
        return 1;
    } else if (intNum1 < intNum2) {
        return -1;
    } else {
        return 0;
    }
}
