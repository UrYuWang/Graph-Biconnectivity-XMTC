#include <xmtc.h>
#include <xmtio.h>
#include "/opt/xmt/class/include/library.h"
psBaseReg psb0;

//#define PRINT
//#define PRINT_FINAL

#define NIL -1
int D[N];
int T[M];
int T_comp[M];
int new_edges[M][2];
int new_M;
int new_degrees[N] = {0};
int new_vertices[N];
int new_antiparallel[M];
int next_D_1[M];
int next_D_2[M];
int R[M];
int dir[M];
int parent[N];
int preorder[N], size_subtree[N];
int edges_idx[M];

int low[3*N], high[3*N];
int aux_low[3*N], aux_high[3*N];

int main() {
    // TODO: compute biconnectivity
    int i;
    
    //1. Compute a spanning tree T of the input graph

#ifdef PRINT
    printf("\n\n\n");
    printf("edges list is as following \n");
    for (i=0; i<M; i++) {
        printf("(%d->%d) : %d, anti : %d\n", edges[i][0],edges[i][1],i,antiparallel[i]);
    }
    printf("\n\n\n");
    printf("D is : \n");
    for (i=0; i<N; i++) {
        printf("i : %d,\t D[i] :\t %d\n", i, D[i]);
    }

#endif
    
    connectivityTG(edges, antiparallel, N, M, D, T);
    //Create a tree
    prefix_sum_int(T,M,T_comp);
    new_M = T_comp[M-1]; //only this many edges are remained

    spawn(0,N-1){
        int new_deg = 0;
        for(int loc=0;loc<degrees[$];loc++){
            if(T[vertices[$]+loc]) new_deg++;
        }
        new_degrees[$] = new_deg;
    }
    spawn(0,N-1){
        int new_vert = 0;
        for(int loc=0;loc<degrees[$];loc++){
            if(T[vertices[$]+loc]) {
                new_vert = T_comp[vertices[$]+loc]-1;
                break;
            }
        }
        new_vertices[$] = new_vert;
    }
    spawn(0,M-1){
        if (T[$]) {
            new_edges[T_comp[$]-1][0] = edges[$][0];
            new_edges[T_comp[$]-1][1] = edges[$][1];
        }
    }
    
    spawn(0,M-1){
        if (T[$]) {
            new_antiparallel[T_comp[$]-1] = T_comp[antiparallel[$]]-1;
        }
    }
    
#ifdef PRINT
    printf("\n\n\n");
    printf("D is : \n");
    for (i=0; i<M; i++) {
        printf("%d - %d : %d, %d\n", edges[i][0], edges[i][1], T[i], T_comp[i]);
    }
    printf("\n\n\n");
    printf("new degrees are : \n");
    for (i=0; i<N; i++) {
        printf("new_deg[%d] : %d\n", i, new_degrees[i]);
    }
    printf("\n\n\n");
    printf("new vertices are : \n");
    for (i=0; i<N; i++) {
        printf("new_vertices[%d] : %d\n", i, new_vertices[i]);
    }
    printf("\n\n\n");
    printf("new edges are : \n");
    for (i=0; i<new_M; i++) {
        printf("%d, %d - %d, anti: %d\n", i, new_edges[i][0], new_edges[i][1],new_antiparallel[i]);
    }
#endif
    
    
    //2. Root the spanning tree and find an Euler tour of the rooted spanning tree
    int root = 0;
    spawn(0,new_M-1){
        int relative_idx, start_ptr_node;
        start_ptr_node = new_vertices[new_edges[$][1]];
        relative_idx = new_antiparallel[$] - start_ptr_node;
        next_D_1[$] = start_ptr_node + ((relative_idx + 1 )% new_degrees[new_edges[$][1]]);
    }


#ifdef PRINT
    printf("\n\n\n");
    printf("D : \n");
    for (i=0; i<new_M; i++) {
        printf("i : %d, D[i] : %d\n", i, next_D_1[i]);
    }
#endif
 
    int head = next_D_1[new_antiparallel[new_vertices[root]]];
    next_D_1[new_antiparallel[new_vertices[root]]] = new_antiparallel[new_vertices[root]];

#ifdef PRINT
    printf("\n\n\n");
    printf("new_M : %d, T_comp[M] : %d, M : %d, head : %d", new_M,T_comp[M],M,head);
    printf("\n\n\n");
    printf("D : \n");
    for (i=0; i<new_M; i++) {
        printf("i : %d, D[i] : %d\n", i, next_D_1[i]);
    }
#endif
    
    //Get the list ranking
    spawn(0,new_M-1){
        next_D_2[$] = next_D_1[$];
        R[$] = 1;
    }
    R[new_antiparallel[new_vertices[root]]] = 0;
    list_rank(next_D_1, R, new_M, head);

#ifdef PRINT
    printf("\n\n\n");
    printf("Ranking is : \n");
    for (i=0; i<new_M; i++) {
        printf("i : %d, R[i] : %d\n", i, R[i]);
    }
#endif
    
    //Get the direction of tree
    spawn(0,new_M-1){
        dir[$] = 0;
        if (R[$] < R[new_antiparallel[$]]) {
            dir[$] = 1;
            //parent[new_edges[$][0]] = new_edges[$][1]; // Probably don't need
            edges_idx[new_edges[$][0]] = new_antiparallel[$]; //edge index for u -> v
        }
    }

#ifdef PRINT
    printf("\n\n\n");
    printf("Directed Tree : \n");
    for (i=0; i<new_M; i++) {
        printf("(%d->%d), dir[i] : %d\n", new_edges[i][0], new_edges[i][1], dir[i]);
    }
    printf("\n\n\n");
    printf("Edge Index : \n");
    for (i=0; i<M; i++) {
        printf("%d : %d\n", i, edges_idx[i]);
    }
#endif

    //3. Determine the preorder number of each vertex and the size of the subtree rooted at each vertex
    
    //Set the preorder/size numbering
    spawn(0,new_M-1){
        R[$] = 0;
        if(dir[$]==0) R[$] = 1;
    }
    R[new_antiparallel[new_vertices[root]]] = 0;
    
#ifdef PRINT
    printf("\n\n\n");
    printf("Direction and Ranking for preorder is : \n");
    for (i=0; i<new_M; i++) {
        printf("i: %d, D[i] : %d, R : %d, dir : %d\n", i, next_D_2[i], R[i], dir[i]);
    }
#endif
    
    list_rank(next_D_2, R, new_M, head);

#ifdef PRINT
    printf("\n\n\n");
    printf("Ranking is : \n");
    for (i=0; i<new_M; i++) {
        printf("i : %d, R[i] : %d\n", i, R[i]);
    }
#endif
    
    preorder[root] = 1;
    low[root] = 1;
    high[root] = preorder[root] + N - 1;
    size_subtree[root] = N;
    spawn(0,new_M-1){
        if (dir[$]==0) {//for every tree edgee
            int v = new_edges[$][1];
            preorder[v] = N - R[$] + 1;
            size_subtree[v] = R[$] - R[new_antiparallel[$]];
            low[v] = preorder[v];
            high[v] = preorder[v] + size_subtree[v] - 1;
        }
    }

#ifdef PRINT
    printf("\n\n\n");
    printf("size_subtree : \n");
    for (i=0; i<N; i++) {
        printf("%d : %d\n",i,size_subtree[i]);
    }
    printf("\n\n\n");
    printf("preorder : \n");
    for (i=0; i<N; i++) {
        printf("%d : %d\n",i,preorder[i]);
    }
    printf("\n\n\n");
    printf("parent : \n");
    for (i=0; i<N; i++) {
        printf("%d : %d->%d\n",i,parent[i],i);
    }
    printf("\n\n\n");
    printf("low/high numbering : \n");
    for (i=0; i<N; i++) {
        printf("i:\t%d \t| preorder:\t%d \t| low:\t%d \t| high:\t%d\n",i,preorder[i],low[i],high[i]);
    }
#endif
    
    //4. For each vertex v,search the subtree T(v) rooted at v for ...
    
    spawn(0,N-1){
        for (int i=0; i<degrees[$]; i++) {
            int v = edges[vertices[$]+i][1];
            if (T[vertices[$]+i]==0 && low[$] > preorder[v]) {//for non-tree edges only
                low[$] = preorder[v];
            }
            if (T[vertices[$]+i]==0 && high[$] < preorder[v]) {//for non-tree edges only
            high[$] = preorder[v];
            }
        }
    }
     

#ifdef PRINT
    printf("\n\n\n");
    printf("low/high numbering after non-tree edges : \n");
    for (i=0; i<N; i++) {
        printf("i:\t%d \t| low:\t%d \t| high:\t%d\n",i,low[i],high[i]);
    }
#endif
    
    //Sort the aux_high and aux_low based on preorder numbers which are already sorted
    
    
    spawn(0,N-1){
        aux_low[preorder[$]-1] = low[$];
        aux_high[preorder[$]-1] = high[$];
    }
    
    
    preprocess_range_min_int(aux_low,N);
    preprocess_range_max_int(aux_high,N);
    
    
    spawn(0,N-1){
        int begin = preorder[$] - 1;
        int end = (preorder[$] + size_subtree[$] - 1) - 1;
        int sub_non_tree_min = query_range_min_int(aux_low,N,begin,end);
        int sub_non_tree_max = query_range_max_int(aux_high,N,begin,end);
        low[$] = sub_non_tree_min;
        high[$] = sub_non_tree_max;
    }
    
    
    

    
#ifdef PRINT
    printf("\n\n\n");
    printf("low/high final : \n");
    for (i=0; i<N; i++) {
        printf("i:\t%d \t| low:\t%d \t| high:\t%d\n",i,low[i],high[i]);
    }
#endif
    
    //5. Construct the aux graph
    psb0 = 0;
    int mapping_num[M];
    int g_prime_vert[M];
    int g_prime_edges[2*M][2];
    int g_prime_degrees[M];
    int g_prime_size;

    spawn(0,new_M-1){
        int local_val = 1;
        if( dir[$] == 0/*( new_edges[$][0] == parent[new_edges[$][1]] )*/ ){//edges in the tree get new numbers
            ps(local_val,psb0);
            mapping_num[$] = local_val;
            //printf("$:%d,  local_val:%d\n",$,local_val);
        }
    }

#ifdef PRINT
    printf("\n\n\n");
    printf("mapping num\n");
    for (int i=0; i<M-1; i++) {
        printf("i:%d\tnum:%d\n",i,mapping_num[i]);
    }
#endif

    psb0 = 0;
    int vertex_idx[M];
    spawn(0,M-1){
        int local_val = 1;
        int u = edges[$][0];
        int v = edges[$][1];
        if(T[$] == 0){//edges not in the tree get new numbers edge u-v
            if(u<v){//added by me!!
                if( ! (preorder[u] < preorder[v] && preorder[v] < preorder[u]+size_subtree[u]) ){
                    if( ! (preorder[v] < preorder[u] && preorder[u] < preorder[v]+size_subtree[v])){
                        //add p[v] and ....
                        ps(local_val,psb0);
                        g_prime_edges[local_val][0] = mapping_num[ edges_idx[u] ];
                        g_prime_edges[local_val][1] = mapping_num[ edges_idx[v] ];
                        //printf("first if %d - %d | u:%d\tv:%d | num_u:%d\tnum_v:%d\n", u,v,edges_idx[u],edges_idx[v],mapping_num[edges_idx[u]],mapping_num[edges_idx[v]]);
                    }
                }
            }
        }
        else if(T[$] == 1 && dir[T_comp[$]-1] == 0/*parent[v] == u*/){//parent[v] == u is for making sure we are looking at u -> v
            if(low[v] < preorder[u] || high[v] >= preorder[u] + size_subtree[u]){
                ps(local_val,psb0);
                g_prime_edges[local_val][0] = mapping_num[ edges_idx[u] ];
                g_prime_edges[local_val][1] = mapping_num[ edges_idx[v] ]; //true
                //printf("second if %d - %d | u:%d\tv:%d | num_u:%d\tnum_v:%d\n", u,v,edges_idx[u],edges_idx[v],mapping_num[edges_idx[u]],mapping_num[edges_idx[v]]);
            }
            
        }
    }
    g_prime_size = psb0;

    
#ifdef PRINT
    printf("\n\n\n");
    printf("g_prime_size : %d\n",g_prime_size);
    printf("\n\n\n");
    printf("new_M: %d\n",new_M);
    printf("\n\n\n");
    printf("G_prime graph : \n");
    for (i=0; i<g_prime_size; i++) {
        printf("i:\t%d\t %d - %d\n",i,g_prime_edges[i][0],g_prime_edges[i][1]);
    }
#endif
    
    
    //6. Compute the connected components of G_prime
    int g_prime_D[M];//probably we can use D
    connectivity(g_prime_edges,new_M/2,g_prime_size,g_prime_D);
    
#ifdef PRINT
    printf("\n\n\n");
    printf("g_prime_D : \n");
    for (i=0; i<new_M/2; i++) {
        //printf("i:\t %d\t(%d-%d)\tD:\t %d \n",i,g_prime_edges[i][0],g_prime_edges[i][1],g_prime_D[i]);
    }
#endif
    
    //7. For each non-tree edge, identify the biconnected component
    spawn(0,M-1){
        int val;
        if(T[$]==1 && dir[T_comp[$]-1] == 0){ //tree edges
            val = g_prime_D[mapping_num[T_comp[$]-1]];
            bcc[$] = val;
            bcc[antiparallel[$]] = val;
            //printf("$: %d, anit_$ : %d\n", $,antiparallel[$]);
        }
        else if(T[$]==0){
            int node = edges[$][0];
            int pre_u = preorder[edges[$][0]];
            int pre_v = preorder[edges[$][1]];
            int max_pre = pre_u;
            if (pre_v>max_pre) {node = edges[$][1];}
            bcc[$] = g_prime_D[mapping_num[edges_idx[node]]]; //edge_idx will look at the parent edge, then it will get converted, then the connected component
        }
    }
    
#ifdef PRINT_FINAL
    printf("\n\n\n");
    printf("bcc : \n");
    for (i=0; i<M; i++) {
        printf("i:\t %d\tbcc:\t %d\n",i,bcc[i]);
    }
#endif
    
	return 0;
}
