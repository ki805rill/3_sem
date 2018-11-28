#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct {
    int p2c[2];
    int c2p[2];
} dpipe_t;
int dpipe_init_pipes(dpipe_t *dp);
int dpipe_cleanup(dpipe_t *dp, int is_parent);
ssize_t dpipe_send(dpipe_t *dp, const void *msg, size_t sz, int is_parent);
ssize_t dpipe_rcv(dpipe_t *dp, void *msg, size_t sz, int is_parent);
int main()
{
    char buf[BUFSIZ];
    size_t size;
    dpipe_t dp;
    dpipe_init_pipes(&dp);
    const pid_t pid = fork();
    if (pid < 0) {
        printf("fork failed!\n");
        return -1;
    }
    dpipe_cleanup(&dp, pid);
    if(pid){
        while((size = read(0, buf, sizeof(buf) - 1)) > 0){
            buf[size] = 0; // the text string data is expected
            buf[size - 1] = 0; // useless '\n' in the end of the read string
            size = dpipe_send(&dp,(void*)buf, size, pid);
            printf("Sent by the parent         (*)p----->c:    |%s|\n", buf);
            size = dpipe_rcv(&dp,(void*)buf, size, pid);
            printf("Received in the parent        c----->p(*): |%s|\n", buf);
        }
    } else {
        while((size = dpipe_rcv(&dp, (void*)buf, sizeof(buf)-1, pid) > 0)){
            printf("Received in the child         p----->c(*): |%s|\n", buf);
            size = dpipe_send(&dp, (void*)buf, size, pid);
            printf("Sent by the child          (*)c----->p:    |%s|\n", buf);
        }
    }
    return 0;
}
int dpipe_init_pipes(dpipe_t *dp)
{
    if((pipe(dp->p2c) < 0) || (pipe(dp->c2p) < 0)){
        printf("Pipe creation has failed!\n");
        return -1;
    }
    return 0;
}
int dpipe_cleanup(dpipe_t *dp, int is_parent)
{
    if(is_parent){
        // parent can write to p2c[1] and read from c2p[0]
        close(dp->p2c[0]);
        close(dp->c2p[1]);
    } else {
        // child can read from p2c[0] and write to c2p[1]
        close(dp->p2c[1]);
        close(dp->c2p[0]);
    }
    return 0;
}
ssize_t dpipe_send(dpipe_t *dp, const void *msg, size_t sz, int is_parent)
{
    return write(is_parent ? dp->p2c[1] : dp->c2p[1], msg, sz);
}
ssize_t dpipe_rcv(dpipe_t *dp, void *msg, size_t sz, int is_parent)
{
    return read(is_parent ? dp->c2p[0] : dp->p2c[0], msg, sz);
}
