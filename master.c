/*************************************************************************
> File Name: master.c
> Author: 
> Mail: 
> Created Time: 2018年11月15日 星期四 19时57分51秒
************************************************************************/

#include "common.h"
//最多设置的线程数
#define INS 5
#define MAX_SIZE 1000
#define PORT 1028
#define PORT1 8201
#define PORT_bash 1029
#define PORT_warning 7771

int warn_socket;
int new_socket;
int queue[INS + 1] = {0};
pthread_mutex_t mutex[10];
#ifdef _DEBUG
#define DBG(fmt , args...)  printf(fmt, ##args) 
#else
#define DBG(fmt , args...)  
#endif


typedef struct Node {
    struct sockaddr_in client_addr;
    struct Node *next;
} Node, *LinkedList;

LinkedList linkedlist[INS + 1];

struct mypara {
    LinkedList l;
    int num;
};

Node *getNode(struct sockaddr_in client_addr) {
    Node *p = (Node *)malloc(sizeof(Node));
    p->client_addr = client_addr;
    p->next = NULL;
    return p;
}

Node *init_node () {
    Node *node = (Node *)malloc(sizeof(Node));
    node->next = NULL;
    return node;
}
/***************** 函数声明 *****************/
void check_list();
int socket_listen(int);
void *func(void *);
void *func1(void *);
LinkedList connect_or_delete(LinkedList, int);
char *get_conf_value(const char *, const char *);

/**********************插入********************/
Node insert(LinkedList head, Node *node, int index) {
    Node *p, ret;
    p = &ret;
    ret.next = head;
    while(p->next && index) {
        p = p->next;
        --index;
    }
    if(index == 0) {
        node->next = p->next;
        p->next = node;
    }
    return ret;
}

/**************************判重******************************/
int search(Node *head, struct sockaddr_in client_addr) {
    Node *p = head;
    while(p) {
        if(p->client_addr.sin_addr.s_addr == client_addr.sin_addr.s_addr) {
            return 1;
            //找到相同，返回1, 删除；
        }
        p = p->next;
    }
    return 0;
}

/**********************删除不存在的地址**********************/
Node* delete_node(LinkedList head, Node *q, int num) {
    Node ret;
    ret.next = head;
    Node *p = &ret, *w = p->next;
    if (head->client_addr.sin_addr.s_addr == q->client_addr.sin_addr.s_addr) {
        ret.next = head->next;
        queue[num]--;
        DBG("***删除IP为：%s，", inet_ntoa(w->client_addr.sin_addr));
        DBG("***第 %d 个链表剩余 %d 个元素\n", num, queue[num]);
        free(w);
        //printf("fanhui is %s\n", inet_ntoa(ret.next->client_addr.sin_addr));
        return ret.next;
    }
    while (w) {
        if (w->client_addr.sin_addr.s_addr == q->client_addr.sin_addr.s_addr) {
            p->next = w->next;
            free(w);
            queue[num]--;
            DBG("***删除IP为：%s，", inet_ntoa(w->client_addr.sin_addr));
            DBG("***第 %d 个链表剩余 %d 个元素\n", num, queue[num]);
            return ret.next;
        }
        p = p->next;
        w = w->next;
    }
    return ret.next;
}

/***************输出*****************/
void output(LinkedList head, int num) {
    //struct sockaddr_in client_addr;
    Node *p = head;
    if (p == NULL) return ;
    while(p) {
        printf("%d : %s  %d\n", num, inet_ntoa(p->client_addr.sin_addr), p->client_addr.sin_port);
        p = p->next;
    }
    printf("\n");
}

/**************链表输出*******************/
void check_list() {
    for(int i = 0; i < INS; i++) {
        printf("%d\n", queue[i]);
        printf("----------- %d.log -----------\n", i);
        output(linkedlist[i], i);
    }
}

/****************清除****************/
void clear(LinkedList head) {
    Node *p, *q;
    p = head;
    while(p) {
        q = p->next;
        free(p);
        p = q;
    }
    return ;
}

/************链表平衡****************/
int find_min(int N, int *arr) {
    int *min = arr;
    int ans = 0;
    for(int i = 0; i < N; ++i) {
        if(*(arr + i) < *min) {
            min = arr + i;
            ans = i;
        }
    }
    return ans;
}


int main() {
    /**********链表初始化**************/
    /*
    LinkedList head;
    Node *p = head;
    Node *q = p->next;
    struct sockaddr_in client_addr;
    new_socket= socket_listen(PORT_bash);
    warn_socket = socket_listen(PORT_warning);
    socklen_t len;
    char *prename = get_conf_value("./init.conf", "prename");
    char *start = get_conf_value("./init.conf", "start");
    char *finish = get_conf_value("./init.conf", "finish");
    for(int i = atoi(start); i <= atoi(finish); i++) {
        char temp[20];
        strcpy(temp, prename);
        //不减一，则拼接时会有换行
        sprintf(temp + strlen(temp) - 1, ".%d", i);
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(PORT);
        client_addr.sin_addr.s_addr = inet_addr(temp);
        //选择元素较少的链表插入，均衡线程时间的负载
        int ind = find_min(INS, queue);
        Node *p, ret;
        p = (Node *)malloc(sizeof(Node));
        p->client_addr = client_addr;
        p->next = NULL;
        ret = insert(linkedlist[ind], p, queue[ind]);
        linkedlist[ind] = ret.next;
        queue[ind]++;
        //printf("%s\n", temp);
    }
    */

    //LinkedList head;
    //Node *p = head;
    //Node *q = p->next;
    

    int pid = fork();
    if(pid < 0) {
        perror("fork");
        return 0;
    }
    if(pid > 0) {
        FILE *fp = fopen("/etc/Pimonitor_sny.pid", "w");
        if(fp == NULL) perror("fopen");
        //printf("pid = %d\n", pid);
        fprintf(fp, "%d", pid);
        fclose(fp);
        return 0;
    }
    //if(pid > 0) exit(0);
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);
    struct sockaddr_in client_addr;
    new_socket= socket_listen(PORT_bash);
    warn_socket = socket_listen(PORT_warning);
    socklen_t len;
    /****************创建线程****************/
    int sockfd;
    int socket_server = socket_listen(PORT);
    pthread_t t[INS + 1];
    struct mypara para[INS + 1];
    
    for(int i = 0; i < INS; ++i) {
        para[i].l = linkedlist[i];
        para[i].num = i;
        if(pthread_create(&t[i], NULL, func, (void *)&para[i]) == -1) {
            printf("func error\n");
            return -1;
        }
        DBG("Current pthread id = %ld \n", t[i]);
    }

    if(pthread_create(&t[INS], NULL, func1, (void *)&para[INS]) == -1) {
        printf("func1 error\n");
        return -1;
    }
    DBG("Current pthread id1 = %ld \n", t[INS]);
    check_list();

    /*************线程锁初始化***************/
    /*for(int i = 0; i < 6; i++) {
        pthread_mutex_init(&mutex[i], NULL);
    }*/

    /**************监听连入*******************/
    while(1) {
        socklen_t len = sizeof(struct sockaddr);
        if((sockfd = accept(socket_server, (struct sockaddr*) &client_addr, &len)) < 0){
            perror("accept");
            close(sockfd);
            continue;
        }
        close(sockfd);
        //printf("link success on %s!!\n", inet_ntoa(client_addr.sin_addr));

        /******************不存在即插入*************************/
        int ans = 0;
        for(int i = 0; i < INS; i++) {
            ans = search(linkedlist[i], client_addr);
            if(ans == 1) break;
        }
        // = 1 :有相同的
        if(ans == 0) {
            int sub = find_min(INS, queue);
            Node *p, ret;
            p = (Node *)malloc(sizeof(Node));
            p->client_addr = client_addr;
            p->next = NULL; 
            //if(pthread_mutex_lock(&mutex[sub]) == 0) {
                ret = insert(linkedlist[sub], p, queue[sub]);
                queue[sub]++;
                linkedlist[sub] = ret.next;
                DBG("insert !!\n");
                output(linkedlist[sub], sub);
                //pthread_mutex_unlock(&mutex[sub]);
            //}
        } else if(ans == 1) {
            DBG("wo zai aa~\n");
        }
        DBG("***** insert IP : %s ******\n", inet_ntoa(client_addr.sin_addr));
        check_list();
    }
    close(new_socket);
    pthread_join(t[0], NULL);
    pthread_join(t[1], NULL);
    pthread_join(t[2], NULL);
    pthread_join(t[3], NULL);
    pthread_join(t[4], NULL);
    printf("\n");
    return 0;
}

/***********************子线程对 IP 操作**********************/
void *func(void *argv) {
    struct mypara *para;
    para = (struct mypara *)argv;
    LinkedList l = para->l;
    while(1) {
        if (linkedlist[para->num] == NULL) sleep(5);
        linkedlist[para->num] = connect_or_delete(linkedlist[para->num], para->num);
    }
    pthread_exit(NULL);
    return NULL;
}

void *func1(void *argv) {
    FILE *fp;
    struct mypara *para;
    para = (struct mypara *)argv;
    char value[30];
    char buffer[MAX_SIZE];
    while(1) {
        socklen_t len;
        struct sockaddr_in w_addr;
        int confd;
        int login;
        char *erron = get_conf_value("./dirname.conf", "ero");
        if((confd = accept(warn_socket, (struct sockaddr *) &w_addr, &len)) < 0) {
            perror("accept_warning");
            continue;
        }
        while((login = recv(confd, buffer, MAX_SIZE, 0)) > 0) {
            DBG("*************WARNING!!!****************\n");
            buffer[login] = '\0';
            DBG("%s\n", buffer);
            char str[100] = "./warn/";
            mkdir(str, 0777);
            strcat(str, erron);
            fp = fopen(str, "a+");
            fprintf(fp, "%s\n", buffer);
            DBG("[Warning] : success to recv %d   \n", login);
            fflush(stdout);
            fclose(fp);
            close(confd);
        }
        sleep(3);
    }
    close(warn_socket);
}

/*************判断是否连接上，连接则关闭套接，否则删除************/
LinkedList connect_or_delete(LinkedList head, int num) {
    socklen_t len;
    Node *p = head;
    int a, login;
    while(p) {
        int newfd;
        struct sockaddr_in c_addr;
        if((newfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket");
        }
        c_addr.sin_family = AF_INET;
        c_addr.sin_port = htons(PORT1);
        c_addr.sin_addr.s_addr = p->client_addr.sin_addr.s_addr;

        if(connect(newfd, (struct sockaddr *) &c_addr, sizeof(c_addr)) < 0) {
            Node *q = p->next;
            //pthread_mutex_lock(&mutex[num]);
            head = delete_node(head, p, num);
            p = q;
            //pthread_mutex_unlock(&mutex[num]);
            close(newfd);
        } else {
            /*****recv file ???*******/
            DBG("******** [connect success] ********\n");

            len = sizeof(c_addr);
            int buffer_num;
            for(int i = 0; i < 6; i++) { 
                FILE *fpt;
                char buffer[MAX_SIZE];
                sleep(3);
                if(recv(newfd, &buffer_num, 4, 0) <= 0) {
                    perror("recv");
                    break;
                }
                if(send(newfd, &buffer_num, 4, 0) <= 0) {
                    perror("send");
                    break;
                }
                int confd;
                if((confd = accept(new_socket, (struct sockaddr *) &c_addr, &len)) < 0) {
                    perror("accept");        
                    continue;
                }
                while((login = recv(confd, buffer, MAX_SIZE, 0)) > 0) {
                    buffer[login] = '\0';
                    //printf("%s\n", buffer);
  
                    char IP[30] = {0};
                    strcpy(IP, inet_ntoa(c_addr.sin_addr));
                    char str[201] = "./client ";
                    strcat(str, IP);
                    mkdir(str, 0777);
  
                    if(buffer_num == 100){ strcat(str, "/cpu.log"); }
                    else if(buffer_num == 101) { strcat(str, "/disk.log"); }
                    else if(buffer_num == 102) { strcat(str, "/mem.log"); }
                    else if(buffer_num == 103) { strcat(str, "/cup.log"); }
                    else if(buffer_num == 104) { strcat(str, "/mallice.log"); }
                    else if(buffer_num == 105) { strcat(str, "/user.log"); }
                    fpt = fopen(str, "a+");
                    fprintf(fpt, "%s\n", buffer);
                    DBG("***** [%d] success to recv %d ******\n", i, login);
                    fflush(stdout);
                    fclose(fpt);
                    close(confd);
                }
            }
            close(newfd);
            p = p->next;
        }
    }
    return head;
}

/***************监听********************/
int socket_listen(int port) {
    int socket_server, sockfd;
    struct sockaddr_in s_addr;
    socklen_t len;

    if((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = htons(INADDR_ANY);

    int opt = 1;
    setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
    if((bind(socket_server, (struct sockaddr*) &s_addr, sizeof(struct sockaddr))) < 0) {
        perror("bind");
        return -1;
    }

    if(listen(socket_server, 20) < 0) {
        perror("listen");
        return -1;
    }
    return socket_server;
}

/*********************查找配置文件信息**********************/
char * get_conf_value(const char *pathname, const char *key_name){
    char *line;
    size_t len = 0;
    ssize_t read;
    char *value = (char *)calloc(sizeof(char), 100);
    FILE *fp = NULL;
    fp = fopen(pathname, "r");
    if (fp == NULL) {
        perror("fopen:");
        return NULL;
    }
    while ((read = getline(&line,&len,fp)) > 0) {
        char *ptr = strstr(line,key_name);
        if (ptr == NULL) continue;
        ptr += strlen(key_name);
        if (*ptr != '=') continue;
        strncpy(value, (ptr+1), strlen(ptr+2));//strlen(per+2) 少获取一个长度，代表换行
        int tempvalue = strlen(value);
        value[tempvalue] = '\0';
    }
    fclose(fp);
    return value;
}
