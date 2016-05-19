#include <xmtc.h>
#include <xmtio.h>

//#define PRINT

#define NIL -1

#define WHITE 0
#define GRAY  1
#define BLACK 2



int num[N] = {0};
int low[N] = {0};
int estack[M] = {0};
int state_node[N] = {WHITE};
int st_ptr = 0;
int art_cnt = 0;
int num_cntr = 0;
int min(int a, int b){
    if(a<b) return a;
    return b;
}

void DFS (int cur, int par){
#ifdef PRINT
    printf("cur:\t %d,\t par:\t %d,\t num_cur:\t %d\n", cur, par,num_cntr);
#endif
    int cur_st_ptr;
    state_node[cur] = BLACK;
    for (int i=0; i<degrees[cur]; i++) {
        int nxt = edges[vertices[cur]+i][1];
        if(nxt != par){
            if(state_node[nxt]== WHITE){ //case a
                //push cur - nxt to estack
                cur_st_ptr = st_ptr;
                estack[st_ptr] = vertices[cur]+i;
                st_ptr++;
                //
                num_cntr++;
                num[nxt] = num_cntr;
                low[nxt] = num_cntr;
                DFS(nxt, cur);

                low[cur] = min(low[cur],low[nxt]);
                if(low[nxt] >= num[cur]) {
                    //cur is articulation point and pop.
#ifdef PRINT
                    printf("\n\n");
                    for(int j = 0; j<st_ptr;j++){
                        printf("%d \t",estack[j]);
                    }
                    printf("\n\n");
                    printf("st_ptr : %d, cur_st_ptr : %d\n\n",st_ptr, cur_st_ptr);
                    for (int j=0; j<M; j++) {
                        printf("%d:%d  ",j,bcc[j]);
                    }
                    printf("\n\n");
                    printf("art_cnt : %d\n",art_cnt);
#endif
                    while (st_ptr>cur_st_ptr){
                        st_ptr--;
                        int val = estack[st_ptr];
                        bcc[val] = art_cnt;
                        bcc[antiparallel[val]] = art_cnt;
                    }
                    art_cnt++;
#ifdef PRINT
                    for (int j=0; j<M; j++) {
                        printf("%d:%d  ",j,bcc[j]);
                    }
                    printf("\n\n");
                    printf("art_cnt : %d\n",art_cnt);
#endif
                }
            }
            else{ // node is already visited
                if(num[nxt] < num[cur]){ //case b
                    //push cur - nxt to estack
                    estack[st_ptr] = vertices[cur]+i;
                    st_ptr++;
                    //
                    low[cur] = min(low[cur], num[nxt]);
                }
                /*else{ //case c
                    //ignore e
                }*/
            }
        }
    }
    return;
}

int main() {
	// TODO: compute biconnectivity
    int i;
    
#ifdef PRINT
    printf("N : %d\n\n\n",N);
    printf("M : %d\n\n\n",M);
    printf("Edges are \n");
    for (i=0; i<M; i++) {
        printf("%d : %d -> %d \n", i,edges[i][0], edges[i][1]);
    }
    printf("\n\n");
    printf("Degrees are \n");
    for (i=0; i<N; i++) {
        printf("%d : %d\n", i, degrees[i]);
    }
    printf("\n\n");
#endif
    
    num[0] = 0;
    low[0] = 0;
    DFS(0,0);
    
    
#ifdef PRINT
    printf("\n\nbcc array is as follows :\n");
    for (i=0; i<M; i++) {
        printf("%d : %d -> %d \n",bcc[i],edges[i][0],edges[i][1]);
    }
    printf("\n\n");
#endif
    
    
	return 0;
}
