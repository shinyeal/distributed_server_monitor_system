/*************************************************************************
> File Name: client.c
> Author: 
> Mail: 
> Created Time: 2018年11月15日 星期四 19时57分51秒
************************************************************************/

#include "common.h"
#define INS 6
#define MAX_SIZE 1000
#define PORT 8201
pthread_mutex_t mutex[10];

struct mypara {
    char path[50];
    char dirname[20];
    char error[20];
    char errpath[50];
    int num;
    char sleep_mes[10];
};
/*
void init(int n) {
    struct mypara *para;
    para->dirname = get_conf_value("./dirname.conf", n);
    para->path = get_conf_value("./path.conf", n);
    para->num = n;
}
*/
/*****************函数声明****************/
int socket_listen(int );
int socket_create(int ,char *);
void *func(void *);
void *func1(void *);

int main(int argc, char *argv[]) {
    for(int i = 0; i < 6; i++) {
        pthread_mutex_init(&mutex[i], NULL);
    } 
    char buffer[MAX_SIZE];
    int port = atoi(argv[2]);
    //int port1 = atoi(argv[3]);
    char *host = argv[1];
    pthread_t t[INS + 1];
    struct mypara para[INS + 1]; 
   
    
    /*****pthread begin !!*******/
    for(int i = 0; i < INS; ++i) {
        para[i].num = i;
        if(pthread_create(&t[i], NULL, func, (void *)&para[i]) == -1) {
            printf("error\n");
            return -1;
        }
        printf("Current pthread 1 id = %ld \n", t[i]);
    }

   if(pthread_create(&t[INS], NULL, func1, (void *)&para[INS]) == -1) {
        printf("error\n");
        return -1;
    }
    printf("Current pthread 2 id = %ld \n", t[INS]);
    /*******pthread end !!******/
    int sock_client;
    while(1) {
        sock_client = socket_create(port, host);
        if(sock_client > 0) printf("[success]heart shot \n");
        else printf("[false]heart shot\n");
        close(sock_client);
        sleep(15);
    }
    return 0;
}


/*********查找配置文件信息*********/
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

/*********监听**********/
int socket_listen(int port) {
    int socket_server;
    struct sockaddr_in s_addr;
    socklen_t len;
    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket create");
        return -1;
    }

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    //printf("port:%d\n", s_addr.sin_port);
    s_addr.sin_addr.s_addr = htons(INADDR_ANY);

    if ((bind(socket_server, (struct sockaddr*)&s_addr, sizeof(struct sockaddr))) < 0) {
        perror("bind");
        return -1;
    }
    if (listen(socket_server, 20) < 0) {
        perror("listen");
        return -1;
    }
    return socket_server;
}

/********连接********/
int socket_create(int port, char *host) {
    int sock_client;
    struct sockaddr_in dest_addr;
    if ((sock_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        return -1;
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(host);

    if (connect(sock_client, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Connect");
        return -1;
    }
    return sock_client;
}

/**********保存运行信息**********/
void *func(void *argv) {
    FILE *fp;
    struct mypara *para;
    para = (struct mypara *)argv;
    char buffer[MAX_SIZE];
    char num[50];
    int warn_send;
    sprintf(num, "%d", para->num);
    char *path = get_conf_value("./path.conf", num);
    char *dirname = get_conf_value("./dirname.conf", num);
    char *sleep_mes = get_conf_value("./sleep.conf", num);
    int sleep_num = atoi(sleep_mes);
    while(1) {
        int retread = 0;
        fp = popen(path, "r");
        while ((retread = fread(buffer, 1, MAX_SIZE, fp)) > 0) {
            buffer[retread] = '\0';
            //printf("%s\n", buffer);
            char str[100] = "./log/";
            mkdir(str, 0777);
            strcat(str, dirname);
            pthread_mutex_lock(&mutex[para->num]);
            fp = fopen(str, "a+");
            fprintf(fp, "%s\n", buffer);
            pthread_mutex_unlock(&mutex[para->num]);
            /********warning message********/
            if(para->num == 3 || para->num == 0) {
                if(strstr(buffer, "warning") != NULL) {
                    warn_send = socket_create(7771, "192.168.1.156");
                    if(send(warn_send, &buffer, strlen(buffer), 0) <= 0) {
                        perror("send_erron");
                    }
                    close(warn_send);
                }
            } else if(para->num == 4) {
                if(strlen(buffer) != 0) {
                    warn_send = socket_create(7771, "192.168.1.156");
                    if(send(warn_send, &buffer, strlen(buffer), 0) <= 0) {
                        perror("send_erron");
                    }
                    close(warn_send);
                } 
            }
            fclose(fp);
            //pclose(fp);
        }
        fflush(stdout);
        memset(buffer, 0, sizeof(buffer));
        fclose(fp);
        sleep(sleep_num);
    }
    //pthread_exit(NULL);
    return NULL;
}

/**************发送文件信息****************/
void *func1(void *argv) {
    FILE *fp;
    struct mypara *para;
    para = (struct mypara *)argv;
    char buffer[MAX_SIZE];
    int recv_socket = socket_listen(PORT);
    while(1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int confd;
        if ((confd = accept(recv_socket, (struct sockaddr*)&client_addr, &len)) < 0) {
            perror("accept");
            close (confd);
            continue;
        } else {
            char *temp = inet_ntoa(client_addr.sin_addr);
            printf("receive connect from server:%s\n", temp);

            /********send start!!!************/
            //FILE *fp;
            int retnum;
            int i;
            for(retnum = 100, i = 0; retnum <= 105, i < 6; retnum++, i++) {
                char num[50];
                sprintf(num, "%d", i);
                char *dirname = get_conf_value("./dirname.conf", num);
                char str[200] = "./log/";
                strcat(str, dirname);
                sleep(3);
                if(send(confd, &retnum, 4, 0) <= 0) {
                    perror("send");
                    break;
                }
                if(recv(confd, &buffer, 4, 0) <= 0) {
                    perror("recv");
                    break;
                }
                sleep(5);
                int newfd = socket_create(1029, "192.168.1.156");
                if (newfd == -1) continue;
                if(retnum == 100) {
                    fp = fopen("./log/cpu.log", "r");
                } else if(retnum == 101) {
                    fp = fopen("./log/disk.log", "r");
                } else if(retnum == 102) {
                    fp = fopen("./log/mem.log", "r");
                } else if(retnum == 103) {
                    fp = fopen("./log/cup.log", "r");
                } else if(retnum == 104) {
                    fp = fopen("./log/malice.log", "r"); 
                } else if(retnum == 105) {
                    fp = fopen("./log/user.log", "r");
                }
                int retread = 0;
                pthread_mutex_lock(&mutex[i]);
                while ((retread = fread(buffer, 1, MAX_SIZE, fp)) > 0) {
                    ///printf("%s\n", buffer);
                    printf("[%d]th had send from %s!!!\n", i, dirname);
                    send(newfd, buffer, retread, 0);
                    memset(buffer, 0, sizeof(buffer));
                }
                fclose(fp);
                pthread_mutex_unlock(&mutex[i]);
                printf("send end!!\n");
                //fclose(fp);
                fflush(stdout);
                close(newfd);
                char str1[50];
                sprintf(str1, "> %s",str);
                system(str1);
                printf("\n");
                printf("[%d] remove success\n", i);
            }
            fclose(fp);
            close(confd);

        }
        fclose(fp);
        //int pthread_mutex_unlock(pthread_mutex_t *mutex);
    }
    fclose(fp);
    close(recv_socket);
    //pthread_exit(NULL);
    return NULL;
}
